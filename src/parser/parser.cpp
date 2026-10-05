#include "parser/parser.hpp"
#include <stdexcept>
#include <string>

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
  return peek().type == TokenType::EndOfFile;
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

const Token &Parser::consume(TokenType type, const char *message)
{
  if (check(type))
  {
    return advance();
  }
  error(peek(), message);
}

[[noreturn]] void Parser::error(const Token &token, const char *message) const
{
  throw std::runtime_error("Parser error at line " + std::to_string(token.line) +
                           ", column " + std::to_string(token.column) + ": " + message);
}

Program Parser::parse()
{
  Program p;
  while (!isAtEnd())
  {
    p.push_back(statement());
  }
  return p;
}

StmtPtr Parser::statement()
{
  if (match(TokenType::Let))
  {
    return letStatement();
  }
  if (match(TokenType::Say))
  {
    return sayStatement();
  }
  return expressionStatement();
}

StmtPtr Parser::letStatement()
{
  const Token &name = consume(TokenType::Identifier, "expected variable name after 'let'.");
  consume(TokenType::Equal, "expected '=' after variable name.");

  auto init = expression();
  consume(TokenType::Semicolon, "expected ';' after variable declaration.");

  return std::make_unique<VariableDeclaration>(name.lexeme, std::move(init));
}

StmtPtr Parser::sayStatement()
{
  auto e = expression();
  consume(TokenType::Semicolon, "expected ';' after 'say' expression.");
  return std::make_unique<SayStatement>(std::move(e));
}

StmtPtr Parser::expressionStatement()
{
  if (check(TokenType::Identifier) && current_ + 1 < tokens_.size() && tokens_[current_ + 1].type == TokenType::Equal)
  {
    auto name = advance().lexeme;
    advance(); // consume '='

    auto v = expression();
    consume(TokenType::Semicolon, "expected ';' after assignment.");
    return std::make_unique<Assignment>(name, std::move(v));
  }

  auto e = expression();
  consume(TokenType::Semicolon, "expected ';' after expression.");
  return std::make_unique<ExpressionStatement>(std::move(e));
}

ExprPtr Parser::expression()
{
  return equality();
}

ExprPtr Parser::equality()
{
  auto e = comparison();

  while (match(TokenType::EqualEqual) || match(TokenType::BangEqual))
  {
    auto op = previous().lexeme;
    auto r = comparison();
    e = std::make_unique<BinaryExpr>(std::move(e), op, std::move(r));
  }
  return e;
}

ExprPtr Parser::comparison()
{
  auto e = term();

  while (match(TokenType::Greater) || match(TokenType::GreaterEqual) ||
         match(TokenType::Less) || match(TokenType::LessEqual))
  {
    auto op = previous().lexeme;
    auto r = term();
    e = std::make_unique<BinaryExpr>(std::move(e), op, std::move(r));
  }
  return e;
}

ExprPtr Parser::term()
{
  auto e = factor();

  while (match(TokenType::Plus) || match(TokenType::Minus))
  {
    auto op = previous().lexeme;
    auto r = factor();
    e = std::make_unique<BinaryExpr>(std::move(e), op, std::move(r));
  }
  return e;
}

ExprPtr Parser::factor()
{
  auto e = unary();

  while (match(TokenType::Star) || match(TokenType::Slash))
  {
    auto op = previous().lexeme;
    auto r = unary();
    e = std::make_unique<BinaryExpr>(std::move(e), op, std::move(r));
  }
  return e;
}

ExprPtr Parser::unary()
{
  if (match(TokenType::Minus))
  {
    return std::make_unique<UnaryExpr>("-", unary());
  }
  return primary();
}

ExprPtr Parser::primary()
{
  if (match(TokenType::Number))
  {
    return std::make_unique<NumberExpr>(std::stoll(previous().lexeme));
  }
  if (match(TokenType::True))
  {
    return std::make_unique<BooleanExpr>(true);
  }
  if (match(TokenType::False))
  {
    return std::make_unique<BooleanExpr>(false);
  }
  if (match(TokenType::Identifier))
  {
    return std::make_unique<VariableExpr>(previous().lexeme);
  }
  if (match(TokenType::LeftParen))
  {
    auto e = expression();
    consume(TokenType::RightParen, "expected ')' after expression.");
    return e;
  }
  error(peek(), "expected expression.");
}
