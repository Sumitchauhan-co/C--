#pragma once

#include <cstddef>
#include <string>

enum class TokenType {
    Number,
    Identifier,

    Let,
    Say,

    Plus,
    Minus,
    Star,
    Slash,
    Equal,

    LeftParen,
    RightParen,
    Semicolon,

    EndOfFile
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::size_t line;
    std::size_t column;
};

std::string tokenTypeName(TokenType type);
