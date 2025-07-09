#ifndef CHECKER_HPP
#define CHECKER_HPP

#include "Music.hpp"
#include "Parser.hpp"

namespace Checker {
struct SemanticError : std::runtime_error {
  using std::runtime_error::runtime_error;
};
struct LengthError : SemanticError {
  LengthError(Music::Length bar, Music::Length measure) :
      SemanticError(std::format(
          "Bar length of {} when time signature expected {}", bar, measure)) {}
};

void analyze(const Parser::Ast&, Music::Length = {1, 1});
} // namespace Checker

#endif
