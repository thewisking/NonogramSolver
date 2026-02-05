#include "../include/NonogramSource.h"
#include "../include/NonogramTextFormat.h"

#include <fstream>
#include <utility>

NonogramSource::NonogramSource(std::string path)
    : path_(std::move(path)) {}

bool NonogramSource::read(Nonogram &out_puzzle, std::string &out_error) const {
    out_error.clear();
    out_puzzle = Nonogram{};

    std::ifstream in(path_);
    if (!in) {
        out_error = "Failed to open file: " + path_;
        return false;
    }

    return TextFormat::read_text_format(in, out_puzzle, out_error);
}
