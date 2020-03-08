#
#
# makefile: version 1.8 of 7/14/83
# Unix Library Source File
#
# @(#)makefile	1.8 (NSC) 7/14/83
#

.SUFFIXES: .o .c .S

SCCS	= /v/mesaux/src/lib/libdbm
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AR	= ar
RANLIB	= nranlib
CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}
SRCS	= dbm.c
OBJS	= dbm.o

all: libdbm.a
libdbm.a: ${OBJS}
	${AR} cr libdbm.a ${OBJS}

install:
	minstall -r libdbm.a ${RELDIR}

clean:
	rm -f libdbm.a ${OBJS}

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.o:
	${CC} ${CFLAGS} -c $*.c
