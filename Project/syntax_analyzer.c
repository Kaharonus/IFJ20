//
// Created by tomas on 19.11.20.
//

#include "syntax_analyzer.h"


bool is_type(lex_token* t){
    return t->keyword_value == INT_KEYWORD || t->keyword_value == FLOAT64_KEYWORD || t->keyword_value == STRING_KEYWORD;
}

symbol_type translate_keyword(keyword_type kw){
    switch (kw) {
        case INT_KEYWORD:
            return TYPE_INT;
        case FLOAT64_KEYWORD:
            return TYPE_FLOAT;
        case STRING_KEYWORD:
            return TYPE_STRING;
        default:
            throw_err(SA_ERR);
    }
    return TYPE_NONE;
}

symbol_type translate_type(lex_token_type kw){
    switch (kw) {
        case INT:
            return TYPE_INT;
        case FLOAT:
            return TYPE_FLOAT;
        case STRING:
            return TYPE_STRING;
        default:
            throw_err(SA_ERR);
    }
    return TYPE_NONE;
}


const char parse_table[8][8] = {
        //            +    -    *    /    (    )   ID   end
        /* + */     {'>', '>', '<', '<', '>', '<', '>', '>'},
        /* - */     {'>', '>', '<', '<', '<', '>', '<', '>'},
        /* * */     {'>', '>', '>', ' ', '<', '>', '<', '>'},
        /* / */     {'>', '>', ' ', '>', '<', '>', '<', '>'},
        /* ( */     {'<', '<', '<', '<', '<', '=', '<', ' '},
        /* ) */     {'>', '>', '>', '>', ' ', '>', ' ', '>'},
        /* ID */    {'>', '>', '>', '>', ' ', '>', ' ', '>'},
        /* end */   {'<', '<', '<', '<', '<', ' ', '<', ' '},
};


void check_expression(tree_node * tree, scanner *s){
    lex_token t = get_next_token(s);
}

void check_basic_assignment(tree_node *tree, scanner *s, symbol_table **table){

}

void check_assignment(tree_node *tree, scanner *s, symbol_table **table){

}

void check_variable_definition(tree_node *tree, scanner *s, symbol_table **table){
    lex_token t = get_next_token(s);
    if(t.type != ID){
        throw_err(SA_ERR);
    }
    symbol_table * item = create_ht_item(t.string_value);
    insert_ht(table, item);
    t = get_next_token(s);
    if(t.type != VAR_DEF){
        throw_err(SA_ERR);
    }

    tree_node* node = create_node();
    insert_node(tree, node);
    node->type = VAR_DEFINITION;
    node->string_value = item->id;
    check_expression(node, s);
}

void check_return(tree_node *tree, scanner *s, symbol_table **table){

}

void check_function_call(tree_node *tree, scanner *s, symbol_table **table){
    lex_token token = get_next_token(s);
    tree_node* node = create_node();
    insert_node(tree, node);
    node->type = FUNCTION_CALL;
    node->string_value = token.string_value;
    token = get_next_token(s);
    token = get_next_token(s);
    while(token.type != CLOSE_PARENTHESIS){
        tree_node * param = create_node();
        switch(token.type){
            case ID:
                param->type = IDENTIFICATOR;
                param->string_value = token.string_value;
                break;
            case STRING:
                param->type = VALUE;
                param->value_type = TYPE_STRING;
                param->string_value = token.string_value;
                break;
            case INT:
                param->type = VALUE;
                param->value_type = TYPE_INT;
                param->number_value.i = token.number_value.i;
                break;
            case FLOAT:
                param->type = VALUE;
                param->value_type = TYPE_FLOAT;
                param->number_value.d = token.number_value.d;
                break;
            case COMMA:
                token = get_next_token(s);
                continue;
            default:
                throw_err(SA_ERR);
        }
        insert_node(node, param);
        token = get_next_token(s);
    }
}


void check_block(tree_node *tree, scanner *s, symbol_table **table, symbol_table** root) {
    lex_token t = get_next_token(s);
    if (t.type != OPEN_BRACKET) {
        throw_err(SA_ERR);
    }
    while (t.type != CLOSE_BRACKET) {
        t = get_next_token(s);
        switch (t.type) {
            case DISCARD:
            case ID:;
                lex_token nextID = get_next_token(s);
                unget_token(s,nextID);
                unget_token(s,t);
                switch (nextID.type) {
                    case ASSIGN:
                        check_basic_assignment(tree,s,table);
                        break;
                    case COMMA:
                        check_assignment(tree,s,table);
                        break;
                    case VAR_DEF:
                        check_variable_definition(tree,s,table);
                        break;
                    case OPEN_PARENTHESIS:
                        check_function_call(tree,s,table);
                        break;
                    default:
                        throw_err(SA_ERR);
                }
                break;
            case KEYWORD:;
                unget_token(s,t);
                switch (t.keyword_value) {
                    case FOR:
                        check_basic_assignment(tree,s,table);
                        break;
                    case IF:
                        check_assignment(tree,s,table);
                        break;
                    case RETURN:
                        check_return(tree,s,table);
                        break;
                    default:
                        throw_err(SA_ERR);
                }
                break;
            case END_OF_LINE:
                continue;
            case CLOSE_BRACKET:
                return;
            default:
                throw_err(SA_ERR);
        }

    }

}



void handle_fn_args(scanner*s, symbol_table * symbol){
    symbol_table ** table = symbol->scope_table;
    lex_token token1 = get_next_token(s);
    lex_token token2 = get_next_token(s);
    lex_token token3 = get_next_token(s);
    symbol_table *var;
    while(token3.type != CLOSE_PARENTHESIS){
        if(token1.type != ID || (token2.type != KEYWORD && !is_type(&token2)) || token3.type != COMMA ){
            throw_err(SA_ERR);
        }
        var = create_ht_item(token1.string_value);
        var->type = translate_keyword(token2.keyword_value);
        add_arg(symbol, var->type, var->id);
        insert_ht(table, var);
        token1 = get_next_token(s);
        token2 = get_next_token(s);
        token3 = get_next_token(s);
    }
    if(token1.type != ID || (token2.type != KEYWORD && !is_type(&token2))){
        throw_err(SA_ERR);
    }
    var = create_ht_item(token1.string_value);
    var->type = translate_keyword(token2.keyword_value);
    add_arg(symbol, var->type, var->id);
    insert_ht(table, var);
}

void handle_fn_ret_types(scanner*s, symbol_table * symbol){
    lex_token token1 = get_next_token(s);
    lex_token token2 = get_next_token(s);
    while(token2.type != CLOSE_PARENTHESIS){
        if((token1.type != KEYWORD && !is_type(&token1)) || token2.type != COMMA ){
            throw_err(SA_ERR);
        }
        add_ret_type(symbol, translate_keyword(token1.keyword_value));
        token1 = get_next_token(s);
        token2 = get_next_token(s);
    }
    if(token1.type != KEYWORD && !is_type(&token1)){
        throw_err(SA_ERR);
    }
    add_ret_type(symbol, translate_keyword(token1.keyword_value));
}

void check_function_definition(tree_node *tree, scanner *s, symbol_table **table) {
    tree_node* node = create_node();
    node->type = FUNCTION_DEFINITION;

    lex_token token = get_next_token(s);
    if(token.type != ID){
        throw_err(SA_ERR);
    }
    symbol_table* sym = create_ht_item(token.string_value);
    insert_ht(table, sym);
    sym->type = TYPE_FUNCTION;
    token = get_next_token(s);
    if(token.type != OPEN_PARENTHESIS){
        throw_err(SA_ERR);
    }
    token = get_next_token(s);
    sym->scope_table = create_ht();
    if(token.type != CLOSE_PARENTHESIS){
        unget_token(s, token);
        handle_fn_args(s, sym);
    }
    token = get_next_token(s);
    if(token.type != OPEN_BRACKET && token.type != OPEN_PARENTHESIS){
       throw_err(SA_ERR);
    }
    if(token.type != OPEN_BRACKET){
        handle_fn_ret_types(s,sym);
        token = get_next_token(s);
        if(token.type != OPEN_BRACKET){
            throw_err(SA_ERR);
        }
    }
    unget_token(s,token);
    insert_node(tree, node);
    check_block(node,s,sym->scope_table, table);

}

/// Root block must start with certain things.
/// \param tree
/// \param s
/// \param table
/// \return
void check_root_block(tree_node *tree, scanner *s, symbol_table **table) {
    lex_token t = get_next_token(s);
    if (t.type != KEYWORD || t.keyword_value != PACKAGE) {
        throw_err(SA_ERR);
    }
    t = get_next_token(s);
    if (t.type != ID || strcmp(t.string_value, "main") != 0) {
        throw_err(SA_ERR);
    }
    t = get_next_token(s);
    if (t.type != END_OF_LINE) {
        throw_err(SA_ERR);
    }
    while (t.type != END_OF_FILE) {
        do {
            t = get_next_token(s);
        } while (t.type == END_OF_LINE);
        if(t.type == END_OF_FILE){
            return;
        }
        if(t.type != KEYWORD || t.keyword_value != FUNC){
            throw_err(SA_ERR);
        }
        check_function_definition(tree,s,table);
    }
}


tree_node *get_syntax_tree(scanner *sc, symbol_table **table) {
    tree_node *tree = create_node();
    check_root_block(tree, sc, table);
    return tree;
}
