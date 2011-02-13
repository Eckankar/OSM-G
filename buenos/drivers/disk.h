/*
 * Disk driver
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
 * $Id: disk.h,v 1.4 2003/05/02 08:02:44 javirta2 Exp $
 *
 */

#ifndef DRIVERS_DISK_H
#define DRIVERS_DISK_H

#include "lib/libc.h"
#include "kernel/spinlock.h"
#include "kernel/semaphore.h"
#include "drivers/device.h"
#include "drivers/yams.h"
#include "drivers/gbd.h"


#define DISK_COMMAND_READ            0x1
#define DISK_COMMAND_WRITE           0x2 
#define DISK_COMMAND_RIRQ            0x3
#define DISK_COMMAND_WIRQ            0x4  
#define DISK_COMMAND_BLOCKS          0x5
#define DISK_COMMAND_BLOCKSIZE       0x6
#define DISK_COMMAND_BLOCKSPERCYL    0x7
#define DISK_COMMAND_ROTTIME         0x8
#define DISK_COMMAND_SEEKTIME        0x9

#define DISK_STATUS_RBUSY(status)  ((status) & 0x00000001)
#define DISK_STATUS_WBUSY(status)  ((status) & 0x00000002)
#define DISK_STATUS_RIRQ(status)   ((status) & 0x00000004)
#define DISK_STATUS_WIRQ(status)   ((status) & 0x00000008) 


#define DISK_STATUS_ISECT(status)  ((status) & 0x08000000) 
#define DISK_STATUS_IADDR(status)  ((status) & 0x10000000) 
#define DISK_STATUS_ICOMM(status)  ((status) & 0x20000000) 
#define DISK_STATUS_EBUSY(status)  ((status) & 0x40000000) 
#define DISK_STATUS_ERROR(status)  ((status) & 0x80000000) 

#define DISK_STATUS_ERRORS(status) ((status) & 0xf8000000)


/* Structure of YAMS disk io area. */
typedef struct {
    volatile uint32_t status;
    volatile uint32_t command;
    volatile uint32_t data;
    volatile uint32_t tsector;
    volatile uint32_t dmaaddr;
} disk_io_area_t;

/* Internal data structure for disk driver. */
typedef struct {
    /* spinlock for synchronization of access to this data structure. */
    spinlock_t                 slock;

    /* Queue of pending requests. New requests are placed to queue
       by disk scheduling policy (see disksched_schedule()). */
    volatile gbd_request_t     *request_queue;    

    /* Request currently served by the driver. If NULL device is idle. */
    volatile gbd_request_t     *request_served;
} disk_real_device_t;


/* functions */
device_t *disk_init(io_descriptor_t *desc);


#endif /* DRIVERS_DISK_H */

