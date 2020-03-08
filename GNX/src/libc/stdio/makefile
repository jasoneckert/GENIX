#
# makefile: version 1.17 of 8/27/83
# Mesa Unix C Library Source File
#
# @(#)makefile	1.17 (NSC) 8/27/83
#

.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/libc/stdio
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AS	= nasm

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}
AFLAGS	=

SRCS	= clrerr.c data.c doprnt.s doscan.c error.c fdopen.c fgetc.c	\
	fgets.c filbuf.c flsbuf.c fopen.c fprintf.c fputc.c fputs.c	\
	freopen.c fseek.c fstab.c ftell.c gcvt.c getchar.c getgrent.c	\
	getgrgid.c getgrnam.c getpass.c getpw.c getpwent.c getpwnam.c	\
	getpwuid.c gets.c getw.c intss.c popen.c printf.c putchar.c	\
	puts.c putw.c rdwr.c rew.c scanf.c setbuf.c sprintf.c strout.c	\
	stuff.c system.c tmpnam.c ungetc.c

OBJS	= clrerr.o data.o doprnt.o doscan.o error.o fdopen.o fgetc.o	\
	fgets.o filbuf.o flsbuf.o fopen.o fprintf.o fputc.o fputs.o	\
	freopen.o fseek.o fstab.o ftell.o gcvt.o getchar.o getgrent.o	\
	getgrgid.o getgrnam.o getpass.o getpw.o getpwent.o getpwnam.o	\
	getpwuid.o gets.o getw.o intss.o popen.o printf.o putchar.o	\
	puts.o putw.o rdwr.o rew.o scanf.o setbuf.o sprintf.o strout.o	\
	stuff.o system.o tmpnam.o ungetc.o

all: ${OBJS}

clean:
	rm -f ${OBJS}

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

# Temporary until compiler supports floating point.

doscan.o: doscan.c
	${CC} ${CFLAGS} -c $<

.c.o:
	${CC} ${CFLAGS} -c $*.c

.s.o:
	${AS} ${AFLAGS} -o $*.o $*.s
