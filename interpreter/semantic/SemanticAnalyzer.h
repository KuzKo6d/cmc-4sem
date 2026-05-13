#pragma once

#include "../ast/Statements.h"

#include <unordered_map>
#include <string>

class SemanticAnalyzer {
private:
    std::unordered_map<std::string, ValueType> types;

public:
    void analyze(Statement* stmt);
};
