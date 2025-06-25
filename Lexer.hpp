#ifndef LEXER_HPP
#define LEXER_HPP

#include "Music.hpp"
#include <fstream>
#include <queue>
#include <regex>
#include <variant>

namespace Lexer {
using Token = std::variant<Music::Note, Music::Rest>;

namespace {
const std::map<std::string, Music::Tone> tone_from_string = {
    {"C", Music::Tone::C},   {"C#", Music::Tone::CS}, {"D", Music::Tone::D},
    {"D#", Music::Tone::DS}, {"E", Music::Tone::E},   {"F", Music::Tone::F},
    {"F#", Music::Tone::FS}, {"G", Music::Tone::G},   {"G#", Music::Tone::GS},
    {"A", Music::Tone::A},   {"A#", Music::Tone::AS}, {"B", Music::Tone::B}};

static Music::Length length_from_string(std::string&& str) {
  Music::Length length;
  std::istringstream iss(std::move(str));
  iss >> length.num;

  if (iss.peek() != '/') {
    length.dem = 1;
  } else {
    iss.ignore(1);
    iss >> length.dem;
  }
  return length;
}

const static std::regex note_regex{R"(([A-Ga-g](#)?)(\d+)-(\d+(/\d+)?))"};
const static std::regex rest_regex{R"(M-(\d+(/\d+)))"};

Token match(const std::string& word) {
  std::smatch match;
  if (std::regex_match(word, match, note_regex))
    return Music::Note{tone_from_string.at(match[1]), std::stoi(match[3]),
                       length_from_string(std::move(match[4]))};

  if (std::regex_match(word, match, rest_regex))
    return Music::Rest{length_from_string(match[1])};

  throw std::invalid_argument("Token no reconocido " + word);
}
} // namespace

using Tokens = std::queue<Token>;

inline Tokens analyze(std::ifstream& file) {
  int line_number;
  Tokens tokens;

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(std::move(line));
    std::string word;
    while (iss >> word)
      tokens.emplace(match(word));
    line_number++;
  }

  return tokens;
}

inline Token poll_token(Tokens& tokens) {
  if (tokens.empty())
    throw std::out_of_range("expected another token");

  Token token = tokens.front();
  tokens.pop();
  return token;
}
}; // namespace Lexer

#endif
