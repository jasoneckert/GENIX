#
# makefile: version 1.1 of 5/10/83
# GENIX termcap source file
#
# @(#)makefile	1.1 (NSC) 5/10/83
#

SCCS	= /v/mesaux/src/cmd/termcap/tabset
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

SRCS	= 3101 beehive diablo direct std teleray vt100 xerox1720

install: ${SRCS}
	minstall ${SRCS} ${DESTDIR}/usr/lib/tabset

clean:

clobber:
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@
