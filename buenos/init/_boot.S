/*
 * The boot assembler code (linker entry point is _start).
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
 * $Id: _boot.S,v 1.3 2003/05/27 16:17:07 ttakanen Exp $
 *
 */

#include "kernel/asm.h"
	
        .text
	.align	2
	.globl	_start
	.ent	_start

_start:	
	# If we are running on another CPU than 0, wait until CPU 0
	# has initialized the system
	_FETCH_CPU_NUM(t0)
	bnez	t0, init_wait_loop

	# Only CPU 0 runs these
	li      sp, 0x8000fffc    # Initialize SP
	j	init               # Jump to init() in main.c
        nop
        .end    _start


	.ent init_wait_loop
init_wait_loop:
	# Wait until kernel_bootstrap_finished != 0 
	la	t0, kernel_bootstrap_finished
init_wait_more:
	lw	t1, (t0)
	beqz	t1, init_wait_more

	li      sp, 0x80007ffc    # Initialize SP
	
	# Exit bootstrap mode and enable interrupts
	jal _interrupt_clear_bootstrap
	jal _interrupt_enable

	# Force a context switch and scheduling
	jal _interrupt_generate_sw0

	j _idle_thread_wait_loop
	.end init_wait_loop
