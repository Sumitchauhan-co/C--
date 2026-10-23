#pragma once

#include "ast/ast.hpp"
#include "runtime/environment.hpp"
#include "runtime/value.hpp"
#include <vector>
#include <memory>

class Interpreter
{
public:
  // Run an entire program (list of statements)
  void run(const Program &program);

  // Alternative entry point matching standard execution naming if needed
  void execute(const Program &program) { run(program); }

private:
  Environment environment_;

  // Evaluation of expressions
  Value evaluate(const ExprPtr &expression);

  // Statement execution
  void execute(const StmtPtr &statement);

  // Block and control flow execution
  void executeBlock(const BlockStmt &block);
  void executeIf(const IfStmt &stmt);
};