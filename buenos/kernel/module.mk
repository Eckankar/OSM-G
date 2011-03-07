# Makefile for the kernel module

# Set the module name
MODULE := kernel


FILES := cswitch.S panic.c kmalloc.c interrupt.c thread.c \
         scheduler.c _interrupt.S _spinlock.S idle.S sleepq.c semaphore.c \
         exception.c halt.c lock_cond.c

SRC += $(patsubst %, $(MODULE)/%, $(FILES))

