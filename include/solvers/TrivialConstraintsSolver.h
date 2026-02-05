#pragma once

#include "../Nonogram.h"
#include "../core/enums.h"
#include "ISolverStrategy.h"
#include <string>

class TrivialConstraintsSolver : public ISolverStrategy {
  public:
    bool solve(Nonogram &puzzle, std::string &error) override;

  private:
};
