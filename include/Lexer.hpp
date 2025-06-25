#ifndef LEXER_HPP
#define LEXER_HPP

#include "Music.hpp"
#include <fstream>
#include <queue>
#include <variant>

namespace Lexer {
using Token = std::variant<Music::Note, Music::Rest>;
using Tokens = std::queue<Token>;

Tokens analyze(std::ifstream& file);
Token poll_token(Tokens& tokens);
}; // namespace Lexer

#endif
