// Nonogram.h
#pragma once
#include "Cell.h"
#include <cstdint>
#include <vector>

class Nonogram {
  public:
    std::vector<std::vector<int>> row_clues{};
    std::vector<std::vector<int>> col_clues{};

    std::vector<std::vector<Cell>> cells{};

    // nonogram size is row_clues.size() by col_clues.size(), use a function so we return the correct value every time.
    size_t rows() const;
    size_t cols() const;

    // resize cells to a grid of Unknown as per the cell enum
    void resize_from_clues();
};
