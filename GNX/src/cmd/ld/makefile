
.SUFFIXES: .c .s .o 

SCCS = /v/mesaux/src/cmd/ld
SCCSD	= sccs -d${SCCS}
DESTDIR	= /mesa
RELDIR	= /bin

cfiles = meld.c symbols.c libs.c lookup.c alloc.c flsbuf.c

hfiles = ross.h meld2.h lookup.h symbols.h system.h

ofiles = meld.o symbols.o libs.o lookup.o alloc.o flsbuf.o mrw.o

sfile = mrw.s

INCL = /v/mesaux/src/NSC/include

CC = nmcc 
AS = nasm
LD = nmeld
CEFLAGS = 
CFLAGS = -DSCCS -O $(CEFLAGS)
AEFLAGS =
AFLAGS = $(AEFLAGS)

INSTALL	= minstall

FILES	= *.[o] rdwr.a *.ll xxx* yyy* *.od core asm.code {a,a16}.out *.x

all meld16.x: ${hfiles} $(ofiles) $(sfiles)
	$(LD)    -o meld16.x $(ofiles) /mesa/lib/crt0.o -lc

install: meld16.x
	${INSTALL} -s meld16.x ${DESTDIR}$(RELDIR)/ld

.c.o:
	$(CC) -c $(CFLAGS) -DON16K -Dfastio $*.c

.s.o:
	$(AS) -o $*.o $*.s

sources: ${cfiles} ${hfiles}
$(cfiles) $(hfiles):
	$(SCCSD) get $@

clean:
	rm -f $(FILES)

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

cleaner:
	rm -f $(FILES) $(cfiles) $(hfiles)

tkmeld.x: $(cfiles) $(hfiles)
	make ${ARGS} 'CEFLAGS=-DTEST ${VFLAGS} ${CEFLAGS}' MELD=tmeld.x xmeld

tcmeld.x:  $(cfiles) $(hfiles)
	make ${ARGS} 'CEFLAGS=-DTCUST ${VFLAGS} ${CEFLAGS}' MELD=tmeld.x xmeld

testinstallk: tkmeld.x
	${INSTALL} -s tmeld.x $(RELDIR)/tmeld

finalinstallk:  kmeld.x
	${INSTALL} -s kmeld.x $(RELDIR)/kmeld
	rm -f $(RELDIR)/tmeld

testinstallc: tcmeld.x
	${INSTALL} -s tmeld.x $(RELDIR)/tnmeld

finalinstallc: cmeld.x
	${INSTALL} -s cmeld.x $(RELDIR)/nmeld
	rm -f $(RELDIR)/tnmeld

depend:
	grep '^#include' $(cfiles) \
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


meld.o:#include <signal.h>
meld.o: ross.h
meld.o:#include <stdio.h>
meld.o: system.h
meld.o:#include <a.out.h>
meld.o:#include <ar.h>
meld.o:#include <stab.h>
meld.o: meld2.h
meld.o: symbols.h
meld.o: lookup.h
symbols.o:#include <stdio.h>
symbols.o:#include <stab.h>
symbols.o: ross.h
symbols.o: meld2.h
symbols.o: lookup.h
symbols.o: symbols.h
libs.o:#include <stdio.h>
libs.o:#include <sys/types.h>
libs.o:#include <sys/stat.h>
libs.o: ross.h
libs.o: meld2.h
libs.o: lookup.h
libs.o: symbols.h
libs.o:#include <ar.h>
libs.o:#include <a.out.h>
libs.o:#include <ranlib.h>
libs.o:#include <stab.h>
lookup.o: ross.h
lookup.o:#include <stdio.h>
lookup.o: system.h
lookup.o: meld2.h
lookup.o: lookup.h
alloc.o: ross.h
flsbuf.o:#include	<stdio.h>
