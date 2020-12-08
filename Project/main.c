#include <stdio.h>
#include "lex_analyzer.h"
#include "error.h"
#include "lex_token.h"
#include "syntax_tree.h"
#include "syntax_analyzer.h"
#include "semantic_analyzer.h"
#include "garbage_collector.h"

int main(int argc, char *argv[]) {
    scanner scan;
    FILE *f = fopen(argv[1], "r");
    if(f == NULL){
        fprintf(stderr,"File cannot be opened");
        throw_err(INTERN_ERR);
    }
    init_gc();
    scan.source = f;
    scan.stack.index = 0;
    symbol_table ** table = create_ht();
    tree_node* tree = get_syntax_tree(&scan, table);
    check_semantics(tree, table);
    print_tree(tree);

    free_gc();
    fclose(f);
    return 0;
}
