#ifndef BUENOS_KERNEL_LOCK_H
#define BUENOS_KERNEL_LOCK_H

typedef struct {
	int locked;
} lock_t;

int lock_reset(lock_t*);
void lock_acquire(lock_t*);
void lock_release(lock_t*);
#endif

