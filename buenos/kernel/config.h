/*
 * Kernel configuration options.
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
 * $Id: config.h,v 1.19 2003/05/27 16:06:07 ttakanen Exp $
 *
 */

#ifndef BUENOS_CONFIG_H
#define BUENOS_CONFIG_H

/* Define the maximum number of threads supported by the kernel 
 * Range from 2 (idle + init) to 256 (ASID size)
 */
#define CONFIG_MAX_THREADS 32

/* Size of the stack of a kernel thread */
#define CONFIG_THREAD_STACKSIZE 4096

/* Define the maximum number of CPUs supported by the kernel
 * Range from 1 to 32
 * CONFIG_MAX_THREADS should be the same or greater
 */
#define CONFIG_MAX_CPUS 4

/* Define the length of scheduling interval (timeslice) in 
 * processor cycles. 
 * Range from 200 to 2000000000.
 */
#define CONFIG_SCHEDULER_TIMESLICE 750

/* Sets the maximum number of boot arguments that the kernel will 
 * accept.
 * Range from 1 to 1024
 */ 
#define CONFIG_BOOTARGS_MAX 32

/* Define the maximum number of semaphores.
 * Range from 16 to 1024
 */
#define CONFIG_MAX_SEMAPHORES 128

/* Define maximum number of devices.
 * Range from 16 to 128
 */
#define CONFIG_MAX_DEVICES 128

/* Define maximum number of mounted filesystems
 * Range from 1 to 128
 */

#define CONFIG_MAX_FILESYSTEMS 8

/* Define maximum number of open files
 * Range from 16 to 65536
 */

#define CONFIG_MAX_OPEN_FILES 512

/* Maximum number of simultaneously open sockets for POP/SOP 
 * Range from 4 to 65536
 */
#define CONFIG_MAX_OPEN_SOCKETS 64

/* Size of the POP receive queue 
 * Range from 4 to 512
 */
#define CONFIG_POP_QUEUE_SIZE 32

/* Minimum time in milliseconds that POP packets stay in the input queue
 * if nobody is interested in receiving them.
 * Range from 0 to 10000
 */
#define CONFIG_POP_QUEUE_MIN_AGE 250

/* Maximum number of network interfaces 
 * Range from 1 to 64
 */
#define CONFIG_MAX_GNDS 4

/* Defines the number of pages allocated for userland stacks.
 * Range from 1 to 1000
 */
#define CONFIG_USERLAND_STACK_SIZE 1

#endif /* BUENOS_CONFIG_H */
