#
# makefile: version 1.1 of 6/7/83
# Mesa Unix System Command Makefile
#
# @(#)makefile	1.1 (NSC) 6/7/83
#

SCCS	= /v/mesaux/usr/lib/macros
SCCSD	= sccs -d${SCCS}

SRCS = mmn mmt

all: sources

install: sources

clean:

clobber:
	sccs -d${SCCS} clean; ${SCCSD} get makefile

# Handy dandy building rules.

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@
