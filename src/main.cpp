#include "interpreter/interpreter.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static std::string readFile(const std::string &path)
{
    std::ifstream file(path);

    if (!file)
    {
        throw std::runtime_error("Could not open file: " + path);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: c-- <file.cmm>\n";
        return 1;
    }

    try
    {
        const std::string source = readFile(argv[1]);

        Lexer lexer(source);
        const auto tokens = lexer.tokenize();

        Parser parser(tokens);
        const auto program = parser.parse();

        Interpreter interpreter;
        interpreter.execute(program);

        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
