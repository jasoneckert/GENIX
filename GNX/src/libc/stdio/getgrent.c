/*
 * getgrent.c: version 1.3 of 9/3/82
 * Mesa Unix C Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)getgrent.c	1.3 (NSC) 9/3/82";
# endif

#include <stdio.h>
#include <grp.h>

#define	CL	':'
#define	CM	','
#define	NL	'\n'
#define	MAXGRP	100

static char XGROUP[] = "/etc/group";
static FILE *grf = NULL;
static char line[BUFSIZ+1];
static struct group group;
static char *gr_mem[MAXGRP];

setgrent()
{
	if( !grf )
		grf = fopen( XGROUP, "r" );
	else
		rewind( grf );
}

endgrent()
{
	if( grf ){
		fclose( grf );
		grf = NULL;
	}
}

/*static*/ char *
grskip(p,c)
register char *p;
register c;
{
	while( *p && *p != c ) ++p;
	if( *p ) *p++ = 0;
	return( p );
}

struct group *
getgrent()
{
	register char *p, **q;

	if( !grf && !(grf = fopen( XGROUP, "r" )) )
		return(NULL);
	if( !(p = fgets( line, BUFSIZ, grf )) )
		return(NULL);
	group.gr_name = p;
	group.gr_passwd = p = grskip(p,CL);
	group.gr_gid = atoi( p = grskip(p,CL) );
	group.gr_mem = gr_mem;
	p = grskip(p,CL);
	grskip(p,NL);
	q = gr_mem;
	while( *p ){
		*q++ = p;
		p = grskip(p,CM);
	}
	*q = NULL;
	return( &group );
}
