#include "../include/NonogramPrinter.h"

void NonogramPrinter::setStyle(PrintStyle style) {
    this->style = style;
}

NonogramPrinter::NonogramPrinter() : NonogramPrinter(PrintStyle{}) {}
NonogramPrinter::NonogramPrinter(PrintStyle style) : style(style) {}

void NonogramPrinter::printCell(Cell cell) {
    if (cell == Cell::Filled)
        std::cout << style.filled << style.cellSpace;
    else if (cell == Cell::Empty)
        std::cout << style.empty << style.cellSpace;
    else
        std::cout << style.unknown << style.cellSpace;
}

void NonogramPrinter::printRow(std::vector<Cell> &cells) {
    for (auto &cell : cells)
        printCell(cell);
    std::cout << style.rowSpace;
}

void NonogramPrinter::print(Nonogram &puzzle) {
    for (auto &row : puzzle.cells)
        printRow(row);
}
