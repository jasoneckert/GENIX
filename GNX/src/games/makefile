#
# makefile: version 1.12 of 9/14/83
# 
#
# @(#)makefile	1.12 (NSC) 9/14/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/games
SCCSD	= sccs -d${SCCS}
DESTDIR	= /v/mesaux

CC	= nmcc
AS	= nasm
LD	= nmeld
PC	= nmpc
RANLIB	= nranlib

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

ARGS	= CC="${CC}" AS="${AS}" PC="${PC}" LD="${LD}" RANLIB="${RANLIB}" CEFLAGS="${CEFLAGS}"

# List of all source files in this directory.  This is the same as the list
# of files SCCS/s.*, minus the `SCCS/s.', and minus the makefile.  This list
# is used to get sources out of sccs when necessary.

SRCS	= backgammon.c banner.c bcd.c fish.c hangman.c number.c \
	quiz.c rain.c worm.c worms.c wump.c

OBJS	= backgammon.x banner.x bcd.x fish.x hangman.x number.x \
	quiz.x rain.x worm.x worms.x wump.x

SUBDIRS	= mille fortune

all: ${OBJS}
	for i in ${SUBDIRS}; do cd $$i; echo Making $$i; make ${ARGS}; cd ..; done

install: ${OBJS}
	minstall -s -x ${OBJS} ${DESTDIR}/usr/games
	for i in ${SUBDIRS}; do cd $$i; make ${ARGS} install; cd ..; done

clean:
	rm -f ${OBJS} *.o
	for i in ${SUBDIRS}; do cd $$i; make clean; cd ..; done

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile
	for i in ${SUBDIRS}; do cd $$i; make clobber; cd ..; done

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

rain.x: rain.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c -ltermlib
	chmod 770 $@

hangman.x: hangman.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c -lcurses -ltermlib -lm
	chmod 770 $@

worm.x: worm.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c -lcurses -ltermlib
	chmod 770 $@

worms.x: worms.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c -ltermlib
	chmod 770 $@

# Handy dandy building rules.

.c.x:
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@
