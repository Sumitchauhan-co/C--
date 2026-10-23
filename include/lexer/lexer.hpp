#pragma once

#include "lexer/token.hpp"

#include <string>
#include <vector>

class Lexer
{
public:
    explicit Lexer(std::string source);

    // Main entry point for tokenization (alias support included)
    std::vector<Token> scan();
    std::vector<Token> tokenize() { return scan(); }

private:
    // Core character navigation
    char peek(std::size_t distance = 0) const;
    char peekNext() const { return peek(1); }
    char take();
    char advance() { return take(); }
    bool isAtEnd() const;

    // Token scanning routines
    Token scanToken();
    Token scanNumber();
    Token scanIdentifier();
    Token scanString();

    // Character classification helpers
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierPart(char c) const;

    // Member variables
    std::string source_;
    std::size_t current_ = 0;
    std::size_t line_ = 1;
    std::size_t column_ = 1;
};