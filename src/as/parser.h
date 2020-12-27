#ifndef PARSER_H
#define PARSER_H

#include "lex.h"
#include "utils/list.h"

enum {
    PARSER_OK,
    PARSER_ERR_CMD,
    PARSER_ERR_SYNTAX,
    PARSER_ERR_NUMBER,
    PARSER_ERR_NUMBER_S8,
    PARSER_ERR_NUMBER_FF00,
    PARSER_ERR_INVALID_LABEL,
};

typedef struct {
    lex_t l;
    token_t *saved_tok;
    list_t *label_list;
    list_t *jump_list;
} parser_t;

int parser_init(parser_t *p, FILE *input, FILE *output);
void parser_finish(parser_t *p);
int parser_exec(parser_t *p);
void parser_print_error(parser_t *p, int error);

#endif /* PARSER_H */
