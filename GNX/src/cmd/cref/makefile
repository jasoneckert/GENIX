#
# makefile: version 1.5 of 9/28/83
# 
#
# @(#)makefile	1.5 (NSC) 9/28/83
#

LN = ln
CC = nmcc
CCV =cc
SRC=.
SRCS=acts.c asym atable c.c ccmn.h crpost.c csym ctable dr.c econs.h esym \
	etable mcons.h mign.c mtab.c put.c upost.c
FRC=
SCCS=/v/mesaux/src/cmd/cref
SCCSD= sccs -d${SCCS}
INSTALL=minstall
CFLAGS=-O ${CEFLAGS} -DSCCS
LDFLAGS=-s
TESTDIR =.
DESTDIR=/v/mesaux/usr
RELDIR=$(DESTDIR)/bin
all:	cref crpost upost atab ctab etab aign cign eign
${SRCS}:
	${SCCSD} get $@
cref::	dr.o put.o acts.o $(FRC)
	$(CC) $(LDFLAGS) dr.o put.o acts.o -o ./cref
install: cref
	$(INSTALL)  $(TESTDIR)/cref $(RELDIR)/cref
	$(INSTALL)  $(TESTDIR)/crpost $(DESTDIR)/lib/cref/crpost
	$(INSTALL)  $(TESTDIR)/upost $(DESTDIR)/lib/cref/upost
	$(INSTALL)  $(TESTDIR)/atab $(DESTDIR)/lib/cref/atab
	$(INSTALL)  $(TESTDIR)/ctab  $(DESTDIR)/lib/cref/ctab 
	$(INSTALL)  $(TESTDIR)/etab  $(DESTDIR)/lib/cref/etab
	$(INSTALL)  $(TESTDIR)/aign  $(DESTDIR)/lib/cref/aign
	$(INSTALL)  $(TESTDIR)/cign  $(DESTDIR)/lib/cref/cign
	$(INSTALL)  $(TESTDIR)/eign  $(DESTDIR)/lib/cref/eign
#	@set +e; if test $(LN) = ln ; then
	rm -f $(DESTDIR)/lib/eign 
	$(LN) $(DESTDIR)/lib/cref/eign $(DESTDIR)/lib/eign

dr.o: ccmn.h mcons.h
acts.o: ccmn.h mcons.h

crpost::	crpost.c $(FRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/crpost crpost.c

upost::	upost.c $(FRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/upost upost.c
mtab: mtab.c econs.h
	$(CCV) mtab.c -o mtab
atab:: mtab atable
	./mtab atable $(TESTDIR)/atab
ctab:: mtab ctable
	./mtab ctable $(TESTDIR)/ctab
etab:: mtab etable
	./mtab etable $(TESTDIR)/etab
mign: mign.c
	$(CCV) mign.c -o mign
aign:: mign asym
	./mign asym $(TESTDIR)/aign
cign:: mign csym
	./mign csym $(TESTDIR)/cign
eign:: esym
	cp esym $(TESTDIR)/eign

#install:  all
#	make -f cref.mk all INSTALL=/etc/install LN=ln INSDIR=$(INSDIR) TESTDIR=$(TESTDIR)

clean:
	rm -f *.o
	rm -f mtab mign

clobber: clean
	cd $(TESTDIR); rm -f cref crpost upost *ign *tab
	rm -f *ign *tab
	${SCCSD} clean
	${SCCSD} get makefile
