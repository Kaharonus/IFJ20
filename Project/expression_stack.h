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
} expression_stack_data;

typedef struct {
    unsigned index;
    expression_stack_data data[1024];
} expression_stack;

void expression_stack_push(expression_stack * stack, expression_stack_data data);
expression_stack_data expression_stack_pop(expression_stack* stack, bool* err);
expression_stack *expression_stack_init();
bool expression_stack_empty(expression_stack* stack);
expression_stack_data expression_stack_top(expression_stack* ss);
