//
// Created by tomas on 19.11.20.
//

#include "symtable.h"

symbol_table **create_ht() {
    symbol_table** ptr = (symbol_table**)malloc(TABLE_SIZE* sizeof(symbol_table *));
    for(int i = 0; i < TABLE_SIZE; i++){
        ptr[i] = NULL;
    }
    return ptr;
}

symbol_table *create_ht_item(char *id) {
    symbol_table* ptr = (symbol_table*)malloc(sizeof(symbol_table));
    if(ptr == NULL)
        return NULL;
    ptr->has_been_defined = false;
    ptr->ptrNext = NULL;
    ptr->id = id;
    ptr->return_types = malloc(sizeof(struct argument));
    ptr->ret_type_count = 0;
    ptr->args = malloc(sizeof(struct argument));
    ptr->arg_count = 0;
    ptr->type = TYPE_NONE;
    return ptr;
}


unsigned int ht_calc_hash(const char *str) {
    uint32_t h=0;     // musí mít 32 bitů
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % TABLE_SIZE;
}

void print_ht(symbol_table **table) {
    for(int i = 0; i < TABLE_SIZE; i++){
        symbol_table *item = table[i];
        if(item == NULL){
            continue;
        }
        fprintf(stderr,"%d.  ", i+1);
        symbol_table* prev = NULL;
        while(item != NULL){
            fprintf(stderr,"%s ->", item->id);
            if(prev == item){
                break;
            }
            prev = item;
            item = item->ptrNext;
        }
        fprintf(stderr,"\n");
    }
}

void insert_ht(symbol_table **hashTable, symbol_table *ptr) {
    if(ptr != NULL){
        symbol_table* temp = search_ht(hashTable, ptr->id);
        if(temp != NULL){
            return;
        }
        unsigned hash = ht_calc_hash(ptr->id);
        if(&hashTable[hash] != NULL){
            ptr->ptrNext = hashTable[hash];
        }
        hashTable[hash] = ptr;
    }
}

symbol_table *search_ht(symbol_table **hashTable, char *id) {
    if(id == NULL){
        return NULL;
    }
    char* searchedID = id;

    unsigned hash = ht_calc_hash(searchedID);
    symbol_table* ptr = hashTable[hash];
    if(ptr == NULL)
        return NULL;
    while(strcmp(ptr->id, searchedID)){
        if(ptr->ptrNext == NULL)
            return NULL;
        ptr = ptr->ptrNext;
    }
    return ptr;
}

void add_arg(symbol_table *item, symbol_type type, char *id) {
    struct argument* loc = item->args;
    for(int i = 0; i < item->arg_count; i++){
        loc = item->args->nextArg;
    }
    loc->nextArg = malloc(sizeof(struct argument));
    loc->id = id;
    loc->type = type;
    item->arg_count++;

}

void add_ret_type(symbol_table *item, symbol_type type) {
    struct argument* loc = item->return_types;
    for(int i = 0; i < item->ret_type_count; i++){
        loc = item->return_types->nextArg;
    }
    loc->nextArg = malloc(sizeof(struct argument));
    loc->type = type;
    item->ret_type_count++;
}
