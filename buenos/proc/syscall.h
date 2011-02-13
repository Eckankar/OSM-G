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
 * $Id: syscall.h,v 1.5 2005/02/23 09:34:34 lsalmela Exp $
 *
 */

#ifndef BUENOS_PROC_SYSCALL
#define BUENOS_PROC_SYSCALL

/* Syscall function numbers. You may add to this list but do not
 * modify the existing ones.
 */
#define SYSCALL_HALT 0x001
#define SYSCALL_EXEC 0x101
#define SYSCALL_EXIT 0x102
#define SYSCALL_JOIN 0x103
#define SYSCALL_FORK 0x104
#define SYSCALL_MEMLIMIT 0x105
#define SYSCALL_OPEN 0x201
#define SYSCALL_CLOSE 0x202
#define SYSCALL_SEEK 0x203
#define SYSCALL_READ 0x204
#define SYSCALL_WRITE 0x205
#define SYSCALL_CREATE 0x206
#define SYSCALL_DELETE 0x207

/* When userland program reads or writes these already open files it
 * actually accesses the console.
 */
#define FILEHANDLE_STDIN 0
#define FILEHANDLE_STDOUT 1
#define FILEHANDLE_STDERR 2

#endif
