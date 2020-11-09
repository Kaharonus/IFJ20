//
// Created by tomas on 09.11.20.
//

#include "lex_analyzer.h"


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

lex_token get_next_token(scanner* sc){
    lex_token t;
    t.type = ERROR;
    t.keyword_value = NOT_KEYWORD;
    t.string_value = NULL;
    t.number_value.i = t.number_value.d = 0;
    while(true){
        char c = (char)getc(sc->source);
        switch(c){
            case '\n':
                t.type = END_OF_LINE;
                return t;
            case ',':
                t.type = COMMA;
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
                    return t;
                } else if (is_num_char(c)) {
                    ungetc(c,sc->source);
                    handle_number(sc, &t);
                    return t;
                }
        }

    }
}
