#pragma once

#include "ast/ast.hpp"
#include "runtime/environment.hpp"

class Interpreter {
public:
    void execute(const Program& program);

private:
    void executeStatement(const Stmt& statement);
    long long evaluate(const Expr& expression);

    Environment environment_;
};
