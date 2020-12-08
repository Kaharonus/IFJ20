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

void check_zero_division(tree_node* node){
}


int find_index(tree_node** from, int size, tree_node* of){
    for(int i = 0; i < size; i++){
        if(from[i] == of){
            return i;
        }
    }
    return -1;
}

int find_last(tree_node** from, int size, tree_node_type type){
    int last = -1;
    for(int i = 0; i < size ; i++){
        if(from[i]->type == type){
            last = i;
        }
    }
    return last;
}

void check_last_pair(tree_node* node){
    if(node->subnode_len == 0){
        throw_err(PARAM_ERR);
    }
    tree_node * last = node->nodes[node->subnode_len-1];
    if(last->type == IF_ELSE){
        check_last_pair(last->nodes[1]);
        check_last_pair(last->nodes[2]);
    }else if(last->type == FOR_LOOP){
        check_last_pair(last->nodes[last->subnode_len-1]);
    }else if(last->type != RETURN_VALUE){
        throw_err(SA_ERR);
    }
}

void check_return_types_semantics(tree_node* node, symbol_table* table){
    int size = 0;
    tree_node ** data = get_preorder(node, &size);
    for(int i = 0; i < size; i++){
        tree_node * item = data[i];
        if(item->type != RETURN_VALUE){
            continue;
        }
        if(item->subnode_len != table->ret_type_count){
            throw_err(PARAM_ERR);
        }
        struct argument* tmp = table->return_types;
        for(int j = 0; j < item->subnode_len; j++){
            struct argument* arg = tmp;
            tmp = tmp->next_arg;
            tree_node * expression = item->nodes[j];
            if(expression->value_type != arg->type){
                throw_err(PARAM_ERR);
            }
        }
    }
}


void check_return_semantics(tree_node* node, symbol_table** table){
    symbol_table * symbol = search_ht(table, node->string_value);
    check_return_types_semantics(node, symbol);
    if(symbol->ret_type_count == 0){
        return;
    }
    if(node->subnode_len == 0){
        throw_err(PARAM_ERR);
    }
    check_last_pair(node->nodes[node->subnode_len-1]);
}

void check_semantics(tree_node *node, symbol_table **table) {
    int size = 0;
    bool contains_main = false;
    tree_node ** nodes = get_preorder(node, &size);
    for(int i = 0; i < size; i++){
        switch(nodes[i]->type){
            case FUNCTION_DEFINITION:
                if(strcmp(nodes[i]->string_value, "main") == 0){
                    if(nodes[i]->symbol->ret_type_count != 0 || nodes[i]->symbol->arg_count != 0){
                        throw_err(OTHER_ERR);
                    }
                    contains_main = true;
                }
                check_return_semantics(nodes[i], table);
                break;
            case FUNCTION_CALL:
                check_function_call_semantics(nodes, &i , table);
                break;
            case ASSIGNMENT:
                check_assignment_semantics(nodes, &i, table);
                break;
            case DIVISION:
            case INT_DIVISION:
                check_zero_division(nodes[i]);
                break;
            default:
                break;
        }
    }
    if(!contains_main){
        throw_err(OTHER_ERR);
    }

}
