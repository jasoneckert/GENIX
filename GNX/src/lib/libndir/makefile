#
# makefile: version 1.10 of 7/14/83
# 4.2 -> 4.1 Directory Retrofit Makefile
#
# @(#)makefile	1.10 (NSC) 7/14/83
#

# SCCS  :source files sccs directory
# LSCCS :local sccs directory

.SUFFIXES: .o .S

DESTDIR	= /v/mesaux
RELDIR  = ${DESTDIR}/usr/lib
SCCS	= /usr/src/lib/libndir
LSCCS	= /v/mesaux/src/lib/libndir
SCCSD	= sccs -d${SCCS}
LSCCSD	= sccs -d${LSCCS}

CC	= nmcc
LD	= nmeld
RANLIB	= nranlib

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}
LDFLAGS	= -x -r

OBJS	= closedir.o opendir.o readdir.o scandir.o seekdir.o telldir.o walkdir.o
SRCS	= closedir.c opendir.c readdir.c scandir.c seekdir.c telldir.c walkdir.c
DIST	= README makefile dir.h ${SRCS}


libndir: dir.h ${OBJS}
	ar cru libndir ${OBJS}
	${RANLIB} libndir

install: libndir ${DIST}
#	rm -f ${DESTDIR}/usr/include/dir.h
#	minstall dir.h ${DESTDIR}/usr/include/dir.h
	rm -f ${RELDIR}/libndir.a
	minstall -r libndir ${RELDIR}/libndir.a

clean:
	rm -f libndir ${OBJS}

clobber: clean
	${SCCSD} clean
	${LSCCSD} clean; ${LSCCSD} get makefile

sources: ${DIST}
${DIST}:
	${SCCSD} get $@

dist: ${DIST}
	echo "#!/bin/csh -f" >dist
	echo "# Run this file as shell script" >> dist
	echo "mkdir libndir" >> dist
	echo "chdir libndir" >> dist
	for i in ${DIST}; do ( \
		echo "echo extracting $$i" >> dist; \
		echo "cat > $$i <<'E--O--F'" >> dist; \
		cat $$i >> dist; \
		echo "'E--O--F'" >> dist); done
	chmod 775 dist

.c.o:
	${CC} ${CFLAGS} -I. -c $*.c
