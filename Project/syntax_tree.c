//
// Created by tomas on 19.11.20.
//

#include <stdlib.h>
#include "syntax_tree.h"
#include "error.h"

const char *node_types[100] = {
        "EXPRESSION",
        "ADDITION",
        "SUBTRACTION",
        "MULTIPLICATION",
        "DIVISION",
        "INT_DIVISION",
        "ASSIGNMENT",
        "CONDITION",
        "BLOCK",
        "PROGRAM_ROOT",
        "IDENTIFICATOR",
        "IF_ELSE",
        "WHILE_LOOP",
        "VALUE",
        "FLOAT_TO_INT",
        "FUNCTION_DEFINITION",
        "FUNCTION_CALL",
        "RETURN_VALUE",
        "VAR_DEFINITION",
        NULL
};

tree_node *create_node() {
    tree_node * node = malloc(sizeof(tree_node));
    if (node == NULL) {
        throw_err(INTERN_ERR);

    }
    node->type = PROGRAM_ROOT;
    node->subnode_len = 0;
    node->capacity = BASE_CAPACITY;
    node->number_value.d = node->number_value.i = 0;
    node->string_value = NULL;
    node->cond_type = OP_NONE;
    node->nodes = malloc(BASE_CAPACITY * sizeof(tree_node*));
    if (node->nodes == NULL) {
        throw_err(INTERN_ERR);
    }
    return node;
}
void insert_node(tree_node* node, tree_node* new) {
    if ((node->subnode_len + 1) > node->capacity) {
        tree_node ** tmp = node->nodes;
        node->nodes = realloc(node->nodes, node->capacity * 10);
        node->capacity *= 10;
    }
    node->nodes[node->subnode_len] = new;
    node->subnode_len += 1;
}

void print_node_name(tree_node * tree, int indent){
    for(int i = 0; i < indent*2; i++){
        fprintf(stderr," ");
    }
    fprintf(stderr,"%s", node_types[tree->type]);
    if(tree->type == VALUE){
        switch (tree->value_type) {
            case TYPE_INT:
                fprintf(stderr," (%d)", tree->number_value.i);
                break;
            case TYPE_FLOAT:
                fprintf(stderr," (%f)", tree->number_value.d);
                break;
            case TYPE_STRING:
                fprintf(stderr," (%s)", tree->string_value);
                break;
            default:
                break;
        }
    }
    fprintf(stderr, "\n");
}
void print_tree_private(tree_node * tree, int indent){
    indent += 1;
    print_node_name(tree, indent);
    for(unsigned int i = 0; i < tree->subnode_len; i++){
        print_tree_private(tree->nodes[i], indent);
    }
}

void print_tree(tree_node * tree){
    fprintf(stderr,"Vypis binarniho stromu:\n");
    print_tree_private(tree, -1);
    fprintf(stderr,"Konec stromu\n");
}