#
# makefile: version 1.3 of 3/15/83
# Mesa Unix System Makefile
#
# Copyright (c) 1982 National Semiconductor Corporation
#

#
# Recreate system include files from their corresponding sccs files.
#

STD	= /v/mesaux/usr/include/local
STDINC	= uparm.h
 
all:	${STDINC}

${STDINC}:
	sccs -d${STD}  get $@

install:	all

clean:
	rm -f ${STDINC}
