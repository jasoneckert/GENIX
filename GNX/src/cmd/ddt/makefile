#
# makefile: version 1.12 of 9/16/83
# 
#
# @(#)makefile	1.12 (NSC) 9/16/83
#

# makes done by this makefile:
#
#	mddt	latest version of ddt which runs on 16k
#	ddt	latest version of ddt which runs on VAX
#	installv replaces ddt version in /usr/nsc
#	install replaces ddt version in /v/mesaux/usr/nsc
#	clean	deletes files from this directory
#	sources sccs get sources
#	print	prints files from this directory
#	depend	recreate ddt dependencies

.SUFFIXES: .O .o .c .s

SCCS 	= /v/mesaux/src/cmd/ddt
RELDIR 	= /usr/nsc
MRELDIR = /v/mesaux/usr/nsc
INCL 	= /v/mesaux/usr/include 

SCCSD	= sccs -d${SCCS}

CC 	= nmcc
AS 	= nasm
LCRT	= /mesa/lib/crt0.o
LD 	= nmeld
CEFLAGS = 
CFLAGS 	= -g -DSCCS ${CEFLAGS}
AEFLAGS =
AFLAGS 	= ${AEFLAGS}

CFILES 	= main.c display.c parse.c symbol.c scan.c mem.c reg.c\
	process.c bpt.c stops.c call.c machine.c serial.c gpib.c

HFILES 	= main.h error.h scan.h parse.h symbol.h display.h bpt.h machine.h

ofiles 	= main.o display.o symbol.o scan.o parse.o mem.o\
	reg.o process.o bpt.o stops.o call.o machine.o serial.o gpib.o

OFILES	= main.O display.O symbol.O scan.O parse.O mem.O\
	reg.O process.O bpt.O stops.O call.O machine.O serial.O gpib.O

SFILES 	= main.S display.S parse.S symbol.S scan.S mem.S reg.S\
	process.S bpt.S stops.S call.S machine.S serial.S gpib.S

FILES	= *.[oOS] core asm.code {a,a16}.out

#  mddt	run on 16K

mddt mddt.x: ${HFILES} ${OFILES}
	rm -f mddt.x
	${LD} ${LCRT} -o mddt.x ${OFILES} -lc
	chmod 770 mddt.x

# version	flag	usage
#  ddt		VAX	run on VAX

allc ddt ddt.x:	${CFILES} ${HFILES}
	rm -f ddt.x
	make 'CC = cc' 'AS = as' 'LD = ld' 'CEFLAGS = -DVAX -I${INCL} ${CEFLAGS}' 'DDT = ddt.x' xddt
	chmod 770 ddt.x

xddt:	${ofiles}
	${CC} -o ${DDT} ${ofiles}

# /usr/nsc is where the ddt releases are
#
#	ddt	currently usable ddt
#	vdt	experimental version
#

install: mddt.x
	minstall mddt.x ${MRELDIR}/ddt

installc installv: ddt.x
	minstall -x ddt.x ${RELDIR}/ddt

installx: ddt.x
	minstall ddt.x ${RELDIR}/vdt

clean:
	rm -f ${FILES} *.x

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${HFILES} ${CFILES}
${HFILES} ${CFILES}:
	${SCCSD} get $@

print:
	pr ${HFILES} ${CFILES} | lpr

# building rules

.c.O: 
	${CC} -b -c ${CFLAGS} $*.c

.c.o:
	${CC} -c ${CFLAGS} $*.c

depend:
	grep '^#include' ${CFILES} \
		| sed 's/:[^"]*"\([^"]*\)".*/: \1/' \
		| sed 's/\.c/.o/' >makedep
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp makefile makefile.bak
	ed - makefile < eddep
	rm eddep makedep makefile.bak
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> makefile
	echo '# see make depend above' >> makefile

# DO NOT DELETE THIS LINE -- make depend uses it

main.o:#include <setjmp.h>
main.o:#include <signal.h>
main.o:#include <sgtty.h>
main.o:#include <stdio.h>
main.o: main.h
main.o: error.h
main.o: scan.h
main.o: parse.h
main.o: display.h
main.o: symbol.h
main.o: machine.h
main.o: bpt.h
display.o:#include <ctype.h>
display.o:#include <meld3.h>;
display.o:#include <sys/param.h>;
display.o: main.h
display.o: symbol.h
display.o: parse.h
display.o: display.h
parse.o:#include <setjmp.h>
parse.o:#include <ctype.h>
parse.o: main.h
parse.o: error.h
parse.o: parse.h
parse.o: scan.h
parse.o: symbol.h
symbol.o:#include <stdio.h>
symbol.o:#include <meld3.h>
symbol.o:#include <a.out.h>
symbol.o:#include <stab.h>
symbol.o: main.h
scan.o:#include <setjmp.h>		/* read setjmp definitions */
scan.o:#include <sgtty.h>		/* read stty definitions */
scan.o: scan.h
scan.o: error.h
mem.o:#include <setjmp.h>
mem.o:#include <meld3.h>
mem.o:#include <stdio.h>
mem.o: symbol.h
mem.o:#include <sys/param.h>
mem.o:#include <sys/systm.h>
mem.o:#include <sys/dir.h>
mem.o:#include <sys/user.h>
mem.o:#include <sys/reg.h>
mem.o: main.h
mem.o: parse.h
mem.o: display.h
mem.o: error.h
mem.o: machine.h
reg.o:#include <setjmp.h>
reg.o:#include <sys/param.h>
reg.o:#include <sys/systm.h>
reg.o:#include <sys/dir.h>
reg.o:#include <sys/user.h>
reg.o:#include <sys/reg.h>
reg.o: main.h
reg.o: parse.h
reg.o: display.h
reg.o: error.h
reg.o: machine.h
process.o:#include <setjmp.h>
process.o:#include <stdio.h>
process.o:#include <signal.h>
process.o:#include <sys/reg.h>
process.o: main.h
process.o: symbol.h
process.o: display.h
process.o: parse.h
process.o: error.h
process.o: scan.h
process.o: bpt.h
process.o: machine.h
bpt.o:#include <setjmp.h>
bpt.o:#include <stdio.h>
bpt.o: main.h
bpt.o: symbol.h
bpt.o: display.h
bpt.o: error.h
bpt.o: bpt.h
bpt.o: parse.h
bpt.o: machine.h
stops.o:#include <setjmp.h>
stops.o:#include <stdio.h>
stops.o: main.h
stops.o: symbol.h
stops.o: scan.h
stops.o: display.h
stops.o: error.h
stops.o: bpt.h
stops.o: parse.h
stops.o: machine.h
call.o:#include <ctype.h>
call.o:#include <signal.h>
call.o: main.h
call.o: symbol.h
call.o: parse.h
call.o: display.h
call.o: bpt.h
call.o: machine.h
machine.o:#include <setjmp.h>
machine.o:#include <sgtty.h>
machine.o:#include <stdio.h>
machine.o:#include <signal.h>
machine.o: main.h
machine.o: symbol.h
machine.o: display.h
machine.o: parse.h
machine.o: error.h
machine.o: scan.h
machine.o: bpt.h
machine.o: machine.h
serial.o:#include <stdio.h>
serial.o:#include <meld3.h>
serial.o:#include <a.out.h>
serial.o:#include <stab.h>
serial.o:#include <setjmp.h>
serial.o:#include <sgtty.h>
serial.o:#include <signal.h>
serial.o: main.h
serial.o: parse.h
serial.o: machine.h
gpib.o:#include	<stdio.h>
gpib.o:#include	<sys/param.h>
gpib.o:#include	<sys/stat.h>
gpib.o:#include	<sys/dir.h>
gpib.o: main.h
gpib.o: machine.h
gpib.o:#include	<meld3.h>
gpib.o: symbol.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
