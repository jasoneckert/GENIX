#
# makefile: version 1.12 of 11/14/83
# Mesa Unix System Makefile
#
# Copyright (c) 1982, 1983 National Semiconductor Corporation
#

#
# Recreate system include files from their corresponding sccs files.
#	Some include files come directly from the kernel source.
#	These are accessed through the KERN and KERNINC macros.
#	Most of the rest are independent of the kernel and are accessed
#	through the STD and STDINC macros.
#	Curses.h is accessed through the CURS and CURSINC macros.
#	System III-derived files are accessed through the SYS3 and
#	SYS3INC macros.
#

STD	= /v/mesaux/usr/include
STDINC	= a.out.h ar.h assert.h cnlist.h ctype.h dbm.h dumprestor.h	\
	  errno.h fstab.h fsr.h grp.h ident.h lastlog.h math.h		\
	  meld3.h mp.h mtab.h nlist.h pagsiz.h pwd.h ranlib.h		\
	  saio.h walkdir.h sccs.h setjmp.h sgtty.h signal.h stab.h	\
	  stdio.h sysexits.h time.h utmp.h valign.h varargs.h		\
	  wait.h whoami whoami.h
 
KERN	= /v/mesaux/src/sys/h
KERNINC	= vfont.h

CURS	= /v/mesaux/src/lib/libcurses
CURSINC	= curses.h unctrl.h

SYS3	= /v/sys3/usr/include
SYS3INC	= macros.h fatal.h

UCB42	= /usr/src/lib/libndir
UCB42INC= dir.h

SUBDIRS = local sys

all:	${STDINC} ${KERNINC} ${CURSINC} ${SYS3INC} ${UCB42INC}
	for i in ${SUBDIRS}; do cd $$i; make; cd ..; done

${STDINC}:
	sccs -d${STD}  get $@

${KERNINC}:
	sccs -d${KERN} get $@

${CURSINC}:
	sccs -d${CURS} get $@

${SYS3INC}:
	sccs -d${SYS3} get $@

${UCB42INC}:
	sccs -d${UCB42} get $@

install:	all

clean:
	rm -f ${STDINC} ${KERNINC} ${CURSINC} ${SYS3INC} ${UCB42INC}
	for i in ${SUBDIRS}; do cd $$i; make clean; cd ..; done
