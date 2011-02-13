/*
 * Generic block device
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
 * $Id: gbd.h,v 1.5 2003/04/03 15:04:15 javirta2 Exp $
 *
 */

#ifndef DRIVERS_GBD_H
#define DRIVERS_GBD_H

#include "lib/libc.h"
#include "drivers/device.h"
#include "kernel/semaphore.h"

/* Operation codes for Generic Block Device requests. */

typedef enum {
    GBD_OPERATION_READ,
    GBD_OPERATION_WRITE
} gbd_operation_t;

/**
 * Block Device Request Descriptor. When using generic block device
 * read or write functions a pointer to this structure is given as
 * argument. Fill fields block, buf and sem before calling GBD
 * functions, the rest are used only internally in the driver.
 *
 * Note that when you call asynchronic versions of read_block or
 * write_block (sem is not NULL), this structure must stay in memory
 * until the operation is complete (the semaphore is raised). Be
 * careful when allocating this structure from stack.
 *
 */

typedef struct gbd_request_struct {
    /* Block number to operate on */
    uint32_t        block;

    /* Pointer to buffer whose size is equal to block size of the device.

       Note that this pointer must be a PHYSICAL address, not
       segmented address.
    */
    uint32_t       buf;

    /* Semaphore which is signaled (increased by one) when the operation
       is complete. If this is set to NULL in call of read or write,
       the call will block until the request is complete.
    */
    semaphore_t    *sem;

    /* Operation code for the request. Filled by the driver. */
    gbd_operation_t operation;

    /* Driver internal data. */
    void           *internal;

    /* Changing pointer for request queues. Used internally by drivers. */ 
    struct gbd_request_struct *next;

    /* Return value for asynchronous call of read or write. After
       the sem is signaled, return value can be read from this field. 
       0 is success, other values indicate failure. */
    int             return_value;
} gbd_request_t;

/* Generic block device descriptor. */
typedef struct gbd_struct {
    /* Pointer to the real device driver under this interface. */
    device_t       *device;

    /* A pointer to a function which reads one block from the device.
       
       Before calling, fill fields block, buf and sem in request.
       If sem is set to NULL, this call will block until the
       request is complete (a block is read). If sem is not NULL,
       this function will return immediately and sem is signaled
       when the request is complete.
    */
    int (*read_block)(struct gbd_struct *gbd, gbd_request_t *request);

    /* A pointer to a function which writes one block to the device.
       
       Before calling, fill fields block, buf and sem in request.
       If sem is set to NULL, this call will block until the
       request is complete (a block is written). If sem is not NULL,
       this function will return immediately and sem is signaled
       when the request is complete.
    */
    int (*write_block)(struct gbd_struct *gbd, gbd_request_t *request);

    /* A pointer to a function which returns the block size of the device
       in bytes. */
    uint32_t (*block_size)(struct gbd_struct *gbd);

    /* A pointer to a function which returns the total number of 
       blocks in this device. */
    uint32_t (*total_blocks)(struct gbd_struct *gbd);
} gbd_t;


#endif /* DRIVERS_GBD_H */

