#pragma once

#include "../Nonogram.h"
#include "ISolverStrategy.h"
#include <string>

class DPSolver : ISolverStrategy {
  private:
  public:
    bool solve(Nonogram &puzzle, std::string &error);
};