#pragma once

#include "ast/ast.hpp"
#include "runtime/environment.hpp"
#include "runtime/value.hpp"
#include <vector>
#include <memory>

class Interpreter
{
public:
  Interpreter() : environment_(std::make_shared<Environment>()) {}

  // Run an entire program (list of statements)
  void run(const Program &program);

  // Alternative entry point matching standard execution naming if needed
  void execute(const Program &program) { run(program); }

private:
  std::shared_ptr<Environment> environment_;
  int loopDepth_ = 0; // Tracks nesting level for valid break/continue statements

  // Evaluation of expressions
  Value evaluate(const ExprPtr &expression);

  // Statement execution
  void execute(const StmtPtr &statement);

  // Block and control flow execution
  void executeBlock(const BlockStmt &block);

  // v0.5 Loop and Control Flow Execution
  void executeWhile(const WhileStmt &stmt);
  void executeDoWhile(const DoWhileStmt &stmt);
  void executeFor(const ForStmt &stmt);

  // v0.6 Function Execution helper
  Value callFunction(const FunctionStmt &fnStmt, const std::vector<Value> &arguments);
};