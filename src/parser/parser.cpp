#include "parser/parser.hpp"
#include <stdexcept>
#include <utility>

Parser::Parser(const std::vector<Token> &tokens) : tokens_(tokens) {}

const Token &Parser::peek() const
{
  return tokens_[current_];
}

const Token &Parser::previous() const
{
  return tokens_[current_ - 1];
}

const Token &Parser::advance()
{
  if (!isAtEnd())
  {
    ++current_;
  }
  return previous();
}

bool Parser::isAtEnd() const
{
  return peek().type == TokenType::EndOfFile || peek().type == TokenType::End;
}

bool Parser::check(TokenType type) const
{
  return peek().type == type;
}

bool Parser::match(TokenType type)
{
  if (!check(type))
  {
    return false;
  }
  advance();
  return true;
}

Token Parser::consume(TokenType type, const std::string &message)
{
  if (check(type))
  {
    return advance();
  }
  error(peek(), message);
}

[[noreturn]] void Parser::error(const Token &token, const std::string &message) const
{
  throw std::runtime_error("Parser error at line " + std::to_string(token.line) +
                           ", column " + std::to_string(token.column) + ": " + message +
                           " (near '" + token.lexeme + "')");
}

void Parser::skipSeparators()
{
  while (match(TokenType::Newline) || match(TokenType::Semicolon))
  {
  }
}

std::unique_ptr<BlockStmt> Parser::block()
{
  consume(TokenType::LeftBrace, "Expected '{' to start block.");
  auto result = std::make_unique<BlockStmt>(Program{});
  skipSeparators();
  while (!check(TokenType::RightBrace) && !isAtEnd())
  {
    result->statements.push_back(statement());
    skipSeparators();
  }
  consume(TokenType::RightBrace, "Expected '}' to close block.");
  return result;
}

Program Parser::parse()
{
  Program program;
  skipSeparators();
  while (!isAtEnd())
  {
    program.push_back(statement());
    skipSeparators();
  }
  return program;
}

StmtPtr Parser::statement()
{
  // Handle variable declarations (let / fix)
  if (match(TokenType::Let) || match(TokenType::Fix))
  {
    bool immutable = (previous().type == TokenType::Fix);
    Token nameToken = consume(TokenType::Identifier, "Expected variable name after declaration.");
    consume(TokenType::Equal, "Expected '=' after variable name.");
    ExprPtr initializer = expression();
    return std::make_unique<VariableDeclaration>(nameToken.lexeme, immutable, std::move(initializer));
  }

  // Handle conditional statements (if / elif / else)
  if (match(TokenType::If))
  {
    std::vector<IfBranch> branches;
    ExprPtr condition = expression();
    std::unique_ptr<BlockStmt> body = block();
    branches.push_back({std::move(condition), std::move(body)});
    skipSeparators();

    while (match(TokenType::Elif))
    {
      ExprPtr elifCond = expression();
      std::unique_ptr<BlockStmt> elifBody = block();
      branches.push_back({std::move(elifCond), std::move(elifBody)});
      skipSeparators();
    }

    std::unique_ptr<BlockStmt> otherwise = nullptr;
    if (match(TokenType::Else))
    {
      otherwise = block();
    }
    return std::make_unique<IfStmt>(std::move(branches), std::move(otherwise));
  }

  // Handle assignments
  if (check(TokenType::Identifier) && current_ + 1 < tokens_.size() && tokens_[current_ + 1].type == TokenType::Equal)
  {
    std::string name = advance().lexeme;
    advance(); // consume '='
    auto val = expression();
    return std::make_unique<Assignment>(name, std::move(val));
  }

  // Default to expression statement
  auto expr = expression();
  return std::make_unique<ExpressionStatement>(std::move(expr));
}

// ==========================================
// Expression Precedence Hierarchy
// ==========================================

ExprPtr Parser::expression()
{
  return logicalOr();
}

ExprPtr Parser::logicalOr()
{
  ExprPtr expr = logicalAnd();
  while (match(TokenType::Or))
  {
    ExprPtr right = logicalAnd();
    expr = std::make_unique<BinaryExpr>(std::move(expr), "or", std::move(right));
  }
  return expr;
}

ExprPtr Parser::logicalAnd()
{
  ExprPtr expr = equality();
  while (match(TokenType::And))
  {
    ExprPtr right = equality();
    expr = std::make_unique<BinaryExpr>(std::move(expr), "and", std::move(right));
  }
  return expr;
}

ExprPtr Parser::equality()
{
  ExprPtr expr = comparison();
  while (match(TokenType::EqualEqual) || match(TokenType::BangEqual))
  {
    std::string op = previous().lexeme;
    ExprPtr right = comparison();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::comparison()
{
  ExprPtr expr = term();
  while (match(TokenType::Greater) || match(TokenType::GreaterEqual) ||
         match(TokenType::Less) || match(TokenType::LessEqual))
  {
    std::string op = previous().lexeme;
    ExprPtr right = term();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::term()
{
  ExprPtr expr = factor();
  while (match(TokenType::Plus) || match(TokenType::Minus))
  {
    std::string op = previous().lexeme;
    ExprPtr right = factor();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::factor()
{
  ExprPtr expr = unary();
  while (match(TokenType::Star) || match(TokenType::Slash))
  {
    std::string op = previous().lexeme;
    ExprPtr right = unary();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::unary()
{
  if (match(TokenType::Minus) || match(TokenType::Not))
  {
    std::string op = previous().lexeme;
    return std::make_unique<UnaryExpr>(op, unary());
  }
  return primary();
}

ExprPtr Parser::primary()
{
  if (match(TokenType::Number))
  {
    return std::make_unique<LiteralExpr>(std::stod(previous().lexeme));
  }
  if (match(TokenType::String))
  {
    return std::make_unique<LiteralExpr>(previous().lexeme);
  }
  if (match(TokenType::True))
  {
    return std::make_unique<LiteralExpr>(true);
  }
  if (match(TokenType::False))
  {
    return std::make_unique<LiteralExpr>(false);
  }
  if (match(TokenType::Identifier))
  {
    return std::make_unique<VariableExpr>(previous().lexeme);
  }
  if (match(TokenType::LeftParen))
  {
    ExprPtr expr = expression();
    consume(TokenType::RightParen, "Expected ')' after expression.");
    return expr;
  }
  if (match(TokenType::Say) || match(TokenType::Ask))
  {
    TokenType kind = previous().type;
    consume(TokenType::LeftParen, "Expected '(' after keyword call.");
    std::vector<ExprPtr> args;
    if (!check(TokenType::RightParen))
    {
      args.push_back(expression());
      while (match(TokenType::Comma))
      {
        args.push_back(expression());
      }
    }
    consume(TokenType::RightParen, "Expected ')' after arguments.");
    return std::make_unique<CallExpr>(kind, std::move(args));
  }
  error(peek(), "Expected an expression.");
}