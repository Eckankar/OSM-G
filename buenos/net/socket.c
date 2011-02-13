/*
 * Common socket functionality
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
 * $Id: socket.c,v 1.5 2004/01/13 11:32:06 ttakanen Exp $
 *
 */

#include "net/socket.h"
#include "net/pop.h"
#include "net/protocols.h"
#include "kernel/config.h"
#include "kernel/semaphore.h"
#include "kernel/panic.h"
#include "kernel/assert.h"
#include "vm/pagepool.h"
#include "lib/types.h"

/* open socket table and a semaphore to synch access to it */
socket_descriptor_t open_sockets[CONFIG_MAX_OPEN_SOCKETS];
semaphore_t *open_sockets_sem;


/** Initializes the socket system. Creates the semaphore and sets
 *  the open socket table entries to null values.
 */
void socket_init()
{
    static int init_done = 0;
    int i;

    /* Make sure this is not called more than once */
    KERNEL_ASSERT(!init_done);
    init_done = 1;


    open_sockets_sem = semaphore_create(1);

    if (open_sockets_sem == NULL) {
	KERNEL_PANIC("socket_init: semaphore allocation failed\n");
    }

    /* init socket table */
    for (i=0; i<CONFIG_MAX_OPEN_SOCKETS; i++) {
	open_sockets[i].port = 0;
	open_sockets[i].protocol = 0;
	open_sockets[i].rbuf = NULL;
	open_sockets[i].bufsize = 0;
	open_sockets[i].sender = NULL;
	open_sockets[i].copied = NULL;
	open_sockets[i].sport = NULL;
	open_sockets[i].receive_complete = NULL;
    }

}



/** Opens a socket to be used for subsequent network communication.
 * The socket is bound to the given port and will be of the given
 * protocol, which should be one of the supported ones (POP or SOP)
 *
 * @param protocol The protocol of the socket
 * @param port The port to * bind this socket to. If 0, a free port is
 * selected automatically.
 *
 * @return The opened socket (which is the index of the socket in 
 * the socket table) or negative on failure.
 */
sock_t socket_open(uint8_t protocol, uint16_t port)
{
    int i, s;

    /* protocol must be supported: */
    if (protocol != PROTOCOL_POP && protocol != PROTOCOL_SOP)
	return -1;

    semaphore_P(open_sockets_sem);

    /* find an empty slot from the table */
    for (i=0; i<CONFIG_MAX_OPEN_SOCKETS; i++) {
	if (open_sockets[i].protocol == 0)
	    break;
    }

    /* socket table full, return error */
    if (i == CONFIG_MAX_OPEN_SOCKETS) {
	semaphore_V(open_sockets_sem);
	return -1;
    }
    s = i;

    if (port == 0) { /* find the first free port */
	int notfree = 1;

	while (notfree) {
	    port++;
	    notfree = 0;
	    
	    for (i=0; i<CONFIG_MAX_OPEN_SOCKETS; i++) {
		if (open_sockets[i].protocol != 0 &&
		    open_sockets[i].port == port) {
		    notfree = 1;
		    break;
		}
	    }
	    
	}
	
	KERNEL_ASSERT(port != 0);

    } else { /* check that the port is unused */
	for (i=0; i<CONFIG_MAX_OPEN_SOCKETS; i++) {
	    if (open_sockets[i].protocol != 0 &&
		open_sockets[i].port == port) {
		semaphore_V(open_sockets_sem);
		return -1;
	    }
	}
    }

    /* allocate the signaling semaphore*/
    open_sockets[s].receive_complete = semaphore_create(0);
    if (open_sockets[s].receive_complete == NULL) {
	semaphore_V(open_sockets_sem);
	return -1;
    }

    /* init the entry */
    open_sockets[s].port = port;
    open_sockets[s].protocol = protocol;
    open_sockets[s].rbuf = NULL;
    open_sockets[s].bufsize = 0;
    open_sockets[s].sender = NULL;
    open_sockets[s].copied = NULL;

    semaphore_V(open_sockets_sem);

    return s;
}


/** Close the given socket. The socket must not be used after this
 * operation.
 *
 * @param socket The socket to be closed
 */
void socket_close(sock_t socket)
{
    /* check sanity */
    KERNEL_ASSERT(socket >= 0 && socket < CONFIG_MAX_OPEN_SOCKETS);

    semaphore_P(open_sockets_sem);

    /* zero the entry if it is an open socket */
    if (open_sockets[socket].receive_complete != NULL) {
	open_sockets[socket].port = 0;
	open_sockets[socket].protocol = 0;
	open_sockets[socket].rbuf = NULL;
	open_sockets[socket].bufsize = 0;
	open_sockets[socket].sender = NULL;
	open_sockets[socket].copied = NULL;
	
	semaphore_destroy(open_sockets[socket].receive_complete);
	open_sockets[socket].receive_complete = NULL;
    }

    semaphore_V(open_sockets_sem);
}
