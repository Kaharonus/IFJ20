//
// Created by tomas on 09.11.20.
//

#ifndef PROJECT_LEX_ANALYZER_H

#include "lex_token.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct{
    int indent;
    FILE* source;
} scanner;

lex_token get_next_token(scanner* sc);


#define PROJECT_LEX_ANALYZER_H

#endif //PROJECT_LEX_ANALYZER_H
