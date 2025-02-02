#ifndef GUSLIB_LIST_H
#define GUSLIB_LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct node {
    struct node *next;
    struct node *prev;
    void *data;
} node_t;

typedef struct list list_t;

list_t *list_create(void);
void list_destroy(list_t *l);
bool list_empty(list_t *l);
node_t *list_get_first(list_t *l);
node_t *list_get_last(list_t *l);
size_t list_size(list_t *l);
void list_insert(list_t *l, void *data);
void list_insert_after(list_t *l, node_t *node, void *data);
void *list_remove(list_t *l, node_t *node);

#endif /* GUSLIB_LIST_H */
