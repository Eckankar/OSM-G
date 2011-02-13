/*
 * Kernel boot arguments.
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
 * $Id: bootargs.c,v 1.6 2004/02/18 14:33:07 ttakanen Exp $
 *
 */

#include <lib/libc.h>
#include <drivers/device.h>
#include "kernel/kmalloc.h"
#include "kernel/config.h"

/** @name Kernel boot arguments
 *
 *  This module implements kernel boot argument handling. YAMS
 *  will store boot arguments to a special memory address for
 *  the kernel. Initialization function will fetch this string
 *  and parse it into (key, value) pairs. The arguments can
 *  be accessed with a key.
 *
 *  Boot arguments are given in form "key1=value1 key2=value2".
 *   @{
 */

/**
 * Table for storing the boot argument strings. The length of the table
 * is fixed.
 *
 */

static struct bootargs_t {
    char *key;
    char *value;
} bootargs_values[CONFIG_BOOTARGS_MAX];


static void bmemcopy(void *target, const void *source, int len)
{
    while(len-- > 0) {
	*((uint8_t *) target++) = *((uint8_t *) source++);
    }
}

/**
 * Initializes the boot argument system. Reads boot arguments from
 * YAMS defined memory address.
 *
 */

void bootargs_init(void)
{
    int l,i,o,last;
    char *bootarg_area = (char *) BOOT_ARGUMENT_AREA;
    char *value_area = NULL;

    for(i=0; i<CONFIG_BOOTARGS_MAX; i++) {
	bootargs_values[i].key = NULL;
    }
    
    l = strlen(bootarg_area);
    value_area = (char *) kmalloc(l+1);
    bmemcopy(value_area, bootarg_area, l+1);

    i=0; o=0; last=0;

    while(o<=l) {
	/* search for = or end of key (space or 0) */
	if(*(value_area+o) == ' ' ||
	   *(value_area+o) == 0   ||
	   *(value_area+o) == '=') {
	    int thiskey;
	    bootargs_values[i].key   = value_area+last;
	    bootargs_values[i].value = "";
	    thiskey=last;


	    last = o+1;


	    if(*(value_area+o) == '=') {
		*(value_area+o) = 0;
		/* we have value for this key */
		o++;
		while(o<=l) {
		    if(*(value_area+o) == ' ' ||
		       *(value_area+o) == 0) {
			/* value ends */
			bootargs_values[i].value = value_area+last;
			*(value_area+o) = 0;
			o++;
			last=o;
			break;
		    }
		    o++;
		}
	    } else {
		*(value_area+o) = 0;
		o++;
	    }

	    i++;
	} else {
	    o++;
	}
    }
}

/**
 * Gets specified boot argument. 
 *
 * @param key The key to search for.
 *
 * @return Value of the key. If key is not found, NULL is returned.
 *
 */

char *bootargs_get(char *key)
{
    int i,o;

    for(i=0; i<CONFIG_BOOTARGS_MAX; i++) {
	if(bootargs_values[i].key == NULL)
	    return NULL;

	o=0;
	while(*(bootargs_values[i].key+o) == 
	      *(key+o)) {
	    if(*(key+o) == 0)
		return bootargs_values[i].value;

	    o++;
	}
    }
    
    return NULL;
}

/** @} */
