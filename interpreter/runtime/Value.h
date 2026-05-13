#pragma once

#include <string>
#include <stdexcept>

enum class ValueType {
    INT,
    STRING,
    BOOLEAN
};

class Value {
public:
    ValueType type;

    int intValue{};
    bool boolValue{};
    std::string stringValue;

    static Value makeInt(int v) {
        Value x;
        x.type = ValueType::INT;
        x.intValue = v;
        return x;
    }

    static Value makeBool(bool v) {
        Value x;
        x.type = ValueType::BOOLEAN;
        x.boolValue = v;
        return x;
    }

    static Value makeString(const std::string& v) {
        Value x;
        x.type = ValueType::STRING;
        x.stringValue = v;
        return x;
    }
};
