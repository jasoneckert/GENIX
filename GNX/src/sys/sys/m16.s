; @(#)m16.s	3.40	10/8/83
; @(#)Copyright (C) 1983 by National Semiconductor Corp.

#include "../h/param.h"
#include <sys/reboot.h>

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;	machine language assist for the 16032
;;
;;	Michael Puckett 5-5-82
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


_u::	.equ	@BOTU		;the absolute location of the u.

.import _main, %initfun, _kdebug, _initvm, _end, _initmem, _topsys
.import	_alloctables, _zeroprogress, _cpanic, _pblk, _spt, _proc, _version
.import	_bmconsf, _topmem

	.static
_furetries::	.double	0	; debugging information for fubyte/fuword
_fudata1::	.double	0
_fudata2::	.double	0
	.endseg


_program::
startcode::
	setcfg	m,i		; enable vectored interrupts, mmu instructions
	bicpsrw	psr_i+psr_s	; disable interrupts,access ksp
	;
	; r0 contains value for f key press(set by boot, rdt, or ddt)
	; if r0 is 8 then set kdebug to nonzero to allow bpt,trc, and nmi
	; to trap to rdt for remote debugging
	cmpd	8,r0		; is equal to 8
	bne	noremote	; no, then unix gets traps
	movqd	-1,_kdebug	; yes, then rdt gets traps
noremote:
	;
	addr	_end,r0		; find end of kernel image
	addd	KERNELSTACK+PGOFSET,r0	; add some stack space
	bicd	PGOFSET,r0	; round up to next page boundary
	movd	r0,_topsys	; store into topsys
	lprd	sp,r0		; initialize ksp
	lprd	fp,r0		; and fp
	cxp	%initfun	; initialize modules
	cxp	_initmem	; setup memory layout
	cxp	_alloctables	; allocate system tables
	addr	_pblk,r0	; get address of the panic block
	movd	r0,@PANICADDR	; store its address in a known place (loc 12)
	addr	_version,PS_VERSION(r0)	;store version pointer in panic block
	movd	_proc,PS_PROC(r0)	;store address of proc table
	movd	_spt,PS_SPT(r0)		;and address of spt table
	cxp	initicu		; setup the interrupt vectors, spl7()
	cxp	_initvm		; turn on kernel mapping
	addr	@BOTU+USIZE-4,r0  ; move to process zero stack
	lprd	sp,r0
	lprd	fp,r0
	movqd	0,r1		; spl0();
	bsr	spl
	cxp	_main		; INITIALIZE UNIX

	; Set up initial user state and simulate a return to the user domain.
	;	The initial user state is quite constrained in that it has
	;	a starting pc of 0 and an invalid mod table.  This address
	;	space was just set up in main().  It makes no references that
	;	require a valid mod table, but instead just forces the first
	;	page of user stack to be mapped in and then execs init.
	bispsrw	psr_s		; switch to user stack
	lprd	sp,TOPUSER	; initialize usp
	lprd	fp,TOPUSER	; and fp...
	bicpsrw	psr_s		; return to supervisor stack
	movw	psr_i+psr_u+psr_s,tos	; store psr value on stack
	movqw	0,tos		; store a legal mod value on stack
	movqd	0,tos		; store beginning pc on stack
	rett	0		; "return" to user mode to begin unix
;	br	rettpanic	; failed, go die


;Here if a return from trap failed unexpectedly
rettpanic::
	addr	rettst,tos	; failed, get panic string
	cxp	_panic		; and die

rettst:	.byte	'badrett', x'0	; panic string


;Routines to read or write bytes and doublewords from the user.
;These use the movus and movsu instructions.  The obscure code around
;them is for handling the cases when the instructions cause mmu aborts.
;Read supabort in vm.c to see how this is handled.

uad:	.equ	8(sp)		; user address
data:	.equ	12(sp)		; data value

;fubyte(uad) char *uad;
;read a byte of data from the user.

;TEMPORARY CHANGE:  THESE ROUTINES HAVE AN X IN THEIR NAME AND ARE CALLED 
;ONLY IF THE MMU INSTRUCTIONS CAN BE TRUSTED.  OTHERWISE THEY ARE SIMULATED
;BY CALLS TO COPYIN AND COPYOUT.  THE REAL ROUTINES ARE TEMPORARILY IN SUFU.C.
;MMU BUG: FUBYTE AND FUWORD DO THE MOVUS INSTRUCTION TWICE, TO GET AROUND
;RELIABILITY PROBLEMS.

fubretry:			;temporary - here on fuword failures
	addqd	1,_furetries	;count retries
	movd	r0,_fudata1	;remember data values
	movd	r1,_fudata2
_fuibyte::
_fubyte::
	movd	uad,r0		;get user address
	movqd	0,tos		;store zero on stack
	addr	tos,r1		;get its address
	addqd	0,r0		;quick way to clear "F" flag
	movusb	r0,r1		;read the byte from the user (maybe faulting)
	movd	tos,r1		;get result
	bfc	fuby1		;skip if it worked
	movqd	-1,r1		;otherwise get error value
fuby1:	movqd	0,tos		;another zero
	addr	tos,r2		;get its address
	addqd	0,r0		;clear flag
	movusb	r0,r2		;read the byte again
	movd	tos,r0		;get result
	bfc	fuby2		;skip if worked
	movqd	-1,r0		;get error value
fuby2:	cmpd	r0,r1		;get same answer twice?
	bne	fubretry	;nope, try again
	rxp	0		;return


;subyte(uad,data) byte *uad,data;
;store a byte of data into the user's memory.

_xsuibyte::
_xsubyte::
	addr	data,r0		;get address of data
	movd	uad,r1		;and user address
	addqd	0,r0		;quick way to clear "F" flag
	movsub	r0,r1		;store byte into user memory (maybe faulting)
	bfc	suok1		;skip if it worked
badad2:	movqd	-1,r0		;say we failed
	rxp	0		;return


;fuword(uad) int *uad;
;return a word (actually a double word, 4 bytes) from the user.

fuwretry:			;temporary - here on fuword failures
	addqd	1,_furetries	;count retries
	movd	r0,_fudata1	;remember data values
	movd	r1,_fudata2
_fuiword::
_fuword::
	movd	uad,r0		;get user address
	movqd	0,tos		;store zero on stack
	addr	tos,r1		;get its address
	addqd	0,r0		;quick way to clear "F" flag
	movusd	r0,r1		;read the doubleword (maybe faulting)
	movd	tos,r1		;get result
	bfc	fuw1		;skip if it worked
	movqd	-1,r1		;otherwise get error value
fuw1:	movqd	0,tos		;another zero
	addr	tos,r2		;get its address
	addqd	0,r0		;clear flag
	movusd	r0,r2		;read doubleword again
	movd	tos,r0		;get result
	bfc	fuw2		;skip if it worked
	movqd	-1,r0		;get error value
fuw2:	cmpd	r0,r1		;same value twice?
	bne	fuwretry	;no, try again
	rxp	0		;return


;suword(uad,data) int *uad,data;
;store a word (actually a doubleword) into the user's memory.

_xsuiword::
_xsuword::
	addr	data,r0		;get address of data
	movd	uad,r1		;and user address
	addqd	0,r0		;quick way to clear "F" flag
	movsud	r0,r1		;store the doubleword (maybe faulting)
	bfs	badad2		;skip if it failed
suok1:	movqd	0,r0		;say we succeeded
	rxp	0		;return



;;	copyin(src,dst,cnt) char *src,*dst; int cnt;
;;	copy cnt bytes from user address space to kernel address space.
;;
;;	copyout(src,dst,cnt) char *src,*dst; int cnt;
;;	copy cnt bytes from kernel address space to user address space.

usradr:	.equ	r7			;user address (MUST be register)
excadr:	.equ	r6			;exec address
pagadr:	.equ	r5			;physical address of user page
cnt:	.equ	r4			;number of bytes left
inc:	.equ	r3			;current number of bytes
out:	.equ	-4(fp)			;nonzero if copying out to user

	.routine _pagein, _physaddr, _lockaddr, _unlockaddr, _setmod

_copyin::
	enter	[r3,r4,r5,r6,r7],4	;save registers
	movd	12(fp),usradr		;save user address
	movd	16(fp),excadr		;save exec address
	movd	20(fp),cnt		;save count
	movqd	0,out			;we are reading from user
	br	copyloop		;join common code

_copyout::
	enter	[r3,r4,r5,r6,r7],4	;save registers
	movd	12(fp),excadr		;save exec address
	movd	16(fp),usradr		;save user address
	movd	20(fp),cnt		;save count
	movqd	1,out			;we are writing to user
;	br	copyloop		;join common code

;Here for each page of the transfer.  Determine the number of bytes left to
;be copied in the current page, validate the user address, and do the copy.

copyloop:
	cmpqd	0,cnt			;copied all the bytes yet?
	bge	cpdone			;yes, return zero
	addr	@MCSIZE,inc		;size of a page
	addr	@MCOFSET,r1		;mask for extracting offset
	andd	usradr,r1		;get page offset of user address
	subd	r1,inc			;get number of bytes left in page
	cmpd	inc,cnt			;compare with remaining count
	ble	cp1			;skip if less
	movd	cnt,inc			;remaining count is less, use that
cp1:	movd	usradr,tos		;user address
	movd	@(BOTU+U_SPTI),tos	;spt index
	cxp	_physaddr		;return physical address of page
	adjspb	-8
	cmpd	r0,_topmem		;check return value
	blo	cp2			;proceed if was in memory
	movqd	-1,tos			;must page it in, get invalid pc
	movd	usradr,tos		;user address
	movd	@(BOTU+U_SPTI),tos	;spt index
	cxp	_pagein			;bring the page into memory
	adjspb	-12
	cmpqd	0,r0			;check return
	bgt	cperr			;failed if negative
cp2:	movd	r0,pagadr		;save physical page address
	cmpqb	0,out			;copying to user??
	beq	cp3			;no, skip write check
	movd	usradr,r0		;get user address
	wrval	r0			;check its legality (should not abort)
	bfs	cperr			;lose if protection was read only
cp3:	movd	pagadr,tos		;page address
	cxp	_lockaddr		;lock the memory page
	adjspb	-4
	movd	pagadr,r1		;assume source address is user
	movd	excadr,r2		;and assume destination address is exec
	cmpqb	0,out			;check direction of copy
	beq	cpdo			;proceed if correct
	movd	excadr,r1		;nope, then source address is exec
	movd	pagadr,r2		;and destination address is user
cpdo:	movd	inc,r0			;current number of bytes
	ashd	-2,r0			;turn into doublewords
	movsd				;copy doublewords
	movqd	3,r0			;get mask
	andw	inc,r0			;compute number of remaining bytes
	movsb				;copy remaining bytes
	cmpqb	0,out			;copying out?
	beq	cpnmod			;no, skip
	movd	usradr,tos		;user address
	movd	@(BOTU+U_SPTI),tos	;spt index
	cxp	_setmod			;set modify bit in user's pte
	adjspb	-8
cpnmod:	movd	pagadr,tos		;page address
	cxp	_unlockaddr		;unlock the page
	adjspb	-4
	subd	inc,cnt			;decrement number of bytes left
	addd	inc,usradr		;increment user address
	addd	inc,excadr		;increment exec address
	br	copyloop		;and loop for next page

cperr:	movqd	-1,r0			;return failure
	br	cpret			;go return
cpdone:	movqd	0,r0			;return success
cpret:	exit	[r3,r4,r5,r6,r7]	;restore acs
	rxp	0			;return result in r0



;Clear a page of memory which contains the given address.
;	clearpage(addr);

_clearpage::
	addr	@(MCSIZE/4-1),r0	;doublewords in a page minus one
	movd	8(sp),r1		;address to be cleared
	bicw	MCOFSET,r1		;back up to beginning of page
	addr	4(r1),r2		;get next doubleword address
	movqd	0,0(r1)			;clear first doubleword
	movsd				;then rest of the page
	rxp	0			;done


;Copy data from one page of memory to another, given the source and destination
;addresses.  The addresses are page aligned for the copy.
;	copypage(sourceaddr, destaddr);

_copypage::
	addr	@(MCSIZE/4),r0		;doublewords in a page
	movd	8(sp),r1		;get source address
	bicw	MCOFSET,r1		;back up to beginning of page
	movd	12(sp),r2		;get destination address
	bicw	MCOFSET,r2		;back up to beginning of page
	movsd				;do the copy
	rxp	0			;done


;Copy count bytes from from to to. (Hope for word alignment)
;	bcopy(from, to, count);

_bcopy::
	movd	8(sp),r1		;address of source string
	movd	12(sp),r2		;address of destination string
	movd	16(sp),r0		;number of bytes to move
	ashd	-2,r0			;number of doublewords to move
	movsd				;move the doublewords
	movqd	3,r0			;mask bytecount out
	andw	16(sp),r0		;number of bytes to move
	movsb				;move the bytes
	rxp	0			;done


;Zero count bytes starting at given address.  Odd counts are handled correctly.
;	bzero(addr, count);

_bzero::
	movd	12(sp),r0		;count of bytes
	movd	8(sp),r1		;first address to zero
	ashd	-2,r0			;number of doublewords to zero
	cmpqd	0,r0			;any doublewords to zero?
	bge	bzerob			;nope, skip on
	addqd	-1,r0			;yes, decrement count by one
	movqd	0,0(r1)			;clear first doubleword
	addr	4(r1),r2		;get address of next doubleword
	movsd				;clear rest of doublewords
	movd	r2,r1			;get next address to be zeroed
bzerob:	movqd	3,r0			;get mask for remaining bytes
	andw	12(sp),r0		;get remaining bytes
	cmpqd	0,r0			;see if any left
	bne	bzerol			;if so, go clear them
	rxp	0			;otherwise done

bzerol:	movqb	0,0(r1)			;clear a byte
	addqd	1,r1			;increment address
	acbd	-1,r0,bzerol		;loop
	rxp	0			;done


;Find next nonzero doubleword in an array.  Used to skip null ptes.
;	newaddr = skippte(addr, endaddr);
;addr is starting address of search, endaddr is address of end of search.
;Returns address of next nonempty doubleword, or endaddr if reached end.

_skippte::
	movd	r4,tos			;save register
	movd	12(sp),r1		;get address of array
	movd	16(sp),r0		;get ending address
	subd	r1,r0			;compute difference of addresses
	divd	4,r0			;turn into count of doublewords
	movqd	0,r4			;zero value
	skpsdw				;skip while zeroes are found
	movd	r1,r0			;get termination address
	movd	tos,r4			;restore register
	rxp	0			;done


;Tell the MMU to invalidate the specified address.  The mtpr(eia,addr) call
;can also do this, but this is faster.
_mteia::
	lmr	eia,8(sp)		;write to eia to invalidate address
#if MCPAGES == 2
	addd	PGSIZE,8(sp)
	lmr	eia,8(sp)
#endif
	rxp	0			;done

; set or clear the ben bit in the msr for breakpoint enable
; This is when using breakpoints
_setben::
	smr	msr,r0
	sbitb	20,r0		; set breakpoint enable
	sbitb	23,r0		; set user flow trace(don't do with MMU rev. G)
	lmr	msr,r0
	rxp	0

_clearben::
	smr	msr,r0
	cbitb	20,r0		; clear breakpoint enable
	lmr	msr,r0
	rxp	0

_clearberr::
	smr	msr,r0
	sbitb	1,r0		; clear msr status bits(MMU rev. G and on)
	lmr	msr,r0
	rxp	0

;Enable or disable the floating point instructions.  This is done by executing
;the appropriate setcfg instruction.
_enablefpu::
	setcfg	m,i,f
	rxp	0

_disablefpu::
	setcfg	m,i
	rxp	0


;Special code to clear several doublewords for the idleclearpage routine.
_idleclearbytes::
	movd	_zeroprogress,r0	;get current address to zero
	movqd	0,0(r0)			;clear one doubleword
	movqd	0,4(r0)			;and more too
	movqd	0,8(r0)
	movqd	0,12(r0)
	movqd	0,16(r0)
	movqd	0,20(r0)
	movqd	0,24(r0)
	movqd	0,28(r0)
	addd	32,_zeroprogress	;increment address
	rxp	0			;done


;Routine to allocate a bit from a bit array.  Called from C by:
;	num = bitalloc(tableaddress, tablelength, beginningbit)
;where:
; tableaddress/	address of the bit table
; tablelength/	number of bits in the table
; beginningbit/	requested starting bit number to help avoid compaction
;returns:
; num/	-1	no free bit in table
; num/	>=0	bit number of allocated bit
;Allocated bits in the table have the value 0.  This routine can safely run
;with the interrupts enabled.


tabadr:	.equ	16(sp)			;address of bit table
tablen:	.equ	20(sp)			;size of table in bits
begbit:	.equ	24(sp)			;requested starting bit


_bitalloc::
	enter	[r4],0			;save registers
scan:
	movd	tablen,r0		;number of bits in table
	addd	31,r0			;round up to next doubleword
	divd	32,r0			;convert to count of doublewords
	movd	begbit,r1		;starting bit number
	divd	32,r1			;convert to doubleword offset
	subd	r1,r0			;decrement doubleword count by skip
	addd	tabadr,r1		;add to get beginning doubleword address
	movqd	0,r4			;want to scan for zeroes
	skpsdw				;skip zeroes (ie, allocated bits)
	bfc	failed			;failed if count ran out
	ffsd	0(r1),r4		;find bit offset in the nonzero word
	cvtp	r4,0(r1),r0		;find corresponding bit pointer
	movqd	0,r2			;bit offset of zero
	cvtp	r2,0(tabadr),r2		;find bit pointer for start of table
	subd	r2,r0			;compute bit difference
	cmpd	r0,tablen		;compare with table size
	bge	failed			;failed if beyond end of table
	cbitd	r4,0(r1)		;clear the found bit
	bfc	scan			;was clear, interrupt ripped us off !!!
	exit	[r4]			;restore acs
	rxp	0			;return bit number

failed:	cmpqd	0,begbit		;see if tried from the top yet
	movqd	0,begbit		;and set to do so
	bne	scan			;try again from the top
	movqd	-1,r0			;scan failed, get error value
	exit	[r4]			;restore acs
	rxp	0			;return value


;Routine to free a bit from a bit table.   Call from C:
;	val = bitfree(tableaddress, bitnumber)
;returns nonzero if the bit was already free.

_bitfree::
	sbitd	12(sp),0(8(sp))		;set the bit to free it
	sfsd	r0			;return nonzero if already free
	rxp	0			;done



;Routine to initialize a bit table to the free state.  Call from C:
;	bitinit(tableaddress, tablelength)

_bitinit::
	enter	0			;make ddt happy
	movd	12(sp),r1		;table address
	movd	16(sp),r0		;size of table
	divd	32,r0			;number of full doublewords
	addr	r1[r0:d],r2		;address after the doublewords
	cmpqd	0,r0			;have any full doublewords?
	bge	bitfin			;no, go finish up
	movqd	-1,0(r1)		;set first doubleword full of ones
	addqd	-1,r0			;decrement count
	addr	4(r1),r2		;address of next doubleword
	movsd				;set bits in remainder of doublewords

bitfin:					;here to set bits at end of table
	movd	16(sp),r0		;size of table
	modd	32,r0			;number of leftover bits in doubleword
	movqd	1,r1			;get a bit
	lshd	r0,r1			;shift left by number of bits
	addqd	-1,r1			;produce mask value
	ord	r1,0(r2)		;set bits in last partial doubleword
	exit				;restore frame
	rxp	0			;done


;; This is the layout of the context saved by save, and restored by
;; resume. Two instances of this structure exist within the u. area,
;; they are u.qsav, and u.ssav.

;;typedef struct ucontext {
;;	int	r7;
;;	int	r6;
;;	int	r5;
;;	int	r4;
;;	int	r3;
;;	int	sp;	/* stored in r2 */
;;	int	pc;	/* stored in r1 */
;;	int	mod;	/* stored in r0 */
;;	int	fp;
;;} label_t;

;;
;;	save(u.{usav,ssav,qsav}) char *u.{usav,ssav,qsav}
;;
;;	save user state in u.?
;;
;;	Much trickiness here. R0 contains the return psrw & mod, and
;;	R1 contains the address which will be used as the return address
;;	used by the resume. R2 contains the sp to be used by the resume.
;;	The ksp is changed to point to the u. area, and the entire state
;;	is saved with the enter instruction. The current psrw is saved
;;	so that interrupts may be disabled within this routine while it
;;	is running with the changed ksp. A rett (return from trap) will
;;	restore the interrupt conditions after execution of the return.
;;
;;	Note that the value saved for the sp is the value it will have after
;;	resumption or return, (not including call-site stack cleanup --
;;	throwing away the argument).
;;
;;	***** THIS ROUTINE DEPENDS ON THE C COMPILER USING R0,R1,R2 AS
;;	***** TEMPORARIES ********************************************
;;
;;	WARNING: This routine temporarily invalidates the stack pointer.
;;		Trying to single step through those instructions that
;;		operate with altered sp will NOT work.
;;
_save::
	sprw	psr,6(sp)		; save the current psr
	movd	4(sp),r0		; fetch mod&psrw
	movd	0(sp),r1		; fetch return address
	bicpsrw	psr_i			; interrupts off for trickiness
	addr	8(sp),r2		; save the return ksp in r2
					; BEGIN invalid stack...
	lprd	sp,8(sp)		; point the stack at the u.
	adjspb	-9*4			; (the top of the save area for push's)
	; CONTEXT SAVE user regs, fp, sp, mod, psrw, & return link
	sprd	fp,tos
	save	[r0,r1,r2,r3,r4,r5,r6,r7]
	lprd	sp,r2			; restore the "real" ksp
	adjspb	8
					; END invalid stack
	movqd	0,r0			; return a zero to caller
	rett	0			; (restore interrupt state)
	br	rettpanic		; failed
	;bispsrw	psr_i
	;rxp	0

;;
;;	resume(u.{usav,ssav,qsav}) char *u.{usav,ssav,qsav}
;;
;;	restore user state from u.?
;;
;;	WARNING: This routine temporarily invalidates the stack pointer.
;;		Trying to single step through those instructions that
;;		operate with altered sp will NOT work.
;;
_resume::
	bicpsrw	psr_i			; interrupts off for trickiness
					; BEGIN invalid stack...
	lprd	sp,8(sp)		; point my stack into u.
	restore	[r0,r1,r2,r3,r4,r5,r6,r7]
	lprd	fp,tos			; restore fp
	lprd	sp,r2			; ... and ksp
					; END invalid stack
	; Set up simulated return to resumee (using new calling sequence):
	; 	At this point the sp is what it was at the point of saving,
	;	that is, pointing at the arg to save.
	movd	r0,tos			; push psr & mod
	movd	r1,tos			; ... and return address
	movqd	1,r0			; return value is 1
	rett	0			; (restore saved interrupt state)
	br	rettpanic		; failed

#ifdef notdef
_assert::
	cmpqd	0,8(sp)
	beq	die
	rxp	0
die:
	bpt
#endif

;; move stack and frame pointers by offset
;;
;;	movestack(offset)
;;
;; interrupts should be off during call

_movestack::
	movd	8(sp),r0		;get offset
	sprd	sp,r1			;get old stack pointer
	addd	r0,r1			;add in offset
	lprd	sp,r1			;set new stack pointer
	sprd	fp,r1			;get old frame pointer
	addd	r0,r1			;add in offset
	lprd	fp,r1			;set new frame pointer
	rxp	0			;return

_ffs::
	movqd	0,r0
	ffsd	8(sp),r0
	bfs	nothing
	addqd	1,r0
	rxp	0
nothing:
	movqd	0,r0
	rxp	0


;Save and restore the floating point registers.  This is only called for
;those processes which are actively using floating point.

_savefpu::
	sfsr	r0			;need two steps because of assembler bug
	movd	r0,@(BOTU+U_FSR)	;save status register
	addr	@(BOTU+U_FREGS),r0	;get address of storage area
	movl	f0,0(r0)		;save data registers in pairs
	movl	f2,8(r0)
	movl	f4,16(r0)
	movl	f6,24(r0)
	rxp	0

_restorefpu::
	lfsr	@(BOTU+U_FSR)		;restore status register
	addr	@(BOTU+U_FREGS),r0	;get address of storage area
	movl	0(r0),f0		;restore data registers in pairs
	movl	8(r0),f2
	movl	16(r0),f4
	movl	24(r0),r6
	rxp	0
;
;Code for init
;
;This is the code for the initial user process.  It is copied out
;to a user's address space in main.c

exec:	.equ	11

_icode::
	movqd	0,tos
	addr	execs,tos
	addr	0(sp),r1
	addr	@exec,r0
	svc
self:	br	self
execs:	.byte	'/etc/init', x'0
_szicode::
	.double	*-_icode


;Panic routine.  After saving relevant registers and other data into the
;panic structure, it calls the higher level cpanic routine.  This allows
;easy examination of system dump files.  The panic block is pointed to
;by location PANICADDR, so it is easy to find it in a dump.

_panic::
	addqd	1,_pblk + PS_NEST	;add to nesting level
	cmpqd	1,_pblk + PS_NEST	;first time?
	bne	nonest			;nope, don't overwrite original panic
	movd	r0,_pblk + PS_GEN	;first general register
	addr	_pblk,r0		;point at panic block
	movd	r1,(PS_GEN+04)(r0)	;save remaining general registers
	movd	r2,(PS_GEN+08)(r0)
	movd	r3,(PS_GEN+12)(r0)
	movd	r4,(PS_GEN+16)(r0)
	movd	r5,(PS_GEN+20)(r0)
	movd	r6,(PS_GEN+24)(r0)
	movd	r7,(PS_GEN+28)(r0)
	movd	0(sp),PS_PC(r0)		;pc of caller
	movd	4(sp),PS_PSRMOD(r0)	;psr and mod of caller
	sprd	sp,r1			;current stack pointer
	addd	12,r1			;adjust for this call
	movd	r1,PS_SP(r0)		;caller's stack pointer
	sprd	fp,PS_FP(r0)		;caller's frame pointer
	sprd	intbase,PS_INTBASE(r0)	;interrupt base
	smr	msr,PS_MSR(r0)		;mmu status register
	smr	eia,PS_EIA(r0)		;mmu eia register
	smr	ptb0,PS_PTB0(r0)	;mmu ptb0 register
	smr	ptb1,PS_PTB1(r0)	;mmu ptb1 register
	smr	bpr0,PS_BPR0(r0)	;first mmu breakpoint register
	smr	bpr1,PS_BPR1(r0)	;second mmu breakpoint register
	movd	8(sp),PS_STRING(r0)	;address of panic string
	addr	12(sp),PS_ARGS(r0)	;address of argument list
nonest:	addr	12(sp),tos		;push argument list on stack
	movd	12(sp),tos		;push panic string for cpanic
	cxp	_cpanic			;call higher level code
	bpt				;should not get here
