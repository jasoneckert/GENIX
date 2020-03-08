/*
 * @(#)psr.h	3.2	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

#ifdef LOCORE
;
; 16032 psr configuration
;
 psr_c:		.equ	1 shl 0		; carry
 psr_t:		.equ	1 shl 1		; trace
 psr_l:		.equ	1 shl 2		; lower
 psr_f:		.equ	1 shl 5		; flag
 psr_z:		.equ	1 shl 6		; zero
 psr_n:		.equ	1 shl 7		; negative
 psr_u:		.equ	1 shl 8		; user/supervisor
 psr_s:		.equ	1 shl 9		; sp0/sp1
 psr_p:		.equ	1 shl 10	; pending
 psr_i:		.equ	1 shl 11	; ion

#else

#define PSR_C	(1 << 0	)	/* carry	*/
#define PSR_T	(1 << 1	)	/* trace	*/
#define PSR_L	(1 << 2	)	/* lower	*/
#define PSR_F	(1 << 5	)	/* flag		*/
#define PSR_Z	(1 << 6	)	/* zero		*/
#define PSR_N	(1 << 7	)	/* negative	*/
#define PSR_U	(1 << 8	)	/* user/supervisor	*/
#define PSR_S	(1 << 9	)	/* sp0/sp1	*/
#define PSR_P	(1 << 10)	/* pending	*/
#define PSR_I	(1 << 11)	/* ion		*/

struct	modpsr {
	u_short	mod,psr;
};

#define	PSR(s)	s.psr
#define MOD(m)	m.mod

#endif
