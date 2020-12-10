//
// Created by tomas on 07.12.20.
//

#ifndef PROJECT_SEMANTIC_ANALYZER_H
#define PROJECT_SEMANTIC_ANALYZER_H


#include "syntax_tree.h"

void check_semantics(tree_node* node, symbol_table** table);

#endif //PROJECT_SEMANTIC_ANALYZER_H
