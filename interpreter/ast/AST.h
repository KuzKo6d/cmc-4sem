#pragma once

#include "../runtime/Value.h"

class Interpreter;

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
    virtual Value evaluate(Interpreter& interpreter) = 0;
};

class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
    virtual void execute(Interpreter& interpreter) = 0;
};
