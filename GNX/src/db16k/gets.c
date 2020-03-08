/*
 * gets.c: version 2.2 of 10/1/82
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)gets.c	2.2 (NSC) 10/1/82";
# endif

gets(buf)
	char *buf;
{
	register char *lp;
	register c;

	lp = buf;
	for (;;) {
		c = getchar() & 0177;
	store:
		switch(c) {
		case '\n':
		case '\r':
			c = '\n';
			*lp++ = '\0';
			return;
		case '\b':
		case '#':
			lp--;
			if (lp < buf)
				lp = buf;
			continue;
		case '@':
		case 'u'&037:
			lp = buf;
			putchar('\n');
			continue;
		default:
			*lp++ = c;
		}
	}
}
