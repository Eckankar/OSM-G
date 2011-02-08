#ifndef BINTREE_H
#define BINTREE_H

#define TNODE_LEFT 1
#define TNODE_RIGHT 2

#define TOK 0
#define TERR -1


// Til opgave 1 og 2

typedef struct tnode_t {
	int data;
	struct tnode_t *lchild;
	struct tnode_t *rchild;
} tnode_t;

void insert(tnode_t **, int);
void print_inorder(tnode_t *);
int size(tnode_t *);
int *to_array(tnode_t *);

// Custom functions:
void delete_tree(tnode_t **);

// Til opgave 3

typedef struct tnode_t2 {
	void *data;
	struct tnode_t2 *lchild;
	struct tnode_t2 *rchild;
} tnode_t2;

void insert2(tnode_t2 **, void*, int (*comp)(void *, void *));

// Custom functions:
void print_inorder2(tnode_t2 *, void (*print)(void *));
void delete_tree2(tnode_t2 **);

#endif
