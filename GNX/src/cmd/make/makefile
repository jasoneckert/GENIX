#
# makefile: version 1.10 of 8/17/83
# Unix System Command Source File
#
# @(#)makefile	1.10 (NSC) 8/17/83
#

# Description file for the Make command

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/make
SCCSD	= sccs -d${SCCS}
DESTDIR	= /mesa

CC	= nmcc
CEFLAGS	=
CFLAGS	= -O -DASCARCH -DSCCS ${CEFLAGS}

T =
LINT = lint -ps

SRCS = makefile ident.c defs.h main.c doname.c misc.c files.c dosys.c\
	gram.y gcos.c

GCOSSRCS = defs.h ident.c main.c doname.c misc.c gram.c gcos.c

OBJS = ident.o main.o doname.o misc.o files.o dosys.o gram.o

all:	make.x

make.x:  ${OBJS}
	rm -f $@
	${CC} ${CFLAGS} ${OBJS} -o make.x
	chmod 770 $@

${OBJS}:  defs.h

install:
	minstall -s -x make.x ${DESTDIR}/bin

clean:
	-rm -f *.o gram.c make.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

# Handy dandy building rules.

.c.o:
	${CC} ${CFLAGS} -c $*.c

printall: # Print files off line.
	-pr ${SRCS}
	touch print

print:  ${SRCS}# print recently changed files
	-pr $?
	touch print

test: 1zap 2zap
	diff 1zap 2zap
	rm -f 1zap 2zap

1zap:
	./make.x -dp | grep -v TIME >1zap

2zap:
	/bin/make -dp | grep -v TIME >2zap

time:  time1 time2 ;

time1 time2:
	time ./make.x $T
	time /bin/make $T

lint:	dosys.c doname.c files.c main.c misc.c ident.c gram.c
	${LINT} dosys.c doname.c files.c main.c misc.c ident.c gram.c
	rm -f gram.c
