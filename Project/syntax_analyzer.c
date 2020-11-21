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

void check_block(tree_node *tree, scanner *s, symbol_table **table) {
    lex_token t = get_next_token(s);
    if (t.type != OPEN_BRACKET) {
        throw_err(SA_ERR);
    }
    while (t.type != CLOSE_BRACKET) {
        t = get_next_token(s);
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
    check_block(node,s,sym->scope_table);

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
