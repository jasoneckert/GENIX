#
# hdr.mk: version 1.1 of 11/3/82
# (System III) Sccs makefile
#
# @(#)hdr.mk	1.1 (NSC) 11/3/82
#

#
# Reconstruct all header files for sccs.
#

SCCS	= .

GET	= sccs -d${SCCS} get

HFILES	= defines.h had.h

all sources: ${HFILES}
${HFILES}:
	${GET} $@

install: all

clean:
#	rm -f ${HFILES}
	echo 'clean sccs/hdr'
