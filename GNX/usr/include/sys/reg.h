/*
 * @(#)reg.h	3.8	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * Location of the users' stored
 * registers relative to R0.
 * Usage is u.u_ar0[XX].
 */
#define	R0	(0)
#define R1	(-1)
#define R2	(-2)
#define	R3	(-10)
#define	R4	(-11)
#define	R5	(-12)
#define	R6	(-13)
#define	R7	(-14)
#define RSP	(-3)
#define RFP	(2)
#define RPC	(3)
#define	RPSRMOD	(4)
#define	RPS	RPSRMOD

#define	TBIT	2		/* PSR trace bit */
