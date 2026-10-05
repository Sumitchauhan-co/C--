#pragma once
#include "value.hpp"
#include <string>
#include <unordered_map>

class Environment
{
public:
  void define(const std::string &name, Value value);
  void assign(const std::string &name, Value value);

  Value get(const std::string &name) const;

private:
  std::unordered_map<std::string, Value> values_;
};
