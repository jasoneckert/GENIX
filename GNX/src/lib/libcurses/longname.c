/*
 * longname.c: version 1.1 of 9/1/82
 * Curses Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)longname.c	1.1 (NSC) 9/1/82";
# endif

# define	reg	register

/*
 *	This routine returns the long name of the terminal or "def"
 *	if none can be found.
 */
char *
longname(bp, def)
reg char	*bp, *def; {

	reg char	*cp;
	static char ttytype[20];
	char save;

	while (*bp && *bp != ':' && *bp != '|')
		bp++;
	if (*bp == '|') {
		bp++;
		cp = bp;
		while (*cp && *cp != ':' && *cp != '|')
			cp++;
		save = *cp;
		*cp = 0;
		strcpy(ttytype, bp);
		*cp = save;
		return ttytype;
	}
	strcpy(ttytype, def);
	return ttytype;
}
