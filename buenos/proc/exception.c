/*
 * Exception handling.
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
 * $Id: exception.c,v 1.4 2003/05/20 16:55:31 lsalmela Exp $
 *
 */

#include "kernel/panic.h"
#include "kernel/interrupt.h"
#include "lib/libc.h"
#include "kernel/thread.h"
#include "kernel/exception.h"

void syscall_handle(context_t *user_context);

/** Handles an exception (code != 0) that occured in user mode. Will
 * call appropiate handlers for the exception or panic if the
 * exception should not have occured or does not (yet) have a handler.
 * Interrupts are disabled by EXL when this function is called, and
 * must be so when this fucntion returns. Interrupts may be enabled
 * during execution of this function if needed.
 *
 * @param exception The exception code field from the CP0 Cause register
 */
void user_exception_handle(int exception)
{
    thread_table_t *my_entry;

    /* While interrupts are disabled here, they can be enabled when
       handling system calls and certain other exceptions if needed.
       For normal TLB exceptions it is not desirable that context is
       switched before TLB is filled. */
    _interrupt_disable();

    /* Clear EXL to make normal interrupt disable/enable work. */
    _interrupt_clear_EXL();

    /* Save usermode context to user_context for later reference in syscalls */
    my_entry= thread_get_current_thread_entry();
    my_entry->user_context = my_entry->context;

    switch(exception) {
    case EXCEPTION_TLBM:
	KERNEL_PANIC("TLB Modification: not handled yet");
	break;
    case EXCEPTION_TLBL:
	KERNEL_PANIC("TLB Load: not handled yet");
	break;
    case EXCEPTION_TLBS:
	KERNEL_PANIC("TLB Store: not handled yet");
	break;
    case EXCEPTION_ADDRL:
	KERNEL_PANIC("Address Error Load: not handled yet");
	break;
    case EXCEPTION_ADDRS:
	KERNEL_PANIC("Address Error Store: not handled yet");
	break;
    case EXCEPTION_BUSI:
	KERNEL_PANIC("Bus Error Instruction: not handled yet");
	break;
    case EXCEPTION_BUSD:
	KERNEL_PANIC("Bus Error Data: not handled yet");
	break;
    case EXCEPTION_SYSCALL:
        _interrupt_enable();
        syscall_handle(my_entry->user_context);
        _interrupt_disable();
	break;
    case EXCEPTION_BREAK:
	KERNEL_PANIC("Breakpoint: not handled yet");
	break;
    case EXCEPTION_RESVI:
	KERNEL_PANIC("Reserved instruction: not handled yet");
	break;
    case EXCEPTION_COPROC:
	KERNEL_PANIC("Coprocessor unusable: buggy assembler code?");
	break;
    case EXCEPTION_AOFLOW:
	KERNEL_PANIC("Arithmetic overflow: buggy assembler code?");
	break;
    case EXCEPTION_TRAP:
	KERNEL_PANIC("Trap: this just should not happen");
	break;
    default:
	KERNEL_PANIC("Unknown exception");
    }

    /* Interrupts are disabled by setting EXL after this point. */
    _interrupt_set_EXL();
    _interrupt_enable();

}
