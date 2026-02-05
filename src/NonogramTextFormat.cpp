#include "../include/NonogramTextFormat.h"
#include "../include/core/enums.h"

#include "../include/Nonogram.h"
#include "../include/core/TextFormatMessage.h"
#include "../include/core/TextFormatParseUtil.h"

#include <algorithm>
#include <cctype>
#include <istream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace nonogram::io::textformat::msg;
using namespace nonogram::io::textformat;

namespace {
bool consume_clue_token(std::string &out_error, std::string_view token,
                        std::vector<int> &out_clues, size_t line_index,
                        std::string_view line_text) {
    int value = 0;
    TokenParseErr err{};
    const auto kind = parse_positive_int_token(token, value, err);

    if (kind == TokenParseKind::Error)
        return fail(out_error, with_line(token_error(err, token), line_index, line_text));
    if (kind == TokenParseKind::Value)
        out_clues.push_back(value);

    return true;
}

// I could probably extract the common stuff from both of these under me. maybe use a lambda again. I cba, I want a proper solver
bool consume_column_clue_line(std::string &out_error, const std::vector<std::string> &right_tokens,
                              std::vector<std::vector<int>> &col_clues, std::size_t line_i, std::string_view line_text) {
    if (right_tokens.size() != col_clues.size())
        return fail(out_error, with_line(wrong_column_token_count(
                                             right_tokens.size(), col_clues.size()),
                                         line_i, line_text));

    for (size_t col = 0; col < col_clues.size(); col++)
        if (!consume_clue_token(out_error, right_tokens[col], col_clues[col], line_i, line_text))
            return false;
    return true;
}

bool consume_row_clue_line(std::string &out_error, const std::vector<std::string> &tokens,
                           std::vector<int> &out_clues, size_t line_index, std::string_view line_text) {
    for (const auto &tok : tokens)
        if (!consume_clue_token(out_error, tok, out_clues, line_index, line_text))
            return false;
    return true;
}

bool split_pipe_and_tokenise(std::string &out_error,
                             const std::vector<std::string> &lines,
                             std::size_t line_index,
                             std::vector<std::string> &out_left_tokens,
                             std::vector<std::string> &out_right_tokens,
                             bool is_column_section) {
    std::string left, right;
    if (!split_pipe(lines[line_index], left, right)) {
        return fail(out_error, with_line(
                                   is_column_section ? missing_pipe_column()
                                                     : missing_pipe_row(),
                                   line_index, lines[line_index]));
    }

    out_left_tokens = split_ws(left);
    out_right_tokens = split_ws(right);
    return true;
}

template <class Fn>
bool for_each_pipe_line(std::string &out_error, const std::vector<std::string> &lines,
                        size_t begin, size_t end, bool is_column_section, Fn &&handle_tokens) {
    for (size_t i = begin; i < end; i++) {
        std::vector<std::string> left_tokens, right_tokens;

        if (!split_pipe_and_tokenise(out_error, lines, i, left_tokens, right_tokens, is_column_section))
            return false;
        if (!handle_tokens(i, left_tokens, right_tokens))
            return false;
    }
    return true;
}

} // namespace

namespace TextFormat {

// I wanted to shorten this method I just cba
bool read_text_format(std::istream &in, Nonogram &out_puzzle, std::string &out_error) {
    out_error.clear();
    out_puzzle = Nonogram{};

    int row_size = 0, col_size = 0;
    if (!(in >> row_size >> col_size))
        return fail(out_error, expected_rows_cols());
    if (row_size <= 0 || col_size <= 0)
        return fail(out_error, non_positive_dimensions());

    const size_t rows = static_cast<size_t>(row_size);
    const size_t cols = static_cast<size_t>(col_size);

    // consume remainder of first line
    std::string line;
    std::getline(in, line);

    // read remaining non-empty trimmed lines
    std::vector<std::string> lines;
    while (std::getline(in, line)) {
        line = ::trim(line);
        if (!line.empty())
            lines.push_back(line);
    }

    if (lines.empty())
        return fail(out_error, no_puzzle_data());

    // find separator line
    std::optional<size_t> sep;
    for (size_t i = 0; i < lines.size(); ++i)
        if (is_separator_line(lines[i])) {
            sep = i;
            break;
        }

    if (!sep) return fail(out_error, missing_separator());

    // column clues: above separator, right side has exactly `cols` tokens
    std::vector<std::vector<int>> col_clues(cols);

    if (!for_each_pipe_line(out_error, lines, 0, *sep, true,
                            [&](size_t i, const std::vector<std::string> &, const std::vector<std::string> &right) {
                                return consume_column_clue_line(out_error, right, col_clues, i, lines[i]);
                            }))
        return false;

    // row clues: below separator take exactly 'rows' lines with '|'
    std::vector<std::vector<int>> row_clues;
    row_clues.reserve(rows);

    const size_t row_begin = *sep + 1;
    const size_t row_end = std::min(lines.size(), row_begin + rows);

    if (!for_each_pipe_line(out_error, lines, row_begin, row_end, false,
                            [&](size_t i, const std::vector<std::string> &left, const std::vector<std::string> &) {
                                std::vector<int> row;
                                if (!consume_row_clue_line(out_error, left, row, i, lines[i]))
                                    return false;
                                row_clues.push_back(std::move(row));
                                return true;
                            }))
        return false;

    if (row_clues.size() != rows)
        return fail(out_error, wrong_row_count(row_clues.size(), rows));

    out_puzzle.row_clues = std::move(row_clues);
    out_puzzle.col_clues = std::move(col_clues);
    out_puzzle.resize_from_clues();
    return true;
}

} // namespace TextFormat
