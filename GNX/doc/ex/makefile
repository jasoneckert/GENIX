#
# makefile: version 1.4 of 11/8/83
# 
#
# @(#)makefile	1.4 (NSC) 11/8/83
#

# Our typesetter program is currently qtroff,
# which drives a QMS laser printer.  We use two variants
# below: with and without the -t flag, so that we have
# the option of saving typeset output or driving the
# laser printer directly.
TROFF	= qtroff -F cm

PUBDIR	= /o/vgasio/volume2

all: exrm exsummary exchanges edittut viin viapp visummary viapwh
allq: exrmq exsummaryq exchangesq edittutq viinq viappq visummaryq viapwhq

exrm:
	${TROFF} -ms ex.rm
exrmq:
	rm -f ${PUBDIR}/ex.rm.q
	${TROFF} -t -ms ex.rm >${PUBDIR}/ex.rm.q
	chmod 444 ${PUBDIR}/ex.rm.q

exsummary:
	tbl ex.summary | ${TROFF} -ms
exsummaryq:
	rm -f ${PUBDIR}/ex.summary.q
	tbl ex.summary | ${TROFF} -t -ms >${PUBDIR}/ex.summary.q
	chmod 444 ${PUBDIR}/ex.summary.q

exchanges:
	${TROFF} -ms ex1.1-2.0
	${TROFF} -ms ex2.0-3.1
	${TROFF} -ms ex3.1-3.5
exchangesq:
	rm -f ${PUBDIR}/ex1.1-2.0.q
	rm -f ${PUBDIR}/ex2.0-3.1.q
	rm -f ${PUBDIR}/ex3.1-3.5.q
	${TROFF} -t -ms ex1.1-2.0 >${PUBDIR}/ex1.1-2.0.q
	${TROFF} -t -ms ex2.0-3.1 >${PUBDIR}/ex2.0-3.1.q
	${TROFF} -t -ms ex3.1-3.5 >${PUBDIR}/ex3.1-3.5.q
	chmod 444 ${PUBDIR}/ex1.1-2.0.q
	chmod 444 ${PUBDIR}/ex2.0-3.1.q
	chmod 444 ${PUBDIR}/ex3.1-3.5.q

edittut:
	${TROFF} -ms edit.tut
edittutq:
	rm -f ${PUBDIR}/edit.tut.q
	${TROFF} -t -ms edit.tut >${PUBDIR}/edit.tut.q
	chmod 444 ${PUBDIR}/edit.tut.q


# index for versatec is different from the one in edit.tut
# because the fonts are different and entries reference page
# rather than section numbers.  if you have a typesetter
# you should just use the index in edit.tut, and ignore editvindex.
editvindex:
	${TROFF} -ms -n22 edit.vindex
editvindexq:
	rm -f ${PUBDIR}/edit.vindex.q
	${TROFF} -t -ms -n22 edit.vindex >${PUBDIR}/edit.vindex.q
	chmod 444 ${PUBDIR}/edit.vindex.q

viin:
	tbl vi.in | ${TROFF} -ms 
viinq:
	rm -f ${PUBDIR}/vi.in.q
	tbl vi.in | ${TROFF} -t -ms  >${PUBDIR}/vi.in.q
	chmod 444 ${PUBDIR}/vi.in.q

# numbers below are guessed at
# if using a typesetter use -n23 instead of -n25.
viapp:
	tbl vi.chars | ${TROFF} -ms -n23
viappq:
	rm -f ${PUBDIR}/vi.chars.q
	tbl vi.chars | ${TROFF} -t -ms -n23 >${PUBDIR}/vi.chars.q
	chmod 444 ${PUBDIR}/vi.chars.q

visummary:
	tbl vi.summary | ${TROFF} -ms
visummaryq:
	rm -f ${PUBDIR}/vi.summary.q
	tbl vi.summary | ${TROFF} -t -ms >${PUBDIR}/vi.summary.q
	chmod 444 ${PUBDIR}/vi.summary.q

viapwh:
	${TROFF} -ms vi.apwh.ms;
viapwhq:
	rm -f ${PUBDIR}/vi.apwh.q
	${TROFF} -t -ms vi.apwh.ms >${PUBDIR}/vi.apwh.q
	chmod 444 ${PUBDIR}/vi.apwh.q
