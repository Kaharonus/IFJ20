//
// Created by tomas on 19.11.20.
//

#include "syntax_analyzer.h"

void check_block(tree_node *tree, scanner *s, symbol_table **table) {
    lex_token t = get_next_token(s);
    if (t.type != OPEN_PARENTHESIS) {
        throw_err(SA_ERR);
    }
    while (t.type != CLOSE_PARENTHESIS) {
        t = get_next_token(s);
    }

}

void check_function_definition(tree_node *tree, scanner *s, symbol_table **table) {

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
        if(t.type != KEYWORD || t.keyword_value != FUNC){
            throw_err(SA_ERR);
        }
        check_function_definition(tree,s,table);
    }

}

tree_node **get_syntax_tree(scanner *sc, symbol_table **table) {
    lex_token t;
    tree_node *tree = create_node();
    check_root_block(tree, sc, table);
    /*while ((t = get_next_token(sc)).type != END_OF_FILE) {
        if (t.type == ERROR) {
            throw_err(LA_ERR);
        }
        fprintf(stderr, "TOKEN: %s, %s, %s, %f, %d \n", TOKEN_TYPES[t.type], KEYWORDS[t.keyword_value], t.string_value,
                t.number_value.d, t.number_value.i);
    }*/
    return NULL;
}
