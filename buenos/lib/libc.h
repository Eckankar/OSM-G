/*
 * Library routines header files for BUENOS
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
 * $Id: libc.h,v 1.19 2004/02/18 14:33:07 ttakanen Exp $
 *
 */
#ifndef BUENOS_LIB_LIBC_H
#define BUENOS_LIB_LIBC_H

/* This should come with the compiler (at least GCC) */
#include <stdarg.h>
#include <stddef.h>
#include "lib/types.h"

#define MIN(arg1,arg2) ((arg1) > (arg2) ? (arg2) : (arg1))
#define MAX(arg1,arg2) ((arg1) > (arg2) ? (arg1) : (arg2))


/* Kernel print routine */
void kwrite(char *s);

/* Kernel read routine */
void kread(char *s, int len);

/* formatted printing functions */
int kprintf(const char *, ...);
int snprintf(char *, int, const char *, ...);

/* the same with va_list arguments */
int kvprintf(const char *, va_list);
int vsnprintf(char *, int, const char *, va_list);

/* Prototypes for random number generator functions */
void _set_rand_seed(uint32_t seed);
uint32_t _get_rand(uint32_t range);

/* Prototypes for string manipulation functions */
int stringcmp(const char *str1, const char *str2);
char *stringcopy(char *target, const char *source, int buflen);
int strlen(const char *str);

/* memory copy */
void memcopy(int buflen, void *target, const void *source);

/* memory set */
void memoryset(void *target, char value, int size);

/* convert string to integer */
int atoi(const char *s);

#endif /* BUENOS_LIB_LIBC_H */
