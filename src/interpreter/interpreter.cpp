#include "interpreter/interpreter.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

// ==========================================
// Control Flow Signals
// ==========================================
struct BreakSignal
{
};
struct ContinueSignal
{
};

// ==========================================
// Program & Statement Execution
// ==========================================

void Interpreter::run(const Program &program)
{
    for (const auto &statement : program)
    {
        execute(statement);
    }
}

void Interpreter::execute(const StmtPtr &statement)
{
    if (!statement)
        return;

    if (auto node = dynamic_cast<VariableDeclaration *>(statement.get()))
    {
        Value initVal = node->initializer ? evaluate(node->initializer) : 0.0;
        environment_->declare(node->name, initVal, node->immutable);
        return;
    }

    if (auto node = dynamic_cast<Assignment *>(statement.get()))
    {
        environment_->assign(node->name, evaluate(node->value));
        return;
    }

    if (auto node = dynamic_cast<SayStatement *>(statement.get()))
    {
        std::cout << valueToString(evaluate(node->expression)) << '\n';
        return;
    }

    if (auto node = dynamic_cast<ExpressionStatement *>(statement.get()))
    {
        evaluate(node->expression);
        return;
    }

    if (auto node = dynamic_cast<IfStmt *>(statement.get()))
    {
        for (const auto &branch : node->branches)
        {
            if (requireBoolean(evaluate(branch.condition)))
            {
                executeBlock(*branch.body);
                return;
            }
        }
        if (node->otherwiseBranch)
        {
            executeBlock(*node->otherwiseBranch);
        }
        return;
    }

    if (auto node = dynamic_cast<WhileStmt *>(statement.get()))
    {
        executeWhile(*node);
        return;
    }

    if (auto node = dynamic_cast<DoWhileStmt *>(statement.get()))
    {
        executeDoWhile(*node);
        return;
    }

    if (auto node = dynamic_cast<ForStmt *>(statement.get()))
    {
        executeFor(*node);
        return;
    }

    if (dynamic_cast<BreakStmt *>(statement.get()))
    {
        if (loopDepth_ <= 0)
        {
            throw std::runtime_error("Runtime error: 'break' outside of loop.");
        }
        throw BreakSignal{};
    }

    if (dynamic_cast<ContinueStmt *>(statement.get()))
    {
        if (loopDepth_ <= 0)
        {
            throw std::runtime_error("Runtime error: 'continue' outside of loop.");
        }
        throw ContinueSignal{};
    }

    if (dynamic_cast<PassStmt *>(statement.get()))
    {
        return; // pass does nothing
    }

    throw std::runtime_error("Runtime error: unknown statement type.");
}

void Interpreter::executeBlock(const BlockStmt &block)
{
    auto oldEnv = environment_;
    environment_ = std::make_shared<Environment>(oldEnv);

    try
    {
        for (const auto &stmt : block.statements)
        {
            execute(stmt);
        }
        environment_ = oldEnv;
    }
    catch (...)
    {
        environment_ = oldEnv;
        throw;
    }
}

// ==========================================
// v0.5 Loop Execution Handlers
// ==========================================

void Interpreter::executeWhile(const WhileStmt &stmt)
{
    ++loopDepth_;
    try
    {
        while (requireBoolean(evaluate(stmt.condition)))
        {
            try
            {
                executeBlock(*stmt.body);
            }
            catch (const ContinueSignal &)
            {
                continue;
            }
            catch (const BreakSignal &)
            {
                break;
            }
        }
        --loopDepth_;
    }
    catch (...)
    {
        --loopDepth_;
        throw;
    }
}

void Interpreter::executeDoWhile(const DoWhileStmt &stmt)
{
    ++loopDepth_;
    try
    {
        do
        {
            try
            {
                executeBlock(*stmt.body);
            }
            catch (const ContinueSignal &)
            {
                // Continue goes to condition check
            }
            catch (const BreakSignal &)
            {
                break;
            }
        } while (requireBoolean(evaluate(stmt.condition)));
        --loopDepth_;
    }
    catch (...)
    {
        --loopDepth_;
        throw;
    }
}

void Interpreter::executeFor(const ForStmt &stmt)
{
    auto oldEnv = environment_;
    environment_ = std::make_shared<Environment>(oldEnv);

    ++loopDepth_;
    try
    {
        if (stmt.initializer)
        {
            execute(stmt.initializer);
        }

        while (true)
        {
            if (stmt.condition)
            {
                if (!requireBoolean(evaluate(stmt.condition)))
                {
                    break;
                }
            }

            try
            {
                executeBlock(*stmt.body);
            }
            catch (const ContinueSignal &)
            {
                if (stmt.update)
                {
                    execute(stmt.update);
                }
                continue;
            }
            catch (const BreakSignal &)
            {
                break;
            }

            if (stmt.update)
            {
                execute(stmt.update);
            }
        }

        environment_ = oldEnv;
        --loopDepth_;
    }
    catch (...)
    {
        environment_ = oldEnv;
        --loopDepth_;
        throw;
    }
}

// ==========================================
// Expression Evaluation
// ==========================================

Value Interpreter::evaluate(const ExprPtr &expression)
{
    if (!expression)
        throw std::runtime_error("Runtime error: null expression encountered.");

    if (auto node = dynamic_cast<LiteralExpr *>(expression.get()))
    {
        return node->value;
    }

    if (auto node = dynamic_cast<VariableExpr *>(expression.get()))
    {
        return environment_->get(node->name);
    }

    // Handle CallExpr (e.g. say(...) and ask(...))
    if (auto node = dynamic_cast<CallExpr *>(expression.get()))
    {
        if (node->kind == TokenType::Say)
        {
            for (std::size_t i = 0; i < node->arguments.size(); ++i)
            {
                if (i)
                    std::cout << ' ';
                std::cout << valueToString(evaluate(node->arguments[i]));
            }
            std::cout << '\n';
            return false;
        }

        if (node->kind == TokenType::Ask)
        {
            if (!node->arguments.empty())
            {
                std::cout << valueToString(evaluate(node->arguments[0]));
            }
            std::string input;
            if (!std::getline(std::cin, input))
            {
                throw std::runtime_error("Runtime error: could not read input.");
            }
            if (input == "true")
                return true;
            if (input == "false")
                return false;
            try
            {
                std::size_t used = 0;
                double num = std::stod(input, &used);
                if (used == input.size())
                    return num;
            }
            catch (...)
            {
            }
            return input;
        }

        throw std::runtime_error("Runtime error: unknown function call kind.");
    }

    if (auto node = dynamic_cast<AskExpr *>(expression.get()))
    {
        if (node->prompt)
        {
            std::cout << valueToString(evaluate(node->prompt));
        }
        std::string input;
        if (!std::getline(std::cin, input))
        {
            throw std::runtime_error("Runtime error: failed to read user input.");
        }
        if (input == "true")
            return true;
        if (input == "false")
            return false;
        try
        {
            std::size_t used = 0;
            double num = std::stod(input, &used);
            if (used == input.size())
                return num;
        }
        catch (...)
        {
        }
        return input;
    }

    if (auto node = dynamic_cast<UnaryExpr *>(expression.get()))
    {
        Value rightVal = evaluate(node->operand);
        if (node->op == "-")
        {
            return -requireNumber(rightVal);
        }
        if (node->op == "not" || node->op == "!")
        {
            return !requireBoolean(rightVal);
        }
        throw std::runtime_error("Runtime error: unknown unary operator '" + node->op + "'.");
    }

    if (auto node = dynamic_cast<BinaryExpr *>(expression.get()))
    {
        if (node->op == "and" || node->op == "&&")
        {
            Value leftVal = evaluate(node->left);
            if (!requireBoolean(leftVal))
                return false;
            return requireBoolean(evaluate(node->right));
        }

        if (node->op == "or" || node->op == "||")
        {
            Value leftVal = evaluate(node->left);
            if (requireBoolean(leftVal))
                return true;
            return requireBoolean(evaluate(node->right));
        }

        Value leftVal = evaluate(node->left);
        Value rightVal = evaluate(node->right);
        const auto &op = node->op;

        if (op == "+")
        {
            if (std::holds_alternative<std::string>(leftVal) && std::holds_alternative<std::string>(rightVal))
            {
                return std::get<std::string>(leftVal) + std::get<std::string>(rightVal);
            }
            return requireNumber(leftVal) + requireNumber(rightVal);
        }
        if (op == "-")
            return requireNumber(leftVal) - requireNumber(rightVal);
        if (op == "*")
            return requireNumber(leftVal) * requireNumber(rightVal);
        if (op == "/")
        {
            double rNum = requireNumber(rightVal);
            if (rNum == 0.0)
                throw std::runtime_error("Runtime error: division by zero.");
            return requireNumber(leftVal) / rNum;
        }

        if (op == ">")
            return requireNumber(leftVal) > requireNumber(rightVal);
        if (op == ">=")
            return requireNumber(leftVal) >= requireNumber(rightVal);
        if (op == "<")
            return requireNumber(leftVal) < requireNumber(rightVal);
        if (op == "<=")
            return requireNumber(leftVal) <= requireNumber(rightVal);

        if (op == "==")
            return valuesEqual(leftVal, rightVal);
        if (op == "!=")
            return !valuesEqual(leftVal, rightVal);

        throw std::runtime_error("Runtime error: unknown binary operator '" + op + "'.");
    }

    throw std::runtime_error("Runtime error: unknown expression type.");
}