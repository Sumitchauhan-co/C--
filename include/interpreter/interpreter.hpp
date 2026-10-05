#pragma once

#include "ast/ast.hpp"
#include "runtime/environment.hpp"
#include "runtime/value.hpp"

class Interpreter
{
public:
  void execute(const Program &program);

private:
  void executeStatement(const Stmt &statement);
  Value evaluate(const Expr &expression);

  Environment environment_;
};
