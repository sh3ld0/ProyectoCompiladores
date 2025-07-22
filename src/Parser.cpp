#include "Parser.hpp"
#include "Lexer.hpp"
#include <print>

namespace {
Parser::Ast build_bar(Lexer::Tokens& tokens) {
  if (tokens.empty())
    return nullptr;

  return std::visit(
      [&tokens]<class T>(const T& token) -> Parser::Ast {
        if constexpr (std::is_same_v<T, Music::Note>) {
          tokens.pop();
          return std::make_unique<Parser::NoteNode>(token, build_bar(tokens));
        } else if constexpr (std::is_same_v<T, Music::Rest>) {
          tokens.pop();
          return std::make_unique<Parser::RestNode>(token, build_bar(tokens));
        } else
          return nullptr;
      },
      tokens.front());
}
}; // namespace

namespace Parser {
NoteNode::NoteNode(Music::Note _note, Ast&& _next) :
    note{_note},
    Node{std::move(_next)} {}

void NoteNode::evaluate(Music::Midi& context) const {
  context.addNote(note);
  if (next)
    next->evaluate(context);
}

void NoteNode::print(std::ostream& os, const std::string& indent) const {
  std::print(os, "{}{}\n", indent, note);
  if (next)
    next->print(os, indent + "  ");
  else
    std::print("{}End Bar\n", indent);
}

RestNode::RestNode(Music::Rest _rest, Ast&& _next) :
    rest{_rest},
    Node{std::move(_next)} {}

void RestNode::evaluate(Music::Midi& context) const {
  context.addRest(rest);
  if (next)
    next->evaluate(context);
}

void RestNode::print(std::ostream& os, const std::string& indent) const {
  std::print(os, "{}{}\n", indent, rest);
  if (next)
    next->print(os, indent + "  ");
  else
    std::print("{}End Bar\n", indent);
}

SignatureNode::SignatureNode(Music::Length _length, Ast&& _next) :
    length(_length),
    Node(std::move(_next)) {}

void SignatureNode::evaluate(Music::Midi& context) const {
  if (next)
    next->evaluate(context);
  else
    context.write();
}

void SignatureNode::print(std::ostream& os, const std::string& indent) const {
  std::print(os, "{}Time signature {}\n", indent, length);
  if (next)
    next->print(os, indent + "  ");
  else
    std::print(os, "{}End score\n", indent);
}

BarNode::BarNode(Parser::Ast&& _bar, Parser::Ast&& _next) :
    bar{std::move(_bar)},
    Node{std::move(_next)} {}

void BarNode::evaluate(Music::Midi& context) const {
  if (bar)
    bar->evaluate(context);

  if (next)
    next->evaluate(context);
  else
    context.write();
}

void BarNode::print(std::ostream& os, const std::string& indent = "") const {
  std::print(os, "{}Bar\n", indent);
  if (bar)
    bar->print(os, indent + "  ");

  if (next)
    next->print(os, indent + "  ");
  else
    std::print(os, "{}End score\n", indent);
}

Ast analyze(Lexer::Tokens& tokens) {
  if (tokens.empty())
    return nullptr;

  auto token = Lexer::poll_token(tokens);
  return std::visit(
      [&tokens]<class T>(const T& token) -> Ast {
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
} // namespace Parser
