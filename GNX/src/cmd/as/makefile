# @(#)makefile	3.34     9/9/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp. 

.SUFFIXES:
.SUFFIXES:  .p .s .o

DESTDIR = /mesa
RELDIR 	= /bin
SCCS 	= /v/mesaux/src/cmd/as
SCCSD	= sccs -d${SCCS}

V	= H
SEL 	= n
INSTALL	= minstall

CC 	= nmcc
AS 	= nasm
PC 	= nmpc
E1 	= /e/epascal/neH
LD 	= nmeld

CEFLAGS	=
PEFLAGS =
AEFLAGS =
PFLAGS 	= ${PEFLAGS}
AFLAGS 	= ${AEFLAGS}
CFLAGS	= -O -DSCCS ${CEFLAGS}

XFILES 	= a.out.e cmpc.e codegen.e dirs.e dirsx.e \
     disp.e e1bio.e e1opss.e e2bio.e e2opss.e float.e formatsmod.e \
     genrout.e glob_const.e glob_types.e glob_vars.e init1.e insts.e \
     list_types.e mnem_def.e opss.e \
     prsymtable.e scan.e scanexp.e scanr.e scantoken.e scmpc.e \
     sdis.e stab.e traverse.e

HFILES 	= kasm.h xasm.h lasm.h pasm.h nasm.h as.h casm.h

EFILES 	= asm.p codegen.p dirs.p dirsx.p disp.p dispnot.p dumbio.p float.p \
	genbytes.p \
     genrout.p init1.p insts.p lst.p ops.p prsymtable.p scan.p scanexp.p \
     scanr.p scantoken.p sdis.p traverse.p fopenfile1.p fopenfile2.p 

SFILES 	= asops.s bio.s cmpc.s fnew.s pexit.s ptime.s scmpc.s

CFILES 	= n_asm.c 

SRCS	= ${XFILES} ${HFILES} ${EFILES} ${SFILES} ${CFILES}

cofiles = asops.o init1.o scan.o prsymtable.o scanr.o scanexp.o insts.o \
	dirs.o sdis.o genrout.o codegen.o dirsx.o traverse.o cmpc.o \
	scmpc.o disp.o float.o dumbio.o fopenfile2.o scantoken.o \
	pexit.o asm.o ptime.o genbytes.o fnew.o

lstOfiles = lst.o fopenfile2.o pexit.o

all: asm2 list2 as

install: all
	${INSTALL} -s as ${DESTDIR}${RELDIR}
	${INSTALL} -s asm2 ${DESTDIR}/lib/asm
	${INSTALL} -s list2 ${DESTDIR}/lib/list

clean: 
	rm -f *.o asfile listfile

cleaner: 
	rm -f *.o *.e *.p *.s *.c asfile listfile

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS}
${SRCS}:
	${SCCSD} get $@

asm2:	e2bio.e ${cofiles} 
	${LD}  -m  ${cofiles} -lp -lc
	mv a16.out asm2

list2:	e2bio.e ${lstOfiles}
	${LD} ${lstOfiles} -lp -lc
	mv a16.out list2

as:     n_asm.c as.h
	cp as.h n_asm.h
	chmod +w n_asm.h
	${CC} ${CFLAGS} -I. -o as n_asm.c
	cat n_asm.h

wher:   ${efiles}
	fgrep -n reporterror *.p|expand|sed -e '1,$$s/:.*report/: report/' >wher

.s.o:
	${AS} ${AFLAGS} -q -o $*.o $*.s

.p.o:
	cp e2bio.e bio.e
	chmod +w bio.e
	${PC} -c $*.p

.c.X:
	${CC} ${CFLAGS} -o $*.X $*.c

depend:
	fgrep '#cmdfile' makefile|sed '1,$$s/#cmdfile //'|sed '/fgrep/d' >cmd
	fgrep DEPENDENCY *.p |sed -f cmd >makedep
	sed '/o: bio.e/s/bio.e/e2bio.e/' <makedep >>MAKEDEP
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r MAKEDEP' >>eddep
	echo 'w' >>eddep
	cp makefile makefile.bak
	ed - makefile < eddep
	rm eddep makedep MAKEDEP makefile.bak
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> makefile
	echo '# see make depend above' >> makefile

#cmdfile 1,$s/import//
#cmdfile 1,$s/export//
#cmdfile 1,$s/\.p/.o/
#cmdfile 1,$s/'//g
#cmdfile 1,$s/;.*//

# DO NOT DELETE THIS LINE -- make depend uses it
# DEPENDENCIES MUST END AT END OF FILE
asm.o: glob_const.e
asm.o: formatsmod.e
asm.o: glob_types.e
asm.o: glob_vars.e
asm.o: list_types.e
asm.o: e2bio.e
asm.o: scantoken.e
asm.o: init1.e
asm.o: scan.e
asm.o: scanr.e
asm.o: dirs.e
asm.o: insts.e
asm.o: traverse.e
asm.o: sdis.e
asm.o: genrout.e
asm.o: codegen.e
asm.o: disp.e
codegen.o: glob_const.e
codegen.o: formatsmod.e
codegen.o: glob_types.e
codegen.o: glob_vars.e
codegen.o: scanexp.e
codegen.o: scan.e
codegen.o: sdis.e
codegen.o: genrout.e
codegen.o: e2bio.e
codegen.o: scantoken.e
codegen.o: init1.e
codegen.o: list_types.e
codegen.o: dirs.e
codegen.o: stab.e
codegen.o: cmpc.e
codegen.o: float.e
codegen.o: disp.e
codegen.o: codegen.e
dirs.o: glob_const.e
dirs.o: formatsmod.e
dirs.o: glob_types.e
dirs.o: glob_vars.e
dirs.o: mnem_def.e
dirs.o: scantoken.e
dirs.o: scanr.e
dirs.o: scan.e
dirs.o: genrout.e
dirs.o: sdis.e
dirs.o: prsymtable.e
dirs.o: scanexp.e
dirs.o: codegen.e
dirs.o: e2bio.e
dirs.o: init1.e
dirs.o: dirsx.e
dirs.o: insts.e
dirs.o: cmpc.e
dirs.o: scmpc.e
dirs.o: stab.e
dirs.o: disp.e
dirs.o: float.e
dirs.o: dirs.e
dirsx.o: glob_const.e
disp.o: glob_const.e
disp.o: formatsmod.e
disp.o: glob_types.e
disp.o: glob_vars.e
disp.o: mnem_def.e
disp.o: scantoken.e
disp.o: e2bio.e
disp.o: sdis.e
disp.o: genrout.e
disp.o: disp.e
float.o: glob_const.e
float.o: formatsmod.e
float.o: glob_types.e
float.o: scan.e
float.o: float.e
fopenfile1.o: e2bio.e
fopenfile2.o: e2bio.e
genbytes.o: glob_const.e
genbytes.o: formatsmod.e
genbytes.o: glob_types.e
genbytes.o: glob_vars.e
genbytes.o: scanexp.e
genbytes.o: scan.e
genbytes.o: sdis.e
genbytes.o: genrout.e
genbytes.o: e2bio.e
genbytes.o: scantoken.e
genbytes.o: init1.e
genbytes.o: list_types.e
genbytes.o: dirs.e
genbytes.o: stab.e
genbytes.o: cmpc.e
genbytes.o: float.e
genbytes.o: disp.e
genbytes.o: codegen.e
genrout.o: glob_const.e
genrout.o: formatsmod.e
genrout.o: glob_types.e
genrout.o: glob_vars.e
genrout.o: scantoken.e
genrout.o: dirs.e
genrout.o: sdis.e
genrout.o: disp.e
genrout.o: genrout.e
init1.o: glob_const.e
init1.o: formatsmod.e
init1.o: glob_types.e
init1.o: glob_vars.e
init1.o: mnem_def.e
init1.o: e2bio.e
init1.o: scantoken.e
init1.o: scanexp.e
init1.o: scanr.e
init1.o: dirs.e
init1.o: insts.e
init1.o: scan.e
init1.o: dirsx.e
init1.o: prsymtable.e
init1.o: sdis.e
init1.o: codegen.e
init1.o: genrout.e
init1.o: traverse.e
init1.o: disp.e
init1.o: init1.e
init1.o: a.out.e
init1.o: stab.e
insts.o: glob_const.e
insts.o: formatsmod.e
insts.o: glob_types.e
insts.o: glob_vars.e
insts.o: mnem_def.e
insts.o: scantoken.e
insts.o: prsymtable.e
insts.o: scanr.e
insts.o: scan.e
insts.o: scanexp.e
insts.o: sdis.e
insts.o: genrout.e
insts.o: e2bio.e
insts.o: init1.e
insts.o: cmpc.e
insts.o: float.e
insts.o: disp.e
insts.o: dirs.e
insts.o: insts.e
lst.o: glob_const.e
lst.o: formatsmod.e
lst.o: glob_types.e
lst.o: glob_vars.e
lst.o: a.out.e
lst.o: stab.e
lst.o: list_types.e
lst.o: e2bio.e
ops.o: glob_const.e
ops.o: opss.e
ops.o: formatsmod.e
prsymtable.o: glob_const.e
prsymtable.o: formatsmod.e
prsymtable.o: glob_types.e
prsymtable.o: glob_vars.e
prsymtable.o: scantoken.e
prsymtable.o: scan.e
prsymtable.o: traverse.e
prsymtable.o: scmpc.e
prsymtable.o: stab.e
prsymtable.o: sdis.e
prsymtable.o: disp.e
prsymtable.o: prsymtable.e
scan.o: glob_const.e
scan.o: formatsmod.e
scan.o: glob_types.e
scan.o: glob_vars.e
scan.o: list_types.e
scan.o: scantoken.e
scan.o: e2bio.e
scan.o: init1.e
scan.o: scan.e
scanexp.o: glob_const.e
scanexp.o: formatsmod.e
scanexp.o: glob_types.e
scanexp.o: glob_vars.e
scanexp.o: mnem_def.e
scanexp.o: scantoken.e
scanexp.o: scanr.e
scanexp.o: prsymtable.e
scanexp.o: scan.e
scanexp.o: dirs.e
scanexp.o: sdis.e
scanexp.o: scmpc.e
scanexp.o: stab.e
scanexp.o: disp.e
scanexp.o: scanexp.e
scanexp1.o: glob_const.e
scanexp1.o: formatsmod.e
scanexp1.o: glob_types.e
scanexp1.o: glob_vars.e
scanexp1.o: mnem_def.e
scanexp1.o: scantoken.e
scanexp1.o: scanr.e
scanexp1.o: prsymtable.e
scanexp1.o: scan.e
scanexp1.o: dirs.e
scanexp1.o: sdis.e
scanexp1.o: scmpc.e
scanexp1.o: stab.e
scanexp1.o: disp.e
scanexp1.o: scanexp.e
scanr.o: glob_const.e
scanr.o: formatsmod.e
scanr.o: glob_types.e
scanr.o: glob_vars.e
scanr.o: mnem_def.e
scanr.o: scantoken.e
scanr.o: prsymtable.e
scanr.o: scan.e
scanr.o: scanexp.e
scanr.o: dirs.e
scanr.o: sdis.e
scanr.o: disp.e
scanr.o: scanr.e
scantoken.o: glob_const.e
scantoken.o: formatsmod.e
scantoken.o: glob_types.e
scantoken.o: glob_vars.e
scantoken.o: mnem_def.e
scantoken.o: scantoken.e
scantoken.o: scan.e
scantoken.o: prsymtable.e
scantoken.o: cmpc.e
scantoken.o: sdis.e
scantoken.o: disp.e
scantoken.o: float.e
sdis.o: glob_const.e
sdis.o: formatsmod.e
sdis.o: glob_types.e
sdis.o: glob_vars.e
sdis.o: scantoken.e
sdis.o: scanr.e
sdis.o: scan.e
sdis.o: scanexp.e
sdis.o: genrout.e
sdis.o: traverse.e
sdis.o: sdis.e
sdis.o: disp.e
traverse.o: glob_const.e
traverse.o: formatsmod.e
traverse.o: glob_types.e
traverse.o: glob_vars.e
traverse.o: scantoken.e
traverse.o: prsymtable.e
traverse.o: sdis.e
traverse.o: disp.e
traverse.o: scanexp.e
traverse.o: scan.e
traverse.o: traverse.e
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
