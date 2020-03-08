/*
 * lpq.c: version 1.4 of 8/12/83
 * Mesa Unix System Command Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)lpq.c	1.4 (NSC) 8/12/83";
# endif

/*
 * Line printer queue
 */

#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <stdio.h>

#define	MAXJOBS 	100
#define SERIAL		0
#define PARALLEL	1

struct	direct  dirent;
struct	stat stbuf;
char	slpdir[] =	"/usr/spool/slp";	/* serial	*/
char	plpdir[] =	"/usr/spool/lpd";	/* parallel	*/
char	*lpddir = slpdir;
int	nextflag;
int	linecnt;
FILE	*df;
FILE	*jf;
char	line[100];
char	username[10];
int	cnt;
int	isdown;

main()
{
	char whichp;

/*
	if (access("/usr/bin/lpr", 1) && access("/bin/lpr", 1)
	    && access("/usr/ucb/lpr", 1))
		isdown++;
*/

	for (whichp=SERIAL; whichp<=PARALLEL; whichp++) {
		if (whichp) lpddir = plpdir;
		if (chdir(lpddir) < 0) {
			perror(lpddir);
			continue;
		}
	
		df = fopen(".", "r");
		if (df == NULL) {
			perror(lpddir);
			break;
		}
	
		fseek(df, 0l, 0);
		linecnt = 0;
		cnt = 0;
		while (fread(&dirent, sizeof dirent, 1, df) == 1) {
			if (dirent.d_ino == 0)
				continue;
			if (dirent.d_name[0] != 'd')
				continue;
			if (dirent.d_name[1] != 'f')
				continue;
			if (stat(dirent.d_name, &stbuf) < 0)
				continue;
			if (cnt == 0) {
				if (whichp) printf ("Parallel queue:\n");
				else printf ("Serial queue:\n");
				printf("Owner\t  Id      Chars  Filename\n");
			}
			cnt++;
			process();
		}
		if (cnt == 0) {
			if (whichp == SERIAL) printf ("Serial ");
			else printf ("Parallel ");
			if (isdown)
				printf("line printer is down.\n");
			else
				printf("line printer queue is empty.\n");
		}
		fclose (df);
	}
	exit(0);
}

process()
{
	char *cp;

	jf = fopen(dirent.d_name, "r");
	if (jf == NULL)
		return;
	while (getline()) {
		switch (line[0]) {

		case 'L':
			strcpy(username, line+1);
			break;

		case 'B':
		case 'F':
			cp = line+1;
			while (*cp++ != ',') ;
			*(cp-1) = '\0';
			if (stat(line+1, &stbuf) < 0)
				stbuf.st_size = 0;
			printf("%-10s%5s%8d  %s (%s)\n",username,
				dirent.d_name+3, stbuf.st_size, line+1, cp);
			break;
		}
	}
	fclose(jf);
}

getline()
{
	register int i, c;

	i = 0;
	while ((c = getc(jf)) != '\n') {
		if (c <= 0)
			return(0);
		if (i < 100)
			line[i++] = c;
	}
	line[i++] = 0;
	return (1);
}
