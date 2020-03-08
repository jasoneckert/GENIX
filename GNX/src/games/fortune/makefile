# @(#)makefile	1.3	9/14/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

RELDIR	= /v/mesaux
LIBDIR	= /usr/games/lib
DESTDIR = ${RELDIR}/usr/games
DESTLIB = ${RELDIR}${LIBDIR}
SCCS	= /v/mesaux/src/games/fortune/SCCS
CCV	= cc
CC	= nmcc
CFLAGS	= -O -s
INSTALL = minstall
SCCSD	= sccs -d${SCCS} get
SOURCES = fortune.c strfile.h strfile.c unstr.c scene obscene


all:	fortune.x strfile unstr fortunes.dat

fortune.x: strfile.h fortune.c
	${CC} ${CFLAGS} -DFORTFILE='"${LIBDIR}/fortunes.dat"' -o fortune.x fortune.c

strfile: strfile.h strfile.c
	${CCV} ${CFLAGS} -o strfile strfile.c

unstr:	strfile.h unstr.c
	${CCV} ${CFLAGS} -o unstr unstr.c

fortunes.dat: fortunes strfile
	./strfile fortunes

fortunes: scene obscene
	cat scene > fortunes
	echo "%-" >> fortunes
	cat obscene >> fortunes
	echo "%%" >> fortunes

install: all
	chmod 644 fortunes.dat
	${INSTALL} fortunes.dat ${DESTLIB}/fortunes.dat
	chmod 755 fortune.x
	${INSTALL} -s -x fortune.x ${DESTDIR}

clean:
	rm -f *.o fortune.x fortunes fortunes.dat fortunes.tar strfile unstr

sources:	${SOURCES}

${SOURCES}:
	${SCCSD} $@
