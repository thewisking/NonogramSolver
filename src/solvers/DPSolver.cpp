#include "../../include/solvers/DPSolver.h"

#include "../../include/Cell.h"

#include <algorithm>
#include <cstdint>
#include <deque>
#include <string>
#include <utility>
#include <vector>

namespace {

// Domain bitmask per cell
// bit0 (1) = Empty possible
// bit1 (2) = Filled possible
constexpr std::uint8_t D_EMPTY = 0x1;
constexpr std::uint8_t D_FILLED = 0x2;

inline std::uint8_t domain_from_cell(Cell c) {
    switch (c) {
    case Cell::Empty:
        return D_EMPTY;
    case Cell::Filled:
        return D_FILLED;
    case Cell::Unknown:
    default:
        return D_EMPTY | D_FILLED;
    }
}

inline Cell cell_from_domain(std::uint8_t d) {
    if (d == D_EMPTY)
        return Cell::Empty;
    if (d == D_FILLED)
        return Cell::Filled;
    return Cell::Unknown;
}

inline int popcount2(std::uint8_t d) {
    // only need for {0..3}
    return (d & 1 ? 1 : 0) + (d & 2 ? 1 : 0);
}

// Line automaton state:
// j   = number of blocks already completed (0..M)
// rem = remaining filled cells to place in current block (0..N)
// sep = must place at least one empty before starting next block (only meaningful if rem==0 and j<M)
struct State {
    int j;
    int rem;
    int sep;
};

struct LineRef { bool is_row; int idx; };

struct DomainGrid {
    int R = 0;
    int C = 0;
    std::vector<std::uint8_t> d; // flattened R*C

    std::uint8_t &at(int r, int c) { return d[r * C + c]; }
    std::uint8_t at(int r, int c) const { return d[r * C + c]; }

    bool all_singleton() const {
        for (auto v : d) {
            if (v != D_EMPTY && v != D_FILLED)
                return false;
        }
        return true;
    }
};

// Per-line propagation using reachability through a small automaton.
// Returns false on contradiction (no valid completion), else true and fills out_new_domains.
bool propagate_line_domains(const std::vector<int> &clues,
                            const std::vector<std::uint8_t> &in_domains,
                            std::vector<std::uint8_t> &out_new_domains) {
    const int N = static_cast<int>(in_domains.size());
    const int M = static_cast<int>(  clues.size()   );

    if (N == 0) {
        out_new_domains.clear();
        return true;
    }

    // Precompute mapping from (j, rem, sep) -> id.
    // We keep the full rectangle for simplicity: id = ((j*(N+1) + rem)*2 + sep)
    const int REM_MAX       = N;
    const int STATE_COUNT   = (M + 1) * (REM_MAX + 1) * 2;

    auto id_of = [REM_MAX](int j, int rem, int sep) -> int {
        return ((j * (REM_MAX + 1) + rem) * 2 + sep);
    };

    auto unpack = [REM_MAX](int id) -> State {
        const int sep = id % 2;
        id /= 2;
        const int rem = id % (REM_MAX + 1);
        const int j = id / (REM_MAX + 1);
        return {j, rem, sep};
    };

    // Transitions for EMPTY and FILLED from any state id.
    std::vector<int> trans_empty(STATE_COUNT, {-1});
    std::vector<int> trans_filled(STATE_COUNT, {-1});

    for (int id = 0; id < STATE_COUNT; ++id) {
        const State s = unpack(id);
        const int j = s.j;
        const int rem = s.rem;
        const bool sep = (s.sep != 0);

        // EMPTY transition
        if (rem == 0) {
            // placing empty is always allowed when not in a block
            // if sep was required, this empty satisfies it.
            const int next_sep = 0;
            trans_empty[id] = {id_of(j, 0, next_sep)};
        } 

        // FILLED transition
        if (!sep) {
            if (rem > 0) {
                // continue current block
                const int rem2 = rem - 1;
                if (rem2 == 0) {
                    // block completes now
                    const int j2 = j + 1;
                    if (j2 <= M) {
                        const int sep2 = (j2 < M) ? 1 : 0;
                        trans_filled[id] = {id_of(j2, 0, sep2)};
                    }
                } else {
                    trans_filled[id] = {id_of(j, rem2, 0)};
                }
            } else {
                // rem == 0, can start a new block if any left
                if (j < M) {
                    const int len = clues[j];
                    if (len <= 0 || len > N) {
                        // invalid clue
                        return false;
                    }
                    if (len == 1) {
                        const int j2 = j + 1;
                        const int sep2 = (j2 < M) ? 1 : 0;
                        trans_filled[id] = {id_of(j2, 0, sep2)};
                    } else {
                        // we place first filled now; remaining is len-1
                        trans_filled[id] = {id_of(j, len - 1, 0)};
                    }
                }
            }
        }
    }

    auto is_accept = [M](const State &s) -> bool {
        return (s.j == M) && (s.rem == 0);
    };

    // Forward reachability: forward[i][state] = reachable after i cells
    std::vector<std::vector<std::uint8_t>> forward(N + 1, std::vector<std::uint8_t>(STATE_COUNT, 0));
    std::vector<std::vector<std::uint8_t>> backward(N + 1, std::vector<std::uint8_t>(STATE_COUNT, 0));

    const int start = id_of(0, 0, 0);
    forward[0][start] = 1;

    for (int i = 0; i < N; ++i) {
        const std::uint8_t dom = in_domains[i];
        for (int sid = 0; sid < STATE_COUNT; ++sid) {
            if (!forward[i][sid])
                continue;

            if (dom & D_EMPTY) {
                const int nid = trans_empty[sid];
                if (nid >= 0)
                    forward[i + 1][nid] = 1;
            }
            if (dom & D_FILLED) {
                const int nid = trans_filled[sid];
                if (nid >= 0)
                    forward[i + 1][nid] = 1;
            }
        }
    }

    // Backward reachability: backward[i][state] = can reach accept from here with remaining cells
    for (int sid = 0; sid < STATE_COUNT; ++sid) {
        const State s = unpack(sid);
        if (is_accept(s))
            backward[N][sid] = 1;
    }
    for (int i = N - 1; i >= 0; --i) {
        const std::uint8_t dom = in_domains[i];
        for (int sid = 0; sid < STATE_COUNT; ++sid) {
            bool ok = false;
            if (dom & D_EMPTY) {
                const int nid = trans_empty[sid];
                if (nid >= 0 && backward[i + 1][nid])
                    ok = true;
            }
            if (!ok && (dom & D_FILLED)) {
                const int nid = trans_filled[sid];
                if (nid >= 0 && backward[i + 1][nid])
                    ok = true;
            }
            backward[i][sid] = ok ? 1 : 0;
        }
    }

    // If no accepting path exists at all -> contradiction
    bool any_path = false;
    for (int sid = 0; sid < STATE_COUNT; ++sid) {
        if (forward[N][sid] && backward[N][sid]) {
            any_path = true;
            break;
        }
    }
    if (!any_path)
        return false;

    // Per-position possibility marking via prefix/suffix connectivity
    out_new_domains.assign(N, 0);
    for (int i = 0; i < N; ++i) {
        bool can_empty = false;
        bool can_filled = false;

        const std::uint8_t dom = in_domains[i];

        for (int sid = 0; sid < STATE_COUNT; ++sid) {
            if (!forward[i][sid])
                continue;

            if ((dom & D_EMPTY) && !can_empty) {
                const int nid = trans_empty[sid];
                if (nid >= 0 && backward[i + 1][nid])
                    can_empty = true;
            }
            if ((dom & D_FILLED) && !can_filled) {
                const int nid = trans_filled[sid];
                if (nid >= 0 && backward[i + 1][nid])
                    can_filled = true;
            }
            if (can_empty && can_filled)
                break;
        }

        std::uint8_t nd = 0;
        if (can_empty)
            nd |= D_EMPTY;
        if (can_filled)
            nd |= D_FILLED;
        out_new_domains[i] = nd;
        if (nd == 0)
            return false;
    }

    return true;
}


bool enforce_arc_consistency(const Nonogram &puz, DomainGrid &g) {
    std::deque<LineRef> q;
    std::vector<std::uint8_t> inQ_row(g.R, 0), inQ_col(g.C, 0);

    for (int r = 0; r < g.R; ++r) {
        q.push_back({true, r});
        inQ_row[r] = 1;
    }
    for (int c = 0; c < g.C; ++c) {
        q.push_back({false, c});
        inQ_col[c] = 1;
    }

    std::vector<std::uint8_t> line_domains;
    std::vector<std::uint8_t> new_line_domains;

    while (!q.empty()) {
        const auto cur = q.front();
        q.pop_front();
        if (cur.is_row)
            inQ_row[cur.idx] = 0;
        else
            inQ_col[cur.idx] = 0;

        if (cur.is_row) {
            const int r = cur.idx;
            line_domains.assign(g.C, 0);
            for (int c = 0; c < g.C; ++c)
                line_domains[c] = g.at(r, c);

            if (!propagate_line_domains(puz.row_clues[r], line_domains, new_line_domains))
                return false;

            for (int c = 0; c < g.C; ++c) {
                const std::uint8_t oldv = g.at(r, c);
                const std::uint8_t newv = new_line_domains[c];
                if (newv == 0)
                    return false;
                if (newv != oldv) {
                    g.at(r, c) = newv;
                    if (!inQ_col[c]) {
                        q.push_back({false, c});
                        inQ_col[c] = 1;
                    }
                }
            }
        } else {
            const int c = cur.idx;
            line_domains.assign(g.R, 0);
            for (int r = 0; r < g.R; ++r)
                line_domains[r] = g.at(r, c);

            if (!propagate_line_domains(puz.col_clues[c], line_domains, new_line_domains))
                return false;

            for (int r = 0; r < g.R; ++r) {
                const std::uint8_t oldv = g.at(r, c);
                const std::uint8_t newv = new_line_domains[r];
                if (newv == 0)
                    return false;
                if (newv != oldv) {
                    g.at(r, c) = newv;
                    if (!inQ_row[r]) {
                        q.push_back({true, r});
                        inQ_row[r] = 1;
                    }
                }
            }
        }
    }

    return true;
}

bool dfs_solve(const Nonogram &puz, DomainGrid &g) {
    if (!enforce_arc_consistency(puz, g))
        return false;
    if (g.all_singleton())
        return true;

    // Pick the most constrained unknown cell
    int best_r = -1, best_c = -1;
    int best_size = 3;

    for (int r = 0; r < g.R; ++r) {
        for (int c = 0; c < g.C; ++c) {
            const std::uint8_t v = g.at(r, c);
            const int pc = popcount2(v);
            if (pc == 0)
                return false;
            if (pc > 1 && pc < best_size) {
                best_size = pc;
                best_r = r;
                best_c = c;
                if (best_size == 2)
                    goto chosen;
            }
        }
    }

chosen:
    if (best_r < 0)
        return false;

    const std::uint8_t v = g.at(best_r, best_c);

    // Try Filled first (often helps), then Empty
    if (v & D_FILLED) {
        DomainGrid g2 = g;
        g2.at(best_r, best_c) = D_FILLED;
        if (dfs_solve(puz, g2)) {
            g = std::move(g2);
            return true;
        }
    }
    if (v & D_EMPTY) {
        DomainGrid g2 = g;
        g2.at(best_r, best_c) = D_EMPTY;
        if (dfs_solve(puz, g2)) {
            g = std::move(g2);
            return true;
        }
    }

    return false;
}

} // namespace

bool DPSolver::solve(Nonogram &puzzle, std::string &error) {
    error.clear();

    if (puzzle.rows() == 0 || puzzle.cols() == 0) {
        error = "DPSolver: puzzle has zero size";
        return false;
    }

    // If user forgot to resize cells, fix it.
    if (puzzle.cells.size() != puzzle.rows() ||
        (!puzzle.cells.empty() && puzzle.cells[0].size() != puzzle.cols())) {
        puzzle.resize_from_clues();
    }

    DomainGrid g;
    g.R = static_cast<int>(puzzle.rows());
    g.C = static_cast<int>(puzzle.cols());
    g.d.assign(g.R * g.C, D_EMPTY | D_FILLED);

    for (int r = 0; r < g.R; ++r) {
        for (int c = 0; c < g.C; ++c) {
            g.at(r, c) = domain_from_cell(puzzle.cells[r][c]);
        }
    }

    DomainGrid solved = g;
    if (!dfs_solve(puzzle, solved)) {
        error = "DPSolver: puzzle is unsatisfiable (no solution found)";
        return false;
    }

    // Write back solution
    for (int r = 0; r < g.R; ++r) {
        for (int c = 0; c < g.C; ++c) {
            puzzle.cells[r][c] = cell_from_domain(solved.at(r, c));
        }
    }

    return true;
}
