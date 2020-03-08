/*
 * dofile.c: version 1.2 of 10/17/83
 * (System III) Sccs Source
 */
# ifdef SCCS
static char *sccsid = "@(#)dofile.c	1.2 (NSC) 10/17/83";
# endif

# include	"../hdr/defines.h"
# include	<dir.h>

int	nfiles;
char	had_dir;
char	had_standinp;


do_file(p,func)
register char *p;
int (*func)();
{
	extern char *Ffile;
	char str[FILESIZE];
	char ibuf[FILESIZE];
	register char *s;

	register DIR		*dirp;
	register struct direct	*entp;

	if (p[0] == '-') {
		had_standinp = 1;
		while (gets(ibuf) != NULL) {
			if (sccsfile(ibuf)) {
				Ffile = ibuf;
				(*func)(ibuf);
				nfiles++;
			}
		}
	}
	else if (exists(p) && (Statbuf.st_mode & S_IFMT) == S_IFDIR) {
		had_dir = 1;
		Ffile = p;
		if ((dirp = opendir (p)) == NULL)
			return;
		/* Examine each directory entry: */
		while ((entp = readdir (dirp)) != NULL) {
			/* Skip "." and "..": */
			if (  (   entp->d_namlen == 1
			      && !strncmp (entp->d_name, ".", 1)
			      )
			   || (   entp->d_namlen == 2
			      && !strncmp (entp->d_name, "..", 2)
			      )
			   )
				continue;
			sprintf (str,"%s/%s", p, entp->d_name);
			if(sccsfile(str)) {
				Ffile = str;
				(*func)(str);
				nfiles++;
			}
		}
		closedir (dirp);
	}
	else {
		Ffile = p;
		(*func)(p);
		nfiles++;
	}
}
