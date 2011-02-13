/*
 * Kernel memory allocation.
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
 * $Id: kmalloc.c,v 1.11 2007/02/25 15:16:29 jaatroko Exp $
 *
 */

#include "lib/types.h"
#include "lib/libc.h"
#include "drivers/device.h"
#include "kernel/kmalloc.h"
#include "kernel/panic.h"

/** @name Permanent kernel memory allocation
 *
 * This module implements permanent kernel memory allocation in
 * unmapped memory. The memory is allocated after the kernel image.
 * The memory allocated through this module cannot be freed.
 *
 * @{
 */

/* Special symbol, which is put to the end of the kernel binary by the
   linker */
extern uint32_t KERNEL_ENDS_HERE;

/**  The start of the free area. */
static uint32_t free_area_start;

/** End of available memory. */
static uint32_t memory_end;

/**
 * Returns the number of memory pages present in the system. Causes
 * kernel panic if the MemInfo device is not found.
 *
 * @return The number of memory pages
 */
int kmalloc_get_numpages()
{
    uint32_t num_pages = 0;
    int i;

    io_descriptor_t *io_desc;

    io_desc = (io_descriptor_t *)IO_DESCRIPTOR_AREA;
    
    /* Find MemInfo meta device */
    for(i = 0; i < YAMS_MAX_DEVICES; i++) {
        if (io_desc->type == 0x101) {
	    num_pages = (*(uint32_t *)io_desc->io_area_base);
            break;
        }
        io_desc++;
    }

    if (num_pages == 0)
        KERNEL_PANIC("No MemInfo device found.");

    return num_pages;
}

/**
 * Returns the number of pages statically reserved for the kernel.
 *
 * @return The number of pages
 */
int kmalloc_get_reserved_pages()
{

    int num_res_pages;

    num_res_pages = (free_area_start - 0x80000000) / PAGE_SIZE;

    if (((free_area_start - 0x80000000) % PAGE_SIZE) != 0)
        num_res_pages++;

    return num_res_pages;

}

/**
 * Disable static memory allocation for kernel. This is called from
 * the virtual memory initialization function.
 */
void kmalloc_disable()
{
    free_area_start = 0xffffffff;
}

/**
 * Initializes the variables used by kmalloc. Searches for the MemInfo
 * device descriptor to find out the memory size. Sets the
 * \texttt{free_area_start} to contain the address that comes right
 * after the kernel image. Should not be called before polling TTY is
 * initialized. Causes kernel panic if the MemInfo device descriptor
 * is not found.
 */
void kmalloc_init(void)
{
    uint32_t system_memory_size = 0;

    io_descriptor_t *io_desc;

    memory_end = 0;
    io_desc = (io_descriptor_t *)IO_DESCRIPTOR_AREA;
    
    system_memory_size = kmalloc_get_numpages() * PAGE_SIZE;

    memory_end = 0x80000000 + system_memory_size;

    free_area_start = (uint32_t) &KERNEL_ENDS_HERE;

    /* Check that the address is aligned on a word boundary */
    if (free_area_start & 0x03) {
        free_area_start += 4;
        free_area_start &= 0xfffffffc;
    }

    kprintf("Kernel size is 0x%.8x (%d) bytes\n", 
	    (free_area_start - KERNEL_BOOT_ADDRESS),
	    (free_area_start - KERNEL_BOOT_ADDRESS));

    kprintf("System memory size is 0x%.8x (%d) bytes\n",
	    system_memory_size, system_memory_size);
	    
}

/**
 * Allocates permanent memory for the kernel in unmapped memory. Call
 * of this function after virtual memory has been initialized will
 * cause kernel panic. Panics if memory can't be allocated.
 *
 * @param bytes The number of bytes to be allocated.
 *
 * @return The start address of the reseved memory address.
 */
void *kmalloc(int bytes)
{
    uint32_t res;

    /* Panic if VM is initialized */
    if (free_area_start == 0xffffffff){
        KERNEL_PANIC("Attempting to use kmalloc after vm init\n");
    }

    if (free_area_start == 0) {
        KERNEL_PANIC("Attempting to use kmalloc before initialization\n");
    }    

    /* bytes == 0 may be useful for aligning memory so it is allowed */
    if (bytes < 0)
	KERNEL_PANIC("Attempting to kmalloc negative amount of bytes\n");

    if (free_area_start + bytes > memory_end)
	KERNEL_PANIC("Out of memory\n");

    res = free_area_start;

    free_area_start += bytes;

    /* Check that the start of free area is aligned on a word
       boundary */
    if (free_area_start & 0x03) {
        free_area_start += 4;
        free_area_start &= 0xfffffffc;
    }

    return (void *)res;
}


/** @} */
