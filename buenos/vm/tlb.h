/*
 * TLB handling
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
 * $Id: tlb.h,v 1.5 2003/05/08 13:47:20 ttakanen Exp $
 *
 */

#ifndef BUENOS_VM_TLB_H
#define BUENOS_VM_TLB_H

#include "lib/libc.h"

/* TLB-entry. These fields match CP0 registers, which means
   they should not be modified. Any extensions should be made into
   a separate structure. */

typedef struct {
    /* Virtual page pair number. These are the upper 19 bits of a 
       virtual address. VPN2 describes which 2 page (8096 bytes)
       region of virtual address space this entry maps. */
    unsigned int VPN2:19    __attribute__ ((packed));
    unsigned int dummy1:5   __attribute__ ((packed));
    /* Address space identifier. When ASID matches CP0 setted ASID
       this entry is valid. In Buenos, we use mapping ASID = Thread Id. */
    unsigned int ASID:8     __attribute__ ((packed));

    unsigned int dummy2:6   __attribute__ ((packed));
    /* Physical page number for even page (VPN2 + 0 bit) maps to PFN0 */
    unsigned int PFN0:20    __attribute__ ((packed));
    /* Cache settings. Not used. */
    unsigned int C0:3       __attribute__ ((packed));
    /* Dirty bit for even page. If this is 0, page is write protected. If 1 
       page can be written. */
    unsigned int D0:1       __attribute__ ((packed));
    /* Valid bit for even page. */ 
    unsigned int V0:1       __attribute__ ((packed));
    /* Global bit for even page. Can't be used without the global bit
       of odd page.*/
    unsigned int G0:1       __attribute__ ((packed));

    unsigned int dummy3:6   __attribute__ ((packed));
    /* Physical page number for even page (VPN2 + 1 bit) maps to PFN1 */
    unsigned int PFN1:20    __attribute__ ((packed));
    /* Cache settings. Not used. */
    unsigned int C1:3       __attribute__ ((packed));
    /* Dirty bit for odd page. If this is 0, page is write protected. If 1 
       page can be written. */
    unsigned int D1:1       __attribute__ ((packed));
    /* Valid bit for odd page. */ 
    unsigned int V1:1       __attribute__ ((packed));
    /* Global bit for odd page. Can't be used without the global bit
       of even page.*/
    unsigned int G1:1       __attribute__ ((packed));
} tlb_entry_t;


/** A datatype containing the information needed in TLB exception handlers. */
typedef struct {
    uint32_t badvaddr; /* Address that caused the exception*/
    uint32_t badvpn2;  /* VPN2 of the above */
    uint32_t asid; /* ASID of the causing process, only 8 lowest bits used */
} tlb_exception_state_t;

/* exception handlers */
void tlb_modified_exception(void);
void tlb_load_exception(void);
void tlb_store_exception(void);

/* Forward declare pagetable_t (== struct pagetable_struct_t) */
struct pagetable_struct_t;
void tlb_fill(struct pagetable_struct_t *pagetable);

/* assembler function wrappers */
void _tlb_get_exception_state(tlb_exception_state_t *state);
void _tlb_set_asid(uint32_t asid);
uint32_t _tlb_get_maxindex(void);

int _tlb_probe(tlb_entry_t *entry);
int _tlb_read(tlb_entry_t *entries, uint32_t index, uint32_t num);
int _tlb_write(tlb_entry_t *entries, uint32_t index, uint32_t num);
void _tlb_write_random(tlb_entry_t *entry);


#endif /* BUENOS_VM_TLB_H */
