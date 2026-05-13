#pragma once

#include "SymbolTable.h"

class Statement;

class Interpreter {
public:
    SymbolTable symbols;
    void execute(Statement& stmt);
};
