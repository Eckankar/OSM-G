/*
 * Pagetable.
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
 * $Id: pagetable.h,v 1.4 2003/05/08 13:47:19 ttakanen Exp $
 *
 */

#ifndef BUENOS_VM_PAGETABLE_H
#define BUENOS_VM_PAGETABLE_H

#include "lib/libc.h"
#include "vm/tlb.h"

/* Number of mapping entries in one pagetable. This is the number
   of entries that fits on a single hardware memory page (4k). */
#define PAGETABLE_ENTRIES 340

/* A pagetable. This structure fits on one physical page (4k). */
typedef struct pagetable_struct_t{
    /* Address space identifier. We use Thread Ids in Buenos. */
    uint32_t ASID;
    /* Number of valid consecutive mappings in this pagetable. */
    uint32_t valid_count;
    /* Actual virtual memory mapping entries*/
    tlb_entry_t entries[PAGETABLE_ENTRIES];
} pagetable_t;

#endif /* BUENOS_VM_PAGETABLE_H */
