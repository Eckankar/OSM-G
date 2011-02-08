#ifndef DLIST_H
#define DLIST_H

#include "bintree.h"

typedef struct dlist_t {
	int data;
	struct dlist_t *prev;
	struct dlist_t *next;
} dlist_t;


dlist_t * tree2dlist(tnode_t *);

// Custom functions
void remove_and_free(dlist_t **);

#endif
