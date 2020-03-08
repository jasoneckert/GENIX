#
# makefile: version 1.8 of 7/11/83
# Unix System Command Source File
#
# @(#)makefile	1.8 (NSC) 7/11/83
#

.SUFFIXES: .O .T .N .c .x

SCCS	= /v/mesaux/src/cmd/troff
SCCSD	= sccs -d${SCCS}
DESTDIR	= /v/mesaux

CC	= nmcc
CEFLAGS	= 
CFLAGS	= -O -DSCCS -DVMUNIX -DBIG=1000 -DLOTSN ${CEFLAGS}

SRCS	= README d.h str.h tdef.h tdef.h.big tw.h v.h ni.c nii.c\
	n1.c n2.c n3.c n4.c n5.c t6.c svt6.c n6.c n7.c n8.c n9.c t10.c n10.c\
	hytab.c ntab.c suftab.c tab3.c

# .T and .N files are just like the .O files, but compiled without and
# with -DNROFF.  This allows all object files to be kept around when
# rebuilding several times.

TOBJS	= ni.T nii.T n1.T n2.T n3.T n4.T n5.T t6.T n7.T n8.T n9.T\
	t10.T hytab.T suftab.T tab3.T
NOBJS	= ni.N nii.N n1.N n2.N n3.N n4.N n5.N n6.N n7.N n8.N n9.N\
	n10.N hytab.N suftab.N ntab.N

all: troff.x nroff.x term.done font.done

troff.x: ${TOBJS}
	${CC} -o troff.x ${TOBJS}

nroff.x: ${NOBJS}
	${CC} -o nroff.x ${NOBJS}

term.done:
	cd term; make "CC=${CC}"
	touch term.done

font.done:
	cd font; make "CC=${CC}"
	touch font.done

clean:
	rm -f *.[TN] [tn]roff.x term.done font.done
	cd term; make clean
	cd font; make clean

clobber:
	rm -f *.[TN] [tn]roff.x term.done font.done
	${SCCSD} clean; ${SCCSD} get makefile
	cd term; make clobber
	cd font; make clobber

install: all
	minstall -s -x troff.x nroff.x ${DESTDIR}/usr/bin
	cd term; make "CC=${CC}" DESTDIR=${DESTDIR} install
	cd font; make "CC=${CC}" DESTDIR=${DESTDIR} install

${TOBJS} ${NOBJS}: d.h str.h tdef.h tw.h v.h

# Handy dandy building rules.

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.T:
	${CC} ${CFLAGS} -c $*.c
	mv $*.o $*.T

.c.N:
	${CC} ${CFLAGS} -DNROFF -c $*.c
	mv $*.o $*.N
