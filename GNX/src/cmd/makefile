# @(#)makefile	1.164	11/14/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

# This makefile builds all of the standard Unix system commands.  To add
# a new simple command, first add the name of its source file to SRCS.
# This is usually just a .c file.  Shell script sources are given the
# extension .sh.  Then add the name of the image file to one of BINFILES,
# ETCFILES, USRBINFILES, USRUCBFILES, USRNSCFILES or USRLIBFILES,
# depending on the directory it should be released into.  Image files
# normally have the extension .x so they won't be run by mistake when
# you're in this directory.  Finally, if the command requires a special
# owner (like root) or file modes (like set-user-id), add the name of the
# command (without the .x) to one of BINROOT, USRBINROOT, USRUCBROOT,
# USRLIBROOT, or TMPROOT, depending on the directory it should be
# released into.
# 
# More complex commands live in their own directory here.  Such a command
# should have its own makefile modeled after this file.  The makefile should
# have at least the following rules and in the same order: 
# 
# <default rule>
# 	Build a private copy of the command without releasing it
# 
# install
# 	Copy (not move) the command and any support files to the proper
# 	directories.  This should set up any necessary links too.
# 
# clean
# 	Delete all object files like .x and .o files.
#
# clobber
#	Does a clean and an sccs clean of the directory.  Then get the
#	latest copy of the makefile from SCCS.
# 
# sources
# 	Get latest copy of the source files from SCCS.  This rule is not
#	invoked recursively, but is used by the release procedures to
#	get the sources.  This also implies that all source files to be
#	shipped should be under SCCS, even those that were not changed
#	from 4.1bsd.
#
# Then add the name of the directory to SUBDIRS.

.SUFFIXES: .o .c .p .s .x

SCCS	= /v/mesaux/src/cmd
BIN	= /mesa/bin
ETC	= /mesa/etc
USRBIN	= /v/mesaux/usr/bin
USRUCB	= /v/mesaux/usr/ucb
USRLIB	= /v/mesaux/usr/lib
USRNSC	= /v/mesaux/usr/nsc

SCCSD	= sccs -d${SCCS}

# Where to find include files: needed for rules stating explicit dependencies.

INC	= /v/mesaux/usr/include

# The following definitions define the language software that is
# to be used.  All of these variables will be passed to lower-level
# make files.

CC	= nmcc
AS	= nasm
LD	= nmeld
PC	= nmpc
RANLIB  = nranlib
STRIP	= nstrip
CEFLAGS =
AEFLAGS =
ARGS	= CC="${CC}" AS="${AS}" PC="${PC}" LD="${LD}" RANLIB="${RANLIB}" CEFLAGS="${CEFLAGS}"

# These variables allow additional flags for C and the assembler
# to be specified that are peculiar to this directory and
# should not be passed to lower directories.

CFLAGS	= -O -DSCCS ${CEFLAGS}
AFLAGS	= ${AEFLAGS}

# List of all source files in this directory.  This is the same as the list
# of files SCCS/s.*, minus the `SCCS/s.', and minus the makefile.  This list
# is used to get sources out of sccs when necessary.

SRCS	= \
	ac.c accton.c apropos.sh ar.c at.c atrun.c\
	badsect.c basename.c bc.y burn.c\
	cal.c cat.c catman.c cb.c\
	checkeq.c checknr.c chfn.c chgrp.c chmod.c chsh.c chown.c\
	clear.c clri.c cmp.c col.c colcrt.c colrm.c comm.c\
	cp.c cron.c crypt.c csym.c ctags.c cu.c cu16.c\
	date.c dcheck.c dcerr.c dd.c df.c dmesg.c du.c dumpdir.c\
	echo.c egrep.y expand.c expr.y false.sh file.c fgrep.c find.c\
	finger.c fold.c from.c fsck.c\
	getNAME.c gets.c getty.c halt.c head.c help.sh hostname.c\
	icheck.c init.c kill.c\
	last.c lastcomm.c leave.c ln.c lock.c\
	login.c look.c mail.c makekey.c makewhatis.sh mesg.c\
	minstall.sh mkdir.c mkfs.c mkl+s.sh mknod.c mkstr.c \
	mmerr.c more.c mount.c monfix.c mt.c mv.c nburn.c ncheck.c newgrp.c\
	newsym.csh nice.c nm.c nohup.sh num.c od.c\
	passwd.c pr.c print.sh printenv.c prmail.c prof.c ps.c ptx.c\
	pwd.c quot.c ranlib.c rc.sh reboot.c remind.p renice.c\
	reset.c restor.c rev.c rewind.c rm.c rmail.c rmdir.c sa.c savecore.c\
	see.sh setroot.csh shutdown.c size.c sleep.c soelim.c sort.c\
	split.c stat.c strings.c strip.c stty.c su.c su1.c sum.c sync.c\
	tabs.c tail.c tar.c tcerr.c tee.c test.c time.c touch.c\
	trman.sh tr.c true.sh tsort.c tty.c\
	uid.c ul.c umount.c unexpand.c uniq.c units.d units.c update.c users.c\
	uudecode.c uuencode.c uusend.c uvers.c\
	vipw.sh vmstat.c w.c wall.c wc.c\
	what.c whatis.c whereis.c which.sh who.c whoami.c\
	write.c yes.c

# List of image files by destination directory.

BINFILES = ar.x cat.x chmod.x cmp.x cp.x date.x dd.x df.x du.x\
	echo.x expr.x hostname.x\
	kill.x ln.x login.x mail.x mkdir.x mt.x mv.x nice.x nm.x od.x\
	passwd.x pr.x ps.x pwd.x rm.x rmdir.x rmail.x\
	size.x strip.x stty.x su.x sync.x\
	tar.x tee.x test.x time.x wall.x who.x write.x
ETCFILES = ac.x accton.x badsect.x catman.x chgrp.x chown.x clri.x\
	cron.x dcheck.x dmesg.x dumpdir.x\
	fsck.x getty.x halt.x icheck.x init.x\
	mkfs.x mknod.x mount.x ncheck.x\
	quot.x reboot.x renice.x restor.x sa.x savecore.x\
 	shutdown.x umount.x update.x
USRBINFILES = at.x basename.x bc.x\
	cal.x cb.x checkeq.x col.x comm.x crypt.x cu.x\
	egrep.x fgrep.x file.x find.x\
	look.x mesg.x newgrp.x prof.x ptx.x\
	ranlib.x rev.x sleep.x sort.x split.x sum.x\
	tabs.x tail.x touch.x tr.x tsort.x tty.x uniq.x units.x wc.x
USRUCBFILES = checknr.x chfn.x chsh.x\
	clear.x colcrt.x colrm.x ctags.x\
	expand.x finger.x fold.x from.x gets.x head.x\
	last.x lastcomm.x leave.x lock.x mkstr.x more.x num.x\
	printenv.x prmail.x reset.x rewind.x soelim.x strings.x\
	ul.x unexpand.x users.x uudecode.x uuencode.x uusend.x\
	vmstat.x w.x what.x whatis.x whereis.x whoami.x yes.x
USRLIBFILES = atrun.x getNAME.x makekey.x
USRNSCFILES = burn.x csym.x cu16.x dcerr.x monfix.x\
	mmerr.x nburn.x remind.x stat.x su1.x tcerr.x uid.x uvers.x

FILES = ${BINFILES} ${ETCFILES} ${USRBINFILES} ${USRUCBFILES} ${USRLIBFILES}\
	${USRNSCFILES}

# List of files that are here, but either don't build or are known to fail.
# Need to be fixed sometime.

NOBUILDFILES =

# List of files that need special owners (usually root) or protections.

BINROOT		= df login mkdir mv passwd rmdir su
USRBINROOT	= at newgrp
USRUCBROOT	= chfn chsh w
USRLIBROOT	= atrun
TMPROOT		= tmp usr/tmp usr/msgs


# Language directories to do recursive make.

LSUB	= as c2 cpp ld pcc

# List of directories to do recursive make commands in.

SUBDIRS	= \
	awk calendar compact csh dc ddt delivermail diction diff diff3 dpy \
	dump ed eqn ex grep lex lint lpr ls m4 make man msgs neqn pstat \
	sccs sed sh spell termcap tbl troff tset ucbmail uucp xsend yacc cref

all: ${FILES} lang
	for i in ${SUBDIRS}; do cd $$i; echo MAKING $$i; make -k ${ARGS}; cd ..; done


install: ${FILES} inslang
	minstall -s -x ${BINFILES} false.sh nohup.sh true.sh ${BIN}
	minstall -s -x ${ETCFILES} mkl+s.sh newsym.csh rc.sh\
	setroot.csh vipw.sh ${ETC}
	minstall -s -x ${USRBINFILES} minstall.sh ${USRBIN}
	minstall -s -x ${USRUCBFILES} apropos.sh help.sh print.sh\
	see.sh trman.sh which.sh ${USRUCB}
	minstall -s -x ${USRLIBFILES} makewhatis.sh units.d ${USRLIB}
	minstall -s -x ${USRNSCFILES} ${USRNSC}
	rm -f ${ETC}/wall
	ln ${BIN}/wall ${ETC}/wall
	rm -f "${BIN}/["
	ln ${BIN}/test "${BIN}/["
	rm -f ${USRUCB}/page
	ln ${USRUCB}/more ${USRUCB}/page
	rm -f ${USRUCB}/uptime
	ln ${USRUCB}/w ${USRUCB}/uptime
	rm -f ${USRUCB}/f
	ln ${USRUCB}/finger ${USRUCB}/f
	rm -f ${USRUCB}/u
	ln ${USRUCB}/users ${USRUCB}/u
	mv ${ETC}/mkl+s ${ETC}/mklost+found
	rm -f ${USRLIB}/whatis
#	mmakewhatis
	for i in ${SUBDIRS}; do cd $$i; make ${ARGS} install; cd ..; done

lang:
	for i in ${LSUB}; do cd $$i; echo MAKING $$i; make -k ${ARGS}; cd ..; done

inslang:
	for i in ${LSUB}; do cd $$i; make ${ARGS} install; cd ..; done

cleanlang:
	for i in ${LSUB}; do cd $$i; make clean; cd ..; done

clean:
	rm -f ${FILES} *.o
	for i in ${SUBDIRS} ${LSUB}; do cd $$i; make clean; cd ..; done

clobber:
	rm -f ${FILES} *.o
	for i in ${SUBDIRS} ${LSUB}; do cd $$i; make clobber; cd ..; done
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

# `make modes' must be run as user root after `make install'.  It sets
# the appropriate owners and file protections for utilities that must be
# run as set-user-id programs and such.

modes:
	cd ${DESTDIR}/bin; ${CHOWN} root ${BINROOT}; chmod 4755 ${BINROOT}
	cd ${DESTDIR}/usr/bin; ${CHOWN} root ${USRBINROOT}; chmod 4755 ${USRBINROOT}
	cd ${DESTDIR}/usr/ucb; ${CHOWN} root ${USRUCBROOT}; chmod 4755 ${USRUCBROOT}
	cd ${DESTDIR}/usr/lib; ${CHOWN} root ${USRLIBROOT}; chmod 4755 ${USRLIBROOT}
	cd ${DESTDIR}; chmod 777 ${TMPROOT}

try: ${NOBUILDFILES}

clear.x: clear.c
	rm -f $*.x
	${CC} ${CFLAGS} -o $*.x $*.c -ltermlib
	chmod 770 $*.x

# Build finger without optimizer until bugs are fixed.
finger.x: finger.c
	rm -f $*.x
	${CC} -DSCCS -o $*.x $*.c
	chmod 770 $*.x

init.x:	init.c
	rm -f $*.x
	${CC} ${CFLAGS} -DFIX -o $*.x $*.c
	chmod 770 $*.x

more.x: more.c
	rm -f $*.x
	${CC} ${CFLAGS} -o $*.x $*.c -ltermlib
	chmod 770 $*.x

ul.x: ul.c
	rm -f $*.x
	${CC} ${CFLAGS} -o $*.x $*.c -ltermlib
	chmod 770 $*.x

vmstat.x: ${INC}/stdio.h ${INC}/sys/param.h   ${INC}/sys/dk.h
vmstat.x: ${INC}/nlist.h ${INC}/sys/vmmeter.h ${INC}/sys/vmsystm.h

# Handy dandy building rules.

.o.x:
	rm -f $@
	${LD} -o $@ $*.o -lc
	chmod 770 $@

.c.x:	$*.c
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@

.y.x:
	rm -f $*.x
	yacc $*.y
	${CC} ${CFLAGS} -o $*.x y.tab.c -lln
	rm -f y.tab.[cO]
	chmod 770 $*.x

.c.o:
	${CC} ${CFLAGS} -c $*.c

.s.o:
	${AS} ${AFLAGS} -o $*.o $*.s

.p.x:
	rm -f $*.x
	${PC} -o $*.x $*.p
	rm -f $*.o
	rm -f $*.s
	chmod 770 $*.x
