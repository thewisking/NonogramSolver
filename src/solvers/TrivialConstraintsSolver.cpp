#include "../../include/solvers/TrivialConstraintsSolver.h"

namespace {
bool setCell(Cell &cell, Cell target, bool &madeProgress) {
    if ((target == Cell::Filled && cell == Cell::Empty) ||
        (target == Cell::Empty && cell == Cell::Filled))
        return false;

    if (cell != target) {
        cell = target;
        madeProgress = true;
    }
    return true;
}

template <typename CellAccessor>
LineResult solveExactFitLine(const std::vector<int> &clues, size_t line_length, bool &madeProgress, CellAccessor cellAt) {
    int sum = 0;
    for (int run_length : clues)
        sum += run_length;

    size_t minimum_length = sum + clues.size() - 1;
    if (minimum_length > line_length)
        return LineResult::Contradiction;
    if (minimum_length != line_length)
        return LineResult::NoChange;

    int cell_index = 0;

    for (size_t run_index = 0; run_index < clues.size(); run_index++) {
        int run_length = clues[run_index];

        for (int run_cell = 0; run_cell < run_length; run_cell++, cell_index++) {
            if (!setCell(cellAt(cell_index), Cell::Filled, madeProgress))
                return LineResult::Contradiction;
        }

        if (run_index + 1 < clues.size()) {
            if (!setCell(cellAt(cell_index), Cell::Empty, madeProgress))
                return LineResult::Contradiction;

            cell_index++;
        }
    }

    return madeProgress ? LineResult::Changed : LineResult::NoChange;
}

template <typename CellAccessor>
LineResult forceWholeLine(size_t line_length, Cell required, bool &madeProgress, CellAccessor cellAt) {
    for (size_t cell_index = 0; cell_index < line_length; cell_index++) {
        if (!setCell(cellAt(cell_index), required, madeProgress))
            return LineResult::Contradiction;
    }

    return madeProgress ? LineResult::Changed : LineResult::NoChange;
}

template <typename CellAccessor>
LineResult startLine(const std::vector<int> &clues, size_t line_length, bool &madeProgress, CellAccessor cellAt) {
    if (clues.empty())
        return forceWholeLine(line_length, Cell::Empty, madeProgress, cellAt);

    return solveExactFitLine(clues, line_length, madeProgress, cellAt);
}

LineResult applyLine(Nonogram &puzzle, int index, bool isRow) {
    bool madeProgress = false;
    if (isRow) {
        auto cellAt = [&puzzle, index](size_t cell_index) -> Cell & { return puzzle.cells[index][cell_index]; };
        return startLine(puzzle.row_clues.at(index), puzzle.cells[index].size(), madeProgress, cellAt);
    }
    auto cellAt = [&puzzle, index](size_t row_index) -> Cell & { return puzzle.cells[row_index][index]; };
    return startLine(puzzle.col_clues.at(index), puzzle.rows(), madeProgress, cellAt);
}

LineResult applyAllLines(Nonogram &puzzle, int count, bool isRow) {
    LineResult result = LineResult::NoChange;
    for (int i = 0; i < count; i++) {
        LineResult temp = applyLine(puzzle, i, isRow);
        if (temp == LineResult::Contradiction)
            return LineResult::Contradiction;
        if (temp == LineResult::Changed)
            result = LineResult::Changed;
    }

    return result;
}

LineResult applyPass(Nonogram &puzzle) {
    LineResult rows = applyAllLines(puzzle, puzzle.rows(), true);
    LineResult cols = applyAllLines(puzzle, puzzle.cols(), false);
    if (rows == LineResult::Contradiction || cols == LineResult::Contradiction)
        return LineResult::Contradiction;

    return (rows == LineResult::Changed || cols == LineResult::Changed) ? LineResult::Changed : LineResult::NoChange;
}
} // namespace

bool TrivialConstraintsSolver::solve(Nonogram &puzzle, std::string &error) {
    error.clear();

    const auto res = applyPass(puzzle);
    if (res == LineResult::Contradiction) {
        error = "TrivialConstraintsSolver: contradiction detected";
        return false;
    }
    return res == LineResult::Changed;
}



