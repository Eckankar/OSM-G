#include "kernel/lock_cond.h"
#include "kernel/spinlock.h"
#include "kernel/interrupt.h"
#include "kernel/sleepq.h"
#include "kernel/thread.h"

// Locks {{{

// XXX When does this return -1 ???
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
    spinlock_acquire(&lock->spinlock);
    lock->locked = 0;
    spinlock_release(&lock->spinlock);
    _interrupt_set_state(intr_status);

    sleepq_wake(&lock->spinlock);
}

// Acquire a lock by utilizing the sleep queue,
// to wait for availability.
void lock_acquire(lock_t *lock) {
    interrupt_status_t intr_status;

    intr_status = _interrupt_disable();
    spinlock_acquire(&lock->spinlock);

    while(lock->locked) {
        sleepq_add(&lock->spinlock);
        spinlock_release(&lock->spinlock);
        thread_switch();
        spinlock_acquire(&lock->spinlock);
    }

    lock->locked = 1;

    spinlock_release(&lock->spinlock);
    _interrupt_set_state(intr_status);
}
/// end locks }}}
// Conditional variables {{{
int condition_reset(cond_t *cond) {
    // Wake anything that may be waiting.
    sleepq_wake_all(cond);

    // XXX: What is this supposed to return?
    return 0;
}

void condition_wait(cond_t *cond, lock_t *lock) {
    interrupt_status_t intr_status;
    intr_status = _interrupt_disable();

    // No matter what, sleep.
    sleepq_add(cond);

    // Unlock the acquired lock
    lock_release(lock);

    // Re-enable interrupts and switch out
    _interrupt_set_state(intr_status);
    thread_switch();

    // Re-acquire lock before returning
    lock_acquire(lock);
}

void condition_signal(cond_t *cond, lock_t *lock) {
    // Wake a waiting thread
    sleepq_wake(cond);

    lock = lock; // XXX: What should we do with the lock?
}

void condition_broadcast(cond_t *cond, lock_t *lock) {
    // Wake all waiting threads
    sleepq_wake_all(cond);

    lock = lock; // XXX: What should we do with the lock?
}
// end cond }}}
