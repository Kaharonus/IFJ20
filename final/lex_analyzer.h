//
// Created by tomas on 09.11.20.
//

#ifndef PROJECT_LEX_ANALYZER_H

#define STACK_RETURN_MAX_SIZE 20
#include "lex_token.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct {
    unsigned index;
    lex_token data[STACK_RETURN_MAX_SIZE];
} token_stack;

typedef struct{
    token_stack stack;
    int indent;
    FILE* source;
} scanner;

lex_token get_next_token(scanner* sc);
void unget_token(scanner* sc, lex_token t);

#define PROJECT_LEX_ANALYZER_H

#endif //PROJECT_LEX_ANALYZER_H
