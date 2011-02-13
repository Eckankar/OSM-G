# Makefile for the drivers module

# Set the module name
MODULE := drivers

FILES := polltty.c _timer.S timer.c bootargs.c device.c drivers.c tty.c \
	 disk.c disksched.c metadev.c

SRC += $(patsubst %, $(MODULE)/%, $(FILES))
