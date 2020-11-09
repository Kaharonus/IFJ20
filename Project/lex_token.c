//
// Created by tomas on 09.11.20.
//

#include "lex_token.h"
const char KEYWORDS[][15] = {
        "else",
        "float64",
        "for",
        "func",
        "if",
        "int",
        "package",
        "return",
        "string",
        "non_keyword"
};

const char TOKEN_TYPES[][30] = {
    "ERROR",
    "KEYWORD",
    "ID",
    "DISCARD",
    "STRING",
    "FLOAT",
    "INT",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "ASSIGN",
    "GREATER",
    "LESSER",
    "GREATER_OR_EQ",
    "LESSER_OR_EQ",
    "EQ",
    "NON_EQ",
    "COLON",
    "COMMA",
    "OPEN_PARENTHESIS",
    "CLOSE_PARENTHESIS",
    "OPEN_BRACKET",
    "CLOSE_BRACKET",
    "END_OF_FILE",
    "END_OF_LINE"
};

//Switch nefunguje - nelze použít strcmp ve switchi a v C jinak string neporovnám...
keyword_type get_keyword(char* input){
    if (strcmp(input, KEYWORDS[0]) == 0) {
        return ELSE;
    } else if (strcmp(input, KEYWORDS[1]) == 0) {
        return FLOAT64_KEYWORD;
    } else if (strcmp(input, KEYWORDS[2]) == 0) {
        return FOR;
    } else if (strcmp(input, KEYWORDS[3]) == 0) {
        return FUNC;
    } else if (strcmp(input, KEYWORDS[4]) == 0) {
        return IF;
    } else if (strcmp(input, KEYWORDS[5]) == 0) {
        return INT_KEYWORD;
    } else if (strcmp(input, KEYWORDS[6]) == 0) {
        return PACKAGE;
    } else if (strcmp(input, KEYWORDS[7]) == 0) {
        return RETURN;
    }else if (strcmp(input, KEYWORDS[8]) == 0) {
        return STRING_KEYWORD;
    }else {
        return NOT_KEYWORD;
    }
}

