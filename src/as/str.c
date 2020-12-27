#include "str.h"
#include <stdlib.h>

#define STRING_INIT_SIZE 8

str_t *str_create(void)
{
    str_t *s = malloc(sizeof(str_t));
    s->buf_size = STRING_INIT_SIZE;
    s->str_len = 0;
    s->str = malloc(STRING_INIT_SIZE);
    return s;
}

char *str_destroy(str_t *s)
{
    char *tmp = s->str;
    free(s);
    return tmp;
}

void str_append(str_t *s, char c)
{
    if (s->str_len + 1 == s->buf_size) {
        s->buf_size *= 2;
        s->str = realloc(s->str, s->buf_size);
    }
    s->str[s->str_len++] = c;
}
