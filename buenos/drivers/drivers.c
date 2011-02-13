/*
 * Drivers
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
 * $Id: drivers.c,v 1.5 2003/03/27 11:38:16 javirta2 Exp $
 *
 */
#include "lib/libc.h"
#include "drivers/drivers.h"
#include "drivers/tty.h"
#include "drivers/disk.h"
#include "drivers/yams.h"
#include "drivers/metadev.h"

/**  
 * A table of available device drivers.
 */
drivers_available_t drivers_available[] = {
    {YAMS_TYPECODE_TTY, "Console", &tty_init} ,
    {YAMS_TYPECODE_RTC, "System RTC", &rtc_init} ,
    {YAMS_TYPECODE_MEMINFO, "System memory information", &meminfo_init} ,
    {YAMS_TYPECODE_SHUTDOWN, "System shutdown", &shutdown_init} ,
    {YAMS_TYPECODE_CPUSTATUS, "CPU status", &cpustatus_init} ,
    {YAMS_TYPECODE_DISK, "Disk", &disk_init},
    {0, NULL, NULL}
};
