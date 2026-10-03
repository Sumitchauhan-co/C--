// Small standalone example for inspecting the lexer.
// The main C-- executable currently runs the complete pipeline.

#include "lexer/lexer.hpp"

#include <iostream>

int main()
{
    Lexer lexer("let x = 10 + 20;");
    const auto tokens = lexer.tokenize();

    for (const auto &token : tokens)
    {
        std::cout << tokenTypeName(token.type)
                  << " [" << token.lexeme << "]\n";
    }
}
