//
// Created by tomas on 08.12.20.
//

#ifndef PROJECT_GARBAGE_COLLECTOR_H
#define PROJECT_GARBAGE_COLLECTOR_H

#include <stdlib.h>
#include "error.h"

void init_gc();

void add_to_gc(void* ptr);

void update_gc(void* old, void* new);

void free_gc();

#endif //PROJECT_GARBAGE_COLLECTOR_H
