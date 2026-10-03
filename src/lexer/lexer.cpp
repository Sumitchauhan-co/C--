#include "lexer/lexer.hpp"

#include <cctype>
#include <stdexcept>
#include <unordered_map>

Lexer::Lexer(std::string source)
    : source_(std::move(source)) {}

char Lexer::peek() const {
    return isAtEnd() ? '\0' : source_[current_];
}

char Lexer::peekNext() const {
    return current_ + 1 >= source_.size() ? '\0' : source_[current_ + 1];
}

char Lexer::advance() {
    const char c = source_[current_++];
    ++column_;
    return c;
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.size();
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

bool Lexer::isIdentifierStart(char c) const {
    return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool Lexer::isIdentifierPart(char c) const {
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        const char c = peek();

        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
            continue;
        }

        if (c == '\n') {
            advance();
            ++line_;
            column_ = 1;
            continue;
        }

        if (c == '/' && peekNext() == '/') {
            while (!isAtEnd() && peek() != '\n') {
                advance();
            }
            continue;
        }

        break;
    }
}

Token Lexer::scanNumber() {
    const std::size_t start = current_;
    const std::size_t line = line_;
    const std::size_t column = column_;

    while (isDigit(peek())) {
        advance();
    }

    return {TokenType::Number, source_.substr(start, current_ - start), line, column};
}

Token Lexer::scanIdentifier() {
    const std::size_t start = current_;
    const std::size_t line = line_;
    const std::size_t column = column_;

    while (isIdentifierPart(peek())) {
        advance();
    }

    const std::string text = source_.substr(start, current_ - start);

    if (text == "let") {
        return {TokenType::Let, text, line, column};
    }

    if (text == "say") {
        return {TokenType::Say, text, line, column};
    }

    return {TokenType::Identifier, text, line, column};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) {
            break;
        }

        const std::size_t line = line_;
        const std::size_t column = column_;
        const char c = advance();

        switch (c) {
        case '+':
            tokens.push_back({TokenType::Plus, "+", line, column});
            break;
        case '-':
            tokens.push_back({TokenType::Minus, "-", line, column});
            break;
        case '*':
            tokens.push_back({TokenType::Star, "*", line, column});
            break;
        case '/':
            tokens.push_back({TokenType::Slash, "/", line, column});
            break;
        case '=':
            tokens.push_back({TokenType::Equal, "=", line, column});
            break;
        case '(':
            tokens.push_back({TokenType::LeftParen, "(", line, column});
            break;
        case ')':
            tokens.push_back({TokenType::RightParen, ")", line, column});
            break;
        case ';':
            tokens.push_back({TokenType::Semicolon, ";", line, column});
            break;
        default:
            if (isDigit(c)) {
                --current_;
                tokens.push_back(scanNumber());
            } else if (isIdentifierStart(c)) {
                --current_;
                tokens.push_back(scanIdentifier());
            } else {
                throw std::runtime_error(
                    "Lexer error at line " + std::to_string(line) +
                    ", column " + std::to_string(column) +
                    ": unexpected character '" + std::string(1, c) + "'"
                );
            }
            break;
        }
    }

    tokens.push_back({TokenType::EndOfFile, "", line_, column_});
    return tokens;
}

std::string tokenTypeName(TokenType type) {
    switch (type) {
    case TokenType::Number: return "NUMBER";
    case TokenType::Identifier: return "IDENTIFIER";
    case TokenType::Let: return "LET";
    case TokenType::Say: return "SAY";
    case TokenType::Plus: return "PLUS";
    case TokenType::Minus: return "MINUS";
    case TokenType::Star: return "STAR";
    case TokenType::Slash: return "SLASH";
    case TokenType::Equal: return "EQUAL";
    case TokenType::LeftParen: return "LEFT_PAREN";
    case TokenType::RightParen: return "RIGHT_PAREN";
    case TokenType::Semicolon: return "SEMICOLON";
    case TokenType::EndOfFile: return "EOF";
    }

    return "UNKNOWN";
}
