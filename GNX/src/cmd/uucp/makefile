#
# makefile: version 1.12 of 7/6/83
# Uucp Makefile
#
# @(#)makefile	1.12 (NSC) 7/6/83
#

#
# Recreate the uucp subsystem.
#
#	What about uusend?  Do we want to supply it?
#	Ditto for uuencode and uudecode.
#
#	Chksum.c is just a test file for the checksum routine
#	and isn't part of uucp proper.  We may want to excise it.
#
#	So far, this makefile only recreates and installs binaries.
#	It does nothing about the many auxiliary files uucp requires.
#

# Default system name.  Used only when gethostname() and uname()
# aren't available.
SYSNAME	= anon

RELDIR	= /v/mesaux

SCCS	= /v/mesaux/src/cmd/uucp
INSDIR	= ${RELDIR}/usr/lib/uucp
BIN	= ${RELDIR}/usr/bin
PUBDIR	= ${RELDIR}/usr/spool/uucppublic
SPOOL	= ${RELDIR}/usr/spool/uucp
XQTDIR	= ${RELDIR}/usr/spool/uucp/.XQTDIR

CC	= nmcc
GET	= sccs -d${SCCS} get
CLEAN	= sccs -d${SCCS} clean
INSTALL	= minstall

CEFLAGS	= -DSCCS
CFLAGS	= -O -DMYNAME=\"${SYSNAME}\" ${CEFLAGS}
LINTOP	= -hbau

LIBS	=
OWNER	= uucp
PKON	= pkon.o
IOCTL	=

INSCMDS		= uuclean uucico uusub uuxqt
BINCMDS		= uucp uulog uuname uustat uux

CFILES	= \
	anlwrk.c anyread.c chkpth.c chksum.c cico.c cntrl.c conn.c	\
	cpmv.c expfile.c gename.c getargs.c getopt.c getprm.c		\
	getpw.c getpwinfo.c gio.c gnamef.c gnsys.c gnxseq.c gwd.c	\
	imsg.c index.c ioctl.c lastpart.c logent.c mailst.c pfopen.c	\
	pk0.c pk1.c pkon.c prefix.c sdmail.c shio.c sysacct.c		\
	systat.c ub_sst.c ulockf.c us_crs.c us_open.c us_rrs.c		\
	us_sst.c uuclean.c uucp.c uucpdefs.c uucpname.c			\
	uulog.c uuname.c uurecover.c uusecure.c uustat.c uusub.c	\
	uux.c uuxqt.c versys.c xqt.c

OFILES	= \
	anlwrk.o anyread.o chkpth.o chksum.o cico.o cntrl.o conn.o	\
	cpmv.o expfile.o gename.o getargs.o getopt.o getprm.o		\
	getpw.o getpwinfo.o gio.o gnamef.o gnsys.o gnxseq.o gwd.o	\
	imsg.o index.o ioctl.o lastpart.o logent.o mailst.o pfopen.o	\
	pk0.o pk1.o pkon.o prefix.o sdmail.o shio.o sysacct.o		\
	systat.o ub_sst.o ulockf.o us_crs.o us_open.o us_rrs.o		\
	us_sst.o uuclean.o uucp.o uucpdefs.o uucpname.o			\
	uulog.o uuname.o uurecover.o uusecure.o uustat.o uusub.o	\
	uux.o uuxqt.o versys.o xqt.o

HFILES	= pk.h uucp.h uucpdefs.h uust.h uusub.h

PFILES	= pk.p

OCOMM	= \
	cpmv.o expfile.o gename.o getpwinfo.o index.o lastpart.o	\
	prefix.o shio.o ulockf.o xqt.o
LCOMM	= \
	cpmv.c expfile.c gename.c getpwinfo.c index.c lastpart.c	\
	prefix.c shio.c ulockf.c xqt.c
OUUCP	= \
	uucpdefs.o uucp.o gwd.o chkpth.o getargs.o logent.o uucpname.o	\
	versys.o us_crs.o us_open.o
LUUCP	= \
	uucpdefs.c uucp.c gwd.c chkpth.c getargs.c logent.c uucpname.c	\
	versys.c us_crs.c us_open.c
OUUX	= \
	uucpdefs.o uux.o gwd.o anyread.o chkpth.o getargs.o getprm.o	\
	logent.o uucpname.o versys.o
LUUX	= \
	uucpdefs.c uux.c gwd.c anyread.c chkpth.c getargs.c getprm.c	\
	logent.c uucpname.c versys.c
OUUXQT	= uucpdefs.o uuxqt.o mailst.o getprm.o gnamef.o logent.o uucpname.o
LUUXQT	= uucpdefs.c uuxqt.c mailst.c getprm.c gnamef.c logent.c uucpname.c
OUUCICO	= \
	uucpdefs.o cico.o conn.o cntrl.o pk0.o pk1.o gio.o anyread.o	\
	anlwrk.o chkpth.o getargs.o gnamef.o gnsys.o gnxseq.o		\
	imsg.o logent.o sysacct.o systat.o pfopen.o			\
	mailst.o uucpname.o us_rrs.o us_sst.o us_open.o ub_sst.o
LUUCICO	= \
	uucpdefs.c cico.c conn.c cntrl.c pk0.c pk1.c gio.c anyread.c	\
	anlwrk.c chkpth.c getargs.c gnamef.c gnsys.c gnxseq.c		\
	imsg.c logent.c sysacct.c systat.c pfopen.c			\
	mailst.c uucpname.c us_rrs.c us_sst.c us_open.c ub_sst.c
OUULOG	= uucpdefs.o uulog.o prefix.o xqt.o ulockf.o gnamef.o
LUULOG	= uucpdefs.c uulog.c prefix.c xqt.c ulockf.c gnamef.c
OUUCLEAN	= \
	uucpdefs.o uuclean.o gnamef.o prefix.o mailst.o getpwinfo.o	\
	getargs.o
LUUCLEAN	= \
	uucpdefs.c uuclean.c gnamef.c prefix.c mailst.c getpwinfo.c	\
	getargs.c
OUUNAME	= uuname.o uucpname.o
LUUNAME	= uuname.c uucpname.c
OUUSTAT	= \
	uucpdefs.o uustat.o gnamef.o getpwinfo.o us_open.o getopt.o	\
	lastpart.o cpmv.o
LUUSTAT	= \
	uucpdefs.c uustat.c gnamef.c getpwinfo.c us_open.c getopt.c	\
	lastpart.c cpmv.c
OUUSUB	= uucpdefs.o uusub.o getpwinfo.o us_open.o xqt.o getopt.o
LUUSUB	= uucpdefs.c uusub.c getpwinfo.c us_open.c xqt.c getopt.c

all:	sources ${INSCMDS} ${BINCMDS}

sources: ${CFILES} ${HFILES} ${PFILES}

# To install uucp, we first insure all required directories are in place.
# Then we copy files into them.  The permissions set below are correct
# for intermediate staging areas, but not for final resting places.  The
# /etc/setroot script is responsible for setting final owners and permissions.
#
#	We still have to worry about ${PUBDIR}/.hushlogin,
#	${INSDIR}/{L.sys,L-devices,USERFILE,L_stat,L_sub,R_stat,R_sub,L-dialcodes}
#	and so on.
#
install: ${INSCMDS} ${BINCMDS}
	-for i in ${INSDIR} ${SPOOL} ${PUBDIR} ${XQTDIR}; do	\
		if [ ! -d $$i ]; then				\
			rm -f $$i; mkdir $$i; chmod 770 $$i;	\
		fi;						\
	done
	${INSTALL} ${BINCMDS} ${BIN}
	${INSTALL} ${INSCMDS} ${INSDIR}

clean:
	rm -f ${OFILES} ${INSCMDS} ${BINCMDS}

clobber: clean
	${CLEAN}
	${GET} ${Rmakefile} makefile

uucico:	${OUUCICO} ${OCOMM} ${IOCTL} ${PKON}
	${CC} ${CFLAGS} ${OUUCICO} ${OCOMM} ${IOCTL} ${PKON} ${LIBS} \
	-o uucico

uucp:	${OUUCP} ${OCOMM}
	${CC} ${CFLAGS} ${OUUCP} ${OCOMM} ${LIBS} -o uucp

uux:	${OUUX} ${OCOMM}
	${CC} ${CFLAGS} ${OUUX} ${OCOMM} ${LIBS} -o uux

uuxqt:	${OUUXQT} ${OCOMM}
	${CC} ${CFLAGS} ${OUUXQT} ${OCOMM} ${LIBS} -o uuxqt

uulog:	${OUULOG}
	${CC} ${CFLAGS} ${OUULOG} ${LIBS} -o uulog

uuclean:	${OUUCLEAN}
	${CC} ${CFLAGS} ${OUUCLEAN} ${LIBS} -o uuclean

uuname:	${OUUNAME}
	${CC} ${CFLAGS} ${OUUNAME} ${LIBS} -o uuname
 
uustat:	${OUUSTAT}
	${CC} ${CFLAGS} ${OUUSTAT} ${LIBS} -o uustat

uusub:	${OUUSUB}
	${CC} ${CFLAGS} ${OUUSUB} ${LIBS} -o uusub

#  lint procedures

lint:	lintuucp lintuucico lintuux lintuuxqt lintuulog lintuuclean	\
	lintuuname lintuustat lintuusub

lintuucp:
	lint ${LINTOP} ${LUUCP} ${LCOMM}

lintuucico:
	lint ${LINTOP} ${LUUCICO} ${LCOMM}

lintuux:
	lint ${LINTOP} ${LUUX} ${LCOMM}

lintuuxqt:
	lint ${LINTOP} ${LUUXQT} ${LCOMM}

lintuulog:
	lint ${LINTOP} ${LUULOG}

lintuuclean:
	lint ${LINTOP} ${LUUCLEAN}

lintuuname:
	lint ${LINTOP} ${LUUNAME}

lintuustat:
	lint ${LINTOP} ${LUUSTAT} ${LCOMM}

lintuusub:
	lint ${LINTOP} ${LUUSUB} ${LCOMM}

# Recreate the source dependency rules.
sourcerules:
	rm -f src_rules edsrc
	for i in ${CFILES} ${HFILES} ${PFILES};			do	\
		echo "$$i:	\$${SCCS}/SCCS/s.$$i"	>> src_rules;	\
		echo "	\$${GET} \$${R$$i} $$i"		>> src_rules;	\
	done
	echo '$$r src_rules'				>> edsrc
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d'	>> edsrc
	echo '$$r src_rules'				>> edsrc
	echo 'w'					>> edsrc
	cp makefile makefile.bak
	ed - makefile					<  edsrc
	rm edsrc src_rules

# DO NOT DELETE THIS LINE -- make sourcerules depends on it.
#anlwrk.c:	${SCCS}/SCCS/s.anlwrk.c
#	${GET} ${Ranlwrk.c} anlwrk.c
#anyread.c:	${SCCS}/SCCS/s.anyread.c
#	${GET} ${Ranyread.c} anyread.c
#chkpth.c:	${SCCS}/SCCS/s.chkpth.c
#	${GET} ${Rchkpth.c} chkpth.c
#chksum.c:	${SCCS}/SCCS/s.chksum.c
#	${GET} ${Rchksum.c} chksum.c
#cico.c:	${SCCS}/SCCS/s.cico.c
#	${GET} ${Rcico.c} cico.c
#cntrl.c:	${SCCS}/SCCS/s.cntrl.c
#	${GET} ${Rcntrl.c} cntrl.c
#conn.c:	${SCCS}/SCCS/s.conn.c
#	${GET} ${Rconn.c} conn.c
#cpmv.c:	${SCCS}/SCCS/s.cpmv.c
#	${GET} ${Rcpmv.c} cpmv.c
#expfile.c:	${SCCS}/SCCS/s.expfile.c
#	${GET} ${Rexpfile.c} expfile.c
#gename.c:	${SCCS}/SCCS/s.gename.c
#	${GET} ${Rgename.c} gename.c
#getargs.c:	${SCCS}/SCCS/s.getargs.c
#	${GET} ${Rgetargs.c} getargs.c
#getopt.c:	${SCCS}/SCCS/s.getopt.c
#	${GET} ${Rgetopt.c} getopt.c
#getprm.c:	${SCCS}/SCCS/s.getprm.c
#	${GET} ${Rgetprm.c} getprm.c
#getpw.c:	${SCCS}/SCCS/s.getpw.c
#	${GET} ${Rgetpw.c} getpw.c
#getpwinfo.c:	${SCCS}/SCCS/s.getpwinfo.c
#	${GET} ${Rgetpwinfo.c} getpwinfo.c
#gio.c:	${SCCS}/SCCS/s.gio.c
#	${GET} ${Rgio.c} gio.c
#gnamef.c:	${SCCS}/SCCS/s.gnamef.c
#	${GET} ${Rgnamef.c} gnamef.c
#gnsys.c:	${SCCS}/SCCS/s.gnsys.c
#	${GET} ${Rgnsys.c} gnsys.c
#gnxseq.c:	${SCCS}/SCCS/s.gnxseq.c
#	${GET} ${Rgnxseq.c} gnxseq.c
#gwd.c:	${SCCS}/SCCS/s.gwd.c
#	${GET} ${Rgwd.c} gwd.c
#imsg.c:	${SCCS}/SCCS/s.imsg.c
#	${GET} ${Rimsg.c} imsg.c
#index.c:	${SCCS}/SCCS/s.index.c
#	${GET} ${Rindex.c} index.c
#ioctl.c:	${SCCS}/SCCS/s.ioctl.c
#	${GET} ${Rioctl.c} ioctl.c
#lastpart.c:	${SCCS}/SCCS/s.lastpart.c
#	${GET} ${Rlastpart.c} lastpart.c
#logent.c:	${SCCS}/SCCS/s.logent.c
#	${GET} ${Rlogent.c} logent.c
#mailst.c:	${SCCS}/SCCS/s.mailst.c
#	${GET} ${Rmailst.c} mailst.c
#pfopen.c:	${SCCS}/SCCS/s.pfopen.c
#	${GET} ${Rpfopen.c} pfopen.c
#pk0.c:	${SCCS}/SCCS/s.pk0.c
#	${GET} ${Rpk0.c} pk0.c
#pk1.c:	${SCCS}/SCCS/s.pk1.c
#	${GET} ${Rpk1.c} pk1.c
#pkon.c:	${SCCS}/SCCS/s.pkon.c
#	${GET} ${Rpkon.c} pkon.c
#prefix.c:	${SCCS}/SCCS/s.prefix.c
#	${GET} ${Rprefix.c} prefix.c
#sdmail.c:	${SCCS}/SCCS/s.sdmail.c
#	${GET} ${Rsdmail.c} sdmail.c
#shio.c:	${SCCS}/SCCS/s.shio.c
#	${GET} ${Rshio.c} shio.c
#sysacct.c:	${SCCS}/SCCS/s.sysacct.c
#	${GET} ${Rsysacct.c} sysacct.c
#systat.c:	${SCCS}/SCCS/s.systat.c
#	${GET} ${Rsystat.c} systat.c
#ub_sst.c:	${SCCS}/SCCS/s.ub_sst.c
#	${GET} ${Rub_sst.c} ub_sst.c
#ulockf.c:	${SCCS}/SCCS/s.ulockf.c
#	${GET} ${Rulockf.c} ulockf.c
#us_crs.c:	${SCCS}/SCCS/s.us_crs.c
#	${GET} ${Rus_crs.c} us_crs.c
#us_open.c:	${SCCS}/SCCS/s.us_open.c
#	${GET} ${Rus_open.c} us_open.c
#us_rrs.c:	${SCCS}/SCCS/s.us_rrs.c
#	${GET} ${Rus_rrs.c} us_rrs.c
#us_sst.c:	${SCCS}/SCCS/s.us_sst.c
#	${GET} ${Rus_sst.c} us_sst.c
#uuclean.c:	${SCCS}/SCCS/s.uuclean.c
#	${GET} ${Ruuclean.c} uuclean.c
#uucp.c:	${SCCS}/SCCS/s.uucp.c
#	${GET} ${Ruucp.c} uucp.c
#uucpdefs.c:	${SCCS}/SCCS/s.uucpdefs.c
#	${GET} ${Ruucpdefs.c} uucpdefs.c
#uucpname.c:	${SCCS}/SCCS/s.uucpname.c
#	${GET} ${Ruucpname.c} uucpname.c
#uulog.c:	${SCCS}/SCCS/s.uulog.c
#	${GET} ${Ruulog.c} uulog.c
#uuname.c:	${SCCS}/SCCS/s.uuname.c
#	${GET} ${Ruuname.c} uuname.c
#uurecover.c:	${SCCS}/SCCS/s.uurecover.c
#	${GET} ${Ruurecover.c} uurecover.c
#uusecure.c:	${SCCS}/SCCS/s.uusecure.c
#	${GET} ${Ruusecure.c} uusecure.c
#uustat.c:	${SCCS}/SCCS/s.uustat.c
#	${GET} ${Ruustat.c} uustat.c
#uusub.c:	${SCCS}/SCCS/s.uusub.c
#	${GET} ${Ruusub.c} uusub.c
#uux.c:	${SCCS}/SCCS/s.uux.c
#	${GET} ${Ruux.c} uux.c
#uuxqt.c:	${SCCS}/SCCS/s.uuxqt.c
#	${GET} ${Ruuxqt.c} uuxqt.c
#versys.c:	${SCCS}/SCCS/s.versys.c
#	${GET} ${Rversys.c} versys.c
#xqt.c:	${SCCS}/SCCS/s.xqt.c
#	${GET} ${Rxqt.c} xqt.c
#pk.h:	${SCCS}/SCCS/s.pk.h
#	${GET} ${Rpk.h} pk.h
#uucp.h:	${SCCS}/SCCS/s.uucp.h
#	${GET} ${Ruucp.h} uucp.h
#uucpdefs.h:	${SCCS}/SCCS/s.uucpdefs.h
#	${GET} ${Ruucpdefs.h} uucpdefs.h
#uust.h:	${SCCS}/SCCS/s.uust.h
#	${GET} ${Ruust.h} uust.h
#uusub.h:	${SCCS}/SCCS/s.uusub.h
#	${GET} ${Ruusub.h} uusub.h
#pk.p:	${SCCS}/SCCS/s.pk.p
#	${GET} ${Rpk.p} pk.p
