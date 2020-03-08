/*
 * sufu.c: version 1.2 of 1/28/83
 * 
 */
# ifdef SCCS
static char *sccsid = "@(#)sufu.c	1.2 (NSC) 1/28/83";
# endif

/* The following routines are here only temporarily in order to get around
 * a mmu problem in the movsu and movus instructions.  These instructions
 * appear to occasionally modify instructions which follow them, therefore
 * blasting the fubyte and subyte routines and causing the kernel to crash.
 * (Problem due to cpu doing prefetching???).  Furthermore, references to
 * the user's stack area (FFFD00 for example), sometimes drops the high
 * order address bit, causing a bogus reference (as example, 7FFD00).  These
 * problems are of a very low order of frequency, but can appear several
 * times a minute with the right test cases.  (Normally the system can run
 * for hours without ever seeing the problem).
 * Therefore, these routines are written to either use the instructions for
 * debugging and testing purposes, or to call copyin and copyout, which do
 * the work the hard and slow way, but which never fail.  This is determined
 * by the setting of bit 2 in the hardware option switch on the back edge of
 * the cpu board.  Up means use the mmu instructions.
 * NOTE: Problem is only fatal with movsu, so fubyte and fuword are back.
 */

extern	long	suusf;		/* flag saying to do movsu or movus */

xfubyte(addr)
{
	int	temp;

	if (suusf) return(xfubyte(addr));
	temp = 0;
	if (copyin(addr, &temp, 1) < 0) return(-1);
	return(temp);
}

xfuword(addr)
{
	int	temp;

	if (suusf) return(xfuword(addr));
	if (copyin(addr, &temp, 4) < 0) return(-1);
	return(temp);
}

subyte(addr, data)
{
	if (suusf) return(xsubyte(addr, data));
	return(copyout(&data, addr, 1));
}

suword(addr, data)
{
	if (suusf) return(xsuword(addr, data));
	return(copyout(&data, addr, 4));
}
