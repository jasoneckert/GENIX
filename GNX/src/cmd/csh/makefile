#
# makefile: version 3.19 of 9/16/83
# Makefile for csh with job control
#
# @(#)makefile	3.19 (NSC) 9/16/83
#

.SUFFIXES: .o .c .s .x

CEFLAGS =
CFLAGS	= -O -DTELL -DVMUNIX -DVFORK ${CEFLAGS}
XSTR	= /usr/ucb/xstr
ED	= -ed
AS	= -nasm
CC	= nmcc
RM	= -rm
LCRT	= /mesa/lib/crt0.o
LD	= nmeld

CTAGS	= /usr/ucb/ctags
LIBES	= -ljobs -lc
DESTDIR	= /mesa
SCCS	= /v/mesaux/src/cmd/csh
SCCSD	= sccs -d${SCCS}

CFILES	= alloc.c printf.c sh.c sh.dir.c sh.dol.c sh.err.c sh.exec.c \
	sh.exp.c sh.func.c sh.glob.c sh.hist.c sh.init.c sh.lex.c \
	sh.misc.c sh.parse.c sh.print.c sh.proc.c sh.sem.c sh.set.c sh.time.c
HFILES	= sh.dir.h sh.h sh.local.h sh.proc.h

# strings.o must be last in OBJS since it can change when previous files compile
OBJS	= sh.o sh.dol.o sh.err.o sh.exec.o sh.exp.o sh.func.o sh.glob.o \
	sh.hist.o sh.lex.o sh.misc.o sh.parse.o sh.print.o sh.sem.o sh.set.o \
	sh.proc.o sh.dir.o sh.time.o alloc.o sh.init.o printf.o \
	strings.o

csh.x: ${HFILES} ${OBJS}
	rm -f $@
	${LD} ${LCRT} -o $@ ${OBJS} ${LIBES}
	chmod 770 $@

install: csh.x
	minstall -s -x csh.x ${DESTDIR}/bin

clean:
	${RM} -f csh.x ${OBJS} strings x.c xs.c makefile.bak tags
	${RM} -f sh.prof.c

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${CFILES} ${HFILES} rofix.ed

${CFILES} ${HFILES} rofix.ed:
	${SCCSD} get $@

csh.prof: ${OBJS} sh.prof.o sh.local.h mcrt0.o
	rm -f csh.prof
	ld -X mcrt0.o ${OBJS} -o csh.prof ${LIBES} -lc

sh.o.prof:
	cp sh.c sh.prof.c
	cc -c ${CFLAGS} -DPROF sh.prof.c

# strings.o and sh.init.o are specially processed to be shared
strings.o: strings rofix.ed
	${XSTR}
	${CC} ${CFLAGS} -c -S xs.c
	ed - <rofix.ed xs.s
	${AS} -o strings.o xs.s
	rm xs.s

sh.init.o:
	${CC} -E ${CFLAGS} sh.init.c | ${XSTR} -c -
	${CC} ${CFLAGS} -c -R x.c
	mv x.o sh.init.o
	
lint:
	lint ${CFLAGS} sh*.c

tags:	/tmp
	${CTAGS} sh*.c


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
# Special massaging of C files for sharing of strings
.c.o:
	${CC} -E ${CFLAGS} $*.c | ${XSTR} -c -
	${CC} -c ${CFLAGS} x.c 
	mv x.o $*.o

# DO NOT DELETE THIS LINE -- make depend uses it
alloc.o:  ./sh.local.h /v/mesaux/usr/include/pagsiz.h alloc.c
printf.o:  /v/mesaux/usr/include/varargs.h printf.c
sh.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/pwd.h
sh.o: /v/mesaux/usr/include/setjmp.h /v/mesaux/usr/include/signal.h
sh.o: /v/mesaux/usr/include/sys/dir.h /v/mesaux/usr/include/sys/ioctl.h
sh.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h sh.c
sh.dir.o:  ./sh.dir.h ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.dir.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.dir.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.dir.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.dir.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.dir.o: sh.dir.c
sh.dol.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.dol.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.dol.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.dol.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.dol.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.dol.o: sh.dol.c
sh.err.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.err.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.err.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.err.o: /v/mesaux/usr/include/sys/ioctl.h /v/mesaux/usr/include/sys/stat.h
sh.err.o: /v/mesaux/usr/include/sys/times.h /v/mesaux/usr/include/sys/types.h
sh.err.o: /v/mesaux/usr/include/sys/vtimes.h sh.err.c
sh.exec.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.exec.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.exec.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.exec.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.exec.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.exec.o: sh.exec.c
sh.exp.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.exp.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.exp.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.exp.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.exp.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.exp.o: sh.exp.c
sh.func.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.func.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.func.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.func.o: /v/mesaux/usr/include/sys/ioctl.h /v/mesaux/usr/include/sys/stat.h
sh.func.o: /v/mesaux/usr/include/sys/times.h /v/mesaux/usr/include/sys/types.h
sh.func.o: /v/mesaux/usr/include/sys/vlimit.h
sh.func.o: /v/mesaux/usr/include/sys/vtimes.h sh.func.c
sh.glob.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.glob.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.glob.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.glob.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.glob.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.glob.o: sh.glob.c
sh.hist.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.hist.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.hist.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.hist.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.hist.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.hist.o: sh.hist.c
sh.init.o:  ./sh.local.h /v/mesaux/usr/include/pagsiz.h sh.init.c
sh.lex.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.lex.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.lex.o: /v/mesaux/usr/include/sgtty.h /v/mesaux/usr/include/signal.h
sh.lex.o: /v/mesaux/usr/include/sys/dir.h /v/mesaux/usr/include/sys/ioctl.h
sh.lex.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.lex.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.lex.o: sh.lex.c
sh.misc.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.misc.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.misc.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.misc.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.misc.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.misc.o: sh.misc.c
sh.parse.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.parse.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.parse.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.parse.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.parse.o: /v/mesaux/usr/include/sys/types.h
sh.parse.o: /v/mesaux/usr/include/sys/vtimes.h sh.parse.c
sh.print.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.print.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.print.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.print.o: /v/mesaux/usr/include/sys/ioctl.h /v/mesaux/usr/include/sys/stat.h
sh.print.o: /v/mesaux/usr/include/sys/times.h /v/mesaux/usr/include/sys/types.h
sh.print.o: /v/mesaux/usr/include/sys/vtimes.h sh.print.c
sh.proc.o:  ./sh.dir.h ./sh.h ./sh.local.h ./sh.proc.h
sh.proc.o: /v/mesaux/usr/include/errno.h /v/mesaux/usr/include/pagsiz.h
sh.proc.o: /v/mesaux/usr/include/setjmp.h /v/mesaux/usr/include/signal.h
sh.proc.o: /v/mesaux/usr/include/sys/dir.h /v/mesaux/usr/include/sys/ioctl.h
sh.proc.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.proc.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.proc.o: /v/mesaux/usr/include/wait.h sh.proc.c
sh.sem.o:  ./sh.h ./sh.local.h ./sh.proc.h /v/mesaux/usr/include/errno.h
sh.sem.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.sem.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.sem.o: /v/mesaux/usr/include/sys/ioctl.h /v/mesaux/usr/include/sys/stat.h
sh.sem.o: /v/mesaux/usr/include/sys/times.h /v/mesaux/usr/include/sys/types.h
sh.sem.o: /v/mesaux/usr/include/sys/vtimes.h sh.sem.c
sh.set.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.set.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.set.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.set.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.set.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.set.o: sh.set.c
sh.time.o:  ./sh.h ./sh.local.h /v/mesaux/usr/include/errno.h
sh.time.o: /v/mesaux/usr/include/pagsiz.h /v/mesaux/usr/include/setjmp.h
sh.time.o: /v/mesaux/usr/include/signal.h /v/mesaux/usr/include/sys/dir.h
sh.time.o: /v/mesaux/usr/include/sys/stat.h /v/mesaux/usr/include/sys/times.h
sh.time.o: /v/mesaux/usr/include/sys/types.h /v/mesaux/usr/include/sys/vtimes.h
sh.time.o: sh.time.c
