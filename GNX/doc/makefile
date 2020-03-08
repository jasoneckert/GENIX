#
# %M%: version %I% of %G%
# %Y%
#
# %W% (NSC) %G%
#

# Formatters and options.  

# Our typesetter program is currently qtroff, which drives a
# QMS Lasergrafix laser printer.  
# The -t flag allows us to save the typset output.  Without 
# the -t flag, qtroff drives the typsetter directly.
# The -F cm option specifies the font (currently Computer Modern).
#
# Note that for qtroff we don't send output
# through col, so the entire pipe segment is a parameter.
#
# For QMS laser printer output
#
# 	Filename suffix is .q instead of .lpr
# 	qtroff -t -F cm is the format parameter instead of nroff
# 	Output is directed to a publications directory (/o/vgasio/volume2)
#
# Although some files have entries for making .lpr versions, the
# files due to their content really only print on a typestting device.  
# These files include: troff, trofftut, tbl, and eqn.

EQN	= neqn
FMT	= nroff
DEV	= -Tlpr3
MS	= -msl
ME	= -mel
MM	= -mm
COL	= | col

# mel and msl are local names for standard ms and me macro packages

QFMT	= qtroff -t -F cm
QEQN	= eqn

PUBDIR	= /o/vgasio/volume2

# Volume 2

title.lpr: /tmp
	${FMT} ${DEV} vmunix/title.t > $@

title.q: /tmp
	rm -f ${PUBDIR}/title.q
	${QFMT} vmunix/title.t > ${PUBDIR}/title.q
	chmod 444 ${PUBDIR}/title.q

copyr.lpr: /tmp
	${FMT} ${DEV} vmunix/copyr.t > $@

copyr.q: /tmp
	rm -f ${PUBDIR}/copyr.q
	${QFMT} vmunix/copyr.t > ${PUBDIR}/copyr.q
	chmod 444 ${PUBDIR}/copyr.q

contents.lpr: /tmp
	${FMT} ${MS} ${DEV} vmunix/contents.t > $@

contents.q: /tmp
	rm -f ${PUBDIR}/contents.q
	${QFMT} ${MS} vmunix/contents.t > ${PUBDIR}/contents.q
	chmod 444 ${PUBDIR}/contents.q

summary.lpr: /tmp
	${EQN} summary/vhel* | ${FMT} ${MS} ${DEV} ${COL} > $@

summary.q: /tmp
	rm -f ${PUBDIR}/$@
	${QEQN} summary/vhel* | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

cacm.lpr: /tmp
	refer -e cacm/p* | tbl | ${FMT} ${MS} ${DEV} ${COL} > $@

cacm.q: /tmp
	rm -f ${PUBDIR}/$@
	refer -e cacm/p* | tbl | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

db16supt.lpr: /tmp
	tbl db16supt | ${FMT} ${DEV} ${ME} ${COL} > $@

db16supt.q: /tmp
	rm -f ${PUBDIR}/$@
	tbl db16supt | ${QFMT} ${ME} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

dbmon.lpr: /tmp
	tbl dbmon | ${FMT} ${DEV} ${ME} ${COL} > $@

dbmon.q: /tmp
	rm -f ${PUBDIR}/$@
	tbl dbmon | ${QFMT} ${ME} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

beginners.lpr: /tmp
	${EQN} -s9 beginners/u* | ${FMT} ${MS} ${DEV} ${COL} > $@

beginners.q: /tmp
	rm -f ${PUBDIR}/$@
	${QEQN} -s9 beginners/u* | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

shell.lpr: /tmp
	refer -e shell/t* | ${FMT} ${MS} ${DEV} ${COL} > $@

shell.q: /tmp
	rm -f ${PUBDIR}/$@
	refer -e shell/t* | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

csh.lpr: /tmp
	( cd csh; ${FMT} ${MS} ${DEV} csh.* ${COL} ) > $@

csh.q: /tmp
	rm -f ${PUBDIR}/$@
	( cd csh; ${QFMT} ${MS} csh.* ) > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

edtut.lpr: /tmp
	${FMT} ${MS} ${DEV} edtut/e* ${COL} > $@

edtut.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} edtut/e* > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

adv.ed.lpr: /tmp
	${FMT} ${MS} ${DEV} adv.ed/ae* ${COL} > $@

adv.ed.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} adv.ed/ae* > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

mail.lpr: /tmp
	${FMT} ${ME} ${DEV} mail/mail?.nr > $@

mail.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${ME} mail/mail?.nr > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

msmacros.lpr: /tmp
	tbl msmacros/ms | ${FMT} ${MS} ${DEV} ${COL} > $@
	tbl msmacros/refcard | ${EQN} | ${FMT} ${MS} ${DEV} ${COL} >> $@

msmacros.q: /tmp
	rm -f ${PUBDIR}/$@
	tbl msmacros/ms | ${QFMT} ${MS} > ${PUBDIR}/$@
	tbl msmacros/refcard | ${QEQN} | ${QFMT} ${MS} >> ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

memacros.lpr: /tmp
	${FMT} ${ME} ${DEV} memacros/intro.nr > $@
	${FMT} ${ME} ${DEV} memacros/ref.nr >> $@

memacros.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${ME} memacros/*.nr > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

troff.lpr: /tmp
	troff/tprint ${DEV} > $@

troff.q: /tmp
	rm -f ${PUBDIR}/$@
	( cd troff; tprintq ) > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

trofftut.lpr: /tmp
	${FMT} ${MS} ${DEV} trofftut/tt* > $@

trofftut.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} trofftut/tt* > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

tbl.lpr: /tmp
	tbl tbl | ${EQN} | ${FMT} ${MS} ${DEV} ${COL} > $@

tbl.q: /tmp
	rm -f ${PUBDIR}/$@
	tbl tbl | ${QEQN} | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

diction.lpr: /tmp
	tbl diction/rm* diction/ap* | ${EQN} | ${FMT} ${MS} ${DEV} ${COL} > $@

diction.q: /tmp
	rm -f ${PUBDIR}/$@
	tbl diction/rm* diction/ap* | ${QEQN} | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

refer.lpr: /tmp
	refer refer/refer | tbl | ${FMT} ${MS} ${DEV} > $@
	tbl refer/pubuse | ${FMT} -rP1 ${MS} ${DEV} >> $@

refer.q: /tmp
	rm -f ${PUBDIR}/$@
	refer refer/refer | tbl | ${QFMT} ${MS} > ${PUBDIR}/$@
	tbl refer/pubuse | ${QFMT} -rP1 ${MS} >> ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

eqn.lpr: /tmp
	${EQN} eqn/e* | ${FMT} ${MS} ${DEV} > $@
	${EQN} eqn/g* | ${FMT} ${MS} ${DEV} >> $@

eqn.q: /tmp
	rm -f ${PUBDIR}/$@
	${QEQN} eqn/e* | ${QFMT} ${MS} > ${PUBDIR}/$@
	${QEQN} eqn/g* | ${QFMT} ${MS} >> ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

cc.lpr:	/tmp
	tbl cc | ${FMT} ${DEV} ${ME} ${COL} > $@

cc.q:	/tmp
	rm -f ${PUBDIR}/$@
	tbl cc | ${QFMT} ${ME} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

lint.lpr: /tmp
	refer -e lint | ${FMT} ${MS} ${DEV} > $@

lint.q: /tmp
	rm -f ${PUBDIR}/$@
	refer -e lint | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

as.lpr:	/tmp
	tbl as | ${FMT} ${DEV} ${ME} ${COL} > $@

as.q:	/tmp
	rm -f ${PUBDIR}/$@
	tbl as | ${QFMT} ${ME} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

uprog.lpr: /tmp
	sed -f uprog/cwscript uprog/p? | ${FMT} ${MS} ${DEV} uprog/p.mac - > $@

uprog.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} uprog/p? - > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

# Qtroff does not use the cwscript because it requires special
# characters not currently available in our laser printer font set.

yacc.lpr: /tmp
	refer -e yacc/ss* | ${FMT} ${MS} ${DEV} ${COL} > $@

yacc.q: /tmp
	rm -f ${PUBDIR}/$@
	refer -e yacc/ss* | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

lex.lpr: /tmp
	tbl lex | ${FMT} ${MS} ${DEV} > $@

lex.q: /tmp
	rm -f ${PUBDIR}/$@
	tbl lex | ${QFMT} -mcp > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

m4.lpr: /tmp
	${FMT} ${MS} ${DEV} m4 > $@

m4.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} m4 > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

awk.lpr: /tmp
	refer -e awk | tbl | ${FMT} ${MS} ${DEV} ${COL} > $@

awk.q: /tmp
	rm -f ${PUBDIR}/$@
	refer -e awk | tbl | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

curses.lpr: /tmp
	cd curses; make curses.lpr

curses.q: /tmp
	rm -f ${PUBDIR}/$@
	cd curses; make curses.q
	chmod 444 ${PUBDIR}/$@
	
sccs.lpr: /tmp
	${FMT} ${ME} ${DEV} sccs.me > $@

sccs.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${ME} sccs.me > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

sys5.lpr: /tmp
	tbl sys5/sccs_guide | ${FMT} ${MM} ${DEV} -rO1.25i {COL} > $@
	${FMT} ${MM} ${DEV} -rO1.25i sys5/sccs_inter >> $@

sys5.q: /tmp
	rm -f ${PUBDIR}/sccs_guide
	rm -f ${PUBDIR}/sccs_inter
	tbl sys5/sccs_guide | ${QFMT} ${MM} -rO1.25i > ${PUBDIR}/sccs_guide
	${QFMT} ${MM} -rO1.25i sys5/sccs_inter > ${PUBDIR}/sccs_inter
	chmod 444 ${PUBDIR}/sccs_guide
	chmod 444 ${PUBDIR}/sccs_inter

sed.lpr: /tmp
	${FMT} ${MS} ${DEV} sed > $@

sed.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} sed > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

dc.lpr: /tmp
	${EQN} dc | ${FMT} ${MS} ${DEV} ${COL} > $@

dc.q: /tmp
	rm -f ${PUBDIR}/$@
	${QEQN} dc | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

bc.lpr: /tmp
	${FMT} ${MS} ${DEV} bc ${COL} > $@

bc.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} bc > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

make.lpr: /tmp
	${FMT} ${MS} ${DEV} make > $@

make.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} make > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

sys16.lpr: /tmp
	@echo Cannot make SYS16 Systems Manual yet.

sys16.q: /tmp
	@echo Cannot make SYS16 Systems Manual yet.

fsck.lpr: /tmp
	cd fsck; fsck.rc

fsck.q: /tmp
	cd fsck; fsck.rc.q

implement.lpr: /tmp
	refer -e implement | ${FMT} ${MS} ${DEV} > $@

implement.q: /tmp
	rm -f ${PUBDIR}/$@
	refer -e implement | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

iosys.lpr: /tmp
	${FMT} ${MS} ${DEV} iosys > $@

iosys.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} iosys > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

security.lpr: /tmp
	${FMT} ${MS} ${DEV} security > $@

security.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} security > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

password.lpr: /tmp
	tbl password | ${EQN} | ${FMT} ${MS} ${DEV} > $@

password.q: /tmp
	rm -f ${PUBDIR}/$@
	tbl password | ${QEQN} | ${QFMT} ${MS} > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

vmpaper.lpr: /tmp
	${FMT} ${MS} ${DEV} vmpaper | col > $@

vmpaper.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} vmpaper > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

uucp_impl.lpr: /tmp
	${FMT} ${MS} ${DEV} uucp/uucp_impl > uucp_impl.lpr

uucp.q: /tmp
	rm -f ${PUBDIR}/$@
	${QFMT} ${MS} uucp/network > ${PUBDIR}/$@
	chmod 444 ${PUBDIR}/$@

# Should be transferred to makefile in ex directory

vi.lpr: /tmp
	tbl ex/vi.in | ${FMT} ${MS} ${DEV} > $@
	tbl ex/vi.chars | ${FMT} ${MS} ${DEV} -n30 >> $@

vi.q: /tmp
	@echo Use makefile in ex directory

edit.tut.lpr: /tmp
	${FMT} ${MS} ${DEV} ex/edit.tut ${COL} > $@
	
edit.tut.q: /tmp
	@echo Use makefile in ex directory

ex.rm.lpr: /tmp
	${FMT} ${MS} ${DEV} ex/ex.rm > $@

ex.rm.q: /tmp
	@echo Use makefile in ex directory

ex.change.lpr: /tmp
	${FMT} ${MS} ${DEV} ex/ex3.1-3.5 > $@

# Internal documentation 

install.lpr: /tmp
	${FMT} ${MS} ${DEV} install ${COL} > $@

setup.lpr: /tmp
	tbl setup | ${FMT} ${MS} ${DEV} ${COL} > $@

rogue.lpr: /tmp
	tbl rogue | ${FMT} ${MS} ${DEV} ${COL} > $@

epascal.lpr: /tmp
	@echo Don\'t know how to make $@ yet.

ddt.lpr: /tmp
	@echo Don\'t know how to make $@ yet.

road_map.lpr: /tmp
	${FMT} ${MM} ${DEV} sys5/road_map ${COL} > $@

# Index to volume 1.
index.lpr: /tmp
	${FMT} ${MS} ${DEV} v7index > $@

