#include "Parser.h"

#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace {

class ParserImpl {
private:
    std::vector<Token>& tokens;
    size_t& pos;
    int loopDepth = 0;

public:
    ParserImpl(std::vector<Token>& tokens, size_t& pos)
        : tokens(tokens), pos(pos) {}

    Token& current() {
        return tokens[pos];
    }

    bool match(TokenType type) {
        if (current().type == type) {
            pos++;
            return true;
        }
        return false;
    }

    Token expect(TokenType type) {
        if (current().type != type) {
            throw std::runtime_error("Unexpected token: " + current().text);
        }
        return tokens[pos++];
    }

    std::unique_ptr<BlockStatement> parseProgram() {
        expect(TokenType::KW_PROGRAM);
        auto program = parseBlock();
        expect(TokenType::END);
        return program;
    }

    std::unique_ptr<BlockStatement> parseBlock() {
        expect(TokenType::LBRACE);
        auto block = std::make_unique<BlockStatement>();

        while (current().type != TokenType::RBRACE) {
            if (current().type == TokenType::END) {
                throw std::runtime_error("Unterminated block");
            }
            block->add(parseStatement());
        }

        expect(TokenType::RBRACE);
        return block;
    }

    std::unique_ptr<Statement> parseStatement() {
        if (isTypeToken(current().type)) {
            return parseDeclaration();
        }

        if (match(TokenType::KW_CONTINUE)) {
            if (loopDepth == 0) {
                throw std::runtime_error("continue outside loop");
            }
            expect(TokenType::SEMICOLON);
            return std::make_unique<ContinueStatement>();
        }

        if (match(TokenType::KW_READ)) {
            expect(TokenType::LPAREN);
            std::string name = expect(TokenType::IDENTIFIER).text;
            expect(TokenType::RPAREN);
            expect(TokenType::SEMICOLON);
            return std::make_unique<ReadStatement>(name);
        }

        if (match(TokenType::KW_WRITE)) {
            expect(TokenType::LPAREN);
            std::vector<std::unique_ptr<Expression>> expressions;
            expressions.push_back(parseExpression());
            while (match(TokenType::COMMA)) {
                expressions.push_back(parseExpression());
            }
            expect(TokenType::RPAREN);
            expect(TokenType::SEMICOLON);
            return std::make_unique<WriteStatement>(std::move(expressions));
        }

        if (match(TokenType::KW_IF)) {
            expect(TokenType::LPAREN);
            auto condition = parseExpression();
            expect(TokenType::RPAREN);
            auto thenStatement = parseStatement();
            std::unique_ptr<Statement> elseStatement;
            if (match(TokenType::KW_ELSE)) {
                elseStatement = parseStatement();
            }
            return std::make_unique<IfStatement>(
                std::move(condition),
                std::move(thenStatement),
                std::move(elseStatement)
            );
        }

        if (match(TokenType::KW_WHILE)) {
            expect(TokenType::LPAREN);
            auto condition = parseExpression();
            expect(TokenType::RPAREN);
            loopDepth++;
            auto body = parseStatement();
            loopDepth--;
            return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
        }

        if (match(TokenType::KW_FOR)) {
            std::string var = expect(TokenType::IDENTIFIER).text;
            expect(TokenType::ASSIGN);
            auto start = parseExpression();
            expect(TokenType::KW_STEP);
            auto step = parseExpression();
            expect(TokenType::KW_UNTIL);
            auto until = parseExpression();
            expect(TokenType::KW_DO);
            loopDepth++;
            auto body = parseStatement();
            loopDepth--;
            return std::make_unique<ForStatement>(
                var,
                std::move(start),
                std::move(step),
                std::move(until),
                std::move(body)
            );
        }

        if (match(TokenType::KW_CASE)) {
            return parseCase();
        }

        if (current().type == TokenType::LBRACE) {
            return parseBlock();
        }

        auto expression = parseExpression();
        expect(TokenType::SEMICOLON);
        return std::make_unique<ExpressionStatement>(std::move(expression));
    }

    std::unique_ptr<Statement> parseDeclaration() {
        ValueType type = parseType();
        std::vector<DeclarationItem> items;

        do {
            DeclarationItem item;
            item.name = expect(TokenType::IDENTIFIER).text;
            if (match(TokenType::ASSIGN)) {
                item.initializer = parseConstant();
            }
            items.push_back(std::move(item));
        } while (match(TokenType::COMMA));

        expect(TokenType::SEMICOLON);
        return std::make_unique<VariableDeclarationStatement>(type, std::move(items));
    }

    std::unique_ptr<Statement> parseCase() {
        expect(TokenType::LPAREN);
        auto expression = parseExpression();
        expect(TokenType::RPAREN);
        expect(TokenType::KW_OF);

        std::vector<CaseBranch> branches;
        std::unordered_set<int> constants;

        while (current().type != TokenType::KW_END) {
            if (current().type == TokenType::END) {
                throw std::runtime_error("Unterminated case");
            }

            CaseBranch branch;
            do {
                auto constant = parseConstant();
                auto* literal = dynamic_cast<LiteralExpression*>(constant.get());
                Value value = literal->getValue();
                requireType(value, ValueType::INT, "Case constant must be int");
                if (!constants.insert(value.intValue).second) {
                    throw std::runtime_error("Duplicate case constant");
                }
                branch.constants.push_back(value);
            } while (match(TokenType::COMMA));

            expect(TokenType::COLON);
            branch.statement = parseStatement();
            branches.push_back(std::move(branch));
        }

        expect(TokenType::KW_END);
        expect(TokenType::SEMICOLON);
        return std::make_unique<CaseStatement>(std::move(expression), std::move(branches));
    }

    std::unique_ptr<Expression> parseExpression() {
        return parseAssignment();
    }

    std::unique_ptr<Expression> parseAssignment() {
        auto left = parseOr();
        if (match(TokenType::ASSIGN)) {
            return std::make_unique<AssignmentExpression>(std::move(left), parseAssignment());
        }
        return left;
    }

    std::unique_ptr<Expression> parseOr() {
        auto left = parseAnd();
        while (match(TokenType::KW_OR)) {
            left = std::make_unique<BinaryExpression>(
                BinaryOp::OR,
                std::move(left),
                parseAnd()
            );
        }
        return left;
    }

    std::unique_ptr<Expression> parseAnd() {
        auto left = parseEquality();
        while (match(TokenType::KW_AND)) {
            left = std::make_unique<BinaryExpression>(
                BinaryOp::AND,
                std::move(left),
                parseEquality()
            );
        }
        return left;
    }

    std::unique_ptr<Expression> parseEquality() {
        auto left = parseRelational();
        while (true) {
            if (match(TokenType::EQ)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::EQ,
                    std::move(left),
                    parseRelational()
                );
            }
            else if (match(TokenType::NEQ)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::NEQ,
                    std::move(left),
                    parseRelational()
                );
            }
            else {
                return left;
            }
        }
    }

    std::unique_ptr<Expression> parseRelational() {
        auto left = parseAdditive();
        while (true) {
            if (match(TokenType::LT)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::LT,
                    std::move(left),
                    parseAdditive()
                );
            }
            else if (match(TokenType::GT)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::GT,
                    std::move(left),
                    parseAdditive()
                );
            }
            else if (match(TokenType::LE)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::LE,
                    std::move(left),
                    parseAdditive()
                );
            }
            else if (match(TokenType::GE)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::GE,
                    std::move(left),
                    parseAdditive()
                );
            }
            else {
                return left;
            }
        }
    }

    std::unique_ptr<Expression> parseAdditive() {
        auto left = parseMultiplicative();
        while (true) {
            if (match(TokenType::PLUS)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::PLUS,
                    std::move(left),
                    parseMultiplicative()
                );
            }
            else if (match(TokenType::MINUS)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::MINUS,
                    std::move(left),
                    parseMultiplicative()
                );
            }
            else {
                return left;
            }
        }
    }

    std::unique_ptr<Expression> parseMultiplicative() {
        auto left = parseUnary();
        while (true) {
            if (match(TokenType::MUL)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::MUL,
                    std::move(left),
                    parseUnary()
                );
            }
            else if (match(TokenType::DIV)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::DIV,
                    std::move(left),
                    parseUnary()
                );
            }
            else if (match(TokenType::MOD)) {
                left = std::make_unique<BinaryExpression>(
                    BinaryOp::MOD,
                    std::move(left),
                    parseUnary()
                );
            }
            else {
                return left;
            }
        }
    }

    std::unique_ptr<Expression> parseUnary() {
        if (match(TokenType::PLUS)) {
            return std::make_unique<UnaryExpression>(UnaryOp::PLUS, parseUnary());
        }
        if (match(TokenType::MINUS)) {
            return std::make_unique<UnaryExpression>(UnaryOp::MINUS, parseUnary());
        }
        if (match(TokenType::KW_NOT)) {
            return std::make_unique<UnaryExpression>(UnaryOp::NOT, parseUnary());
        }
        return parsePrimary();
    }

    std::unique_ptr<Expression> parsePrimary() {
        if (current().type == TokenType::INT_LITERAL ||
            current().type == TokenType::STRING_LITERAL ||
            current().type == TokenType::BOOL_LITERAL) {
            return parseConstant();
        }

        if (match(TokenType::IDENTIFIER)) {
            return std::make_unique<VariableExpression>(tokens[pos - 1].text);
        }

        if (match(TokenType::LPAREN)) {
            auto expression = parseExpression();
            expect(TokenType::RPAREN);
            return expression;
        }

        throw std::runtime_error("Invalid expression");
    }

    std::unique_ptr<Expression> parseConstant() {
        bool negate = false;
        if (match(TokenType::PLUS)) {
        }
        else if (match(TokenType::MINUS)) {
            negate = true;
        }

        if (match(TokenType::INT_LITERAL)) {
            int value = std::stoi(tokens[pos - 1].text);
            return std::make_unique<LiteralExpression>(Value::makeInt(negate ? -value : value));
        }

        if (negate) {
            throw std::runtime_error("Sign is only allowed before numeric constants");
        }

        if (match(TokenType::STRING_LITERAL)) {
            return std::make_unique<LiteralExpression>(
                Value::makeString(tokens[pos - 1].text)
            );
        }

        if (match(TokenType::BOOL_LITERAL)) {
            return std::make_unique<LiteralExpression>(
                Value::makeBool(tokens[pos - 1].text == "true")
            );
        }

        throw std::runtime_error("Constant expected");
    }

private:
    bool isTypeToken(TokenType type) const {
        return type == TokenType::KW_INT ||
               type == TokenType::KW_STRING ||
               type == TokenType::KW_BOOLEAN;
    }

    ValueType parseType() {
        if (match(TokenType::KW_INT)) {
            return ValueType::INT;
        }
        if (match(TokenType::KW_STRING)) {
            return ValueType::STRING;
        }
        if (match(TokenType::KW_BOOLEAN)) {
            return ValueType::BOOLEAN;
        }
        throw std::runtime_error("Type expected");
    }

};

} // namespace

std::unique_ptr<BlockStatement> Parser::parseProgram() {
    ParserImpl p(tokens, pos);
    return p.parseProgram();
}

std::unique_ptr<Statement> Parser::parseStatement() {
    ParserImpl p(tokens, pos);
    return p.parseStatement();
}

std::unique_ptr<BlockStatement> Parser::parseBlock() {
    ParserImpl p(tokens, pos);
    return p.parseBlock();
}
