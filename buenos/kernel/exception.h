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
 * $Id: exception.h,v 1.2 2003/03/12 22:22:28 jaatroko Exp $
 *
 */

#ifndef BUENOS_KERNEL_EXCEPTION_H
#define BUENOS_KERNEL_EXCEPTION_H

#define EXCEPTION_INTR 0
#define EXCEPTION_TLBM 1
#define EXCEPTION_TLBL 2
#define EXCEPTION_TLBS 3
#define EXCEPTION_ADDRL 4
#define EXCEPTION_ADDRS 5
#define EXCEPTION_BUSI 6
#define EXCEPTION_BUSD 7
#define EXCEPTION_SYSCALL 8
#define EXCEPTION_BREAK 9
#define EXCEPTION_RESVI 10
#define EXCEPTION_COPROC 11
#define EXCEPTION_AOFLOW 12
#define EXCEPTION_TRAP 13

void kernel_exception_handle(int exception);
void user_exception_handle(int exception);

#endif /* BUENOS_KERNEL_EXCEPTION_H */
