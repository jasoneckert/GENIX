#
# makefile: version 1.1 of 6/7/83
# Mesa Unix Makefile
#
# @(#)makefile	1.1 (NSC) 6/7/83
#

SCCS	= /v/mesaux/usr/lib/tmac
SCCSD	= sccs -d${SCCS}

SRCS = tmac.a tmac.an tmac.cp tmac.m tmac.r tmac.s tmac.scover\
	tmac.sdisp tmac.skeep tmac.srefs tmac.vcat tmac.vgrind

all: sources

install: sources

clean:

clobber:
	sccs -d${SCCS} clean; ${SCCSD} get makefile

# Handy dandy building rules.

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@
