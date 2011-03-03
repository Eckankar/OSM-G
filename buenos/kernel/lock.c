#include "kernel/lock.h"
#include "kernel/spinlock.h"
#include "kernel/interrupt.h"

// TODO When does this return -1 ???
// Initialize the lock so its ready to be used.
int lock_reset(lock_t *lock) {
	spinlock_reset(&lock->spinlock);
	lock->locked = 0;
	return 0;
}

// Set locked to 0 and wake from the sleep queue
void lock_release(lock_t *lock) {
	interrupt_status_t intr_status;

	intr_status = _interrupt_disable();
	spinlock_acquire(lock->spinlock);
	lock->locked = 0;
	spinlock_release(lock->spinlock);
	_interrupt_set_state(intr_status);

	sleepq_wake(&lock->spinlock);
}

// Acquire a lock by utilizing the sleep queue,
// to wait for availability.
void lock_acquire(lock_t *lock) {
	interrupt_status_t intr_status;

	intr_status = _interrupt_disable();
	spinlock_acquire(lock->spinlock);

	while(lock->locked) {
		sleepq_add(&lock->spinlock);
		spinlock_release(&lock->spinlock);
		thread_switch();
		spinlock_acquire(&lock->spinlock);
	}

	lock->locked = 1;

	spinlock_release(lock->spinlock);
	_interrupt_set_state(intr_status);
}
