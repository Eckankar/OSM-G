#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

void verify_malloc(void *block) {
    if (block == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(ENOMEM);
    }
}

void err_stop(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

void free_and_null(void **block) {
    free(*block);
    *block = NULL;
}

