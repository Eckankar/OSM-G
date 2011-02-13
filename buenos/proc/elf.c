/*
 * ELF binary format.
 *
 * Copyright (C) 2003-2005 Juha Aatrokoski, Timo Lilja,
 *       Leena Salmela, Teemu Takanen, Aleksi Virtanen.
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
 * $Id: elf.c,v 1.2 2005/05/10 10:33:19 lsalmela Exp $
 *
 */
#include "proc/elf.h"
#include "lib/libc.h"
#include "drivers/yams.h"

/** @name ELF loader.
 *
 * This module contains a function to parse useful information from
 * ELF headers.
 */

/**
 * Parse useful information from a given ELF file into the ELF info
 * structure.
 *
 * @param file The ELF file
 *
 * @param elf Information found in the file is returned in this
 * structure. In case of error this structure may contain arbitrary
 * values.
 *
 * @return 0 on failure, other values indicate success.
 */
int elf_parse_header(elf_info_t *elf, openfile_t file)
{
    Elf32_Ehdr elf_hdr;
    Elf32_Phdr program_hdr;

    int i;
    int current_position;
    int segs = 0;
#define SEG_RO 1
#define SEG_RW 2

    /* Read the ELF header */
    if (vfs_read(file, &elf_hdr, sizeof(elf_hdr))
	!= sizeof(elf_hdr)) {
        return 0;
    }

    /* Check that the ELF magic is correct. */
    if (EI_MAGIC(elf_hdr.e_ident) != ELF_MAGIC) {
        return 0;
    }

    /* File data is not MIPS 32 bit big-endian */
    if (elf_hdr.e_ident[EI_CLASS] != ELFCLASS32
	|| elf_hdr.e_ident[EI_DATA] != ELFDATA2MSB
	|| elf_hdr.e_machine != EM_MIPS) {
	return 0;
    }

    /* Invalid ELF version */
    if (elf_hdr.e_version != EV_CURRENT 
	|| elf_hdr.e_ident[EI_VERSION] != EV_CURRENT) {
	return 0;
    }

    /* Not an executable file */
    if (elf_hdr.e_type != ET_EXEC) {
	return 0;
    }

    /* No program headers */
    if (elf_hdr.e_phnum == 0) {
	return 0;
    }

    /* Zero the return structure. Uninitialized data is bad(TM). */
    memoryset(elf, 0, sizeof(*elf));

    /* Get the entry point */
    elf->entry_point = elf_hdr.e_entry;

    /* Seek to the program header table */
    current_position = elf_hdr.e_phoff;
    vfs_seek(file, current_position);

    /* Read the program headers. */
    for (i = 0; i < elf_hdr.e_phnum; i++) {
	if (vfs_read(file, &program_hdr, sizeof(program_hdr))
	    != sizeof(program_hdr)) {
	    return 0;
	}

	switch (program_hdr.p_type) {
	case PT_NULL:
	case PT_NOTE:
	case PT_PHDR:
	    /* These program headers can be ignored */
	    break;
	case PT_LOAD:
	    /* These are the ones we are looking for */

	    /* The RW segment */
	    if (program_hdr.p_flags & PF_W) {
		if (segs & SEG_RW) { /* already have an RW segment*/
		    return 0;
		}
		segs |= SEG_RW;

		elf->rw_location = program_hdr.p_offset;
		elf->rw_size = program_hdr.p_filesz;
		elf->rw_vaddr = program_hdr.p_vaddr;
		/* memory size rounded up to the page boundary, in pages */
		elf->rw_pages = 
		    (program_hdr.p_memsz + PAGE_SIZE - 1) / PAGE_SIZE;

	    /* The RO segment */
	    } else {
		if (segs & SEG_RO) { /* already have an RO segment*/
		    return 0;
		}
		segs |= SEG_RO; 

		elf->ro_location = program_hdr.p_offset;
		elf->ro_size = program_hdr.p_filesz;
		elf->ro_vaddr = program_hdr.p_vaddr;
		/* memory size rounded up to the page boundary, in pages */
		elf->ro_pages = 
		    (program_hdr.p_memsz + PAGE_SIZE - 1) / PAGE_SIZE;
	    }

	    break;
	default:
	    /* Other program headers indicate an incompatible file */
	    return 0;
	}

	/* In case the program header size is non-standard: */
	current_position += sizeof(program_hdr);
	vfs_seek(file, current_position);
    }

    /* Make sure either RW or RO segment is present: */
    return (segs > 0);
}

/** @} */
