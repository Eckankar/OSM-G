/*
 * System calls.
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
 * $Id: syscall.c,v 1.3 2004/01/13 11:10:05 ttakanen Exp $
 *
 */
#include "kernel/cswitch.h"
#include "proc/syscall.h"
#include "kernel/halt.h"
#include "kernel/panic.h"
#include "lib/libc.h"
#include "kernel/assert.h"

int syscall_read(int fhandle, void *buffer, int length) {
	return 0;
}

int syscall_write(int fhandle, const void *buffer, int length) {
	return 0;
}

void syscall_exit(int retval) {

}

int syscall_exec(const char *filename) {
	return 0;
}

int syscall_join(int pid) {
	return 0;
}
/**
 * Handle system calls. Interrupts are enabled when this function is
 * called.
 *
 * @param user_context The userland context (CPU registers as they
 * where when system call instruction was called in userland)
 */
void syscall_handle(context_t *user_context)
{
    /* When a syscall is executed in userland, register a0 contains
     * the number of the syscall. Registers a1, a2 and a3 contain the
     * arguments of the syscall. The userland code expects that after
     * returning from the syscall instruction the return value of the
     * syscall is found in register v0. Before entering this function
     * the userland context has been saved to user_context and after
     * returning from this function the userland context will be
     * restored from user_context.
     */
#define ARG_REG(n) user_context->cpu_regs[MIPS_REGISTER_A##n]

    switch(ARG_REG(0)) {
    case SYSCALL_HALT:
        halt_kernel();
        break;
	case SYSCALL_READ:
		syscall_read((int)ARG_REG(1), (void *)ARG_REG(2), (int)ARG_REG(3));
        break;
	case SYSCALL_WRITE:
		syscall_write((int)ARG_REG(1), (void *)ARG_REG(2), (int)ARG_REG(3));
        break;
	case SYSCALL_EXIT:
        syscall_exit((int)ARG_REG(1));
        break;
	case SYSCALL_EXEC:
        syscall_exec((const char *)ARG_REG(1));
        break;
	case SYSCALL_JOIN:
        syscall_join((int)ARG_REG(1));
        break;
    default:
        KERNEL_PANIC("Unhandled system call\n");
    }

#undef ARG_REG
    /* Move to next instruction after system call */
    user_context->pc += 4;
}

