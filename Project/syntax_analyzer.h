//
// Created by tomas on 19.11.20.
//

#ifndef PROJECT_SYNTAX_ANALYZER_H
#define PROJECT_SYNTAX_ANALYZER_H

#include "syntax_tree.h"
#include "lex_analyzer.h"
#include "symbol_table.h"


tree_node* get_syntax_tree(scanner * source, symbol_table** table);

#endif //PROJECT_SYNTAX_ANALYZER_H
