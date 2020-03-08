#
#
# makefile: version 1.12 of 8/25/83
# Mesa Unix System Command Source File
#
# @(#)makefile	1.12 (NSC) 8/25/83
#

# Unix version 7.

.SUFFIXES: .o .c .x

SCCS	= /v/mesaux/src/cmd/ucbmail
DESTDIR = /v/mesaux
SCCSD	= sccs -d${SCCS}

CC  = nmcc
AS  = -nasm

CEFLAGS =
CFLAGS = -O -DVMUNIX -DV7 -DSCCS ${CEFLAGS}

XSTR = /usr/ucb/xstr
CTAGS = /usr/ucb/ctags -w
VPRINT = /usr/ucb/vprint
LIBES = -ljobs
ED  = -ed
SIZE = msize
RM  = -rm

OBJS = aux.o cmd1.o cmd2.o cmd3.o cmdtab.o collect.o edit.o fio.o \
	getname.o head.o v7.local.o lock.o optim.o names.o lex.o list.o main.o \
	quit.o send.o strings.o temp.o tty.o vars.o version.o \
	errlst.o str.o

SRCS = aux.c cmd1.c cmd2.c cmd3.c cmdtab.c collect.c edit.c fio.c fmt.c\
	getname.c head.c v7.local.c lock.c optim.c names.c lex.c list.c main.c \
	quit.c send.c strings.c temp.c tty.c vars.c \
	errlst.c

HFILES = rcv.h def.h glob.h local.h v7.local.h

FILES	= Mail.x fmt.x Mail.help Mail.help.~

MISC  = version rofix.ed versfix.ed Mail.help Mail.help.~

Mail.x:	${OBJS} tags
	rm -f Mail.x
	${CC} ${CFLAGS} -o Mail.x ${OBJS} ${LIBES}
#	@{SIZE} Mail
	chmod 770 Mail.x

install: ${FILES}
	minstall -s -x Mail.x ${DESTDIR}/usr/ucb
	minstall -s -x fmt.x ${DESTDIR}/usr/ucb
	minstall Mail.help Mail.help.~ ${DESTDIR}/usr/lib
	cd ${DESTDIR}/usr/ucb; rm -f mail; ln Mail mail

clean:
	${RM} -f *.o
	rm -f Mail.x a.out fmt.x x.c xs.c tags core
	cp /dev/null strings
#	rm -f version.c

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS} ${HFILES} ${MISC}
${SRCS} ${HFILES} ${MISC}:
	${SCCSD} get $@

${OBJS}: ${HFILES}

fmt.x:	fmt.c head.c
	rm -f fmt.x
	${CC} ${CFLAGS} -O -d2 fmt.c head.c -o fmt.x
	${RM} fmt.o head.o
	size fmt.x
	chmod 770 fmt.x

tags:
	${CTAGS} *.c;

lint:
	lint ${CFLAGS} ${SRCS}

str.o: strings
	${XSTR}
	${CC} ${CFLAGS} -S xs.c
	${ED} - <rofix.ed xs.s
	${AS} -o str.o xs.s
	${RM} xs.s
	${RM} xs.c

cmdtab.o: cmdtab.c
	${CC} -S ${CFLAGS} cmdtab.c
	${ED} - <rofix.ed cmdtab.s
	${AS} -o cmdtab.o cmdtab.s
	${RM} cmdtab.s

errlst.o: errlst.c
	${CC} -E ${CFLAGS} errlst.c | ${XSTR} -c -
	${CC} ${CFLAGS} -S x.c
	${ED} - <rofix.ed x.s
	${AS} -o errlst.o x.s
	${RM} x.s
	${RM} x.c

#
# Make the version file.  /tmp is always changing.
#
version.c: #	/tmp
	/bin/sh version > version.c

print:
	@${VPRINT} makefile rcv.h def.h glob.h local.h v7.local.h ${SRCS} fmt.c;

wc:
	@wc rcv.h def.h glob.h local.h v7.local.h ${SRCS}

sc:
	@grep -c \; rcv.h def.h glob.h local.h v7.local.h ${SRCS}

#
# Special massaging of C files for sharing of strings
#

.c.o:
	${CC} -E ${CFLAGS} $*.c | ${XSTR} -c -
	${CC} -c ${CFLAGS} x.c 
	mv x.o $*.o

