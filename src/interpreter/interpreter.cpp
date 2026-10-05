#include "interpreter/interpreter.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

namespace
{

    long long number(const Value &v, const std::string &op)
    {
        if (!std::holds_alternative<long long>(v))
        {
            throw std::runtime_error("Runtime error: '" + op + "' requires numeric operands.");
        }
        return std::get<long long>(v);
    }

    void printValue(const Value &v)
    {
        if (std::holds_alternative<long long>(v))
        {
            std::cout << std::get<long long>(v) << '\n';
        }
        else
        {
            std::cout << (std::get<bool>(v) ? "true" : "false") << '\n';
        }
    }

} // namespace

void Interpreter::execute(const Program &p)
{
    for (const auto &s : p)
    {
        executeStatement(*s);
    }
}

void Interpreter::executeStatement(const Stmt &s)
{
    if (auto *d = dynamic_cast<const VariableDeclaration *>(&s))
    {
        environment_.define(d->name, evaluate(*d->initializer));
        return;
    }
    if (auto *a = dynamic_cast<const Assignment *>(&s))
    {
        environment_.assign(a->name, evaluate(*a->value));
        return;
    }
    if (auto *say = dynamic_cast<const SayStatement *>(&s))
    {
        printValue(evaluate(*say->expression));
        return;
    }
    if (auto *e = dynamic_cast<const ExpressionStatement *>(&s))
    {
        evaluate(*e->expression);
        return;
    }
    throw std::runtime_error("Runtime error: unknown statement.");
}

Value Interpreter::evaluate(const Expr &e)
{
    if (auto *n = dynamic_cast<const NumberExpr *>(&e))
    {
        return n->value;
    }
    if (auto *b = dynamic_cast<const BooleanExpr *>(&e))
    {
        return b->value;
    }
    if (auto *v = dynamic_cast<const VariableExpr *>(&e))
    {
        return environment_.get(v->name);
    }

    if (auto *u = dynamic_cast<const UnaryExpr *>(&e))
    {
        if (u->op == "-")
        {
            return -number(evaluate(*u->operand), "-");
        }
        throw std::runtime_error("Runtime error: unknown unary operator '" + u->op + "'.");
    }

    if (auto *b = dynamic_cast<const BinaryExpr *>(&e))
    {
        Value l = evaluate(*b->left);
        Value r = evaluate(*b->right);
        const auto &op = b->op;

        if (op == "+" || op == "-" || op == "*" || op == "/")
        {
            auto a = number(l, op);
            auto c = number(r, op);
            if (op == "+")
                return a + c;
            if (op == "-")
                return a - c;
            if (op == "*")
                return a * c;
            if (c == 0)
            {
                throw std::runtime_error("Runtime error: division by zero.");
            }
            return a / c;
        }

        if (op == ">" || op == ">=" || op == "<" || op == "<=")
        {
            auto a = number(l, op);
            auto c = number(r, op);
            if (op == ">")
                return a > c;
            if (op == ">=")
                return a >= c;
            if (op == "<")
                return a < c;
            return a <= c;
        }

        if (op == "==" || op == "!=")
        {
            if (l.index() != r.index())
            {
                return op == "!=";
            }

            bool eq;
            if (std::holds_alternative<long long>(l))
            {
                eq = std::get<long long>(l) == std::get<long long>(r);
            }
            else
            {
                eq = std::get<bool>(l) == std::get<bool>(r);
            }
            return op == "==" ? eq : !eq;
        }
        throw std::runtime_error("Runtime error: unknown binary operator '" + op + "'.");
    }
    throw std::runtime_error("Runtime error: unknown expression.");
}
