//
// Created by tomas on 01.12.20.
//

#include "lex_token.h"
#include "syntax_tree.h"

#ifndef PROJECT_EXPRESSION_STACK_H
#define PROJECT_EXPRESSION_STACK_H

#endif //PROJECT_EXPRESSION_STACK_H

typedef struct {
    lex_token t;
    tree_node* node;
} SSData;

typedef struct {
    unsigned index;
    SSData data[SYNTAX_STACK_MAX_SIZE];
} SyntaxStack;
