#
# makefile: version 3.55 of 11/9/83
# Mesa Unix Makefile
#
# @(#)makefile	3.55 (National Semiconductor) 11/9/83
#

CC	= nmcc
AS	= nasm
LD	= nmeld

# Number of versions of unix images (plus 2) to keep.
VERSIONKEEP	= 7

IDENT	= -DNS16032
FONT	= Typerite_10

# To eliminate flavor key debugging typeouts, set FLAVORDUMP to the null string.
FLAVORDUMP = -DFLAVORDUMP

# To configure in a driver, set the definition value to 1:
BM	= -DNBM=0
RS	= -DNRS=1
SIO	= -DNSIO=1
LP	= -DNLP=1
DRIVERS	= ${BM} ${RS} ${SIO} ${LP}

COPTS	= ${IDENT} ${DRIVERS} ${FLAVORDUMP} -DNOSCCS -DKERNEL -DUCB_NTTY \
	-DNO_OLDTTY -I.
CFLAGS  = -O ${COPTS}

OFILES	= acct.o alloc.o clock.o dsort.o fio.o iget.o ioctl.o locore.o \
	machdep.o main.o nami.o param.o partab.o pipe.o prf.o prim.o \
	rdwri.o sig.o slp.o sufu.o subr.o swtch.o sys.o sys1.o sys2.o \
	sys3.o sys4.o sysent.o trap.o vm.o vminit.o vmmem.o vmsched.o \
	vmswap.o vmsys.o vmtext.o vmtrap.o bm.o bio.o conf.o lp.o pty.o \
	rs.o sio.o tty.o ttynew.o dcu.o tcu.o mem.o blt.o termem.o \
	flavors.o ${FONT}.o

CFILES=	../sys/acct.c ../sys/alloc.c ../sys/clock.c ../sys/fio.c \
	../sys/genassym.c ../sys/iget.c ../sys/ioctl.c ../sys/machdep.c \
	../sys/main.c ../sys/nami.c ../sys/param.c ../sys/partab.c \
	../sys/pipe.c ../sys/prf.c ../sys/prim.c ../sys/rdwri.c \
	../sys/sig.c ../sys/slp.c ../sys/stash.c ../sys/subr.c ../sys/sufu.c \
	../sys/swtch.c ../sys/sys.c ../sys/sys1.c ../sys/sys2.c ../sys/sys3.c \
	../sys/sys4.c ../sys/sysent.c ../sys/trap.c \
	../sys/vm.c ../sys/vminit.c ../sys/vmmem.c ../sys/vmsched.c \
	../sys/vmswap.c ../sys/vmsys.c ../sys/vmtext.c ../sys/vmtrap.c \
	../dev/bm.c ../dev/bio.c ../dev/conf.c ../dev/dcu.c ../dev/dsort.c \
	../dev/lp.c ../dev/mem.c ../dev/pty.c ../dev/rs.c ../dev/sio.c	\
	../dev/tcu.c ../dev/termem.c ../dev/tty.c ../dev/ttynew.c \
	../dev/flavors.c

unix:	vmunix

vmunix:	${OFILES} makefile
	@touch version
	@echo 0 `cat version` 1 + p | dc > version.new
	@mv version.new version
	@echo building vmunix, version `cat version`
	@echo 'char version[] = "NSC 16032 GENIX (Rev 4.'`cat version`') '`date`'\n";' > vers.c
	@${CC} ${CFLAGS} -c vers.c
	@echo ${LD} -o vmunix`cat version` -e startcode ${OFILES} vers.o 
	@${LD} -o vmunix`cat version` -e startcode ${OFILES} vers.o 
	@echo chmod +x vmunix`cat version`
	@chmod +x vmunix`cat version`
	rm -f vmunix versions
	@echo ln vmunix`cat version` vmunix
	@ln vmunix`cat version` vmunix
	@echo rm -f `ls -t vmunix* | tail +${VERSIONKEEP}`
	@rm -f `ls -t vmunix* | tail +${VERSIONKEEP}`
	@ls -l vmunix
	@nsize vmunix
	@what -f ../h/*.h ../sys/*.[cs] ../dev/*.[cs] | fgrep '	' > versions`cat version`
	@ln versions`cat version` versions
	@rm -f `ls -t versions* | tail +${VERSIONKEEP}`

sources:

clean:
	rm -f ${OFILES} locore.i locore.c x.i a.out assym.s vers.c vers.o \
	version.new

lint: /tmp
	@lint -hbxn ${COPTS} ../sys/Locore.c \
	  ${CFILES} | \
	    grep -v 'struct/union .* never defined' | \
	    grep -v 'possible pointer alignment problem'

locore.o: assym.s ../sys/icu.s ../sys/m16.s \
    ../h/trap.h ../h/psr.h  ../h/param.h ../h/devvm.h
	cat assym.s ../sys/icu.s ../sys/m16.s > locore.c
	${CC} -E -DLOCORE ${CFLAGS} locore.c > locore.i
	sed "/^#/s//;	/" locore.i > x.i
	${AS} -o locore.o x.i
	@rm locore.i locore.c x.i

assym.s: ../sys/genassym.c ../h/vmmeter.h ../h/panic.h ../h/user.h ../h/proc.h
	rm -f assym.s
	cc ${CFLAGS} ../sys/genassym.c; ./a.out > assym.s; rm -f a.out

blt.o: ../dev/blt.s ../h/devvm.h
	cat ../dev/blt.s > blt.c
	${CC} -E -DLOCORE ${CFLAGS} blt.c > blt.i
	sed "/^#/s//;	/" blt.i > x.i
	${AS} -o blt.o x.i
	rm -f blt.i blt.c x.i

Typerite_10.o: ../dev/Typerite_10.s
	${AS} -o Typerite_10.o ../dev/Typerite_10.s

# We need to worry about assembly sources below also.  We assume there are no
# files involved that begin with `..' except for `..' itself.
#	The rule works by sequentially producing the dependencies for each
#	source file.  It builds each such list by using the C preprocessor to
#	collect all files required by the current source file.  Grep then
#	collects line directive lines, and awk and sed strip out unwanted
#	fields and punctuation.  The final awk formats the dependencies into
#	a compact form.  (The final grep throws away the entry for the source
#	file itself -- this dependency is recorded elsewhere.)
depend:
	rm -f makedep eddep
	for i in ${CFILES};					do	\
		${CC} -E ${CFLAGS} $$i				|	\
		grep '^#'					|	\
		awk '{ print $$3 }'				|	\
		sed -e 's|/[^/]*/\.\.||g' -e 's|"||g'		|	\
		sort -u						|	\
		grep -v "$$i"					|	\
		awk "BEGIN { of = \"`basename $$i .c`\" \".o: \"; rec = of }\
		     { if (length(rec \$$0) > 78) { print rec; rec = of \$$0 }\
		      else rec = rec \" \" \$$0 }			\
		     END { print rec }" >> makedep		;	\
	done
	echo '$$r makedep' >>eddep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >>eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp makefile makefile.bak
	ed - makefile < eddep
	rm eddep makedep

tags:
	/usr/ucb/ctags ${CFILES}

print:
	@pr makefile
	@/usr/ucb/ctags -x ${CFILES} | pr -h XREF
	@(nsize vmunix *.o) | pr -h sizes
	@ls -ls . ../h ../sys ../dev | pr
	@pr /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sgtty.h
	@pr /v/mesaux/usr/include/wait.h
	@pr *.c *.s
	@cd ../h; pr *.m *.h
	@cd ../sys; pr *.c *.s
	@cd ../dev; pr *.c *.s

acct.o: ../sys/acct.c
	${CC} -c ${CFLAGS} ../sys/acct.c

alloc.o: ../sys/alloc.c
	${CC} -c ${CFLAGS} ../sys/alloc.c

clock.o: ../sys/clock.c
	${CC} -c ${CFLAGS} ../sys/clock.c

dsort.o: ../dev/dsort.c
	${CC} -c ${CFLAGS} ../dev/dsort.c

fio.o: ../sys/fio.c
	${CC} -c ${CFLAGS} ../sys/fio.c

iget.o: ../sys/iget.c
	${CC} -c ${CFLAGS} ../sys/iget.c

ioctl.o: ../sys/ioctl.c
	${CC} -c ${CFLAGS} ../sys/ioctl.c

machdep.o: ../sys/machdep.c
	${CC} -c ${CFLAGS} ../sys/machdep.c

main.o: ../sys/main.c
	${CC} -c ${CFLAGS} ../sys/main.c

nami.o: ../sys/nami.c
	${CC} -c ${CFLAGS} ../sys/nami.c

param.o: ../sys/param.c
	${CC} -c ${CFLAGS} ../sys/param.c

partab.o: ../sys/partab.c
	${CC} -c ${CFLAGS} ../sys/partab.c

pipe.o: ../sys/pipe.c
	${CC} -c ${CFLAGS} ../sys/pipe.c

prf.o: ../sys/prf.c
	${CC} -c ${CFLAGS} ../sys/prf.c

sufu.o: ../sys/sufu.c
	${CC} -c ${CFLAGS} ../sys/sufu.c

swtch.o: ../sys/swtch.c
	${CC} -c ${CFLAGS} ../sys/swtch.c

vm.o:	../sys/vm.c
	${CC} -c ${CFLAGS} ../sys/vm.c

vminit.o: ../sys/vminit.c
	${CC} -c ${CFLAGS} ../sys/vminit.c

vmmem.o: ../sys/vmmem.c
	${CC} -c ${CFLAGS} ../sys/vmmem.c

vmsched.o: ../sys/vmsched.c
	${CC} -c ${CFLAGS} ../sys/vmsched.c

vmswap.o: ../sys/vmswap.c
	${CC} -c ${CFLAGS} ../sys/vmswap.c

vmsys.o: ../sys/vmsys.c
	${CC} -c ${CFLAGS} ../sys/vmsys.c

vmtext.o: ../sys/vmtext.c
	${CC} -c ${CFLAGS} ../sys/vmtext.c

vmtrap.o: ../sys/vmtrap.c
	${CC} -c ${CFLAGS} ../sys/vmtrap.c

bio.o:	../dev/bio.c
	${CC} -c ${CFLAGS} ../dev/bio.c

prim.o: ../sys/prim.c
	${CC} -c ${CFLAGS} ../sys/prim.c

rdwri.o: ../sys/rdwri.c
	${CC} -c ${CFLAGS} ../sys/rdwri.c

sig.o: ../sys/sig.c
	${CC} -c ${CFLAGS} ../sys/sig.c

stash.o: ../sys/stash.c
	${CC} -c ${CFLAGS} ../sys/stash.c

slp.o: ../sys/slp.c
	${CC} -c ${CFLAGS} ../sys/slp.c

subr.o: ../sys/subr.c
	${CC} -c ${CFLAGS} ../sys/subr.c

sys.o: ../sys/sys.c
	${CC} -c ${CFLAGS} ../sys/sys.c

sys1.o: ../sys/sys1.c
	${CC} -c ${CFLAGS} ../sys/sys1.c

sys2.o: ../sys/sys2.c
	${CC} -c ${CFLAGS} ../sys/sys2.c

sys3.o: ../sys/sys3.c
	${CC} -c ${CFLAGS} ../sys/sys3.c

sys4.o: ../sys/sys4.c
	${CC} -c ${CFLAGS} ../sys/sys4.c

sysent.o: ../sys/sysent.c
	${CC} -c ${CFLAGS} ../sys/sysent.c

trap.o: ../sys/trap.c
	${CC} -c ${CFLAGS} ../sys/trap.c

conf.o: ../dev/conf.c
	${CC} -c ${CFLAGS} ../dev/conf.c

bm.o: ../dev/bm.c
	${CC} -c ${CFLAGS} ../dev/bm.c

termem.o: ../dev/termem.c
	${CC} -c ${CFLAGS} -DFONT=${FONT} ../dev/termem.c

tty.o: ../dev/tty.c
	${CC} -c ${CFLAGS} ../dev/tty.c

ttynew.o: ../dev/ttynew.c
	${CC} -c ${CFLAGS} ../dev/ttynew.c

ttyold.o: ../dev/ttyold.c
	${CC} -c ${CFLAGS} ../dev/ttyold.c

dcu.o: ../dev/dcu.c
	${CC} -c ${CFLAGS} ../dev/dcu.c

tcu.o: ../dev/tcu.c
	${CC} -c ${CFLAGS} ../dev/tcu.c

lp.o: ../dev/lp.c
	${CC} -c ${CFLAGS} ../dev/lp.c

mem.o: ../dev/mem.c
	${CC} -c ${CFLAGS} ../dev/mem.c

rs.o: ../dev/rs.c
	${CC} -c ${CFLAGS} ../dev/rs.c

sio.o: ../dev/sio.c
	${CC} -c ${CFLAGS} ../dev/sio.c

pty.o: ../dev/pty.c
	${CC} -c ${CFLAGS} ../dev/pty.c

flavors.o: ../dev/flavors.c
	${CC} -c ${CFLAGS} ../dev/flavors.c

# DO NOT DELETE THIS LINE -- make depend uses it
acct.o:  ../h/acct.h ../h/dir.h ../h/inode.h ../h/param.h ../h/proc.h
acct.o: ../h/pte.h ../h/systm.h ../h/types.h ../h/user.h ../h/vtimes.h
acct.o: /v/mesaux/usr/include/signal.h
alloc.o:  ../h/buf.h ../h/dir.h ../h/fblk.h ../h/filsys.h ../h/ino.h
alloc.o: ../h/inode.h ../h/mount.h ../h/param.h ../h/pte.h ../h/systm.h
alloc.o: ../h/types.h ../h/user.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
clock.o:  ../h/callout.h ../h/dir.h ../h/dk.h ../h/icu.h ../h/param.h
clock.o: ../h/proc.h ../h/psr.h ../h/pte.h ../h/reg.h ../h/systm.h ../h/types.h
clock.o: ../h/user.h ../h/vlimit.h ../h/vm.h ../h/vtimes.h
clock.o: /v/mesaux/usr/include/signal.h
fio.o:  ../h/acct.h ../h/conf.h ../h/dir.h ../h/file.h ../h/filsys.h
fio.o: ../h/inode.h ../h/mount.h ../h/param.h ../h/pte.h ../h/reg.h
fio.o: ../h/systm.h ../h/types.h ../h/user.h ../h/vtimes.h
fio.o: /v/mesaux/usr/include/signal.h
iget.o:  ../h/buf.h ../h/conf.h ../h/dir.h ../h/filsys.h ../h/ino.h
iget.o: ../h/inode.h ../h/mount.h ../h/param.h ../h/pte.h ../h/systm.h
iget.o: ../h/types.h ../h/user.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
ioctl.o:  ../h/conf.h ../h/dir.h ../h/file.h ../h/inode.h ../h/ioctl.h
ioctl.o: ../h/param.h ../h/proc.h ../h/pte.h ../h/reg.h ../h/systm.h ../h/tty.h
ioctl.o: ../h/types.h ../h/user.h ../h/vtimes.h /v/mesaux/usr/include/sgtty.h
ioctl.o: /v/mesaux/usr/include/signal.h
machdep.o:  ../h/buf.h ../h/conf.h ../h/devvm.h ../h/dir.h ../h/icu.h
machdep.o: ../h/inode.h ../h/mmu.h ../h/param.h ../h/proc.h ../h/psr.h
machdep.o: ../h/pte.h ../h/reboot.h ../h/reg.h ../h/systm.h ../h/timer.h
machdep.o: ../h/types.h ../h/user.h ../h/vm.h ../h/vtimes.h
machdep.o: /v/mesaux/usr/include/signal.h
main.o:  ../h/buf.h ../h/conf.h ../h/devvm.h ../h/dir.h ../h/filsys.h
main.o: ../h/inode.h ../h/mmu.h ../h/mount.h ../h/param.h ../h/proc.h
main.o: ../h/pte.h ../h/systm.h ../h/types.h ../h/user.h ../h/vlimit.h
main.o: ../h/vm.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
nami.o:  ../h/buf.h ../h/dir.h ../h/inode.h ../h/mount.h ../h/param.h
nami.o: ../h/pte.h ../h/systm.h ../h/types.h ../h/user.h ../h/vtimes.h
nami.o: /v/mesaux/usr/include/signal.h
param.o:  ../h/buf.h ../h/callout.h ../h/clist.h ../h/dir.h ../h/file.h
param.o: ../h/inode.h ../h/mount.h ../h/param.h ../h/proc.h ../h/pte.h
param.o: ../h/systm.h ../h/types.h ../h/user.h ../h/vm.h ../h/vtimes.h
param.o: /v/mesaux/usr/include/signal.h
partab.o:  ../h/param.h ../h/types.h /v/mesaux/usr/include/signal.h
pipe.o:  ../h/dir.h ../h/file.h ../h/inode.h ../h/param.h ../h/proc.h
pipe.o: ../h/pte.h ../h/reg.h ../h/systm.h ../h/types.h ../h/user.h
pipe.o: ../h/vtimes.h /v/mesaux/usr/include/signal.h
prf.o:  ../h/buf.h ../h/conf.h ../h/dir.h ../h/filsys.h ../h/ioctl.h
prf.o: ../h/mount.h ../h/msgbuf.h ../h/panic.h ../h/param.h ../h/pte.h
prf.o: ../h/reboot.h ../h/systm.h ../h/tty.h ../h/types.h ../h/user.h ../h/vm.h
prf.o: ../h/vtimes.h /v/mesaux/usr/include/sgtty.h
prf.o: /v/mesaux/usr/include/signal.h
prim.o:  ../h/buf.h ../h/clist.h ../h/conf.h ../h/ioctl.h ../h/param.h
prim.o: ../h/systm.h ../h/tty.h ../h/types.h /v/mesaux/usr/include/sgtty.h
prim.o: /v/mesaux/usr/include/signal.h
rdwri.o:  ../h/buf.h ../h/conf.h ../h/dir.h ../h/inode.h ../h/param.h
rdwri.o: ../h/pte.h ../h/systm.h ../h/types.h ../h/user.h ../h/vlimit.h
rdwri.o: ../h/vtimes.h /v/mesaux/usr/include/signal.h
sig.o:  ../h/dir.h ../h/fpu.h ../h/inode.h ../h/param.h ../h/proc.h ../h/psr.h
sig.o: ../h/pte.h ../h/reg.h ../h/systm.h ../h/types.h ../h/user.h
sig.o: ../h/vlimit.h ../h/vm.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
slp.o:  ../h/buf.h ../h/devvm.h ../h/dir.h ../h/file.h ../h/inode.h
slp.o: ../h/param.h ../h/proc.h ../h/pte.h ../h/systm.h ../h/types.h
slp.o: ../h/user.h ../h/vm.h ../h/vmsystm.h ../h/vtimes.h
slp.o: /v/mesaux/usr/include/signal.h
stash.o: 
subr.o:  ../h/buf.h ../h/conf.h ../h/dir.h ../h/inode.h ../h/param.h
subr.o: ../h/proc.h ../h/pte.h ../h/systm.h ../h/types.h ../h/user.h
subr.o: ../h/vtimes.h /v/mesaux/usr/include/signal.h
sufu.o: 
swtch.o:  ../h/devvm.h ../h/dir.h ../h/file.h ../h/inode.h ../h/mmu.h
swtch.o: ../h/param.h ../h/proc.h ../h/pte.h ../h/systm.h ../h/types.h
swtch.o: ../h/user.h ../h/vm.h ../h/vmmeter.h ../h/vtimes.h
swtch.o: /v/mesaux/usr/include/signal.h
sys.o:  ../h/conf.h ../h/dir.h ../h/ioctl.h ../h/param.h ../h/proc.h ../h/pte.h
sys.o: ../h/systm.h ../h/tty.h ../h/types.h ../h/user.h ../h/vtimes.h
sys.o: /v/mesaux/usr/include/sgtty.h /v/mesaux/usr/include/signal.h
sys1.o:  ../h/buf.h ../h/dir.h ../h/file.h ../h/inode.h ../h/mmu.h ../h/param.h
sys1.o: ../h/proc.h ../h/psr.h ../h/pte.h ../h/reg.h ../h/systm.h ../h/types.h
sys1.o: ../h/user.h ../h/vlimit.h ../h/vm.h ../h/vtimes.h
sys1.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/wait.h
sys2.o:  ../h/dir.h ../h/file.h ../h/inode.h ../h/param.h ../h/proc.h
sys2.o: ../h/pte.h ../h/reg.h ../h/systm.h ../h/types.h ../h/user.h
sys2.o: ../h/vtimes.h /v/mesaux/usr/include/signal.h
sys3.o:  ../h/buf.h ../h/conf.h ../h/dir.h ../h/file.h ../h/filsys.h ../h/ino.h
sys3.o: ../h/inode.h ../h/mount.h ../h/param.h ../h/pte.h ../h/reg.h
sys3.o: ../h/stat.h ../h/systm.h ../h/types.h ../h/user.h ../h/vtimes.h
sys3.o: /v/mesaux/usr/include/signal.h
sys4.o:  ../h/dir.h ../h/inode.h ../h/param.h ../h/proc.h ../h/pte.h
sys4.o: ../h/reboot.h ../h/reg.h ../h/systm.h ../h/timeb.h ../h/times.h
sys4.o: ../h/types.h ../h/user.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
sysent.o:  ../h/param.h ../h/systm.h ../h/types.h
sysent.o: /v/mesaux/usr/include/signal.h
trap.o:  ../h/dir.h ../h/mmu.h ../h/param.h ../h/proc.h ../h/psr.h ../h/pte.h
trap.o: ../h/reg.h ../h/systm.h ../h/trap.h ../h/types.h ../h/user.h
trap.o: ../h/vtimes.h /v/mesaux/usr/include/signal.h
vm.o:  ../h/buf.h ../h/conf.h ../h/devpm.h ../h/devvm.h ../h/dir.h ../h/mmu.h
vm.o: ../h/param.h ../h/proc.h ../h/psr.h ../h/pte.h ../h/reg.h ../h/systm.h
vm.o: ../h/types.h ../h/user.h ../h/vlimit.h ../h/vm.h ../h/vmmeter.h
vm.o: ../h/vtimes.h /v/mesaux/usr/include/signal.h
vminit.o:  ../h/buf.h ../h/conf.h ../h/devpm.h ../h/devvm.h ../h/dir.h
vminit.o: ../h/mmu.h ../h/param.h ../h/proc.h ../h/psr.h ../h/pte.h ../h/reg.h
vminit.o: ../h/systm.h ../h/types.h ../h/user.h ../h/vlimit.h ../h/vm.h
vminit.o: ../h/vmmeter.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
vmmem.o:  ../h/buf.h ../h/conf.h ../h/devpm.h ../h/devvm.h ../h/dir.h
vmmem.o: ../h/mmu.h ../h/param.h ../h/proc.h ../h/psr.h ../h/pte.h ../h/reg.h
vmmem.o: ../h/systm.h ../h/types.h ../h/user.h ../h/vlimit.h ../h/vm.h
vmmem.o: ../h/vmmeter.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
vmsched.o:  ../h/buf.h ../h/dir.h ../h/param.h ../h/proc.h ../h/pte.h
vmsched.o: ../h/systm.h ../h/types.h ../h/user.h ../h/vm.h ../h/vmmeter.h
vmsched.o: ../h/vmtune.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
vmswap.o:  ../h/buf.h ../h/conf.h ../h/devvm.h ../h/dir.h ../h/param.h
vmswap.o: ../h/proc.h ../h/pte.h ../h/reg.h ../h/systm.h ../h/types.h
vmswap.o: ../h/user.h ../h/vm.h ../h/vmmeter.h ../h/vtimes.h
vmswap.o: /v/mesaux/usr/include/signal.h
vmsys.o:  ../h/devvm.h ../h/dir.h ../h/file.h ../h/inode.h ../h/ioctl.h
vmsys.o: ../h/mmu.h ../h/param.h ../h/proc.h ../h/psr.h ../h/pte.h ../h/reg.h
vmsys.o: ../h/systm.h ../h/tty.h ../h/types.h ../h/user.h ../h/vlimit.h
vmsys.o: ../h/vm.h ../h/vtimes.h /v/mesaux/usr/include/sgtty.h
vmsys.o: /v/mesaux/usr/include/signal.h
vmtext.o:  ../h/devvm.h ../h/dir.h ../h/file.h ../h/inode.h ../h/ioctl.h
vmtext.o: ../h/mmu.h ../h/param.h ../h/proc.h ../h/psr.h ../h/pte.h ../h/reg.h
vmtext.o: ../h/systm.h ../h/tty.h ../h/types.h ../h/user.h ../h/vlimit.h
vmtext.o: ../h/vm.h ../h/vtimes.h /v/mesaux/usr/include/sgtty.h
vmtext.o: /v/mesaux/usr/include/signal.h
vmtrap.o:  ../h/buf.h ../h/conf.h ../h/devpm.h ../h/devvm.h ../h/dir.h
vmtrap.o: ../h/mmu.h ../h/param.h ../h/proc.h ../h/psr.h ../h/pte.h ../h/reg.h
vmtrap.o: ../h/systm.h ../h/types.h ../h/user.h ../h/vlimit.h ../h/vm.h
vmtrap.o: ../h/vmmeter.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
bm.o:  ../h/conf.h ../h/devvm.h ../h/dir.h ../h/ioctl.h ../h/param.h ../h/pte.h
bm.o: ../h/systm.h ../h/tty.h ../h/types.h ../h/user.h ../h/vm.h ../h/vtimes.h
bm.o: /v/mesaux/usr/include/sgtty.h /v/mesaux/usr/include/signal.h
bio.o:  ../h/buf.h ../h/conf.h ../h/devvm.h ../h/dir.h ../h/mmu.h ../h/param.h
bio.o: ../h/proc.h ../h/pte.h ../h/reg.h ../h/systm.h ../h/types.h ../h/user.h
bio.o: ../h/vm.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
conf.o:  ../h/buf.h ../h/conf.h ../h/dcusize.h ../h/dir.h ../h/file.h
conf.o: ../h/inode.h ../h/ioctl.h ../h/param.h ../h/proc.h ../h/pte.h
conf.o: ../h/systm.h ../h/tty.h ../h/types.h ../h/user.h ../h/vtimes.h
conf.o: /v/mesaux/usr/include/sgtty.h /v/mesaux/usr/include/signal.h
dcu.o:  ../h/buf.h ../h/conf.h ../h/dcu.h ../h/dcusize.h ../h/devvm.h
dcu.o: ../h/dir.h ../h/param.h ../h/pte.h ../h/systm.h ../h/types.h ../h/user.h
dcu.o: ../h/vtimes.h /v/mesaux/usr/include/signal.h
dsort.o:  ../h/buf.h ../h/param.h ../h/systm.h ../h/types.h
dsort.o: /v/mesaux/usr/include/signal.h
lp.o:  ../h/buf.h ../h/devvm.h ../h/dir.h ../h/icu.h ../h/ioctl.h ../h/param.h
lp.o: ../h/pte.h ../h/systm.h ../h/tty.h ../h/types.h ../h/user.h ../h/vtimes.h
lp.o: /v/mesaux/usr/include/sgtty.h /v/mesaux/usr/include/signal.h
mem.o:  ../h/buf.h ../h/conf.h ../h/devvm.h ../h/dir.h ../h/param.h ../h/pte.h
mem.o: ../h/systm.h ../h/types.h ../h/user.h ../h/vm.h ../h/vtimes.h
mem.o: /v/mesaux/usr/include/signal.h
pty.o:  ../h/buf.h ../h/conf.h ../h/dir.h ../h/file.h ../h/ioctl.h ../h/param.h
pty.o: ../h/pte.h ../h/systm.h ../h/tty.h ../h/types.h ../h/user.h
pty.o: ../h/vtimes.h /v/mesaux/usr/include/sgtty.h
pty.o: /v/mesaux/usr/include/signal.h
rs.o:  ../h/conf.h ../h/devvm.h ../h/dir.h ../h/icu.h ../h/ioctl.h ../h/param.h
rs.o: ../h/pdma.h ../h/pte.h ../h/rs.h ../h/timer.h ../h/tty.h ../h/types.h
rs.o: ../h/user.h ../h/vtimes.h /v/mesaux/usr/include/sgtty.h
rs.o: /v/mesaux/usr/include/signal.h
sio.o:  ../h/conf.h ../h/devvm.h ../h/dir.h ../h/file.h ../h/icu.h ../h/ioctl.h
sio.o: ../h/param.h ../h/pte.h ../h/sio.h ../h/tty.h ../h/types.h ../h/user.h
sio.o: ../h/vtimes.h /v/mesaux/usr/include/sgtty.h
sio.o: /v/mesaux/usr/include/signal.h
tcu.o:  ../h/buf.h ../h/conf.h ../h/devvm.h ../h/dir.h ../h/file.h ../h/mtio.h
tcu.o: ../h/param.h ../h/pte.h ../h/systm.h ../h/tcu.h ../h/types.h ../h/user.h
tcu.o: ../h/vm.h ../h/vtimes.h /v/mesaux/usr/include/signal.h
termem.o:  ../h/blt.h ../h/devvm.h ../h/ioctl.h ../h/param.h ../h/tty.h
termem.o: ../h/types.h ../h/vfont.h /v/mesaux/usr/include/sgtty.h
termem.o: /v/mesaux/usr/include/signal.h
tty.o:  ../h/buf.h ../h/conf.h ../h/dir.h ../h/file.h ../h/inode.h ../h/ioctl.h
tty.o: ../h/param.h ../h/proc.h ../h/pte.h ../h/reg.h ../h/systm.h ../h/tty.h
tty.o: ../h/types.h ../h/user.h ../h/vtimes.h /v/mesaux/usr/include/sgtty.h
tty.o: /v/mesaux/usr/include/signal.h
ttynew.o:  ../h/buf.h ../h/conf.h ../h/dir.h ../h/file.h ../h/inode.h
ttynew.o: ../h/ioctl.h ../h/param.h ../h/proc.h ../h/pte.h ../h/reg.h
ttynew.o: ../h/systm.h ../h/tty.h ../h/types.h ../h/user.h ../h/vtimes.h
ttynew.o: /v/mesaux/usr/include/sgtty.h /v/mesaux/usr/include/signal.h
flavors.o:  ../h/buf.h ../h/conf.h ../h/devvm.h ../h/dir.h ../h/dk.h ../h/icu.h
flavors.o: ../h/ioctl.h ../h/param.h ../h/proc.h ../h/pte.h ../h/reboot.h
flavors.o: ../h/reg.h ../h/rs.h ../h/systm.h ../h/tty.h ../h/types.h
flavors.o: ../h/user.h ../h/vm.h ../h/vmmeter.h ../h/vmtune.h ../h/vtimes.h
flavors.o: /v/mesaux/usr/include/sgtty.h /v/mesaux/usr/include/signal.h
