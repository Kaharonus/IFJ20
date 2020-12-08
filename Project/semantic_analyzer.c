//
// Created by tomas on 07.12.20.
//

#include "semantic_analyzer.h"
#include "error.h"


void check_function_call_semantics(tree_node** nodes, const int *index, symbol_table ** table){
    tree_node * call = nodes[*index];
    symbol_table * item = search_ht(table, call->string_value);
    if(item == NULL){
        throw_err(SA_ERR);
    }
    if(item->arg_count != call->subnode_len){
        throw_err(PARAM_ERR);
    }
}

void check_assignment_semantics(tree_node** nodes, int *index, symbol_table ** table){
    tree_node * call = nodes[*index];
    if(call->subnode_len != 2){
        throw_err(INTERN_ERR);
    }
    call = nodes[*index]->nodes[1];
    if(!(call->subnode_len == 1 && call->nodes[0]->type == FUNCTION_CALL)){
        return;
    }
    symbol_table * item = search_ht(table, call->nodes[0]->string_value);
    if(item == NULL){
        throw_err(SA_ERR);
    }
    if(item->arg_count != call->nodes[0]->subnode_len){
        throw_err(PARAM_ERR);
    }
    call = nodes[*index];
    if(call->nodes[0]->subnode_len != item->ret_type_count){
        throw_err(PARAM_ERR);
    }

}

void check_semantics(tree_node *node, symbol_table **table) {
    int size = 0;
    tree_node ** nodes = get_preorder(node, &size);
    for(int i = 0; i < size; i++){
        switch(nodes[i]->type){
            case FUNCTION_CALL:
                check_function_call_semantics(nodes, &i , table);
                break;
            case ASSIGNMENT:
                check_assignment_semantics(nodes, &i, table);
                break;
            default:
                break;
        }
    }
}
