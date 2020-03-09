#
# makefile: version 1.4 of 6/7/83
# Mesa Unix System Makefile
#
# @(#)makefile	1.4 (NSC) 6/7/83
#

SCCS	= /v/mesaux/usr/lib/me/src
DESTDIR	= /v/mesaux

SCCSD	= sccs -d${SCCS}

MEFILES	= acm.me chars.me deltext.me eqn.me float.me footnote.me index.me \
	local.me null.me sh.me tbl.me thesis.me tmac.e

SRCS = ${MEFILES} install.csh test.accent test.umlaut revisions 

all: sources

install: sources
	chmod 550 install.csh
	install.csh $(MEFILES)

clean:
	rm -f ../*.me ../revisions ../../tmac/tmac.e

clobber:
	rm -f ../*.me ../revisions ../../tmac/tmac.e
	${SCCSD} clean; ${SCCSD} get makefile

# Handy dandy building rules.

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@
