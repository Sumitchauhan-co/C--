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
  if (match(TokenType::Let) || match(TokenType::Fix))
  {
    bool immutable = (previous().type == TokenType::Fix);
    Token nameToken = consume(TokenType::Identifier, "Expected variable name after declaration.");
    consume(TokenType::Equal, "Expected '=' after variable name.");
    ExprPtr initializer = expression();
    return std::make_unique<VariableDeclaration>(nameToken.lexeme, immutable, std::move(initializer));
  }

  if (match(TokenType::Fn))
  {
    Token nameToken = consume(TokenType::Identifier, "Expected function name after 'fn'.");
    consume(TokenType::LeftParen, "Expected '(' after function name.");
    std::vector<std::string> parameters;
    if (!check(TokenType::RightParen))
    {
      parameters.push_back(consume(TokenType::Identifier, "Expected parameter name.").lexeme);
      while (match(TokenType::Comma))
      {
        parameters.push_back(consume(TokenType::Identifier, "Expected parameter name.").lexeme);
      }
    }
    consume(TokenType::RightParen, "Expected ')' after parameters.");
    std::unique_ptr<BlockStmt> body = block();
    return std::make_unique<FunctionStmt>(nameToken.lexeme, std::move(parameters), std::move(body));
  }

  if (match(TokenType::Return))
  {
    ExprPtr value = nullptr;
    if (!check(TokenType::Newline) && !check(TokenType::Semicolon) && !check(TokenType::RightBrace) && !isAtEnd())
    {
      value = expression();
    }
    return std::make_unique<ReturnStmt>(std::move(value));
  }

  if (match(TokenType::Try))
  {
    std::unique_ptr<BlockStmt> tryBody = block();
    skipSeparators();
    consume(TokenType::Catch, "Expected 'catch' after 'try' block.");
    consume(TokenType::LeftParen, "Expected '(' after 'catch'.");
    Token catchVarToken = consume(TokenType::Identifier, "Expected exception variable name in catch clause.");
    consume(TokenType::RightParen, "Expected ')' after catch variable.");
    std::unique_ptr<BlockStmt> catchBody = block();
    return std::make_unique<TryStmt>(std::move(tryBody), catchVarToken.lexeme, std::move(catchBody));
  }

  if (match(TokenType::Throw))
  {
    ExprPtr value = expression();
    return std::make_unique<ThrowStmt>(std::move(value));
  }

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
    skipSeparators();
    if (match(TokenType::Else))
    {
      otherwise = block();
    }
    return std::make_unique<IfStmt>(std::move(branches), std::move(otherwise));
  }

  if (match(TokenType::While))
  {
    ExprPtr condition = expression();
    std::unique_ptr<BlockStmt> body = block();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
  }

  if (match(TokenType::Do))
  {
    std::unique_ptr<BlockStmt> body = block();
    consume(TokenType::While, "Expected 'while' after do-while block.");
    ExprPtr condition = expression();
    return std::make_unique<DoWhileStmt>(std::move(body), std::move(condition));
  }

  // Handle for-in loops: for x in collection { ... }
  if (match(TokenType::For))
  {
    if (check(TokenType::Identifier))
    {
      std::string varName = advance().lexeme;
      if (check(TokenType::Identifier) && peek().lexeme == "in")
      {
        advance(); // consume 'in'
        ExprPtr collection = expression();
        std::unique_ptr<BlockStmt> body = block();
        return std::make_unique<ForInStmt>(varName, std::move(collection), std::move(body));
      }
      // If not 'in', backtrack current_ to handle standard C-style for loop
      current_--;
    }

    consume(TokenType::LeftParen, "Expected '(' after 'for'.");
    StmtPtr initializer = nullptr;
    if (!match(TokenType::Newline) && !match(TokenType::Semicolon))
    {
      if (match(TokenType::Let) || match(TokenType::Fix))
      {
        bool immutable = (previous().type == TokenType::Fix);
        Token nameToken = consume(TokenType::Identifier, "Expected variable name in for-loop initializer.");
        consume(TokenType::Equal, "Expected '=' after variable name.");
        ExprPtr initVal = expression();
        initializer = std::make_unique<VariableDeclaration>(nameToken.lexeme, immutable, std::move(initVal));
      }
      else
      {
        auto expr = expression();
        initializer = std::make_unique<ExpressionStatement>(std::move(expr));
      }
      match(TokenType::Newline);
      match(TokenType::Semicolon);
    }

    ExprPtr condition = nullptr;
    if (!check(TokenType::Newline) && !check(TokenType::Semicolon) && !check(TokenType::RightParen))
    {
      condition = expression();
    }
    match(TokenType::Newline);
    match(TokenType::Semicolon);

    StmtPtr update = nullptr;
    if (!check(TokenType::RightParen))
    {
      if (check(TokenType::Identifier) && current_ + 1 < tokens_.size() &&
          (tokens_[current_ + 1].type == TokenType::PlusPlus || tokens_[current_ + 1].type == TokenType::MinusMinus))
      {
        std::string varName = advance().lexeme;
        TokenType opType = advance().type;
        std::string opStr = (opType == TokenType::PlusPlus) ? "+" : "-";
        auto one = std::make_unique<LiteralExpr>(1.0);
        auto bin = std::make_unique<BinaryExpr>(std::make_unique<VariableExpr>(varName), opStr, std::move(one));
        update = std::make_unique<Assignment>(varName, std::move(bin));
      }
      else if (check(TokenType::Identifier) && current_ + 1 < tokens_.size() && tokens_[current_ + 1].type == TokenType::Equal)
      {
        std::string name = advance().lexeme;
        advance();
        auto val = expression();
        update = std::make_unique<Assignment>(name, std::move(val));
      }
      else
      {
        auto expr = expression();
        update = std::make_unique<ExpressionStatement>(std::move(expr));
      }
    }

    consume(TokenType::RightParen, "Expected ')' after for-loop clauses.");
    std::unique_ptr<BlockStmt> body = block();
    return std::make_unique<ForStmt>(std::move(initializer), std::move(condition), std::move(update), std::move(body));
  }

  if (match(TokenType::Break))
  {
    return std::make_unique<BreakStmt>();
  }

  if (match(TokenType::Continue))
  {
    return std::make_unique<ContinueStmt>();
  }

  if (match(TokenType::Pass))
  {
    return std::make_unique<PassStmt>();
  }

  if (check(TokenType::Identifier) && current_ + 1 < tokens_.size())
  {
    if (tokens_[current_ + 1].type == TokenType::Equal)
    {
      std::string name = advance().lexeme;
      advance();
      auto val = expression();
      return std::make_unique<Assignment>(name, std::move(val));
    }
    if (tokens_[current_ + 1].type == TokenType::PlusPlus || tokens_[current_ + 1].type == TokenType::MinusMinus)
    {
      std::string name = advance().lexeme;
      TokenType opType = advance().type;
      std::string opStr = (opType == TokenType::PlusPlus) ? "+" : "-";
      auto one = std::make_unique<LiteralExpr>(1.0);
      auto bin = std::make_unique<BinaryExpr>(std::make_unique<VariableExpr>(name), opStr, std::move(one));
      return std::make_unique<Assignment>(name, std::move(bin));
    }
  }

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
  return postfix();
}

ExprPtr Parser::postfix()
{
  ExprPtr expr = primary();
  while (true)
  {
    if (match(TokenType::LeftBracket))
    {
      ExprPtr index = expression();
      consume(TokenType::RightBracket, "Expected ']' after index.");
      expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index));
    }
    else if (match(TokenType::LeftParen))
    {
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
      if (auto varExpr = dynamic_cast<VariableExpr *>(expr.get()))
      {
        expr = std::make_unique<CallExpr>(varExpr->name, std::move(args));
      }
      else
      {
        throw std::runtime_error("Parser error: Invalid callee.");
      }
    }
    else
    {
      break;
    }
  }
  return expr;
}

ExprPtr Parser::parseTemplateString(const std::string &rawStr)
{
  size_t i = 0;
  ExprPtr result = nullptr;
  std::string currentLit = "";

  while (i < rawStr.size())
  {
    if (rawStr[i] == '{')
    {
      if (!currentLit.empty())
      {
        ExprPtr litExpr = std::make_unique<LiteralExpr>(currentLit);
        result = result ? std::make_unique<BinaryExpr>(std::move(result), "+", std::move(litExpr)) : std::move(litExpr);
        currentLit.clear();
      }
      size_t start = ++i;
      while (i < rawStr.size() && rawStr[i] != '}')
        i++;
      std::string varName = rawStr.substr(start, i - start);
      if (i < rawStr.size())
        i++;

      ExprPtr varExpr = std::make_unique<VariableExpr>(varName);
      result = result ? std::make_unique<BinaryExpr>(std::move(result), "+", std::move(varExpr)) : std::move(varExpr);
    }
    else
    {
      currentLit += rawStr[i++];
    }
  }

  if (!currentLit.empty() || !result)
  {
    ExprPtr litExpr = std::make_unique<LiteralExpr>(currentLit);
    result = result ? std::make_unique<BinaryExpr>(std::move(result), "+", std::move(litExpr)) : std::move(litExpr);
  }

  return result;
}

ExprPtr Parser::primary()
{
  if (match(TokenType::Number))
  {
    return std::make_unique<LiteralExpr>(std::stod(previous().lexeme));
  }
  if (match(TokenType::TemplateString))
  {
    return parseTemplateString(previous().lexeme);
  }
  if (match(TokenType::String))
  {
    std::string val = previous().lexeme;
    return std::make_unique<LiteralExpr>(val);
  }
  if (match(TokenType::True))
  {
    return std::make_unique<LiteralExpr>(true);
  }
  if (match(TokenType::False))
  {
    return std::make_unique<LiteralExpr>(false);
  }

  // Array literal: [1, 2, 4] with newline/separator skipping
  if (match(TokenType::LeftBracket))
  {
    std::vector<ExprPtr> elements;
    skipSeparators();
    if (!check(TokenType::RightBracket))
    {
      elements.push_back(expression());
      skipSeparators();
      while (match(TokenType::Comma))
      {
        skipSeparators();
        if (check(TokenType::RightBracket))
          break;
        elements.push_back(expression());
        skipSeparators();
      }
    }
    consume(TokenType::RightBracket, "Expected ']' after array elements.");
    return std::make_unique<ArrayExpr>(std::move(elements));
  }

  // Object / Map literal: {"name": 1} with newline/separator skipping
  if (match(TokenType::LeftBrace))
  {
    std::vector<std::pair<ExprPtr, ExprPtr>> pairs;
    skipSeparators();
    if (!check(TokenType::RightBrace))
    {
      do
      {
        skipSeparators();
        if (check(TokenType::RightBrace))
          break;
        ExprPtr key = expression();
        skipSeparators();
        consume(TokenType::Colon, "Expected ':' after key in object literal.");
        skipSeparators();
        ExprPtr val = expression();
        skipSeparators();
        pairs.push_back({std::move(key), std::move(val)});
        skipSeparators();
      } while (match(TokenType::Comma));
    }
    consume(TokenType::RightBrace, "Expected '}' after object literal.");
    return std::make_unique<ObjectExpr>(std::move(pairs));
  }

  if (match(TokenType::Identifier))
  {
    std::string name = previous().lexeme;
    return std::make_unique<VariableExpr>(name);
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