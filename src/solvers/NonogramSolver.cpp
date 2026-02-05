
#include "../../include/solvers/NonogramSolver.h"

NonogramSolver::NonogramSolver() = default;

bool NonogramSolver::solve(Nonogram &puzzle, std::string &out_error) {
    out_error.clear();

    bool is_solved;

    {
        TrivialConstraintsSolver strategy;
        strategy.solve(puzzle, out_error);
    }

    {
        DPSolver strategy;
        is_solved = strategy.solve(puzzle, out_error);
    }

    return is_solved;
}
