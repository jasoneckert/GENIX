;
; icu.s: version 3.12 of 8/22/83
; 
;
; @(#)icu.s	3.12 (NSC) 8/22/83
;

.program

#include "../h/psr.h"
#include "../h/trap.h"
#include "../h/devvm.h"
#include "../h/icu.h"

#define PSR	11(fp)	/* location of the psr on the stack */


; Interrupt module. Sets up the interrupt controller, all trap and
; interrupt vectors, and enables interrupts
;
; ----------------------------------------------------------------------
;

#if NBM > 0
KEYBD:		.equ		3		; kbd status bit
VSYNC:		.equ		2		; vsync status bit
SCROLL:		.equ		1		; scroll status bit
		.static
_cmr::		.double		0 		; graphic interrupt mask
		.endseg
#endif

		.static
_promintbase::	.double		0		; intbase for prom monitor
_unixintbase::	.double		0		; intbase while unix is running
error:		.double		CP_ErrorReg	; cpu error register
tphase:		.double		CP_TestReg	; cpu test phase reg
dcontrol:	.double		CP_DiagReg	; cpu diagnostic reg

__cnt:		.double		0		; &_cnt
__rsdma::	.double		0		; &_rsdma
_cpl::		.word		0		; current priority level
_icumr::	.word		MICURUPTS	; icu mask register
		.endseg

.import		_cnt

initicu::
modbegin:
/*
 *	Initialize the cpu board diagnostic register. This resets
 *	various FF's on the board etc.
 */
	movqb	1,0(dcontrol)
	movb	h'3f,0(dcontrol)

	addr	_cnt,__cnt		; eliminate xternal refs..
/*
 *	The vector table contains preset displacements, but this mod
 *	register must be installed at runtime
 */

	sprd	intbase,_promintbase	; remember prom monitor's intbase
	addr	_intbase,r2		; remember our own intbase
	addr	@vectors,r0
	sprw	mod,r1
	movd	r2,_unixintbase
loop:
	movw	r1,-4(r2)[r0:d]
	acbb	-1,r0,loop

	cmpqd	0,_kdebug		; check to see if kernel debugging
	beq	unixtraps		; if 0 unix keeps vectors
	movd	_promintbase,r1
	movd	1*4(r1),1*4(r2)		; give nmi trap to ddt
	movd	8*4(r1),8*4(r2)		; give bpt trap to ddt
	movd	9*4(r1),9*4(r2)		; give trc trap to ddt

unixtraps:
	movd	CP_MasterICU,r2		; load master icu address
#ifndef counter
	movb	CCON,ICCTL(r2)		; concatenate high low counter
	movb	FREQ and h'ff,ILCSV(r2)	; set counter starting interval
	movb	(FREQ shr 8) and h'ff,ILCSV+2(r2)
	movb	(FREQ shr 16) and h'ff,ILCSV+4(r2)
	movb	(FREQ shr 24) and h'ff,ILCSV+6(r2)
	movb	FREQ and h'ff,ILCCV(r2)	; set counter starting interval
	movb	(FREQ shr 8) and h'ff,ILCCV+2(r2)
	movb	(FREQ shr 16) and h'ff,ILCCV+4(r2)
	movb	(FREQ shr 24) and h'ff,ILCCV+6(r2)
#endif
	movb	COUTD+NTAR,IMCTL(r2)	; disable interrupt sampling
	movqb	0,ICCTL(r2)		; halt counters
	movb	WENH+WENL,ICICTL(r2)	; disable counter interrupts
	movqb	-1,ICIPTR(r2)		; assign counters vector 15
	movqb	-1,IPS(r2)		; select interrupt for i/o ports
	movqb	-1,IPDIR(r2)		; set port direction to input
	movqb	0,IOCASN(r2)		; set counter wave assignment
	movqb	0,IPDAT(r2)		; initialize port data
	movqd	0,ICSRC(r2)		; clear cascade regs.
	movb	MICUBIAS,ISVCT(r2)	; set vector bias
	movb	h'40,IPND(r2)		; dismiss pending interrupts
	movb	h'40,IPND+2(r2)		; dismiss pending interrupts
	movb	ELTG and h'ff,IELTG(r2)	; set trigger mode
	movb	(ELTG shr 8) and h'ff,IELTG+2(r2)
	movb	TPL and h'ff,ITPL(r2)	; set trigger polarity
	movb	(TPL shr 8) and h'ff,ITPL+2(r2)
	movqb	0,IFPRT(r2)		; set first priority
	movb	_icumr,IMASK(r2)	; set interrupt mask
	movb	_icumr+1 ,IMASK+2(r2)

	lprd	intbase,_unixintbase	; establish new intbase
	movqb	NTAR,IMCTL(r2)		; enable interrupt sampling
	movqd	7,r1			; set processor level to 7
	bsr	spl
	rxp	0			; all done.....

;;
;;	Enable/Disable an interrupt at the icu
;;

_icuenable::
	movd	CP_MasterICU,r0
	bicpsrw	psr_i
	bicw	8(sp),_icumr		; enable local mask
	movb	_icumr,IMASK(r0)	; tell icu...
	movb	_icumr+1,IMASK+2(r0)
	bispsrw	psr_i
	rxp	0

_icudisable::
	movd	CP_MasterICU,r0
	bicpsrw	psr_i
	orw	8(sp),_icumr		; disable local mask
	movb	_icumr,IMASK(r0)	; tell icu...
	movb	_icumr+1,IMASK+2(r0)
	bispsrw	psr_i
	rxp	0

/***** ALL CURRENTLY UNDEFINED INTERRUPTS *****/

v0::	enter	[r0,r1,r2],4
	movqd	0,r1
	br	vund
v2::	enter	[r0,r1,r2],4
	movqd	2,r1
	br	vund
v5::	enter	[r0,r1,r2],4
	movqd	5,r1
	br	vund
v6::	enter	[r0,r1,r2],4
	movqd	6,r1
	br	vund
v7::	enter	[r0,r1,r2],4
	movqd	7,r1
	br	vund
v8::	enter	[r0,r1,r2],4
	movzbd	8,r1
	br	vund

#if NBM == 0
vgraf::
#endif NBM
#if NRS == 0
vtx::
vrx::
#endif NRS
#if NSIO == 0
vsio::					; SIO board interrupt
#endif NSIO
#if NLP == 0
vpioa::					; pio a interrupt
vpiob::					; pio b interrupt
#endif NLP

vund::
bpund::
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movzwd	_cpl,-4(fp)		; save previous level
	movqw	7,_cpl			; spl7()
	movqb	1,@CP_MasterICU+ISRV
	bispsrw	psr_i
	movd	r1,tos			; pass interrupt vector
	cxp	_undint
	adjspb	-4
	br	ixit

_interrupt::				; generate a software int
	movd	CP_MasterICU,r1
	movw	8(sp),r0
	orb	h'80,r0
	movb	r0,IPND(r1)
	movb	r0,IPND+2(r1)
	rxp	0

_spl0::
	movqd	0,r1			; load priority level
	br	xspl
_spl1::
	movqd	1,r1			; load priority level
	br	xspl
_spl2::
	movqd	2,r1			; load priority level
	br	xspl
_spl3::
	movqd	3,r1			; load priority level
	br	xspl
_spl4::
	movqd	4,r1			; load priority level
	br	xspl
_spl5::
	movqd	5,r1			; load priority level
	br	xspl
_spl6::
	movqd	6,r1			; load priority level
	br	xspl
_spl7::
	movqd	7,r1			; load priority level
	br	xspl
_splx::
	movzwd	8(sp),r1		; fetch new priority level
	; fall into xspl ....
;;
;;	Set "processor level".
;;
;;	This routine maps the unix idea of processor level
;;	to a value for the in service register for the icu.
;;	This is used by the externally called routines.
;;	
;;	Input:  r1 contains new "processor level"
;;	Returns: r0 containing previous "processor level"
;;

xspl:
	addr	@CP_MasterICU+ISRV,r2	; point to register base
	movzwd	_cpl,r0			; return old priority
	bicpsrw	psr_i			; disable interrupts
	movw	r1,_cpl			; save new level
	movw	pl_isr[r1:w],r1		; fetch isr for this level
	movb	r1,0(r2)		; set isr register in icu
	lshw	-8,r1
	movb	r1,2(r2)
	bispsrw	psr_i			; enable interrupts
	rxp	0

/*
**	This table maps the Unix notion of processor level into
**	the icu notion of an interrupt in service. This is done
**	because Unix has a concept of multiple devices with the
**	same interrupt priority, and the icu assigns a different
**	priority to each device. The way we fake this is by grouping
**	devices into clusters of "processor levels", and raising
**	the level of a interrupting device to it's highest value
**	in the cluster. We raise the level by changing the value
**	of the in service register to appear that the highest
**	priority interrupt in the interrupt cluster is executing.
*/

	.align	4
pl_isr:
	.word	ISR_LEVEL0
	.word	ISR_LEVEL1
	.word	ISR_LEVEL2
	.word	ISR_LEVEL3
	.word	ISR_LEVEL4
	.word	ISR_LEVEL5
	.word	ISR_LEVEL6
	.word	ISR_LEVEL7

/*
**	This routine is the internally used version of the above
**	routine and is bsr'ed to instead of cxp'ed
*/

spl:
	addr	@CP_MasterICU+ISRV,r2	; point to register base
	movzwd	_cpl,r0			; return old priority
	bicpsrw	psr_i			; disable interrupts
	movw	r1,_cpl
	movw	pl_isr[r1:w],r1		; fetch isr for this level
	movb	r1,0(r2)
	lshw	-8,r1
	movb	r1,2(r2)
	bispsrw	psr_i			; enable interrupt
	ret	0

_idle::
	movqd	0,r1			; r0 = spl0()
	bsr	spl

	wait

	movd	r0,r1
	bsr	spl			; splx(r0)
	rxp	0

/***********************************************************************
**
**	The interrupt handlers
**	They switch out to the C interrupt routines
**
***********************************************************************/

.import _dcuintr, _tcuintr, _undint
.import	_hardclock, _softclock

#define LEVEL0	h'80
#define LEVEL1	h'80
#define LEVEL2	h'80
#define LEVEL3	h'80
#define LEVEL4	h'40
#define LEVEL5	h'08
#define LEVEL6	h'02
#define LEVEL7	h'01

vtimer::
	enter	[r0,r1,r2],4
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movw	_cpl,r0
	movzwd	r0,-4(fp)		; save the old processor level
	movqw	6,_cpl			; set new level
	bispsrw	psr_i			; now running at new level
	movw	10(fp),tos		; pass the psr to clock
	movw	r0,tos			; and processor level
	movd	4(fp),tos		; pass the interrupt pc to clock
	cxp	_hardclock
	adjspb	-8
	br	ixit

vsoft::				/* the software clock interrupt */
	enter	[r0,r1,r2],4
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movw	_cpl,r0
	movzwd	r0,-4(fp)		; save the old processor level
	movqw	1,_cpl			; set the new level
	bispsrw	psr_i			; enable ints
	movw	10(fp),tos		; pass the psr to clock
	movw	r0,tos			; and processor level
	movd	4(fp),tos		; pass the interrupt pc to clock
	cxp	_softclock
	adjspb	-8
	br	ixit

vdisk::					; disk interrupt
	enter	[r0,r1,r2],4
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movzwd	_cpl,-4(fp)		; save the old processor level
	movqw	5,_cpl			; set the new level
	bispsrw	psr_i			; now running at new level
	cxp	_dcuintr
	br	ixit

vtape::					; tape interrupt
	enter	[r0,r1,r2],4
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movzwd	_cpl,-4(fp)		; save the old processor level
	movqw	5,_cpl			; set the new processor level
	movb	LEVEL5,@CP_MasterICU+ISRV ; raise us to highest level 5
	bispsrw	psr_i			; now running at new level
	cxp	_tcuintr
	br	ixit

#if NRS > 0
.import	_rsrint,_rsxint
vrx::					; usart rx interrupt
	enter	[r0,r1,r2],4
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movzwd	_cpl,-4(fp)		; save the old processor level
	movqw	4,_cpl			; set the new processor level
	movb	LEVEL4,@CP_MasterICU+ISRV ; raise us to highest level 4
	bispsrw	psr_i			; now running at new level
	cxp	_rsrint
	br	ixit			; done...

vtx::					; usart tx interrupt
	enter	[r0,r1,r2],4
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movd	__rsdma,r0		; r0 = &rsdma
	movd	P_MEM(r0),r2		; r2 = pd->p_mem
	cmpd	r2,P_END(r0)		; p_mem < p_end ?
	bhs	rspcall			; no, go call rsxint
	movd	P_DEV(r0),r1		; r1 = device address
	movb	0(r2),0(r1)		; dev->data = *p_mem
	addr	1(r2),P_MEM(r0)		; p_mem++;
	exit	[r0,r1,r2]		; restore the user temporary regs
	reti				; GET OUT FAST
rspcall:
	movzwd	_cpl,-4(fp)		; save the old processor level
	movqw	4,_cpl			; set the new processor level
	movb	LEVEL4,@CP_MasterICU+ISRV ; raise us to highest level 4
	bispsrw	psr_i			; now running at new level
	movd	P_ARG(r0),tos		; push tty address
	cxp	_rsxint			; call interrupt rtn
	adjspb	-4
	br	trapxit			; done...
#endif NRS

#if NSIO > 0
.import _sio_int
vsio::
	enter	[r0,r1,r2],4
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movzwd	_cpl,-4(fp)		; save the old processor level
	movqw	4,_cpl			; set the new processor level
	movb	LEVEL4,@CP_MasterICU+ISRV ; raise us to highest level 4
	bispsrw	psr_i			; now running at new level
	movqd	0,tos			; pass board #
	cxp	_sio_int
	adjspb	-4
	br	ixit			; done...
#endif NSIO

#if NBM > 0
.import	_bmrint, _bmxint
vgraf::					; graphics interrupt
	enter	[r0,r1,r2],4
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movzwd	_cpl,-4(fp)		; save the old processor level
	movqw	4,_cpl			; set the new processor level
	movb	LEVEL4,@CP_MasterICU+ISRV ; raise us to highest level 4
	bispsrw	psr_i			; now running at new level
	movb	@GB_IntStatus,r1	; get sib status
	movqb	0,@GB_IntClear		; dismiss sib rupt
	andb	_cmr,r1			; mask out unwanted interrupts
	tbitb	SCROLL,r1
	bfc	trykeybd

	cxp	_bmxint
	bicpsrw	psr_i			; in case we were lowered...
	movqw	4,_cpl			; set the new processor level
	movb	LEVEL4,@CP_MasterICU+ISRV ; raise us to highest level 4
	bispsrw	psr_i			; now running at new level

trykeybd:	
	movb	@GB_IntStatus,r1	; get sib status
	andb	_cmr,r1			; mask out unwanted interrupts
	tbitb	KEYBD,r1
	bfc	ixit
	cxp	_bmrint
	br	ixit
#endif NBM

#if NLP > 0
.import	_lpintr, _lperrint
vpiob::
	enter	[r0,r1,r2],4
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movzwd	_cpl,-4(fp)		; save the old processor level
	movqw	4,_cpl			; set the new processor level
	movb	LEVEL4,@CP_MasterICU+ISRV ; raise us to highest level 4
	bispsrw	psr_i			; now running at new level
	cxp	_lperrint
	br	ixit			; done...

vpioa::
	enter	[r0,r1,r2],4
	addqd	1,V_INTR(__cnt)		; increment interrupt total
	movzwd	_cpl,-4(fp)		; save the old processor level
	movqw	4,_cpl			; set the new processor level
	movb	LEVEL4,@CP_MasterICU+ISRV ; raise us to highest level 4
	bispsrw	psr_i			; now running at new level
	cxp	_lpintr
	br	ixit			; done...
#endif NLP

#define VECTOR(label).double	((label)-modbegin) shl 16

	.align	4
_intbase::		; the trap base, intbase points HERE....
	VECTOR(nvi)
	VECTOR(nmi)
	VECTOR(abt)
	VECTOR(fpu)
	VECTOR(ill)
	VECTOR(svc)
	VECTOR(dvz)
	VECTOR(flg)
	VECTOR(bpt)
	VECTOR(trc)
	VECTOR(und)

	VECTOR(res)	; reserved traps 11-15
	VECTOR(res)	; reserved traps 11-15
	VECTOR(res)	; reserved traps 11-15
	VECTOR(res)	; reserved traps 11-15
	VECTOR(res)	; reserved traps 11-15

	VECTOR(v0)	; undefined vector	-- 0
	VECTOR(vtimer)	; timer vector		-- 1
	VECTOR(v2)	; undefined vector	-- 2
	VECTOR(vdisk)	; disk vector		-- 3
	VECTOR(vtape)	; tape vector		-- 4
	VECTOR(v5)	; undefined vector	-- 5
	VECTOR(v6)	; gpib vector		-- 6
	VECTOR(v7)	; undefined vector	-- 7
	VECTOR(v8)	; undefined vector	-- 8
	VECTOR(vrx)	; usart tx vector	-- 9
	VECTOR(vtx)	; usart tx vector	--10
	VECTOR(vgraf)	; graphics vector	--11
	VECTOR(vpiob)	; pio port b vector	--12
	VECTOR(vpioa)	; pio port a vector	--13
	VECTOR(vsio)	; sio board vector	--14
	VECTOR(vsoft)	; software clock	--15

vectors:	.equ	(*-_intbase)/4	; # vectors to init

/***********************************************************************
**
**	The trap handlers
**	They switch out to the C trap routine
**
***********************************************************************/

abt::
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)		; save processor level
	bispsrw	psr_i			; re-enable interrupts
	addr	@ABTTRAP,r1
	br	alltraps
und::
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)		; save processor level
	addr	@UNDTRAP,r1
	br	alltraps
ill::
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)		; save processor level
	addr	@ILLTRAP,r1
	br	alltraps
dvz::
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)		; save processor level
	addr	@DVZTRAP,r1
	br	alltraps
bpt::
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)		; save processor level
	addr	@BPTTRAP,r1
	br	alltraps
trc::
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)		; save processor level
	addr	@TRCTRAP,r1
	br	alltraps
fpu::
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)		; save processor level
	addr	@FPUTRAP,r1
	br	alltraps
flg::
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)		; save processor level
	addr	@FLGTRAP,r1
	br	alltraps
nvi::					; non vectored interrupt
	enter	[r0,r1,r2],4
	addr	@NVITRAP,r1
	br	alltraps
.static
_erreg::	.blkw
.endseg

nmi::
	movb	0(error),0(tphase)	; note occurrence of err...
	movqw	0,_erreg		; in the cpu board lights
	comb	0(error),_erreg		; save it away
	movqb	1,0(dcontrol)		; and re-initialize
	movb	h'3f,0(dcontrol)
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)		; save processor level
	bispsrw	psr_i			; re-enable interrupts
	addr	@NMITRAP,r1
	br	alltraps
res::
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)		; save processor level
	addr	@RESTRAP,r1
	br	alltraps
segv::
	enter	[r0,r1,r2],4		; save processor level
	movzwd	_cpl,-4(fp)
	addr	@SEGVTRAP,r1
	br	alltraps

/*---------------------------------------------------------------------
	The layout of the stack after every trap or interrupt
	before going to the handlers:

	UPS
	UPC
	UFP	<- KFP
	PPL	(previous processor level)
	UR0
	UR1
	UR2
	USP
	TYP	<-KSP

---------------------------------------------------------------------*/

.import _runrun
.import	_syscall, _trap

svc::				/* Supervisor call */
	enter	[r0,r1,r2],4
	movzwd	_cpl,-4(fp)	; save current ipl
	bispsrw	psr_s		; switch to usp
	sprd	sp,r0		; fetch usp
	bicpsrw	psr_s		; back to ksp
	movd	r0,tos		; push usp
	addr	@SVCTRAP,tos	; ... and traptype

	cxp	_syscall	; call the system
	addqd	1,_cnt+V_SYSCALL; bump syscall count
	adjspb	-4		; pop traptype

	movd	tos,r0		; restore the user sp
	bispsrw	psr_s		; back to usp
	lprd	sp,r0		; restore it
	bicpsrw	psr_s		; back to ksp

	cmpqb	0,_runrun	; need to reschedule?
	beq	trapxit		; nope

resched:
	addr	@SWTCHTRAP,r1	; yep ... trap(SWTCH)

		    ; FALL INTO TRAP COMMON CODE

bptrap::
alltraps:
	bispsrw	psr_s		; switch to usp
	sprd	sp,r0		; fetch usp
	bicpsrw	psr_s		; back to ksp
	movd	r0,tos		; push usp
	movd	r1,tos		; ... and traptype

	cxp	_trap		; call trap handler
	addqd	1,_cnt+V_TRAP	; bump trap count
	adjspb	-4		; pop traptype

	movd	tos,r0		; restore the user sp
	bispsrw	psr_s		; back to usp
	lprd	sp,r0		; restore it
	bicpsrw	psr_s		; back to ksp
ixit:
	tbitb	0,PSR		; ****** are we returning to user mode?
	bfc	trapxit		; nope... just exit
	cmpqb	0,_runrun	; need to reschedule?
	bne	resched		; yep

/*
**	At this point we are committed to restarting the user. This
**	must be done without interruption. The processor priority
**	level is restored manually instead of with spl for this
**	reason.
*/

bptrapxit::
trapxit:
	bicpsrw	psr_i		; disable interrupts
	movzwd	-4(fp),r0	; fetch previous priority level
	movw	r0,_cpl		; restore cpl
	movw	pl_isr[r0:w],r0		; fetch isr for this level
	movb	r0,@CP_MasterICU+ISRV	; load the isr
	lshw	-8,r0
	movb	r0,@CP_MasterICU+ISRV+2
	exit	[r0,r1,r2]	; restore the user temporary regs
 	rett	0		; restart the user....
	br	segv		; cannot, go cause segment fault

.endseg
