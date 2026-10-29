#pragma once

#include <string>

enum class TokenType
{
    // Literals & Identifiers
    EndOfFile,
    Newline,
    Number,
    String,
    TemplateString, // Backtick strings: `...`
    Identifier,
    True,
    False,

    // Keywords
    Let,
    Fix,
    Say,
    Ask,
    If,
    Elif,
    Else,
    While,
    Do,
    For,
    Break,
    Continue,
    Pass,
    Fn,
    Return,
    Try,
    Catch,
    Throw,

    // Logical Operators
    And,
    Or,
    Not,

    // Arithmetic & Assignment Operators
    Plus,
    PlusPlus,
    Minus,
    MinusMinus,
    Star,
    Slash,
    Equal,

    // Comparison Operators
    EqualEqual,
    BangEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,

    // Delimiters & Grouping
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftBracket,  // [
    RightBracket, // ]
    Colon,        // :
    Comma,
    Semicolon
};

struct Token
{
    TokenType type;
    std::string lexeme;
    std::size_t line;
    std::size_t column;
};

std::string tokenTypeName(TokenType type);