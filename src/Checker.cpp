#include "Checker.hpp"

namespace Checker {
void analyze(const Parser::Ast& ast) {
  if (!ast)
    return;

  auto bar_node = dynamic_cast<Parser::BarNode*>(ast.get());
  if (!bar_node)
    return analyze(ast->next);

  Music::Length measure{0, 1};
  auto note_node = bar_node->bar.get();
  while (note_node) {
    note_node = note_node->next.get();
  }

  analyze(ast->next);
}
} // namespace Checker
