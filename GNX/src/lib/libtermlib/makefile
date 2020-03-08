#
# %M%: version %I% of %G%
# %Y%
#
# %W% (NSC) %G%
#

.SUFFIXES: .o .c .S

SCCS	= /v/mesaux/src/lib/libtermlib
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AR	= ar
RANLIB	= nranlib
VGRIND	= csh /usr/ucb/vgrind

CEFLAGS =
CFLAGS	= -O -DCM_N -DCM_GT -DCM_B -DCM_D -DSCCS ${CEFLAGS}

OFILES	= termcap.o tgoto.o tputs.o

SRCS 	= termcap.c tgoto.c tputs.c

all:	termcap.a
termcap.a: ${OFILES}
	${AR} cr termcap.a termcap.o tgoto.o tputs.o

clean:
	-rm -f ${OFILES} termcap.a

install: termcap.a
	minstall -r termcap.a ${RELDIR}/libtermcap.a
	rm -f ${RELDIR}/libtermlib.a
	ln ${RELDIR}/libtermcap.a ${RELDIR}/libtermlib.a

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

vgrind:
	cp /dev/null index
	${VGRIND} -h "Termcap library" termcap.c tputs.c tgoto.c
	${VGRIND} -h "Termcap library" -x index

.c.o:
	${CC} ${CFLAGS} -c $*.c
