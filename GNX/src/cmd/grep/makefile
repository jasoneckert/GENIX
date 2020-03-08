#
# makefile: version 1.7 of 8/15/83
# Unix System Command Source File
#
# @(#)makefile	1.7 (NSC) 8/15/83
#

.SUFFIXES: .c .o .x
SCCS	= /v/mesaux/src/cmd/grep
BIN = /mesa/bin
USRUCB = /v/mesaux/usr/ucb
SCCSD	= sccs -d${SCCS}

CC	= nmcc
LD	= nmeld

CEFLAGS	=
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= btlgrep.c ucbgrep.c

OBJS	= btlgrep.x ucbgrep.x

all: ${OBJS}

install:
	minstall -s btlgrep.x ${BIN}/grep
	minstall -s ucbgrep.x ${USRUCB}/grep

clean:
	rm -f *.x *.o

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.o:	*.c
	${CC} ${CFLAGS} -c $*.c

.o.x:
	rm -f $*.x
	${LD} -o $*.x $*.o -lc
	chmod 770 $*.x

.c.x:	
	rm -f $*.x
	${CC} ${CFLAGS} -o $*.x $*.c
	chmod 770 $*.x

