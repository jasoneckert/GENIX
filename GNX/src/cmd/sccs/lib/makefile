#
# %M%: version %I% of %G%
# %Y%
#
# %W% (NSC) %G%
#

#
# Recreate the library of common routines for the sccs commands.
#

SCCS	= /v/mesaux/src/cmd/sccs/lib

CC	= nmcc
LD	= nmeld
RANLIB	= nranlib
GET	= sccs -d${SCCS} get

CCFLAGS	= -v -O
LDFLAGS	= -x

CFILES	= auxf.c chkid.c chksid.c date_ab.c date_ba.c del_ab.c del_ba.c	\
	  dodelt.c dofile.c dohist.c doie.c dolist.c eqsid.c flushto.c	\
	  fmterr.c getline.c logname.c newstats.c permiss.c pf_ab.c	\
	  putline.c rdmod.c setup.c sid_ab.c sid_ba.c sidtoser.c	\
	  sinit.c stats_ab.c

OFILES	= auxf.o chkid.o chksid.o date_ab.o date_ba.o del_ab.o del_ba.o	\
	  dodelt.o dofile.o dohist.o doie.o dolist.o eqsid.o flushto.o	\
	  fmterr.o getline.o logname.o newstats.o permiss.o pf_ab.o	\
	  putline.o rdmod.o setup.o sid_ab.o sid_ba.o sidtoser.o	\
	  sinit.o stats_ab.o

comobj.a: ${OFILES}
	rm -f comobj.a
	ar r comobj.a ${OFILES}
	${RANLIB} comobj.a

all sources: ${CFILES}
${CFILES}:
	${GET} $@

clean clobber:
	rm -f ${OFILES} comobj.a
