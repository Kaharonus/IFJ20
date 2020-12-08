//
// Created by tomas on 19.11.20.
//

#include <stdlib.h>
#include "syntax_tree.h"
#include "error.h"
#include "garbage_collector.h"

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
        "FOR_LOOP",
        "VALUE",
        "INT_TO_FLOAT",
        "FLOAT_TO_INT",
        "FUNCTION_DEFINITION",
        "FUNCTION_CALL",
        "RETURN_VALUE",
        "VAR_DEFINITION",
        "ASSIGN_LEFT",
        "ASSIGN_RIGHT",
        "DISCARD_VALUE",
        NULL
};

tree_node *create_node() {
    tree_node * node = malloc(sizeof(tree_node));
    if (node == NULL) {
        throw_err(INTERN_ERR);

    }
    add_to_gc(node);

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
    add_to_gc(node->nodes);
    return node;
}
void insert_node(tree_node* node, tree_node* new) {
    if ((node->subnode_len + 1) > node->capacity) {
        node->capacity *= 10;
        tree_node ** tmp = node->nodes;
        node->nodes = realloc(node->nodes, node->capacity * sizeof(tree_node*));
        update_gc(tmp, node->nodes);

    }
    node->nodes[node->subnode_len] = new;
    node->subnode_len += 1;
}


void remove_last_node(tree_node *node) {
    if(node->subnode_len > 0){
        node->nodes[node->subnode_len] = NULL;
        node->subnode_len -= 1;
    }

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

void traverse_left(tree_node* tree,tree_node** result, int *counter, int *currentCap);

void traverse_left(tree_node *tree, tree_node **result, int *counter, int *currentCap) {
    result[*counter] = tree;
    (*counter)++;
    for(unsigned i = 0; i < tree->subnode_len; i++){
        if(*counter + 2 >= *currentCap){
            tree_node ** tmp = result;
            *currentCap += 1024;
            result = realloc(result, sizeof(tree_node*)* *currentCap);
            if(result == NULL){
                throw_err(INTERN_ERR);
            }
            update_gc(tmp, result);
        }
        traverse_left(tree->nodes[i],result, counter, currentCap);
    }
}

tree_node **get_preorder(tree_node *root, int *size) {
    int cap = 1024;
    tree_node** result = malloc(sizeof(tree_node*) * cap);
    add_to_gc(result);
    traverse_left(root, result, size, &cap);
    return result;
}


