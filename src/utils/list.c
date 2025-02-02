#include "list.h"
#include <assert.h>
#include <stdlib.h>

struct list {
    node_t *first;
    node_t *last;
    size_t size;
};

list_t *list_create(void)
{
    list_t *l = malloc(sizeof(list_t));
    l->first = NULL;
    l->last = NULL;
    l->size = 0u;
    return l;
}

void list_destroy(list_t *l)
{
    assert(l->first == NULL);
    assert(l->last == NULL);
    free(l);
}

bool list_empty(list_t *l)
{
    return l->first == NULL;
}

node_t *list_get_first(list_t *l)
{
    return l->first;
}

node_t *list_get_last(list_t *l)
{
    return l->last;
}

size_t list_size(list_t *l)
{
    return l->size;
}

static node_t *node_create(void *data)
{
    node_t *node = malloc(sizeof(node_t));
    node->data = data;
    return node;
}

void list_insert_after(list_t *l, node_t *node, void *data)
{
    node_t *new = node_create(data);
    new->prev = node;
    new->next = node->next;
    if (node->next == NULL)
        l->last = new;
    else
        node->next->prev = new;
    node->next = new;
    l->size++;
}

void list_insert(list_t *l, void *data)
{
    if (list_empty(l)) {
        node_t *node = node_create(data);
        l->first = node;
        l->last = node;
        node->prev = NULL;
        node->next = NULL;
        l->size++;
    } else {
        list_insert_after(l, l->last, data);
    }
}

void *list_remove(list_t *l, node_t *node)
{
    void *data = node->data;
    if (node == l->first) {
        l->first = node->next;
    } else {
        node->prev->next = node->next;
    }
    if (node == l->last) {
        l->last = node->prev;
    } else {
        node->next->prev = node->prev;
    }
    free(node);
    l->size--;
    return data;
}
