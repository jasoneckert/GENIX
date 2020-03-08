# @(#)makefile	3.10	8/11/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

#
# Recreate kernel sources (dev subdirectory).
#	To get a specific release of a particular file,
#	add a macro definition of the form:
#		Rfile.c	= -r3.17
#	Lacking such a definition, the most recent version is retrieved.
#
#	The `sourcerules' rule rebuilds the set of specific
#	source/sccs dependencies.
#

SCCS	= /v/mesaux/src/sys/dev

GET	= sccs -d${SCCS} get
CLEAN	= sccs -d${SCCS} clean

CFILES	= \
	bio.c bm.c conf.c dcu.c dsort.c lp.c mem.c pty.c rs.c	\
	sio.c tcu.c termem.c tty.c ttynew.c flavors.c

SFILES	= blt.s Typerite_10.s

sources: ${CFILES} ${SFILES}

clean:
	${CLEAN}

clobber: clean
	${GET} ${Rmakefile} makefile

# Recreate the source dependency rules.
sourcerules:
	rm -f src_rules edsrc
	for i in ${CFILES} ${SFILES};				do	\
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
bio.c:	${SCCS}/SCCS/s.bio.c
	${GET} ${Rbio.c} bio.c
bm.c:	${SCCS}/SCCS/s.bm.c
	${GET} ${Rbm.c} bm.c
conf.c:	${SCCS}/SCCS/s.conf.c
	${GET} ${Rconf.c} conf.c
dcu.c:	${SCCS}/SCCS/s.dcu.c
	${GET} ${Rdcu.c} dcu.c
dsort.c:	${SCCS}/SCCS/s.dsort.c
	${GET} ${Rdsort.c} dsort.c
lp.c:	${SCCS}/SCCS/s.lp.c
	${GET} ${Rlp.c} lp.c
mem.c:	${SCCS}/SCCS/s.mem.c
	${GET} ${Rmem.c} mem.c
pty.c:	${SCCS}/SCCS/s.pty.c
	${GET} ${Rpty.c} pty.c
rs.c:	${SCCS}/SCCS/s.rs.c
	${GET} ${Rrs.c} rs.c
sio.c:	${SCCS}/SCCS/s.sio.c
	${GET} ${Rsio.c} sio.c
tcu.c:	${SCCS}/SCCS/s.tcu.c
	${GET} ${Rtcu.c} tcu.c
termem.c:	${SCCS}/SCCS/s.termem.c
	${GET} ${Rtermem.c} termem.c
tty.c:	${SCCS}/SCCS/s.tty.c
	${GET} ${Rtty.c} tty.c
ttynew.c:	${SCCS}/SCCS/s.ttynew.c
	${GET} ${Rttynew.c} ttynew.c
flavors.c:	${SCCS}/SCCS/s.flavors.c
	${GET} ${Rflavors.c} flavors.c
blt.s:	${SCCS}/SCCS/s.blt.s
	${GET} ${Rblt.s} blt.s
Typerite_10.s:	${SCCS}/SCCS/s.Typerite_10.s
	${GET} ${RTyperite_10.s} Typerite_10.s
