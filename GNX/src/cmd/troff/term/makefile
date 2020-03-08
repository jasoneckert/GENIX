#
# Makefile: version 1.9 of 6/6/83
# Unix System Command Source File
#
# @(#)Makefile	1.9 (NSC) 6/6/83
#

SCCS	= /v/mesaux/src/cmd/troff/term
SCCSD	= sccs -d${SCCS}
DESTDIR	= /v/mesaux

VCC	= cc

CFLAGS	= -O -DSCCS

SRCS = code.300 tab300.c \
	tab300s.c tab300S.c tab300X.c tab37.c \
	tab450.c tab450-12.c tab450-12-8.c tab450X.c tablpr.c tabtn300.c \
	tabdm.c tabh2.c tablpr2.c tabx1700.c \
	maketerms.c
FILES = tab300 tab300s tab300S tab300X tab37 \
	tab450 tab450-12 tab450-12-8 tab450X tablpr tabtn300 \
	tabdm tabh2 tablpr2 tabx1700

maketerms compile all:  sources ../tw.h
	${VCC} ${CFLAGS} -DPART1 -o maketerms maketerms.c
	./maketerms
	${VCC} ${CFLAGS} -DPART2 -o maketerms maketerms.c
	./maketerms
	${VCC} ${CFLAGS} -DPART3 -o maketerms maketerms.c
	./maketerms

../tw.h: 
	cd ..; ${SCCSD} get tw.h

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

install:  maketerms
	minstall ${FILES} ${DESTDIR}/usr/lib/term

clean:
	rm -f maketerms
	rm -f ${FILES}
clobber:  clean
	${SCCSD} clean; ${SCCSD} get Makefile
	rm -f tab450-12-8.c
