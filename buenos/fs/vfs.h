/*
 * Virtual Filesystem (VFS).
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
 * $Id: vfs.h,v 1.9 2003/05/21 13:59:25 lsalmela Exp $
 *
 */

#ifndef BUENOS_FS_VFS_H
#define BUENOS_FS_VFS_H

#include "drivers/gbd.h"

/* Return codes for filesytem functions. Positive return values equal
   to VFS_OK */
#define VFS_OK 0
#define VFS_NOT_SUPPORTED -1
#define VFS_ERROR -2
#define VFS_INVALID_PARAMS -3
#define VFS_NOT_OPEN -4
#define VFS_NOT_FOUND -5
#define VFS_NO_SUCH_FS -6
#define VFS_LIMIT -7
#define VFS_IN_USE -8
#define VFS_UNUSABLE -9

/* Maximum length of filename without mountpoint and directory names. */
#define VFS_NAME_LENGTH 16

/* Maximum length of the full name of a file, including mountpoint and
   directories. */
#define VFS_PATH_LENGTH 256

/* Type for open file entries. This is actually index to open files table */
typedef int openfile_t;

/* Structure defining a filesystem driver instance for one filesystem.
   Instances of this structure are created by filesystem init-function and
   they are used only inside VFS. */
typedef struct fs_struct{
    /* Driver internal data */
    void *internal;
    /* Name of the volume (advisory mounpoint) */
    char volume_name[16];

    /* Function pointer to a function which will be called when this
       filesystem is unmounted. In this call the driver must flush
       all cached data to disk and release allocated structures,
       including the filesystem structure given as argument. */ 
    int (*unmount)(struct fs_struct *fs); 

    /* Function pointer to a function which opens a file in the
       filesystem. A pointer to this structure as well as name of a
       file is given as argument. Returns non-negative file id which
       must be unique for this filesystem. Negative values are errors. */
    int (*open)(struct fs_struct *fs, char *filename);

    /* Function pointer to a function which closes the given (open) file.
       A pointer to this structure as will as fileid previously returned
       by open is given as argument. Zero return value is success,
       negative values are errors. */
    int (*close)(struct fs_struct *fs, int fileid);

    /* Function pointer to a function which reads at most bufsize
       bytes from given open file (fileid). A pointer to this
       structure is given as first argument. The data is read to given
       buffer and read is started from given absolute (not relative to
       seek position) offset in the file.

       The number of bytes actually read is returned. Zero is returned
       only at the end of the file (nothing to read). Negative values
       are errors. */
    int (*read)(struct fs_struct *fs, int fileid, void *buffer,
		int bufsize, int offset);

    /* Function pointer to a function which writes datasize
       bytes to given open file (fileid). A pointer to this
       structure is given as first argument. The data is read from given
       buffer and write is started from given absolute (not relative to
       seek position) offset in the file.

       The number of bytes actually written is returned. Any value
       other than datasize as return code is error.
    */
    int (*write)(struct fs_struct *fs, int fileid, void *buffer,
		 int datasize, int offset);

    /* Function pointer to a function which creates new file in the
       filesystem. A pointer to this structure is given as the first
       argument, name of the file to be created as second argument and
       size of the created file as third. 

       Returns success value as defined above (VFS_OK, etc.) */
    int (*create)(struct fs_struct *fs, char *filename, int size);

    /* Function pointer to a function which removes file from the
       filesystem. A pointer to this structure is given as the first
       argument and name of the file to be deleted as the second argument.

       Returns success value as defined above (VFS_OK, etc.) */
    int (*remove)(struct fs_struct *fs, char *filename);

    /* Function pointer to a function which returns the number of free
       bytes in the filesystem. Pointer to this structure is given as
       argument to the function. 

       Returns the number of free bytes, negative values are errors. */
    int (*getfree)(struct fs_struct *fs);
} fs_t;


void vfs_init(void);
void vfs_mount_all(void);
void vfs_deinit(void);

int vfs_mount_fs(gbd_t *disk, char *volumename);
int vfs_mount(fs_t *fs, char *name);
int vfs_unmount(char *name);

openfile_t vfs_open(char *pathname);
int vfs_close(openfile_t file);
int vfs_seek(openfile_t file, int seek_position);
int vfs_read(openfile_t file, void *buffer, int bufsize);
int vfs_write(openfile_t file, void *buffer, int datasize);

int vfs_create(char *pathname, int size);
int vfs_remove(char *pathname);
int vfs_getfree(char *filesystem);

#endif
