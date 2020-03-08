#
# %M%: version %I% of %G%
# %Y%
#
# %W% (NSC) %G%
#
#  JZ 12/3/82:  macro INCLUDE added which is defined to the string for
#               the include path needed.
#  JZ  2/15/83: Changed to conform with new Mesa makefile conventions as
#   		specified below.  With a prefix of inst it will install each of
#		those as in installn installc etc.  Also added sccs stuff and
#		:rofix rule, although it does nothing now because of text 
#		swapping bugs.
#		Also changed so that it MCC is the name of nmcc and cpp16 is
#		the name of the file produced and the actual case.
#  modified to produce cpp for four environments
#  1. alln  cpp: Mesa vax software with include directory ~mesaux/include and 
#     temp directory /tmp
#  2. all cpp16: Mesa 16k runs on our work station with include directory
#     /usr/include and temp directory /usr/tmp.
#  3. allk cpp : Kernel group vax software. This is exactly the same as 1 above.
#  4. allc ccpp: Customer vax software.  This runs on the vax with include
#		 directory /usr/NSC/include.
#
# 2/17/83  The makefile is currently wrong in the way it passes CEFLAGS and
#	   CVEFLAGS.  CVEFLAGS are to only be passed to the vax and CEFLAGS
#	   are only to be passed to the 16K.  Presently it is correct for ccpp
#	   and all or in other words for the 16k and for customer it is correct
#	   otherwise not.
#JZ 3/7/83: Changed to use include path /v/mesaux/usr/include and to have
#           both testinstallk and finalinstallk and so all the calls work 
#	    correctly
#JZ 6/8/83: Changed CFLAGS to undefine vax so that preprocessor will not define
#	    vax.

#  The following is a cross software library for the includes for the vax
#  Although it is searched for all includes, that should not hurt 16k files as
#  it is the same as files that would be included otherwise from
#  /v/mesaux/usr/include.



DESTDIR = /mesa
RELDIR	= ${DESTDIR}/lib

INCL	= /v/mesaux/src/NSC/include
INCLUDEPATH = \"/v/mesaux/usr/include\"

SCCS	= /v/mesaux/src/cmd/cpp
SCCSD	= sccs -d${SCCS}

CC	= nmcc
CCV	= cc

INSTALL	= minstall

PATHFLAGS = -DINCLUDE=${INCLUDEPATH}
CFLAGS	= -O -Dunix=1 -q -Uvax -Dns16000=1 -DFLEXNAMES ${PATHFLAGS} ${CEFLAGS} -I${INCL}
NAME	= cpp
ARGS	= CC="${CC}" NAME=cpp16 INCLUDEPATH=\\\"/usr/include\\\" \
	INCL="${INCL}" CEFLAGS="${CEFLAGS}"

#ARGVS	= CC="${CC}" NAME=ccpp INCLUDEPATH=\\\"/usr/NSC/include\\\" \
	INCL="${INCL}" CEFLAGS="${CVEFLAGS}" 

#16k vax software to run on the 16000 for kernel or anyone 

SRCS	= cpp.c cpy.y yylex.c
OBJS	= cpp.o cpy.o rodata.o

all:
	make ${ARGS} cpp16



install: all
	${INSTALL} cpp16 ${RELDIR}/cpp

clean :
	rm -f *.s *.o cpy.c ccpp cpp cpp16 rodata.c colonfiles

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS} colonfiles
${SRCS}:
	${SCCSD} get $@

# It will get :yyfix and :rofix by default since a bug in make prevents
# files starting with a ':' from being in dependancies.

colonfiles:
#	rm -f :yyfix :rofix
#	${SCCSD} get :yyfix :rofix
	chmod 755 :yyfix :rofix	
	touch colonfiles

#customer vax software

cpp16: ${OBJS}
	${CC} ${CFLAGS} -o ${NAME} ${OBJS}

cpy.c : cpy.y colonfiles
	yacc cpy.y
	:yyfix yyexca yyact yypact yypgo yyr1 yyr2 yychk yydef
	mv y.tab.c cpy.c

cpy.o :	cpy.c yylex.c


rodata.s: cpy.c colonfiles
	${CC} ${CFLAGS} -S rodata.c
	./:rofix rodata.s

rodata.o: rodata.s 
	${CC} ${CFLAGS}  -c rodata.s

.c.o:
	${CC} -c ${CFLAGS}  $*.c 
