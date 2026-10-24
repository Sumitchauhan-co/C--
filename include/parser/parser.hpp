#pragma once

#include "ast/ast.hpp"
#include "lexer/token.hpp"
#include <cstddef>
#include <memory>
#include <string>
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
  Token consume(TokenType type, const std::string &message);
  void skipSeparators();

  // Statements and Blocks
  StmtPtr statement();
  StmtPtr variableDeclaration(); // Handles 'let' and 'fix' declarations
  StmtPtr sayStatement();
  StmtPtr ifStatement(); // Handles 'if / elif / else' branches
  StmtPtr whileStatement();
  StmtPtr doWhileStatement();
  StmtPtr forStatement();
  StmtPtr breakStatement();
  StmtPtr continueStatement();
  StmtPtr passStatement();
  StmtPtr expressionStatement();
  std::unique_ptr<BlockStmt> block();

  // Expressions (Precedence hierarchy from lowest to highest)
  ExprPtr expression();
  ExprPtr logicalOr();
  ExprPtr logicalAnd();
  ExprPtr equality();
  ExprPtr comparison();
  ExprPtr term();
  ExprPtr factor();
  ExprPtr unary();
  ExprPtr primary();

  // Error handling
  [[noreturn]] void error(const Token &token, const std::string &message) const;

  // Member variables
  const std::vector<Token> &tokens_;
  std::size_t current_ = 0;
};