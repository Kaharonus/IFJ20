//
// Created by tomas on 07.12.20.
//

#include "semantic_analyzer.h"
#include "error.h"


void check_function_call_semantics(tree_node* call, const int *index, symbol_table ** table){
    symbol_table * item = search_ht(table, call->string_value);
    if(item == NULL){
        throw_err(SA_ERR);
    }
    call->symbol = item;
    if(item->arg_count == -1){ //vestavěné funkce může mít -1
        return;
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
    tree_node * n =node->nodes[node->subnode_len-1];
    if(n->type != VALUE){
        return;
    }
    if((n->value_type == TYPE_INT && n->number_value.i == 0) || (n->value_type == TYPE_FLOAT && n->number_value.d == 0)){
        throw_err(ZERO_DIV_ERR);
    }
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

void create_inputs(symbol_table ** table){
    symbol_table * inputs = create_ht_item("inputs");
    add_ret_type(inputs, TYPE_STRING);
    add_ret_type(inputs, TYPE_INT);
    insert_ht(table, inputs);

    symbol_table * inputi = create_ht_item("inputi");
    add_ret_type(inputi, TYPE_INT);
    add_ret_type(inputi, TYPE_INT);
    insert_ht(table, inputi);

    symbol_table * inputf = create_ht_item("inputf");
    add_ret_type(inputf, TYPE_FLOAT);
    add_ret_type(inputf, TYPE_INT);
    insert_ht(table, inputf);
}

void create_type_conversions(symbol_table ** table){
    symbol_table * int2float = create_ht_item("int2float");
    add_ret_type(int2float, TYPE_FLOAT);
    add_arg(int2float, TYPE_INT, "i");
    insert_ht(table, int2float);

    symbol_table * float2int = create_ht_item("float2int");
    add_ret_type(float2int, TYPE_INT);
    add_arg(float2int, TYPE_FLOAT, "f");
    insert_ht(table, float2int);
}

void create_string_functions(symbol_table ** table){
    symbol_table * int2float = create_ht_item("len");
    add_ret_type(int2float, TYPE_INT);
    add_arg(int2float, TYPE_STRING, "s");
    insert_ht(table, int2float);

    symbol_table * substr = create_ht_item("substr");
    add_ret_type(substr, TYPE_STRING);
    add_ret_type(substr, TYPE_INT);
    add_arg(substr, TYPE_STRING, "s");
    add_arg(substr, TYPE_INT, "i");
    add_arg(substr, TYPE_INT, "n");
    insert_ht(table, substr);

    symbol_table * ord = create_ht_item("ord");
    add_ret_type(ord, TYPE_INT);
    add_ret_type(ord, TYPE_INT);
    add_arg(ord, TYPE_STRING, "s");
    add_arg(ord, TYPE_INT, "i");
    insert_ht(table, ord);

    symbol_table * chr = create_ht_item("chr");
    add_ret_type(chr, TYPE_STRING);
    add_ret_type(chr,TYPE_INT);
    add_arg(chr, TYPE_INT, "i");
    insert_ht(table, chr);

}

void add_build_in(symbol_table** table){
    create_inputs(table);
    symbol_table * print = create_ht_item("print");
    print->arg_count = -1;
    insert_ht(table, print);
    create_type_conversions(table);
    create_string_functions(table);

}

void check_semantics(tree_node *node, symbol_table **table) {
    add_build_in(table);
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
                check_function_call_semantics(nodes[i], &i , table);
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
