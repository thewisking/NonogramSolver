#pragma once

#include "enums.h"
#include <string>
#include <vector>

namespace nonogram::io::textformat {

std::string trim(const std::string &s);
std::vector<std::string> split_ws(const std::string &s);
bool fail(std::string &out_error, std::string msg);
bool split_pipe(const std::string &line, std::string &left, std::string &right);
bool is_separator_line(const std::string &line);
TokenParseKind parse_positive_int_token(const std::string_view token, int &out_value, TokenParseErr &out_error);

// One of my extentions automatically puts the next comment. not sure why
} // namespace nonogram::io::textformat
