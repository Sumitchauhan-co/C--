#pragma once

#include <string>
#include <unordered_map>

class Environment {
public:
    void define(const std::string& name, long long value);
    void assign(const std::string& name, long long value);
    long long get(const std::string& name) const;

private:
    std::unordered_map<std::string, long long> values_;
};
