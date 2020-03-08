#
# makefile: version 1.18 of 11/17/83
# Mesa Unix System Command Source File
#
# @(#)makefile	1.18 (NSC) 11/17/83
#

#
#  VAX Version
#
#	@(#)makefile	2.2	12/6/80
#
#	Compilation Flags (in CFLAGS):
#		-DDEBUG -- compile in debug stuff.  This is not enabled
#			unless the debug flag is given, so the only
#			advantage in not including this is that the
#			binaries will be smaller.
#		-DLOG -- include log information.  This is probably
#			only useful on systems that include the logger.
#		-DVFORK -- use 'vfork' instead of 'fork'.
#		-DDBM -- use the dbm package instead of scanning the
#			ALIASFILE sequentially.  LIBS must include
#			-ldbm for this to work.
#

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/delivermail
SCCSD	= sccs -d${SCCS}

LIBS	= -ldbm
#LIBS 	=
TARG	= /mesa/etc

OBJS1	= conf.o main.o maketemp.o parse.o alias.o deliver.o \
	savemail.o addr.o err.o
OBJS2	= matchhdr.o sysexits.o util.o bmove.o
SRCS	= useful.h dlvrmail.h \
	conf.c deliver.c main.c parse.c err.c alias.c savemail.c addr.c \
	matchhdr.c sysexits.c util.c bmove.c bmove.11.s bmove.vax.s \
	arpa.c arpadate.c version.c maketemp.c \
	newaliases.c praliases.c Mail.rc
#	newaliases.c praliases.c

ALL	= delivermail.x newaliases arpa
#ALL	= delivermail.x arpa

CHOWN	= -echo chown
CHMOD	= chmod
CEFLAGS =
CFLAGS	= -O -DDBM -DVFORK -DDEBUG -DSCCS ${CEFLAGS}
#CFLAGS	= -O -DVFORK -DDEBUG -DSCCS ${CEFLAGS}
LDFLAGS	=
AR	= -ar
ARFLAGS	= rvu
LINT	= lint
LINTFLAGS = -bxa
XREF	= csh /usr/bin/xref
CP	= cp
CC	= nmcc

ROOT	= root
OBJMODE	= 775
DESTDIR	= /v/mesaux

delivermail.x: ${OBJS1} ${OBJS2} version.o Mail.rc
	${CC} ${CFLAGS} ${LDFLAGS} -o delivermail.x version.o ${OBJS1} ${OBJS2} ${LIBS}
	${CHMOD} ${OBJMODE} delivermail.x
	size delivermail.x; ls -l delivermail.x

install: ${ALL}
	minstall -s -x delivermail.x ${TARG}
	minstall Mail.rc ${DESTDIR}/usr/lib
	minstall -s -x newaliases ${DESTDIR}/usr/ucb

clean:
	rm -f core delivermail.x newaliases arpa uucp a.out xref
	rm -f *.o

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

${OBJS1}: dlvrmail.h

dlvrmail.h util.o: useful.h

all: ${ALL}

#
#  Auxiliary stuff
#

archive:
	${AR} ${ARFLAGS} delivermail.a READ_ME TO_BE_DONE makefile makefl.* *.h *.c *.s makeversion *.[123456789u]

print: xref
	@ls -l | pr -h "delivermail directory"
	@pr -h "cross reference listing" xref
	@size *.o | pr -h "object code sizes"
	@pr *.h *.[cs]

lint:
	${LINT} ${LINTFLAGS} ${SRCS}

xref: *.c
	${XREF} *.c > xref

#
#  Data base maintenance routines
#
AOBJS=	newaliases.o parse.o conf.o util.o
#AOBJS=	parse.o conf.o util.o

newaliases: ${AOBJS}
	${CC} ${CFLAGS} ${LDFLAGS} ${AOBJS} -o newaliases ${LIBS}

praliases: praliases.c
	${CC} ${CFLAGS} praliases.c -o praliases ${LIBS}

newaliases.o praliases.o: dlvrmail.h
#praliases.o: dlvrmail.h

#
#  Auxiliary mailers
#

arpa: arpa.o matchhdr.o arpadate.o
	${CC} ${CFLAGS} ${LDFLAGS} -o arpa arpa.o matchhdr.o arpadate.o ${LIBS}
	${CHMOD} ${OBJMODE} arpa
	size arpa; ls -l arpa

uucp: uucp.o
	${CC} ${CFLAGS} ${LDFLAGS} -o uucp uucp.o ${LIBS}
	${CHMOD} ${OBJMODE} uucp
	size uucp; ls -l uucp

mail:	mail.o getname.o
	${CC} ${CFLAGS} ${LDFLAGS} -o mail mail.o getname.o
	size mail; ls -l mail

.c.o:
	${CC} ${CFLAGS} -c $*.c
