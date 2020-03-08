/*
 * auxf.c: version 1.1 of 11/3/82
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)auxf.c	1.1 (NSC) 11/3/82";
# endif

# include	"../hdr/defines.h"

/*
	Figures out names for g-file, l-file, x-file, etc.

	File	Module	g-file	l-file	x-file & rest

	a/s.m	m	m	l.m	a/x.m

	Second argument is letter; 0 means module name is wanted.
*/

auxf(sfile,ch)
register char *sfile;
register char ch;
{
	static char auxfile[FILESIZE];
	register char *snp;

	snp = sname(sfile);

	switch(ch) {

	case 0:
	case 'g':	copy(&snp[2],auxfile);
			break;

	case 'l':	copy(snp,auxfile);
			auxfile[0] = 'l';
			break;

	default:
			copy(sfile,auxfile);
			auxfile[snp-sfile] = ch;
	}
	return(auxfile);
}
