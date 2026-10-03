#pragma once

#include "lexer/token.hpp"

#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(std::string source);

    std::vector<Token> tokenize();

private:
    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;

    void skipWhitespace();
    Token scanNumber();
    Token scanIdentifier();

    bool isDigit(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierPart(char c) const;

    std::string source_;
    std::size_t current_ = 0;
    std::size_t line_ = 1;
    std::size_t column_ = 1;
};
