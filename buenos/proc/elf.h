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
 * $Id: elf.h,v 1.2 2007/04/04 12:27:27 jaatroko Exp $
 *
 */

#ifndef BUENOS_PROC_ELF
#define BUENOS_PROC_ELF

#include "lib/types.h"
#include "fs/vfs.h"


/* Return data type for the ELF header parser. This structure
 * contains the information about RO and RW segments and the entry point.
 *
 * In theory, there are four kinds of segments that could be in the
 * executable: the program code (.text), read-only data (.rodata),
 * read-write initialized data (.data) and read-write uninitialized
 * data (.bss, not stored in the binary). However, the MIPS memory
 * architecture can only differentiate between RO and RW pages. Thus
 * there are only two segment types in practice: RO, which includes
 * program code and read-only data, and RW, which includes all
 * writable data. The GNU ELF ld will do this grouping into program
 * segments even if not specifically instructed so in the linker
 * script. This is why this data type only needs two segments.
 *
 * NB: since the RW segment includes bss, pages needed may (of course)
 * be much larger than indicated by the size field.
 */
typedef struct {
    uint32_t entry_point; /* Entry point for the code */

    uint32_t ro_location; /* Location of RO segment in the file */
    uint32_t ro_size;     /* Size of RO segment in the file */
    uint32_t ro_pages;    /* Pages needed by the RO segment */
    uint32_t ro_vaddr;    /* Virtual address of the RO segment */

    uint32_t rw_location; /* Location of RW segment in the file */
    uint32_t rw_size;     /* Size of RW segment in the file */
    uint32_t rw_pages;    /* Pages needed by the RW segment */
    uint32_t rw_vaddr;    /* Virtual address of the RW segment */
} elf_info_t;

int elf_parse_header(elf_info_t *elf, openfile_t file);


/* These are directly from the TIS/ELF 1.2 specification */

#define ET_EXEC 2
#define EM_MIPS 8

#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6

#define ELFCLASS32 1
#define ELFDATA2MSB 2
#define EV_CURRENT 1

/* Cast the identification string into uint32_t */
#define EI_MAGIC(x) (*((uint32_t*)&(x)))
/* ELF magic, "\177ELF", big-endian format */
#define ELF_MAGIC 0x7f454c46

/* ELF file header. The structure fields are aligned by data type
 * size, so the packed-attribute is not really necessary. 
 */
#define EI_NIDENT 16
typedef struct {
    /* ELF identification data */
    unsigned char e_ident[EI_NIDENT] __attribute__ ((packed));
    /* Object file type (executable, relocatable...) */
    uint16_t e_type                  __attribute__ ((packed));
    /* Machine architecture (we want MIPS) */
    uint16_t e_machine               __attribute__ ((packed));
    /* ELF version */
    uint32_t e_version               __attribute__ ((packed));
    /* Program entry point virtual address */
    uint32_t e_entry                 __attribute__ ((packed));
    /* Program header table's file offset */
    uint32_t e_phoff                 __attribute__ ((packed));
    /* Section header table's file offset */
    uint32_t e_shoff                 __attribute__ ((packed));
    /* Processor specific flags */
    uint32_t e_flags                 __attribute__ ((packed));
    /* ELF header size in bytes */
    uint16_t e_ehsize                __attribute__ ((packed));
    /* Program header entry size */
    uint16_t e_phentsize             __attribute__ ((packed));
    /* Number of program headers */
    uint16_t e_phnum                 __attribute__ ((packed));
    /* Section header entry size */
    uint16_t e_shentsize             __attribute__ ((packed));
    /* Number of section headers */
    uint16_t e_shnum                 __attribute__ ((packed));
    /* Section header index of the section name string table */
    uint16_t e_shstrndx              __attribute__ ((packed));
} Elf32_Ehdr;

/* Segment types */
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7fffffff

/* Segment flags */
#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

/* ELF program header */
typedef struct {
    /* Segment type */
    uint32_t p_type   __attribute__ ((packed));
    /* Location in the file */
    uint32_t p_offset __attribute__ ((packed));
    /* Virtual address */
    uint32_t p_vaddr  __attribute__ ((packed));
    /* Physical address (not used on many platforms) */
    uint32_t p_paddr  __attribute__ ((packed));
    /* Number of bytes stored in the file */
    uint32_t p_filesz __attribute__ ((packed));
    /* Number of bytes this segment occupies in memory */
    uint32_t p_memsz  __attribute__ ((packed));
    /* Segment flags */
    uint32_t p_flags  __attribute__ ((packed));
    /* Alignment in file and memory */
    uint32_t p_align  __attribute__ ((packed));
} Elf32_Phdr;


#endif
