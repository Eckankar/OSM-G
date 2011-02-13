/*
 * YAMS specific constants
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
 * $Id: yams.h,v 1.8 2003/05/16 10:13:54 ttakanen Exp $
 *
 */

#ifndef DRIVERS_YAMS_H
#define DRIVERS_YAMS_H

#include "lib/types.h"

/* Start address of BUENOS kernel. */
#define KERNEL_BOOT_ADDRESS 0x80010000

/* Start address of YAMS IO descriptor area. */
#define IO_DESCRIPTOR_AREA 0xb0000000

/* The maximum number of devices supported by YAMS. */
#define YAMS_MAX_DEVICES 128

/* Start address of YAMS boot parameter area. */
#define BOOT_ARGUMENT_AREA 0xb0001000

/* Page size */
#define PAGE_SIZE 4096

/* Page portion of any address */
#define PAGE_SIZE_MASK 0xfffff000

/* Ensure that the srtuctures are correctly packed into memory */

/* The structure of YAMS IO descriptor. */
typedef struct {
    /* Type of the device */
    uint32_t type          __attribute__ ((packed)); 

    /* Start address of the device io base */
    uint32_t io_area_base  __attribute__ ((packed));     

    /* Length of the device io base */
    uint32_t io_area_len   __attribute__ ((packed));

    /* The interrupt line used by the device */
    uint32_t irq           __attribute__ ((packed));

     /* Vendor string of the device (Note: This 
        is NOT null terminated! */
    char vendor_string[8]  __attribute__ ((packed));

    /* Reserved area (unused). */
    uint32_t resv[2]       __attribute__ ((packed));
} io_descriptor_t;


#define YAMS_TYPECODE_TTY 0x201
#define YAMS_TYPECODE_RTC 0x102
#define YAMS_TYPECODE_DISK 0x301
#define YAMS_TYPECODE_NIC 0x401
#define YAMS_TYPECODE_MEMINFO 0x101
#define YAMS_TYPECODE_SHUTDOWN 0x103
#define YAMS_TYPECODE_CPUSTATUS 0xc00
#define YAMS_TYPECODE_CPUMASK 0xffffff00

#endif
