#include "runtime/environment.hpp"
#include <stdexcept>
#include <utility>

void Environment::define(const std::string &name, Value value)
{
  values_[name] = std::move(value);
}

void Environment::assign(const std::string &name, Value value)
{
  auto it = values_.find(name);
  if (it == values_.end())
  {
    throw std::runtime_error("Runtime error: cannot assign to undefined variable '" + name + "'.");
  }
  it->second = std::move(value);
}

Value Environment::get(const std::string &name) const
{
  auto it = values_.find(name);
  if (it == values_.end())
  {
    throw std::runtime_error("Runtime error: undefined variable '" + name + "'.");
  }
  return it->second;
}
