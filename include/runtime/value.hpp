#pragma once

#include <string>
#include <variant>

// ==========================================
// C-- Runtime Value Definition
// ==========================================

using Value = std::variant<double, bool, std::string>;

// ==========================================
// Runtime Utilities & Type Checkers
// ==========================================

std::string valueToString(const Value &value);
double requireNumber(const Value &value);
bool requireBoolean(const Value &value);
bool valuesEqual(const Value &left, const Value &right);