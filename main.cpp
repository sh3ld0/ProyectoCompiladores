#include "Checker.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

template <class... Args>
void println_err(std::format_string<Args...> fmt, Args&&... args) {
  std::println(std::cerr, fmt, std::forward<Args>(args)...);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    println_err("Pass input file name as first argument");
    println_err("and output file name as second argument");
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    println_err("Could not open file: {}", argv[1]);
    return 1;
  }

  Lexer::Tokens tokens;
  try {
    tokens = Lexer::analyze(file);
  } catch (const std::invalid_argument& i_a) {
    println_err("Lexical error: {}", i_a.what());
    return 1;
  }

  Parser::Ast ast;
  try {
    ast = Parser::analyze(tokens);
    ast->print(std::cout);
  } catch (std::runtime_error& r_e) {
    println_err("Syntax error: {}", r_e.what());
    return 1;
  }

  try {
    Checker::analyze(ast);
  } catch (Checker::SemanticError& s_e) {
    println_err("Semantics error: {}", s_e.what());
    return 1;
  }

  Music::Midi context{argv[2], 4 * 60};
  ast->evaluate(context);
  std::cout << "File " << argv[2] << " generated correctly.\n";
}
