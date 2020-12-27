#ifndef LEX_H
#define LEX_H

#include <stdio.h>
#include "token.h"

typedef struct {
    FILE *input;
    int line;
    int col;
    int last_tok_line;
    int last_tok_col;
    long checkpoint;
} lex_t;

int lex_init(lex_t *l, FILE *input);
token_t *lex_next_token(lex_t *l);
void lex_checkpoint(lex_t *l);
void lex_error(lex_t *l, const char *msg);

#endif /* LEX_H */
