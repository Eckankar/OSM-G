#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bintree.h"
#include "dlist.h"
#include "util.h"

int cmpstr(void *left, void *right) {
    int cmpres = strcmp((char *)left, (char *)right);
    return cmpres > 0 ? 1 : (cmpres < 0 ? -1 : 0);
}

void printstr(void *string) {
    printf("%s ", (char *)string);
}

int main() {
    tnode_t *tree = NULL;

    insert(&tree, 5);
    insert(&tree, 3);
    insert(&tree, 9);
    insert(&tree, 1);
    insert(&tree, 4);
    insert(&tree, 6);

    // Burde printe 1 3 4 5 6 9
    printf("inorder tree walk\nactual:   ");
    print_inorder(tree);
    printf("\nexpected: 1 3 4 5 6 9\n");

    int treesize = size(tree);
    printf("size: %d, expected: %d\n", treesize, 6);

    printf("actual:   ");
    int* arr = to_array(tree);
    for (int i = 0; i < treesize; i++) {
        printf("%d ", arr[i]);
    }
    printf("\nexpected: 1 3 4 5 6 9\n");

    dlist_t *list = tree2dlist(tree);
    dlist_t *it = list;
    printf("actual:   ");
    do {
        printf("%d ", it->data);
        it = it->next;
    } while (it != list);
    printf("\nexpected: 1 3 4 5 6 9\n");

    printf("freeing tree...");
    delete_tree(&tree);
    printf("%s\n", tree == NULL ? "sucess" : "failure");
    printf("freeing array...");
    free_and_null((void *)&arr);
    printf("%s\n", arr == NULL ? "sucess" : "failure");
    printf("freeing doubly linked list...");
    while (list != NULL) {
        remove_and_free(&list);
    }
    printf("success\n");

    char *abe  = "abe",
         *ale  = "ale",
         *kat  = "kat",
         *snot = "snot",
         *vat  = "vat";

    tnode_t2 *tree2 = NULL;
    insert2(&tree2, snot, cmpstr);
    insert2(&tree2, ale, cmpstr);
    insert2(&tree2, vat, cmpstr);
    insert2(&tree2, kat, cmpstr);
    insert2(&tree2, abe, cmpstr);

    printf("actual:   ");
    print_inorder2(tree2, printstr);
    printf("\nexpected: abe ale kat snot vat\n");

    printf("freeing tree2...");
    delete_tree2(&tree2);
    printf("%s\n", tree2 == NULL ? "sucess" : "failure");

    return 0;
}

