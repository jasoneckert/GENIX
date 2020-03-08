#
# makefile: version 1.12 of 9/26/83
# Unix System Command Source File
#
# @(#)makefile	1.12 (NSC) 9/26/83
#

.SUFFIXES: .c .o .S .x

SCCS	= /v/mesaux/src/cmd/sh
DESTDIR	= /mesa
SCCSD	= sccs -d${SCCS}

CC	= nmcc
LCRT	= /mesa/lib/crt0.o
LD	= nmeld
INSTALL = minstall

CEFLAGS =
CFLAGS	= -O -DFIX -DSCCS ${CEFLAGS}

OFILES	= setbrk.o builtin.o blok.o stak.o cmd.o fault.o main.o word.o	\
	string.o name.o args.o xec.o service.o error.o io.o print.o	\
	macro.o expand.o ctype.o msg.o

CFILES	= args.c blok.c builtin.c cmd.c ctype.c error.c expand.c	\
	fault.c io.c macro.c main.c msg.c name.c print.c service.c	\
	setbrk.c stak.c string.c word.c xec.c

HFILES	= brkincr.h ctype.h defs.h dup.h mac.h mode.h name.h stak.h	\
	sym.h timeout.h

sh.x:	${HFILES} ${OFILES}
	rm -f sh.x
	${LD} ${LCRT} -o sh.x ${OFILES} -lc
	chmod 770 sh.x

install:
	${INSTALL} -s -x sh.x ${DESTDIR}/bin

clean:
	rm -f sh.x ${OFILES} makefile.bak

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${CFILES} ${HFILES} ${depend}
${CFILES} ${HFILES}:
	${SCCSD} get $@

# We assume there are no files involved that begin with `..' except for
#	`..' itself.
#
#	The rule works by sequentially producing the dependencies for each
#	source file.  It builds each such list by using the C preprocessor to
#	collect all files required by the current source file.  Grep then
#	collects line directive lines, and awk and sed strip out unwanted
#	fields and punctuation.  The final awk formats the dependencies into
#	a compact form.
depend:
	rm -f makedep eddep
	for i in ${CFILES};					do	\
		${CC} -E ${CFLAGS} $$i				|	\
		grep '^#'					|	\
		awk '{ print $$3 }'				|	\
		sed -e 's|/[^/]*/\.\.||g' -e 's|"||g'		|	\
		sort -u						|	\
		awk "BEGIN { of = \"`basename $$i .c`\" \".o: \"; rec = of }\
		     { if (length(rec \$$0) > 78) { print rec; rec = of \$$0 }\
		      else rec = rec \" \" \$$0 }			\
		     END { print rec }" >> makedep		;	\
	done
	echo '$$r makedep' >>eddep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >>eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp makefile makefile.bak
	ed - makefile < eddep
	rm eddep makedep

# Handy dandy building rules.

.c.o:	$*.c
	${CC} ${CFLAGS} -c $*.c

.o.x:	$*.o ${dep}
	rm -f $@
	${LD} ${LCRT} -o $@ $*.o -lc
	chmod 770 $@

.c.x:	$*.c ${dep}
	rm -f $@
	${CC} ${CFLAGS} -o $@ $*.c
	chmod 770 $@

# DO NOT DELETE THIS LINE -- make depend uses it
args.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
args.o: /v/mesaux/usr/include/setjmp.h args.c
blok.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
blok.o: /v/mesaux/usr/include/setjmp.h blok.c
builtin.o:  builtin.c
cmd.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
cmd.o: ./sym.h /v/mesaux/usr/include/setjmp.h cmd.c
ctype.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
ctype.o: /v/mesaux/usr/include/setjmp.h ctype.c
error.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
error.o: /v/mesaux/usr/include/setjmp.h error.c
expand.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
expand.o: /v/mesaux/usr/include/setjmp.h /v/mesaux/usr/include/sys/dir.h
expand.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/types.h
expand.o: expand.c
fault.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
fault.o: /v/mesaux/usr/include/setjmp.h fault.c
io.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
io.o: /v/mesaux/usr/include/setjmp.h io.c
macro.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
macro.o: ./sym.h /v/mesaux/usr/include/setjmp.h macro.c
main.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
main.o: ./sym.h ./timeout.h /v/mesaux/usr/include/setjmp.h
main.o: /v/mesaux/usr/include/sgtty.h /v/mesaux/usr/include/sys/ioctl.h
main.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/types.h
main.o: main.c
msg.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
msg.o: ./sym.h /v/mesaux/usr/include/setjmp.h msg.c
name.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
name.o: /v/mesaux/usr/include/setjmp.h name.c
print.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
print.o: /v/mesaux/usr/include/setjmp.h print.c
service.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
service.o: /v/mesaux/usr/include/setjmp.h service.c
setbrk.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
setbrk.o: /v/mesaux/usr/include/setjmp.h setbrk.c
stak.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
stak.o: /v/mesaux/usr/include/setjmp.h stak.c
string.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
string.o: /v/mesaux/usr/include/setjmp.h string.c
word.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
word.o: ./sym.h /v/mesaux/usr/include/setjmp.h word.c
xec.o:  ./brkincr.h ./ctype.h ./defs.h ./mac.h ./mode.h ./name.h ./stak.h
xec.o: ./sym.h /v/mesaux/usr/include/setjmp.h xec.c
