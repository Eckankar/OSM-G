#ifndef BUENOS_KERNEL_LOCK_H
#define BUENOS_KERNEL_LOCK_H
#include "kernel/spinlock.h"

typedef struct {
	spinlock_t spinlock;
	int locked;
} lock_t;

int lock_reset(lock_t*);
void lock_acquire(lock_t*);
void lock_release(lock_t*);
#endif

