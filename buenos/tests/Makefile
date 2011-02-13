# BUENOS -- Buenos is University Educational Nutshell Operating System
#
# This is a Makefile for BUENOS userland binaries.
#
#
# Copyright (C) 2003-2005  Juha Aatrokoski, Timo Lilja,
#        Leena Salmela, Teemu Takanen, Aleksi Virtanen
#
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above
#     copyright notice, this list of conditions and the following
#     disclaimer in the documentation and/or other materials provided
#     with the distribution.
#  3. The name of the author may not be used to endorse or promote
#     products derived from this software without specific prior
#     written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# $Id: Makefile,v 1.6 2005/05/09 00:05:44 jaatroko Exp $

# Add your _userland_ program sources to this variable:
SOURCES  := halt.c

OBJECTS  := $(patsubst %.c, %.o, $(SOURCES))
TARGETS  := $(patsubst %.o, %, $(OBJECTS))

# crt.o must be the first one and the $(SYSLIBS) must come first in
# the pre-requisites list (or object files list).
SYSLIBS := crt.o _syscall.o lib.o

# Compiler configuration
CC      := mips-elf-gcc
AS      := mips-elf-as
LD      := mips-elf-ld
CFLAGS  += -G0 -I.. -Wall -W -Werror
LDFLAGS := --script=ld.userland.script --cref -s -G0
ASFLAGS := -I.. -Wa,-mips32

# -G0 is needed to avoid GP optimization (LD won't link if it is used)

# Compiling targets (Add explicit targets here if you need to do more
# complex linking (e.g., your source code is divided to multiple
# files))
all: $(TARGETS)

$(TARGETS): %: $(SYSLIBS) %.o
	$(LD) -o $@ $(LDFLAGS) -Map $@.map $^

%.o: %.S
	$(CC) -o $@ $(ASFLAGS) -c $<


%.o: %.c
	$(CC) -o $@  $(CFLAGS) -c $<

clean:
	rm -f $(SYSLIBS) $(OBJECTS) *.map $(TARGETS) *~

.PHONY: clean
