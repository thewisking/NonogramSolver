#pragma once

#include "Nonogram.h"
#include "core/enums.h"

#include <iosfwd>
#include <string>

namespace TextFormat {
bool read_text_format(std::istream &in, Nonogram &out_puzzle, std::string &out_error);
}
