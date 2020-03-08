#
# makefile: version 1.2 of 11/8/83
# GENIX Library Source code
#
# @(#)makefile	1.2 (NSC) 11/8/83
#

.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/lib/libplot
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AS	= nasm
AR	= ar
LD	= nmeld
PC	= nmpc
RANLIB	= nranlib

CEFLAGS =
CFLAGS	= -c -O -DSCCS ${CEFLAGS}
AFLAGS	=
ARGS	= CC=${CC} AS=${AS} LD=${LD} PC=${PC} RANLIB=${RANLIB} \
	CEFLAGS=${CEFLAGS}

SUBDIRS = plot t300 t300s t4014 t450 vt0

all:
	for i in ${SUBDIRS}; do cd $$i; make ${ARGS}; cd ..; done

install:
	for i in ${SUBDIRS}; do cd $$i; make ${ARGS} install; cd ..; done

clean:
	for i in ${SUBDIRS}; do cd $$i; make clean; cd ..; done

clobber: 
	for i in ${SUBDIRS}; do cd $$i; make clobber; cd ..; done
	${SCCSD} clean; ${SCCSD} get makefile
