#
# makefile: version 1.26 of 9/15/83
# Mesa Unix C Library Makefile
#
# @(#)makefile	1.26 (NSC) 9/15/83
#

# UNRESOLVED:
#	Should display.O be part of this library?
#	The status of many of the .s files is also unclear.
#		E.g.: what is nsrt.s? Should it supercede str.s?
#		Checksp.s has been sccsified, but left out of this makefile,
#		as it seems to be only a debugging aid used for nmcc
#		development.

.SUFFIXES: .o .c .s

SCCS	= /v/mesaux/src/libc/gen
SCCSD	= sccs -d${SCCS}

CC	= nmcc
AS	= nasm

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}
AFLAGS	= -u

SRCS	= atof.s atoi.c atol.c calloc.c chrtab.c cnlist.c crypt.c ctime.c \
	ctype_.c ecvt.c epdtoadr.s errlst.c execvp.c frexp.s getenv.c	\
	getlogin.c index.c isatty.c l3tol.c ldexp.s ltol3.c malloc.c	\
	mktemp.c modf.s mon.c nlist.c perror.c qsort.c rand.c regex.c	\
	rin.c rindex.c sleep.c strcat.c strcatn.c strcmp.c \
	strcmpn.c strcpy.c strcpyn.c strlen.c strncat.c strncmp.c	\
	strncpy.c stty.c swab.c tell.c timezone.c ttyname.c ttyslot.c	\
	udiv.s valloc.c fstrcat.s fstrcmp.s fstrcpy.s fstrlen.s 

OBJS	= atof.o atoi.o atol.o calloc.o chrtab.o cnlist.o crypt.o ctime.o \
	ctype_.o epdtoadr.o errlst.o execvp.o frexp.o getenv.o getlogin.o \
	index.o isatty.o l3tol.o ldexp.o ltol3.o malloc.o mktemp.o mon.o \
	modf.o nlist.o perror.o qsort.o rand.o regex.o rindex.o \
	sleep.o strcat.o strcatn.o strcmp.o strcmpn.o strcpy.o	\
	strcpyn.o strlen.o strncat.o strncmp.o strncpy.o stty.o swab.o	\
	tell.o timezone.o ttyname.o ttyslot.o udiv.o valloc.o		\
	fstrcat.o fstrcmp.o fstrcpy.o fstrlen.o 

NOBUILD	= ecvt.o rin.o

all: ${OBJS}

clean:
	rm -f ${OBJS}

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

.c.o:
	${CC} ${CFLAGS} -c $*.c

.s.o:
	${AS} ${AFLAGS} -o $*.o $*.s
