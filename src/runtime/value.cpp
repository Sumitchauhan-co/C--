#include "runtime/value.hpp"
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
  return std::get<std::string>(value);
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
  return std::get<std::string>(left) == std::get<std::string>(right);
}