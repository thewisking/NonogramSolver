// Nonogram.cpp
#include "../include/Nonogram.h"

// nonogram size is row_clues.size() by col_clues.size(), use a function so we return the correct value every time.
size_t Nonogram::rows() const { return row_clues.size(); }
size_t Nonogram::cols() const { return col_clues.size(); }

// resize cells to a grid of Unknown as per the cell enum
void Nonogram::resize_from_clues() { cells.assign(rows(), std::vector<Cell>(cols(), Cell::Unknown)); }
