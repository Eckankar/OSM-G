/*
 * Register name definitions for assembler code.
 *
 * Copyright (C) 2003  Juha Aatrokoski, Timo Lilja,
 *   Leena Salmela, Teemu Takanen, Aleksi Virtanen
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *  3. The name of the author may not be used to endorse or promote
 *     products derived from this software without specific prior
 *     written permission.
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
 * $Id: registers.h,v 1.1 2003/02/10 08:45:33 lsalmela Exp $	
 */

#ifndef LIB_REGISTERS_H
#define LIB_REGISTERS_H

#define zero $0
#define AT   $1   /* at is used in assembler directive .set at */
#define v0   $2   /* return value registers */
#define v1   $3 
#define a0   $4   /* argument registers */
#define a1   $5
#define a2   $6
#define a3   $7
#define t0   $8   /* Caller saves registers */
#define t1   $9
#define t2   $10
#define t3   $11
#define t4   $12
#define t5   $13
#define t6   $14
#define t7   $15
#define s0   $16  /* Callee saves registers */
#define s1   $17
#define s2   $18
#define s3   $19
#define s4   $20
#define s5   $21
#define s6   $22
#define s7   $23
#define t8   $24  /* More caller saves registers */
#define t9   $25
#define k0   $26  /* kernel registers */
#define k1   $27
#define gp   $28  /* global pointer */
#define sp   $29  /* stack pointer */
#define fp   $30  /* frame pointer */
#define ra   $31  /* return address */

/* Co-processor 0 registers (names as listed by YAMS) */
#define Index  $0
#define Random $1
#define EntLo0 $2
#define EntLo1 $3
#define Contxt $4
#define PgMask $5
#define Wired  $6
#define BadVAd $8
#define Count  $9
#define EntrHi $10
#define Compar $11
#define Status $12
#define Cause  $13
#define EPC    $14
#define PRId   $15
#define Conf   $16  /* Conf0 with selection 0, Conf1 with selection 1 */
#define LLAddr $17
#define ErrEPC $30

#endif
