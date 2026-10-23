#include "interpreter/interpreter.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

int main(int argc, char *argv[])
{
  try
  {
    std::string source;

    if (argc > 1)
    {
      std::string filename = argv[1];
      std::ifstream file(filename);
      if (!file)
      {
        throw std::runtime_error("Could not open file '" + filename + "'.");
      }
      std::ostringstream buffer;
      buffer << file.rdbuf();
      source = buffer.str();
    }
    else
    {
      // Fallback to standard input if no file argument is provided
      std::ostringstream buffer;
      buffer << std::cin.rdbuf();
      source = buffer.str();
    }

    // 1. Lexical Analysis
    Lexer lexer(source);
    auto tokens = lexer.scan();

    // 2. Syntactic Analysis (Parsing)
    Parser parser(tokens);
    auto program = parser.parse();

    // 3. Execution / Interpretation
    Interpreter interpreter;
    interpreter.run(program);
  }
  catch (const std::exception &error)
  {
    std::cerr << "C-- error: " << error.what() << '\n';
    return 1;
  }

  return 0;
}