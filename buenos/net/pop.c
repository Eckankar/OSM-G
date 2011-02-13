/*
 * POP (Packet Oriented Protocol) protocol layer
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
 * $Id: pop.c,v 1.9 2005/01/17 07:38:56 lsalmela Exp $
 *
 */

#include "net/socket.h"
#include "net/pop.h"
#include "net/network.h"
#include "net/protocols.h"
#include "kernel/config.h"
#include "kernel/semaphore.h"
#include "vm/pagepool.h"
#include "kernel/panic.h"
#include "kernel/assert.h"
#include "kernel/thread.h"
#include "lib/libc.h"
#include "drivers/metadev.h"
#include "kernel/interrupt.h"

/* socket data from socket.c */
extern socket_descriptor_t open_sockets[CONFIG_MAX_OPEN_SOCKETS];
extern semaphore_t *open_sockets_sem;

/* input queue to hold incoming packets and a semaphore to synch access */
static pop_queue_t pop_queue[CONFIG_POP_QUEUE_SIZE];
static semaphore_t *pop_queue_sem;

/* Buffer to hold packets that are being sent to the network (+ semaphore) */
static void *pop_send_buffer;
static semaphore_t *pop_send_buffer_sem;

/* The thread ID of the service thread */
static TID_t pop_service_thread_id;

/* Semaphore used to signal the service thread when there are new
 * packets in the queue 
 */
static semaphore_t *pop_service_thread_sem;




/** Send a POP packet to the network. The packet is sent to the given
 * address and port, using socket s. The data is read from buf, no
 * more than size bytes are sent (but no more bytes than fit in one
 * packet are sent either, so return value may be less than size).
 *
 * @param s     The socket to be used
 * @param addr  The address of the recipient
 * @param dport The destination port on the recipient host
 * @param buf   The buffer holding the payload of the packet
 * @param size  The size of the payload
 *
 * @return The number of bytes actually sent, or negative on error
 */
int socket_sendto(sock_t s,
		  network_address_t addr,
		  uint16_t dport,
		  void *buf,
		  int size)
{
    uint16_t sport;
    pop_header_t *hdr;
    int r;

    /* check sanity */
    KERNEL_ASSERT(s >= 0 && s < CONFIG_MAX_OPEN_SOCKETS);

    /* 0 is a special port and not used for communication */
    if (dport == 0)
	return -1;

    /* parameter sanity... */
    KERNEL_ASSERT(size >= 1 && buf != NULL);

    /* Limit the size to the MTU or page size */
    size = MIN(MIN((uint32_t)size, 
		   network_get_mtu(NETWORK_BROADCAST_ADDRESS) -
		   sizeof(pop_header_t)),
	       PAGE_SIZE - sizeof(pop_header_t));

    semaphore_P(open_sockets_sem);

    /* Check that it is a POP socket */
    if (open_sockets[s].protocol != PROTOCOL_POP) {
	semaphore_V(open_sockets_sem);
	return -1;
    }
    sport = open_sockets[s].port;

    semaphore_V(open_sockets_sem);

    semaphore_P(pop_send_buffer_sem);

    /* construct the header at the beginning of the send buffer */
    hdr = (pop_header_t *)pop_send_buffer;
    hdr->source_port = sport;
    hdr->dest_port = dport;
    hdr->size = size;

    /* Copy the payload to its place in the send buffer */
    memcopy(size,
	    (void*)((uint32_t)pop_send_buffer + sizeof(pop_header_t)),
	    buf);

    /* Send the packet through ALL network interfaces */
    r = network_send(NETWORK_BROADCAST_ADDRESS, /* source: don't care */
		     addr,                      /* destination */
		     PROTOCOL_POP,
		     size + sizeof(pop_header_t),
		     (void *)pop_send_buffer);

    /* Return value to error if send failed */
    if (r != NET_OK)
	size = -1;

    semaphore_V(pop_send_buffer_sem);
    
    return size;
}



/** Receive a POP packet from the network. Waits for a packet whose
 * destination is the given socket and copies the packet payload to
 * the given buffer. The sender's address and port are placed in add
 * and sport.
 *
 * @param s         Use this socket
 * @param addr      Place the sender's address here
 * @param sport     Place the sender's port here
 * @param buf       Copy the packet payload here
 * @param maxlength Do not copy more than this amount of bytes
 * @param length    The number of bytes actually received is placed here
 *
 * @return The number of bytes received, or negative on error
 */
int socket_recvfrom(sock_t s,
		    network_address_t *addr,
		    uint16_t *sport,
		    void *buf,
		    int buflength,
		    int *length)
{

    /* check parameter sanity */
    KERNEL_ASSERT(s >= 0 && s < CONFIG_MAX_OPEN_SOCKETS);
    
    KERNEL_ASSERT(buflength >= 1 && buf != NULL && addr != NULL && 
		  sport != NULL && length != NULL);

    semaphore_P(open_sockets_sem);

    /* either no POP socket or another recvfrom already in progress
     * (no queueing implemented)
     */
    if (open_sockets[s].protocol != PROTOCOL_POP ||
	open_sockets[s].rbuf != NULL) {
	semaphore_V(open_sockets_sem);
	return -1;
    }

    /* place the return value variables into the socket structure */
    open_sockets[s].rbuf = buf;
    open_sockets[s].bufsize = buflength;
    open_sockets[s].sender = addr;
    open_sockets[s].copied = length;
    open_sockets[s].sport = sport;

    /* release the semaphore */
    semaphore_V(open_sockets_sem);

    /* Note: no one can foul up the FIFO in
     * open_sockets[s].receive_complete between these two semaphore
     * operations since there can be only one active recvfrom at a
     * time 
     */

    /* Wake up service thread so that we can get packets which have
       already received before the call to this function. */
    semaphore_V(pop_service_thread_sem);

    /* and wait until the packet has arrived and been copied to our buffer */
    semaphore_P(open_sockets[s].receive_complete);
    
    return *length;
}



/* forward for pop_init */
static void pop_service_thread(uint32_t dummy);


/** Initialize the POP protocol. Allocate the send buffer, create the
 * semaphores, zero the POP queue entries and start the service
 * thread.
 */
void pop_init()
{
    static int init_done = 0;
    uint32_t addr;
    int i;

    /* do not execute more than once */
    KERNEL_ASSERT(!init_done);
    init_done = 1;

    /* Check that the compiler has made correct size structures
     * (ie. that __attribute__((packed)) works)
     */
    KERNEL_ASSERT(sizeof(pop_header_t) == 8);

    /* Allocate a page for the send buffer */
    addr = pagepool_get_phys_page();

    if (addr == 0) {
	KERNEL_PANIC("pop_init: page allocation failed\n");
    }

    pop_send_buffer = (void*)ADDR_PHYS_TO_KERNEL(addr);

    /* semaphores: */
    pop_send_buffer_sem = semaphore_create(1);    /* this is a lock */
    pop_queue_sem = semaphore_create(1);          /* this too */
    pop_service_thread_sem = semaphore_create(0); /* this is a signaler */

    if ((pop_send_buffer_sem == NULL) || (pop_queue_sem == NULL) ||
	(pop_service_thread_sem == NULL)) {
	KERNEL_PANIC("pop_init: semaphore allocation failed\n");
    }


    /* zero the POP queue entries */
    for (i=0; i<CONFIG_POP_QUEUE_SIZE; i++) {
	pop_queue[i].frame = NULL;
	pop_queue[i].socket = -1;
	pop_queue[i].timestamp = 0;
	pop_queue[i].from = 0;
	pop_queue[i].busy = 0;
    }

    /* start the service thread (the argument is a dummy) */
    pop_service_thread_id = thread_create(&pop_service_thread, 0);
    thread_run(pop_service_thread_id);
}


/** Push a frame to the POP queue. This will place the given frame
 * (packet) into the POP queue. The frame's sender and recipient
 * addresses are supplied as parameters. If this function returns 0,
 * nothing is done for the frame and it can be freed/reused
 * immediately. If the return value is 1, the frame will be freed
 * later by the service thread by calling
 * network_free_frame(frame). NOTE: This function may block, since it
 * uses semaphores.
 *
 * @param fromaddr    Sender address of the frame
 * @param toaddr      Recipient address of the frame
 * @param protocol_id Protocol of the frame, should be PROTOCOL_POP 
 * @param frame       The frame payload
 *
 * @return 1 if the frame was accepted, 0 i f not
 */
int pop_push_frame(network_address_t fromaddr,
		   network_address_t toaddr,
		   uint32_t protocol_id,
		   void *frame)
{
    int i, free = -1, oldest = -1;

    /* unused variables will cause a warning: (since all sockets
     * bound, we don't need toaddr anywhere)
     */
    toaddr = toaddr;

    /* Wrong protocol */
    KERNEL_ASSERT(protocol_id == PROTOCOL_POP);

    semaphore_P(pop_queue_sem);

    /* find a free slot or the oldest slot */
    for (i=0; i<CONFIG_POP_QUEUE_SIZE; i++) {
	/* free slot: */
	if (free == -1 && pop_queue[i].frame == NULL)
	    free = i;
	
	/* oldest nonbusy slot: */
	if (!pop_queue[i].busy &&
	    (oldest == -1 || 
	     pop_queue[i].timestamp < pop_queue[oldest].timestamp))
	    oldest = i;
    }

    /* no free slot or the oldest slot is not old enough
     * => the packet is dropped
     */
    if (free == -1 && (oldest == -1 || 
		       rtc_get_msec() - pop_queue[oldest].timestamp 
		       < CONFIG_POP_QUEUE_MIN_AGE)) { /* queue full */
	semaphore_V(pop_queue_sem);
	return 0;
    }

    /* use the oldest slot if no free slot was found (this is btw the
     * only way that the packets in the queue will age)
     */
    if (free == -1)
	free = oldest;

    /* set the entry values for the given frame */
    if( pop_queue[free].frame) 
        network_free_frame(pop_queue[free].frame);
    pop_queue[free].frame = frame;
    pop_queue[free].socket = -1;
    pop_queue[free].timestamp = rtc_get_msec();
    pop_queue[free].from = fromaddr;
    pop_queue[free].busy = 0;

    semaphore_V(pop_queue_sem);

    /* signal the service thread that a frame has arrived */
    semaphore_V(pop_service_thread_sem);

    return 1; /* accepted */
}


/* action for the service thread */
#define POP_ACTION_NONE 0
#define POP_ACTION_DISCARD 1
#define POP_ACTION_TRANSFER 2


/** The service thread for incoming packets. This thread loops through
 * the POP queue looking for packets to deliver (if recvfrom has been
 * called) or discard (no one listening at the given port). When there
 * is nothing to do, it will wait on the service thread semaphore.
 *
 * @param dummy Dummy parameter, required for threads
 */
static void pop_service_thread(uint32_t dummy)
{
    int action, i, j, slot = 0;
    pop_header_t *f = NULL;
    void *wake;

    dummy = 0; /* prevent warning */

    /* loop the POP queue */
    while(1) {
	/* lock the queue and the socket table */
	semaphore_P(open_sockets_sem);
	semaphore_P(pop_queue_sem);
	
	action = POP_ACTION_NONE;

	/* find a nonempty slot in the queue */
	for (i=0; i<CONFIG_POP_QUEUE_SIZE; i++) {
	    if (pop_queue[i].frame == NULL) continue; /* skip empty slots */

	    f = (pop_header_t*)pop_queue[i].frame;

	    /* find the recipient socket, if necessary */
	    if (pop_queue[i].socket == -1) {
		for (j=0; j<CONFIG_MAX_OPEN_SOCKETS; j++) {
		    if (open_sockets[j].protocol == PROTOCOL_POP &&
			open_sockets[j].port == f->dest_port) {
			pop_queue[i].socket = j;
			break;
		    }
		}
	    }
  	    
            slot = i;

	    /* dest port not listened */
	    if (pop_queue[i].socket == -1) {
		action = POP_ACTION_DISCARD;
		break;
	    }

	    /* frame has recipient socket, but it has been closed */
	    if (pop_queue[i].socket >= 0 && 
		open_sockets[pop_queue[i].socket].protocol != PROTOCOL_POP) {
		action = POP_ACTION_DISCARD;
		break;
	    }
	    
	    /* someone has called recvfrom for the destination socket */
	    if (open_sockets[pop_queue[i].socket].rbuf != NULL) {
		pop_queue[i].busy = 1; /* mark busy so it won't be touched */
		action = POP_ACTION_TRANSFER;
		break;
	    }

	}

	/* unlock the queue and the socket table */
	semaphore_V(pop_queue_sem);
	semaphore_V(open_sockets_sem);


	/* the actions themselves are done here, where no locks are held */

	/* discard the frame */
	if (action == POP_ACTION_DISCARD) {
	    network_free_frame(f);
            pop_queue[slot].frame = NULL;
	}

	/* transfer frame payload to the buffer specified by the
	 * caller of recvfrom
	 */
	if (action == POP_ACTION_TRANSFER) {
	    int bytes;

	    /* copy the payload */
	    bytes = MIN(f->size, open_sockets[pop_queue[slot].socket].bufsize);
	    memcopy(bytes,
		    open_sockets[pop_queue[slot].socket].rbuf,
		    (void*)((uint32_t)f + sizeof(pop_header_t)));

	    /* set return value variables */
	    *(open_sockets[pop_queue[slot].socket].sender) = pop_queue[slot].from;
	    *(open_sockets[pop_queue[slot].socket].copied) = bytes;
	    *(open_sockets[pop_queue[slot].socket].sport) = f->source_port;
	    
	    /* save the receive buffer address */
	    wake = open_sockets[pop_queue[slot].socket].rbuf;

	    /* zero the return value variables from the socket */
	    open_sockets[pop_queue[slot].socket].rbuf = NULL;
	    open_sockets[pop_queue[slot].socket].bufsize = 0;
	    open_sockets[pop_queue[slot].socket].sender = NULL;
	    open_sockets[pop_queue[slot].socket].copied = NULL;
	    open_sockets[pop_queue[slot].socket].sport = NULL;

	    /* discard the frame */
	    network_free_frame(f);

	    /* wake the caller of recvfrom */
	    semaphore_V(open_sockets[pop_queue[slot].socket].receive_complete);

	    /* This will mark the queue slot as free. No synch needed,
	     * since this is only one write operation. 
	     */
	    pop_queue[slot].frame = NULL;
	}

	/* only sleep if nothing was done, otherwise there may be more
	 * frames ready to be handled.
	 */
	if (action == POP_ACTION_NONE)
	    semaphore_P(pop_service_thread_sem);
    } /* while(1) */
}
