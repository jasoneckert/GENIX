# @(#)makefile	1.4	8/29/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

#
# Makefile for awk.
#	This makefile contains some subtleties.  The source file
#	`proctab.c' is created dynamically by running `proc',
#	which, in turn, is built on the fly by compiling `proc.c'
#	and `tmptoken.c'.  To complicate things further, `tmptoken.c'
#	arises from munching on `token.c' and `awk.h', which is produced by
#	running yacc on `awk.g.y'.  Gasp!
#
#	To handle this situation, we need versions of the C compiler
#	that produce code for both the host and target machines
#	(CCV and CC, respectively).
#
#	`Tmptoken.c' is compiled to produce both host and target .o
#	file versions.  This circumstance is the cause of the rules
#	involving `h-token.o', which is the host machine version.
#

SCCS	= /v/mesaux/src/cmd/awk
DESTDIR	= /mesa

CC	= nmcc
CCV	= cc
GET	= sccs -d${SCCS} get
CLEAN	= sccs -d${SCCS} clean

CEFLAGS	=
CFLAGS	= -O -DSCCS ${CEFLAGS}
YFLAGS	= -d

# Various flavors of source file:
CFILES	= b.c freeze.c lib.c main.c parse.c proc.c run.c token.c tran.c
HFILES	= awk.def
LFILES	= awk.lx.l
YFILES	= awk.g.y
XFILES	= tokenscript

H-OFILES	= \
		h-token.o proc.o
T-HDEPS		= \
		awk.lx.o b.o freeze.o lib.o main.o parse.o proctab.o	\
		run.o token.o tran.o
T-OFILES	= awk.g.o ${T-HDEPS}

awk.x:	${T-OFILES}
	${CC} ${CFLAGS} -o awk.x ${T-OFILES} -lm

install:
	minstall -s -x awk.x ${DESTDIR}/bin

clean:
	rm -f ${H-OFILES} ${T-OFILES} awk.x tmptoken.c proc	\
		awk.h y.tab.h lex.yy.c proctab.c y.tab.c

clobber: clean
	${CLEAN}; ${GET} ${Rmakefile} makefile

sources: ${CFILES} ${HFILES} ${LFILES} ${YFILES} ${XFILES}

# Y.tab.h really depends on awk.g.y, being produced at the same time
# awk.g.o is produced as a consequence of yacc's -d flag being specified.
# However, there's no way to state this without running yacc twice,
# so we cop out this way instead.
y.tab.h:	awk.g.o

token.o:	tmptoken.c
	${CC} ${CFLAGS} -c tmptoken.c
	mv tmptoken.o token.o

# This rule implements some trickiness to avoid major
# recompilation when it's not really required.
awk.h:	y.tab.h
	-cmp -s y.tab.h awk.h || cp y.tab.h awk.h

${T-HDEPS}:	awk.h awk.def
awk.g.o:	awk.def

proctab.c:	proc
	proc > proctab.c


# The following rules generate the `proc' program and its component pieces.
# These .o files are all host-resident.

proc:	proc.o h-token.o
	${CCV} ${CFLAGS} -o proc proc.o h-token.o

proc.o:	proc.c awk.h
	${CCV} ${CFLAGS} -c proc.c

h-token.o:	tmptoken.c
	rm -f h-token.c
	ln tmptoken.c h-token.c
	${CCV} ${CFLAGS} -c h-token.c
	rm -f h-token.c

tmptoken.c:	awk.h token.c tokenscript
	rm -f temp tmptoken.c
	ed - < tokenscript
	rm -f temp

find:
	egrep -n "${PAT}" *.[ylhc] awk.def

list:
	-pr ${SRCS} makefile tokenscript README EXPLAIN

lint:
	lint -spu b.c main.c token.c tran.c run.c lib.c parse.c -lm |\
		egrep -v '^(error|free|malloc)'

# Recreate the source dependency rules.
sourcerules:
	rm -f src_rules edsrc
	for i in ${CFILES} ${HFILES} ${LFILES} ${YFILES} ${XFILES}; do	\
		echo "$$i:	\$${SCCS}/SCCS/s.$$i"	>> src_rules;	\
		echo "	\$${GET} \$${R$$i} $$i"		>> src_rules;	\
	done
	echo '$$r src_rules'				>> edsrc
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d'	>> edsrc
	echo '$$r src_rules'				>> edsrc
	echo 'w'					>> edsrc
	cp makefile makefile.bak
	ed - makefile					<  edsrc
	rm edsrc src_rules

# DO NOT DELETE THIS LINE -- make sourcerules depends on it.
#b.c:	${SCCS}/SCCS/s.b.c
#	${GET} ${Rb.c} b.c
#freeze.c:	${SCCS}/SCCS/s.freeze.c
#	${GET} ${Rfreeze.c} freeze.c
#lib.c:	${SCCS}/SCCS/s.lib.c
#	${GET} ${Rlib.c} lib.c
#main.c:	${SCCS}/SCCS/s.main.c
#	${GET} ${Rmain.c} main.c
#parse.c:	${SCCS}/SCCS/s.parse.c
#	${GET} ${Rparse.c} parse.c
#proc.c:	${SCCS}/SCCS/s.proc.c
#	${GET} ${Rproc.c} proc.c
#run.c:	${SCCS}/SCCS/s.run.c
#	${GET} ${Rrun.c} run.c
#token.c:	${SCCS}/SCCS/s.token.c
#	${GET} ${Rtoken.c} token.c
#tran.c:	${SCCS}/SCCS/s.tran.c
#	${GET} ${Rtran.c} tran.c
#awk.def:	${SCCS}/SCCS/s.awk.def
#	${GET} ${Rawk.def} awk.def
#awk.lx.l:	${SCCS}/SCCS/s.awk.lx.l
#	${GET} ${Rawk.lx.l} awk.lx.l
#awk.g.y:	${SCCS}/SCCS/s.awk.g.y
#	${GET} ${Rawk.g.y} awk.g.y
#tokenscript:	${SCCS}/SCCS/s.tokenscript
#	${GET} ${Rtokenscript} tokenscript
