#ifndef BUENOS_KERNEL_LOCK_COND_H
#define BUENOS_KERNEL_LOCK_COND_H

#include "kernel/spinlock.h"

// Lock-related things
typedef struct {
	spinlock_t spinlock;
	int locked;
} lock_t;
typedef lock_t usr_lock_t;

int lock_reset(lock_t*);
void lock_acquire(lock_t*);
void lock_release(lock_t*);

// Conditional variable related things
typedef void* cond_t;
typedef cond_t usr_cond_t;

int condition_reset(cond_t*);
void condition_wait(cond_t*, lock_t*);
void condition_signal(cond_t*, lock_t*);
void condition_broadcast(cond_t*, lock_t*);

#endif
