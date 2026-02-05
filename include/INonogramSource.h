// INonogramSource.h
#pragma once
#include "Nonogram.h"
#include <string>

class INonogramSource {
  public:
    virtual ~INonogramSource() = default;

    // Fills out_puzzle with row/col clues
    // calls out_puzzle.resize_from_clues() once clues are set
    // Returns false on failure
    virtual bool read(Nonogram &out_puzzle, std::string &out_error) = 0;
};