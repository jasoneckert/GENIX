/*
 * uvers.c: version 1.5 of 10/31/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)uvers.c	1.5 (NSC) 10/31/83";
# endif

/* Type out the version of either the running kernel or a kernel image */
/*					David I. Bell			*/

#include <sys/panic.h>		/* definitions for panic block */
#include <nlist.h>		/* nlist definitions */
#include <a.out.h>		/* a.out definitions */
#include <stdio.h>		/* standard output routines */

int	fd;			/* file descriptor to read from */
int	err;			/* exit status */

struct	exec	execblk;	/* a.out header */
struct	nlist	nl[] = {	/* nlist data */
	{ "_version" },
	{ 0 }
};


main(argc, argv)
	register char	**argv;
{
	register char	*name;		/* name of file */
	register long	addr;		/* current address in file */

	err = 0;
	if (argc < 2) {			/* examine running kernel */
		name = "/dev/kmem";
		fd = open(name, 0);
		if (fd < 0) {
			perror(name);
			exit(1);
		}
		addr = getword(name, PANICADDR);
		addr += (int) (&(((struct panic *)(0))->ps_version));
		addr = getword(name, addr);
		typestring(name, addr);
		exit(err);
	}
	while (argc-- > 1) {		/* examine specified files */
		argv++;
		name = *argv;
		if (access(name, 4)) {	/* check access */
			perror(name);
			err = 1;
			continue;
		}
		fd = open(name, 0);
		if (fd < 0) {
			perror(name);
			err = 1;
			continue;
		}
		if (read(fd, &execblk, sizeof(execblk)) != sizeof(execblk)) {
			perror(name);
			close(fd);
			err = 1;
			continue;
		}
		if (N_BADMAG(execblk)) {
			fprintf(stderr, "%s: not in a.out format\n", name);
			close(fd);
			err = 1;
			continue;
		}
		nlist(name, nl);
		if (nl[0].n_type == 0) {
			fprintf(stderr, "%s: cannot find \"_version\"\n", name);
			close(fd);
			err = 1;
			continue;
		}
		addr = nl[0].n_value + N_TXTOFF(execblk);
		typestring(name, addr);
		close(fd);
	}
	exit(err);
}


/* Type the version string at the specified address in the file */
typestring(name, addr)
	char	*name;		/* name of file */
{
	char	ch;

	if (lseek(fd, addr, 0) < 0) {
		perror(name);
		err = 1;
		return(0);
	}
	while (1) {
		if (read(fd, &ch, 1) != 1) {
			perror(name);
			err = 1;
			return(0);
		}
		if (ch == 0) break;
		putchar(ch);
	}
}


/* Read a long value at the specified address */
getword(name, addr)
	char	*name;		/* name of file */
{
	long	data;		/* data read */

	if ((lseek(fd, addr, 0) < 0) || (read(fd, &data, 4) != 4)) {
		perror(name);
		err = 1;
		return(0);
	}
	return(data);
}
