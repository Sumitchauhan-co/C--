#include "runtime/value.hpp"
#include "ast/ast.hpp"
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <iomanip>

std::string valueToString(const Value &value)
{
  if (std::holds_alternative<double>(value))
  {
    double number = std::get<double>(value);
    if (std::floor(number) == number)
      return std::to_string(static_cast<long long>(number));
    std::ostringstream out;
    out << std::setprecision(15) << number;
    return out.str();
  }
  if (std::holds_alternative<bool>(value))
    return std::get<bool>(value) ? "true" : "false";
  if (std::holds_alternative<std::string>(value))
    return std::get<std::string>(value);
  if (std::holds_alternative<std::shared_ptr<FunctionStmt>>(value))
  {
    auto fn = std::get<std::shared_ptr<FunctionStmt>>(value);
    return "<fn " + (fn ? fn->name : "anonymous") + ">";
  }
  if (std::holds_alternative<ArrayValue>(value))
  {
    auto arr = std::get<ArrayValue>(value);
    std::string result = "[";
    if (arr)
    {
      for (size_t i = 0; i < arr->size(); ++i)
      {
        if (i > 0)
          result += ", ";
        result += valueToString((*arr)[i]);
      }
    }
    return result + "]";
  }
  if (std::holds_alternative<ObjectValue>(value))
  {
    auto obj = std::get<ObjectValue>(value);
    std::string result = "{";
    if (obj)
    {
      bool first = true;
      for (const auto &[k, v] : *obj)
      {
        if (!first)
          result += ", ";
        result += "\"" + k + "\": " + valueToString(v);
        first = false;
      }
    }
    return result + "}";
  }
  return "<unknown>";
}

double requireNumber(const Value &value)
{
  if (!std::holds_alternative<double>(value))
    throw std::runtime_error("Expected a number, got '" + valueToString(value) + "'");
  return std::get<double>(value);
}

bool requireBoolean(const Value &value)
{
  if (!std::holds_alternative<bool>(value))
    throw std::runtime_error("Condition must be true or false, got '" + valueToString(value) + "'");
  return std::get<bool>(value);
}

bool valuesEqual(const Value &left, const Value &right)
{
  if (left.index() != right.index())
    return false;
  if (std::holds_alternative<double>(left))
    return std::get<double>(left) == std::get<double>(right);
  if (std::holds_alternative<bool>(left))
    return std::get<bool>(left) == std::get<bool>(right);
  if (std::holds_alternative<std::string>(left))
    return std::get<std::string>(left) == std::get<std::string>(right);
  if (std::holds_alternative<std::shared_ptr<FunctionStmt>>(left))
    return std::get<std::shared_ptr<FunctionStmt>>(left) == std::get<std::shared_ptr<FunctionStmt>>(right);
  if (std::holds_alternative<ArrayValue>(left))
    return std::get<ArrayValue>(left) == std::get<ArrayValue>(right);
  if (std::holds_alternative<ObjectValue>(left))
    return std::get<ObjectValue>(left) == std::get<ObjectValue>(right);
  return false;
}