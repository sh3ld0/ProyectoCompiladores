#include "Parser.hpp"
#include "Lexer.hpp"
#include <memory>
#include <print>

namespace {
struct NoteNode : Parser::Node {
  Music::Note note;
  NoteNode(Music::Note _note, Parser::Ast&& _next) :
      note{_note},
      Node{std::move(_next)} {}
  virtual ~NoteNode() override = default;
  virtual void evaluate(Music::Midi& context) const override {
    context.addNote(note);
    if (next)
      next->evaluate(context);
  }
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const override {
    std::print(os, "{}{}\n", indent, note);
    if (next)
      next->print(os, indent + "  ");
    else
      os << indent << "End Bar\n";
  }
};

struct RestNode : Parser::Node {
  Music::Rest rest;
  RestNode(Music::Rest _rest, Parser::Ast&& _next) :
      rest{_rest},
      Node{std::move(_next)} {}
  virtual ~RestNode() override = default;
  virtual void evaluate(Music::Midi& context) const override {
    context.addRest(rest);
    if (next)
      next->evaluate(context);
  }
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const override {
    std::print(os, "{}{}\n", indent, rest);
    if (next)
      next->print(os, indent + "  ");
    else
      os << indent << "End Bar\n";
  }
};

struct SignatureNode : Parser::Node {
  Music::Length length;

  SignatureNode(Music::Length _length, Parser::Ast&& _next) :
      length(_length),
      Node(std::move(_next)) {}
  virtual ~SignatureNode() override = default;
  virtual void evaluate(Music::Midi& context) const override {
    if (next)
      next->evaluate(context);
  }
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const override {
    std::print(os, "{}Time signature {}\n", indent, length);
    if (next)
      next->print(os, indent + "  ");
    else
      std::print(os, "{}End score\n", indent);
  }
};

struct BarNode : Parser::Node {
  Parser::Ast bar;

  BarNode(Parser::Ast&& _bar, Parser::Ast&& _next) :
      bar{std::move(_bar)},
      Node{std::move(_next)} {}
  virtual void evaluate(Music::Midi& context) const override {
    if (bar)
      bar->evaluate(context);

    if (next)
      next->evaluate(context);
  }
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const override {
    std::print(os, "{}Bar\n", indent);
    if (bar)
      bar->print(os, indent + "  ");

    if (next)
      next->print(os, indent + "  ");
    else
      std::print(os, "{}End score\n", indent);
  }
};

Parser::Ast build_bar(Lexer::Tokens& tokens) {
  if (tokens.empty())
    return nullptr;

  return std::visit(
      [&tokens]<class T>(const T& token) -> Parser::Ast {
        if constexpr (std::is_same_v<T, Music::Note>) {
          tokens.pop();
          return std::make_unique<NoteNode>(token, build_bar(tokens));
        } else if constexpr (std::is_same_v<T, Music::Rest>) {
          tokens.pop();
          return std::make_unique<RestNode>(token, build_bar(tokens));
        } else
          return nullptr;
      },
      tokens.front());
}
}; // namespace

Parser::Ast Parser::analyze(Lexer::Tokens& tokens) {
  if (tokens.empty())
    return nullptr;

  auto token = Lexer::poll_token(tokens);
  return std::visit(
      [&tokens]<class T>(const T& token) -> Parser::Ast {
        if constexpr (std::is_same_v<T, Music::Note> ||
                      std::is_same_v<T, Music::Rest>)
          throw std::runtime_error("Free note not allowed");
        else if constexpr (std::is_same_v<T, Music::Signature>)
          return std::make_unique<SignatureNode>(token.length, analyze(tokens));
        else if constexpr (std::is_same_v<T, Music::Bar>) {
          auto bar = build_bar(tokens);
          return std::make_unique<BarNode>(std::move(bar), analyze(tokens));
        }
      },
      token);
}
