#pragma once

#include "ast/ast.hpp"
#include "lexer/token.hpp"
#include <memory>
#include <string>
#include <vector>

class Parser
{
public:
  explicit Parser(const std::vector<Token> &tokens);
  Program parse();

private:
  StmtPtr statement();
  std::unique_ptr<BlockStmt> block();

  ExprPtr expression();
  ExprPtr logicalOr();
  ExprPtr logicalAnd();
  ExprPtr equality();
  ExprPtr comparison();
  ExprPtr term();
  ExprPtr factor();
  ExprPtr unary();
  ExprPtr postfix(); // Handles index access (e.g., a[0]) and calls
  ExprPtr primary();

  // Template string interpolation helper
  ExprPtr parseTemplateString(const std::string &rawStr);

  const Token &peek() const;
  const Token &previous() const;
  const Token &advance();
  bool isAtEnd() const;
  bool check(TokenType type) const;
  bool match(TokenType type);
  Token consume(TokenType type, const std::string &message);
  [[noreturn]] void error(const Token &token, const std::string &message) const;
  void skipSeparators();

  std::vector<Token> tokens_;
  std::size_t current_ = 0;
};