#
# makefile: version 1.5 of 6/6/83
# Mesa Unix System Command Makefile
#
# @(#)makefile	1.5 (NSC) 6/6/83
#

.SUFFIXES: .O .c .x

SCCS	= /v/mesaux/src/cmd/troff/font
SCCSD	= sccs -d${SCCS}
DESTDIR	= /v/mesaux

CC	= nmcc
# This is necessary for building on the vax, must be adjusted once we're
# self hosted.
VCC	= cc

CFLAGS	= -O -DSCCS

SRCS = chars.c ftB.c ftBC.c ftBC.n ftC.c ftC.n ftCE.c ftCI.c ftCK.c ftCS.c\
	ftCW.c ftCW.n ftG.c ftGI.c ftGM.c ftGR.c ftGR.n ftI.c ftL.c ftLI.c\
	ftPA.c ftPB.c ftPI.c ftR.c ftR.n ftS.c ftSB.c ftSI.c ftSM.c ftUD.c\
	ftXM.c makefile makefonts.c mkfont.c mkfont1.c

FILES = ftB ftBC ftC ftCE ftCI ftCK ftCS ftCW ftG ftGI ftGM ftGR ftI \
	ftL ftLI ftPA ftPB ftPI ftR ftS ftSB ftSI ftSM ftUD ftXM

makefonts compile all:  sources 
	${VCC} ${CFLAGS} -o makefonts makefonts.c
	./makefonts
	chmod 664 ${FILES}

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

mkfont: mkfont.O
	${CC} mkfont.O -o mkfont.x

install:  makefonts
	minstall ${FILES} ${DESTDIR}/usr/lib/font
	cd ${DESTDIR}/usr/lib/font; rm -f ftH ftHI ftHM ftFD ftCI
	cd ${DESTDIR}/usr/lib/font; ln ftG ftH; ln ftGI ftHI; ln ftGM ftHM; \
		ln ftUD ftFD; ln ftCK ftCI

clean:
	rm -f makefonts ${FILES} mkfont.x mkfont.O
clobber:  clean
	sccs -d${SCCS} clean; ${SCCSD} get makefile

.c.O:
	${CC} ${CFLAGS} -c $*.c
