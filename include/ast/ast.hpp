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
  // For built-in keyword calls (say, ask)
  CallExpr(TokenType kind, std::vector<ExprPtr> arguments)
      : isBuiltin(true), kind(kind), name(""), arguments(std::move(arguments)) {}

  // For user-defined function calls
  CallExpr(std::string name, std::vector<ExprPtr> arguments)
      : isBuiltin(false), kind(TokenType::EndOfFile), name(std::move(name)), arguments(std::move(arguments)) {}

  bool isBuiltin;
  TokenType kind;
  std::string name;
  std::vector<ExprPtr> arguments;
};

// ==========================================
// v0.7 Containers & Indexing Expressions
// ==========================================

struct ArrayExpr final : Expr
{
  explicit ArrayExpr(std::vector<ExprPtr> elements) : elements(std::move(elements)) {}
  std::vector<ExprPtr> elements;
};

struct ObjectExpr final : Expr
{
  explicit ObjectExpr(std::vector<std::pair<ExprPtr, ExprPtr>> pairs) : pairs(std::move(pairs)) {}
  std::vector<std::pair<ExprPtr, ExprPtr>> pairs;
};

struct IndexExpr final : Expr
{
  IndexExpr(ExprPtr objectOrArray, ExprPtr index)
      : objectOrArray(std::move(objectOrArray)), index(std::move(index)) {}

  ExprPtr objectOrArray;
  ExprPtr index;
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

struct ForInStmt final : Stmt
{
  ForInStmt(std::string variable, ExprPtr collection, std::unique_ptr<BlockStmt> body)
      : variable(std::move(variable)), collection(std::move(collection)), body(std::move(body)) {}

  std::string variable;
  ExprPtr collection;
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

// ==========================================
// v0.6 Functions & Exception Handling
// ==========================================

struct FunctionStmt final : Stmt
{
  FunctionStmt(std::string name, std::vector<std::string> parameters, std::unique_ptr<BlockStmt> body)
      : name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)) {}

  std::string name;
  std::vector<std::string> parameters;
  std::unique_ptr<BlockStmt> body;
};

struct ReturnStmt final : Stmt
{
  explicit ReturnStmt(ExprPtr value = nullptr) : value(std::move(value)) {}
  ExprPtr value;
};

struct TryStmt final : Stmt
{
  TryStmt(std::unique_ptr<BlockStmt> tryBody, std::string catchVar, std::unique_ptr<BlockStmt> catchBody)
      : tryBody(std::move(tryBody)), catchVar(std::move(catchVar)), catchBody(std::move(catchBody)) {}

  std::unique_ptr<BlockStmt> tryBody;
  std::string catchVar;
  std::unique_ptr<BlockStmt> catchBody;
};

struct ThrowStmt final : Stmt
{
  explicit ThrowStmt(ExprPtr expression) : expression(std::move(expression)) {}
  ExprPtr expression;
};