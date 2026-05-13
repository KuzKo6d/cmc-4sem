#pragma once

#include "AST.h"
#include "../runtime/Interpreter.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

class LiteralExpression : public Expression {
private:
    Value value;

public:
    explicit LiteralExpression(Value value) : value(value) {}

    const Value& getValue() const {
        return value;
    }

    Value evaluate(Interpreter&) override {
        return value;
    }
};

class VariableExpression : public Expression {
private:
    std::string name;

public:
    explicit VariableExpression(std::string name)
        : name(std::move(name)) {}

    const std::string& getName() const {
        return name;
    }

    Value evaluate(Interpreter& interpreter) override;
};

enum class BinaryOp { PLUS, MINUS, MUL, DIV, MOD, LT, GT, LE, GE, EQ, NEQ, AND, OR };

class BinaryExpression : public Expression {
private:
    BinaryOp op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

public:
    BinaryExpression(
        BinaryOp op,
        std::unique_ptr<Expression> left,
        std::unique_ptr<Expression> right
    )
        : op(op), left(std::move(left)), right(std::move(right)) {}

    Value evaluate(Interpreter& interpreter) override;
};

enum class UnaryOp { PLUS, MINUS, NOT };

class UnaryExpression : public Expression {
private:
    UnaryOp op;
    std::unique_ptr<Expression> expression;

public:
    UnaryExpression(UnaryOp op, std::unique_ptr<Expression> expression)
        : op(op), expression(std::move(expression)) {}

    Value evaluate(Interpreter& interpreter) override;
};

class AssignmentExpression : public Expression {
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

public:
    AssignmentExpression(
        std::unique_ptr<Expression> left,
        std::unique_ptr<Expression> right
    )
        : left(std::move(left)), right(std::move(right)) {}

    Value evaluate(Interpreter& interpreter) override;
};

inline Value VariableExpression::evaluate(Interpreter& interpreter) {
    return interpreter.symbols.get(name);
}

inline void requireType(const Value& value, ValueType type, const char* message) {
    if (value.type != type) {
        throw std::runtime_error(message);
    }
}

inline bool valueEquals(const Value& left, const Value& right) {
    if (left.type != right.type) {
        throw std::runtime_error("Type mismatch in comparison");
    }

    if (left.type == ValueType::INT) {
        return left.intValue == right.intValue;
    }
    if (left.type == ValueType::BOOLEAN) {
        return left.boolValue == right.boolValue;
    }
    return left.stringValue == right.stringValue;
}

inline Value BinaryExpression::evaluate(Interpreter& interpreter) {
    Value lhs = left->evaluate(interpreter);
    Value rhs = right->evaluate(interpreter);

    switch (op) {
        case BinaryOp::PLUS:
            if (lhs.type == ValueType::STRING && rhs.type == ValueType::STRING) {
                return Value::makeString(lhs.stringValue + rhs.stringValue);
            }
            requireType(lhs, ValueType::INT, "Integer operand expected");
            requireType(rhs, ValueType::INT, "Integer operand expected");
            return Value::makeInt(lhs.intValue + rhs.intValue);
        case BinaryOp::MINUS:
            requireType(lhs, ValueType::INT, "Integer operand expected");
            requireType(rhs, ValueType::INT, "Integer operand expected");
            return Value::makeInt(lhs.intValue - rhs.intValue);
        case BinaryOp::MUL:
            requireType(lhs, ValueType::INT, "Integer operand expected");
            requireType(rhs, ValueType::INT, "Integer operand expected");
            return Value::makeInt(lhs.intValue * rhs.intValue);
        case BinaryOp::DIV:
            requireType(lhs, ValueType::INT, "Integer operand expected");
            requireType(rhs, ValueType::INT, "Integer operand expected");
            if (rhs.intValue == 0) {
                throw std::runtime_error("Division by zero");
            }
            return Value::makeInt(lhs.intValue / rhs.intValue);
        case BinaryOp::MOD:
            requireType(lhs, ValueType::INT, "Integer operand expected");
            requireType(rhs, ValueType::INT, "Integer operand expected");
            if (rhs.intValue == 0) {
                throw std::runtime_error("Modulo by zero");
            }
            return Value::makeInt(lhs.intValue % rhs.intValue);
        case BinaryOp::LT:
            if (lhs.type == ValueType::STRING && rhs.type == ValueType::STRING) {
                return Value::makeBool(lhs.stringValue < rhs.stringValue);
            }
            requireType(lhs, ValueType::INT, "Integer operand expected");
            requireType(rhs, ValueType::INT, "Integer operand expected");
            return Value::makeBool(lhs.intValue < rhs.intValue);
        case BinaryOp::GT:
            if (lhs.type == ValueType::STRING && rhs.type == ValueType::STRING) {
                return Value::makeBool(lhs.stringValue > rhs.stringValue);
            }
            requireType(lhs, ValueType::INT, "Integer operand expected");
            requireType(rhs, ValueType::INT, "Integer operand expected");
            return Value::makeBool(lhs.intValue > rhs.intValue);
        case BinaryOp::LE:
            requireType(lhs, ValueType::INT, "Integer operand expected");
            requireType(rhs, ValueType::INT, "Integer operand expected");
            return Value::makeBool(lhs.intValue <= rhs.intValue);
        case BinaryOp::GE:
            requireType(lhs, ValueType::INT, "Integer operand expected");
            requireType(rhs, ValueType::INT, "Integer operand expected");
            return Value::makeBool(lhs.intValue >= rhs.intValue);
        case BinaryOp::EQ:
            return Value::makeBool(valueEquals(lhs, rhs));
        case BinaryOp::NEQ:
            return Value::makeBool(!valueEquals(lhs, rhs));
        case BinaryOp::AND:
            requireType(lhs, ValueType::BOOLEAN, "Boolean operand expected");
            requireType(rhs, ValueType::BOOLEAN, "Boolean operand expected");
            return Value::makeBool(lhs.boolValue && rhs.boolValue);
        case BinaryOp::OR:
            requireType(lhs, ValueType::BOOLEAN, "Boolean operand expected");
            requireType(rhs, ValueType::BOOLEAN, "Boolean operand expected");
            return Value::makeBool(lhs.boolValue || rhs.boolValue);
    }

    throw std::runtime_error("Unknown binary operator");
}

inline Value UnaryExpression::evaluate(Interpreter& interpreter) {
    Value value = expression->evaluate(interpreter);

    switch (op) {
        case UnaryOp::PLUS:
            requireType(value, ValueType::INT, "Integer operand expected");
            return value;
        case UnaryOp::MINUS:
            requireType(value, ValueType::INT, "Integer operand expected");
            return Value::makeInt(-value.intValue);
        case UnaryOp::NOT:
            requireType(value, ValueType::BOOLEAN, "Boolean operand expected");
            return Value::makeBool(!value.boolValue);
    }

    throw std::runtime_error("Unknown unary operator");
}

inline Value AssignmentExpression::evaluate(Interpreter& interpreter) {
    auto* variable = dynamic_cast<VariableExpression*>(left.get());
    if (variable == nullptr) {
        throw std::runtime_error("Left side of assignment must be a variable");
    }

    Value value = right->evaluate(interpreter);
    interpreter.symbols.assign(variable->getName(), value);
    return value;
}
