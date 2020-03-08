#
# makefile: version 1.3 of 6/7/83
# Unix Lib Library Source File
#
# @(#)makefile	1.3 (NSC) 6/7/83
#

.SUFFIXES: .O .c .S

SCCS	= /v/mesaux/src/lib/lib
SCCSD	= sccs -d${SCCS}
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib

CC	= nmcc
AR	= ar
RANLIB	= nranlib
CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}
GET	= sccs -d${SCCS} get
SRCS	= lib.b
OBJS	= lib.b

all: ${OBJS}

install:
	minstall lib.b ${RELDIR}

clean:
	echo LIB/LIB
#	rm -f lib.b

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${GET} $@
