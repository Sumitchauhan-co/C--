#include "lexer/lexer.hpp"
#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <utility>

// ==========================================
// Token Type String Conversion
// ==========================================

std::string tokenTypeName(TokenType type)
{
    switch (type)
    {
    case TokenType::EndOfFile:
        return "EndOfFile";
    case TokenType::Newline:
        return "Newline";
    case TokenType::Number:
        return "Number";
    case TokenType::String:
        return "String";
    case TokenType::Identifier:
        return "Identifier";
    case TokenType::True:
        return "True";
    case TokenType::False:
        return "False";
    case TokenType::Let:
        return "Let";
    case TokenType::Fix:
        return "Fix";
    case TokenType::Say:
        return "Say";
    case TokenType::Ask:
        return "Ask";
    case TokenType::If:
        return "If";
    case TokenType::Elif:
        return "Elif";
    case TokenType::Else:
        return "Else";
    case TokenType::While:
        return "While";
    case TokenType::Do:
        return "Do";
    case TokenType::For:
        return "For";
    case TokenType::Break:
        return "Break";
    case TokenType::Continue:
        return "Continue";
    case TokenType::Pass:
        return "Pass";
    case TokenType::And:
        return "And";
    case TokenType::Or:
        return "Or";
    case TokenType::Not:
        return "Not";
    case TokenType::Plus:
        return "Plus";
    case TokenType::PlusPlus:
        return "PlusPlus";
    case TokenType::Minus:
        return "Minus";
    case TokenType::MinusMinus:
        return "MinusMinus";
    case TokenType::Star:
        return "Star";
    case TokenType::Slash:
        return "Slash";
    case TokenType::Equal:
        return "Equal";
    case TokenType::EqualEqual:
        return "EqualEqual";
    case TokenType::BangEqual:
        return "BangEqual";
    case TokenType::Greater:
        return "Greater";
    case TokenType::GreaterEqual:
        return "GreaterEqual";
    case TokenType::Less:
        return "Less";
    case TokenType::LessEqual:
        return "LessEqual";
    case TokenType::LeftParen:
        return "LeftParen";
    case TokenType::RightParen:
        return "RightParen";
    case TokenType::LeftBrace:
        return "LeftBrace";
    case TokenType::RightBrace:
        return "RightBrace";
    case TokenType::Comma:
        return "Comma";
    case TokenType::Semicolon:
        return "Semicolon";
    }
    return "Unknown";
}

// ==========================================
// Lexer Core Implementation
// ==========================================

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

char Lexer::peek(std::size_t distance) const
{
    return current_ + distance < source_.size() ? source_[current_ + distance] : '\0';
}

char Lexer::take()
{
    char c = peek();
    if (c != '\0')
    {
        ++current_;
        if (c == '\n')
        {
            ++line_;
            column_ = 1;
        }
        else
        {
            ++column_;
        }
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

bool Lexer::isAlpha(char c) const
{
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

bool Lexer::isIdentifierStart(char c) const
{
    return isAlpha(c) || c == '_';
}

bool Lexer::isIdentifierPart(char c) const
{
    return isIdentifierStart(c) || isDigit(c);
}

std::vector<Token> Lexer::scan()
{
    std::vector<Token> tokens;

    while (peek() != '\0')
    {
        char c = peek();
        auto tokenLine = line_;
        auto tokenCol = column_;

        if (c == ' ' || c == '\t' || c == '\r')
        {
            take();
            continue;
        }

        if (c == '\n' || c == ';')
        {
            take();
            tokens.push_back({TokenType::Newline, ";", tokenLine, tokenCol});
            continue;
        }

        if (c == '/' && peek(1) == '/')
        {
            while (peek() != '\0' && peek() != '\n')
                take();
            continue;
        }

        if (c == '/' && peek(1) == '*')
        {
            take();
            take();
            while (peek() != '\0' && !(peek() == '*' && peek(1) == '/'))
                take();
            if (peek() == '\0')
                throw std::runtime_error("Lexer error: Unterminated block comment.");
            take();
            take();
            continue;
        }

        if (isDigit(c) || (c == '.' && isDigit(peek(1))))
        {
            std::string number;
            bool dot = false;
            while (isDigit(peek()) || (!dot && peek() == '.'))
            {
                if (peek() == '.')
                    dot = true;
                number += take();
            }
            tokens.push_back({TokenType::Number, number, tokenLine, tokenCol});
            continue;
        }

        if (c == '"')
        {
            take();
            std::string value;
            while (peek() != '\0' && peek() != '"')
            {
                if (peek() == '\n')
                    throw std::runtime_error("Lexer error: String literal cannot cross a newline.");
                value += take();
            }
            if (peek() == '\0')
                throw std::runtime_error("Lexer error: Unterminated string literal.");
            take();
            tokens.push_back({TokenType::String, value, tokenLine, tokenCol});
            continue;
        }

        if (isIdentifierStart(c))
        {
            std::string word;
            while (isIdentifierPart(peek()))
            {
                word += take();
            }

            TokenType type = TokenType::Identifier;
            static const std::unordered_map<std::string, TokenType> keywords = {
                {"let", TokenType::Let},
                {"fix", TokenType::Fix},
                {"say", TokenType::Say},
                {"ask", TokenType::Ask},
                {"if", TokenType::If},
                {"elif", TokenType::Elif},
                {"else", TokenType::Else},
                {"while", TokenType::While},
                {"do", TokenType::Do},
                {"for", TokenType::For},
                {"break", TokenType::Break},
                {"continue", TokenType::Continue},
                {"pass", TokenType::Pass},
                {"true", TokenType::True},
                {"false", TokenType::False},
                {"and", TokenType::And},
                {"or", TokenType::Or},
                {"not", TokenType::Not}};

            auto it = keywords.find(word);
            if (it != keywords.end())
            {
                type = it->second;
            }

            tokens.push_back({type, word, tokenLine, tokenCol});
            continue;
        }

        take();
        switch (c)
        {
        case '(':
            tokens.push_back({TokenType::LeftParen, "(", tokenLine, tokenCol});
            break;
        case ')':
            tokens.push_back({TokenType::RightParen, ")", tokenLine, tokenCol});
            break;
        case '{':
            tokens.push_back({TokenType::LeftBrace, "{", tokenLine, tokenCol});
            break;
        case '}':
            tokens.push_back({TokenType::RightBrace, "}", tokenLine, tokenCol});
            break;
        case ',':
            tokens.push_back({TokenType::Comma, ",", tokenLine, tokenCol});
            break;
        case '+':
            if (peek() == '+')
            {
                take();
                tokens.push_back({TokenType::PlusPlus, "++", tokenLine, tokenCol});
            }
            else
            {
                tokens.push_back({TokenType::Plus, "+", tokenLine, tokenCol});
            }
            break;
        case '-':
            if (peek() == '-')
            {
                take();
                tokens.push_back({TokenType::MinusMinus, "--", tokenLine, tokenCol});
            }
            else
            {
                tokens.push_back({TokenType::Minus, "-", tokenLine, tokenCol});
            }
            break;
        case '*':
            tokens.push_back({TokenType::Star, "*", tokenLine, tokenCol});
            break;
        case '/':
            tokens.push_back({TokenType::Slash, "/", tokenLine, tokenCol});
            break;

        case '=':
            if (peek() == '=')
            {
                take();
                tokens.push_back({TokenType::EqualEqual, "==", tokenLine, tokenCol});
            }
            else
            {
                tokens.push_back({TokenType::Equal, "=", tokenLine, tokenCol});
            }
            break;

        case '!':
            if (peek() == '=')
            {
                take();
                tokens.push_back({TokenType::BangEqual, "!=", tokenLine, tokenCol});
            }
            else
            {
                throw std::runtime_error("Lexer error: unexpected '!'. Use 'not' or '!='.");
            }
            break;

        case '>':
            if (peek() == '=')
            {
                take();
                tokens.push_back({TokenType::GreaterEqual, ">=", tokenLine, tokenCol});
            }
            else
            {
                tokens.push_back({TokenType::Greater, ">", tokenLine, tokenCol});
            }
            break;

        case '<':
            if (peek() == '=')
            {
                take();
                tokens.push_back({TokenType::LessEqual, "<=", tokenLine, tokenCol});
            }
            else
            {
                tokens.push_back({TokenType::Less, "<", tokenLine, tokenCol});
            }
            break;

        default:
            throw std::runtime_error("Lexer error at line " + std::to_string(tokenLine) +
                                     ", column " + std::to_string(tokenCol) +
                                     ": unexpected character '" + std::string(1, c) + "'.");
        }
    }

    tokens.push_back({TokenType::EndOfFile, "", line_, column_});
    return tokens;
}