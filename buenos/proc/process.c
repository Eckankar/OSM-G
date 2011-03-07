/*
 * Process startup.
 *
 * Copyright (C) 2003-2005 Juha Aatrokoski, Timo Lilja,
 *       Leena Salmela, Teemu Takanen, Aleksi Virtanen.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: process.c,v 1.11 2007/03/07 18:12:00 ttakanen Exp $
 *
 */

#include "proc/process.h"
#include "proc/elf.h"
#include "kernel/thread.h"
#include "kernel/assert.h"
#include "kernel/sleepq.h"
#include "kernel/interrupt.h"
#include "kernel/config.h"
#include "fs/vfs.h"
#include "drivers/yams.h"
#include "vm/vm.h"
#include "vm/pagepool.h"


/** @name Process startup
 *
 * This module contains a function to start a userland process.
 */

/** The table containing all processes in the system, whether active or not. */
process_t process_table[MAX_PROCESSES];

/** Spinlock which must be held when manipulating the process table */
spinlock_t process_table_slock;

/**
 * Returns the process ID of the currently running thread.
 */
process_id_t process_get_current_process() {
	thread_table_t *my_thread = thread_get_current_thread_entry();
	return my_thread->process_id;
}


/**
 * Starts one userland process. The thread calling this function will
 * be used to run the process and will therefore never return from
 * this function. This function asserts that no errors occur in
 * process startup (the executable file exists and is a valid ecoff
 * file, enough memory is available, file operations succeed...).
 * Therefore this function is not suitable to allow startup of
 * arbitrary processes.
 *
 * @executable The name of the executable to be run in the userland
 * process
 */
void process_start(process_id_t pid) {
    thread_table_t *my_entry;
    pagetable_t *pagetable;
    uint32_t phys_page;
    context_t user_context;
    uint32_t stack_bottom;
    elf_info_t elf;
    openfile_t file;
    char executable[MAX_NAME_LENGTH];

    int i;

    interrupt_status_t intr_status;

    my_entry = thread_get_current_thread_entry();

    // Set process id on thread
    my_entry->process_id = pid;

    // Ensure that we're the only ones touching the process table.
    intr_status = _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    // Copy over the name of file we're supposed to execute.
    stringcopy(executable, process_table[pid].name, MAX_NAME_LENGTH);

    // Free our locks.
    spinlock_release(&process_table_slock);
    _interrupt_set_state(intr_status);

    /* If the pagetable of this thread is not NULL, we are trying to
       run a userland process for a second time in the same thread.
       This is not possible. */
    KERNEL_ASSERT(my_entry->pagetable == NULL);

    pagetable = vm_create_pagetable(thread_get_current_thread());
    KERNEL_ASSERT(pagetable != NULL);

    intr_status = _interrupt_disable();
    my_entry->pagetable = pagetable;
    _interrupt_set_state(intr_status);

    file = vfs_open((char *)executable);
    /* Make sure the file existed and was a valid ELF file */
    KERNEL_ASSERT(file >= 0);
    KERNEL_ASSERT(elf_parse_header(&elf, file));

    /* Trivial and naive sanity check for entry point: */
    KERNEL_ASSERT(elf.entry_point >= PAGE_SIZE);

    /* Calculate the number of pages needed by the whole process
       (including userland stack). Since we don't have proper tlb
       handling code, all these pages must fit into TLB. */
    KERNEL_ASSERT(elf.ro_pages + elf.rw_pages + CONFIG_USERLAND_STACK_SIZE
		  <= _tlb_get_maxindex() + 1);

    /* Allocate and map stack */
    for(i = 0; i < CONFIG_USERLAND_STACK_SIZE; i++) {
        phys_page = pagepool_get_phys_page();
        KERNEL_ASSERT(phys_page != 0);
        vm_map(my_entry->pagetable, phys_page,
               (USERLAND_STACK_TOP & PAGE_SIZE_MASK) - i*PAGE_SIZE, 1);
    }

    /* Allocate and map pages for the segments. We assume that
       segments begin at page boundary. (The linker script in tests
       directory creates this kind of segments) */
    for(i = 0; i < (int)elf.ro_pages; i++) {
        phys_page = pagepool_get_phys_page();
        KERNEL_ASSERT(phys_page != 0);
        vm_map(my_entry->pagetable, phys_page,
               elf.ro_vaddr + i*PAGE_SIZE, 1);
    }

    for(i = 0; i < (int)elf.rw_pages; i++) {
        phys_page = pagepool_get_phys_page();
        KERNEL_ASSERT(phys_page != 0);
        vm_map(my_entry->pagetable, phys_page,
               elf.rw_vaddr + i*PAGE_SIZE, 1);
    }

    /* Put the mapped pages into TLB. Here we again assume that the
       pages fit into the TLB. After writing proper TLB exception
       handling this call should be skipped. */
    intr_status = _interrupt_disable();
    tlb_fill(my_entry->pagetable);
    _interrupt_set_state(intr_status);

    /* Now we may use the virtual addresses of the segments. */

    /* Zero the pages. */
    memoryset((void *)elf.ro_vaddr, 0, elf.ro_pages*PAGE_SIZE);
    memoryset((void *)elf.rw_vaddr, 0, elf.rw_pages*PAGE_SIZE);

    stack_bottom = (USERLAND_STACK_TOP & PAGE_SIZE_MASK) -
        (CONFIG_USERLAND_STACK_SIZE-1)*PAGE_SIZE;
    memoryset((void *)stack_bottom, 0, CONFIG_USERLAND_STACK_SIZE*PAGE_SIZE);

    /* Copy segments */

    if (elf.ro_size > 0) {
	/* Make sure that the segment is in proper place. */
        KERNEL_ASSERT(elf.ro_vaddr >= PAGE_SIZE);
        KERNEL_ASSERT(vfs_seek(file, elf.ro_location) == VFS_OK);
        KERNEL_ASSERT(vfs_read(file, (void *)elf.ro_vaddr, elf.ro_size)
		      == (int)elf.ro_size);
    }

    if (elf.rw_size > 0) {
	/* Make sure that the segment is in proper place. */
        KERNEL_ASSERT(elf.rw_vaddr >= PAGE_SIZE);
        KERNEL_ASSERT(vfs_seek(file, elf.rw_location) == VFS_OK);
        KERNEL_ASSERT(vfs_read(file, (void *)elf.rw_vaddr, elf.rw_size)
		      == (int)elf.rw_size);
    }


    /* Set the dirty bit to zero (read-only) on read-only pages. */
    for(i = 0; i < (int)elf.ro_pages; i++) {
        vm_set_dirty(my_entry->pagetable, elf.ro_vaddr + i*PAGE_SIZE, 0);
    }

    /* Insert page mappings again to TLB to take read-only bits into use */
    intr_status = _interrupt_disable();
    tlb_fill(my_entry->pagetable);
    _interrupt_set_state(intr_status);

    /* Initialize the user context. (Status register is handled by
       thread_goto_userland) */
    memoryset(&user_context, 0, sizeof(user_context));
    user_context.cpu_regs[MIPS_REGISTER_SP] = USERLAND_STACK_TOP;
    user_context.pc = elf.entry_point;

    thread_goto_userland(&user_context);

    KERNEL_PANIC("thread_goto_userland failed.");
}

uint32_t process_join(process_id_t pid) {
	interrupt_status_t intr_status;
	uint32_t retval;

	// Disable interrupts and acquire resource lock
	intr_status = _interrupt_disable();
	spinlock_acquire(&process_table_slock);

	// Sleep while the process isn't in its "dying" state.
	while(process_table[pid].state != PROCESS_DYING) {
		sleepq_add(&process_table[pid]);
		spinlock_release(&process_table_slock);
		thread_switch();
		spinlock_acquire(&process_table_slock);
	}

	retval = process_table[pid].retval;
	process_table[pid].state = PROCESS_SLOT_AVAILABLE;

    // Restore interrupts and free our lock
	spinlock_release(&process_table_slock);
	_interrupt_set_state(intr_status);
	return retval;
}

/**
 * This function inserts the userspace thread stack in a list of free
 * stacks maintained in the process table entry.  This means that
 * when/if the next thread is created, we can reuse one of the old
 * stacks, and reduce memory usage.  Note that the stack is not really
 * "deallocated" per se, and still counts towards the 64KiB memory
 * limit for processes.  This is a simple mechanism, not a very good
 * one.  This function assumes that the process table is already
 * locked.
 * 
 * @param my_thread The thread whose stack should be deallocated.
 *
 */
void process_free_stack(thread_table_t *my_thread)
{
    /* Assume we have lock on the process table. */
    process_id_t my_pid = my_thread->process_id;
    uint32_t old_free_list = process_table[my_pid].bot_free_stack;
    /* Find the stack by applying a mask to the stack pointer. */
    uint32_t stack =
        my_thread->user_context->cpu_regs[MIPS_REGISTER_SP] & USERLAND_STACK_MASK;

    KERNEL_ASSERT(stack >= process_table[my_pid].stack_end);

    process_table[my_pid].bot_free_stack = stack;
    *(uint32_t*)stack = old_free_list;
}

/**
 * Terminates the current process and sets a return value
 */
void process_finish(uint32_t retval) {
    interrupt_status_t intr_status;
    process_id_t pid;
	thread_table_t *my_thread;

    // Find out who we are.
    pid = process_get_current_process();
    my_thread = thread_get_current_thread_entry();

    // Ensure that we're the only ones touching the process table.
    intr_status = _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    // Mark the stack as free so new threads can reuse it.
    process_free_stack(my_thread);

    if(--process_table[pid].threads == 0) {
        // Last thread in process; now we die.

        // Mark ourself as dying.
        process_table[pid].retval = retval;
        process_table[pid].state = PROCESS_DYING;

        vm_destroy_pagetable(my_thread->pagetable);

        // Wake whomever may be sleeping for the process
        sleepq_wake(&process_table[pid]);
    }

    // Free our locks.
    spinlock_release(&process_table_slock);
    _interrupt_set_state(intr_status);

    my_thread->pagetable = NULL;

    // Kill the thread.
    thread_finish();
}

// A wrapper for process_start, such that we can
// pass it to thread_create
void process_start_wrapper(uint32_t pid) {
    process_start((process_id_t) pid);
}

/**
 * Spawns a new process in a separate kernel thread, running
 * the given executable
 */
process_id_t process_spawn(const char *executable) {
    TID_t tid;
    process_id_t pid;

    pid = process_obtain_slot(executable);
    tid = thread_create(&process_start_wrapper, (uint32_t)pid);

    thread_run(tid);
    return pid;
}

/**
 * Obtains and initializes a free process table slot.
 */
process_id_t process_obtain_slot(const char *executable) {
    interrupt_status_t intr_status;
    process_id_t pid = -1;

    // Ensure that we're the only ones touching the process table.
    intr_status = _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    // Find a free process slot.
    int i;
	for(i = 0; i < MAX_PROCESSES; i++) {
		if(process_table[i].state == PROCESS_SLOT_AVAILABLE) {
			pid = i;
            break;
        }
	}

	// No free process slots.
	KERNEL_ASSERT(pid != -1);

	process_table[pid].state = PROCESS_RUNNING;
	stringcopy(process_table[pid].name, executable, MAX_NAME_LENGTH);
    process_table[pid].threads = 1;
    process_table[pid].stack_end = (USERLAND_STACK_TOP & PAGE_SIZE_MASK) -
                                        (CONFIG_USERLAND_STACK_SIZE-1)*PAGE_SIZE;
    process_table[pid].bot_free_stack = 0;

	// Free our locks.
    spinlock_release(&process_table_slock);
    _interrupt_set_state(intr_status);

    return pid;
}

/**
 * Initializes the process table for use.
 */
void process_init() {
    // Initialize our spinlock.
    spinlock_reset(&process_table_slock);

    // Mark all process slots as available.
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].state = PROCESS_SLOT_AVAILABLE;
    }
}

// From flertraadning.txt {{{

/**
 * We need to pass a bunch of data to the new thread, but we can only
 * pass a single 32 bit number!  How do we deal with that?  Simple -
 * we allocate a structure on the stack of the forking kernel thread
 * containing all the data we need, with a 'done' field that indicates
 * when the new thread has copied over the data.  See process_fork().
 */
typedef struct thread_params_t {
    volatile uint32_t done; /* Don't cache in register. */
    void (*func)(int);
    int arg;
    process_id_t pid;
    pagetable_t *pagetable;
} thread_params_t;


void setup_thread(thread_params_t *params)
{
    context_t user_context;
    uint32_t phys_page;
    int i;
    interrupt_status_t intr_status;
    thread_table_t *thread= thread_get_current_thread_entry();

    /* Copy thread parameters. */
    int arg = params->arg;
    void (*func)(int) = params->func;
    process_id_t pid = thread->process_id = params->pid;
    thread->pagetable = params->pagetable;
    params->done = 1; /* OK, we don't need params any more. */

    intr_status = _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    /* Set up userspace environment. */
    memoryset(&user_context, 0, sizeof(user_context));

    user_context.cpu_regs[MIPS_REGISTER_A0] = arg;
    user_context.pc = (uint32_t)func;

    /* Allocate thread stack */
    if (process_table[pid].bot_free_stack != 0) {
        /* Reuse old thread stack. */
        user_context.cpu_regs[MIPS_REGISTER_SP] =
            process_table[pid].bot_free_stack
            + CONFIG_USERLAND_STACK_SIZE*PAGE_SIZE
            - 4; /* Space for the thread argument */
        process_table[pid].bot_free_stack =
            *(uint32_t*)process_table[pid].bot_free_stack;
    } else {
        /* Allocate physical pages (frames) for the stack. */
        for (i = 0; i < CONFIG_USERLAND_STACK_SIZE; i++) {
            phys_page = pagepool_get_phys_page();
            KERNEL_ASSERT(phys_page != 0);
            vm_map(thread->pagetable, phys_page,
                    process_table[pid].stack_end - (i+1)*PAGE_SIZE, 1);
        }
        user_context.cpu_regs[MIPS_REGISTER_SP] =
            process_table[pid].stack_end-4; /* Space for the thread argument */
        process_table[pid].stack_end -= PAGE_SIZE*CONFIG_USERLAND_STACK_SIZE;
    }

    tlb_fill(thread->pagetable);

    spinlock_release(&process_table_slock);
    _interrupt_set_state(intr_status);

    thread_goto_userland(&user_context);
}


TID_t process_fork(void (*func)(int), int arg)
{
    TID_t tid;
    thread_table_t *thread = thread_get_current_thread_entry();
    process_id_t pid = thread->process_id;
    interrupt_status_t intr_status;
    thread_params_t params;
    params.done = 0;
    params.func = func;
    params.arg = arg;
    params.pid = pid;
    params.pagetable = thread->pagetable;

    intr_status = _interrupt_disable();
    spinlock_acquire(&process_table_slock);

    tid = thread_create((void (*)(uint32_t))(setup_thread), (uint32_t)&params);

    if (tid < 0) {
        spinlock_release(&process_table_slock);
        _interrupt_set_state(intr_status);
        return -1;
    }

    process_table[pid].threads++;

    spinlock_release(&process_table_slock);
    _interrupt_set_state(intr_status);

    thread_run(tid);

    /* params will be dellocated when we return, so don't until the
        new thread is ready. */
    while (!params.done);

    return tid;
}

// end flertraadning }}}
/** @} */
