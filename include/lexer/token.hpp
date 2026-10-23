#pragma once

#include <cstddef>
#include <string>

enum class TokenType
{
    // Special / End of File
    EndOfFile,
    End = EndOfFile,
    Newline,

    // Literals and Identifiers
    Number,
    String,
    Identifier,
    True,
    False,

    // Keywords
    Let,
    Fix, // For immutable declarations
    Say,
    Ask, // For input syntax
    If,
    Elif,
    Else,
    And,
    Or,
    Not,

    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Equal,
    EqualEqual,
    BangEqual,

    // Comparisons
    Greater,
    GreaterEqual,
    Less,
    LessEqual,

    // Delimiters & Grouping
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Comma,
    Semicolon
};

struct Token
{
    TokenType type;
    std::string lexeme; // Using lexeme (compatible with text via alias if needed)
    std::size_t line;
    std::size_t column;
};

std::string tokenTypeName(TokenType type);