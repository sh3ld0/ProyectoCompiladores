#include "Parser.hpp"
#include "Lexer.hpp"
#include <map>
#include <memory>

namespace {
const std::map<Music::Tone, std::string> tone_to_string = {
    {Music::Tone::C, "C"},   {Music::Tone::CS, "C#"}, {Music::Tone::D, "D"},
    {Music::Tone::DS, "D#"}, {Music::Tone::E, "E"},   {Music::Tone::F, "F"},
    {Music::Tone::FS, "F#"}, {Music::Tone::G, "G"},   {Music::Tone::GS, "G#"},
    {Music::Tone::A, "A"},   {Music::Tone::AS, "A#"}, {Music::Tone::B, "B"}};

struct LengthNode : Parser::Node {
  using List = std::unique_ptr<LengthNode>;
  static constexpr bool check(const Lexer::Token& token) {
    return std::visit(
        []<class T>(const T&) -> bool {
          if constexpr (std::is_same_v<T, Music::Note>)
            return true;
          else if constexpr (std::is_same_v<T, Music::Rest>)
            return true;
          else
            return false;
        },
        token);
  };

  Lexer::Token token;
  List next;

  LengthNode(const Lexer::Token& _token, List&& _next) :
      token{_token},
      next{std::move(_next)} {}
  virtual ~LengthNode() override = default;
  virtual void evaluate(Music::Midi& context) const override {
    std::visit(
        [&context]<class T>(const T& token) {
          if constexpr (std::is_same_v<T, Music::Note>)
            context.addNote(token);
          else if constexpr (std::is_same_v<T, Music::Rest>)
            context.addRest(token);
          else if constexpr (std::is_same_v<T, Music::Bar>)
            throw std::runtime_error("Bar in unexpected place");
          else
            static_assert(false, "non-exhaustive visitor");
        },
        token);
    if (next)
      next->evaluate(context);
  }
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const override {
    std::visit(
        [&]<class T>(const T& token) {
          if constexpr (std::is_same_v<T, Music::Note>) {
            os << indent << tone_to_string.at(token.tone) << token.octave << '-'
               << token.length.num << '/' << token.length.dem << '\n';
          } else if constexpr (std::is_same_v<T, Music::Rest>) {
            os << indent << "R-" << token.length.num << '/' << token.length.dem
               << '\n';
          }
        },
        token);

    if (next)
      next->print(os, indent + "  ");
    else
      os << indent << "End Bar\n";
  }
};

struct BarNode : Parser::Node {
  using List = std::unique_ptr<BarNode>;
  static constexpr bool check(const Lexer::Token& token) {
    return std::visit(
        []<class T>(const T&) -> bool {
          return std::is_same_v<T, Music::Bar>;
        },
        token);
  };

  LengthNode::List bar;
  List next;

  static LengthNode::List build_list(Lexer::Tokens& tokens) {
    if (!tokens.empty() && LengthNode::check(tokens.front())) {
      Lexer::Token token = Lexer::poll_token(tokens);
      return std::make_unique<LengthNode>(token, build_list(tokens));
    } else
      return nullptr;
  }

  BarNode(LengthNode::List&& _bar, List&& _next) :
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
    os << indent << "Bar\n";
    if (bar)
      bar->print(os, indent + "  ");

    if (next)
      next->print(os, indent + "  ");
    else
      os << indent << "End score\n";
  }
};

struct ScoreNode : Parser::Node {
  BarNode::List score;

  static BarNode::List build_list(Lexer::Tokens& tokens) {
    if (!tokens.empty() && BarNode::check(tokens.front())) {
      tokens.pop();
      auto bar = BarNode::build_list(tokens);
      return std::make_unique<BarNode>(std::move(bar), build_list(tokens));
    } else
      return nullptr;
  }

  ScoreNode(Lexer::Tokens& tokens) : score{build_list(tokens)} {}
  virtual void evaluate(Music::Midi& context) const override {
    if (score)
      score->evaluate(context);
    context.write();
  }
  virtual void print(std::ostream& os,
                     const std::string& indent = "") const override {
    os << indent << "Score\n";
    if (score)
      score->print(os, indent + "  ");
  }
};
}; // namespace

Parser::Ast Parser::analyze(Lexer::Tokens& tokens) {
  auto ans = std::make_unique<ScoreNode>(tokens);
  if (!tokens.empty())
    throw std::runtime_error("Trailing tokens");

  return ans;
}
