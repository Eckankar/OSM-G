/*
 * Stream protocol layer
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
 * $Id: sop.h,v 1.2 2003/05/15 13:41:03 lsalmela Exp $
 *
 */

#ifndef NET_SOP_H
#define NET_SOP_H

#include "lib/types.h"
#include "net/network.h"
#include "net/socket.h"

/* NOTE: None of these functions are implemented, this is just an API. */

/* Initialization function for streaming protocol. Implements interface
to protocols_init() */
void sop_init();


/* Implementation for frame_handler_t (protocols.h) for streaming protocol. */
int sop_push_frame(network_address_t fromaddr,
		   network_address_t toaddr,
		   uint32_t protocol_id,
		   void *frame);

/* Connect to remote address addr, port port with given socket s.
Return 0 on success and 1 on failure. */
int socket_connect(sock_t s, network_address_t addr, int port);

/* Wait until some remote entity has connected to given socket s. */
void socket_listen(sock_t s);

/* Read at most length bytes from given socket s to buffer buf.
Return number of bytes read, zero on end of stream and negative on error. */
int socket_read(sock_t s, void *buf, int length);

/* Write length bytes from buffer buf to socket s. Return number of
bytes delivered to target socket. If return value is not equal to
length, connection (and some data) has been lost. */
int socket_write(sock_t s, void *buf, int length);

#endif /* NET_SOP_H */
