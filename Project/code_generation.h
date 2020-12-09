//
// Created by tomas on 09.12.20.
//

#include "syntax_tree.h"
#include <stdarg.h>

#ifndef PROJECT_CODE_GENERATION_H
#define PROJECT_CODE_GENERATION_H

void generate_code(tree_node* node, symbol_table** table);

#endif //PROJECT_CODE_GENERATION_H
