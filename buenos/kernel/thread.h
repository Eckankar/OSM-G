/*
 * Threading system.
 *
 * Copyright (C) 2003 Juha Aatrokoski, Timo Lilja,
 *   Leena Salmela, Teemu Takanen, Aleksi Virtanen.
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
 * $Id: thread.h,v 1.12 2004/01/13 11:11:56 ttakanen Exp $
 *
 */

#ifndef BUENOS_KERNEL_THREAD_H
#define BUENOS_KERNEL_THREAD_H

#include "lib/types.h"
#include "kernel/cswitch.h"
#include "vm/pagetable.h"
#include "proc/process.h"

/* Thread ID data type (index in thread table) */
typedef int TID_t;
typedef enum {
    THREAD_FREE,
    THREAD_RUNNING,
    THREAD_READY,
    THREAD_SLEEPING,
    THREAD_NONREADY,
    THREAD_DYING
} thread_state_t;

#define IDLE_THREAD_TID 0

/* thread table data structure */
typedef struct {
    /* context save areas context and user_context*/
    /* for interrupts */
    context_t *context;
    /* for traps (syscalls), if applicable */
    context_t *user_context;

    /* thread state */
    thread_state_t state;
    /* which resource this thread sleeps on (0 for none) */
    uint32_t sleeps_on;
    /* pointer to this thread's pagetable */
    pagetable_t *pagetable;

    /* PID. Currently not used for anything, but might be useful
       if process table is implemented. */
    process_id_t process_id;
    /* pointer to the next thread in list (<0 = end of list) */
    TID_t next; 

    /* pad to 64 bytes */
    uint32_t dummy_alignment_fill[9]; 
} thread_table_t;

/* function prototypes */
void thread_table_init(void);
TID_t thread_create(void (*func)(uint32_t), uint32_t arg);
void thread_run(TID_t t);

TID_t thread_get_current_thread(void);
thread_table_t *thread_get_current_thread_entry(void);

void thread_switch(void);
#define thread_yield thread_switch

void thread_goto_userland(context_t *usercontext);

void thread_finish(void);


#define USERLAND_ENABLE_BIT 0x00000010

#endif /* BUENOS_KERNEL_THREAD_H */
