#pragma once

#include <cstddef>
#include <string>

enum class TokenType
{
    // Literals and Identifiers
    Number,
    Identifier,
    True,
    False,

    // Keywords
    Let,
    Say,

    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Equal,

    // Comparisons
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    EqualEqual,
    BangEqual,

    // Delimiters
    LeftParen,
    RightParen,
    Semicolon,

    // Special
    EndOfFile
};

struct Token
{
    TokenType type;
    std::string lexeme;
    std::size_t line;
    std::size_t column;
};

std::string tokenTypeName(TokenType type);
