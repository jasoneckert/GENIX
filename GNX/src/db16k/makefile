#
# makefile: version 1.13 of 9/22/83
# 
#
# @(#)makefile	1.13 (NSC) 9/22/83
#

.SUFFIXES : .o  .V .p .s

DESTDIR	= /v/mesaux
RELDIR	= /usr/src

SCCS      = /v/mesaux/src/db16k
SCCSD     = sccs -d${SCCS}

CC        = nmcc
AS        = nasm
LD        = nmeld
PC        = nmpc
INSTALL   = minstall
base      = 0

AEFLAGS   =
AFLAGS    = ${AEFLAGS}
CEFLAGS   =
CFLAGS    = -O -DSCCS ${CEFLAGS}

SRCS      = pio.p sio.s rom.s newvers.sh
CFILES    = prf.c gets.c
OBJS      = prf.o gets.o pio.o sio.o rom.o
RFILES    = vers.s version rom.V pio.s 
FILES     = dbmon.x ${SRCS} ${OBJS} ${RFILES} README
XFILES    = dbmon.x core ${OBJS} ${RFILES} temp

all: dbmon.x ${OBJS}

dbmon.x: newvers.sh rom.V rom.o 
	    $(LD) -p -1 -e start -T $(base) -o dbmon.x rom.o
	    monfix dbmon.x
	    size dbmon.x

install: ${FILES} make2
	${INSTALL} ${FILES} ${DESTDIR}${RELDIR}/db16k
	${INSTALL} make2 ${DESTDIR}${RELDIR}/db16k/makefile

clean:
	rm -f ${XFILES}

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS} ${CFILES} README make2
${SRCS} ${CFILES} README make2:
	${SCCSD} get $@

rom.V: rom.s
	sh newvers.sh
	cat rom.s > rom.V
	cat vers.s >> rom.V

rom.o: rom.V
	${AS} ${AFLAGS} -o rom.o rom.V
	  
pio.s: pio.p
	rm -f pio.s
	${PC} -S pio.p
	mv pio.s temp
	sed '1,$$s/wrote/write/' < temp > pio.s
	mv pio.s temp
	sed '1,$$s/road/read/' < temp > pio.s

.p.s:
	rm -f $*.s
	${PC} -S $*.p

.s.o: 
	${AS} ${AFLAGS} -o $*.o $*.s

.c.o:
	${CC} ${CFLAGS} -c $*.c
