# %W%	%G%
# %Z%Copyright (C) 1983 by National Semiconductor Corp.

.SUFFIXES: .x

DESTDIR	= /v/mesaux
SCCS	= /v/mesaux/src/games/adventure

CC	= nmcc
SCCSD	= sccs -d${SCCS}

CFLAGS	= -O ${CEFLAGS}

SRCS	= glorkz hdr.h done.c init.c io.c main.c okplay.c save.c subr.c\
	vocab.c wizard.c
OBJS	= done.o init.o io.o main.o save.o subr.o vocab.o wizard.o

all: adventure.x glorkz

adventure.x: a.out
#ifndef CROSS-SUPPORT
#	rm -f $@
#	echo y | ./a.out
#else
	@echo
	@echo '*** WARNING!! ***'
	@echo 'To finish building adventure, copy a.out and glorkz down to a
	@echo 'SYS16 system and give the command:
	@echo
	@echo '    echo y | ./a.out'
	@echo
	@echo 'Then bring the resulting file "advent" back to this directory
	@echo 'and call it adventure.x.'
	@echo '*****************'
	@echo
#endif

a.out: ${OBJS}
	${CC} ${CFLAGS} ${OBJS}

# Dependencies
done.c init.c io.c main.c subr.c vocab.c wizard.c: hdr.h

#save.o:	save.c	# vax C optimizer bug - infinite loop
#	${CC} -c save.c

# always recomple okplay
okplay.o: /tmp

install: adventure.x
	minstall adventure.x ${DESTDIR}/usr/games
#	cp okplay /usr/games/adventure
#	cp advsaver /usr/games/lib
#	dd if=/usr/games/lib/adventure of=/usr/games/lib/advheader bs=1 count=16
#	chmod 711 /usr/games/lib
#	chmod 700 /usr/games/lib/adventure
#	chmod 755 /usr/games/lib/advheader
#	chmod 755 /usr/games/lib/advsaver
#	chmod 4711 /usr/games/adventure
#	chown games /usr/games/adventure /usr/games/lib/adventure /usr/games/lib/advheader /usr/games/lib/advsaver
#	rm advent a.out okplay 

okplay: okplay.c
	${CC} ${CFLAGS} -o okplay okplay.c

advsaver: advsaver.s
	${AS} -o advsaver advsaver.s

clean:
	rm -f ${OBJS} a.out adventure.x core errs

clobber: clean
	${SCCSD} clean
	${SCCSD} get Makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@
