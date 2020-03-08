
/*
 * machine.h: version 1.4 of 12/6/82
 * 
 *
 * @(#)machine.h	1.4	(National Semiconductor)	12/6/82
 */

/* operation type for ptrace and remotemachine access */

#define INIT 0
#define RMEM 1
#define RREG 3
#define WMEM 4
#define WREG 6
#define STEP 9
#define GOGO 7
#define KILL 8
#define GONT 10

/* type of stuff seen on serial line */
#define NOSTAR -1
#define STAR 1
#define EQDATA 2
#define ENTRYMSG 3
#define QUEST 4
#define BANNER 5
#define STUFF 6

extern int port;
