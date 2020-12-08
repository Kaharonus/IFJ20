//
// Created by tomas on 19.11.20.
//

#include "syntax_analyzer.h"



void check_block(tree_node *tree, scanner *s, symbol_table **table);
void check_function_call(tree_node *tree, scanner *s, symbol_table **table);

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

bool is_type(lex_token* t){
    return t->keyword_value == INT_KEYWORD || t->keyword_value == FLOAT64_KEYWORD || t->keyword_value == STRING_KEYWORD;
}

void set_type(lex_token t, tree_node* node, symbol_table** table){
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
        case ID:
            node->type = IDENTIFICATOR;
            symbol_table * item = search_ht(table, t.string_value);
            if(item == NULL){
                throw_err(SEM_ERR);
            }
            node->value_type = item->type;
            node->symbol = item;
            node->string_value = t.string_value;
            break;
        default:
            throw_err(SA_ERR);
    }
}

bool is_expression_id(lex_token t) {
    return t.type == ID || t.type == INT || t.type == FLOAT || t.type == STRING;
}

bool is_expression_symbol(lex_token t){
    lex_token_type type = t.type;
    return is_expression_id(t) || type == OPEN_PARENTHESIS || type == CLOSE_PARENTHESIS
    || type == ADD || type == SUB || type == MUL || type == DIV;
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

void add_node(expression_stack* ptr, lex_token t){
    bool err;
    tree_node * left = expression_stack_pop(ptr, &err).node;
    if(err){
        throw_err(SA_ERR);
    }
    tree_node * right = expression_stack_pop(ptr, &err).node;
    if(err){
        throw_err(SA_ERR);
    }
    tree_node * new = create_node();
    if(right->value_type != left->value_type){
        if(right->value_type == TYPE_STRING || left->value_type == TYPE_STRING){
            throw_err(TYPE_ERR);
        }
        tree_node* type_node = create_node();
        new->value_type = TYPE_FLOAT;
        type_node->type = INT_TO_FLOAT;
        type_node->value_type = TYPE_FLOAT;
        if(left->value_type == TYPE_INT && right->value_type == TYPE_FLOAT){ //Left side of the expression is int, right is float
            insert_node(new, right);
            insert_node(type_node, left);
            insert_node(new, type_node);
        }else if(right->value_type == TYPE_INT && left->value_type == TYPE_FLOAT){ //reverse of the previous one
            insert_node(type_node, right);
            insert_node(new, type_node);
            insert_node(new, left);
        }
    }else{
        new->value_type = left->value_type;
        insert_node(new, right);
        insert_node(new, left);
    }

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
            if(right->value_type == TYPE_FLOAT || left->value_type == TYPE_FLOAT) {
                new->type = DIVISION;
            }else{
                new->type = INT_DIVISION;
            }
            break;
    }

    expression_stack_data data;
    data.node = new;
    expression_stack_push(ptr, data);
}

symbol_type check_expression(tree_node * tree, scanner *s, symbol_table** table){
    lex_token t = get_next_token(s);
    expression_stack* operators = expression_stack_init();
    expression_stack* operands = expression_stack_init();
    symbol_type result;
    while(is_expression_symbol(t)){
        expression_stack_data data;
        bool err = false;
        switch(t.type){
            case OPEN_PARENTHESIS:;
                data.t = t;
                expression_stack_push(operators, data);
                break;
            case CLOSE_PARENTHESIS:;
                bool succ = false;
                while(!expression_stack_empty(operators)) {
                    data = expression_stack_pop(operators, &err);
                    if(err){
                        throw_err(SA_ERR);
                    }
                    if(data.t.type == OPEN_PARENTHESIS){
                        succ = true;
                        break;
                    }
                    add_node(operands, data.t);
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
                            expression_stack_pop(operators, &err);
                            if(err){
                                throw_err(SA_ERR);
                            }
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
                    set_type(t, node, table);
                    data.node = node;
                    expression_stack_push(operands, data);
                }
        }
        t = get_next_token(s);
    }
    bool err = false;
    while(!expression_stack_empty(operators)) {
        add_node(operands, expression_stack_pop(operators, &err).t);
        if(err){
            throw_err(SA_ERR);
        }
    }

    unget_token(s, t); //Return the last token back
    tree_node * result_node = expression_stack_pop(operands, &err).node;
    if(err){
        throw_err(SA_ERR);
    }
    if(!expression_stack_empty(operands) || !expression_stack_empty(operators)){
        throw_err(SA_ERR);
    }

    tree_node * expression_node = create_node();
    expression_node->type = EXPRESSION;
    expression_node->value_type = result_node->value_type;
    insert_node(expression_node, result_node);
    insert_node(tree, expression_node);
    return result_node->value_type;
}

void check_basic_assignment(tree_node *tree, scanner *s, symbol_table **table){
    lex_token t = get_next_token(s);
    if(!(t.type == ID || t.type == DISCARD)){
        throw_err(SA_ERR);
    }
    tree_node * base_node = create_node();
    base_node->type = ASSIGNMENT;

    tree_node * left = create_node();
    left->type = ASSIGN_LEFT;

    tree_node *left_value = create_node();
    if(t.type == DISCARD){
        left_value->type = DISCARD_VALUE;
        left_value->value_type = TYPE_NONE;
    }else{
        left_value->type = IDENTIFICATOR;
        left_value->string_value = t.string_value;
        symbol_table * tab = search_ht(table, t.string_value);
        if(tab == NULL){
            throw_err(SEM_ERR);
        }
        left_value->symbol = tab;
        left_value->value_type = left_value->symbol->type;
    }

    insert_node(left, left_value);
    insert_node(base_node, left);

    t = get_next_token(s);
    if(t.type != ASSIGN){
        throw_err(SA_ERR);
    }
    tree_node * right = create_node();
    insert_node(tree, base_node);
    insert_node(base_node, right);

    right->type = ASSIGN_RIGHT;
    t = get_next_token(s);
    lex_token t_next = get_next_token(s);
    unget_token(s, t_next);
    unget_token(s, t);
    if(t.type == ID && t_next.type == OPEN_PARENTHESIS){
        check_function_call(right, s, table);
        return;
    }
    symbol_type expression_type = check_expression(right, s, table);
    if(expression_type != left_value->value_type){
        if(left_value->type == DISCARD_VALUE){
            insert_node(tree, base_node);
            return;
        }
        if(expression_type == TYPE_STRING || left_value->symbol->type == TYPE_STRING){ //If just one of them is string, and the other one isn't its an error
            throw_err(TYPE_ERR);
        }
        //Here conversion from int to double or the other way around has to happen at the end of the expression
        tree_node * type_node = create_node();
        insert_node(type_node, right->nodes[0]->nodes[0]);
        remove_last_node(right->nodes[0]);
        insert_node(right->nodes[0], type_node);
        if(expression_type == FLOAT){
            type_node->type = FLOAT_TO_INT;
        }else{
            type_node->type = INT_TO_FLOAT;
        }
    }
}

void check_assignment(tree_node *tree, scanner *s, symbol_table **table){
    lex_token t;
    tree_node * base_node = create_node();
    base_node->type = ASSIGNMENT;
    insert_node(tree, base_node);
    tree_node * left = create_node();
    left->type = ASSIGN_LEFT;
    insert_node(base_node, left);
    while(true){
        t = get_next_token(s);
        tree_node* id = create_node();
        if(t.type == DISCARD){
            id->type = DISCARD_VALUE;
            id->value_type = TYPE_NONE;
        }else if(t.type == ID){
            symbol_table * item = search_ht(table, t.string_value);
            if(item == NULL){
                throw_err(SEM_ERR);
            }
            id->value_type = item->type;
            id->type = IDENTIFICATOR;
            id->string_value = t.string_value;
            id->symbol = item;
        }else{
            throw_err(SA_ERR);
        }

        insert_node(left,id);
        t = get_next_token(s);
        if(t.type != COMMA){
            if(t.type == ASSIGN){
                break;
            }
            throw_err(SA_ERR);
        }
    }
    tree_node * right = create_node();
    right->type = ASSIGN_RIGHT;
    insert_node(base_node, right);
    t = get_next_token(s);
    lex_token t_next = get_next_token(s);
    unget_token(s, t_next);
    unget_token(s, t);
    if(t.type == ID && t_next.type == OPEN_PARENTHESIS){
        check_function_call(right, s, table);
        return;
    }
    while(true){
        check_expression(right, s, table);
        t = get_next_token(s);
        if(t.type != COMMA){
            if(t.type == END_OF_LINE){
                break;
            }
            throw_err(SA_ERR);
        }
    }
    if(left->subnode_len != right->subnode_len){
        throw_err(SA_ERR);
    }
    for(int i = 0; i < left->subnode_len; i++){
        tree_node* left_node = left->nodes[i];
        tree_node* right_node = right->nodes[i];
        if(left_node->value_type == right_node->value_type || left_node->type == DISCARD_VALUE){
            continue;
        }
        if(right_node->value_type == TYPE_STRING || left_node->value_type == TYPE_STRING){ //If just one of them is string, and the other one isn't its an error
            throw_err(TYPE_ERR);
        }
        //Here conversion from int to double or the other way around has to happen at the end of the expression
        tree_node * type_node = create_node();
        if(left_node->value_type == FLOAT){
            type_node->type = FLOAT_TO_INT;
        }else{
            type_node->type = INT_TO_FLOAT;
        }
        insert_node(type_node, right_node->nodes[0]);
        remove_last_node(right_node);
        insert_node(right_node, type_node);
    }
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
    node->value_type = check_expression(node, s, table);
    item->type = node->value_type;
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

void chceck_conditon(tree_node *tree, scanner *s){

    lex_token t = get_next_token(s);
    tree_node * node = create_node();

    if (t.type != ID && t.type != INT){
        throw_err(SA_ERR);
    }

    t = get_next_token(s);

    lex_token t_next = get_next_token(s);

    if (t_next.type != ID && t_next.type != INT){
        throw_err(SA_ERR);
    }

    switch(t.type){
        case GREATER:
            node->type = CONDITION;
            node->cond_type = OP_GR;
            insert_node(tree, node);
            break;
        case LESSER:
            node->type = CONDITION;
            node->cond_type = OP_LS;
            insert_node(tree, node);
            break;
        case GREATER_OR_EQ:
            node->type = CONDITION;
            node->cond_type = OP_GREQ;
            insert_node(tree, node);
            break;
        case LESSER_OR_EQ:
            node->type = CONDITION;
            node->cond_type = OP_LSEQ;
            insert_node(tree, node);
            break;
        case EQ:
            node->type = CONDITION;
            node->cond_type = OP_EQ;
            insert_node(tree, node);
            break;
        case NOT_EQ:
            node->type = CONDITION;
            node->cond_type = OP_NEQ;
            insert_node(tree, node);
            break;
        default:
            throw_err(SA_ERR);
    }

    return;
}

void check_for(tree_node *tree, scanner *s, symbol_table **table){

    lex_token t = get_next_token(s);

    tree_node * for_node = create_node();
    for_node->type = FOR_LOOP;
    insert_node(tree, for_node);

    t = get_next_token(s);

    if (t.type != COLLON){
        check_expression(for_node,s);
        if((t = get_next_token(s)).type != COLLON){
            throw_err(SA_ERR);
        }
    }
    
    chceck_conditon(for_node, s);

    t = get_next_token(s);

    if (t.type != COLLON){
        throw_err(SA_ERR);
    }

    t = get_next_token(s);

    if (t.type != OPEN_BRACKET){
        check_expression(for_node, s);
    }
    else{
        unget_token(s, t);
    }

    check_block(for_node, s, table);

    return;
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
                        check_for(tree,s,table);
                        break;
                    case IF:
                        //check_assignment(tree,s,table);
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
