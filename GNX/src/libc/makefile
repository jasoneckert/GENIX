#
# makefile: version 1.19 of 9/10/83
# Unix C Library Source File
#
# @(#)makefile	1.19 (NSC) 9/10/83
#

DESTDIR	= /mesa
RELDIR	= ${DESTDIR}/lib

CC = nmcc
AS = nasm
LD = nmeld
CEFLAGS =
RANLIB = nranlib

ARGS	= CC="${CC}" AS="${AS}" PC="${PC}" LD="${LD}" RANLIB="${RANLIB}" CEFLAGS="${CEFLAGS}"

SUBDIRS = sys gen stdio csu crt

all: libc.a
libc.a:
	for i in ${SUBDIRS}; do cd $$i; make ${ARGS} all; cd ..; done
	ls gen/*.o sys/*.o stdio/*.o crt/*.o | sort -t/ +1 > t1
	ar cr libc.a `cat t1`
#	ar ma flsbuf.O libc.a exit.O
	rm -f t1

install: all
	minstall -r libc.a ${RELDIR}
	minstall csu/crt0.o ${RELDIR}/crt0.o
	rm -f ${RELDIR}/crt0.O
	ln ${RELDIR}/crt0.o ${RELDIR}/crt0.O
	minstall csu/mcrt0.o ${RELDIR}/mcrt0.o
	rm -f ${RELDIR}/mcrt0.O
	ln ${RELDIR}/mcrt0.o ${RELDIR}/mcrt0.O

clean:
	for i in ${SUBDIRS}; do cd $$i; make clean; cd ..; done
	rm -f t1 *.o libc.a

clobber:
	for i in ${SUBDIRS}; do cd $$i; make clobber; cd ..; done
	rm -f t1 *.o libc.a

errlst.o: gen/errlst.c
	cc -S gen/errlst.c
	ed - <gen/:errfix errlst.s
	as -o errlst.o errlst.s
	rm errlst.s

