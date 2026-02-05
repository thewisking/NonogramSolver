#pragma once

#include "enums.h"
#include <cstddef>
#include <string>
#include <string_view>

namespace nonogram::io::textformat::msg {

// Dimension/header issues
std::string expected_rows_cols();
std::string non_positive_dimensions();
std::string no_puzzle_data();
std::string missing_separator();

// Structural line issues
std::string missing_pipe_column();
std::string missing_pipe_row();
std::string wrong_column_token_count(std::size_t got, std::size_t expected);
std::string wrong_row_count(std::size_t got, std::size_t expected);

// Token issues
std::string empty_token();
std::string invalid_integer_token(std::string_view token);
std::string non_positive_clue(std::string_view token);
std::string with_line(std::string_view msg, std::size_t i, std::string_view line);
std::string token_error(TokenParseErr err, std::string_view token);

} // namespace nonogram::io::textformat::msg