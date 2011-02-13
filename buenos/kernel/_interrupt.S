/*
 * Interrupt handling
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
 * $Id: _interrupt.S,v 1.5 2003/07/22 19:51:28 ttakanen Exp $
 *
 */

#include "kernel/asm.h"

        .text
	.align	2

# interrupt_state_t _interrupt_disable(void); 
	.globl	_interrupt_disable
	.ent	_interrupt_disable

_interrupt_disable:
	mfc0	v0, Status, 0
	li	t0, 0xffff00fe
	and     t1, t0, v0
	mtc0	t1, Status, 0
	j ra
        .end    _interrupt_disable


# interrupt_state_t _interrupt_enable(void); 
	.globl	_interrupt_enable
	.ent	_interrupt_enable

_interrupt_enable:
	mfc0	v0, Status, 0
	li	t0, 0x0000ff01
	or      t1, t0, v0
	mtc0	t1, Status, 0
	j ra
        .end    _interrupt_enable


# interrupt_state_t interrupt_set_state(interrupt_state_t)
	.globl	_interrupt_set_state
	.ent	_interrupt_set_state

_interrupt_set_state:
	mfc0	v0, Status, 0
	li	t0, 0x0000ff01
	and	t1, t0, a0
	not     t2, t0
	and	t3, v0, t2
	or	t4, t1, t3
	mtc0	t4, Status, 0
	j ra
        .end    _interrupt_set_state

# interrupt_state_t interrupt_get_state(void)
	.globl	_interrupt_get_state
	.ent	_interrupt_get_state

_interrupt_get_state:
	mfc0	v0, Status, 0
	j ra
        .end    _interrupt_get_state

# void _interrupt_generate_sw0(void);

	.globl	_interrupt_generate_sw0
	.ent	_interrupt_generate_sw0

_interrupt_generate_sw0:	
	mfc0	t0, Cause, 0
	li	t1, 0x00000100
	or      t2, t0, t1
	mtc0	t2, Cause, 0
	j ra
        .end    _interrupt_generate_sw0

# void _interrupt_clear_sw(void);

	.globl	_interrupt_clear_sw
	.ent	_interrupt_clear_sw

_interrupt_clear_sw:	
	mfc0	t0, Cause, 0
	li	t1, 0xfffffcff
	and     t2, t0, t1
	mtc0	t2, Cause, 0
	j ra
        .end    _interrupt_clear_sw


# void _interrupt_clear_sw0(void);

	.globl	_interrupt_clear_sw0
	.ent	_interrupt_clear_sw0

_interrupt_clear_sw0:	
	mfc0	t0, Cause, 0
	li	t1, 0xfffffeff
	and     t2, t0, t1
	mtc0	t2, Cause, 0
	j ra
        .end    _interrupt_clear_sw0

# void _interrupt_clear_sw1(void);

	.globl	_interrupt_clear_sw1
	.ent	_interrupt_clear_sw1

_interrupt_clear_sw1:	
	mfc0	t0, Cause, 0
	li	t1, 0xfffffdff
	and     t2, t0, t1
	mtc0	t2, Cause, 0
	j ra
        .end    _interrupt_clear_sw1


# void _interrupt_clear_bootstrap(void);

	.globl	_interrupt_clear_bootstrap
	.ent	_interrupt_clear_bootstrap

_interrupt_clear_bootstrap:	
	mfc0	t0, Status, 0
	li	t1, 0xffbffffb
	and     t2, t0, t1
	mtc0	t2, Status, 0
	j ra
        .end    _interrupt_clear_bootstrap

# /* Returns the CPU on which the calling code is running*/
# int _interrupt_getcpu(void); 

	.globl	_interrupt_getcpu
	.ent	_interrupt_getcpu

_interrupt_getcpu:	
	_FETCH_CPU_NUM(v0)
	j ra
        .end    _interrupt_getcpu

# void _interrupt_clear_EXL(void);

	.globl	_interrupt_clear_EXL
	.ent	_interrupt_clear_EXL

_interrupt_clear_EXL:
	mfc0	t0, Status, 0
	li	t1, 0xfffffffd
	and     t2, t0, t1
	mtc0	t2, Status, 0
	j ra
        .end    _interrupt_clear_EXL

# void _interrupt_set_EXL(void);

	.globl	_interrupt_set_EXL
	.ent	_interrupt_set_EXL

_interrupt_set_EXL:
	mfc0	t0, Status, 0
	li	t1, 0x00000002
	or      t2, t0, t1
	mtc0	t2, Status, 0
	j ra
        .end    _interrupt_set_EXL
