# Makefile for the kernel module

# Set the module name
MODULE := vm

FILES := vm.c pagepool.c _tlb.S tlb.c

SRC += $(patsubst %, $(MODULE)/%, $(FILES))

