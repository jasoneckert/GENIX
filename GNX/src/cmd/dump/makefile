#
# makefile: version 1.6 of 8/15/83
# 
#
# @(#)makefile	1.6 (NSC) 8/15/83
#

#
#	dump.h			header file
#	dumpitime.c		reads /etc/ddate
#	dumpmain.c		driver
#	dumpoptr.c		operator interface
#	dumptape.c		handles the mag tape and opening/closing
#	dumptrav.c		traverses the file system
#	unctime.c		undo ctime
#
#	DEBUG			use local directory to find ddate and dumpdates
#	TDEBUG			trace out the process forking
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/dump
DESTDIR	= /mesa
SCCSD	= sccs -d${SCCS}

CC	= nmcc

PSRCS	= dump.h dumpmain.c dumptrav.c dumptape.c dumpoptr.c dumpitime.c

SRCS	= dump.h dumpitime.c \
	dumptape.c \
	dumptrav.c unctime.c dumpmain.c dumpoptr.c

OBJS	= dumpitime.o \
	dumptape.o \
	dumptrav.o unctime.o dumpmain.o dumpoptr.o

PROFS	= ../../libc/csu/mcrt0.o ../../libc/csu/mon.o ../../libc/csu/prof.o

CEFLAGS =
DFLAGS = -DSCCS
CFLAGS = -O ${DFLAGS} ${CEFLAGS}

dump.x:	${OBJS}
	rm -f $@
	${CC} ${CFLAGS} ${OBJS} -o $@
	chmod 770 $@

install:
	minstall -s -x dump.x ${DESTDIR}/etc

clean:
	rm -f *.o dump.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

prof:	${OBJS}
	rm -f dump.x
	${CC} ${CFLAGS} -DMONITOR ${OBJS} ${PROFS} -o dump.x
	chmod 770 dump.x

dumpitime.c dumptape.c dumptrav.c dumpmain.c dumpoptr.c: dump.h

lint:
	lint ${DFLAGS} ${SRCS}

psrcs:
	echo ${PSRCS}

# Handy dandy building rules.

.c.o:
	${CC} ${CFLAGS} -c $*.c
