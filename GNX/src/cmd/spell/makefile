#
# makefile: version 1.5 of 8/25/83
# Unix System Command Source File
#
# @(#)makefile	1.5 (NSC) 8/25/83
#

.SUFFIXES: .c .o .x

SCCS	= /v/mesaux/src/cmd/spell
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc
LD	= nmeld

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= american british local spell.c spell.h spell.sh spellin.c spellout.c\
		stop

OBJS	= spell.x hlist hlista hlistb hstop spellin.x spellout.x

all: ${OBJS}

install:
	minstall -s -x spell.x ${DESTDIR}/usr/lib
	minstall -s -x spellin.x ${DESTDIR}/usr/bin
	minstall -s -x spellout.x ${DESTDIR}/usr/bin
	minstall -x spell.sh ${DESTDIR}/usr/bin/spell
	minstall hlista $(DESTDIR)/usr/dict
	minstall hlistb $(DESTDIR)/usr/dict
	minstall hstop $(DESTDIR)/usr/dict

clean:	
	rm -f spell.x spellin.x hlista hlistb hlist hstop spellout.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

hlist: ${DESTDIR}/usr/dict/words
	spellin < ${DESTDIR}/usr/dict/words > hlist
hlista: american local hlist 
	(cat american local) | spellin hlist > hlista
hlistb: british local hlist 
	(cat british local) | spellin hlist > hlistb
hstop: stop 
	spellin < stop > hstop

spell.x: spell.h spell.sh

.c.o:	
	*.c
	${CC} ${CFLAGS} -c $*.c

.o.x:	
	rm -f $@
	${LD} -o $@ $*.o -lc
	chmod 770 $@

.c.x:	$*.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@
