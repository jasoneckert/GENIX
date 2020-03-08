#
# makefile: version 1.10 of 8/25/83
# Unix System Command Source File
#
# @(#)makefile	1.10 (NSC) 8/25/83
#

# The TABFILES macro names files containing tab setting escape sequences
# for various terminals.  By convention these files live in /usr/lib/tabset.
# Unfortunately, these files cannot be sccs'ed, as some of them contain
# character sequences that get can't handle.  Hence we keep them in bare form
# in the tabset subdirectory.

.SUFFIXES: .o .c .S .x

SCCS	= /v/mesaux/src/cmd/tset
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

TABFILES	= \
	3101 aa beehive diablo std stdcrt teleray vt100 xerox1720

SRCS	= tset.c tset.dels.h

all:	tset.x

install: tset.x
	minstall -s -x tset.x ${DESTDIR}/usr/ucb
#	-if [ ! -d ${DESTDIR}/usr/lib/tabset ]; then		\
#		mkdir ${DESTDIR}/usr/lib/tabset;		\
	fi
	cd tabset && minstall ${TABFILES} ${DESTDIR}/usr/lib/tabset
	cd ${DESTDIR}/usr/lib/tabset && chmod 664 ${TABFILES}

clean:
	rm -f *.o tset.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

tset.x:	tset.dels.h

.c.x:	$*.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c -ltermlib
	chmod 770 $@
