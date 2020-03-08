#
# makefile: version 1.9 of 8/15/83
# Unix System Command Source File
#
# @(#)makefile	1.9 (NSC) 8/15/83
#
.SUFFIXES: .c .o .x

SCCS	= /v/mesaux/src/cmd/calendar
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc
LD	= nmeld

CEFLAGS	=
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= calendar.c calendar.sh

OBJS	= calendar.x calendar.sh

calendar.x: calendar.c

all: ${OBJS}

install: ${OBJS}
	minstall -s -x calendar.x ${DESTDIR}/usr/lib
	minstall -x calendar.sh ${DESTDIR}/usr/bin

clean:
	rm -f calendar.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.o:	*.c
	${CC} ${CFLAGS} -c $*.c

.o.x:	
	rm -f $@
	${LD} -o $@ $*.o -lc
	chmod 770 $@

.c.x:	
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@
