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
 * $Id: lib.h,v 1.4 2004/02/18 15:14:43 ttakanen Exp $
 *
 */

#ifndef BUENOS_USERLAND_LIB_H
#define BUENOS_USERLAND_LIB_H

#include "lib/types.h"

/* Filehandles for input and output */
#define stdin 0
#define stdout 1
#define stderr 2

/* Makes the syscall 'syscall_num' with the arguments 'a1', 'a2' and 'a3'. */
uint32_t _syscall(uint32_t syscall_num, uint32_t a1, uint32_t a2, uint32_t a3);

/* The library functions which are just wrappers to the _syscall function. */

void syscall_halt(void);

int syscall_exec(const char *filename);
int syscall_execp(const char *filename, int argc, const char **argv);
int syscall_join(int pid);
void syscall_exit(int retval);

int syscall_open(const char *filename);
int syscall_close(int filehandle);
int syscall_seek(int filehandle, int offset);
int syscall_read(int filehandle, void *buffer, int length);
int syscall_write(int filehandle, const void *buffer, int length);
int syscall_create(const char *filename, int size);
int syscall_delete(const char *filename);

int syscall_fork(void (*func)(int), int arg);
void *syscall_memlimit(void *heap_end);


#endif /* BUENOS_USERLAND_LIB_H */
