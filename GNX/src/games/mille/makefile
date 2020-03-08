#
# makefile: version 1.4 of 9/6/83
# UNIX Game Source File
#
# @(#)makefile	1.4 (NSC) 9/6/83
#

.SUFFIXES: .po .i .o .x

SCCS	= /v/mesaux/src/games/mille
SCCSD	= sccs -d${SCCS}

DESTDIR	= /v/mesaux

CC	= nmcc

CEFLAGS =
CFLAGS	= -O -DSCCS -DSTANDOUT ${CEFLAGS}

SRCS	= mille.h unctrl.h\
	comp.c end.c extern.c init.c mille.c misc.c move.c print.c \
	roll.c save.c types.c varpush.c
OBJS	= comp.o end.o extern.o init.o mille.o misc.o move.o print.o \
	roll.o save.o types.o varpush.o

CRL	= -lcurses
#CRL	= ../crlib
LIBS=	${CRL} -ltermlib

mille.x: ${OBJS}
	rm -f $@
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}
	chmod 770 $@

a.out: ${OBJS}
	rm -f $@
	${CC} ${CFLAGS} ${OBJS} ${LIBS}
	chmod 770 $@

install: mille.x
	minstall -s -x mille.x ${DESTDIR}/usr/games

clean:
	rm -f mille.x a.out *.o

clobber:
	rm -f mille.x a.out *.o
	-${SCCSD} clean
	-${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

table.x: table.o extern.o
	${CC} ${CFLAGS} -o table.x table.o extern.o

readdump.x: readdump.o extern.o varpush.o
	${CC} ${CFLAGS} -o readdump.x readdump.o extern.o varpush.o

ctags:
	ctags ${SRCS}
	ed - tags < :ctfix
	sort tags -o tags

lint:
	lint -hxb ${SRCS} > lint.out

mille.ar: ${SRCS}
	ar ruv mille.ar Makefile tags ${SRCS}

tar: ${SRCS}
	tar crvf mille.tar Makefile tags :ctfix ${SRCS}

lpr:
	pr Makefile ${SRCS} tags | lpr ; lpq

misc.c move.c save.c: unctrl.h
comp.c end.c extern.c init.c mille.c misc.c move.c print.c\
save.c table.c typec varpush.c: mille.h

.c.o:
	${CC} ${CFLAGS} -c $*.c
