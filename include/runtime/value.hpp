#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <variant>

// Forward declaration
struct FunctionStmt;
struct Value;

// Container type aliases using shared pointers to avoid recursive incomplete type issues
using ArrayValue = std::shared_ptr<std::vector<Value>>;
using ObjectValue = std::shared_ptr<std::map<std::string, Value>>;

struct Value : public std::variant<
                   double,
                   bool,
                   std::string,
                   std::shared_ptr<FunctionStmt>,
                   ArrayValue,
                   ObjectValue>
{
    using variant::variant; // Inherit constructors
};

// ==========================================
// Runtime Utilities & Type Checkers
// ==========================================

std::string valueToString(const Value &value);
double requireNumber(const Value &value);
bool requireBoolean(const Value &value);
bool valuesEqual(const Value &left, const Value &right);