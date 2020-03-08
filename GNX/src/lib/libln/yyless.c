/*
 * yyless.c: version 1.1 of 12/16/82
 * Unix Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)yyless.c	1.1 (NSC) 12/16/82";
# endif

yyless(x)
{
extern char yytext[];
register char *lastch, *ptr;
extern int yyleng;
extern int yyprevious;
lastch = yytext+yyleng;
if (x>=0 && x <= yyleng)
	ptr = x + yytext;
else
	ptr = x;
while (lastch > ptr)
	yyunput(*--lastch);
*lastch = 0;
if (ptr >yytext)
	yyprevious = *--lastch;
yyleng = ptr-yytext;
}
