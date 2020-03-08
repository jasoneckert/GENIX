#
# makefile: version 1.10 of 8/23/83
# Mesa Unix command source file
#
# @(#)makefile	1.10 (NSC) 8/23/83
#

.SUFFIXES: .c .o .O .x .y

SCCS 	= /v/mesaux/src/cmd/lint
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/bin
SCCSD	= sccs -d${SCCS}

CC	= nmcc
VAXCC	= cc

INCL	= /v/mesaux/NSC/include
C16FLAGS= -b -q -F
CFLAGS	= -O  -DFLEXNAMES -DSCCS -I${INCL}

M	= .#./mip
LOAD1	= #-s
LOAD2	= #-s

SRCS 	= SHELL allo.c cgram.y comm1.c comm2.c common lint.c llib-lc\
	llib-port lmanifest lpass2.c macdefs manifest match.c mfile1\
	mfile2 optim.c pftn.c reader.c scan.c trees.c xdefs.c

OBJS	= cgram.O xdefs.O scan.O comm1.O pftn.O trees.O optim.O lint.O
VAXOBJS	= cgram.o xdefs.o scan.o comm1.o pftn.o trees.o optim.o lint.o

FILES	= lpass1 lpass1.vax lpass2 llib-port.ln llib-lc.ln

head: ${FILES}

install: head
	minstall lpass1 ${DESTDIR}/usr/lib/lint/lint1
	minstall lpass2 ${DESTDIR}/usr/lib/lint/lint2
	minstall llib-*.ln ${DESTDIR}/usr/lib/lint
	minstall SHELL ${RELDIR}/lint

clean: shrink
	rm -f ${FILES} cgram.c 

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

shrink:
	rm -f *.O *.o

lpass1.vax: ${VAXOBJS}
	${VAXCC} ${LOAD1} ${VAXOBJS}
	mv a.out lpass1.vax

lpass1: ${OBJS}
	${CC} -b ${LOAD1} ${OBJS}
	mv a16.out lpass1

${OBJS}: manifest mfile1 common macdefs lmanifest

cgram.c: cgram.y
	yacc cgram.y
	mv y.tab.c cgram.c

llib-port.ln: llib-port lpass1.vax
	-(/lib/cpp -C -Dlint llib-port | lpass1.vax -puv > llib-port.ln )

llib-lc.ln: llib-lc lpass1.vax
	-(/lib/cpp -C -Dlint llib-lc | lpass1.vax -v > llib-lc.ln )

lpass2: lpass2.O
	${CC} -b ${LOAD2} lpass2.O
	mv a16.out lpass2

lpass2.O: manifest lmanifest lpass2.c
	${CC} ${CEFLAGS} ${C16FLAGS} ${CFLAGS} -c -I$M -I. lpass2.c

lintall:
	lint -hpv -I. -I$M  cgram.c xdefs.c scan.c pftn.c trees.c optim.c lint.c

# building rules

.c.O:	$*.c
	${CC} -c ${CFLAGS} ${C16FLAGS} ${CEFLAGS} -I$M -I. $*.c

.c.o:
	${VAXCC}  -c ${CFLAGS} -I$M -I. $*.c
