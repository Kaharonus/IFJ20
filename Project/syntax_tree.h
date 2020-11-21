//
// Created by tomas on 19.11.20.
//

#ifndef PROJECT_SYNTAX_TREE_H
#define PROJECT_SYNTAX_TREE_H

#include "lex_token.h"
#include "symbol_table.h"

#define BASE_CAPACITY 10


typedef enum {
    EXPRESSION,
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    INT_DIVISION,
    ASSIGNMENT,
    CONDITION,
    BLOCK,
    PROGRAM_ROOT,
    IDENTIFICATOR,
    IF_ELSE,
    WHILE_LOOP,
    VALUE,
    FLOAT_TO_INT,
    FUNCTION_DEFINITION,
    FUNCTION_CALL,
    RETURN_VALUE,
} tree_node_type;

typedef enum {
    OP_NONE,
    OP_GR,
    OP_LS,
    OP_EQ,
    OP_NEQ,
    OP_GREQ,
    OP_LSEQ
} tree_node_cond_type;


typedef struct syntax_tree_node{
    tree_node_type type;

    tree_node_cond_type cond_type;

    number_value_token number_value;
    char* string_value;


    struct sym_table_struct* symbol;
    unsigned int subnode_len;
    unsigned int capacity;
    struct syntax_tree_node** nodes;

}tree_node;

tree_node *create_node();
void insert_node(tree_node* orig, tree_node* new);
void print_tree(tree_node* tree);

tree_node** get_all_expressions(tree_node* root, unsigned* len);


#endif //PROJECT_SYNTAX_TREE_H
