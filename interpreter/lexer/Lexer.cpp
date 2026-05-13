#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

Lexer::Lexer(const std::string& src)
    : source(src) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    std::unordered_map<std::string, TokenType> keywords = {
        {"program", TokenType::KW_PROGRAM},
        {"int", TokenType::KW_INT},
        {"string", TokenType::KW_STRING},
        {"boolean", TokenType::KW_BOOLEAN},
        {"if", TokenType::KW_IF},
        {"else", TokenType::KW_ELSE},
        {"while", TokenType::KW_WHILE},
        {"for", TokenType::KW_FOR},
        {"step", TokenType::KW_STEP},
        {"until", TokenType::KW_UNTIL},
        {"do", TokenType::KW_DO},
        {"case", TokenType::KW_CASE},
        {"of", TokenType::KW_OF},
        {"end", TokenType::KW_END},
        {"continue", TokenType::KW_CONTINUE},
        {"read", TokenType::KW_READ},
        {"write", TokenType::KW_WRITE},
        {"and", TokenType::KW_AND},
        {"or", TokenType::KW_OR},
        {"not", TokenType::KW_NOT}
    };

    while (pos < source.size()) {
        char c = source[pos];

        if (isspace(static_cast<unsigned char>(c))) {
            pos++;
            continue;
        }

        if (isalpha(static_cast<unsigned char>(c)) || c == '_') {
            std::string id;

            while (pos < source.size() &&
                   (isalnum(static_cast<unsigned char>(source[pos])) || source[pos] == '_')) {
                id += source[pos++];
            }

            if (id == "true" || id == "false") {
                tokens.push_back({TokenType::BOOL_LITERAL, id});
            }
            else if (keywords.contains(id)) {
                tokens.push_back({keywords[id], id});
            }
            else {
                tokens.push_back({TokenType::IDENTIFIER, id});
            }

            continue;
        }

        if (isdigit(static_cast<unsigned char>(c))) {
            std::string number;

            while (pos < source.size() && isdigit(static_cast<unsigned char>(source[pos]))) {
                number += source[pos++];
            }

            tokens.push_back({TokenType::INT_LITERAL, number});
            continue;
        }

        if (c == '"') {
            pos++;
            std::string str;

            while (pos < source.size() && source[pos] != '"') {
                str += source[pos++];
            }

            if (pos >= source.size()) {
                throw std::runtime_error("Unterminated string");
            }

            pos++;

            tokens.push_back({TokenType::STRING_LITERAL, str});
            continue;
        }

        switch (c) {
            case '+':
                tokens.push_back({TokenType::PLUS, "+"});
                break;

            case '-':
                tokens.push_back({TokenType::MINUS, "-"});
                break;

            case '*':
                tokens.push_back({TokenType::MUL, "*"});
                break;

            case '/':
                if (pos + 1 < source.size() && source[pos + 1] == '*') {
                    pos += 2;
                    while (pos + 1 < source.size() &&
                           !(source[pos] == '*' && source[pos + 1] == '/')) {
                        pos++;
                    }
                    if (pos + 1 >= source.size()) {
                        throw std::runtime_error("Unterminated comment");
                    }
                    pos += 2;
                    continue;
                }
                tokens.push_back({TokenType::DIV, "/"});
                break;

            case '%':
                tokens.push_back({TokenType::MOD, "%"});
                break;

            case '(':
                tokens.push_back({TokenType::LPAREN, "("});
                break;

            case ')':
                tokens.push_back({TokenType::RPAREN, ")"});
                break;

            case '{':
                tokens.push_back({TokenType::LBRACE, "{"});
                break;

            case '}':
                tokens.push_back({TokenType::RBRACE, "}"});
                break;

            case ';':
                tokens.push_back({TokenType::SEMICOLON, ";"});
                break;

            case ',':
                tokens.push_back({TokenType::COMMA, ","});
                break;

            case ':':
                tokens.push_back({TokenType::COLON, ":"});
                break;

            case '=':
                if (pos + 1 < source.size() && source[pos + 1] == '=') {
                    tokens.push_back({TokenType::EQ, "=="});
                    pos++;
                }
                else {
                    tokens.push_back({TokenType::ASSIGN, "="});
                }
                break;

            case '!':
                if (pos + 1 < source.size() && source[pos + 1] == '=') {
                    tokens.push_back({TokenType::NEQ, "!="});
                    pos++;
                }
                else {
                    throw std::runtime_error("Unexpected character");
                }
                break;

            case '<':
                if (pos + 1 < source.size() && source[pos + 1] == '=') {
                    tokens.push_back({TokenType::LE, "<="});
                    pos++;
                }
                else {
                    tokens.push_back({TokenType::LT, "<"});
                }
                break;

            case '>':
                if (pos + 1 < source.size() && source[pos + 1] == '=') {
                    tokens.push_back({TokenType::GE, ">="});
                    pos++;
                }
                else {
                    tokens.push_back({TokenType::GT, ">"});
                }
                break;

            default:
                throw std::runtime_error("Unexpected character");
        }

        pos++;
    }

    tokens.push_back({TokenType::END, ""});

    return tokens;
}
