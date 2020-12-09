//
// Created by tomas on 09.12.20.
//

#include "code_generation.h"
#include "garbage_collector.h"


char* escape_string(const char* str){
    char* result = malloc(1024*sizeof(char));
    add_to_gc(result);
    int result_size = 1024;
    int index = 0;

    int write_index = 0;
    while(str[index] != 0){
        if(index + 3 > result_size){
            char* tmp = result;
            result_size += 1024;
            result = realloc(result, result_size*sizeof(char));
            update_gc(tmp, result);
        }
        char c = str[index];
        if(!(c == 35 || c == 92 || (c >=0 && c<=32))){
            result[write_index] = c;
            index++;
            write_index++;
            continue;
        }
        int i = (int)c;
        result[write_index] = '\\';
        result[write_index + 1] = '0';
        result[write_index + 2] = (i/10) + '0';
        result[write_index + 3] = (i%10) + '0';

        write_index+=4;
        index++;
    }
    return result;
}

void print_line(const char *text, ...){
    va_list args;
    va_start(args, text);

    vfprintf(stdout, text, args);
    fprintf(stdout, "\n");
}



void generate_non_built_in_call(tree_node * node, symbol_table* item){

}

void generate_print(tree_node* call){
    for(int i = 0; i < call->subnode_len; i++){
        tree_node * node = call->nodes[i];
        if(node->type == IDENTIFICATOR){
            print_line("WRITE LF@%s", node->string_value);
        }else if((node->type == VALUE && node->value_type == TYPE_STRING)){
            print_line("WRITE string@%s", escape_string(node->string_value));
        }else if(node->type == VALUE && node->value_type == TYPE_FLOAT){
            print_line("WRITE float@%a", node->number_value.d);
        }else{
            print_line("WRITE int@%d", node->number_value.i);
        }
    }
}


void generate_input(tree_node * node, char* type){
    node = node->nodes[0];
    if(node->type == DISCARD_VALUE){
        print_line("READ GF@$DISCARD %s", type);
    }else{
        print_line("READ LF@%s %s", node->string_value,type);

    }
}


void generate_function_call(tree_node * node, symbol_table* item){
    char* name = item->id;
    if(strcmp(name, "inputs") == 0){
        generate_input(node, "string");
    }else if(strcmp(name, "inputi") == 0){
        generate_input(node, "int");
    }else if(strcmp(name, "inputf") == 0){
        generate_input(node, "float");
    }else if(strcmp(name, "int2float") == 0){

    }else if(strcmp(name, "float2int") == 0){

    }else if(strcmp(name, "len") == 0){

    }else if(strcmp(name, "print") == 0){
        generate_print(node);
    }else if(strcmp(name, "substr") == 0){

    }else if(strcmp(name, "ord") == 0){

    }else if(strcmp(name, "chr") == 0){

    }else{
        generate_non_built_in_call(node,item);
    }
}


void generate_string_exp(tree_node* tree){
    //NOT ENOUGH TIME :(
}

static char* get_expression_instruction(tree_node_type type){
    switch(type){
        case ADDITION:
            return "ADDS";
        case SUBTRACTION:
            return "SUBS";
        case MULTIPLICATION:
            return "MULS";
        case DIVISION:
            return "DIVS";
        case INT_DIVISION:
            return "IDIVS";
        default: return "";
    }
}

char* get_argument(tree_node* tree){
    char* arr = malloc(sizeof(char)* 512);
    add_to_gc(arr);
    if(tree->type == IDENTIFICATOR){
        strcpy(arr, "LF");
        strcat(arr, "@");
        strcat(arr, tree->symbol->id);
    }else if(tree->value_type == TYPE_STRING){
        strcpy(arr, "string@");
        strcat(arr, tree->string_value);
    }else if(tree->value_type == TYPE_INT){
        strcpy(arr, "int@");
        char tmp[128] = {0};
        sprintf(tmp,"%d",tree->number_value.i);
        strcat(arr, tmp);
    }else{
        strcpy(arr, "float@");
        char tmp[128] = {0};
        sprintf(tmp,"%a",tree->number_value.d);
        strcat(arr, tmp);
    }
    return arr;
}


void generate_exp(tree_node * tree){
    if(tree->value_type == TYPE_STRING){
        generate_string_exp(tree);
    }else if(!(tree->nodes[0]->type == IDENTIFICATOR || tree->nodes[0]->type == VALUE)){
        generate_exp(tree->nodes[0]);
        print_line("PUSHS %s", get_argument(tree->nodes[1]));
        print_line("%s\n", get_expression_instruction(tree->type));

    } else if (!(tree->nodes[1]->type == IDENTIFICATOR || tree->nodes[1]->type == VALUE)){
        generate_exp(tree->nodes[1]);
        print_line("PUSHS %s",get_argument(tree->nodes[0]));
        print_line("%s",get_expression_instruction(tree->type));

    }else{
        print_line("PUSHS %s",get_argument(tree->nodes[0]));
        print_line("PUSHS %s",get_argument(tree->nodes[1]));
        print_line("%s",get_expression_instruction(tree->type));
    }
}


void generate_expression(tree_node* tree){
    tree_node *value = tree->nodes[0];
    if(value->type == IDENTIFICATOR){
        print_line("PUSHS %s", get_argument(tree));
    }else if(value->type == VALUE){
        printf("PUSHS ");
        switch (value->value_type) {
            case TYPE_NONE:
                print_line("nil@nil");
                break;
            case TYPE_FLOAT:
                print_line("float@%a", value->number_value.d);
                break;
            case TYPE_INT:
                print_line("int@%d", value->number_value.i);
                break;
            case TYPE_STRING:
                print_line("string@%s", value->string_value);
                break;
            default:
                // wut
                break;
        }
    }else{
        generate_exp(value);
    }

}

void generate_var_def(tree_node* node){
    print_line("DEFVAR LF@%s", node->string_value);
    generate_expression(node->nodes[0]);
    print_line("POPS LF@%s", node->string_value);
}

void generate_assignment(tree_node* node){
    if(node->nodes[1]->subnode_len == 1 && node->nodes[0]->subnode_len != 1){ //its a me! function call!
        generate_function_call(node->nodes[0], node->nodes[1]->nodes[0]->symbol);
        return;
    }
    for(int i = 0; i < node->nodes[1]->subnode_len; i++){
        generate_expression(node->nodes[1]->nodes[i]);
    }
    for(int i = (int)node->nodes[0]->subnode_len - 1; i >= 0; i--){
        tree_node * n = node->nodes[0]->nodes[i];
        if(n->type == DISCARD_VALUE){
            print_line("POPS GF@$DISCARD");

        }else{
            print_line("POPS LF@%s", n->string_value);

        }

    }

}

void generate_code(tree_node *tree, symbol_table **table) {
    int size = 0;
    tree_node ** items = get_preorder(tree, &size);
    tree_node * node;

    print_line(".IFJcode20\n");
    print_line("DEFVAR GF@$DISCARD");
    print_line("CALL main");
    print_line("EXIT int@0\n");
    tree_node * last;

    unsigned var_count = 0;
    for(int i = 0; i < size; i++){
        node = items[i];
        bool last_item = node == last;
        switch (node->type) {
            case FUNCTION_DEFINITION:
                print_line("LABEL %s", node->symbol->id);
                print_line("CREATEFRAME");
                print_line("PUSHFRAME");
                int last_i = 0;
                last = get_preorder(node, &last_i)[last_i-1];

                break;
            case FUNCTION_CALL:
                if(items[i-1]->type == ASSIGN_RIGHT){
                    break;
                }
                generate_function_call(node, node->symbol);
                break;
            case ASSIGNMENT:
                generate_assignment(node);
                break;
            case VAR_DEFINITION:
                generate_var_def(node);
                break;
            default:
                break;
        }
        if(last_item){
            print_line("POPFRAME");
            print_line("RETURN");
        }
    }
}
