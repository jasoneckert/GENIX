#
# makefile: version 1.5 of 11/16/82
# (System III) Sccs Makefile
#
# @(#)makefile	1.5 (NSC) 11/16/82
#

#
# Recreate source files for all sccs-related programs.
#

SCCS	= .

GET	= sccs -d${SCCS} get

CFILES	= \
	admin.c bdiff.c comb.c delta.c get.c sccshelp.c prs.c prt.c	\
	rmchg.c sccs.c unget.c val.c vc.c what.c

SHFILES	= sccsdiff

all sources: ${CFILES} ${SHFILES}
${CFILES} ${SHFILES}:
	${GET} $@

clean:
#	rm -f ${CFILES} ${SHFILES}
	echo 'clean sccs/src'
