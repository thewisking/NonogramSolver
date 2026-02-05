#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../include/Nonogram.h"
#include "../include/NonogramPrinter.h"
#include "../include/NonogramSource.h"
#include "../include/solvers/DPSolver.h"
#include "../include/solvers/NonogramSolver.h"
#include "../include/solvers/TrivialConstraintsSolver.h"

// #include "src/Nonogram.cpp"
// #include "src/NonogramPrinter.cpp"
// #include "src/NonogramSource.cpp"
// #include "src/NonogramTextFormat.cpp"
// #include "src/core/TestFormatMessage.cpp"
// #include "src/core/TextFormatParseUtil.cpp"
// #include "src/solvers/DPSolver.cpp"
// #include "src/solvers/NonogramSolver.cpp"
// #include "src/solvers/TrivialConstraintsSolver.cpp"

int main() {
    std::vector<int> OH_YEAH_VECTOR;
    std::string puzzle_name = "0003.txt";

    std::ifstream in("../puzzleName.txt");
    if (in) {
        std::string line;
        if (std::getline(in, line) && !line.empty())
            puzzle_name = line;
    }

    const std::string puzzle_directory = "puzzles/";

    Nonogram puzzle;
    std::string error_message;

    PrintStyle style;
    style.filled = "+";
    style.cellSpace = " ";
    style.empty = ".";
    style.unknown = "?";

    NonogramPrinter printer(style);

    NonogramSource source(puzzle_directory + puzzle_name);
    if (!source.read(puzzle, error_message)) {
        std::cerr << "READ FAILED:\n"
                  << error_message << "\n";
        return 1;
    }

    std::string error;
    std::cout << "Read OK: " << puzzle_name << "\n";

    NonogramSolver strategy;

    const auto t0 = std::chrono::steady_clock::now();
    bool solved = strategy.solve(puzzle, error);
    const auto t1 = std::chrono::steady_clock::now();

    const auto elapsed_us =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Solve time: " << (elapsed_us / 1000.0) << " ms" << " (" << (elapsed_us / 1000000.0) << " s)\n";

    if (!error_message.empty())
        std::cerr << "SOLVE ERROR:\n"
                  << error_message << "\n";

    std::cout << (solved ? "" : "not ") << "solved" << "\n";
    printer.print(puzzle);

    int pause = 0;
    std::cin >> pause;
}
