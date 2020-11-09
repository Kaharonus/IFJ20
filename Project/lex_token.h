//
// Created by tomas on 09.11.20.
//

#ifndef PROJECT_LEX_TOKEN_H
#include <string.h>


typedef enum {
    ERROR,
    KEYWORD,
    ID,
    DISCARD,
    STRING,
    FLOAT,
    INT,
    ADD,
    SUB,
    MUL,
    DIV,
    ASSIGN,
    GREATER,
    LESSER,
    GREATER_OR_EQ,
    LESSER_OR_EQ,
    EQ,
    NOT_EQ,
    COMMA,
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    END_OF_FILE,
    END_OF_LINE,
    VAR_DEF
} lex_token_type;

typedef enum {
    ELSE,
    FLOAT64_KEYWORD,
    FOR,
    FUNC,
    IF,
    INT_KEYWORD,
    PACKAGE,
    RETURN,
    STRING_KEYWORD,
    NOT_KEYWORD
}keyword_type;

typedef union{
    int i;
    double d;
} number_value_token;

typedef struct{
    lex_token_type type;
    keyword_type keyword_value;
    char* string_value;
    number_value_token number_value;
}lex_token;

extern const char KEYWORDS[][15];

extern const char TOKEN_TYPES[][30];


keyword_type get_keyword(char*);

#define PROJECT_LEX_TOKEN_H

#endif //PROJECT_LEX_TOKEN_H
