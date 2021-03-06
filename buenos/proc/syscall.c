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
 * $Id: syscall.c,v 1.3 2004/01/13 11:10:05 ttakanen Exp $
 *
 */
#include "kernel/cswitch.h"
#include "proc/syscall.h"
#include "kernel/halt.h"
#include "kernel/panic.h"
#include "lib/libc.h"
#include "kernel/assert.h"
#include "proc/process.h"
#include "drivers/gcd.h"
#include "kernel/lock_cond.h"
#include "drivers/device.h"

// TODO Test this
int syscall_read(int fhandle, void *buffer, int length) {
	device_t *dev;
	gcd_t *gcd;

	if(fhandle == FILEHANDLE_STDIN) {
		dev = device_get(YAMS_TYPECODE_TTY, 0);
		if(dev == NULL) return -1;

		gcd = (gcd_t *)dev->generic_device;
		if(gcd == NULL) return -1;

		return gcd->read(gcd, buffer, length);
	} else {
	// Read at most length bytes from fhandle.
		return vfs_read((openfile_t)fhandle, buffer, length);
	}
}

// TODO Redo to allow files to be written to
int syscall_write(int fhandle, const void *buffer, int length) {
	device_t *dev;
	gcd_t *gcd;

	if(fhandle == FILEHANDLE_STDOUT) {
		dev = device_get(YAMS_TYPECODE_TTY, 0);
		if(dev == NULL) return -1;

		gcd = (gcd_t *)dev->generic_device;
		if(gcd == NULL) return -1;

		return gcd->write(gcd, buffer, length);
	} else {
		return vfs_write((openfile_t)fhandle, buffer, length);
	}
}

void syscall_exit(int retval) {
    process_finish(retval);
}

int syscall_exec(const char *filename) {
    return process_spawn(filename);
}

int syscall_join(int pid) {
	return process_join(pid);
}

int syscall_fork(void (*func)(int), int arg) {
    return process_fork(func, arg);
}

int syscall_lock_create(usr_lock_t *lock) {
	return lock_reset(lock);
}

void syscall_lock_acquire(usr_lock_t *lock) {
	lock_acquire(lock);
}

void syscall_lock_release(usr_lock_t *lock) {
	lock_release(lock);
}

int syscall_condition_create(usr_cond_t *cond) {
	return condition_reset(cond);
}

void syscall_condition_wait(usr_cond_t *cond, usr_lock_t *lock) {
	condition_wait(cond, lock);
}

void syscall_condition_signal(usr_cond_t *cond, usr_lock_t *lock) {
	condition_signal(cond, lock);
}

void syscall_condition_broadcast(usr_cond_t *cond, usr_lock_t *lock) {
	condition_broadcast(cond, lock);
}

int syscall_open(const char *filename) {
	return vfs_open(filename);
}

int syscall_close(int filehandle) {
	return vfs_close((openfile_t)filehandle);
}

int syscall_create(const char *filename, int size) {
	return vfs_create(filename, size);
}

int syscall_delete(const char *filename) {
	return vfs_remove(filename);
}

void syscall_seek(int filehandle, int offset) {
	return vfs_seek((openfile_t)filehandle, offset);
}

/**
 * Handle system calls. Interrupts are enabled when this function is
 * called.
 *
 * @param user_context The userland context (CPU registers as they
 * where when system call instruction was called in userland)
 */
void syscall_handle(context_t *user_context)
{
    /* When a syscall is executed in userland, register a0 contains
     * the number of the syscall. Registers a1, a2 and a3 contain the
     * arguments of the syscall. The userland code expects that after
     * returning from the syscall instruction the return value of the
     * syscall is found in register v0. Before entering this function
     * the userland context has been saved to user_context and after
     * returning from this function the userland context will be
     * restored from user_context.
     */
#define ARG_REG(n) user_context->cpu_regs[MIPS_REGISTER_A##n]
#define RET_REG(n) user_context->cpu_regs[MIPS_REGISTER_V0]
    switch(ARG_REG(0)) {
    case SYSCALL_HALT:
        halt_kernel();
        break;
	case SYSCALL_READ:
		RET_REG(0) = syscall_read((int)ARG_REG(1), (void *)ARG_REG(2), (int)ARG_REG(3));
        break;
	case SYSCALL_WRITE:
		RET_REG(0) = syscall_write((int)ARG_REG(1), (const void *)ARG_REG(2), (int)ARG_REG(3));
        break;
	case SYSCALL_EXIT:
        syscall_exit((int)ARG_REG(1));
        break;
	case SYSCALL_EXEC:
        RET_REG(0) = syscall_exec((const char *)ARG_REG(1));
        break;
	case SYSCALL_JOIN:
        RET_REG(0) = syscall_join((int)ARG_REG(1));
        break;
    case SYSCALL_FORK:
        RET_REG(0) = syscall_fork((void (*)(int))ARG_REG(1), ARG_REG(2));
        break;
	case SYSCALL_LOCK_CREATE:
		RET_REG(0) = syscall_lock_create((usr_lock_t*)ARG_REG(1));
		break;
	case SYSCALL_LOCK_ACQUIRE:
		syscall_lock_acquire((usr_lock_t*)ARG_REG(1));
		break;
	case SYSCALL_LOCK_RELEASE:
		syscall_lock_release((usr_lock_t*)ARG_REG(1));
		break;
	case SYSCALL_COND_CREATE:
		RET_REG(0) = syscall_condition_create((usr_cond_t*)ARG_REG(1));
		break;
	case SYSCALL_COND_WAIT:
		syscall_condition_wait((usr_cond_t*)ARG_REG(1),
								(usr_lock_t*)ARG_REG(2));
		break;
	case SYSCALL_COND_BROADCAST:
		syscall_condition_broadcast((usr_cond_t*)ARG_REG(1),
								(usr_lock_t*)ARG_REG(2));
		break;
	case SYSCALL_COND_SIGNAL:
		syscall_condition_signal((usr_cond_t*)ARG_REG(1),
								(usr_lock_t*)ARG_REG(2));
		break;
	case SYSCALL_VFS_OPEN:
		RET_REG(0) = syscall_open((const char*)ARG_REG(1));
		break;
	case SYSCALL_VFS_CLOSE:
		RET_REG(0) = syscall_close((int)ARG_REG(1));
		break;
	case SYSCALL_VFS_CREATE:
		RET_REG(0) = syscall_create((const char *)ARG_REG(1),(int)ARG_REG(2));
		break;
	case SYSCALL_VFS_DELETE:
		RET_REG(0) = syscall_delete((const char *)ARG_REG(1));
		break;
	case SYSCALL_VFS_SEEK:
		syscall_seek((int)ARG_REG(1), (int)ARG_REG(2));
    default:
        KERNEL_PANIC("Unhandled system call\n");
    }

#undef ARG_REG
#undef RET_REG
    /* Move to next instruction after system call */
    user_context->pc += 4;
}

