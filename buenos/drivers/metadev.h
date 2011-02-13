/*
 * Metadevices (RTC, meminfo, CPUinfo, shutdown)
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
 * $Id: metadev.h,v 1.3 2005/03/08 12:57:34 lsalmela Exp $
 *
 */

#ifndef DRIVERS_METADEV_H
#define DRIVERS_METADEV_H

#include "lib/types.h"
#include "drivers/yams.h"
#include "drivers/device.h"
#include "kernel/spinlock.h"

#define DEFAULT_SHUTDOWN_MAGIC 0xdeadc0de
#define POWEROFF_SHUTDOWN_MAGIC 0x0badf00d

#define CPU_COMMAND_RAISE_IRQ 0x00
#define CPU_COMMAND_CLEAR_IRQ 0x01

#define CPU_STATUS_IRQ(status) \
    ((status) & 0x00000002)

/* The structure of the YAMS CPU status device IO area */
typedef struct {
    uint32_t status;   /* Status port of the CPU status device */
    uint32_t command;  /* Command port of the CPU status device */
} cpu_io_area_t;


/* The real device structure for a CPU status device. Structure of
   this type is stored the real_device field of device_t data
   structure. Currently this only provides synchronization for
   generating and clearing interrupts. */
typedef struct {
    /* Spinlock to synchronize access to the driver */
    spinlock_t slock;
    /* If you are using inter-cpu interrupts for something, the
       necessary data may be added here. */
} cpu_real_device_t;

device_t *rtc_init(io_descriptor_t *desc);
uint32_t rtc_get_msec(void);
uint32_t rtc_get_clockspeed(void);

device_t *meminfo_init(io_descriptor_t *desc);
uint32_t meminfo_get_pages(void);

device_t *cpustatus_init(io_descriptor_t *desc);
int cpustatus_count(void);
void cpustatus_generate_irq(device_t *dev);
void cpustatus_interrupt_handle(device_t *dev);

device_t *shutdown_init(io_descriptor_t *desc);
void shutdown(uint32_t magic);

#endif /* DRIVERS_METADEV_H */



