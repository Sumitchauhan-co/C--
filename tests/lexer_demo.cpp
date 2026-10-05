// Small standalone example for inspecting the lexer.
// The main C-- executable currently runs the complete pipeline.

#include "lexer/lexer.hpp"
#include <iostream>

int main()
{
    // Tests both math assignments, boolean definitions, and multi-type comparisons
    Lexer lexer(
        "let x = 10 + 20; "
        "let age = 20; "
        "let adult = age >= 18; "
        "say adult == true; "
        "say age != 15;");

    const auto tokens = lexer.tokenize();

    for (const auto &token : tokens)
    {
        std::cout << tokenTypeName(token.type)
                  << " [" << token.lexeme << "]\n";
    }
}
