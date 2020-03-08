#
# makefile: version 1.4 of 8/16/83
# Unix System Command Source File
#
# @(#)makefile	1.4 (NSC) 8/16/83
#

.SUFFIXES: .o .c .x .y

SCCS	= /v/mesaux/src/cmd/lex
DESTDIR = /v/mesaux/usr
SCCSD	= sccs -d${SCCS}

CC	= nmcc
CEFLAGS	=
CFLAGS 	= -O -DSCCS ${CEFLAGS}

OBJS	= lmain.o y.tab.o sub1.o sub2.o header.o

SRCS	= lmain.c sub1.c sub2.c header.c ldefs.c once.c ncform

FILES	= lex.x ncform

all: ${FILES}

lex.x: ${OBJS}
	rm -f lex.x
	${CC} ${CFLAGS} ${OBJS} -o lex.x
	chmod 770 lex.x

install: ${FILES}
	minstall -s -x lex.x ${DESTDIR}/bin
	minstall ncform ${DESTDIR}/lib/lex

clean:
	rm -f *.o lex.x y.tab.c

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS} parser.y
${SRCS} parser.y:
	${SCCSD} get $@

smallex:
	${CC} -DSMALL ${CFLAGS} ${SRCS} y.tab.c -o smallex.x

lmain.o:ldefs.c once.c

sub1.o: ldefs.c

sub2.o: ldefs.c

header.o: ldefs.c

y.tab.c: parser.y
	yacc parser.y

.c.o:
	${CC} ${CFLAGS} -c $*.c
