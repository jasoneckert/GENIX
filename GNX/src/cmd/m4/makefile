#
# makefile: version 1.6 of 9/16/83
# Unix System Command Source File
#
# @(#)makefile	1.6 (NSC) 9/16/83
#

.SUFFIXES: .c .o .x

SCCS	= /v/mesaux/src/cmd/m4
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}


CC	= nmcc
LCRT	= /mesa/lib/crt0.o
LD	= nmeld
CEFLAGS =

CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= m4.c m4y.y
OBJS	= m4.o m4y.o

all:	m4.x

install:	m4.x
	minstall -s -x m4.x ${DESTDIR}/usr/bin
	
clean:
	rm -f m4.x ${OBJS}

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

m4.o:	m4.c
	${CC} ${CFLAGS} -c m4.c

m4y.o:	m4y.y
	yacc m4y.y
	${CC} ${CFLAGS} -c y.tab.c
	mv y.tab.o m4y.o
	rm y.tab.c

m4.x:	m4.o m4y.o
	${LD} ${LCRT} -o m4.x m4.o m4y.o -lln -lc
	chmod 770 m4.x
