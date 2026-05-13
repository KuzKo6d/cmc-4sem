#pragma once

#include "Value.h"
#include <unordered_map>
#include <string>
#include <stdexcept>

struct VariableInfo {
    ValueType type;
    Value value;
    bool initialized = false;
};

class SymbolTable {
private:
    std::unordered_map<std::string, VariableInfo> table;

public:
    void declare(const std::string& name, ValueType type) {
        if (table.contains(name)) {
            throw std::runtime_error("Variable redeclaration");
        }

        table[name] = VariableInfo{type, {}, false};
    }

    void assign(const std::string& name, const Value& value) {
        if (!table.contains(name)) {
            throw std::runtime_error("Unknown variable");
        }

        if (table[name].type != value.type) {
            throw std::runtime_error("Type mismatch in assignment");
        }

        table[name].value = value;
        table[name].initialized = true;
    }

    Value get(const std::string& name) {
        if (!table.contains(name)) {
            throw std::runtime_error("Unknown variable");
        }

        if (!table[name].initialized) {
            throw std::runtime_error("Uninitialized variable");
        }

        return table[name].value;
    }

    ValueType typeOf(const std::string& name) const {
        if (!table.contains(name)) {
            throw std::runtime_error("Unknown variable");
        }

        return table.at(name).type;
    }
};
