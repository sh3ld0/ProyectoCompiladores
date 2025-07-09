#include "Parser.hpp"
#include <memory>
#include <print>

namespace {
struct LengthNode : Parser::Node {
  Lexer::Token token;
  std::unique_ptr<LengthNode> next;

  LengthNode(const Lexer::Token& _token, std::unique_ptr<LengthNode>&& _next) :
      token{_token},
      next{std::move(_next)} {}
  virtual ~LengthNode() override = default;
  virtual void evaluate(Music::Midi& context) const override {
    std::visit(
        [&context]<class T>(const T& token) {
          if constexpr (std::is_same_v<T, Music::Note> ||
                        std::is_same_v<T, Music::Note>)
            context.addNote(token);
          else
            throw std::runtime_error("Non lengthed token in unexpected place");
        },
        token);
    if (next)
      next->evaluate(context);
  }
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const override {
    std::visit(
        [&]<class T>(const T& token) {
          if constexpr (std::is_same_v<T, Music::Note> ||
                        std::is_same_v<T, Music::Rest>)
            std::print(os, "{}{}\n", indent, token);
        },
        token);

    if (next)
      next->print(os, indent + "  ");
    else
      os << indent << "End Bar\n";
  }
};

struct SignatureNode : Parser::Node {
  Music::Length length;
  std::unique_ptr<Node> next;

  SignatureNode(Music::Length _length, Parser::Ast&& _next) :
      length(_length),
      next(std::move(_next)) {}
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
  std::unique_ptr<LengthNode> bar;
  std::unique_ptr<Node> next;

  BarNode(std::unique_ptr<LengthNode>&& _bar, std::unique_ptr<Node>&& _next) :
      bar{std::move(_bar)},
      next{std::move(_next)} {}
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

std::unique_ptr<LengthNode> build_bar(Lexer::Tokens& tokens) {
  static constexpr auto check = []<class T>(const T&) -> bool {
    return (std::is_same_v<T, Music::Note> || std::is_same_v<T, Music::Rest>);
  };

  if (!tokens.empty() && std::visit(check, tokens.front())) {
    Lexer::Token token = Lexer::poll_token(tokens);
    return std::make_unique<LengthNode>(token, build_bar(tokens));
  } else
    return nullptr;
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
