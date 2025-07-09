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

struct NoteNode final : Parser::Node {
  Music::Note note;
  NoteNode(Music::Note _note, Parser::Ast&& _next);
  virtual void evaluate(Music::Midi& context) const override;
  virtual void print(std::ostream& os,
                     const std::string& indent) const override;
};
struct RestNode final : Parser::Node {
  Music::Rest rest;
  RestNode(Music::Rest _rest, Parser::Ast&& _next);
  virtual void evaluate(Music::Midi& context) const override;
  virtual void print(std::ostream& os,
                     const std::string& indent) const override;
};
struct SignatureNode final : Parser::Node {
  Music::Length length;
  SignatureNode(Music::Length _length, Parser::Ast&& _next);
  virtual void evaluate(Music::Midi& context) const override;
  virtual void print(std::ostream& os,
                     const std::string& indent) const override;
};
struct BarNode final : Parser::Node {
  Parser::Ast bar;
  BarNode(Parser::Ast&& _bar, Parser::Ast&& _next);
  virtual void evaluate(Music::Midi& context) const override;
  virtual void print(std::ostream& os,
                     const std::string& indent) const override;
};

Ast analyze(Lexer::Tokens& tokens);
}; // namespace Parser

#endif
