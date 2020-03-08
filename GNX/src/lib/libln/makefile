#
# makefile: version 1.11 of 7/14/83
# Unix Library Source File
#
# @(#)makefile	1.11 (NSC) 7/14/83
#

.SUFFIXES: .o .c .S

SCCS	= /v/mesaux/src/lib/libln
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AR	= ar
RANLIB	= nranlib
CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= allprint.c main.c reject.c yyless.c yywrap.c

OBJS	= allprint.o main.o reject.o yyless.o yywrap.o

all:	libln.a
libln.a: ${OBJS}
	${AR} rvc libln.a ${OBJS}

install: libln.a
	minstall -r libln.a ${RELDIR}
	-rm -f ${RELDIR}/libl.a
	ln ${RELDIR}/libln.a ${RELDIR}/libl.a

clean:
	rm -f *.o libln.a

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.o:
	${CC} ${CFLAGS} -c $*.c
