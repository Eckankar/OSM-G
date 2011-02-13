/*
 * Random numbers.
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
 * $Id: rand.S,v 1.1 2003/02/20 16:59:57 lsalmela Exp $
 *
 */

#include "lib/registers.h"
        
        .data
rand5_a:       
        .word 314159557

rand5_c:
        .word 23550511
rand5_seed:
        .word 0
        
       	.text
	.align	2

        .globl  _set_rand_seed
        .ent    _set_rand_seed
_set_rand_seed:
        sw      a0, rand5_seed
        jr      ra
        .end _set_rand_seed
        
	.globl	_get_rand
	.ent	_get_rand
_get_rand:
        lw t0, rand5_a     # Initialize a                     (1)
        lw t1, rand5_c     # Initialize c
        lw t2, rand5_seed  # Load X

        multu t2, t0        # Compute a*X                      (2)
        mflo t2             # get a*X mod m (m=2^32)
        addu t2, t2, t1     # X=((a*X) mod m)+ c mod m == X=(a*X+c) mod m

        sw t2, rand5_seed   # Save X for next call             (3)

        # Compute uniformly distributed number between 0 and 33
        # Use highest bits of X since they are "best" in terms
        # of random distribution

                             #                                  (4)
        multu t2, a0         # X*range, think X as a fraction of X and
                             # it makes much more sense. Note that
                             # X is never more than 0xffffffff so now
                             # we have number [0,33] in $hi.

        mfhi v0              # X=[0,range-1]
        jr      ra
 
	.end	_get_rand
