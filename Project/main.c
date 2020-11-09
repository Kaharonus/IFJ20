#include <stdio.h>
#include "lex_analyzer.h"
#include "error.h"
#include "lex_token.h"

int main(int argc, char *argv[]) {
    scanner scan;
    FILE *f = fopen(argv[1], "r");
    if(f == NULL){
        fprintf(stderr,"File cannot be opened");
        throw_err(INTERN_ERR);
    }
    scan.source = f;
    lex_token t;
    while((t=get_next_token(&scan)).type != END_OF_FILE){
        fprintf(stderr,"TOKEN: %s, %s, %s, %f, %d \n",TOKEN_TYPES[t.type], KEYWORDS[t.keyword_value], t.string_value, t.number_value.d, t.number_value.i);
    }

    return 0;
}
