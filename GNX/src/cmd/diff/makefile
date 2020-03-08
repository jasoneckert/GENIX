#
# makefile: version 1.6 of 8/17/83
# 
#
# @(#)makefile	1.6 (NSC) 8/17/83
#

.SUFFIXES:  .o .c .x

SCCS	= /v/mesaux/src/cmd/diff
BIN	= /mesa/bin
USRLIB	= /v/mesaux/usr/lib
SCCSD	= sccs -d${SCCS}

CC	= nmcc

DIFF	= /bin/diff
DIFFH	= /usr/lib/diffh
PR	= /usr/bin/pr
CEFLAGS =
CFLAGS	= -O -DDIFF='"${DIFF}"' -DDIFFH='"${DIFFH}"' -DPR='"${PR}"' -DSCCS ${CEFLAGS}

SRCS	= diff.h diff.c diffdir.c diffreg.c diffh.c
OBJS	= diff.o diffdir.o diffreg.o

BINFILES= diff.x
USRLIBFILES= diffh.x

all: diff.x diffh.x

install:
	minstall -s -x ${BINFILES} ${BIN}
	minstall -s -x ${USRLIBFILES} ${USRLIB}

clean:
	rm -f diff.x diffh.x ${OBJS} diffh.o

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

diff.x: ${OBJS}
	rm -f $@
	${CC} ${CFLAGS} -o $@ ${OBJS}
	chmod 770 $@

diff.c diffdir.c diffreg.c: diff.h

# Handy dandy building rules.

.c.x:
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@

.c.o:
	${CC} ${CFLAGS} -c $*.c
