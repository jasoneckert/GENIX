#
# makefile: version 1.6 of 8/25/83
# Unix System Command Source File
#
# @(#)makefile	1.6 (NSC) 8/25/83
#

.SUFFIXES: .o .c .x

DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/bin
SCCS	= /v/mesaux/src/cmd/yacc
SCCSD	= sccs -d${SCCS}

CC	= nmcc
CEFLAGS =

CFLAGS	= -O -DWORD32 -DSCCS ${CEFLAGS}
VGRIND	= csh /usr/ucb/vgrind -W

SRCS	= dextern files y1.c y2.c y3.c y4.c yaccdiffs yaccnews yaccpar
OBJS	= y1.o y2.o y3.o y4.o

all: yacc.x yaccpar

install:
	minstall -s -x yacc.x ${RELDIR}
	minstall yaccpar ${DESTDIR}/usr/lib

clean:
	rm -f *.o yacc.x 

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

yacc.x: ${OBJS}
	rm -f $@
	${CC} ${CFLAGS} -o yacc.x y?.o 
	chmod 770 $@

${OBJS}: dextern files

vgrind:
	cp /dev/null index
	${VGRIND} -h 'Yacc' dextern files y1.c y2.c y3.c y4.c
	${VGRIND} -h 'Yacc' -x index

# Handy-dandy building rules.

.c.o:
	${CC} ${CFLAGS} -c $*.c
