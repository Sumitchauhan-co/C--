#include "interpreter/interpreter.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

// ==========================================
// Control Flow & Exception Signals
// ==========================================
struct BreakSignal
{
};
struct ContinueSignal
{
};
struct ReturnSignal
{
    Value value;
};
struct ThrowSignal
{
    Value value;
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

    if (auto node = dynamic_cast<ForInStmt *>(statement.get()))
    {
        Value collVal = evaluate(node->collection);
        if (std::holds_alternative<ArrayValue>(collVal))
        {
            auto arr = std::get<ArrayValue>(collVal);
            ++loopDepth_;
            auto oldEnv = environment_;
            try
            {
                for (const auto &item : *arr)
                {
                    environment_ = std::make_shared<Environment>(oldEnv);
                    environment_->declare(node->variable, item, false);
                    try
                    {
                        executeBlock(*node->body);
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
                environment_ = oldEnv;
                --loopDepth_;
            }
            catch (...)
            {
                environment_ = oldEnv;
                --loopDepth_;
                throw;
            }
            return;
        }
        throw std::runtime_error("Runtime error: 'for...in' expects an array collection.");
    }

    if (auto node = dynamic_cast<FunctionStmt *>(statement.get()))
    {
        environment_->declare(node->name, std::shared_ptr<FunctionStmt>(node, [](FunctionStmt *) {}), true);
        return;
    }

    if (auto node = dynamic_cast<ReturnStmt *>(statement.get()))
    {
        Value val = node->value ? evaluate(node->value) : 0.0;
        throw ReturnSignal{val};
    }

    if (auto node = dynamic_cast<ThrowStmt *>(statement.get()))
    {
        Value val = evaluate(node->expression);
        throw ThrowSignal{val};
    }

    if (auto node = dynamic_cast<TryStmt *>(statement.get()))
    {
        try
        {
            executeBlock(*node->tryBody);
        }
        catch (const ThrowSignal &ts)
        {
            auto oldEnv = environment_;
            environment_ = std::make_shared<Environment>(oldEnv);
            if (!node->catchVar.empty())
            {
                environment_->declare(node->catchVar, ts.value, false);
            }
            try
            {
                executeBlock(*node->catchBody);
                environment_ = oldEnv;
            }
            catch (...)
            {
                environment_ = oldEnv;
                throw;
            }
        }
        catch (const std::runtime_error &e)
        {
            auto oldEnv = environment_;
            environment_ = std::make_shared<Environment>(oldEnv);
            if (!node->catchVar.empty())
            {
                environment_->declare(node->catchVar, std::string(e.what()), false);
            }
            try
            {
                executeBlock(*node->catchBody);
                environment_ = oldEnv;
            }
            catch (...)
            {
                environment_ = oldEnv;
                throw;
            }
        }
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
// Loop Execution Handlers
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
// Function Call Handler
// ==========================================

Value Interpreter::callFunction(const FunctionStmt &fnStmt, const std::vector<Value> &arguments)
{
    auto oldEnv = environment_;
    auto funcEnv = std::make_shared<Environment>(oldEnv);

    for (std::size_t i = 0; i < fnStmt.parameters.size(); ++i)
    {
        funcEnv->declare(fnStmt.parameters[i], arguments[i], false);
    }

    environment_ = funcEnv;
    try
    {
        if (fnStmt.body)
        {
            for (const auto &stmt : fnStmt.body->statements)
            {
                execute(stmt);
            }
        }
        environment_ = oldEnv;
        return 0.0;
    }
    catch (const ReturnSignal &rs)
    {
        environment_ = oldEnv;
        return rs.value;
    }
    catch (...)
    {
        environment_ = oldEnv;
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

    if (auto node = dynamic_cast<ArrayExpr *>(expression.get()))
    {
        auto arr = std::make_shared<std::vector<Value>>();
        for (const auto &elemExpr : node->elements)
        {
            arr->push_back(evaluate(elemExpr));
        }
        return arr;
    }

    if (auto node = dynamic_cast<ObjectExpr *>(expression.get()))
    {
        auto obj = std::make_shared<std::map<std::string, Value>>();
        for (const auto &pair : node->pairs)
        {
            Value keyVal = evaluate(pair.first);
            Value val = evaluate(pair.second);
            std::string keyStr = valueToString(keyVal);
            (*obj)[keyStr] = val;
        }
        return obj;
    }

    if (auto node = dynamic_cast<IndexExpr *>(expression.get()))
    {
        Value targetVal = evaluate(node->objectOrArray);
        Value indexVal = evaluate(node->index);

        if (std::holds_alternative<ArrayValue>(targetVal))
        {
            auto arr = std::get<ArrayValue>(targetVal);
            double idxDouble = requireNumber(indexVal);
            long long idx = static_cast<long long>(idxDouble);
            if (idx < 0 || static_cast<size_t>(idx) >= arr->size())
            {
                throw std::runtime_error("Runtime error: Array index out of bounds.");
            }
            return (*arr)[idx];
        }
        else if (std::holds_alternative<ObjectValue>(targetVal))
        {
            auto obj = std::get<ObjectValue>(targetVal);
            std::string key = valueToString(indexVal);
            auto it = obj->find(key);
            if (it == obj->end())
            {
                throw std::runtime_error("Runtime error: Object key '" + key + "' not found.");
            }
            return it->second;
        }
        throw std::runtime_error("Runtime error: Indexing is only supported on arrays and objects.");
    }

    // Handle CallExpr (builtin or user-defined function)
    if (auto node = dynamic_cast<CallExpr *>(expression.get()))
    {
        if (node->isBuiltin)
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

            throw std::runtime_error("Runtime error: unknown builtin function call.");
        }
        else
        {
            Value calleeVal = environment_->get(node->name);
            if (!std::holds_alternative<std::shared_ptr<FunctionStmt>>(calleeVal))
            {
                throw std::runtime_error("Runtime error: '" + node->name + "' is not a function.");
            }
            auto fnStmt = std::get<std::shared_ptr<FunctionStmt>>(calleeVal);

            if (node->arguments.size() != fnStmt->parameters.size())
            {
                throw std::runtime_error("Runtime error: function '" + fnStmt->name + "' expects " +
                                         std::to_string(fnStmt->parameters.size()) + " arguments, but got " +
                                         std::to_string(node->arguments.size()) + ".");
            }

            std::vector<Value> evaluatedArgs;
            for (const auto &argExpr : node->arguments)
            {
                evaluatedArgs.push_back(evaluate(argExpr));
            }

            return callFunction(*fnStmt, evaluatedArgs);
        }
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