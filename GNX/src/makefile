#
# makefile: version 1.13 of 9/27/83
# 
#
# @(#)makefile	1.13 (NSC) 9/27/83
#

DESTDIR	=
RELDIR	=

SCCS	= /v/mesaux/src
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AS	= nasm
LD	= nmeld
RANLIB	= nranlib
CEFLAGS	=

ARGS	= CC="${CC}" AS="${AS}" LD="${LD}" RANLIB="${RANLIB}" CEFLAGS="${CEFLAGS}"

# List of library directories

LIBDIR 	= lib libc 

# List of directories where recursive make commands are.

SUBDIRS = cmd db16k games

ALLDIRS	= ${LIBDIR} ${SUBDIRS}

# Directory that contains me macro sources.

MEDIR	= /v/mesaux/usr/lib/me/src


all:	allcmd alldb16k allgames

# library must be built and istalled before the building of the rest of
# the commands.

cleanlib:
	for i in ${LIBDIR}; do cd $$i; echo CLEANING $$i; make clean; cd ..; done
	cd ${MEDIR}; echo CLEANING ${MEDIR}; make clean

alllib:
	for i in ${LIBDIR}; do cd $$i; echo MAKING $$i; make -k ${ARGS}; cd ..; done
	cd ${MEDIR}; make ${ARGS}

inslib:
	for i in ${LIBDIR}; do cd $$i; make ${ARGS} install; cd ..; done
	cd ${MEDIR}; make ${ARGS} install
	
#
# NOTE: library must be built and installed before making any GENIX command
#

allcmd:
	cd cmd; echo REBUILDING cmd;make -k ${ARGS}

alldb16k:
	cd db16k; echo REBUILDING db16k; make -k ${ARGS}

allgames:
	cd games; echo REBUILDING games; make -k ${ARGS}

# install rules

install: inscmd insdb16k insgames

inscmd:
	cd cmd; make -k ${ARGS} install

insdb16k:
	cd db16k; make -k ${ARGS} install

insgames:
	cd games; make -k ${ARGS} install


clean:
	for i in ${SUBDIRS}; do cd $$i; echo CLEANING $$i; make clean; cd ..; done

cleanall:
	for i in ${ALLDIRS}; do cd $$i; echo CLEANING $$i; make clean; cd ..; done
	cd ${MEDIR}; make clean

clobber:
	for i in ${ALLDIRS}; do cd $$i; make clobber; cd ..; done
	cd ${MEDIR}; make clobber
	${SCCSD} clean; ${SCCSD} get makefile
