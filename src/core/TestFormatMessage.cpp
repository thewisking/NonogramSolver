#include "../../include/core/TextFormatMessage.h"

#include <string>

namespace nonogram::io::textformat::msg {

// Dimension/header issues
std::string expected_rows_cols() {
    return "Expected first line: rows cols (e.g. \"5 5\").";
}
std::string non_positive_dimensions() {
    return "Expected positive non-zero values for rows and cols.";
}
std::string no_puzzle_data() {
    return "No puzzle data found after rows cols.";
}
std::string missing_separator() {
    return "Missing separator line (e.g. \"------+-----\").";
}

// Structural line issues
std::string missing_pipe_column() {
    return "Column-clue line missing \"|\".";
}
std::string missing_pipe_row() {
    return "Row-clue line missing \"|\".";
}
std::string wrong_column_token_count(std::size_t got, std::size_t expected) {
    return "Column-clue line has " + std::to_string(got) +
           " tokens on the right, expected " + std::to_string(expected) + ".";
}
std::string wrong_row_count(std::size_t got, std::size_t expected) {
    return "Expected " + std::to_string(expected) +
           " row lines after separator, got " + std::to_string(got) + ".";
}

// Token issues
std::string empty_token() {
    return "Empty token where clue expected.";
}
std::string invalid_integer_token(std::string_view token) {
    return "Invalid integer token: '" + std::string(token) + "'";
}
std::string non_positive_clue(std::string_view token) {
    return "Non-positive clue: '" + std::string(token) + "'";
}
std::string with_line(std::string_view msg, std::size_t i, std::string_view line) {
    return std::string(msg) + "\nLine " + std::to_string(i + 1) + ":\n\"" + std::string(line) + "\"";
}
std::string token_error(TokenParseErr error, std::string_view token) {
    switch (error) {
    case TokenParseErr::EmptyToken:
        return empty_token();
    case TokenParseErr::InvalidInteger:
        return invalid_integer_token(token);
    case TokenParseErr::NonPositive:
        return non_positive_clue(token);
    case TokenParseErr::None:
    default:
        return "Unknown token parse error.";
    }
}

} // namespace nonogram::io::textformat::msg