/*
 * v7.local.h: version 1.1 of 2/15/83
 * Mesa Unix System Command Source File
 *
 * @(#)v7.local.h	1.1	(National Semiconductor)	2/15/83
 */

/*
 * Declarations and constants specific to an installation.
 *
 * Vax/Unix version 7.
 */
 
#define	LOCAL		'r'		/* Local machine id */
#define	MAIL		"/bin/mail"	/* Name of mail sender */
#define DELIVERMAIL	"/etc/delivermail"
					/* Name of classy mail deliverer */
#define	EDITOR		"/usr/ucb/ex"	/* Name of text editor */
#define	VISUAL		"/usr/ucb/vi"	/* Name of display editor */
#define	SHELL		"/bin/csh"	/* Standard shell */
#define	HELPFILE	"/usr/lib/Mail.help"
					/* Name of casual help file */
#define	THELPFILE	"/usr/lib/Mail.help.~"
					/* Name of casual tilde help */
#define	UIDMASK		0177777		/* Significant uid bits */
#define	MASTER		"/usr/lib/Mail.rc"
#define	APPEND				/* New mail goes to end of mailbox */
#define CANLOCK				/* Locking protocol actually works */
#define	UTIME				/* System implements utime(2) */
