#include "Parser.hpp"
#include <map>

namespace {
const std::map<Music::Tone, std::string> tone_to_string = {
    {Music::Tone::C, "C"},   {Music::Tone::CS, "C#"}, {Music::Tone::D, "D"},
    {Music::Tone::DS, "D#"}, {Music::Tone::E, "E"},   {Music::Tone::F, "F"},
    {Music::Tone::FS, "F#"}, {Music::Tone::G, "G"},   {Music::Tone::GS, "G#"},
    {Music::Tone::A, "A"},   {Music::Tone::AS, "A#"}, {Music::Tone::B, "B"}};

struct TokenNode : Parser::Node {
  Lexer::Token token;

  TokenNode(Lexer::Token&& _token) : token{_token} {}
  virtual ~TokenNode() override = default;
  virtual void evaluate(Music::Midi& context) const override {
    std::visit(
        [&context]<class T>(const T& token) {
          if constexpr (std::is_same_v<T, Music::Note>)
            context.addNote(token);
          else if constexpr (std::is_same_v<T, Music::Rest>)
            context.addRest(token);
          else
            static_assert(false, "non-exhaustive visitor");
        },
        token);
  }
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const override {
    std::visit(
        [&]<class T>(const T& token) {
          if constexpr (std::is_same_v<T, Music::Note>) {
            os << indent << tone_to_string.at(token.tone) << token.octave
               << token.length.num << '/' << token.length.dem << '\n';
          } else if constexpr (std::is_same_v<T, Music::Rest>) {
            os << indent << token.length.num << '/' << token.length.dem << '\n';
          }
        },
        token);
  }
};

struct EndNode : Parser::Node {
  virtual ~EndNode() override = default;
  virtual void evaluate(Music::Midi& context) const override {
    context.write();
  }
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const override {
    os << indent << "END\n";
  }
};

struct ConnectorNode : Parser::Node {
  Parser::Ast left;
  Parser::Ast right;

  ConnectorNode(Parser::Ast&& _left, Parser::Ast&& _right) :
      left{std::move(_left)},
      right{std::move(_right)} {}
  virtual ~ConnectorNode() override = default;
  virtual void evaluate(Music::Midi& context) const override {
    left->evaluate(context);
    right->evaluate(context);
  }
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const override {
    os << indent << "Connector\n";
    left->print(os, indent + "├─ ");
    right->print(os, indent + "└─ ");
  }
};
}; // namespace

Parser::Ast Parser::analyze(Lexer::Tokens& tokens) {
  if (tokens.empty())
    return std::make_unique<EndNode>();

  auto token_node = std::make_unique<TokenNode>(Lexer::poll_token(tokens));
  return std::make_unique<ConnectorNode>(std::move(token_node),
                                         analyze(tokens));
}
