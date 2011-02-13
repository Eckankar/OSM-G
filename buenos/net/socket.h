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
 * $Id: socket.h,v 1.4 2003/05/17 17:33:37 jaatroko Exp $
 *
 */

#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include "lib/types.h"
#include "net/network.h"
#include "net/protocols.h"
#include "kernel/semaphore.h"

/* sock_t is an index to the open socket table 
 * valid values 0..CONFIG_MAX_OPEN_SOCKETS-1
 */
typedef int sock_t; 


/* Open socket structure */
typedef struct {
    uint16_t port;             /* port this socket is bound to */
    uint8_t protocol;          /* protocol of this socket */

    /* These are used when waiting for a packet on this socket */
    semaphore_t *receive_complete; /* signaled when packet arrived */
    void *rbuf;                    /* payload copied here */
    uint32_t bufsize;              /* size of the receive buffer */
    network_address_t *sender;     /* sender's address stored here */
    int *copied;                   /* bytes copied stored here */
    uint16_t *sport;               /* sender's port stored here */
} socket_descriptor_t;


/* function prototypes */
void socket_init();
sock_t socket_open(uint8_t protocol, uint16_t port);
void socket_close(sock_t socket);
int socket_sendto(sock_t s,
		  network_address_t addr,
		  uint16_t dport,
		  void *buf,
		  int size);
int socket_recvfrom(sock_t s,
		    network_address_t *addr,
		    uint16_t *sport,
		    void *buf,
		    int maxlength,
		    int *length);


#endif /* NET_SOCKET_H */



