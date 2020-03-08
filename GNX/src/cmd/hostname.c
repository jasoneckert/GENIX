/*
 * hostname.c: version 1.1 of 5/10/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)hostname.c	1.1 (NSC) 5/10/83";
# endif


/* Set or obtain the host name */

#define	NAMESIZE 32		/* maximum size of host name */

int	namelen;		/* length of host name */
char	name[NAMESIZE+1];	/* host name string */

main(argc, argv)
	char	**argv;
{
	if (argc > 1) {
		argv++;
		if (sethostname(*argv, strlen(*argv))) {
			perror("cannot set hostname");
			exit(1);
		}
		exit(0);
	}
	namelen = NAMESIZE;
	if (gethostname(name, &namelen)) {
		perror("cannot read hostname");
		exit(1);
	}
	if (namelen == 0) {
		printf("Host name was not set\n");
		exit(1);
	}
	printf("%s\n", name);
}
