/*
 * List of available filesystem drivers.
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
 * $Id: filesystems.c,v 1.2 2004/01/13 11:26:04 ttakanen Exp $
 *
 */

#include "fs/filesystems.h"
#include "fs/tfs.h"
#include "drivers/device.h"

/* NULL terminated table of all available filesystems. */

static filesystems_t filesystems[] = {
    {"TFS", &tfs_init},
    { NULL, NULL} /* Last entry must be a NULL pair. */ 
};

/**
 * Tries to mount given GBD (disk) with all available filesystem drivers.
 *
 * @param disk Generic Block Device on which a filesystem is supposed to be.
 *
 * @return Initialized filesystem (first match), or NULL if all mount
 * attempts failed.
 */

fs_t *filesystems_try_all(gbd_t *disk)
{
    filesystems_t *driver;
    fs_t *fs;

    for(driver=filesystems; driver->name != NULL; driver++) {
	fs=driver->init(disk);
	if(fs!=NULL) {
	    /* Init succeeded. */
	    kprintf("VFS: %s initialized on disk at 0x%8.8x\n",
		    driver->name, disk->device->io_address);
	    return fs;
	}
    }

    /* No match. */
    return NULL;
}
