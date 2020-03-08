#
# makefile: version 1.4 of 8/25/83
# Unix System Command Source File
#
# @(#)makefile	1.4 (NSC) 8/25/83
#

.SUFFIXES: .o .c .e .s .x

SCCS	= /v/mesaux/src/cmd/tbl
DESTDIR	= /v/mesaux
SCCSD	= sccs -d${SCCS}

CC	= nmcc

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

SRCS	= t..c t0.c t1.c t2.c t3.c t4.c t5.c t6.c t7.c t8.c t9.c\
	tb.c tc.c te.c tf.c tg.c ti.c tm.c ts.c tt.c tu.c tv.c

OBJS	= t0.o t1.o t2.o t3.o t4.o t5.o t6.o t7.o t8.o t9.o\
	tb.o tc.o te.o tf.o tg.o ti.o tm.o ts.o tt.o tu.o tv.o

tbl.x:	${OBJS}
	rm -f tbl.x
	${CC} ${OBJS} -o tbl.x
	chmod 770 tbl.x

install: 
	minstall -s -x tbl.x ${DESTDIR}/usr/bin

clean:
	rm -f *.o tbl.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

${OBJS}: t..c

# Handy dandy building rules.

.c.o:
	${CC} ${CFLAGS} -c $*.c
