# Makefile for the util subdirectory

# Set the module name
MODULE := util

NATIVECC      := gcc
NATIVECFLAGS  += -O2 -g -I. -Wall -W
TARGETS       += util/tfstool

util/tfstool: util/tfstool.o
	$(NATIVECC) -o $@ $^

util/tfstool.o: util/tfstool.c util/tfstool.h fs/tfs.h lib/bitmap.h
	$(NATIVECC) -o $@  $(NATIVECFLAGS) -c $<

utilclean:
	rm -f util/*.[od] util/tfstool
