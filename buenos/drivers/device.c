/*
 * Devices.
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
 * $Id: device.c,v 1.3 2003/03/27 09:30:16 jaatroko Exp $
 *
 */

#include "lib/libc.h"
#include "drivers/device.h"
#include "kernel/config.h"
#include "drivers/drivers.h"

/**@name Device Drivers
 *
 * This module contains functions for initializing device drivers and
 * maintaining a list of found devices.
 *
 * @{
 */

/** Table of initialized device drivers. */
static device_t *device_table[CONFIG_MAX_DEVICES];

/** Number of initialized device drivers. */
static int number_of_devices = 0;

/**
 * Finds a driver for a given type of device.
 *
 * @param typecode The typecode of the device.
 *
 * @return Device driver descriptor. NULL if not found.
 */
static drivers_available_t *find_driver(uint32_t typecode)
{
    drivers_available_t *driver;

    driver = drivers_available;

    if ((typecode & YAMS_TYPECODE_CPUMASK) == YAMS_TYPECODE_CPUSTATUS)
	typecode = YAMS_TYPECODE_CPUSTATUS;

    while(driver->typecode != 0) {
        if (driver->typecode == typecode)
            return driver;
        driver++;
    }
    return NULL;
}

/**
 * Initializes all device drivers. Tries to find a driver for each
 * device in the system and initializes the found drivers.
 *
 * Initialized drivers are stored to device_table and can be
 * accessed by calling device_get.
 *
 */

void device_init(void)
{
    int i;
    io_descriptor_t *descriptor;
    drivers_available_t *driver;

    descriptor = (io_descriptor_t*)IO_DESCRIPTOR_AREA;

    /* search _all_ descriptors (see YAMS documentation) */
    for (i=0; i<YAMS_MAX_DEVICES; i++) {
        if (descriptor->type != 0) {
            driver = find_driver(descriptor->type);
            if (driver == NULL) {
                kprintf("Warning: Unknown hardware device type "
                        "0x%3.3x at 0x%8.8x\n",
                        descriptor->type, descriptor->io_area_base);
            } else {
                if (descriptor->irq != 0xffffffff)  
                    kprintf("Device: Type 0x%3.3x at 0x%8.8x irq 0x%x "
                            "driver '%s'\n",
                            descriptor->type, descriptor->io_area_base,
                            descriptor->irq, driver->name);
                else
                    kprintf("Device: Type 0x%3.3x at 0x%8.8x no irq  "
                            "driver '%s'\n",
                            descriptor->type, descriptor->io_area_base,
                            driver->name);
                
                device_table[number_of_devices]=driver->initfunc(descriptor);
                if (device_table[number_of_devices] != NULL) {
                    number_of_devices++;
                    if (number_of_devices >= CONFIG_MAX_DEVICES)
                        break;
                }
            }
        }
	descriptor++;
    }

}

/**
 * Gets device driver of a given device.
 *
 * @param typecode The typecode of the device
 *
 * @param n The n:th device of given type is returned. Indexing begins
 * from 0.
 *
 * @return The device driver. NULL if not found or n too large.
 */
device_t *device_get(uint32_t typecode, uint32_t n)
{
    int i;

    for(i = 0; i < number_of_devices; i++) {
        if (device_table[i]->type == typecode) {
            if (n == 0)
                return device_table[i];
            else
                n--;
        }
    }
    return NULL;
}

/** @} */
