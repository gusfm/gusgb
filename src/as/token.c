#include "token.h"
#include <stdlib.h>

token_t *token_create(token_type_t type)
{
    token_t *t = malloc(sizeof(token_t));
    t->type = type;
    t->s = NULL;
    return t;
}

token_t *token_create_string(token_type_t type, char *s)
{
    token_t *t = token_create(type);
    t->s = s;
    return t;
}

void token_destroy(token_t *t)
{
    if (t->type == TOKEN_CONSTANT || t->type == TOKEN_STRING_LITERAL ||
        t->type == TOKEN_IDENT) {
        free(t->s);
    }
    free(t);
}
