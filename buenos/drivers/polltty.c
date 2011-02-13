/*
 * Polling TTY driver.
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
 * $Id: polltty.c,v 1.13 2003/04/03 15:22:46 tlilja Exp $
 *
 */

#include "drivers/device.h"
#include "drivers/tty.h"

/** @name Polling TTY driver
 * 
 * This module implements the polling TTY driver. The polling TTY
 * driver is used to print kernel messages and to get input from the
 * user to the kernel. Since the driver is a polling driver, the I/O
 * will be performed before advancing in the code.
 *
 * @{
 */

/** 
 * The io base area for the polling TTY. This is initialized in the
 * initialization function of the polling TTY driver. After that it
 * can be used when outputting or inputting characters. If no TTY is
 * found, the value of this variable is zero.
 */
static volatile tty_io_area_t *polltty_iobase;

/**
 * Initializes the polling TTY driver. Goes through the IO descriptors
 * provided by YAMS searching for the first TTY. Sets
 * \texttt{polltty\_iobase} to point to the io base area of the
 * polling TTY. If no TTY is found sets \texttt{polltty\_iobase} to
 * zero.
 */
void polltty_init() 
{
    io_descriptor_t *io_desc;

    io_desc = (io_descriptor_t *)IO_DESCRIPTOR_AREA;
    
    /* Find first TTY and store its io base */
    while(io_desc->type != 0) {
        if (io_desc->type == 0x201) {
            polltty_iobase = (tty_io_area_t *) io_desc->io_area_base;
            return;
        }
        io_desc++;
    }

    /* No TTY found */
    polltty_iobase = 0;
}

/**
 * Gets one character from the TTY.
 *
 * @return The character read from the TTY. If the
 * \texttt{polltty_iobase} is invalid, returns 0.
 *
 */
int polltty_getchar() 
{
    /* Check that the iobase is valid */
    if (polltty_iobase == 0)
        return 0;

    /* Wait until there is a character available */
    while(TTY_STATUS_RAVAIL(polltty_iobase->status) == 0);

    /* Clear interrupt */
    polltty_iobase->command = TTY_COMMAND_RIRQ;

    return polltty_iobase->data;
}

/**
 * Outputs one character to the TTY. If the \texttt{polltty_iobase} is
 * invalid nothing is done.
 *
 * @param c The character to be output to the TTY.
 *
 */
void polltty_putchar(char c)
{
    /* Check that the iobase is valid */
    if (polltty_iobase != 0) {
    
        /* Wait until the TTY is no longer busy */
        while(TTY_STATUS_WBUSY(polltty_iobase->status) != 0);
        polltty_iobase->data = c;
        while(TTY_STATUS_WBUSY(polltty_iobase->status) != 0);

        /* We can't clear the interrupt here because it will break the
         * interrupt driven tty driver. The IRQ will be handled later
         * by the interrupt driven tty driver.
         */
    }
}

/** @} */
