#pragma once

#include "../ast/Statements.h"
#include "../lexer/Token.h"

#include <vector>
#include <memory>

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos = 0;

public:
    explicit Parser(std::vector<Token> tokens)
        : tokens(std::move(tokens)) {}

    std::unique_ptr<BlockStatement> parseProgram();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<BlockStatement> parseBlock();
};
