#include "interpreter/interpreter.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: cmm <file.cmm>\n";
    return 1;
  }

  std::string filename = argv[1];
  if (filename.size() < 4 || filename.substr(filename.size() - 4) != ".cmm")
  {
    std::cerr << "Error: source file must use the .cmm extension.\n";
    return 1;
  }

  std::ifstream file(filename);
  if (!file)
  {
    std::cerr << "Error: could not open file '" << filename << "'.\n";
    return 1;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  try
  {
    Lexer lexer(buffer.str());
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto program = parser.parse();

    Interpreter interpreter;
    interpreter.execute(program);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
