#
# makefile: version 1.9 of 8/17/83
# Unix System Command Source File
#
# @(#)makefile	1.9 (NSC) 8/17/83
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/diction
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

DICT	= -DDICT=\"/usr/lib/dict.d\"

CC	= nmcc

CEFLAGS	=
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= README conp.h deroff.c dict.c dict.d diction.c diction.sh\
	edict.c ehash.c end.l explain.d explain.sh names.h nhash.c nwords.l\
	outp.c part.l pscan.c style.h style.sh ydict.c

OBJS	= README deroff.x dict.d diction.sh dprog.x explain.d explain.sh\
	style.sh style1.x style2.x style3.x

all: ${OBJS}

style: style1 style2 style3

install: all
	minstall -x style.sh ${DESTDIR}/usr/bin
	minstall -s -x style[123].x ${DESTDIR}/usr/lib
	minstall -x diction.sh explain.sh ${DESTDIR}/usr/bin
	minstall dict.d explain.d ${DESTDIR}/usr/lib
	minstall -s -x dprog.x deroff.x ${DESTDIR}/usr/lib
	rm -f ${DESTDIR}/usr/bin/deroff
	ln ${DESTDIR}/usr/lib/deroff ${DESTDIR}/usr/bin/deroff

clean:
	rm -f prtlex.c wdslex.c endlex.c
	rm -f *.o *.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

dprog.x: diction.c
	rm -f dprog.x
	${CC} ${CFLAGS} ${DICT} diction.c -o dprog.x
	chmod 770 dprog.x

deroff.x: deroff.c
	rm -f deroff.x
	${CC} ${CFLAGS} deroff.c -o deroff.x
	chmod 770 deroff.x

style1.x: wdslex.o
	${CC} ${CFLAGS} wdslex.o -ll -o style1.x
wdslex.o: wdslex.c nhash.c dict.c ydict.c names.h
	${CC} ${CFLAGS} -c wdslex.c
wdslex.c: nwords.l
	lex nwords.l
	mv lex.yy.c wdslex.c

style2.x: endlex.o
	${CC} ${CFLAGS} endlex.o -ll -o style2.x
endlex.o: names.h endlex.c ehash.c edict.c
	${CC} ${CFLAGS} -c endlex.c
endlex.c: end.l
	lex end.l
	mv lex.yy.c endlex.c

style3.x: prtlex.o pscan.o outp.o
	${CC} ${CFLAGS} prtlex.o pscan.o outp.o -ll -o style3.x
prtlex.o: style.h names.h conp.h prtlex.c
	${CC} ${CFLAGS} -c prtlex.c
prtlex.c: part.l
	lex part.l
	mv lex.yy.c prtlex.c
pscan.o: names.h conp.h pscan.c
	${CC} ${CFLAGS} -c pscan.c
outp.o: names.h conp.h style.h outp.c
	${CC} ${CFLAGS} -c outp.c
