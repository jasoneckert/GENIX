#
# makefile: version 1.13 of 8/25/83
# GENIX system makefile
#
# @(#)makefile	1.13 (NSC) 8/25/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/xsend
DESTDIR	=/v/mesaux
SCCSD	= sccs -d${SCCS}

LIBS	= -lmp
CC	= nmcc
CEFLAGS	=
CFLAGS	= -O -d2 -DSCCS ${CEFLAGS}

SRCS	= enroll.c lib.c notice xget.c xsend.c xmail.h
OBJS	= enroll.o notice xget.o xsend.o lib.o
FILES	= enroll.x xsend.x xget.x

all:	${FILES}

install: ${FILES}
	minstall -x -s ${FILES} ${DESTDIR}/usr/bin
	minstall -s notice ${DESTDIR}/usr/spool/secretmail

clean:
	rm -f *.o *.x yacc.* y.tab.c

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

${OBJS}: xmail.h lib.o

list:
	@pr -2 -w124 m.h xmail.h enroll.c xsend.c xget.c lib.c makefile

.o.x:	lib.o $*.o
	rm -f $@
	${CC} ${CFLAGS} $*.o lib.o -lmp -o $@ ${LIBS}
	chmod 770 $@

.c.o:
	${CC} ${CFLAGS} -c $*.c
