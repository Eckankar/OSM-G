#include <stdlib.h>
#include <stdio.h>
#include "bintree.h"
#include "util.h"

// tnode_t {{{
void insert(tnode_t **tree, int data) {
    if (*tree == NULL) {
        *tree = malloc(sizeof(tnode_t));
        verify_malloc(*tree);

        (*tree)->data = data;
        (*tree)->lchild = NULL;
        (*tree)->rchild = NULL;
    } else {
        if ((*tree)->data >= data) {
            insert(&(*tree)->lchild, data);
        } else {
            insert(&(*tree)->rchild, data);
        }
    }
}

void print_inorder(tnode_t *tree) {
    if (tree == NULL) return;

    print_inorder(tree->lchild);
    printf("%d ", tree->data);
    print_inorder(tree->rchild);
}

int size(tnode_t *tree) {
    if (tree == NULL) return 0;

    return 1 + size(tree->lchild) + size(tree->rchild);
}

void dump_to_array(tnode_t *tree, int **arr) {
    if (tree == NULL) return;

    dump_to_array(tree->lchild, arr);
    **arr = tree->data;
    (*arr)++;
    dump_to_array(tree->rchild, arr);
}

// Returns the array in-order.
int *to_array(tnode_t *tree) {
    if (tree == NULL) return NULL;

    int *arr = malloc(size(tree) * sizeof(int));
    verify_malloc(arr);

    int *darr = arr;
    dump_to_array(tree, &darr);

    return arr;
}

void delete_tree(tnode_t **tree) {
    if (*tree == NULL) return;

    delete_tree(&(*tree)->lchild);
    delete_tree(&(*tree)->rchild);

    free(*tree);
    *tree = NULL;
}
// }}} end of tnode_t
// tnode_t2 {{{

void insert2(tnode_t2 **tree, void *data, int (*comp)(void *, void *)) {
    if (*tree == NULL) {
        *tree = malloc(sizeof(tnode_t2));
        verify_malloc(*tree);

        (*tree)->data = data;
        (*tree)->lchild = NULL;
        (*tree)->rchild = NULL;
    } else {
        if (comp((*tree)->data, data) >= 0) {
            insert2(&(*tree)->lchild, data, comp);
        } else {
            insert2(&(*tree)->rchild, data, comp);
        }
    }
}

void print_inorder2(tnode_t2 *tree, void (*print)(void *)) {
    if (tree == NULL) return;

    print_inorder2(tree->lchild, print);
    print(tree->data);
    print_inorder2(tree->rchild, print);
}

void delete_tree2(tnode_t2 **tree) {
    if (*tree == NULL) return;

    delete_tree2(&(*tree)->lchild);
    delete_tree2(&(*tree)->rchild);

    free(*tree);
    *tree = NULL;
}
// }}} end of tnode_t2
