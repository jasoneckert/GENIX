# @(#)make2	1.1	9/23/83
# @(#)Copyright (C) 1983 by National Semiconductor Corp.

.SUFFIXES: .o .V .p .s

AS        = as
LD        = ld
PC        = pc
base      = 0

AEFLAGS   =
AFLAGS    = ${AEFLAGS}

OBJS      = prf.o gets.o pio.o sio.o rom.o

all: ${OBJS} dbmon.x

dbmon.x: newvers.sh rom.V rom.o 
	    ${LD} -p -1 -e start -T ${base} -o dbmon.x rom.o
	    monfix dbmon.x
	    size dbmon.x

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
