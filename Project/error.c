//
// Created by tomas on 09.11.20.
//

#include "error.h"
#include "garbage_collector.h"

void throw_err(error_type et){
    free_gc();
    exit(et);
}
