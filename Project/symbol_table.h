//
// Created by tomas on 19.11.20.
//

#ifndef PROJECT_SYMBOL_TABLE_H
#define PROJECT_SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define TABLE_SIZE 97


typedef enum {
    TYPE_NONE,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_FUNCTION
}symbol_type;

struct argument{
    char* id;
    symbol_type  type;
    struct argument* nextArg;
};

typedef struct sym_table_struct{
    bool has_been_defined;
    char* id;
    symbol_type type;
    int arg_count;
    struct argument* args;
    int ret_type_count;
    struct argument* return_types;
    struct sym_table_struct* ptrNext;

    struct sym_table_struct** scope_table;
}symbol_table;

symbol_table ** create_ht();
unsigned int ht_calc_hash(const char *str);

void print_ht(symbol_table ** table);

void insert_ht(symbol_table** hashTable, symbol_table* ptr);

symbol_table* search_ht(symbol_table** hashTable, char* id);

symbol_table* create_ht_item(char* id);

void add_ret_type(symbol_table* item,symbol_type type);

void add_arg(symbol_table* item,symbol_type type, char *id);

#endif //PROJECT_SYMBOL_TABLE_H
