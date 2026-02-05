#include "../../include/core/TextFormatParseUtil.h"
#include "../../include/core/enums.h"

#include <cctype>
#include <sstream>

namespace nonogram::io::textformat {

std::string trim(const std::string &s) {
    size_t first = 0;
    while (first < s.size() && std::isspace(static_cast<unsigned char>(s[first])))
        ++first;

    size_t last = s.size();
    while (last > first && std::isspace(static_cast<unsigned char>(s[last - 1])))
        --last;

    return s.substr(first, last - first);
}

std::vector<std::string> split_ws(const std::string &s) {
    std::istringstream iss(s);
    std::vector<std::string> out;
    std::string tok;
    while (iss >> tok)
        out.push_back(tok);
    return out;
}

bool fail(std::string &out_error, std::string msg) {
    out_error = std::move(msg);
    return false;
}

bool split_pipe(const std::string &line, std::string &left, std::string &right) {
    const auto pos = line.find('|');
    if (pos == std::string::npos)
        return false;
    left = line.substr(0, pos);
    right = line.substr(pos + 1);
    return true;
}

bool is_separator_line(const std::string &line) {
    bool has_plus = false;
    bool has_dash = false;

    for (unsigned char ch : line) {
        if (ch == '+')
            has_plus = true;
        else if (ch == '-')
            has_dash = true;
        else if (std::isspace(ch))
            continue;
        else
            return false; // any other char => not a separator
    }
    return has_plus && has_dash;
}

// Returns true if token is a clue int, false if token is "."
// On invalid token, returns false and sets out_error (caller must treat as hard error)
TokenParseKind parse_positive_int_token(const std::string_view token,
                                        int &out_value, TokenParseErr &out_error) {
    if (token == ".")
        return TokenParseKind::Dot;

    if (token.empty()) {
        out_error = TokenParseErr::EmptyToken;
        return TokenParseKind::Error;
    }

    int value = 0;
    for (unsigned char ch : token) {
        if (!std::isdigit(ch)) {
            out_error = TokenParseErr::InvalidInteger;
            return TokenParseKind::Error;
        }
        value = value * 10 + (ch - '0');
    }

    if (value <= 0) {
        out_error = TokenParseErr::NonPositive;
        return TokenParseKind::Error;
    }

    out_value = value;
    return TokenParseKind::Value;
}

} // namespace nonogram::io::textformat
