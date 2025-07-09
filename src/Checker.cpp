#include "Checker.hpp"
#include "Parser.hpp"
#include <print>

namespace Checker {
void analyze(const Parser::Ast& ast) {
  if (!ast)
    return;

  auto bar_node = dynamic_cast<Parser::BarNode*>(ast.get());
  if (!bar_node)
    return analyze(ast->next);

  Music::Length measure{0, 1};
  auto lengthed_node = bar_node->bar.get();
  while (lengthed_node) {
    auto node_not = dynamic_cast<Parser::NoteNode*>(lengthed_node);
    if (node_not) {
      measure += node_not->note.length;
    } else {
      auto rest_node = dynamic_cast<Parser::RestNode*>(lengthed_node);
      measure += rest_node->rest.length;
    }
    lengthed_node = lengthed_node->next.get();
  }

  std::print("Bar length: {}\n", measure);
  if (measure != Music::Length(3, 4))
    throw Checker::LengthError(measure, {3, 4});

  analyze(ast->next);
}
} // namespace Checker
