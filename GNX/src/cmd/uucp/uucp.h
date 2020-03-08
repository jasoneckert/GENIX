/*
 * uucp.h: version 1.5 of 5/13/83
 * Uucp Header
 *
 * @(#)uucp.h	1.5	(National Semiconductor)	5/13/83
 */

#include "stdio.h"
/* define DATAKIT if datakit is available */

/* define DIALOUT if new dialout routine is available */

/* define the last characters for ACU */
#define ACULAST "-<"

/*
 * Site name definition:
 *	Define UNAME if uname() should be used to get uucpname.
 *	This definition is suitable for System III and V sites.
 *	Define HOSTNAME if gethostname() should be used to get
 *	uucpname.  This definition is suitable for GENIX and 4.1a
 *	(and later) systems.
 */
#define HOSTNAME

/* define the value of WFMASK - for umask call - used for all uucp work files */
#define WFMASK 0133

/* define UUSTAT if you need "uustat" command */
#define UUSTAT
 
/*	define UUSUB if you need "uusub" command */
#define UUSUB

	/*  some system names  */

/*
 * put in local uucp name of this machine
 *	This name is used only if neither UNAME nor HOSTNAME is defined.
 */
#ifndef MYNAME
#define MYNAME		"anon"
#endif  MYNAME

#define THISDIR		"/usr/lib/uucp"
#define XQTDIR		"/usr/spool/uucp/.XQTDIR"
#define SQFILE		"/usr/lib/uucp/SQFILE"
#define SQTMP		"/usr/lib/uucp/SQTMP"
#define SLCKTIME	5400	/* system/device timeout (LCK.. files) */
#define SEQFILE		"/usr/lib/uucp/SEQF"
#define SYSFILE		"/usr/lib/uucp/L.sys"
#define DEVFILE		"/usr/lib/uucp/L-devices"
#define DIALFILE	"/usr/lib/uucp/L-dialcodes"
#define USERFILE	"/usr/lib/uucp/USERFILE"
#define ADMIN		"/usr/lib/uucp/ADMIN"

#define SPOOL		"/usr/spool/uucp"
#define LOGDIR		"/usr/spool/uucp"
#define SQLOCK		"/usr/spool/uucp/LCK.SQ"
#define SYSLOG		"/usr/spool/uucp/SYSLOG"
#define PUBDIR		"/usr/spool/uucppublic"

#define SEQLOCK		"LCK.SEQL"
#define CMDPRE		'C'
#define DATAPRE		'D'
#define XQTPRE		'X'

#define LOGPREFIX	"LOG."
#define LOGLOCK	"/usr/spool/uucp/LCK.LOG"
#define LOGFILE	"/usr/spool/uucp/LOGFILE"

#define RMTDEBUG	"AUDIT"
#define SQTIME		60
#define TRYCALLS	2	/* number of tries to dial call */

/*define PROTODEBUG = 1 if testing protocol - introduce errors */
#define DEBUG(l, f, s) if (Debug >= l) fprintf(stderr, f, s)

#define ASSERT(e, f, v) if (!(e)) {\
fprintf(stderr, "AERROR - (%s) ", "e");\
fprintf(stderr, f, v);\
cleanup(FAIL);};


#define SAME 0
#define ANYREAD 04
#define ANYWRITE 02
#define FAIL -1
#define SUCCESS 0
#define CNULL (char *) 0
#define STBNULL (struct sgttyb *) 0
#define MASTER 1
#define SLAVE 0
#define MAXFULLNAME 100
#define MAXMSGTIME 45
#define MAXCHARTIME 15
#define NAMESIZE 15
#define EOTMSG "\004\n\004\n"
#define CALLBACK 1

	/*  commands  */
#define SHELL		"/bin/sh"
#define MAIL		"mail"
#define UUCICO		"/usr/lib/uucp/uucico"
#define UUXQT		"/usr/lib/uucp/uuxqt"
#define UUCP		"uucp"


	/*  call connect fail stuff  */
#define CF_SYSTEM	-1
#define CF_TIME		-2
#define CF_LOCK		-3
#define CF_DIAL		-5
#define CF_LOGIN	-6

	/*  system status stuff  */
#define SS_OK		0
#define SS_FAIL		4
#define SS_NODEVICE	1
#define SS_CALLBACK	2
#define SS_INPROGRESS	3
#define SS_BADSEQ	5

	/*  fail/retry parameters  */
#define RETRYTIME 3300
#define MAXRECALLS 10

	/*  stuff for command execution  */
#define X_RQDFILE	'F'
#define X_STDIN		'I'
#define X_STDOUT	'O'
#define X_CMD		'C'
#define X_USER		'U'
#define X_SENDFILE	'S'
#define X_NONOTI	'N'
#define X_NONZERO	'Z'
#define X_LOCK		"LCK.XQT"
#define X_LOCKTIME	3600

#define WKDSIZE 100	/*  size of work dir name  */
extern int Ifn, Ofn;
extern char Rmtname[];
extern char User[];
extern char Loginuser[];
extern char *Thisdir;
extern char *Spool;
extern char Myname[];
extern int Debug;
extern int Pkdebug;
extern int Pkdrvon;
extern int Bspeed;
extern char Wrkdir[];
extern long Retrytime;
extern short Usrf;
