#pragma once

#include "Nonogram.h"
#include "core/PrintStyle.h"

#include <iostream>
#include <optional>
#include <string>

class NonogramPrinter {
  public:
    NonogramPrinter();
    NonogramPrinter(PrintStyle style);
    void setStyle(PrintStyle style);
    void print(Nonogram &puzzle);

  private:
    PrintStyle style;

    void printRow(std::vector<Cell> &cells);
    void printCell(Cell cell);
};