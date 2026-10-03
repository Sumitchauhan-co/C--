#include "runtime/environment.hpp"

#include <stdexcept>

void Environment::define(const std::string& name, long long value) {
    if (values_.find(name) != values_.end()) {
        throw std::runtime_error("Runtime error: variable '" + name + "' is already defined");
    }

    values_[name] = value;
}

void Environment::assign(const std::string& name, long long value) {
    auto it = values_.find(name);

    if (it == values_.end()) {
        throw std::runtime_error("Runtime error: undefined variable '" + name + "'");
    }

    it->second = value;
}

long long Environment::get(const std::string& name) const {
    auto it = values_.find(name);

    if (it == values_.end()) {
        throw std::runtime_error("Runtime error: undefined variable '" + name + "'");
    }

    return it->second;
}
