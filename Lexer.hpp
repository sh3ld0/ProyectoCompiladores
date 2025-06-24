#ifndef LEXER_HPP
#define LEXER_HPP

#include "Token.hpp"
#include <fstream>
#include <regex>
#include <string>
#include <vector>

const static std::regex note_regex(R"(([A-Ga-g](#)?)(\d+)-(\d+/\d+))");
const static std::regex rest_regex(R"(M-(\d+/\d+))");
const static std::regex loop_regex(R"(loop)");
const static std::regex number_regex(R"(\d+)");
const static std::regex lbrace_regex(R"(\{)");
const static std::regex rbrace_regex(R"(\})");

static void match_to_token(const std::string& word, std::vector<Token>& tokens,
                           int line_number) {
  std::smatch match;

  if (std::regex_match(word, match, note_regex)) {
    tokens.push_back({TokenType::NOTE_NAME, match[1], line_number});
    tokens.push_back({TokenType::OCTAVE, match[3], line_number});
    tokens.push_back({TokenType::DASH, "-", line_number});
    tokens.push_back({TokenType::FRACTION, match[4], line_number});
    return;
  }

  if (std::regex_match(word, match, rest_regex)) {
    tokens.push_back({TokenType::REST, "rest", line_number});
    tokens.push_back({TokenType::DASH, "-", line_number});
    tokens.push_back({TokenType::FRACTION, match[1], line_number});
    return;
  }

  if (std::regex_match(word, match, loop_regex)) {
    tokens.push_back({TokenType::LOOP, word, line_number});
    return;
  }

  if (std::regex_match(word, match, number_regex)) {
    tokens.push_back({TokenType::NUMBER, word, line_number});
    return;
  }

  if (std::regex_match(word, match, lbrace_regex)) {
    tokens.push_back({TokenType::LBRACE, "{", line_number});
    return;
  }

  if (std::regex_match(word, match, rbrace_regex)) {
    tokens.push_back({TokenType::RBRACE, "}", line_number});
    return;
  }

  tokens.push_back({TokenType::INVALID, word, line_number});
}

inline std::vector<Token> tokenize(std::ifstream& file) {
  std::vector<Token> tokens;

  int line_number = 1;

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string word;
    while (iss >> word) {}
    line_number++;
  }

  tokens.push_back({TokenType::END_OF_FILE, "", line_number});
  return tokens;
}

#endif
