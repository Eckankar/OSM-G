/*
 * Generic network device
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
 * $Id: gnd.h,v 1.4 2005/05/09 11:22:00 lsalmela Exp $
 *
 */

#ifndef DRIVERS_GND_H
#define DRIVERS_GND_H

#include "drivers/device.h"

typedef uint32_t network_address_t;

/* Generic network device descriptor. */
typedef struct gnd_struct {
    /* Pointer to the device */
    device_t *device;

    /* Pointer to a function which sends one network frame to addr.
     * The frame must be in the format defined by the media. (For YAMS
     * network that means that the first 8 bytes are filled in the
     * network layer and the rest is data). The frame must have the
     * size returned by the function frame_size below. The call of
     * this function will block until the frame is sent. The return
     * value 0 means success. Other values are failures.
     *
     * Note: The pointer to the frame must be a PHYSICAL address, not
     * a segmented one.
     */
    int (*send)(struct gnd_struct *gnd, void *frame, network_address_t addr);

    /* Pointer to a function which receives one network frame. The
     * frame will be in the format defined by the media. (For YAMS
     * network that means that the first 8 bytes are filled in the
     * network layer and the rest is data). The frame must have the
     * size returned by the function frame_size below. The call of
     * this function will block until a frame is received. The return
     * value 0 means success. Other values are failures.
     *
     * Note: The pointer to the frame must be a PHYSICAL address, not
     * a segmented one.
     */
    int (*recv)(struct gnd_struct *gnd, void *frame);

    /* Pointer to a function which returns the size of the network
     * frame for the media in octets.
     */
    uint32_t (*frame_size)(struct gnd_struct *gnd);

    /* Pointer to a function which returns the hardware address (MAC)
     * of the interface.
     */
    network_address_t (*hwaddr)(struct gnd_struct *gnd);

} gnd_t;

#endif /* DRIVERS_GND_H */



