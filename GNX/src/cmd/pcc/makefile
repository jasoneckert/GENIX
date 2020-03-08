#
# makefile:

# version 5.1 of 9/26/83
# 
#
# @(#)makefile	5.1 (NSC) 9/26/83

#
#JZ 9/22/82: Changed rule for rodata.o to just use cc -R instead of
#            :rofix since that should do the same thing
#JZ 12/3/82: Added rules for mesa compilation so we can have one makefile
#JZ 12/8/82: Changed makefile and main in code.c to allow the location of
#            the temporary directory to be defined in the makefile.  The
#	     location is the value of the variable TEMPFILE and the 
#	     preprocessor variable TEMPDIR
#            Added nmcc to the makefile and changed the makefile and nmcc
#            so that the path of all the utilities is defined by the
#	     preprocessor name UTILPATH whose value is the makefile variable
#	     utilloc
#JZ 12/13/82:Added -F flag for all 16k compilations.  Took out -DMEMRMWOK which
#            is now automatically defined in macdefs.  Also added -O flag for
#            nmcc and cc.
#JZ 12/14/82:Changed locations of utilities to /usr/bin/ for the 16k. Fixed 
#            some pathnames for the 16k.
#JZ 1/19/83: Added iccom16 for optimised ccom16 and ex16 for experimental
# version
#JZ 2/15/83: Added all,allk,allc,alln,install,installk,installc,installn for 
#	     making and installing ccom and cc for the 16k, the kernel, the
#	     customer and for us respectively.  Also added CEFLAGS and
#	     INCL which are passed down from the upper makefile. They are
#	     installed in RELDIR which is also passed down from above.
#	     Also added sccs stuff.
#JZ 2/16/83: Changed the rules to eliminate unnecessary recursion and to
#	     always pass CC and CCV around so they can be redefined if necessary
#            Also changed rule for rodata.O to use $(CC) instead of $(AS16K)
#	     This is synomonous and makes the compiler more consistent.
#JZ 2/17/83: Changed the rules to force definition of SCCS always, to always 
#	     define MESA, to have CVEFLAGS for the vax CEFLAGS for the 16k, and
#	     fixed various bugs that caused errors in compiling for the 
#	     customer.
#JZ 2/24/83: Added installl, installx, alll and allx.   Moved the definition of
#	     pathnames from being dynamically determined by mcc.c to being 
#            totally decided here.  Added rules for lintall ccom.L and nmcc.L.
#	     Fixed lgout to create an index and to include mcc.c.
#	     Added a number of comments to clarify.  Added delta and edit for
#	     making delta's and sccs editing the files respectively.
#JZ 3/01/83  Added installtk and installtn and tnmcc and tkmcc as rules for
#	     making test releases of the language software.  Added finalinstall
#	     for finally installing these in their final places.  Also added 
#	     above for customer and as alltk etc.  Probably makes allx and
#	     installs for those obsolete.  However we shall see.
#JZ 3/23/83  Fixed recursive makefile rules so that quotes are correct in nmcc
#JZ 5/6/83   Added rules for fpops.c so that it will be built for the X-support
#            package but not for the native mode compiler.  However this will
#	     only work correctly on the vax, when the compiler is compiled on
# 	     the ns16000 it will not be correct since I am just using the
#	     small o convention to get it right.
#JZ 7/5/83   Changed the default to lower case .o and .s always.  Changed
#	     making of the compiler to nmcc -b
#JZ 7/22/83  Added -q flag to have compiler made with -q option for fast externs
#JZ 8/10/83   Made xdefs.c compile without the -F flag to save space.
#JZ 8/16/83   Changed this to the makefile for only making the version for sys16
#JZ 8/17/83   Changed makefile to have only one rule for rodata.o and to delete
# 	   the object file
#JZ 9/7/83    Changed the default pathname to /bin for utilities and added a 
#	      default alternate directory DEFDIR that is used for -t and -B
#	      options.  utilloc is changed to now be the basic path everyone
#	      is under, usually '/' while UTILPATH is where the main programs
#	      are, usually /bin.  UTILPATH is no longer passed to mcc.c.
#	      The install rules were changed to install everything in
#	      /v/mesaux/bin and /v/mesaux/lib.
#
# The following block of code is to create various forms of nmcc
# The assumption is made that the root path is utilloc and 
# cc and nasm and nmeld are in $utilloc/theirname while ccom and c2 are in
# $utilloc/$LIB/theirname 
# 1. cc16 is the cc for running on the 16k the temporary directory is /usr/tmp
#    All utilities paths are as in 1 above.  The assembler is "as", meld is 
#    "ld",and suffixes are .s and .o .
# 2. cc  is the cc for running on the 16k.  The instructions are for 
#    compiling it on the 16k.
#    

# The following suffix ordering caused direct vax cc compilation from 
#   .c to .o, but a .s file is always made (and saved) when compiling
#   for the 16K from .c to .o.

#.SUFFIXES:
#.SUFFIXES: .c .o .O .S .c .s .y .I

DESTDIR	= /mesa
RELDIR	= ${DESTDIR}/bin

SCCS	= /v/mesaux/src/cmd/pcc
SCCSD	= sccs -d${SCCS}

# The following variables are default names or paths or suffixes.  They
# are redefined as necessary to make the various versions of nmcc.


#CCOM16K = /e/myunive/pcc/ccom -l

AS	= as
AS16K	= nasm
ASM	= nasm#			Mesa assembler

CCV	= cc
CC 	= nmcc
CPP	= cpp#			C preprocessor
C2	= c2#				C optimiser 
CCOM	= ccom#			C compiler
MELD	= nmeld#			Mesa loader and linker
INSTALL = minstall
#INCL	= /v/mesaux/src/NSC/include

OPFLAGS	= -O#			Call optimiser or set debug flags
NAME	= nmcc#			name of the new executable file

OBJS	= o#				suffixes for object and as source
ASMS	= s
OBJSUF	= \'${OBJS}\'
ASMSUF	= \'${ASMS}\'

TEMPFILE = \"/tmp/\"
TEMP	= "${TEMPFILE}"#		pathname of tmp directory default /tmp

utilloc	=/#		Base directory similar to DESTDIR
UTILPATH = ${utilloc}bin/
MELDPATH = \"${UTILPATH}${MELD}\"
ASMPATH	= \"${UTILPATH}${ASM}\"

LDLIB	= /v/mesaux/usr/lib/#	Path for crt0 and mcrt0
LIB	= nlib/#			Name of library under utilloc
LIBPATH	= \"${LIB}\"
CRT	= crt0.
MCRT	= m${CRT}
DEFDIR = \"/usr/c/\"
CRTPATH	= \"${LDLIB}${CRT}${OBJS}\"
MCRTPATH = \"${LDLIB}${MCRT}${OBJS}\"
CPPPATH	= \"${utilloc}${LIB}${CPP}\"
C2PATH	= \"${utilloc}${LIB}${C2}\"    
CCOMPATH = \"${utilloc}${LIB}${CCOM}\"


# mcflags define the preprocessor variables that are used in mcc.c. They
# are set to the value of the appropriate makefile variable.

mcflags = -DTEMPDIR=${TEMP} -DCPPNAME=${CPPPATH} ${OPFLAGS} \
	-DMELDNAME=${MELDPATH} -DASNAME=${ASMPATH}\
	-DCCOMNAME=${CCOMPATH} -DLIBDIR=${LIBPATH} -DC2NAME=${C2PATH} \
	-DCRTNAME=${CRTPATH} -DSCCS -DMCRTNAME=${MCRTPATH} -I${INCL} \
	-DOBJSUF=${OBJSUF} -DASMSUF=${ASMSUF} ${CEFLAGS} -DDEFDIR=${DEFDIR} \
	-DAS=\"${ASM}\" -DLD=\"${MELD}\"

vaxDEFINES  = -O

mesaDEFINES = -F -q -DSCCS -DTEMPDIR=${TEMPFILE} -DMESA -I${INCL} ${CEFLAGS}

#for vax cc only, using built-in rules and with -G flag...

CFLAGS	 = ${vaxDEFINES} -DSCCS -DMESA -DTEMPDIR=${TEMPFILE} -I${INCL} \
	${CVEFLAGS}

CCOM16K	 = ./ccom -l



SRCS	= allo.c code.c comm1.c local.c local2.c match.c optim.c order.c \
    pftn.c reader.c scan.c table.c trees.c xdefs.c \
    common mac2defs macdefs manifest mfile1 mfile2 cgram.y mcc.c makefile \
    comm2.c

allsource = cgram.c rodata.c  \
	allo.c code.c comm1.c local.c local2.c match.c optim.c order.c \
    	pftn.c reader.c scan.c table.c trees.c xdefs.c \
    	common mac2defs macdefs manifest mfile1 mfile2 cgram.y
      
ofiles	 = allo.o cgram.o code.o comm1.o local.o local2.o match.o optim.o \
	 order.o pftn.o reader.o  rodata.o scan.o table.o \
	 trees.o xdefs.o 

sfiles 	= allo.s cgram.s code.s comm1.s local.s local2.s match.s optim.s \
	 order.s pftn.s reader.s  rodata.s scan.s table.s \
	 trees.s xdefs.s 


# make nmcc, xmcc, kmcc, cmcc cc16, cc etc arguments
# These were left in even though this file only compiles for the ns16000 now
# Since we might need these combinations of args.
ARGS	= ASMS=s  OBJS=o CC=${CCV} \
	LIB=lib/  LDLIB=/lib/ \
        NAME=cc TEMP=${TEMPFILE} ASM=as MELD=ld CEFLAGS=${CEFLAGS} \
        INCL=${INCL}

#VARGS	= vaxDEFINES=${vaxDEFINES} CC=${CCV} CVEFLAGS=${CVEFLAGS} INCL=${INCL}

NARGS	= CC=${CCV} INCL=${INCL} CEFLAGS=${CVEFLAGS} \
	CCOMPATH=\\\"${CCOMPATH}\\\" 

XARGS	= CC=${CCV} INCL=${INCL} CEFLAGS=${CVEFLAGS} \
	MELD=xmeld ASM=xasm C2=xc2 CCOM=xccom NAME=xmcc


ARGS16	= ASMS=s  OBJS=o CC="${CC}" LIB=lib/ \
	LDLIB=/lib/  NAME=cc16 \
        CEFLAGS=${CEFLAGS} TEMP=\\\"$(TEMPFILE)\\\" ASM=as \
        MELD=ld INCL=${INCL}


# For work on the vax for customer, kernel or Mesa.
# use optimiser for installed version

all:cc16 iccom16


install:iccom16 cc16
	${INSTALL} ccom16 ${DESTDIR}/lib/ccom
	${INSTALL} cc16 ${RELDIR}/cc

clean: Sclean shrink
	rm -f ${sfiles} ${ofiles} colonfiles ccom16 cc cc16 nmcc \
		ccom cgram.c rodata.c

shrink:
	rm -f ${ofiles}

Sclean: ; rm -f ${sfiles}

clobber: clean
	${SCCSD} clean; ${SCCSD} get makefile

sources: ${SRCS} colonfiles
${SRCS}:
	${SCCSD} get $@

# Defaults to get :rofix and :yyfix since bug in make prevents their being
# in the dependencies.

colonfiles:
#	rm -f :rofix :yyfix
#	${SCCSD} get :rofix :yyfix
	chmod 755 :rofix :yyfix
	touch colonfiles

mcc: mcc.c
	${CC} ${mcflags} -o ${NAME} mcc.c

cc16: mcc.c
	make ${ARGS16} mcc


#Since the following is being made on the 16000 for the 16000 it uses CCV which
#should be cc, the vax cc.

cc: mcc.c
	make ${ARGS} mcc

# ex16 is experimental version; iccom16 is the installed version with the
# optimiser; ccom16 is the installed version with out the optimiser and
# ccom16k is simply what the others call recursively to do the job

ex16:
	make TEMPFILE=\\\"$(TEMP)\\\"   CC='${CC} -B./' ccom16k

iccom16:
	make TEMPFILE=\\\"$(TEMP)\\\"   CC='${CC} -O' ccom16k

ccom16:
	make TEMPFILE=\\\"$(TEMP)\\\"   CC='${CC}' ccom16k

ccom16k:  ${ofiles}
	${CC} ${ofiles} -o ccom16

rodata.s: rodata.c

rodata.o: rodata.s colonfiles
	./:rofix rodata.s
	${CC} -c rodata.s

rodata.c cgram.c: cgram.y colonfiles
	${YACC} cgram.y
	./:yyfix yyexca yyact yypact yypgo yyr1 yyr2 yychk yydef
	mv y.tab.c cgram.c

tags: .
	rm -f tags
	ctags \
	cgram.c code.c local.c local2.c mac2defs macdefs order.c table.c \
	allo.c comm1.c comm2.c common manifest match.c mfile1 mfile2 optim.c \
	pftn.c reader.c scan.c trees.c xdefs.c 

trees.o: manifest macdefs mfile1 trees.c
optim.o: manifest macdefs mfile1 optim.c
pftn.o: manifest macdefs mfile1 pftn.c
code.o: manifest macdefs mfile1
local.o: manifest macdefs mfile1
scan.o: manifest macdefs mfile1 scan.c
xdefs.o: manifest mfile1 macdefs xdefs.c
	${CC} ${CEFLAGS} -q -DSCCS -DTEMPDIR=${TEMPFILE} -DMESA -I${INCL} -c xdefs.c
cgram.o: manifest mfile1 macdefs cgram.c
comm1.o: manifest mfile1 common macdefs comm1.c
table.o: manifest mfile2 mac2defs macdefs table.c
reader.o: manifest mfile2 mac2defs macdefs reader.c
bigtest: manifest mfile2 mac2defs macdefs
local2.o: manifest mfile2 mac2defs macdefs
order.o: manifest mfile2 mac2defs macdefs
match.o: manifest mfile2 mac2defs macdefs match.c
allo.o: manifest mfile2 mac2defs macdefs allo.c


.c.o: 
	${CC} ${mesaDEFINES} -S $*.c
	${CC} ${mesaDEFINES} -c $*.s

.c.s:
	${CC} ${mesaDEFINES} -S $*.c

.s.o:
	${CC} -c $*.S

# was -hpv for lint

lintall: nmcc.L ccom.L

ccom.L: ${allsource}
	lint -hpxc  ${CFLAGS}  cgram.c xdefs.c scan.c pftn.c \
	trees.c optim.c code.c local.c reader.c \
 	local2.c order.c match.c allo.c comm1.c table.c > ccom.L

nmcc.L: mcc.c
	lint -hpxc ${mcflags} mcc.c > nmcc.L

lgout: ${allsource} colonfiles mcc.c
	touch index
	lgrind ${allsource} mcc.c :rofix :yyfix > tmp.lgout
	lgrind -x index > tmp.index
	mv tmp.index index
	mv tmp.lgout lgout

unedit:
	${SCCSD} unedit ${SRCS} :rofix :yyfix

edit:
	cp makefile Makefile
	sccs clean
	rm colonfiles
	make sources get="edit"
	rm -f Makefile

delta:
	${SCCSD} delta SCCS
	rm -f colonfiles

delget:
	${SCCSD} delget SCCS

oall:	${ofiles}
sall:	${sfiles}
