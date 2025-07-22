#include "Checker.hpp"
#include "Parser.hpp"
#include <format>
#include <print>

namespace Checker {
void analyze(const Parser::Ast& ast, Music::Length signature) {
  if (!ast)
    return;

  auto bar_node = dynamic_cast<Parser::BarNode*>(ast.get());
  if (!bar_node) {
    auto signature_node = dynamic_cast<Parser::SignatureNode*>(ast.get());
    if (signature_node)
      signature = signature_node->length;
    return analyze(ast->next, signature);
  }

  Music::Length measure{0, 1};
  auto lengthed_node = bar_node->bar.get();
  while (lengthed_node) {
    auto note_node = dynamic_cast<Parser::NoteNode*>(lengthed_node);
    if (note_node) {
      measure += note_node->note.length;
      std::print("{}, ", note_node->note.length);
    } else {
      auto rest_node = dynamic_cast<Parser::RestNode*>(lengthed_node);
      measure += rest_node->rest.length;
      std::print("{}, ", rest_node->rest.length);
    }
    lengthed_node = lengthed_node->next.get();
  }

  std::println("Length {}", measure);
  if (measure != signature)
    throw Checker::LengthError(measure, signature);

  analyze(ast->next, signature);
}
} // namespace Checker
