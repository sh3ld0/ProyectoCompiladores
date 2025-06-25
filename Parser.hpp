#include "Lexer.hpp"
#include <memory>

namespace Parser {
struct Node {
  virtual ~Node() = default;
  virtual void evaluate(Music::Midi&) const = 0;
  virtual void print(std::ostream& os) const = 0;
};
using NodePtr = std::unique_ptr<Node>;

namespace {
struct TokenNode : Node {
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
  virtual void print(std::ostream& os) const override {
    static const std::map<Music::Tone, std::string> tone_to_string = {
        {Music::Tone::C, "C"},   {Music::Tone::CS, "C#"},
        {Music::Tone::D, "D"},   {Music::Tone::DS, "D#"},
        {Music::Tone::E, "E"},   {Music::Tone::F, "F"},
        {Music::Tone::FS, "F#"}, {Music::Tone::G, "G"},
        {Music::Tone::GS, "G#"}, {Music::Tone::A, "A"},
        {Music::Tone::AS, "A#"}, {Music::Tone::B, "B"}};

    std::visit(
        [&]<class T>(const T& token) {
          if constexpr (std::is_same_v<T, Music::Note>)
            os << tone_to_string.at(token.tone) << token.octave << '-'
               << token.length.num << '/' << token.length.dem << '\n';
          else if constexpr (std::is_same_v<T, Music::Rest>)
            os << "M-" << token.length.num << '/' << token.length.dem << '\n';
          else
            static_assert(false, "non-exhaustive visitor");
        },
        token);
  }
};

struct EndNode : Node {
  virtual ~EndNode() override = default;
  virtual void evaluate(Music::Midi& context) const override {
    context.write();
  }
  virtual void print(std::ostream& os) const override {
    os << "END\n";
  }
};

struct ConnectorNode : Node {
  NodePtr left;
  NodePtr right;

  ConnectorNode(NodePtr&& _left, NodePtr&& _right) :
      left{std::move(_left)},
      right{std::move(_right)} {}
  virtual ~ConnectorNode() override = default;
  virtual void evaluate(Music::Midi& context) const override {
    left->evaluate(context);
    right->evaluate(context);
  }
  virtual void print(std::ostream& os) const override {
    left->print(os);
    right->print(os);
  }
};
}; // namespace

inline NodePtr analyze(Lexer::Tokens& tokens) {
  if (tokens.empty())
    return std::make_unique<EndNode>();

  auto token_node = std::make_unique<TokenNode>(Lexer::poll_token(tokens));
  return std::make_unique<ConnectorNode>(std::move(token_node),
                                         analyze(tokens));
}
}; // namespace Parser
