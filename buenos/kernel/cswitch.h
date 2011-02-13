/*
 * Context switch.
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
 * $Id: cswitch.h,v 1.8 2003/05/08 12:13:48 ttakanen Exp $
 *
 */
#ifndef KERNEL_CSWITCH_H
#define KERNEL_CSWITCH_H

#include "lib/types.h"

/* Indexes of registers in the register table in context_t structure */
#define MIPS_REGISTER_AT 0
#define MIPS_REGISTER_V0 1
#define MIPS_REGISTER_V1 2
#define MIPS_REGISTER_A0 3
#define MIPS_REGISTER_A1 4
#define MIPS_REGISTER_A2 5
#define MIPS_REGISTER_A3 6
#define MIPS_REGISTER_T0 7
#define MIPS_REGISTER_T1 8
#define MIPS_REGISTER_T2 9
#define MIPS_REGISTER_T3 10
#define MIPS_REGISTER_T4 11
#define MIPS_REGISTER_T5 12
#define MIPS_REGISTER_T6 13
#define MIPS_REGISTER_T7 14
#define MIPS_REGISTER_S0 15
#define MIPS_REGISTER_S1 16
#define MIPS_REGISTER_S2 17
#define MIPS_REGISTER_S3 18
#define MIPS_REGISTER_S4 19
#define MIPS_REGISTER_S5 20
#define MIPS_REGISTER_S6 21
#define MIPS_REGISTER_S7 22
#define MIPS_REGISTER_T8 23
#define MIPS_REGISTER_T9 24
#define MIPS_REGISTER_GP 25
#define MIPS_REGISTER_SP 26
#define MIPS_REGISTER_FP 27
#define MIPS_REGISTER_RA 28

/* Thread context data structure */
typedef struct {
    uint32_t cpu_regs[29];   /* The general purpose registers. zero, k0 and 
                                k1 registers are omitted. */
    uint32_t hi;             /* The hi register. */
    uint32_t lo;             /* The lo register. */
    uint32_t pc;             /* The program counter. Actually loaded from 
                                EPC register in co-processor 0. */
    uint32_t status;         /* Status register bits. */
    void    *prev_context;   /* Previous context in a nested exception chain */
} context_t;

/* Code to be inserted to interrupt vector */
void _cswitch_vector_code(void);

/* Userland entering code */
void _cswitch_to_userland(context_t *usercontext);

#endif
