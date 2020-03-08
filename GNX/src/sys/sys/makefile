# @(#)makefile	3.16	8/22/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

#
# Recreate kernel sources (sys subdirectory).
#	To get a specific release of a particular file,
#	add a macro definition of the form:
#		Rfile.c	= -r3.17
#	Lacking such a definition, the most recent version is retrieved.
#
#	The `sourcerules' rule rebuilds the set of specific
#	source/sccs dependencies.
#

SCCS	= /v/mesaux/src/sys/sys

GET	= sccs -d${SCCS} get
CLEAN	= sccs -d${SCCS} clean

CFILES	= acct.c alloc.c clock.c fio.c genassym.c iget.c ioctl.c machdep.c \
	main.c nami.c param.c partab.c pipe.c prf.c prim.c rdwri.c sig.c \
	slp.c stash.c subr.c sufu.c swtch.c sys.c sys1.c sys2.c sys3.c sys4.c \
	sysent.c trap.c vm.c vminit.c vmmem.c vmsched.c vmswap.c vmsys.c \
	vmtext.c vmtrap.c

SFILES	= icu.s m16.s

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
acct.c:	${SCCS}/SCCS/s.acct.c
	${GET} ${Racct.c} acct.c
alloc.c:	${SCCS}/SCCS/s.alloc.c
	${GET} ${Ralloc.c} alloc.c
clock.c:	${SCCS}/SCCS/s.clock.c
	${GET} ${Rclock.c} clock.c
fio.c:	${SCCS}/SCCS/s.fio.c
	${GET} ${Rfio.c} fio.c
genassym.c:	${SCCS}/SCCS/s.genassym.c
	${GET} ${Rgenassym.c} genassym.c
iget.c:	${SCCS}/SCCS/s.iget.c
	${GET} ${Riget.c} iget.c
ioctl.c:	${SCCS}/SCCS/s.ioctl.c
	${GET} ${Rioctl.c} ioctl.c
machdep.c:	${SCCS}/SCCS/s.machdep.c
	${GET} ${Rmachdep.c} machdep.c
main.c:	${SCCS}/SCCS/s.main.c
	${GET} ${Rmain.c} main.c
nami.c:	${SCCS}/SCCS/s.nami.c
	${GET} ${Rnami.c} nami.c
param.c:	${SCCS}/SCCS/s.param.c
	${GET} ${Rparam.c} param.c
partab.c:	${SCCS}/SCCS/s.partab.c
	${GET} ${Rpartab.c} partab.c
pipe.c:	${SCCS}/SCCS/s.pipe.c
	${GET} ${Rpipe.c} pipe.c
prf.c:	${SCCS}/SCCS/s.prf.c
	${GET} ${Rprf.c} prf.c
prim.c:	${SCCS}/SCCS/s.prim.c
	${GET} ${Rprim.c} prim.c
rdwri.c:	${SCCS}/SCCS/s.rdwri.c
	${GET} ${Rrdwri.c} rdwri.c
sig.c:	${SCCS}/SCCS/s.sig.c
	${GET} ${Rsig.c} sig.c
slp.c:	${SCCS}/SCCS/s.slp.c
	${GET} ${Rslp.c} slp.c
stash.c:	${SCCS}/SCCS/s.stash.c
	${GET} ${Rstash.c} stash.c
subr.c:	${SCCS}/SCCS/s.subr.c
	${GET} ${Rsubr.c} subr.c
sufu.c:	${SCCS}/SCCS/s.sufu.c
	${GET} ${Rsufu.c} sufu.c
swtch.c:	${SCCS}/SCCS/s.swtch.c
	${GET} ${Rswtch.c} swtch.c
sys.c:	${SCCS}/SCCS/s.sys.c
	${GET} ${Rsys.c} sys.c
sys1.c:	${SCCS}/SCCS/s.sys1.c
	${GET} ${Rsys1.c} sys1.c
sys2.c:	${SCCS}/SCCS/s.sys2.c
	${GET} ${Rsys2.c} sys2.c
sys3.c:	${SCCS}/SCCS/s.sys3.c
	${GET} ${Rsys3.c} sys3.c
sys4.c:	${SCCS}/SCCS/s.sys4.c
	${GET} ${Rsys4.c} sys4.c
sysent.c:	${SCCS}/SCCS/s.sysent.c
	${GET} ${Rsysent.c} sysent.c
trap.c:	${SCCS}/SCCS/s.trap.c
	${GET} ${Rtrap.c} trap.c
vm.c:	${SCCS}/SCCS/s.vm.c
	${GET} ${Rvm.c} vm.c
vminit.c:	${SCCS}/SCCS/s.vminit.c
	${GET} ${Rvminit.c} vminit.c
vmmem.c:	${SCCS}/SCCS/s.vmmem.c
	${GET} ${Rvmmem.c} vmmem.c
vmsched.c:	${SCCS}/SCCS/s.vmsched.c
	${GET} ${Rvmsched.c} vmsched.c
vmswap.c:	${SCCS}/SCCS/s.vmswap.c
	${GET} ${Rvmswap.c} vmswap.c
vmsys.c:	${SCCS}/SCCS/s.vmsys.c
	${GET} ${Rvmsys.c} vmsys.c
vmtext.c:	${SCCS}/SCCS/s.vmtext.c
	${GET} ${Rvmtext.c} vmtext.c
vmtrap.c:	${SCCS}/SCCS/s.vmtrap.c
	${GET} ${Rvmtrap.c} vmtrap.c
icu.s:	${SCCS}/SCCS/s.icu.s
	${GET} ${Ricu.s} icu.s
m16.s:	${SCCS}/SCCS/s.m16.s
	${GET} ${Rm16.s} m16.s
