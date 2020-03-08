;
; ebootsrt0.s: version 1.5 of 9/22/83
; 
;
; @(#)ebootsrt0.s	1.5 (NSC) 9/22/83
;

; Relocation and assembly support for stand-alone boot program

prom:		.equ	h'e02000	;where boot is starts in proms
reloc:		.equ	h'0e000		;real location of program (56K)
prsize:		.equ	h'03030		;size of program (14 K)
stloc:		.equ	h'018000	;location of stack (96K)
topmem:		.equ	h'040000	;top of physical memory (256K)

sbloc:		.equ	stloc+4		;place to store static base
sploc:		.equ	sbloc+4		;place to store stack pointer
fploc:		.equ	sploc+4		;place to store frame pointer

psr_s:		.equ	h'200		;use interrupt stack
psr_i:		.equ    h'800		;allow interrupts

; program starts here (jump to here from zero)
	.program

_rentry::
	bicpsrw psr_s+psr_i		;use interrupt stack and turn off ints
	lprd	sp,stloc
	lprd	fp,stloc

 	addr    @prsize,r0		;no. of bytes to move
 	addr   	@prom,r1		;move from prom to...
 	addr    @reloc,r2		;relocation address
	movsb				;move the string of bytes (do it)
	cxp	next			;jump through mod table to
					;   relocated code

_nrentry::	.equ	_rentry  	;relocated entry point
;Other initialization.  Now running in relocated code.

	.import _main, _stdio, %initfun

next::	save	[r6,r7]
	cxp     _stdio
	restore	[r6,r7]
	cxp     %initfun
	cxp	_main
	bpt

;Here to dispatch to a user program which fits under boot.  Save our
;important registers so that program can return to us.
;
;Call:
;	dispatch(staddr,newmod,tdsize,totsize);
;	           r7     r6     r5     r4
;
staddr:	.equ	12(fp)		;user's start address
newmod:	.equ	16(fp)		;user's initial mod register
tdsize:	.equ	20(fp)		;user's code plus initialized data
totsize:.equ	24(fp)		;user's total program size

_dispatch::
	enter	[r2,r3,r4,r5,r6,r7],0	;save registers
	movmb	@h'e00010,@16,12	;restore user's interrupt stuff
					;  from copy in monitor
;clear user's bss area
	movd	totsize,r0	;get total length of program
	subd	tdsize,r0	;calculate length of bss area
	movd	tdsize,r1	;beginning of bss area
	addr	1(r1),r2	;2nd byte of bss area
	movqd	0,0(r1)		;clear first byte(s)
	movsb			;clear rest of bss
;set up user registers and jump to user
	sprd	sb,@sbloc	;save static base
	sprd	sp,@sploc	;save stack pointer
	sprd	fp,@fploc	;   and frame pointer
	movd	staddr,r1	;get user's start address
	movd	newmod,r2	;  and new mod register
	lprd	sp,reloc-4	;set user's stack pointer
	lprd	fp,reloc-4	;  and frame pntr (same as sp)
	sprw    psr,tos		;push my processor status
	sprw	mod,tos		;push my mod table pointer
	addr	usrrt,tos	;push return address
	lprw	mod,r2		;set user's mod register
 	movqd	2,r0		;tell kernel boot from keys
	jump	0(r1)		;jump to user's startup code

;here on normal return from user, with my mod table pointer set up properly
usrrt:	lprd	sp,@sploc	;restore my stack pointer
	lprd	sb,@sbloc	;  and my static base
	lprd	fp,@fploc	;  and my frame pointer
	exit	[r2,r3,r4,r5,r6,r7]	;restore registers
	rxp	0

;here to dispatch to a program which is too big to fit underneath boot.
;Hence, boot must overwrite itself, and the program will not be able to
;return.
;
;This routine should preserve the boot flags in r7 for the use of the
;loaded program.
;
;Call:
;	bdispatch(staddr,newmod,tdsize,totsize);
;                                 r5      r4

_bdispatch::
	enter	0		;set fp, but don't bother to save regs
	movmb	@h'e00010,@16,12	;restore user's interrupt stuff
	movd	tdsize,r5	;save length of initialized code and data
	movd	totsize,r4	;save total length
;Set up a stack at the very top of memory (topmem) to look like
;an external procedure call from the (imaginary) instruction before
;the starting address of the program.  At the end of this routine
;we will then 'return' to the program's starting address, with
;the mod, fp, and sp set the way we want.
	lprd	sp,topmem	;set stack pointer to very top of memory
	movqw	0,tos		;space for psr
	movw	newmod,tos	;set program's mod
	movd	staddr,tos	;set 'return address' = starting address
	cmpd	r4,reloc	;is program really too big?
	ble	bjuser		;no, just jump

;if we get here, part of the user program has been read in above us.
;We must save the remainder of this routine in a safe place, move
;the remainder of the user routine down into position, and then jump to it.

	addr	brstart,r1	;address of first location to move
	cmpd	r5,reloc	;maybe boot overlaps only bss area
	bgt	b1		;not so, we really must move user code
	addr	bjuser,r1	;no user code to move, just get boot out of way
b1:	addr	brend,r0	;address of last location to move
	subd	r1,r0		;r0 gets number of bytes of code to move
	movd	r4,r2		;length of user code
	addd	stloc,r2	;add length of boot prog to find unused memory
	movd	r2,r3		;keep a copy in a safe register
	movsb			;move brstart code to safe place
	jump	0(r3)		;jump to it

;Now move the appropriate user code down over boot

brstart:			;** beginning of relocated code
	movd	r5,r0		;length of user code and inited data
	subd	reloc,r0	;amount to move
	movd	stloc,r1	;move it from above boot
	movd	reloc,r2	;   onto boot
	movsb
bjuser:
	movd	r4,r0		;length of program
	subd	r5,r0		;get length of bss
	movd	r5,r1		;first byte of bss
	addr	1(r1),r2	;second byte of bss
	movqd	0,0(r1)		;clear first byte of bss
	movsb			;clear remainder of bss

	lprd	fp,topmem	;initialize frame pointer to topmem
 	movqd	2,r0		;tell kernel boot from keys
	rxp	0		;'return' to starting address of program
	;no return
	bpt			;should never get here
brend:				;** end of relocated code

;Miscellany

initfun::
	rxp	0


__rtt::	bpt			;(lln) add for sys.c

_greloc::
	addr	@reloc,r0	;return value of reloc (first location of boot)
	rxp	0

_gbreak::
	addr	@stloc,r0	;return first free location above boot
	rxp	0
	
	.endseg
	.static
	.endseg
