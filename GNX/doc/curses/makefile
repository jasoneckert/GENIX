# @(#)makefile	1.2	11/14/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

#
#	makefile for screen package documentation
#

NROFF	= nroff
TROFF	= qtroff
VGRIND	= qgrind -f
NFLAGS	= -Tcrt
TFLAGS	= -F cm

BINDIR	= /usr/doc/curses
OUTDIR	= /o/vgasio/volume2

TARFILE	= cf curdoc.tar

DOCS	= doc.I doc.II doc.III doc.IV
RAW	= macros c_macros appen.B appen.A appen.C disclaim intro.0 intro.1 \
	intro.2 intro.3 intro.4 intro.5 ${DOCS} curses.3

CRAW	= win_st.c twinkle1.c twinkle2.c life.c
SOURCE	= macros appen.A.tbl appen.B appen.C disclaim intro.0 intro.1 \
	intro.2.tbl intro.3 intro.4 intro.5

#
# this section formats C input source into nice troffable (or nroffable)
# versions.  It uses the capabilites of "vgrind", which sets keywords in
# bold font, and comments in italics.
#

troffq:	Master macros ${SOURCE}
	soelim Master | ${VGRIND} |			\
	${TROFF} ${TFLAGS} -t -me - > ${OUTDIR}/Curses.q
troff:	Master macros ${SOURCE}
	soelim Master | ${VGRIND} |			\
	${TROFF} ${TFLAGS} -me -

Curses.doc: Master macros ${SOURCE}
	${NROFF} ${NFLAGS} -me Master > ${OUTDIR}/Curses.doc

intsrc: Master macros ${SOURCE}
	soelim Master | ${VGRIND} > ${OUTDIR}/curses.int

intro.5: ${DOCS}
appen.B: win_st.c
appen.C: twinkle1.c life.c twinkle2.c

intro.2.tbl: intro.2
	tbl intro.2 > intro.2.tbl

appen.A.tbl: appen.A
	tbl appen.A > appen.A.tbl

spell: ${SOURCE}
	/bin/csh ./Spellit ${SOURCE} ${DOCS}

install:
	cp makefile ${RAW} ${BINDIR}

tar:
	tar rv${TARFILE} Master ${RAW} ${CRAW} makefile

lpr: Curses
	-lpr Curses.doc

clean:
	rm -f appen.A.tbl intro.2.tbl curses.tar
