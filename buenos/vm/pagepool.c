/*
 * Pagepool
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
 * $Id: pagepool.c,v 1.6 2004/01/14 09:05:58 ttakanen Exp $
 *
 */

#include "vm/pagepool.h"
#include "lib/bitmap.h"
#include "kernel/kmalloc.h"
#include "kernel/spinlock.h"
#include "kernel/interrupt.h"
#include "kernel/assert.h"

/** @name Page pool
 *
 * Functions and data structures for handling physical page reservation.
 *
 * @{
 */

/* Bitmap field of physical pages. Length is number of physical pages
   rounded up to a word boundary */
static bitmap_t *pagepool_free_pages;

/* Number of physical pages */
static int pagepool_num_pages;

/* Number of free physical pages */
static int pagepool_num_free_pages;

/* Number of last staticly reserved page. This is needed to ensure
   that staticly reserved pages are not freed in accident (or in
   purpose).  */
static int pagepool_static_end;

/* Spinlock to handle synchronous access to pagepool_free_pages */
static spinlock_t pagepool_slock;

/**
 * Pagepool initialization. Finds out number of physical pages and
 * number of staticly reserved physical pages. Marks reserved pages
 * reserved in pagepool_free_pages.
 */
void pagepool_init(void)
{
    int num_res_pages;
    int i;

    pagepool_num_pages = kmalloc_get_numpages();

    pagepool_free_pages = 
        (uint32_t *)kmalloc(bitmap_sizeof(pagepool_num_pages));
    bitmap_init(pagepool_free_pages, pagepool_num_pages);

    /* Note that number of reserved pages must be get after we have 
       (staticly) reserved memory for bitmap. */
    num_res_pages = kmalloc_get_reserved_pages();
    pagepool_num_free_pages = pagepool_num_pages - num_res_pages;
    pagepool_static_end = num_res_pages;

    for (i = 0; i < num_res_pages; i++)
        bitmap_set(pagepool_free_pages, i, 1);

    spinlock_reset(&pagepool_slock);

    kprintf("Pagepool: Found %d pages of size %d\n", pagepool_num_pages,
            PAGE_SIZE);
    kprintf("Pagepool: Static allocation for kernel: %d pages\n", 
            num_res_pages);

}

/**
 * Finds first free physical page and marks it reserved.  
 *
 * @return Address of first free physical page, zero if no free pages
 * are available.
 */
uint32_t pagepool_get_phys_page(void)
{
    interrupt_status_t intr_status;
    int i;

    intr_status = _interrupt_disable();
    spinlock_acquire(&pagepool_slock);
    
    if (pagepool_num_free_pages > 0) {
	i = bitmap_findnset(pagepool_free_pages,pagepool_num_pages);
	pagepool_num_free_pages--;

        /* There should have been a free page. Check that the pagepool
           internal variables are in synch. */
	KERNEL_ASSERT(i >= 0 && pagepool_num_free_pages >= 0);
    } else {
        i = 0;
    }

    spinlock_release(&pagepool_slock);
    _interrupt_set_state(intr_status);
    return i*PAGE_SIZE;
}

/**
 * Frees given page. Given page should be reserved, but not staticly
 * reserved.
 *
 * @param phys_addr Page to be freed.
 */
void pagepool_free_phys_page(uint32_t phys_addr)
{
    interrupt_status_t intr_status;
    int i;

    i = phys_addr / PAGE_SIZE;

    /* A page allocated by kmalloc should not be freed. */
    KERNEL_ASSERT(i >= pagepool_static_end);

    intr_status = _interrupt_disable();
    spinlock_acquire(&pagepool_slock);
    
    /* Check that the page was reserved. */
    KERNEL_ASSERT(bitmap_get(pagepool_free_pages, i) == 1);

    bitmap_set(pagepool_free_pages, i, 0);
    pagepool_num_free_pages++;

    spinlock_release(&pagepool_slock);
    _interrupt_set_state(intr_status);
}



/** @} */

