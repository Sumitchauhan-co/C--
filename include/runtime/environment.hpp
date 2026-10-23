#pragma once

#include "runtime/value.hpp"
#include <memory>
#include <string>
#include <unordered_map>

struct Binding
{
  Value value;
  bool immutable;
};

class Environment
{
public:
  Environment() : enclosing_(nullptr) {}
  explicit Environment(std::shared_ptr<Environment> enclosing)
      : enclosing_(std::move(enclosing)) {}

  // Variable declaration (supports mutable 'let' vs immutable 'fix')
  void declare(const std::string &name, Value value, bool immutable);

  // Variable assignment (checks scope hierarchy and immutability rules)
  void assign(const std::string &name, Value value);

  // Variable lookup (searches current scope, then traverses up to enclosing scopes)
  Value get(const std::string &name) const;

  // Scope helper
  std::shared_ptr<Environment> enclosing() const { return enclosing_; }

private:
  std::unordered_map<std::string, Binding> bindings_;
  std::shared_ptr<Environment> enclosing_;
};