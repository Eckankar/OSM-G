#ifndef BUENOS_KERNEL_LOCK_COND_H
#define BUENOS_KERNEL_LOCK_COND_H
#include "kernel/lock.h"

typedef struct{
	
} cond_t;

int condition_reset(cond_t*);
void condition_wait(cond_t*, lock_t*);
void condition_signal(cond_t*, lock_t*);
void condition_broadcast(cond_t*, lock_t*);

#endif
