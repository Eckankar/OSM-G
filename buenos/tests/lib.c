/*
 * Userland library functions
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
 * $Id: lib.c,v 1.6 2004/01/14 10:08:50 ttakanen Exp $
 *
 */

/* You probably want to add new functions to this file. To maintain
 * binary compatibility with other Buenoses (as probably required by
 * your assignments) DO NOT CHANGE EXISTING SYSCALL FUNCTIONS!
 */

#include "proc/syscall.h"
#include "tests/lib.h"


/* Halt the system (sync disks and power off). This function will
 * never return. 
 */
void syscall_halt(void)
{
    _syscall(SYSCALL_HALT, 0, 0, 0);
}


/* Load the file indicated by 'filename' as a new process and execute
 * it. Returns the process ID of the created process. Negative values
 * are errors.
 */
int syscall_exec(const char *filename)
{
    return (int)_syscall(SYSCALL_EXEC, (uint32_t)filename, 0, 0);
}

/* Load the file indicated by 'filename' as a new process and execute
 * it. Returns the process ID of the created process. Negative values
 * are errors.
 */
int syscall_execp(const char *filename, int argc, const char **argv)
{
    return (int)_syscall(SYSCALL_EXEC, (uint32_t)filename, 
                         (uint32_t) argc, 
                         (uint32_t) argv);
}


/* Exit the current process with exit code 'retval'. Note that
 * 'retval' must be non-negative since syscall_join's negative return
 * values are interpreted as errors in the join call itself. This
 * function will never return.
 */
void syscall_exit(int retval)
{
    _syscall(SYSCALL_EXIT, (uint32_t)retval, 0, 0);
}


/* Wait until the execution of the process identified by 'pid' is
 * finished. Returns the exit code of the joined process, or a
 * negative value on error.
 */
int syscall_join(int pid)
{
    return (int)_syscall(SYSCALL_JOIN, (uint32_t)pid, 0, 0);
}


/* Create a new thread running in the same address space as the
 * caller. The thread is started at function 'func', and the thread
 * will end when 'func' returns. 'arg' is passed as an argument to
 * 'func'. Returns 0 on success or a negative value on error.
 */
int syscall_fork(void (*func)(int), int arg)
{
    return (int)_syscall(SYSCALL_FORK, (uint32_t)func, (uint32_t)arg, 0);
}


/* (De)allocate memory by trying to set the heap to end at the address
 * 'heap_end'. Returns the new end address of the heap, or NULL on
 * error. If 'heap_end' is NULL, the current heap end is returned.
 */
void *syscall_memlimit(void *heap_end)
{
    return (void*)_syscall(SYSCALL_MEMLIMIT, (uint32_t)heap_end, 0, 0);
}


/* Open the file identified by 'filename' for reading and
 * writing. Returns the file handle of the opened file (positive
 * value), or a negative value on error.
 */
int syscall_open(const char *filename)
{
    return (int)_syscall(SYSCALL_OPEN, (uint32_t)filename, 0, 0);
}


/* Close the open file identified by 'filehandle'. Zero will be returned
 * success, other values indicate errors. 
 */
int syscall_close(int filehandle)
{
    return (int)_syscall(SYSCALL_CLOSE, (uint32_t)filehandle, 0, 0);
}


/* Read 'length' bytes from the open file identified by 'filehandle'
 * into 'buffer', starting at the current file position. Returns the
 * number of bytes actually read (e.g. 0 if the file position is at
 * the end of file) or a negative value on error.
 */
int syscall_read(int filehandle, void *buffer, int length)
{
    return (int)_syscall(SYSCALL_READ, (uint32_t)filehandle,
                    (uint32_t)buffer, (uint32_t)length);
}


/* Set the file position of the open file identified by 'filehandle'
 * to 'offset'. Returns 0 on success or a negative value on error. 
 */
int syscall_seek(int filehandle, int offset)
{
    return (int)_syscall(SYSCALL_SEEK,
			 (uint32_t)filehandle, (uint32_t)offset, 0);
}


/* Write 'length' bytes from 'buffer' to the open file identified by
 * 'filehandle', starting from the current file position. Returns the
 * number of bytes actually written or a negative value on error.
 */
int syscall_write(int filehandle, const void *buffer, int length)
{
    return (int)_syscall(SYSCALL_WRITE, (uint32_t)filehandle, (uint32_t)buffer,
                    (uint32_t)length);
}


/* Create a file with the name 'filename' and initial size of
 * 'size'. Returns 0 on success and a negative value on error. 
 */
int syscall_create(const char *filename, int size)
{
    return (int)_syscall(SYSCALL_CREATE, (uint32_t)filename, (uint32_t)size, 0);
}


/* Remove the file identified by 'filename' from the file system it
 * resides on. Returns 0 on success or a negative value on error. 
 */
int syscall_delete(const char *filename)
{
    return (int)_syscall(SYSCALL_DELETE, (uint32_t)filename, 0, 0);
}
