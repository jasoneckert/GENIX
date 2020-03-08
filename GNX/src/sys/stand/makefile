# @(#)makefile	2.30	10/10/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

.SUFFIXES: .o .c .s .x

STAND	= /mesa/stand
USRLIB	= /v/mesaux/usr/lib

CC	= nmcc
AS	= nasm
LD	= nmeld
E2	= nmpc
RANLIB	= nranlib

COPTS	= -I/v/mesaux/usr/include -I.
CFLAGS	= -c -O ${COPTS}
EFLAGS	= -c
AFLAGS	=

LIBRARY = libsa.a

# be sure to reflect any changes to RELOC in bootsrt0.s
# RELOC must not be greater than 64K
RELOC = -T 0e000
STANDFLAG = -DSTANDALONE

# List of all source files in this directory.
# This list is used to get sources out of sccs when necessary.

CMDSRCS = icheck.c mkfs.c restor.c
CMD	= /v/mesaux/src/cmd

LIBSYSSRCS = setjmp.s
LIBSYS	= /v/mesaux/src/libc/sys

LIBGENSRCS = atol.c l3tol.c ltol3.c qsort.c strcpy.c
LIBGEN	= /v/mesaux/src/libc/gen

LIBSRCS = bootsrt0.s conf.c dc.c dcusaio.c dcusize.c ebootsrt0.s fake.c gets.c\
	ie.c ieeio.c iomove.s prf.c rs232.c rtt.s ss.s sys.c\
	tc.c tcusaio.c
LIB	= /v/mesaux/src/sys/stand

MAINSRCS = boot.c cat.c copy.c dcutest.c ls.c mkpt.c
MAIN	=  /v/mesaux/src/sys/stand

SRCS	= ${LIBSRCS} ${MAINSRCS} ${LIBGENSRCS} ${LIBSYSSRCS} \
	${CMDSRCS}

# Object files which are not main programs.
# These make up the standalone library.
LIBOBJS	= atol.o bootsrt0.o conf.o dc.o dcusaio.o dcusize.o \
	fake.o gets.o ie.o ieeio.o iomove.o l3tol.o ltol3.o prf.o rtt.o\
	setjmp.o ss.o strcpy.o sys.o tc.o tcusaio.o qsort.o rs232.o

# List of files by destination directory.

STANDFILES = aboot.x boot.x cat.x copy.x dcutest.x eboot.x icheck.x\
	ls.x mkfs.x mkpt.x restor.x

FILES = ${STANDFILES}

all: ${FILES}

sources: ${SRCS}
${CMDSRCS}:
	sccs -d${CMD} get $@

${LIBSYSSRCS}:
	sccs -d${LIBSYS} get $@

${LIBGENSRCS}:
	sccs -d${LIBGEN} get $@

${LIBSRCS}:
	sccs -d${LIB} get $@

${MAINSRCS}:
	sccs -d${MAIN} get $@

install: ${FILES}
	minstall -s -x ${STANDFILES} ${STAND} 
	cp ${LIBRARY} ${USRLIB}/${LIBRARY}

clean:
	rm -f ${FILES} *.o libsa.a

${LIBRARY}: ${LIBOBJS}
	ar cr ${LIBRARY} ${LIBOBJS}
	${RANLIB} ${LIBRARY}

#Relocate in memory, and always ask for file to boot
aboot.x: aboot.o makefile $(LIBRARY)
	rm -f aboot.x
	$(LD) -p -e _nrentry $(RELOC) -o aboot.x aboot.o $(LIBRARY)
	chmod 770 aboot.x

#Relocate in memory, and default boot file (for two tries)
boot.x: boot.o makefile $(LIBRARY)
	rm -f boot.x
	$(LD) -p -e _nrentry $(RELOC) -o boot.x boot.o $(LIBRARY)
	chmod 770 boot.x

# Eprom Relocate to memory, and pick boot based on startup registers
eboot.x: eboot.o ebootsrt0.o makefile $(LIBRARY)
	rm -f eboot.x
	$(LD) -p -e _nrentry $(RELOC) -o eboot.x eboot.o ebootsrt0.o $(LIBRARY)
	monfix eboot.x
	chmod 770 eboot.x

cat.x:	cat.o makefile $(LIBRARY)
	rm -f cat.x
	$(LD) -e realstart -o cat.x cat.o $(LIBRARY)
	chmod 770 cat.x

copy.x:	copy.o makefile $(LIBRARY)
	rm -f copy.x
	$(LD) -o copy.x -e realstart copy.o $(LIBRARY)
	chmod 770 copy.x

dcutest.x:	dcutest.o makefile $(LIBRARY)
	rm -f dcutest.x
	$(LD) -o dcutest.x -e realstart dcutest.o $(LIBRARY)
	chmod 770 dcutest.x

mkpt.x:	mkpt.o makefile $(LIBRARY)
	rm -f mkpt.x
	$(LD) -e realstart -o mkpt.x mkpt.o $(LIBRARY)
	chmod 770 mkpt.x

memtest.x:	memtest.o makefile $(LIBRARY)
	rm -f memtest.x
	$(LD) -e realstart -o memtest.x memtest.o $(LIBRARY)
	chmod 770 memtest.x

icheck.x:	icheck.o makefile $(LIBRARY)
	rm -f icheck.x
	$(LD) -e realstart -o icheck.x icheck.o $(LIBRARY)
	chmod 770 icheck.x

ls.x:	ls.o makefile $(LIBRARY)
	rm -f ls.x
	$(LD) -e realstart -o ls.x ls.o $(LIBRARY)
	chmod 770 ls.x

mkfs.x:	mkfs.o makefile $(LIBRARY)
	rm -f mkfs.x
	$(LD) -e realstart -o mkfs.x mkfs.o $(LIBRARY)
	chmod 770 mkfs.x

restor.x: restor.o makefile $(LIBRARY)
	rm -f restor.x
	$(LD) -e realstart -o restor.x restor.o $(LIBRARY)
	chmod 770 restor.x

tt.x:	tt.o makefile $(LIBRARY)
	rm -f tt.x
	$(LD) -e realstart -o tt.x tt.o $(LIBRARY)
	chmod 770 tt.x

boot.o: boot.c
aboot.o: boot.c
	ln boot.c aboot.c
	${CC} ${CFLAGS} -DJUSTASK aboot.c
	rm -f aboot.c

eboot.o: boot.c
	ln boot.c eboot.c
	${CC} ${CFLAGS} -DEBOOT eboot.c
	rm -f eboot.c

ebootsrt0.o: ebootsrt0.s
cat.o: cat.c
conf.o: conf.c
copy.o: copy.c
dc.o: dc.c
dcusaio.o: dcusaio.c
dcusize.o: dcusize.c
dcutest.o: dcutest.c
fake.o: fake.c
gets.o: gets.c
icheck.o: icheck.c
	${CC} ${CFLAGS} $(STANDFLAG) icheck.c

ie.o: ie.c
ieeio.o: ieeio.c
	${CC} ${CFLAGS} -w $(STANDFLAG) ieeio.c

ls.o: ls.c
mkfs.o:	mkfs.c
	${CC} ${CFLAGS} $(STANDFLAG) mkfs.c

mkpt.o:	mkpt.c
	${CC} ${CFLAGS} $(STANDFLAG) mkpt.c

restor.o: restor.c
	${CC} ${CFLAGS} -w $(STANDFLAG) restor.c

setjmp.o: setjmp.s
strcpy.o: strcpy.c
sys.o:	sys.c
tc.o: tc.c
tcusaio.o: tcusaio.c
qsort.o: qsort.c
rs232.o: rs232.c

# Handy dandy building rules.
.c.o:
	${CC} ${CFLAGS} $*.c

.s.o:
	${AS} ${AFLAGS} -o $*.o $*.s

.e.o:
	${E2} ${EFLAGS} $*.e
