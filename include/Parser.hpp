#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"
#include <memory>

namespace Parser {
struct Node {
  std::unique_ptr<Node> next;
  Node(std::unique_ptr<Node>&& _next) : next(std::move(_next)) {}
  virtual ~Node() = default;
  virtual void evaluate(Music::Midi&) const = 0;
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const = 0;
};

using Ast = std::unique_ptr<Node>;
Ast analyze(Lexer::Tokens& tokens);
}; // namespace Parser

#endif
