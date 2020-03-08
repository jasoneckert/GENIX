#
# makefile: version 1.12 of 8/17/83
# Mesa Unix System Command Makefile
#
# @(#)makefile	1.12 (NSC) 8/17/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/lpr
DESTDIR	= /v/mesaux

SCCSD	= sccs -d${SCCS}

CC	= nmcc
CEFLAGS	=
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= lpd.h lpd.c lpf.c lpr.c lpq.c lprm.c slpd.c slpf.c 
OBJS	= lpd.h lpd.x lpf.x lpr.x lpq.x lprm.x slpd.x slpf.x

all:	${OBJS}

install:
	minstall -s -x lpd.x ${DESTDIR}/usr/lib
	minstall -s -x slpd.x ${DESTDIR}/usr/lib
	minstall -s -x lpf.x ${DESTDIR}/usr/lib
	minstall -s -x slpf.x ${DESTDIR}/usr/lib
	minstall -s -x lpq.x ${DESTDIR}/usr/ucb
	minstall -s -x lpr.x ${DESTDIR}/usr/ucb
	minstall -s -x lprm.x ${DESTDIR}/usr/ucb

clean:
	rm -f *.x *.o

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.x:	$*.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@
