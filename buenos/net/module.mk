# Makefile for the kernel module

# Set the module name
MODULE := net


FILES := network.c protocols.c socket.c pop.c

SRC += $(patsubst %, $(MODULE)/%, $(FILES))

