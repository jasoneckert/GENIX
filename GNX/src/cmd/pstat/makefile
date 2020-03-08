#
# makefile: version 1.6 of 9/16/83
# 
#
# @(#)makefile	1.6 (NSC) 9/16/83
#

.SUFFIXES: .o .c .s .x

SCCS	= /v/mesaux/src/cmd/pstat
DESTDIR	= /mesa
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AS	= nasm
LCRT	= /mesa/lib/crt0.o
LD	= nmeld

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}
AFLAGS	=

SRCS 	= pstat.c bitcount.s
OBJS	= pstat.o bitcount.o

all: pstat.x

pstat.x: ${OBJS}
	rm -f $@
	${LD} ${LCRT} -o $@ ${OBJS} -lc
	chmod 770 $@

install:
	minstall -s -x pstat.x ${DESTDIR}/etc

clean:
	rm -f *.o pstat.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

bitcount.o: bitcount.s
pstat.o: pstat.c
pstat.o: /v/mesaux/usr/include/sys/param.h
pstat.o: /v/mesaux/usr/include/sys/dir.h
pstat.o: /v/mesaux/usr/include/sys/file.h
pstat.o: /v/mesaux/usr/include/sys/user.h
pstat.o: /v/mesaux/usr/include/sys/proc.h
pstat.o: /v/mesaux/usr/include/sys/inode.h
pstat.o: /v/mesaux/usr/include/sys/tty.h
pstat.o: /v/mesaux/usr/include/sys/vm.h
pstat.o: /v/mesaux/usr/include/sys/devvm.h
pstat.o: /v/mesaux/usr/include/nlist.h

# Handy dandy building rules.

.c.o:
	${CC} ${CFLAGS} -c $*.c

.s.o:
	${AS} ${AFLAGS} -o $*.o $*.s

