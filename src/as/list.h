#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>

typedef struct node {
    struct node *forward;
    struct node *bakward;
    void *data;
} node_t;

typedef struct {
    node_t *first;
    node_t *prev;
} list_t;

list_t *list_create(void);
void list_destroy(list_t *list);
bool list_empty(list_t *list);
void list_insert(list_t *list, void *data);
void *list_remove_first(list_t *list);
node_t *list_get_first(list_t *list);

#endif /* __LIST_H__ */
