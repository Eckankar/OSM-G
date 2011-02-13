# Makefile for the kernel module

# Set the module name
MODULE := init


FILES := _boot.S main.c 

SRC += $(patsubst %, $(MODULE)/%, $(FILES))

