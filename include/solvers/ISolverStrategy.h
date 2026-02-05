#pragma once

#include "../Nonogram.h"
#include <string>

class ISolverStrategy {
  public:
    virtual ~ISolverStrategy() = default;

    virtual bool solve(Nonogram &puzzle, std::string &error) = 0;
};
