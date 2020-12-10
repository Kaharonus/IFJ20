//
// Created by tomas on 08.12.20.
//

#include "garbage_collector.h"


int gc_size;
int gc_index;
void ** gc_ptr;

void add_to_gc(void *ptr) {
    if(gc_index >= gc_size){
        gc_size += 1024;
        gc_ptr = realloc(gc_ptr,sizeof(void*) * gc_size);
    }
    gc_ptr[gc_index] = ptr;
    gc_index++;
}

void update_gc(void *old, void *new) {
    for(int i = 0; i < gc_index; i++){
        if(gc_ptr[i] == old){
            gc_ptr[i] = new;
            break;
        }
    }
}

void free_gc() {
    for(int i = 0; i < gc_index; i++){
        free(gc_ptr[i]);
    }
    free(gc_ptr);
}

void init_gc() {
    gc_size = 2048;
    gc_index = 0;
    gc_ptr = malloc(sizeof(void*) * gc_size);
    if(gc_ptr == NULL){
        throw_err(INTERN_ERR);
    }
}
