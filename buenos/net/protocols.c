/*
 * List of network protocols
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
 * $Id: protocols.c,v 1.4 2003/05/25 18:56:56 ttakanen Exp $
 *
 */

#include "net/protocols.h"
#include "net/pop.h"
#include "lib/types.h"
#include "lib/libc.h"

/** @name Network protocols 
 *
 * This module contains a list of available network protocols.
 */

/* A structure containing available network protocols */
typedef struct {
    /* The unique typecode of the protocol */
    uint32_t protocol_id;

    /* Function which handles incoming frames for this protocol. This
       function takes four arguments: source address destination
       address, protocol id and the payload. The return value zero
       means failure. Other values are interpreted as success. */
    frame_handler_t frame_handler;

    /* Initialization function for this protocol. */
    void (*init)(void);
} network_protocols_t;

/** List of available network protocols. */
network_protocols_t network_protocols[] = {
    {PROTOCOL_POP, &pop_push_frame, &pop_init},
    {0, NULL, NULL}
};

/** 
 * Gets the frame handler function for the given protocol_id.
 *
 * @param protocol_id The id of the protocol whose frame handler
 * should be returned.
 *
 * @return The frame handler function.
 *
 */
frame_handler_t protocols_get_frame_handler(uint32_t protocol_id)
{
    network_protocols_t *p;

    for(p = network_protocols; p->frame_handler != NULL; p++) {
	if(p->protocol_id == protocol_id)
	    return p->frame_handler;
    }
    
    return NULL;
}

/**
 * Initialize all network protocols.
 */
void protocols_init(void)
{
    network_protocols_t *p;

    for(p = network_protocols; p->frame_handler != NULL; p++) {
	p->init();
    }
}
