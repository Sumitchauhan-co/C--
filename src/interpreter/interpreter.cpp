#include "interpreter/interpreter.hpp"

#include <iostream>
#include <stdexcept>

void Interpreter::execute(const Program& program) {
    for (const auto& statement : program) {
        executeStatement(*statement);
    }
}

void Interpreter::executeStatement(const Stmt& statement) {
    if (const auto* declaration = dynamic_cast<const VariableDeclaration*>(&statement)) {
        const long long value = evaluate(*declaration->initializer);
        environment_.define(declaration->name, value);
        return;
    }

    if (const auto* assignment = dynamic_cast<const Assignment*>(&statement)) {
        const long long value = evaluate(*assignment->value);
        environment_.assign(assignment->name, value);
        return;
    }

    if (const auto* say = dynamic_cast<const SayStatement*>(&statement)) {
        std::cout << evaluate(*say->expression) << '\n';
        return;
    }

    if (const auto* expression = dynamic_cast<const ExpressionStatement*>(&statement)) {
        static_cast<void>(evaluate(*expression->expression));
        return;
    }

    throw std::runtime_error("Runtime error: unknown statement");
}

long long Interpreter::evaluate(const Expr& expression) {
    if (const auto* number = dynamic_cast<const NumberExpr*>(&expression)) {
        return number->value;
    }

    if (const auto* variable = dynamic_cast<const VariableExpr*>(&expression)) {
        return environment_.get(variable->name);
    }

    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
        const long long value = evaluate(*unary->operand);

        if (unary->op == "-") {
            return -value;
        }

        throw std::runtime_error("Runtime error: unknown unary operator");
    }

    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
        const long long left = evaluate(*binary->left);
        const long long right = evaluate(*binary->right);

        if (binary->op == "+") return left + right;
        if (binary->op == "-") return left - right;
        if (binary->op == "*") return left * right;

        if (binary->op == "/") {
            if (right == 0) {
                throw std::runtime_error("Runtime error: division by zero");
            }
            return left / right;
        }

        throw std::runtime_error("Runtime error: unknown binary operator");
    }

    throw std::runtime_error("Runtime error: unknown expression");
}
