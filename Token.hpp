#pragma once
#include <string>

enum class TokenType {
    NOTE_NAME,
    OCTAVE,
    DASH,
    FRACTION,
    REST,
    LOOP,
    NUMBER,
    LBRACE,
    RBRACE,
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};
