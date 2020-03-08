# @(#)makefile	3.25	9/16/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

#
# Recreate kernel sources (h subdirectory).
#	To get a specific release of a particular file,
#	add a macro definition of the form:
#		Rfile.h	= -r3.17
#	Lacking such a definition, the most recent version is retrieved.
#
#	The `sourcerules' rule rebuilds the set of specific
#	source/sccs dependencies.
#

SCCS	= /v/mesaux/src/sys/h

GET	= sccs -d${SCCS} get
CLEAN	= sccs -d${SCCS} clean

HFILES	= acct.h blt.h bootop.h buf.h callo.h callout.h clist.h conf.h \
	dcu.h dcusize.h devpm.h devvm.h dir.h disk.h dk.h fblk.h file.h \
	filsys.h fpu.h icu.h ino.h inode.h ioctl.h mem.h mmu.h \
	modtable.h mount.h msgbuf.h mtio.h panic.h param.h \
	pdma.h proc.h psr.h pte.h reboot.h reg.h rs.h sio.h stat.h \
	systm.h tcu.h timeb.h timer.h times.h trap.h tty.h types.h \
	user.h vfont.h vlimit.h vm.h vmmeter.h vmswap.h vmsystm.h \
	vmtune.h vtimes.h

all sources: ${HFILES}

clean:
	${CLEAN}

clobber: clean
	${GET} ${Rmakefile} makefile

# Recreate the source dependency rules.
sourcerules:
	rm -f src_rules edsrc
	for i in ${HFILES};					do	\
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
acct.h:	${SCCS}/SCCS/s.acct.h
	${GET} ${Racct.h} acct.h
blt.h:	${SCCS}/SCCS/s.blt.h
	${GET} ${Rblt.h} blt.h
bootop.h:	${SCCS}/SCCS/s.bootop.h
	${GET} ${Rbootop.h} bootop.h
buf.h:	${SCCS}/SCCS/s.buf.h
	${GET} ${Rbuf.h} buf.h
callo.h:	${SCCS}/SCCS/s.callo.h
	${GET} ${Rcallo.h} callo.h
callout.h:	${SCCS}/SCCS/s.callout.h
	${GET} ${Rcallout.h} callout.h
clist.h:	${SCCS}/SCCS/s.clist.h
	${GET} ${Rclist.h} clist.h
conf.h:	${SCCS}/SCCS/s.conf.h
	${GET} ${Rconf.h} conf.h
dcu.h:	${SCCS}/SCCS/s.dcu.h
	${GET} ${Rdcu.h} dcu.h
dcusize.h:	${SCCS}/SCCS/s.dcusize.h
	${GET} ${Rdcusize.h} dcusize.h
devpm.h:	${SCCS}/SCCS/s.devpm.h
	${GET} ${Rdevpm.h} devpm.h
devvm.h:	${SCCS}/SCCS/s.devvm.h
	${GET} ${Rdevvm.h} devvm.h
dir.h:	${SCCS}/SCCS/s.dir.h
	${GET} ${Rdir.h} dir.h
disk.h:	${SCCS}/SCCS/s.disk.h
	${GET} ${Rdisk.h} disk.h
dk.h:	${SCCS}/SCCS/s.dk.h
	${GET} ${Rdk.h} dk.h
fblk.h:	${SCCS}/SCCS/s.fblk.h
	${GET} ${Rfblk.h} fblk.h
file.h:	${SCCS}/SCCS/s.file.h
	${GET} ${Rfile.h} file.h
filsys.h:	${SCCS}/SCCS/s.filsys.h
	${GET} ${Rfilsys.h} filsys.h
fpu.h:	${SCCS}/SCCS/s.fpu.h
	${GET} ${Rfpu.h} fpu.h
icu.h:	${SCCS}/SCCS/s.icu.h
	${GET} ${Ricu.h} icu.h
ino.h:	${SCCS}/SCCS/s.ino.h
	${GET} ${Rino.h} ino.h
inode.h:	${SCCS}/SCCS/s.inode.h
	${GET} ${Rinode.h} inode.h
ioctl.h:	${SCCS}/SCCS/s.ioctl.h
	${GET} ${Rioctl.h} ioctl.h
mem.h:	${SCCS}/SCCS/s.mem.h
	${GET} ${Rmem.h} mem.h
mmu.h:	${SCCS}/SCCS/s.mmu.h
	${GET} ${Rmmu.h} mmu.h
modtable.h:	${SCCS}/SCCS/s.modtable.h
	${GET} ${Rmodtable.h} modtable.h
mount.h:	${SCCS}/SCCS/s.mount.h
	${GET} ${Rmount.h} mount.h
msgbuf.h:	${SCCS}/SCCS/s.msgbuf.h
	${GET} ${Rmsgbuf.h} msgbuf.h
mtio.h:	${SCCS}/SCCS/s.mtio.h
	${GET} ${Rmtio.h} mtio.h
panic.h:	${SCCS}/SCCS/s.panic.h
	${GET} ${Rpanic.h} panic.h
param.h:	${SCCS}/SCCS/s.param.h
	${GET} ${Rparam.h} param.h
pdma.h:	${SCCS}/SCCS/s.pdma.h
	${GET} ${Rpdma.h} pdma.h
proc.h:	${SCCS}/SCCS/s.proc.h
	${GET} ${Rproc.h} proc.h
psr.h:	${SCCS}/SCCS/s.psr.h
	${GET} ${Rpsr.h} psr.h
pte.h:	${SCCS}/SCCS/s.pte.h
	${GET} ${Rpte.h} pte.h
reboot.h:	${SCCS}/SCCS/s.reboot.h
	${GET} ${Rreboot.h} reboot.h
reg.h:	${SCCS}/SCCS/s.reg.h
	${GET} ${Rreg.h} reg.h
rs.h:	${SCCS}/SCCS/s.rs.h
	${GET} ${Rrs.h} rs.h
sio.h:	${SCCS}/SCCS/s.sio.h
	${GET} ${Rsio.h} sio.h
stat.h:	${SCCS}/SCCS/s.stat.h
	${GET} ${Rstat.h} stat.h
systm.h:	${SCCS}/SCCS/s.systm.h
	${GET} ${Rsystm.h} systm.h
tcu.h:	${SCCS}/SCCS/s.tcu.h
	${GET} ${Rtcu.h} tcu.h
timeb.h:	${SCCS}/SCCS/s.timeb.h
	${GET} ${Rtimeb.h} timeb.h
timer.h:	${SCCS}/SCCS/s.timer.h
	${GET} ${Rtimer.h} timer.h
times.h:	${SCCS}/SCCS/s.times.h
	${GET} ${Rtimes.h} times.h
trap.h:	${SCCS}/SCCS/s.trap.h
	${GET} ${Rtrap.h} trap.h
tty.h:	${SCCS}/SCCS/s.tty.h
	${GET} ${Rtty.h} tty.h
types.h:	${SCCS}/SCCS/s.types.h
	${GET} ${Rtypes.h} types.h
user.h:	${SCCS}/SCCS/s.user.h
	${GET} ${Ruser.h} user.h
vfont.h:	${SCCS}/SCCS/s.vfont.h
	${GET} ${Rvfont.h} vfont.h
vlimit.h:	${SCCS}/SCCS/s.vlimit.h
	${GET} ${Rvlimit.h} vlimit.h
vm.h:	${SCCS}/SCCS/s.vm.h
	${GET} ${Rvm.h} vm.h
vmmeter.h:	${SCCS}/SCCS/s.vmmeter.h
	${GET} ${Rvmmeter.h} vmmeter.h
vmswap.h:	${SCCS}/SCCS/s.vmswap.h
	${GET} ${Rvmswap.h} vmswap.h
vmsystm.h:	${SCCS}/SCCS/s.vmsystm.h
	${GET} ${Rvmsystm.h} vmsystm.h
vmtune.h:	${SCCS}/SCCS/s.vmtune.h
	${GET} ${Rvmtune.h} vmtune.h
vtimes.h:	${SCCS}/SCCS/s.vtimes.h
	${GET} ${Rvtimes.h} vtimes.h
