#
# makefile: version 1.6 of 9/29/83
# 
#
# @(#)makefile	1.6 (NSC) 9/29/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/dpy
DESTDIR	= /v/mesaux/usr/nsc

SCCSD	= sccs -d${SCCS}

CC	= nmcc
LCRT	= /mesa/lib/crt0.o
LD	= nmeld
CEFLAGS =

CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= dpy.c scan.c scan.h
OBJS	= dpy.o scan.o

all: dpy.x

install:
	minstall -s -x dpy.x ${DESTDIR}

clean:
	rm -f dpy.x *.o

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

# Handy dandy building rules.

dpy.x:	scan.h ${OBJS}
	rm -f dpy.x
	${LD} ${LCRT} -o dpy.x ${OBJS} -lcurses -ltermcap -ljobs -lc
	chmod 770 dpy.x

.c.o:
	${CC} ${CFLAGS} -c $*.c
