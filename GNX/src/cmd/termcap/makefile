#
# makefile: version 1.3 of 7/5/83
# GENIX system command source file
#
# @(#)makefile	1.3 (NSC) 7/5/83
#

# reorder gives an editor command for most common terminals
# (in reverse order from n'th to 1'st most commonly used)
# to move them to the front of termcap
#

SCCS	= /v/mesaux/src/cmd/termcap
DESTDIR = /mesa
SCCSD	= sccs -d${SCCS}

SRCS	= termcap.src reorder

VGRIND=	csh /usr/ucb/vgrind

termcap.obj: ${SRCS}
	rm -f $@
	ed - termcap.src < reorder

install: termcap.obj
	minstall -x termcap.obj ${DESTDIR}/etc
	cd tabset; make install

clean:
	rm -f termcap.obj

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile
	cd tabset; make clobber

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

vgrind:
	${VGRIND} -n -h "Terminal Capibility Data Base" \
	termcap.src reorder makefile
