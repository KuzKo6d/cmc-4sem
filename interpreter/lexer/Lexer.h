#pragma once

#include "Token.h"
#include <vector>

class Lexer {
private:
    std::string source;
    size_t pos = 0;

public:
    explicit Lexer(const std::string& src);

    std::vector<Token> tokenize();
};
