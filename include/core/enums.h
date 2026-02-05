#pragma once

enum class LineResult { Contradiction,
                        NoChange,
                        Changed
};

enum class TokenParseErr {
    None,
    EmptyToken,
    InvalidInteger,
    NonPositive,
};

enum class TokenParseKind {
    Dot,
    Value,
    Error,
};