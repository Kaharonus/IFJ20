//
// Created by tomas on 19.11.20.
//

#include "syntax_analyzer.h"


bool is_type(lex_token* t){
    return t->keyword_value == INT_KEYWORD || t->keyword_value == FLOAT64_KEYWORD || t->keyword_value == STRING_KEYWORD;
}

void check_block(tree_node *tree, scanner *s, symbol_table **table);

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

void set_type(lex_token t, tree_node* node){
    node->type = VALUE;
    switch (t.type) {
        case INT:
            node->value_type = TYPE_INT;
            node->number_value.i = t.number_value.i;
            return;
        case FLOAT:
            node->value_type = TYPE_FLOAT;
            node->number_value.d = t.number_value.d;
            return;
        case STRING:
            node->value_type = TYPE_STRING;
            node->string_value = t.string_value;
            return;
        default:
            throw_err(SA_ERR);
    }
}


int get_precedence(lex_token_type type){
    switch(type){
        case ADD:
        case SUB:
            return 2;
        case MUL:
        case DIV:
            return 1;
        default:
            throw_err(SA_ERR);
    }
    return -1;
}

bool is_expression_id(lex_token t) {
    return t.type == ID || t.type == INT || t.type == FLOAT || t.type == STRING;
}

bool is_expression_symbol(lex_token t){
    lex_token_type type = t.type;
    return is_expression_id(t) || type == OPEN_PARENTHESIS || type == CLOSE_PARENTHESIS
    || type == ADD || type == SUB || type == MUL || type == DIV;
}

void add_node(expression_stack* ptr, lex_token t){
    tree_node * left = expression_stack_pop(ptr).node;
    tree_node * right = expression_stack_pop(ptr).node;
    tree_node * new = create_node();
    switch (t.type) {
        case ADD:
            new->type = ADDITION;
            break;
        case MUL:
            new->type = MULTIPLICATION;
            break;
        case SUB:
            new->type = SUBTRACTION;
            break;
        case DIV:
            new->type = DIVISION;
            break;
    }
    insert_node(new, right);
    insert_node(new, left);
    expression_stack_data data;
    data.node = new;
    expression_stack_push(ptr, data);
};


int compare_precedence(lex_token t1, lex_token t2){
    int t1p = get_precedence(t1.type);
    int t2p = get_precedence(t2.type);
    if(t1p > t2p){
        return 1;
    }else if(t2p > t1p){
        return -1;
    }
    return 0;
}

void check_expression(tree_node * tree, scanner *s){
    lex_token t = get_next_token(s);
    bool expectId = true;
    expression_stack* operators = expression_stack_init();
    expression_stack* operands = expression_stack_init();
    while(is_expression_symbol(t)){
        expression_stack_data data;
        switch(t.type){
            case OPEN_PARENTHESIS:;
                data.t = t;
                expression_stack_push(operators, data);
                break;
            case CLOSE_PARENTHESIS:;
                bool succ = false;
                while(!expression_stack_empty(operators)) {
                    expression_stack_data data = expression_stack_pop(operators);
                    if(data.t.type == OPEN_PARENTHESIS){
                        succ = true;
                        break;
                    }
                    add_node(operands, t);
                }
                if(!succ){
                    throw_err(SA_ERR);
                }
                break;
            default:
                if(!is_expression_id(t)){
                    expression_stack_data second;
                    while(!expression_stack_empty(operators)){
                        second = expression_stack_top(operators);
                        if(second.t.type != OPEN_PARENTHESIS && compare_precedence(t, second.t) >= 0){
                            printf(" ");
                            expression_stack_pop(operators);
                            add_node(operands, second.t);
                        }else{
                            break;
                        }
                    }
                    data.t = t;
                    expression_stack_push(operators, data);

                }else{
                    tree_node * node = create_node();
                    node->type = VALUE;
                    set_type(t, node);
                    data.node = node;
                    expression_stack_push(operands, data);
                }
        }
        t = get_next_token(s);
    }
    while(!expression_stack_empty(operators)) {
        add_node(operands, expression_stack_pop(operators).t);
    }
    unget_token(s, t);
    insert_node(tree, expression_stack_pop(operands).node);
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

void check_for(tree_node *tree, scanner *s, symbol_table **table){
    //get next token for
    // check d
    //get next token středník

    tree_node * node = create_node()
    node->type = FOR_LOOP;
    insert_node(tree, node);

    lex_token t = get_next_token(s);
    if (t.type != COLLON){
        throw_err(SA_ERR);
    }

    t = get_next_token(s);
    
    switch(t){
        case GREATER:
            node->type = CONDITION;
            node->cond_type = OP_GR;
            insert_node(tree, node);
        case LESSER:
            node->type = CONDITION;
            node->cond_type = OP_LS;
            insert_node(tree, node);
        case GREATER_OR_EQ:
            node->type = CONDITION;
            node->cond_type = OP_GREQ;
            insert_node(tree, node);
        case LESSER_OR_EQ:
            node->type = CONDITION;
            node->cond_type = OP_LSEQ;
            insert_node(tree, node);
        case EQ:
            node->type = CONDITION;
            node->cond_type = OP_EQ;
            insert_node(tree, node);
        case NOT_EQ:
            node->type = CONDITION;
            node->cond_type = OP_NEQ;
            insert_node(tree, node);
        default:
            throw_err(SA_ERR);
    }

    t = get_next_token(s);
    if (t.type != COLLON){
        throw_err(SA_ERR);
    }

    t = get_next_token(s);
    if (t.type == ASSIGN){
        node->cond_type = OP_NONE;
        node->type = ASSIGNMENT;
        insert_node(tree, node);
    }
    else{
        throw_err(SA_ERR);
    }

    t = get_next_token(s);

    if (t.type == OPEN_BRACKET){
        check_block(tree, s, table);
    }
    else{
        throw_err(SA_ERR);
    }
    
    t = get_next_token(s);

    if (t.type == CLOSE_BRACKET){
        check_block(tree, s, table);
    }
    else{
        throw_err(SA_ERR);
    }

}

void check_block(tree_node *tree, scanner *s, symbol_table **table) {
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
