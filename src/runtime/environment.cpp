#include "runtime/environment.hpp"
#include <stdexcept>
#include <utility>

// ==========================================
// Variable Declaration, Assignment, & Lookup
// ==========================================

void Environment::declare(const std::string &name, Value value, bool immutable)
{
  if (bindings_.find(name) != bindings_.end())
  {
    throw std::runtime_error("Runtime error: variable '" + name + "' is already declared in this scope.");
  }
  bindings_.emplace(name, Binding{std::move(value), immutable});
}

void Environment::assign(const std::string &name, Value value)
{
  auto it = bindings_.find(name);
  if (it != bindings_.end())
  {
    if (it->second.immutable)
    {
      throw std::runtime_error("Runtime error: cannot reassign immutable variable '" + name + "'.");
    }
    it->second.value = std::move(value);
    return;
  }

  if (enclosing_)
  {
    enclosing_->assign(name, std::move(value));
    return;
  }

  throw std::runtime_error("Runtime error: cannot assign to undefined variable '" + name + "'.");
}

Value Environment::get(const std::string &name) const
{
  auto it = bindings_.find(name);
  if (it != bindings_.end())
  {
    return it->second.value;
  }

  if (enclosing_)
  {
    return enclosing_->get(name);
  }

  throw std::runtime_error("Runtime error: undefined variable '" + name + "'.");
}