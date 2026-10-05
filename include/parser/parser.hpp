#pragma once

#include "ast/ast.hpp"
#include "lexer/token.hpp"
#include <cstddef>
#include <vector>

class Parser
{
public:
  explicit Parser(const std::vector<Token> &tokens);

  Program parse();

private:
  // Navigation helpers
  const Token &peek() const;
  const Token &previous() const;
  const Token &advance();
  bool isAtEnd() const;

  // Type checking and matching
  bool check(TokenType type) const;
  bool match(TokenType type);
  const Token &consume(TokenType type, const char *message);

  // Statements
  StmtPtr statement();
  StmtPtr letStatement();
  StmtPtr sayStatement();
  StmtPtr expressionStatement();

  // Expressions (Precedence hierarchy)
  ExprPtr expression();
  ExprPtr equality();
  ExprPtr comparison();
  ExprPtr term();
  ExprPtr factor();
  ExprPtr unary();
  ExprPtr primary();

  // Error handling
  [[noreturn]] void error(const Token &token, const char *message) const;

  // Member variables
  const std::vector<Token> &tokens_;
  std::size_t current_ = 0;
};
