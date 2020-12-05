
#include "lex_analyzer.h"
#include <math.h>

bool is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_num_char(char c) {
    return (c >= '0' && c<= '9');
}

bool is_ident_char(char c) {
    return is_letter(c) || is_num_char(c) || c == '_';
}

int read_next_word(scanner * source, char* word, int size){
    if(word == NULL){
        throw_err(INTERN_ERR);
    }
    int counter = 0;
    char c = 0;
    while(is_ident_char(c = (char)getc(source->source))){
        if(counter-1 == size){
            size += 128;
            char *t = word;
            word = realloc(word , size*sizeof(char));
            if(word == NULL){
                throw_err(INTERN_ERR);
            }
        }
        word[counter] = c;
        counter++;
    }
    ungetc(c, source->source);
    return counter;
}


void handle_comments(scanner* sc){
    char secondChar =  (char)getc(sc->source);

    if (secondChar == '/') {
        char checkChar = (char)getc(sc->source);
        while (checkChar != '\n') {
            checkChar = (char)getc(sc->source);
        }
    } else if (secondChar == '*'){
        char checkChar;
        char checkChar2;
        do{
            checkChar = (char)getc(sc->source);
            checkChar2 = (char)getc(sc->source);
        }while (checkChar != '*' || checkChar2 != '/');
    }

}

void handle_word(scanner* sc, lex_token* t){
    char* word = calloc(256, sizeof(char));
    if(word == NULL){
        throw_err(INTERN_ERR);
    }
    read_next_word(sc, word, 256);
    t->keyword_value = get_keyword(word);
    if(t->keyword_value != NOT_KEYWORD){
        t->type = KEYWORD;
    }else{
        t->type = ID;
        t->string_value = word;
    }
}

void handle_number(scanner* sc, lex_token* t){

    double num = 0;

    num = 0;
    char c = (char)getc(sc->source);
    bool IsInt = true;
    while(c >= '0' && c <= '9'){
        num = (num * 10) + atoi(&c);
        c = (char)getc(sc->source);
    }

    if(c == '.'){
        IsInt = false;
        double i = 1;
        while((c = (char)getc(sc->source)) >= '0' && c <= '9'){
            num = num + (atoi(&c) * (pow(0.1, i)));
            i++;
        }
    }

    if(IsInt){
        t->number_value.i = (int)(num + 0.5);
        t->type = INT;
    }
    else{
        t->number_value.d = num;
        t->type = FLOAT;
    }
}

char* get_string_literal(scanner* sc, lex_token* t){
    char* strLiteral = calloc(256, sizeof(char));
    int i = 0;
    if (strLiteral == NULL){
        throw_err(INTERN_ERR);
    }
    int first;
    int second;
    int res;
    t->string_value = strLiteral;

    while(true){
        char c = (char)getc(sc->source);
        if (c == '\"'){
            return strLiteral;
        }
        if (c == '\\'){
            c = (char)getc(sc->source);
            switch(c){
                case '\"':
                    strLiteral[i] = '\"';
                    i++;
                    break;
                case '\n':
                    strLiteral[i] = '\n';
                    i++;
                    break;
                case '\t':
                    strLiteral[i] = '\t';
                    i++;
                    break;
                case '\\':
                    strLiteral[i] = '\\';
                    i++;
                    break;
                case 'x':
                    first = c/16 - 3;
                    second = c % 16;
                    res = (first*10)+second;
                    strLiteral[i] = (char)res;
                    break;
                default:
                    throw_err(LA_ERR);
                    break;
            }
        }else{
            strLiteral[i] = c;
            i++;
        }
    }
}


void handle_comparison(scanner* sc, lex_token* t, char c){
    char next = (char)getc(sc->source);
    switch(c){
        case ':':
            if(next == '='){
                t->type = VAR_DEF;
            }
            break;
        case '=':
            if(next != '='){
                t->type = ASSIGN;
                break;
            }
            t->type = EQ;
            break;
        case '<':
            if(next == '='){
                t->type = LESSER_OR_EQ;
                break;
            }
            t->type = LESSER;
            break;
        case '>':
            if(next == '='){
                t->type = GREATER_OR_EQ;
                break;
            }
            t->type = GREATER;
            break;
        case '!':
            if(next != '='){
                break;
            }
            t->type = NOT_EQ;
            break;
    }
}



bool token_stack_empty(token_stack* ts) {
    return ts->index == 0;
}

lex_token token_stack_pop(token_stack* ts) {
    if (!token_stack_empty(ts)) {
        ts->index--;
    }
    return ts->data[ts->index+1];
}
void token_stack_push(token_stack * ts, lex_token t) {
    ts->index++;
    ts->data[ts->index] = t;
}



lex_token get_next_token(scanner* sc){
    if(!token_stack_empty(&sc->stack)){
        return token_stack_pop(&sc->stack);
    }
    lex_token t;
    t.type = ERROR;
    t.keyword_value = NOT_KEYWORD;
    t.string_value = NULL;
    t.number_value.i = t.number_value.d = 0;
    char next;
    while(true){
        char c = (char)getc(sc->source);
        switch(c){
            case '"':
                get_string_literal(sc, &t);
                t.type = STRING;
                return t;
            case '\n':
                t.type = END_OF_LINE;
                return t;
            case ',':
                t.type = COMMA;
                return t;
            case ';':
                t.type = COLLON;
                return t;
            case '(':
                t.type = OPEN_PARENTHESIS;
                return t;
            case ')':
                t.type = CLOSE_PARENTHESIS;
                return t;
            case '{':
                t.type = OPEN_BRACKET;
                sc->indent++;
                return t;
            case '}':
                t.type = CLOSE_BRACKET;
                sc->indent--;
                return t;
            case '+':
                t.type = ADD;
                return t;
            case '-':
                t.type = SUB;
                return t;
            case '/':
                next = (char)getc(sc->source);
                if(next == '/' || next == '*'){
                    ungetc(next, sc->source);
                    handle_comments(sc);
                    break;
                }
                t.type = DIV;
                return t;
            case '*':
                t.type = MUL;
                return t;
            case ':':
            case '=':
            case '<':
            case '>':
            case '!':
                handle_comparison(sc,&t,c);
                return t;
            case EOF:
                t.type = END_OF_FILE;
                return t;
            default:
                if(is_letter(c) || c == '_'){
                    ungetc(c,sc->source);
                    handle_word(sc, &t);
                    if(t.type == ID && strcmp(t.string_value, "_") == 0){
                        t.type = DISCARD;
                    }
                    return t;
                } else if (is_num_char(c)) {
                    ungetc(c,sc->source);
                    handle_number(sc, &t);
                    return t;
                }
        }

    }
}

void unget_token(scanner *sc, lex_token t) {
    token_stack_push(&sc->stack, t);
}
