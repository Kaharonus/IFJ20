#include <stdio.h>
#include "lex_analyzer.h"
#include "error.h"
#include "lex_token.h"
#include "syntax_tree.h"
#include "syntax_analyzer.h"
#include "semantic_analyzer.h"
#include "garbage_collector.h"
#include "code_generation.h"


int main(int argc, char *argv[]) {
    scanner scan;
    init_gc();
    scan.source = stdin;
    scan.stack.index = 0;
    symbol_table ** table = create_ht();
    tree_node* tree = get_syntax_tree(&scan, table);
    check_semantics(tree, table);
    generate_code(tree, table);
    //print_tree(tree);
    free_gc();
    return 0;
}