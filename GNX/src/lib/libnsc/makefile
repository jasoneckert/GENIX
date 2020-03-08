# @(#)makefile	1.1	7/15/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

SCCS	= /v/mesaux/src/lib/libnsc
DESTDIR	= /v/mesaux
RELDIR	= ${DESTDIR}/usr/lib

CC	= nmcc
AR	= ar
RANLIB	= nranlib
GET	= sccs -d${SCCS} get
CLEAN	= sccs -d${SCCS} clean

CEFLAGS =
CFLAGS	= -O -DSCCS ${CEFLAGS}

CFILES	= fopenp.c openp.c

OBJS	= fopenp.o openp.o

all: libnsc.a

libnsc.a: ${OBJS}
	${AR} cr libnsc.a ${OBJS}

install:
	minstall -r libnsc.a ${RELDIR}

clean:
	rm -f libnsc.a ${OBJS}

clobber: clean
	${CLEAN}
	${GET} ${Rmakefile} makefile

# Recreate the source dependency rules.
sourcerules:
	rm -f src_rules edsrc
	for i in ${CFILES};					do	\
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
#fopenp.c:	${SCCS}/SCCS/s.fopenp.c
#	${GET} ${Rfopenp.c} fopenp.c
#openp.c:	${SCCS}/SCCS/s.openp.c
#	${GET} ${Ropenp.c} openp.c
