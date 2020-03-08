/*
 * linemod.c: version 1.2 of 4/27/83
 * Unix Plot Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)linemod.c	1.2 (NSC) 4/27/83";
# endif

linemod(s)
char *s;
{
	char c;
	putch(033);
	switch(s[0]){
	case 'l':	
		c = 'd';
		break;
	case 'd':	
		if(s[3] != 'd')c='a';
		else c='b';
		break;
	case 's':
		if(s[5] != '\0')c='c';
		else c='`';
	}
	putch(c);
}
