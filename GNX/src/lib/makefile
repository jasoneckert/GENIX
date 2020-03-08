#
# makefile: version 1.9 of 11/8/83
# 
#
# @(#)makefile	1.9 (NSC) 11/8/83
#

DESTDIR	=
RELDIR  = 

SCCS	= /v/mesaux/src/lib
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AS	= nasm
LD	= nmeld
RANLIB	= nranlib
CEFLAGS	=

ARGS	= \
	CC="${CC}" AS="${AS}" PC="${PC}" LD="${LD}"	\
	RANLIB="${RANLIB}" CEFLAGS="${CEFLAGS}"

# Library sub-directories where commands are to be built:
SUBDIRS = \
	lib libcurses libdbm libjobs libln libm libmp libnsc	\
	libndir libPW libplot libtermlib

all: 
	for i in ${SUBDIRS}; do cd $$i; make ${ARGS}; cd ..; done

install:
	for i in ${SUBDIRS}; do cd $$i; make ${ARGS} install; cd ..; done

clean:
	for i in ${SUBDIRS}; do cd $$i; make clean; cd ..; done

clobber:
	for i in ${SUBDIRS}; do cd $$i; make clobber; cd ..; done
	${SCCSD} clean; ${SCCSD} get makefile
