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
 * $Id: metadev.c,v 1.6 2005/03/08 12:57:34 lsalmela Exp $
 *
 */

#include "drivers/metadev.h"
#include "lib/libc.h"
#include "kernel/panic.h"
#include "kernel/assert.h"
#include "kernel/kmalloc.h"
#include "kernel/interrupt.h"

/**@name Metadevices
 *
 * This module implements drivers for so called metadevices. These are
 * devices documented in YAMS specification as non-peripheral devices.
 * Metadevices include RTC (Real Time Clock), MemInfo (Memory
 * Information) and shutdown device.
 *
 * @{
 */


/* A helper to the init functions. Fills the given device structure to
 * values from the given IO descriptor and uses "null" values for the
 * rest. Allocates memory for the structure if it was given as NULL
 */
static device_t *fill_device_t(io_descriptor_t *desc, device_t *dev)
{
    if (dev == NULL)
	dev = kmalloc(sizeof(device_t));
    if (dev == NULL)
	KERNEL_PANIC("Run out ouf memory when allocating struct"
                     " for a metadevice");

    dev->real_device = NULL;
    dev->generic_device = NULL;
    dev->descriptor = desc;
    dev->io_address = desc->io_area_base;
    dev->type = desc->type;

    return dev;
}

/** Used to save the RTC, meminfo and shutdown devices, as there is
 * only one of each in the system
 */
static device_t system_rtc, system_meminfo, system_shutdown;

/** Initializes the RTC (Real Time Clock) device. Note that the system
 * should have only one RTC.
 *
 * @param desc Pointer to the YAMS IO device descriptor of the RTC
 * @return Pointer to the device structure of the RTC 
 */
device_t *rtc_init(io_descriptor_t *desc)
{
    static int init_done = 0;

    if (init_done)
	KERNEL_PANIC("Hardware failure: multiple RTC devices");

    init_done = 1;

    fill_device_t(desc, &system_rtc);

    return &system_rtc;
}

/** Get number of milliseconds elapsed since system startup from the
 * system RTC. Reads the MSEC port of the YAMS RTC device.
 *
 * @return Number of milliseconds elapsed
 */
uint32_t rtc_get_msec()
{
    return *((uint32_t*)system_rtc.io_address); /* MSEC @0x00 */
}

/** Get the machine (virtual/simulated) clock speed in hertz from the
 * system RTC. Reads the CLKSPD port of the YAMS RTC device.
 *
 * @return Machine clockspeed in Hz
 */
uint32_t rtc_get_clockspeed()
{
    return *((uint32_t*)(system_rtc.io_address+4)); /* CLKSPD @0x04 */
}


/** Initializes the system memory information (meminfo) device. Note
 * that the system should have only one meminfo device.
 *
 * @param desc Pointer to the YAMS IO device descriptor of the meminfo device
 * @return Pointer to the device structure of the meminfo device
 */
device_t *meminfo_init(io_descriptor_t *desc)
{
    static int init_done = 0;

    if (init_done)
	KERNEL_PANIC("Hardware failure: multiple MEMINFO devices");

    init_done = 1;

    fill_device_t(desc, &system_meminfo);

    return &system_meminfo;
}

/** Get the number of physical memory pages (4096 bytes/page) in the
 * machine from the system meminfo device. Reads the PAGES port of the
 * YAMS meminfo device.
 *
 * @return Number of physical pages
 */
uint32_t meminfo_get_pages()
{
    return *((uint32_t*)system_meminfo.io_address); /* PAGES @0x00 */
}


/** Initializes a CPU status device. These devices are currently used
 * for detecting the total number of CPUs in the system. In addition
 * to this a mechanism for generating interrupts on the CPU is
 * supported.
 *
 * @param desc Pointer to the YAMS IO device descriptor of the CPU
 * status device 
 *
 * @return Pointer to the device structure of the CPU status device
 */
device_t *cpustatus_init(io_descriptor_t *desc)
{
    device_t *dev;
    cpu_real_device_t *cpu;
    uint32_t irq_mask;

    dev = fill_device_t(desc, NULL);

    cpu = kmalloc(sizeof(cpu_real_device_t));
    if (cpu == NULL) 
        KERNEL_PANIC("Could not reserve memory for CPU status device driver.");
    spinlock_reset(&cpu->slock);

    dev->real_device = cpu;

    irq_mask = 1 << (desc->irq + 10);
    interrupt_register(irq_mask, cpustatus_interrupt_handle, dev);

    return dev;
}


/* Count the number of CPU pseudodevices found in the IO descriptor
 * area and return it.
 *
 * @return The number of CPUs in the system.
 */
int cpustatus_count() {
    io_descriptor_t *dev;
    int i, numcpu = 0;

    dev = (io_descriptor_t*)IO_DESCRIPTOR_AREA;

    /* search _all_ descriptors (see YAMS documentation) */
    for (i=0; i<128; i++) {
	if ((dev->type & 0xFFFFFF00) == YAMS_TYPECODE_CPUSTATUS)
	    numcpu++;
	dev++;
    }

    return numcpu;
}

/**
 * Generate IRQ on given CPU. 
 *
 * @param dev Device descriptor for CPU
 *
 */

void cpustatus_generate_irq(device_t *dev)
{
    interrupt_status_t intr_status;
    volatile cpu_io_area_t *iobase = (cpu_io_area_t *)dev->io_address;
    cpu_real_device_t *cpu = (cpu_real_device_t *)dev->real_device;

    KERNEL_ASSERT(dev != NULL && cpu != NULL);

    intr_status = _interrupt_disable();
    spinlock_acquire(&cpu->slock);

    /* If you really want to do something with inter-cpu interrupts,
       do it here.*/

    /* Generate the IRQ */
    iobase->command = CPU_COMMAND_RAISE_IRQ;

    spinlock_release(&cpu->slock);
    _interrupt_set_state(intr_status);
}

/**
 * Interrupt handler for the CPU status device. Currently the CPU
 * status device driver only supports ping-ponging interrupts between
 * CPUs.
 *
 * @param device Pointer to the CPU status device
 */
void cpustatus_interrupt_handle(device_t *dev){
    volatile cpu_io_area_t *iobase = (cpu_io_area_t *)dev->io_address;
    cpu_real_device_t *cpu = (cpu_real_device_t *)dev->real_device;
    uint32_t this_cpu;

    KERNEL_ASSERT(dev != NULL || cpu != NULL);
    this_cpu = _interrupt_getcpu();

    /* Check that dev is the status device of this CPU */
    if (this_cpu != (dev->type & 0xFF))
        return;

    /* Check that dev has actually generated the interrupt */
    if (!(CPU_STATUS_IRQ(iobase->status)))
        return;

    spinlock_acquire(&cpu->slock);

    /* If you really want to do something with inter-cpu interrupts,
       do it here. */

    /* Clear the interrupt */
    iobase->command = CPU_COMMAND_CLEAR_IRQ;
    
    spinlock_release(&cpu->slock);
}

/** 
 * Indicate whether shotdown device has been initialized. This
 * variable is used by several functions (shutdown_init and shutdown)
 * and needs therefore to be public unlike other indicators for
 * metadevice initialization.
 */
static int shutdown_init_done = 0;

/** Initializes the (software) shutdown device. Note that the system
 * should have only one shutdown device.
 *
 * @param desc Pointer to the YAMS IO device descriptor of the shutdown device
 * @return Pointer to the device structure of the shutdown device 
 */
device_t *shutdown_init(io_descriptor_t *desc)
{
    if (shutdown_init_done)
	KERNEL_PANIC("Hardware failure: multiple SHUTDOWN devices");

    shutdown_init_done = 1;

    fill_device_t(desc, &system_shutdown);

    return &system_shutdown;
}

/** Shutdown the system with the given magic word. This function can
 * be callled even though the shutdown device is not initialized
 * (kernel should always be able to panic). Writes the magic word to
 * the SHUTDN port of the YAMS shutdown device.
 *
 * @param magic The magic word to shutdown with, should be either
 * DEFAULT_SHUTDOWN_MAGIC or POWEROFF_SHUTDOWN_MAGIC
 */
void shutdown(uint32_t magic)
{
    int i;
    io_descriptor_t *descriptor;

    /* If init done, use the saved IO-area */
    if (shutdown_init_done) {
	*((uint32_t*)system_shutdown.io_address) = magic; /* SHUTDN @0x00 */
    }

    /* Else, find the shutdown device: */

    /* Search all device descriptors for the matching type: */
    descriptor = (io_descriptor_t*)IO_DESCRIPTOR_AREA;
    for (i=0; i<YAMS_MAX_DEVICES; i++) {
	/* match found, shutdown: */
	if (descriptor->type == YAMS_TYPECODE_SHUTDOWN) 
	    *((uint32_t*)descriptor->io_area_base) = magic; 

	descriptor++;
    }
    
    /* Uh-oh: no shutdown device! */
    kprintf("Shutdown failed. Hardware failure!\n");
}

/** @} */
