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
 * $Id: exception.c,v 1.6 2004/02/27 10:31:02 ttakanen Exp $
 *
 */

#include "kernel/panic.h"
#include "kernel/interrupt.h"
#include "kernel/exception.h"
#include "lib/libc.h"
#include "vm/tlb.h"

/**
   Prints failed virtual addresses when TLB exception occurs. This
   is helpfull when the only case of TLB exception is a bug in kernel.
   Situation changes a little when VM is implemented and all exceptions
   are no longer error conditions. Calling of this function should be
   limited to kernel errors and debugging.
*/

static void print_tlb_debug(void)
{
   tlb_exception_state_t tes;
   _tlb_get_exception_state(&tes);

   kprintf("TLB exception. Details:\n"
           "Failed Virtual Address: 0x%8.8x\n"
           "Virtual Page Number:    0x%8.8x\n"
           "ASID (Thread number):   %d\n",
           tes.badvaddr, tes.badvpn2, tes.asid);
}

/** Handles an exception (code != 0) that occured in kernel mode. Will
 * call appropiate handlers for the exception or panic if the
 * exception should not have occured or does not (yet) have a handler.
 *
 * @param exception The exception code field from the CP0 Cause register
 */
void kernel_exception_handle(int exception)
{
    interrupt_status_t intr_status;

    /* While interrupts are disabled here, they can be enabled when
       handling system calls and certain other exceptions if needed.
       For normal TLB exceptions it is not desirable that context is
       switched before TLB is filled. */
    intr_status = _interrupt_disable();

    /* Clear EXL to make normal interrupt disable/enable work. */
    _interrupt_clear_EXL();

    switch(exception) {
    case EXCEPTION_TLBM:
        print_tlb_debug();
	KERNEL_PANIC("TLB Modification: not handled yet");
	break;
    case EXCEPTION_TLBL:
        print_tlb_debug();
	KERNEL_PANIC("TLB Load: not handled yet");
	break;
    case EXCEPTION_TLBS:
        print_tlb_debug();
	KERNEL_PANIC("TLB Store: not handled yet");
	break;
    case EXCEPTION_ADDRL:
	print_tlb_debug();
	KERNEL_PANIC("Address Error Load: not handled yet");
	break;
    case EXCEPTION_ADDRS:
	print_tlb_debug();
	KERNEL_PANIC("Address Error Store: not handled yet");
	break;
    case EXCEPTION_BUSI:
	print_tlb_debug();
	KERNEL_PANIC("Bus Error Instruction: not handled yet");
	break;
    case EXCEPTION_BUSD:
	print_tlb_debug();
	KERNEL_PANIC("Bus Error Data: not handled yet");
	break;
    case EXCEPTION_SYSCALL:
	KERNEL_PANIC("Syscall: From _kernel_ mode?");
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
    _interrupt_set_state(intr_status);

}
