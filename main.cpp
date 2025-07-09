#include "Lexer.hpp"
#include "Parser.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Pass input file name as first argument\n"
              << "and output file name as second argument\n";
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "No se pudo abrir el archivo: " << argv[1] << '\n';
    return 1;
  }

  Lexer::Tokens tokens;
  try {
    tokens = Lexer::analyze(file);
  } catch (std::invalid_argument& i_a) {
    std::cerr << "Lexical error\n";
    std::cerr << i_a.what() << '\n';
    return 1;
  }

  Parser::Ast ast;
  try {
    ast = Parser::analyze(tokens);
    ast->print(std::cout);
  } catch (std::runtime_error& i_a) {
    std::cerr << "Syntax error\n";
    std::cerr << i_a.what() << '\n';
    return 1;
  }

  Music::Midi context{argv[2], 4 * 60};
  ast->evaluate(context);
  std::cout << "Archivo " << argv[2] << " generado correctamente.\n";
}
