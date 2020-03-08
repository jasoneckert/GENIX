#
# makefile: version 1.5 of 8/17/83
# Mesa Unix System Command Makefile
#
# @(#)makefile	1.5 (NSC) 8/17/83
#

.SUFFIXES: .o .c .e .S .x

SCCS	= /v/mesaux/src/cmd/eqn
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc

CEFLAGS =
CFLAGS  = -O -d2 -DSCCS ${CEFLAGS}
YFLAGS  = -d

SRCS = e.y e.h diacrit.c eqnbox.c font.c fromto.c funny.c glob.c integral.c \
 io.c lex.c lookup.c mark.c matrix.c move.c over.c paren.c \
 pile.c shift.c size.c sqrt.c text.c

FILES = e.o diacrit.o eqnbox.o font.o fromto.o funny.o glob.o integral.o \
 io.o lex.o lookup.o mark.o matrix.o move.o over.o paren.o \
 pile.o shift.o size.o sqrt.o text.o

eqn.x:	${FILES}
	rm -f $@
	nmcc ${CFLAGS} ${FILES} -o $@
	chmod 770 $@

install:
	minstall -s -x eqn.x ${DESTDIR}/usr/bin

clean:
	rm -f *.o e.c e.def eqn.x
	
clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

${FILES}:	e.h e.def

e.c:	e.def

e.def:	e.y
	yacc -d e.y
	mv y.tab.c e.c
	mv y.tab.h e.def

list:
	pr TODO ${SOURCE} makefile

lint:	y.tab.c
	lint -spb *.c

.c.o:
	${CC} ${CFLAGS} -c $*.c

