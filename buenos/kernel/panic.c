/*
 * Kernel panic.
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
 * $Id: panic.c,v 1.9 2003/05/20 16:55:31 lsalmela Exp $
 *
 */

#include "lib/libc.h"
#include "drivers/metadev.h"
#include "kernel/interrupt.h"

/** @name Kernel panic
 *
 *   This module implements kernel panic. Kernel panic is a situation
 *   where kernel doesn't know what it should do next (unexpected
 *   error, internal error). Panic causes machine to shut down.
 *
 *   @{
 */

/**
 *   Causes kernel panic. In kernel panic the system stops immediately
 *   after description text is printed to screen. This function is
 *   usually called from KERNEL_PANIC macro, which fills the source
 *   code file and line number arguments automatically.
 *
 *   @param file Filename of the module where panic has been called.
 *
 *   @param line Line-number in file where panic has been called.
 * 
 *   @param description Descriptive text that gets printed on screen
 *   before dying.
 *
 */

void _kernel_panic(char *file, int line, char *description)
{
    int cpu;

    _interrupt_disable();
    cpu = _interrupt_getcpu();
    kprintf("Kernel panic (cpu: %d): %s:%d --- %s\n", cpu, file, line, 
            description);
    shutdown(DEFAULT_SHUTDOWN_MAGIC);
}

/** @} */
