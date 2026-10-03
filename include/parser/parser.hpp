#pragma once

#include "ast/ast.hpp"
#include "lexer/token.hpp"

#include <cstddef>
#include <vector>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    Program parse();

private:
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token& consume(TokenType type, const char* message);

    StmtPtr statement();
    StmtPtr letStatement();
    StmtPtr sayStatement();
    StmtPtr expressionStatement();

    ExprPtr expression();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();

    [[noreturn]] void error(const Token& token, const char* message) const;

    const std::vector<Token>& tokens_;
    std::size_t current_ = 0;
};
