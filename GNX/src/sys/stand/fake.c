/*
 * fake.c: version 2.2 of 10/1/82
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)fake.c	2.2 (NSC) 10/1/82";
# endif

signal() {
printf("\nRoutine signal called!\n");
exit();
}
