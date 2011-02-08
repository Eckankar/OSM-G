#include <stdlib.h>
#include "dlist.h"
#include "bintree.h"
#include "util.h"

void insert_before(dlist_t **list, int data) {
    if (*list == NULL) {
        *list = malloc(sizeof(dlist_t));
        verify_malloc(*list);

        (*list)->data = data;
        (*list)->prev = (*list)->next = *list;
    } else {
        dlist_t *new = malloc(sizeof(dlist_t));
        verify_malloc(new);
        new->data = data;

        new->prev = (*list)->prev;
        new->next = *list;

        new->prev->next = new;
        new->next->prev = new;
    }
}

void tree2dlist_recursive(tnode_t *tree, dlist_t **list) {
    if (tree == NULL) return;

    tree2dlist_recursive(tree->lchild, list);
    insert_before(list, tree->data);
    tree2dlist_recursive(tree->rchild, list);
}

dlist_t *tree2dlist(tnode_t *tree) {
    dlist_t *list = NULL;
    tree2dlist_recursive(tree, &list);
    return list;
}

void remove_and_free(dlist_t **list) {
    if ((*list)->next == *list) {
        free(*list);
        *list = NULL;
    } else {
        (*list)->next->prev = (*list)->prev;
        (*list)->prev->next = (*list)->next;

        dlist_t *orig = *list;
        *list = (*list)->next;
        free(orig);
    }
}
