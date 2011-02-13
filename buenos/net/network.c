/*
 * Frame level networking
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
 * $Id: network.c,v 1.12 2007/02/25 15:16:29 jaatroko Exp $
 *
 */

#include "net/network.h"
#include "net/protocols.h"
#include "net/socket.h"
#include "kernel/config.h"
#include "kernel/assert.h"
#include "drivers/yams.h"
#include "kernel/thread.h"
#include "vm/pagepool.h"

/** @name Network frame layer
 *
 * This module contains the implementation of the network frame layer.
 * This layer is an abstraction above the GND layer. Packets are
 * received from all GNDs found in the system and forwarded for
 * further processing to the upper layers.
 */

/* Structure of the network frame header. */
typedef struct {
    /* The destination address of the frame. */
    network_address_t destination   __attribute__ ((packed));

    /* The source address of the frame. */
    network_address_t source        __attribute__ ((packed));

    /* The higher level protocol id for this frame. */
    uint32_t protocol_id             __attribute__ ((packed));
} network_frame_header_t;

/* Structure of the network frame. */
typedef struct {
    /* Header of this frame */
    network_frame_header_t header   __attribute__ ((packed));

    /* Payload of this frame. */
    uint8_t payload[PAGE_SIZE-sizeof(network_frame_header_t)] 
                                    __attribute__ ((packed));
} network_frame_t;

/* Structure holding information of a network interface (GND) */
typedef struct {
    /* The address of this interface. */
    network_address_t address;

    /* The maximum transfer unit of this interface. */
    int mtu;

    /* The GND of this interface. */
    gnd_t *gnd;
} network_interface_info_t;

/* A table of network interfaces. */
network_interface_info_t network_interfaces[CONFIG_MAX_GNDS];

/** 
 * Forwards a received frame to the upper protocol layers. 
 *
 * @param frame The frame that was received.
 *
 * @return 0 on failure. Other values mean success.
 */
static int network_receive_frame(network_frame_t *frame)
{
    frame_handler_t frame_handler;
    
    frame_handler = 
	protocols_get_frame_handler(frame->header.protocol_id);
    if(frame_handler != NULL) {
	/* found handler, known protocol */
	return frame_handler(frame->header.source,
				 frame->header.destination,
				 frame->header.protocol_id,
				 frame->payload);
    }

    return 0;
}

/**
 * Continually receives frames from a given network interface.
 *
 * @param interface The index of the interface from which frames are
 * received.
 */
static void network_receive_thread(uint32_t interface)
{
    network_frame_t *frame = NULL;
    uint32_t frame_phys_addr = 0;
    gnd_t *gnd;
    int ret = 1; /*Initialize to success, so a new page will be allocated.*/

    gnd = network_interfaces[interface].gnd;

    while(1) {
	if(ret != 0) {
	    /* We need new page */
	    frame_phys_addr = pagepool_get_phys_page();
	    KERNEL_ASSERT(frame_phys_addr != 0);
	    frame = (network_frame_t *) ADDR_PHYS_TO_KERNEL(frame_phys_addr);
	}

	ret = 0;

        /* Receive a frame. This call blocks until frame is transfered
           to memory. */
	if(gnd->recv(gnd, (void *) frame_phys_addr) == 0) {
	    /* success */
	    ret = network_receive_frame(frame);
	}
    }
}

/**
 * Initialize the network frame layer and all other network protocols
 * as well.
 */
void network_init(void)
{
    int i;
    device_t *dev;

    /* Find all network devices in system */
    for(i=0; i<CONFIG_MAX_GNDS; i++) {
	dev = device_get(YAMS_TYPECODE_NIC, i);
	if(dev == NULL) {
            /* No more GNDs. Initialize to invalid. */
	    network_interfaces[i].gnd = NULL;
	    network_interfaces[i].address = 0;
	} else {
            /* Initialize this entry. */
	    gnd_t *gnd;

	    gnd = (gnd_t *) dev->generic_device;

	    network_interfaces[i].gnd = gnd;
	    network_interfaces[i].mtu = gnd->frame_size(gnd);

            /* The network code is unable to handle frames which don't
               fit into one page. This is because there is no way to
               allocate two consecutive memory pages. */
	    KERNEL_ASSERT(network_interfaces[i].mtu <= PAGE_SIZE);

	    network_interfaces[i].address = gnd->hwaddr(gnd);
	}
    }

    /* Initialize sockets. Should be done before protocol inits*/
    socket_init();
    /* Initialize upper level network protocols. */
    protocols_init();

    /* Create and start a receiving thread for each network
       interface. */
    for(i=0; i<CONFIG_MAX_GNDS; i++) {
	if(network_interfaces[i].gnd != NULL) {
	    TID_t tid;
	    tid = thread_create(&network_receive_thread, i);
            /* Thread creation should succeed. If not, increase the
               number of threads in the system by editing config.h. */
	    KERNEL_ASSERT(tid > 0);
	    thread_run(tid);
	    kprintf("Network: started network services on device "
		    "at address %8.8x\n", 
		    network_interfaces[i].address);
	} else {
	    break;
	}
    }
}

/**
 * Gets the source address of the given interface.
 *
 * @param interface The index of the interface. The interfaces are
 * indexed from 0 to n-1 if there are n interfaces.
 *
 * @return The address of the interface. 0 if there is no interface with
 * given interface index.
 */
network_address_t network_get_source_address(int interface)
{
    if(interface<0 || interface>= CONFIG_MAX_GNDS)
	return 0;

    return network_interfaces[interface].address;
}

/**
 * Gets the network broadcast address.
 *
 * @return The network broadcast address.
 */
network_address_t network_get_broadcast_address(void)
{
    return NETWORK_BROADCAST_ADDRESS;
}

/**
 * Gets the network loopback address.
 *
 * @return The network loopback address.
 */
network_address_t network_get_loopback_address(void)
{
    return NETWORK_LOOPBACK_ADDRESS;
}

/**
 * Gets the MTU (maximum transfer unit) of the givem interface.
 *
 * @param local_address The address of the interface. If broadcast
 * address is given as argument, the minimum of all MTUs of all GNDs
 * in the system is returned.
 *
 * @return The MTU. 0 if the local_address is invalid.
 */
int network_get_mtu(network_address_t local_address)
{
    network_interface_info_t *n;
    int min=NETWORK_MAX_MTU+1;

    if(local_address == NETWORK_BROADCAST_ADDRESS) {
	/* Find minimum MTU */
	for(n = network_interfaces; n->gnd != NULL; n++) {
	    if(n->mtu < min)
		min = n->mtu;
	}
	
	return (min - sizeof(network_frame_header_t));
    } else if(local_address == NETWORK_LOOPBACK_ADDRESS) {
        return (PAGE_SIZE - sizeof(network_frame_header_t)); 
    } else {
	/* Find MTU of given interface address. */
	for(n = network_interfaces; n->gnd != NULL; n++) {
	    if(n->address == local_address)
		return (n->mtu - sizeof(network_frame_header_t));
	}

	/* not found */
	return 0;
    }
}

/**
 * Get the index of the given interface.
 *
 * @param local_address The address of the queried interface.
 *
 * @return The index of the interface. -1 if not found.
 */
static int network_get_interface(network_address_t local_address)
{
    int i;
    

    for(i = 0; i<CONFIG_MAX_GNDS; i++) {
	if(network_interfaces[i].address == local_address)
	    return i;
    }

    /* not found */
    return -1;
}

/**
 * Send a frame to the destination through the given interface.
 *
 * @param interface Index of the interface.
 *
 * @param destination The address of the destination.
 *
 * @param The frame to be sent.
 *
 * @return 0 on success. Other values indicate failure.
 */
static int network_send_interface(int interface,
				  network_address_t destination,
				  network_frame_t *frame)
{
    gnd_t *gnd;

    gnd = network_interfaces[interface].gnd;
    return gnd->send(gnd, 
		     (void *) ADDR_KERNEL_TO_PHYS((uint32_t) frame), 
		     destination);
}

/**
 * Send a frame from the source interface to the destination address.
 *
 * @param source The interface to use for sending. If this is
 * broadcast address the frame is sent through all interfaces in the
 * system.
 *
 * @param destination The destination address for this frame.
 *
 * @param protocol_id The higher level protocol id to be used with
 * this frame.
 *
 * @param length The length of the buffer.
 *
 * @param buffer A buffer containing the payload for this frame. The
 * buffer should be lenght octets long.
 *
 * @return Generic network error codes.
 */
int network_send(network_address_t source,
		 network_address_t destination,
		 uint32_t protocol_id,
		 int length,
		 void *buffer)
{
    uint32_t phys_frame;
    network_frame_t *frame;
    int send_ret=NET_OK;

    /* The frame should fit into one page. */
    KERNEL_ASSERT(length > 0 &&
		  length <= (int)(PAGE_SIZE-sizeof(network_frame_header_t)));

    /* Allocate a page for this frame. */
    phys_frame = pagepool_get_phys_page();
    if(phys_frame == 0)
	return NET_ERROR;
    frame = (network_frame_t *) ADDR_PHYS_TO_KERNEL(phys_frame);

    /* Initialize the frame header. */
    frame->header.source = source;
    frame->header.destination = destination;
    frame->header.protocol_id = protocol_id;
    memcopy(length, frame->payload, buffer);

    /* If loopback, push the frame immediately to the upper layers. */
    if(destination == NETWORK_LOOPBACK_ADDRESS) {
	if(frame->header.source == NETWORK_BROADCAST_ADDRESS)
	    frame->header.source = NETWORK_LOOPBACK_ADDRESS;
	if(network_receive_frame(frame) == 0) {
	    /* push failed */
	    pagepool_free_phys_page(phys_frame);
	    return NET_ERROR;
	}
	
	return NET_OK;
    }

    /* If source is not broadcast, find the given interface and send
       the frame. */
    if(source != NETWORK_BROADCAST_ADDRESS) {
	int interface;

	interface = network_get_interface(source);
	if(interface < 0) {
            /* No such interface. */
	    pagepool_free_phys_page(phys_frame);
	    return NET_DOESNT_EXIST;
	}

	if(network_send_interface(interface, destination, frame) != 0)
	    send_ret = NET_ERROR;
    } else {
        /* Source is broadcast. Send the the packet through all
           interfaces. */
	int interface;

	for(interface=0; interface<CONFIG_MAX_GNDS; interface++) {
	    if(network_interfaces[interface].gnd == NULL)
		break;

	    if(network_send_interface(interface, destination, frame) != 0)
		send_ret = NET_ERROR;
	}
    }

    pagepool_free_phys_page(phys_frame);
    return send_ret;
}

/**
 * Free the given network frame. This function is called by the upper
 * level frame handlers after they have handled a received frame.
 *
 * @param A pointer to the beginning of the payload in the frame.
 */
void network_free_frame(void *payload_frame)
{
    uint32_t frame = ADDR_KERNEL_TO_PHYS((uint32_t)payload_frame) 
	& PAGE_SIZE_MASK;
    pagepool_free_phys_page(frame);
}

