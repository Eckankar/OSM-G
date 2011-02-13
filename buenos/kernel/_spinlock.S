/*
 * Spinlocks
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
 * $Id: _spinlock.S,v 1.3 2003/02/25 14:28:36 tlilja Exp $
 *
 */

#include "lib/registers.h"

        .text
	.align	2
/*
 * The initialization and releasing functions for spinlocks do the same
 * thing. Therefore the symbols spinlock_reset and spinlock_release map 
 * to the same address. They both set the value of the spinlock to zero 
 * (ie. the spinlock is not held by any cpu).
 */
        
# void spinlock_reset(spinlock_t *slock)
# void spinlock_release(spinlock_t *slock)
	.globl	spinlock_reset
        .globl  spinlock_release        
	.ent	spinlock_reset

spinlock_reset:  
spinlock_release:
        sw      zero, (a0)
        jr      ra
        .end    spinlock_reset

/* Acquire a spinlock. Sets the value of the spinlock to one indicating 
 * that the spinlock is reserved. We need MIPS32 special instructions LL
 * SC to implement this on an SMP.
 */
        
# void spinlock_acquire(spinlock_t *slock)
	.globl	spinlock_acquire
	.ent	spinlock_acquire

spinlock_acquire:
        ll      t0, (a0)
        bnez    t0, spinlock_acquire
        li      t0, 1        
        sc      t0, (a0)
        beqz    t0, spinlock_acquire
        jr      ra
        .end    spinlock_acquire


        
