#include "parser/parser.hpp"

#include <stdexcept>
#include <string>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens) {}

const Token& Parser::peek() const {
    return tokens_[current_];
}

const Token& Parser::previous() const {
    return tokens_[current_ - 1];
}

const Token& Parser::advance() {
    if (!isAtEnd()) {
        ++current_;
    }
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

bool Parser::check(TokenType type) const {
    return !isAtEnd() && peek().type == type;
}

bool Parser::match(TokenType type) {
    if (!check(type)) {
        return false;
    }
    advance();
    return true;
}

const Token& Parser::consume(TokenType type, const char* message) {
    if (check(type)) {
        return advance();
    }
    error(peek(), message);
}

[[noreturn]] void Parser::error(const Token& token, const char* message) const {
    throw std::runtime_error(
        "Parser error at line " + std::to_string(token.line) +
        ", column " + std::to_string(token.column) +
        ": " + message
    );
}

Program Parser::parse() {
    Program program;

    while (!isAtEnd()) {
        program.push_back(statement());
    }

    return program;
}

StmtPtr Parser::statement() {
    if (match(TokenType::Let)) {
        return letStatement();
    }

    if (match(TokenType::Say)) {
        return sayStatement();
    }

    if (check(TokenType::Identifier) &&
        current_ + 1 < tokens_.size() &&
        tokens_[current_ + 1].type == TokenType::Equal) {
        const std::string name = advance().lexeme;
        advance(); // '='

        auto value = expression();
        consume(TokenType::Semicolon, "expected ';' after assignment");
        return std::make_unique<Assignment>(name, std::move(value));
    }

    return expressionStatement();
}

StmtPtr Parser::letStatement() {
    const Token& name = consume(TokenType::Identifier, "expected variable name after 'let'");
    consume(TokenType::Equal, "expected '=' after variable name");

    auto initializer = expression();
    consume(TokenType::Semicolon, "expected ';' after variable declaration");

    return std::make_unique<VariableDeclaration>(
        name.lexeme,
        std::move(initializer)
    );
}

StmtPtr Parser::sayStatement() {
    auto expressionValue = expression();
    consume(TokenType::Semicolon, "expected ';' after 'say' expression");

    return std::make_unique<SayStatement>(std::move(expressionValue));
}

StmtPtr Parser::expressionStatement() {
    auto expressionValue = expression();
    consume(TokenType::Semicolon, "expected ';' after expression");

    return std::make_unique<ExpressionStatement>(std::move(expressionValue));
}

ExprPtr Parser::expression() {
    return term();
}

ExprPtr Parser::term() {
    auto left = factor();

    while (check(TokenType::Plus) || check(TokenType::Minus)) {
        const std::string op = advance().lexeme;
        auto right = factor();
        left = std::make_unique<BinaryExpr>(
            std::move(left),
            op,
            std::move(right)
        );
    }

    return left;
}

ExprPtr Parser::factor() {
    auto left = unary();

    while (check(TokenType::Star) || check(TokenType::Slash)) {
        const std::string op = advance().lexeme;
        auto right = unary();
        left = std::make_unique<BinaryExpr>(
            std::move(left),
            op,
            std::move(right)
        );
    }

    return left;
}

ExprPtr Parser::unary() {
    if (match(TokenType::Minus)) {
        return std::make_unique<UnaryExpr>("-", unary());
    }

    return primary();
}

ExprPtr Parser::primary() {
    if (match(TokenType::Number)) {
        return std::make_unique<NumberExpr>(
            std::stoll(previous().lexeme)
        );
    }

    if (match(TokenType::Identifier)) {
        return std::make_unique<VariableExpr>(previous().lexeme);
    }

    if (match(TokenType::LeftParen)) {
        auto value = expression();
        consume(TokenType::RightParen, "expected ')' after expression");
        return value;
    }

    error(peek(), "expected expression");
}
