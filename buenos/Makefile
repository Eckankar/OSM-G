# BUENOS -- Buenos is University Educational Nutshell Operating System
# Main Makefile for BUENOS OS
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

# This Makefile skeleton is excerpted from the paper
# _Recursive Make Considered Harmful_ by Peter Miller
# <URL:http://www.tip.net.au/~millerp/rmch/recu-make-cons-harm.html>

# $Id: Makefile,v 1.34 2007/10/05 12:23:12 jaatroko Exp $

## User configurable variables
MODULES     := init kernel lib drivers proc vm fs net util
#CHANGEDFLAGS := -DCHANGED_1 -DCHANGED_2 -DCHANGED_3 -DCHANGED_4 -DCHANGED_5
PHASE        := 1

## Below this point, you shouldn't have to change anything.
TARGET     := buenos
UTILTARGET := util/tfstool


# Compiler and tar configuration
CC      := mips-elf-gcc
AS      := mips-elf-as
LD      := mips-elf-ld
CFLAGS  += -g -G0 -O2 -I. -Wall -W -Werror $(CHANGEDFLAGS)
LDFLAGS := --script=ld.script --cref -G0 -Map buenos.map
ASFLAGS := -gstabs+ -I. -Wa,-mips32 $(CHANGEDFLAGS)
# -G0 is needed to avoid GP optimization (LD won't link if it is used)
GTAR    := gtar

# Each module adds to this
SRC :=

all: $(TARGET) $(UTILTARGET)

# include the description for each module
include $(patsubst %, %/module.mk, $(MODULES))

# determine the object files
OBJS :=  $(patsubst %.s,%.o,$(filter %.s,$(SRC))) \
	 $(patsubst %.S,%.o,$(filter %.S,$(SRC))) \
         $(patsubst %.c,%.o,$(filter %.c,$(SRC)))

# link the program

# For some reason the order of the object file matters; the startup
# code (kernel/boot.o) must come before other files.
$(TARGET): $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $^

# Compile rule for assembler source
%.o: %.s
	$(AS) -o $@ -mips32 $<

%.o: %.S
	$(CC) -o $@ $(ASFLAGS) -c $<

# Compile rule for C source
%.o: %.c
	$(CC) -o $@  $(CFLAGS) -c $<


## Dependencies

# determine the dependencies files
DEPS := $(patsubst %.c,%.d,$(filter %.c,$(SRC))) \
        $(patsubst %.S,%.d,$(filter %.S,$(SRC)))

# include the C include depedencies
-include $(DEPS)

# calculate C include depedencies
%.d: %.c
	CC=$(CC) ./depend.sh `dirname $*` $(CFLAGS) $< >$@

# calculate Assembler include depencies
%.d: %.S
	CC=$(CC) ./depend.sh `dirname $*` $(CFLAGS) $< >$@

doc: $(SRC)
	doc++ -t -o doc/code/buenos.tex ${SRC}
	doc++ -t -s -o doc/code/buenos-src.tex ${SRC}

# TAGS
ctags:
	find . -type f -name '*.[ch]' | xargs ctags

etags:
	find . -type f -name '*.[ch]' | xargs etags

tags: ctags etags

# If the no-op rule for source's wouldn't be here, make might not be
# able to run 'make clean' if a source file was missing.
$(SRC):
	@true

# Builds the buenos.tar.gz to the parent directory!
# This target includes docs. Do not use this for submission
buenos.tar.gz: $(SRC)
	(cd ..; find buenos -type f | grep -Ev 'no-dist|/CVS/' \
                | xargs $(GTAR) cvf - | gzip >$@)

# This is the target to build the submission archive.
submit-archive: $(SRC)
	(cd ..; $(GTAR) cf - buenos | gzip >submit-$(PHASE).tar.gz)

clean:	utilclean
	rm -f $(TARGET) $(OBJS) *~ */*~ buenos.map tags TAGS

real-clean: clean
	rm -f $(DEPS)

.PHONY: clean
.PHONY: doc
.PHONY: submit-archive buenos.tar.gz

