#pragma once

#include "runtime/value.hpp"
#include "lexer/token.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Forward declarations
struct Stmt;
using StmtPtr = std::unique_ptr<Stmt>;
using Program = std::vector<StmtPtr>;

// ==========================================
// Expressions (Expr)
// ==========================================

struct Expr
{
  virtual ~Expr() = default;
};

using ExprPtr = std::unique_ptr<Expr>;

struct LiteralExpr final : Expr
{
  explicit LiteralExpr(Value value) : value(std::move(value)) {}
  Value value;
};

struct VariableExpr final : Expr
{
  explicit VariableExpr(std::string name) : name(std::move(name)) {}
  std::string name;
};

struct BinaryExpr final : Expr
{
  BinaryExpr(ExprPtr left, std::string op, ExprPtr right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

  ExprPtr left;
  std::string op;
  ExprPtr right;
};

struct UnaryExpr final : Expr
{
  UnaryExpr(std::string op, ExprPtr operand)
      : op(std::move(op)), operand(std::move(operand)) {}

  std::string op;
  ExprPtr operand;
};

// Ask input syntax (e.g., ask "Enter your name: ")
struct AskExpr final : Expr
{
  explicit AskExpr(ExprPtr prompt = nullptr) : prompt(std::move(prompt)) {}
  ExprPtr prompt;
};

struct CallExpr final : Expr
{
  CallExpr(TokenType kind, std::vector<ExprPtr> arguments)
      : kind(kind), arguments(std::move(arguments)) {}

  TokenType kind;
  std::vector<ExprPtr> arguments;
};

// ==========================================
// Statements (Stmt)
// ==========================================

struct Stmt
{
  virtual ~Stmt() = default;
};

struct BlockStmt final : Stmt
{
  explicit BlockStmt(Program statements) : statements(std::move(statements)) {}
  Program statements;
};

struct VariableDeclaration final : Stmt
{
  VariableDeclaration(std::string name, bool immutable, ExprPtr initializer)
      : name(std::move(name)), immutable(immutable), initializer(std::move(initializer)) {}

  std::string name;
  bool immutable;
  ExprPtr initializer;
};

struct Assignment final : Stmt
{
  Assignment(std::string name, ExprPtr value)
      : name(std::move(name)), value(std::move(value)) {}

  std::string name;
  ExprPtr value;
};

struct IfBranch
{
  ExprPtr condition;
  std::unique_ptr<BlockStmt> body;
};

struct IfStmt final : Stmt
{
  IfStmt(std::vector<IfBranch> branches, std::unique_ptr<BlockStmt> otherwiseBranch)
      : branches(std::move(branches)), otherwiseBranch(std::move(otherwiseBranch)) {}

  std::vector<IfBranch> branches;
  std::unique_ptr<BlockStmt> otherwiseBranch;
};

// ==========================================
// v0.5 Loop & Control Flow Statements
// ==========================================

struct WhileStmt final : Stmt
{
  WhileStmt(ExprPtr condition, std::unique_ptr<BlockStmt> body)
      : condition(std::move(condition)), body(std::move(body)) {}

  ExprPtr condition;
  std::unique_ptr<BlockStmt> body;
};

struct DoWhileStmt final : Stmt
{
  DoWhileStmt(std::unique_ptr<BlockStmt> body, ExprPtr condition)
      : body(std::move(body)), condition(std::move(condition)) {}

  std::unique_ptr<BlockStmt> body;
  ExprPtr condition;
};

struct ForStmt final : Stmt
{
  ForStmt(StmtPtr initializer, ExprPtr condition, StmtPtr update, std::unique_ptr<BlockStmt> body)
      : initializer(std::move(initializer)), condition(std::move(condition)),
        update(std::move(update)), body(std::move(body)) {}

  StmtPtr initializer;
  ExprPtr condition;
  StmtPtr update;
  std::unique_ptr<BlockStmt> body;
};

struct BreakStmt final : Stmt
{
};

struct ContinueStmt final : Stmt
{
};

struct PassStmt final : Stmt
{
};

struct SayStatement final : Stmt
{
  explicit SayStatement(ExprPtr expression) : expression(std::move(expression)) {}
  ExprPtr expression;
};

struct ExpressionStatement final : Stmt
{
  explicit ExpressionStatement(ExprPtr expression) : expression(std::move(expression)) {}
  ExprPtr expression;
};