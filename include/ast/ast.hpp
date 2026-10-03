#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Expr {
    virtual ~Expr() = default;
};

using ExprPtr = std::unique_ptr<Expr>;

struct NumberExpr final : Expr {
    explicit NumberExpr(long long value) : value(value) {}
    long long value;
};

struct VariableExpr final : Expr {
    explicit VariableExpr(std::string name) : name(std::move(name)) {}
    std::string name;
};

struct BinaryExpr final : Expr {
    BinaryExpr(ExprPtr left, std::string op, ExprPtr right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    ExprPtr left;
    std::string op;
    ExprPtr right;
};

struct UnaryExpr final : Expr {
    UnaryExpr(std::string op, ExprPtr operand)
        : op(std::move(op)), operand(std::move(operand)) {}

    std::string op;
    ExprPtr operand;
};

struct Stmt {
    virtual ~Stmt() = default;
};

using StmtPtr = std::unique_ptr<Stmt>;

struct VariableDeclaration final : Stmt {
    VariableDeclaration(std::string name, ExprPtr initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    std::string name;
    ExprPtr initializer;
};

struct Assignment final : Stmt {
    Assignment(std::string name, ExprPtr value)
        : name(std::move(name)), value(std::move(value)) {}

    std::string name;
    ExprPtr value;
};

struct SayStatement final : Stmt {
    explicit SayStatement(ExprPtr expression)
        : expression(std::move(expression)) {}

    ExprPtr expression;
};

struct ExpressionStatement final : Stmt {
    explicit ExpressionStatement(ExprPtr expression)
        : expression(std::move(expression)) {}

    ExprPtr expression;
};

using Program = std::vector<StmtPtr>;
