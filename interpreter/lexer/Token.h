#pragma once

#include <string>

enum class TokenType {
    END,

    IDENTIFIER,

    INT_LITERAL,
    STRING_LITERAL,
    BOOL_LITERAL,

    KW_PROGRAM,
    KW_INT,
    KW_STRING,
    KW_BOOLEAN,

    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_FOR,
    KW_STEP,
    KW_UNTIL,
    KW_DO,
    KW_CASE,
    KW_OF,
    KW_END,
    KW_CONTINUE,
    KW_READ,
    KW_WRITE,

    KW_AND,
    KW_OR,
    KW_NOT,

    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD,

    ASSIGN,

    EQ,
    NEQ,
    LT,
    GT,
    LE,
    GE,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    COMMA,
    SEMICOLON,
    COLON
};

struct Token {
    TokenType type;
    std::string text;
};
