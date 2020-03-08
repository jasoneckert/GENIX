/*
 * roll.c: version 1.2 of 6/9/83
 * UNIX Game Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)roll.c	1.2 (NSC) 6/9/83";
# endif

/*
 *	This routine rolls ndie nside-sided dice.
 */

# define	reg	register

# ifndef vax
# ifndef ns16000
# define	MAXRAND	32767L

roll(ndie, nsides)
int	ndie, nsides; {

	reg long	tot;
	reg unsigned	n, r;

	tot = 0;
	n = ndie;
	while (n--)
		tot += rand();
	return (int) ((tot * (long) nsides) / ((long) MAXRAND + 1)) + ndie;
}

# endif
# endif
# ifdef vax

roll(ndie, nsides)
reg int	ndie, nsides; {

	reg int		tot, r;
	reg double	num_sides;

	num_sides = nsides;
	tot = 0;
	while (ndie--)
		tot += (r = rand()) * (num_sides / 017777777777) + 1;
	return tot;
}
# endif
# ifdef ns16000

roll(ndie, nsides)
reg int ndie, nsides; {

	reg int		tot;

	tot = 0;
	while (ndie--)
		tot += (rand() % nsides) + 1;
	return tot;
}

# endif
