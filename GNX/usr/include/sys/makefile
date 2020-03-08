#
# makefile: version 1.27 of 9/16/83
# Mesa Unix System Makefile
#
# Copyright (c) 1982, 1983 National Semiconductor Corporation
#

#
# Recreate system include files from their corresponding sccs files.
#	Most include files come directly from the kernel source.
#	These are accessed through the KERN and KERNINC macros.
#	The rest are independent of the kernel and are accessed
#	through the STD and STDINC macros.
#

STD	= /v/mesaux/usr/include/sys
STDINC	= dcusaio.h ieeio.h saio.h

KERN	= /v/mesaux/src/sys/h
KERNINC	= \
	acct.h blt.h bootop.h buf.h callout.h clist.h conf.h dcu.h \
	devpm.h devvm.h dir.h disk.h dk.h fblk.h file.h filsys.h fpu.h \
	icu.h ino.h\
	inode.h ioctl.h mem.h mmu.h modtable.h mount.h msgbuf.h mtio.h	\
	panic.h param.h pdma.h proc.h psr.h pte.h reboot.h reg.h rs.h	\
	sio.h stat.h systm.h tcu.h timeb.h timer.h times.h trap.h	\
	tty.h types.h user.h vlimit.h vm.h vmmeter.h vmswap.h vmsystm.h	\
	vmtune.h vtimes.h

all:	${STDINC} ${KERNINC}

${STDINC}:
	sccs -d${STD}  get $@

${KERNINC}:
	sccs -d${KERN} get $@

install:	all

clean:
	rm -f ${STDINC} ${KERNINC}

kernclean:
	rm -f ${KERNINC}
