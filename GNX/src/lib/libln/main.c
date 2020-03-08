/*
 * main.c: version 1.1 of 12/16/82
 * Unix Library Source File
 */
# ifdef SCCS
static char *sccsid = "@(#)main.c	1.1 (NSC) 12/16/82";
# endif

# include "stdio.h"
main(){
yylex();
exit(0);
}
