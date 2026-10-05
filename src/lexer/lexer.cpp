#include "lexer/lexer.hpp"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

std::string tokenTypeName(TokenType type)
{
    switch (type)
    {
    case TokenType::Number:
        return "Number";
    case TokenType::Identifier:
        return "Identifier";
    case TokenType::Let:
        return "Let";
    case TokenType::Say:
        return "Say";
    case TokenType::True:
        return "True";
    case TokenType::False:
        return "False";
    case TokenType::Plus:
        return "Plus";
    case TokenType::Minus:
        return "Minus";
    case TokenType::Star:
        return "Star";
    case TokenType::Slash:
        return "Slash";
    case TokenType::Greater:
        return "Greater";
    case TokenType::GreaterEqual:
        return "GreaterEqual";
    case TokenType::Less:
        return "Less";
    case TokenType::LessEqual:
        return "LessEqual";
    case TokenType::Equal:
        return "Equal";
    case TokenType::EqualEqual:
        return "EqualEqual";
    case TokenType::BangEqual:
        return "BangEqual";
    case TokenType::LeftParen:
        return "LeftParen";
    case TokenType::RightParen:
        return "RightParen";
    case TokenType::Semicolon:
        return "Semicolon";
    case TokenType::EndOfFile:
        return "EndOfFile";
    }
    return "Unknown";
}

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

char Lexer::peek() const
{
    return isAtEnd() ? '\0' : source_[current_];
}

char Lexer::peekNext() const
{
    return (current_ + 1 >= source_.size()) ? '\0' : source_[current_ + 1];
}

char Lexer::advance()
{
    char c = source_[current_++];
    if (c == '\n')
    {
        ++line_;
        column_ = 1;
    }
    else
    {
        ++column_;
    }
    return c;
}

bool Lexer::isAtEnd() const
{
    return current_ >= source_.size();
}

bool Lexer::isDigit(char c) const
{
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

bool Lexer::isIdentifierStart(char c) const
{
    return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool Lexer::isIdentifierPart(char c) const
{
    return isIdentifierStart(c) || isDigit(c);
}

void Lexer::skipWhitespace()
{
    while (!isAtEnd())
    {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n')
        {
            advance();
            continue;
        }
        if (c == '/' && peekNext() == '/')
        {
            while (!isAtEnd() && peek() != '\n')
            {
                advance();
            }
            continue;
        }
        break;
    }
}

Token Lexer::scanNumber()
{
    auto startColumn = column_;
    auto start = current_;

    while (isDigit(peek()))
    {
        advance();
    }

    return {TokenType::Number, source_.substr(start, current_ - start), line_, startColumn};
}

Token Lexer::scanIdentifier()
{
    auto startColumn = column_;
    auto start = current_;

    while (isIdentifierPart(peek()))
    {
        advance();
    }

    auto text = source_.substr(start, current_ - start);
    static const std::unordered_map<std::string, TokenType> keywords{
        {"let", TokenType::Let},
        {"say", TokenType::Say},
        {"true", TokenType::True},
        {"false", TokenType::False}};

    auto it = keywords.find(text);
    return {it == keywords.end() ? TokenType::Identifier : it->second, text, line_, startColumn};
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (!isAtEnd())
    {
        skipWhitespace();
        if (isAtEnd())
            break;

        auto tokenLine = line_;
        auto tokenColumn = column_;
        char c = advance();

        if (isDigit(c))
        {
            --current_;
            --column_;
            tokens.push_back(scanNumber());
            continue;
        }

        if (isIdentifierStart(c))
        {
            --current_;
            --column_;
            tokens.push_back(scanIdentifier());
            continue;
        }

        switch (c)
        {
        case '+':
            tokens.push_back({TokenType::Plus, "+", tokenLine, tokenColumn});
            break;
        case '-':
            tokens.push_back({TokenType::Minus, "-", tokenLine, tokenColumn});
            break;
        case '*':
            tokens.push_back({TokenType::Star, "*", tokenLine, tokenColumn});
            break;
        case '/':
            tokens.push_back({TokenType::Slash, "/", tokenLine, tokenColumn});
            break;
        case '(':
            tokens.push_back({TokenType::LeftParen, "(", tokenLine, tokenColumn});
            break;
        case ')':
            tokens.push_back({TokenType::RightParen, ")", tokenLine, tokenColumn});
            break;
        case ';':
            tokens.push_back({TokenType::Semicolon, ";", tokenLine, tokenColumn});
            break;

        case '>':
            if (peek() == '=')
            {
                advance();
                tokens.push_back({TokenType::GreaterEqual, ">=", tokenLine, tokenColumn});
            }
            else
            {
                tokens.push_back({TokenType::Greater, ">", tokenLine, tokenColumn});
            }
            break;

        case '<':
            if (peek() == '=')
            {
                advance();
                tokens.push_back({TokenType::LessEqual, "<=", tokenLine, tokenColumn});
            }
            else
            {
                tokens.push_back({TokenType::Less, "<", tokenLine, tokenColumn});
            }
            break;

        case '=':
            if (peek() == '=')
            {
                advance();
                tokens.push_back({TokenType::EqualEqual, "==", tokenLine, tokenColumn});
            }
            else
            {
                tokens.push_back({TokenType::Equal, "=", tokenLine, tokenColumn});
            }
            break;

        case '!':
            if (peek() == '=')
            {
                advance();
                tokens.push_back({TokenType::BangEqual, "!=", tokenLine, tokenColumn});
            }
            else
            {
                throw std::runtime_error("Lexer error at line " + std::to_string(tokenLine) +
                                         ", column " + std::to_string(tokenColumn) + ": unexpected '!'.");
            }
            break;

        default:
            throw std::runtime_error("Lexer error at line " + std::to_string(tokenLine) +
                                     ", column " + std::to_string(tokenColumn) +
                                     ": unexpected character '" + std::string(1, c) + "'.");
        }
    }

    tokens.push_back({TokenType::EndOfFile, "", line_, column_});
    return tokens;
}
