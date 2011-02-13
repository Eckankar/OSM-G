/*
 * Virtual memory initialization
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
 * $Id: vm.c,v 1.11 2004/01/13 11:09:50 ttakanen Exp $
 *
 */

#include "vm/pagetable.h"
#include "vm/vm.h"
#include "vm/pagepool.h"
#include "kernel/kmalloc.h"
#include "kernel/assert.h"

/** @name Virtual memory system
 *
 * Functions for pagetable handling and page mappings.
 *
 * @{
 */


/* Check whether given (virtual) address is even or odd mapping
   in a pair of mappings for TLB. */
#define ADDR_IS_ON_ODD_PAGE(addr)  ((addr) & 0x00001000)  
#define ADDR_IS_ON_EVEN_PAGE(addr) (!((addr) & 0x00001000))  


/**
 * Initializes virtual memory system. Initialization consists of page
 * pool initialization and disabling static memory reservation. After
 * this kmalloc() may not be used anymore.
 */ 
void vm_init(void)
{
    /* Make sure that tlb_entry_t really is exactly 3 registers wide
       and thus probably also matches the hardware TLB registers. This
       is needed for assembler wrappers used for TLB manipulation. 
       Any extensions to pagetables should also provide this information
       in this form. */
    KERNEL_ASSERT(sizeof(tlb_entry_t) == 12);

    pagepool_init();
    kmalloc_disable();
}

/**
 *  Creates a new page table. Reserves memory (one page) for the table
 *  and sets the address space identifier for the created page table.
 *
 *  @param asid Address space identifier
 *
 *  @return The created page table
 *
 */

pagetable_t *vm_create_pagetable(uint32_t asid)
{
    pagetable_t *table;
    uint32_t addr;

    addr = pagepool_get_phys_page();
    if(addr == 0) {
	return NULL;
    }

    /* Convert physical page address to kernel unmapped
       segmented address. Since the size of that segment is 512MB,
       this way works only for pages allocated in the first 512MB of
       physical memory. */
    table = (pagetable_t *) (ADDR_PHYS_TO_KERNEL(addr));

    table->ASID        = asid;
    table->valid_count = 0;

    return table;
}

/**
 * Destroys given pagetable. Frees the memory (one page) allocated for
 * the pagetable. Does not remove mappings from the TLB.
 *
 * @param pagetable Page table to destroy
 *
 */

void vm_destroy_pagetable(pagetable_t *pagetable)
{
    pagepool_free_phys_page(ADDR_KERNEL_TO_PHYS((uint32_t) pagetable));
}

/**
 * Maps given virtual address to given physical address in given page
 * table. Does not modify TLB. The mapping is done in 4k chunks (pages).
 *
 * @param pagetable Page table in which to do the mapping
 *
 * @param vaddr Virtual address to map. This address should be in the
 * beginning of a page boundary (4k).
 *
 * @param physaddr Physical address to map to given virtual address.
 * This address should be in the beginning of a page boundary (4k).
 *
 * @param dirty 1 if this is a dirty page (writable), 0 if this
 * page is not dirty (write-protected). The terminology comes
 * from hardware, in reality, this is write enabling bit.
 *
 */

void vm_map(pagetable_t *pagetable, 
	    uint32_t physaddr, 
	    uint32_t vaddr,
            int dirty)
{
    unsigned int i;

    KERNEL_ASSERT(dirty == 0 || dirty == 1);

    for(i=0; i<pagetable->valid_count; i++) {
	if(pagetable->entries[i].VPN2 == (vaddr >> 13)) {
	    /* TLB has separate mappings for even and odd 
	       virtual pages. Let's handle them separately here,
	       and we have much more fun when updating the TLB later.*/
	    if(ADDR_IS_ON_EVEN_PAGE(vaddr)) {
		if(pagetable->entries[i].V0 == 1) {
		    KERNEL_PANIC("Tried to re-map same virtual page");
		} else {
		    /* Map the page on a pair entry */
		    pagetable->entries[i].PFN0 = physaddr >> 12;
		    pagetable->entries[i].V0 = 1;
		    pagetable->entries[i].G0 = 0;
		    pagetable->entries[i].D0 = dirty;
		    return;
		}
	    } else {
		if(pagetable->entries[i].V1 == 1) {
		    KERNEL_PANIC("Tried to re-map same virtual page");
		} else {
		    /* Map the page on a pair entry */
		    pagetable->entries[i].PFN1 = physaddr >> 12;
		    pagetable->entries[i].V1 = 1;
		    pagetable->entries[i].G1 = 0;
		    pagetable->entries[i].D1 = dirty;
		    return;
		}
	    }
	}
    }
    /* No previous or pairing mapping was found */

    /* Make sure that pagetable is not full */
    if(pagetable->valid_count >= PAGETABLE_ENTRIES) {
	kprintf("Thread with ASID=%d run out of pagetable mapping entries\n",
		pagetable->ASID);
	kprintf("during an attempt to map vaddr 0x%8.8x => phys 0x%8.8x.\n",
		vaddr, physaddr);
	KERNEL_PANIC("Thread run out of pagetable mapping entries.");
    }

    /* Map the page on a new entry */

    pagetable->entries[pagetable->valid_count].VPN2 = vaddr >> 13;
    pagetable->entries[pagetable->valid_count].ASID = pagetable->ASID;

    if(ADDR_IS_ON_EVEN_PAGE(vaddr)) {
	pagetable->entries[pagetable->valid_count].PFN0 = physaddr >> 12;
	pagetable->entries[pagetable->valid_count].D0   = dirty;
	pagetable->entries[pagetable->valid_count].V0   = 1;
	pagetable->entries[pagetable->valid_count].G0   = 0;
	pagetable->entries[pagetable->valid_count].V1   = 0;
    } else {
	pagetable->entries[pagetable->valid_count].PFN1 = physaddr >> 12;
	pagetable->entries[pagetable->valid_count].D1   = dirty;
	pagetable->entries[pagetable->valid_count].V1   = 1;
	pagetable->entries[pagetable->valid_count].G1   = 0;
	pagetable->entries[pagetable->valid_count].V0   = 0;
    }

    pagetable->valid_count++;
}

/**
 * Unmaps given virtual address from given pagetable.
 *
 * @param pagetable Page table to operate on
 *
 * @param vaddr Virtual addres to unmap
 *
 */

void vm_unmap(pagetable_t *pagetable, uint32_t vaddr)
{
    pagetable = pagetable;
    vaddr     = vaddr;
    
    /* Not implemented */
}

/**
 * Sets the dirty bit for the given virtual page in the given
 * pagetable. The page must already be mapped in the pagetable.
 * If a page is marked dirty it can be read and written. If it is
 * clean (not dirty), it can be only read.
 *
 * @param pagetable The pagetable where the mapping resides.
 *
 * @param vaddr The virtual address whose dirty bit is to be set.
 *
 * @param dirty What the dirty bit is set to. Must be 0 or 1.
 */
void vm_set_dirty(pagetable_t *pagetable, uint32_t vaddr, int dirty)
{
    unsigned int i;

    KERNEL_ASSERT(dirty == 0 || dirty == 1);

    for(i=0; i<pagetable->valid_count; i++) {
	if(pagetable->entries[i].VPN2 == (vaddr >> 13)) {
            /* Check whether this is an even or odd page */
	    if(ADDR_IS_ON_EVEN_PAGE(vaddr)) {
		if(pagetable->entries[i].V0 == 0) {
		    KERNEL_PANIC("Tried to set dirty bit of an unmapped "
                                 "entry");
		} else {
		    pagetable->entries[i].D0 = dirty;
		    return;
		}
	    } else {
		if(pagetable->entries[i].V1 == 0) {
		    KERNEL_PANIC("Tried to set dirty bit of an unmapped "
                                 "entry");
		} else {
		    pagetable->entries[i].D1 = dirty;
		    return;
		}
	    }
	}
    }
    /* No mapping was found */

    KERNEL_PANIC("Tried to set dirty bit of an unmapped entry");
}

/** @} */
