#pragma once

#include "../../include/solvers/DPSolver.h"
#include "../../include/solvers/TrivialConstraintsSolver.h"
#include <memory>
#include <string>
#include <vector>

class Nonogram;
class ISolverStrategy;

class NonogramSolver {
  public:
    NonogramSolver();
    bool solve(Nonogram &puzzle, std::string &error);

  private:
    std::vector<std::unique_ptr<ISolverStrategy>> strategies_;
};
