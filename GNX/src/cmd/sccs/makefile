#
# makefile: version 1.7 of 11/8/83
# (System III ported to Mesa) sccs makefile
#
# @(#)makefile	1.7 (NSC) 11/8/83
#

#
# Recreate the sccs subsystem.
#

# Installation directories.
#	In order, they are destinations for:
#		user-visible commands,
#		auxiliary programs, and
#		help files.
#	For the distinction between INSDIR and PGM_INSDIR to be really
#	useful, the way auxiliary programs are found and exec'ed in
#	admin, bdiff, delta, and prs must be revised.  (They currently
#	use execvp, which forces PGM_INSDIR to be part of the user's path.)
#	For now, INSDIR and PGM_INSDIR are assumed (by the install rule)
#	to be identical.
# WARNING:
#	If PGM_INSDIR is changed, sccs.c must be changed accordingly.
#	(The preprocessor doesn't have enough power to feed the definition
#	through, since sccs.c needs it in the form of a parameterized macro.)
# HACK:
#	HLP_INSDIR is the directory in which help files will ultimately be
#	installed; MHLP_INSDIR is where the make install rule should put them.
#	This distinction is required by the existence of distinct heirarchy
#	roots on the vax and 16k.
INSDIR		= /v/mesaux/usr/nsc
PGM_INSDIR	= /v/mesaux/usr/nsc
HLP_INSDIR	= /usr/nsc/lib/help
MHLP_INSDIR	= /v/mesaux/usr/nsc/lib/help

# Directories containing various pieces of the overall source:
SCCS	= /v/mesaux/src/cmd/sccs
SRCDIR	= src
HDRDIR	= hdr
LIBDIR	= lib
HELPDIR	= help.d
SUBDIRS	= ${SRCDIR} ${HDRDIR} ${LIBDIR} ${HELPDIR}

CC	= nmcc
LCRT	= /mesa/lib/crt0.o
LD	= nmeld
INSTALL	= minstall

CEFLAGS =
CFLAGS	= -O -DHELPDIR=\"${HLP_INSDIR}/\" -DHELPDFLT=\"${HLP_INSDIR}/default\" \
	-DSCCS ${CEFLAGS}
FFLAG	=
LDFLAGS	= ${FFLAG}
INFLAGS	=

LIBS	= -lPW -lndir

XFILES	= \
	admin.x bdiff.x comb.x delta.x get.x sccshelp.x prs.x prt.x	\
	rmchg.x sccs.x unget.x val.x what.x

OFILES	= \
	admin.o bdiff.o comb.o delta.o get.o sccshelp.o prs.o prt.o	\
	rmchg.o sccs.o unget.o val.o what.o

SHFILES	= sccsdiff

.SUFFIXES: .o .x

all: sources ${XFILES}

admin.x:	admin.o    ${LIBDIR}/comobj.a
bdiff.x:	bdiff.o    ${LIBDIR}/comobj.a
comb.x:		comb.o     ${LIBDIR}/comobj.a
delta.x:	delta.o    ${LIBDIR}/comobj.a
get.x:		get.o      ${LIBDIR}/comobj.a
sccshelp.x:	sccshelp.o ${LIBDIR}/comobj.a
prs.x:		prs.o      ${LIBDIR}/comobj.a
prt.x:		prt.o      ${LIBDIR}/comobj.a
rmchg.x:	rmchg.o    ${LIBDIR}/comobj.a
sccs.x:		sccs.o
unget.x:	unget.o    ${LIBDIR}/comobj.a
val.x:		val.o      ${LIBDIR}/comobj.a
what.x:		what.o     ${LIBDIR}/comobj.a

admin.o:	${SRCDIR}/admin.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/admin.c

bdiff.o:	${SRCDIR}/bdiff.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/bdiff.c

comb.o:		${SRCDIR}/comb.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/comb.c

delta.o:	${SRCDIR}/delta.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/delta.c

get.o:		${SRCDIR}/get.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/get.c

sccshelp.o:	${SRCDIR}/sccshelp.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/sccshelp.c

prs.o:		${SRCDIR}/prs.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/prs.c

prt.o:		${SRCDIR}/prt.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/prt.c

rmchg.o:	${SRCDIR}/rmchg.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/rmchg.c

sccs.o:		${SRCDIR}/sccs.c
	${CC} -c ${CFLAGS} ${SRCDIR}/sccs.c

unget.o:	${SRCDIR}/unget.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/unget.c

val.o:	${SRCDIR}/val.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/val.c

what.o:		${SRCDIR}/what.c ${HDRDIR}/defines.h ${HDRDIR}/had.h
	${CC} -c ${CFLAGS} ${SRCDIR}/what.c

sccsdiff:	${SRCDIR}/sccsdiff
	cp ${SRCDIR}/sccsdiff .

${LIBDIR}/comobj.a:
	cd lib; make comobj.a

# Regenerate sources as a preliminary to complete rebuilding.
#	This rule isn't as air tight as it should be.  We really
#	should rethink the whole subject.
sources:
	for i in ${SUBDIRS}; do					\
		(cd $$i; make SCCS=${SCCS}/$$i all);		\
	done

# This rule assumes that PGM_INSDIR and INSDIR are identical.  If they
# ever become distinct, it must be rewritten, and should force existence
# of PGM_INSDIR before stuffing things into it.
install: ${XFILES} ${SHFILES} helpfiles
	for i in ${XFILES}; do						\
		${INSTALL} ${INFLAGS} $$i ${INSDIR}/`basename $$i .x`;	\
	done
	rm -f ${INSDIR}/cdc
	ln ${INSDIR}/rmchg ${INSDIR}/cdc
	rm -f ${INSDIR}/rmdel
	ln ${INSDIR}/rmchg ${INSDIR}/rmdel
	rm -f ${INSDIR}/sact
	ln ${INSDIR}/unget ${INSDIR}/sact
	${INSTALL} ${INFLAGS} sccsdiff ${INSDIR}
	cd ${HELPDIR}; make INSDIR=${MHLP_INSDIR} install

helpfiles:
	cd ${HELPDIR}; make all

clean:
	rm -f ${XFILES} ${OFILES} ${SHFILES}
	for i in ${SUBDIRS}; do cd $$i; make clean; cd ..; done

clobber:
	rm -f ${XFILES} ${OFILES} ${SHFILES}
	for i in ${SUBDIRS}; do cd $$i; make clobber; cd ..; done

.o.x:
	${LD} ${LCRT} -o $*.x ${LDFLAGS} $*.o ${LIBDIR}/comobj.a ${LIBS} -lc
