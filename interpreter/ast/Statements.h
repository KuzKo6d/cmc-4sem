#pragma once

#include "Expressions.h"
#include "../runtime/Interpreter.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class ContinueSignal {};

class BlockStatement : public Statement {
private:
    std::vector<std::unique_ptr<Statement>> statements;

public:
    void add(std::unique_ptr<Statement> stmt) {
        statements.push_back(std::move(stmt));
    }

    void execute(Interpreter& interpreter) override {
        for (auto& stmt : statements) {
            stmt->execute(interpreter);
        }
    }
};

struct DeclarationItem {
    std::string name;
    std::unique_ptr<Expression> initializer;
};

class VariableDeclarationStatement : public Statement {
private:
    ValueType type;
    std::vector<DeclarationItem> items;

public:
    VariableDeclarationStatement(ValueType type, std::vector<DeclarationItem> items)
        : type(type), items(std::move(items)) {}

    void execute(Interpreter& interpreter) override {
        for (auto& item : items) {
            interpreter.symbols.declare(item.name, type);
            if (item.initializer != nullptr) {
                interpreter.symbols.assign(item.name, item.initializer->evaluate(interpreter));
            }
        }
    }
};

class ExpressionStatement : public Statement {
private:
    std::unique_ptr<Expression> expression;

public:
    explicit ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}

    void execute(Interpreter& interpreter) override {
        expression->evaluate(interpreter);
    }
};

class IfStatement : public Statement {
private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenStatement;
    std::unique_ptr<Statement> elseStatement;

public:
    IfStatement(
        std::unique_ptr<Expression> condition,
        std::unique_ptr<Statement> thenStatement,
        std::unique_ptr<Statement> elseStatement
    )
        : condition(std::move(condition)),
          thenStatement(std::move(thenStatement)),
          elseStatement(std::move(elseStatement)) {}

    void execute(Interpreter& interpreter) override {
        Value value = condition->evaluate(interpreter);
        requireType(value, ValueType::BOOLEAN, "Boolean condition expected");
        if (value.boolValue) {
            thenStatement->execute(interpreter);
        }
        else if (elseStatement != nullptr) {
            elseStatement->execute(interpreter);
        }
    }
};

class WhileStatement : public Statement {
private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

public:
    WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void execute(Interpreter& interpreter) override {
        while (true) {
            Value value = condition->evaluate(interpreter);
            requireType(value, ValueType::BOOLEAN, "Boolean condition expected");
            if (!value.boolValue) {
                break;
            }

            try {
                body->execute(interpreter);
            }
            catch (const ContinueSignal&) {
            }
        }
    }
};

class ForStatement : public Statement {
private:
    std::string variable;
    std::unique_ptr<Expression> start;
    std::unique_ptr<Expression> step;
    std::unique_ptr<Expression> until;
    std::unique_ptr<Statement> body;

public:
    ForStatement(
        std::string variable,
        std::unique_ptr<Expression> start,
        std::unique_ptr<Expression> step,
        std::unique_ptr<Expression> until,
        std::unique_ptr<Statement> body
    )
        : variable(std::move(variable)),
          start(std::move(start)),
          step(std::move(step)),
          until(std::move(until)),
          body(std::move(body)) {}

    void execute(Interpreter& interpreter) override {
        if (interpreter.symbols.typeOf(variable) != ValueType::INT) {
            throw std::runtime_error("For parameter must be int");
        }

        Value startValue = start->evaluate(interpreter);
        Value stepValue = step->evaluate(interpreter);
        Value untilValue = until->evaluate(interpreter);
        requireType(startValue, ValueType::INT, "For start must be int");
        requireType(stepValue, ValueType::INT, "For step must be int");
        requireType(untilValue, ValueType::INT, "For until must be int");
        if (stepValue.intValue <= 0) {
            throw std::runtime_error("For step must be positive");
        }

        for (int i = startValue.intValue; i <= untilValue.intValue; i += stepValue.intValue) {
            interpreter.symbols.assign(variable, Value::makeInt(i));
            try {
                body->execute(interpreter);
            }
            catch (const ContinueSignal&) {
            }
        }
    }
};

class ContinueStatement : public Statement {
public:
    void execute(Interpreter&) override {
        throw ContinueSignal{};
    }
};

class ReadStatement : public Statement {
private:
    std::string variable;

public:
    explicit ReadStatement(std::string variable) : variable(std::move(variable)) {}

    void execute(Interpreter& interpreter) override {
        ValueType type = interpreter.symbols.typeOf(variable);
        if (type == ValueType::BOOLEAN) {
            throw std::runtime_error("Cannot read boolean values");
        }

        if (type == ValueType::INT) {
            int value;
            if (!(std::cin >> value)) {
                throw std::runtime_error("Failed to read int");
            }
            interpreter.symbols.assign(variable, Value::makeInt(value));
            return;
        }

        std::string value;
        if (!(std::cin >> value)) {
            throw std::runtime_error("Failed to read string");
        }
        interpreter.symbols.assign(variable, Value::makeString(value));
    }
};

class WriteStatement : public Statement {
private:
    std::vector<std::unique_ptr<Expression>> expressions;

public:
    explicit WriteStatement(std::vector<std::unique_ptr<Expression>> expressions)
        : expressions(std::move(expressions)) {}

    void execute(Interpreter& interpreter) override {
        for (size_t i = 0; i < expressions.size(); ++i) {
            Value value = expressions[i]->evaluate(interpreter);
            if (i != 0) {
                std::cout << ' ';
            }
            if (value.type == ValueType::INT) {
                std::cout << value.intValue;
            }
            else if (value.type == ValueType::BOOLEAN) {
                std::cout << (value.boolValue ? "true" : "false");
            }
            else {
                std::cout << value.stringValue;
            }
        }
        std::cout << '\n';
    }
};

struct CaseBranch {
    std::vector<Value> constants;
    std::unique_ptr<Statement> statement;
};

class CaseStatement : public Statement {
private:
    std::unique_ptr<Expression> expression;
    std::vector<CaseBranch> branches;

public:
    CaseStatement(std::unique_ptr<Expression> expression, std::vector<CaseBranch> branches)
        : expression(std::move(expression)), branches(std::move(branches)) {}

    void execute(Interpreter& interpreter) override {
        Value value = expression->evaluate(interpreter);
        requireType(value, ValueType::INT, "Case expression must be int");

        for (auto& branch : branches) {
            for (const Value& constant : branch.constants) {
                requireType(constant, ValueType::INT, "Case constant must be int");
                if (constant.intValue == value.intValue) {
                    branch.statement->execute(interpreter);
                    return;
                }
            }
        }

        throw std::runtime_error("No matching case branch");
    }
};
