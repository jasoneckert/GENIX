# @(#)@(#)makefile	1.18 (NSC) 11/9/83

# makes done by this makefile:
# 	c2.x	Cross-compilation for execution on the 16K.
# 	install	replaces /mesa/lib/c2 with c2.x
# 	clean	deletes files from this directory
# 	clobber does an sccs clean and re-gets makefile.  Should be done
#        	before make c2.x to be sure of obtaining the latest source.
# 	depend	recreate c2 dependencies

SCCS 	= /v/mesaux/src/cmd/c2
DESTDIR = /mesa
RELDIR	= ${DESTDIR}/lib

cfiles = c20.c c21.c c22.c c2vers.c

hfiles = c2.h

ofiles = c20.o c21.o c22.o c2vers.o

#Note: The "E" flags (CEFLAGS, etc.) are passed from superior makefiles 
#      during a master build.  The "non-E" flags are actually used in the
#      commands, and consist of the "E" flags plus specific things for 
#      this program.  
CC 	= nmcc
AS 	= nasm
LD 	= nmeld
INSTALL = minstall
CEFLAGS =
CFLAGS 	= -DCOPYCODE -DMULTIVAL -DREGISTER=register -DSCCS -O ${CEFLAGS}

all c2.x: ${ofiles}
	${CC} -o c2.x ${ofiles}
	chmod 770 c2.x

alln allk allc c216.x nc2.x kc2.x cc2.x installn installc installk help:
	@echo 'The "$@" rule is not implemented by this makefile, which only'
	@echo 'cross-builds for 16K execution.  "all" or "c2.x" builds c2.x'
	@echo 'in the current directory, and "install" places it in \
${RELDIR}/c2.'
	@echo 'Note: Cross-development tools are made in /v/mesaux/vsrc/...  .'
	sh -c 'exit 123'

${cfiles}: c2.h

#Special processing for c22 - rofix applied to make read-only sharable tables.
c22.o: c22.s
	${CC} -c ${CFLAGS} c22.s
c22.s: c22.c rofix
	${CC} -S ${CFLAGS} c22.c
	rofix c22.s

install: c2.x
	${INSTALL} c2.x ${RELDIR}/c2

sources: ${cfiles} ${hfiles} rofix

${cfiles} ${hfiles}:
	sccs -d${SCCS} get $@
rofix:
	sccs -d${SCCS} get $@
	chmod +x $@

clean:
	-rm -f ${ofiles} c2.x c22.s

clobber: clean
	sccs -d${SCCS} clean; sccs -d${SCCS} get makefile	

cleaner:
	echo "dangerous - not implemented"

depend: ${cfiles}
	grep '^#[ 	]*include' ${cfiles} /dev/null \
		| sed -e '/</d' -e 's/:[^"]*"\([^"]*\)".*/: \1/' \
		      -e 's/\.c/.o/' >makedep
	echo '/^# DO NOT DELETE THIS LINE -- make depend uses it/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo '1,$$s/\.o/.o/' >>eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp makefile makefile.bak
	ed - makefile < eddep
	rm eddep makedep makefile.bak
	echo '# DEPENDENCIES ARE AUTOMATICALLY PUT AT END OF FILE' >> makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> makefile
	echo '# see make depend above' >> makefile

# DO NOT DELETE THIS LINE -- make depend uses it

c20.o: c2.h
c21.o: c2.h
c22.o: c2.h
c20.o: c2.h
c21.o: c2.h
c22.o: c2.h
# DEPENDENCIES ARE AUTOMATICALLY PUT AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
