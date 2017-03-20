#include "list.h"
#include <search.h>
#include <stdio.h>
#include <stdlib.h>

list_t *list_create(void)
{
    list_t *list = malloc(sizeof(list_t));
    list->first = NULL;
    list->prev = NULL;
    return list;
}

void list_destroy(list_t *list)
{
    free(list);
}

bool list_empty(list_t *list)
{
    return list->first == NULL;
}

void list_insert(list_t *list, void *data)
{
    node_t *node = malloc(sizeof(node_t));
    node->data = data;
    if (list_empty(list)) {
        list->first = node;
        insque(node, NULL);
    } else {
        insque(node, list->prev);
    }
    list->prev = node;
}

void *list_remove_first(list_t *list)
{
    node_t *node = list->first;
    void *data = node->data;
    list->first = node->forward;
    remque(node);
    free(node);
    return data;
}

node_t *list_get_first(list_t *list)
{
    return list->first;
}
