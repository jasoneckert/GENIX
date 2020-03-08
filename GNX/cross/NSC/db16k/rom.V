;
; rom.s: version 1.1 of 3/31/83
; 
;
; @(#)rom.s	1.1 (NSC) 3/31/83
;
.program
.module dbmon
;
; Remote DDT
; ------ ---
;
; Ross Harvey/Duncan Gurley
;
;--------------------------------------------------------------------------
;		HOW TO BUILD A NON MMU MONITOR
;
;	For a NON-MMU monitor, search for NOMMU, this will find
;	MMU dependent lines, which have comments for what to comment out.
;	Also don't use pv[bwd],cv[bvd],q,x#,r#,w# commands(they will E UND)
;	and when using DDT, use -n flag.
;--------------------------------------------------------------------------
;
; 16032 psr configuration
;
 psr_c:		.equ	0		; carry
 psr_t:		.equ	1		; trace
 psr_l:		.equ	2		; lower
 psr_f:		.equ	5		; flag
 psr_z:		.equ	6		; zero
 psr_n:		.equ	7		; negative
 psr_u:		.equ	8		; user/supervisor
 psr_s:		.equ	9		; sp0/sp1
 psr_p:		.equ	10		; pending
 psr_i:		.equ	11		; ion
;
; 16082 msr configuration
;
 msr_tu:	.equ	16		; msr user translate
 msr_ts:	.equ	17		; msr supr translate
 msr_ds:	.equ	18		; msr dual space
 msr_nt:	.equ	25		; msr dual space
;
; Configuration
;
 base:		.equ	0		; load address (Basevalue is zero)
 ramstart:	.equ	h'8000		; start of ram on db16000 board
 ram8:		.equ	2*30*1024	; istack and ustack are defined
 ram9:		.equ	2*32*1024	; ...with these
 ustack:	.equ	ram8		; user stack address (default)
 istack:	.equ	ram9		; interrupt stack address (default)
;
; the input line buffer
;
 ibufsize:	.equ	64
 ibuffer:	.equ	-ibufsize(fp)
;
; ascii fun
;
 bs:		.equ	h'8		; code for backspace
 lf:		.equ	h'a		; code for line feed
 cr:		.equ	h'd		; code for carriage return
 esc:		.equ	h'1b		; code for escape
 dc1:		.equ	h'11		; code for control/q
 dc3:		.equ	h'13		; code for control/s
;
; magic context specifiers - c_byte is known to be exactly zero
;
 c_byte:	.equ	0		; byte context was specified (or none)
 c_word:	.equ	1		; word context was specified
 c_dble:	.equ	2		; double words needed
;
; Need to rip off some low ram for a module table entry. We use module
; one.
;
 rddtmod:	.equ	16		; the address of our module table entry
;
; boot switch options
;
 opt_lfecho:	.equ	1		; bit number of echo option switch (lf)
 opt_echo:	.equ	1		; bit number of echo option switch (all)
;
; these bits are the same for the 8251 and 2651
;
 txrdy: 	.equ	0		; status/transmitter ready bit number
 rxrdy:		.equ	1		; status/receiver ready bit number
 frame:		.equ	5		; status/framing error present
 siocom:	.equ	h'37		; rts low/dtr low/rcvr+xmitr enable
;
; *******************************	Note that all labels are exported
; * These exports must be first *	except for .equ names.  Don't put
; *******************************	a label before these things.
;
; The link page is used as the interrupt vector table, so we wouldn't
; ...want any entries to precede these...
;
.export	nvi,nmi,abt,fpu,ill		; use the link page for intbase
.export	svc,dvz,flg,bpt,trc,und
.export start
start:	
; NOMMU	if we don't have an MMU, should make the next line a setcfg
; with NO m flag. So we will get a Undefined Trap if we try 
; any MMU slave instructions.
; NOTE: setcfg instruction can still exist with other flags(i,f,c)
	setcfg	m			; We know we have an mmu around
	movd	ram9 shr 2,r0		; mem size
	movd	ramstart,r1		; initialize memory 
	addr	4(r1),r2		; clear all memory
	movqd	0,0(r1)			; clear first double
	movsd				; <move>
	lprd	fp,istack 		; supervisor fp (don't actually need...
	lprd	sp,istack		; ...this yet, it's reset at mstart)
	lprd	intbase,@base+rddtmod+4	; use the link page for the vector table
;
; set up context with the user stack
;
	lprw	psr,1 shl psr_s
	lprd	sp,ustack
;
; start up the serial I/O port
;
	movqd	uscount,r0		; get usart initialization count
sfun:	movzbd	usfirst-1[r0:b],o_siocmd(sio); program usart
	acbd	-1,r0,sfun		;
	addr	hello,tos		; print the banner
	bsr	wstrnul			;
	svc				; "start me up" -M. Jagger.

;
; peek - look at the byte that will be read next
;
; returns:
;	r0
;
peek:	movzbd	0(r7),r0		; what's next?
ret0:	ret	0			; 
;
; readbyte - get the next byte from the line buffer. Won't read past <nul>.
;
; returns:
;	r0:	zero-extended lower case input byte
;
readbyte:
	movzbd	0(r7),r0			; just grab the byte
	cmpqb	0,r0				; ...and if it's not
	beq	read1				; ...nil, then
	addqd	1,r7				; ...increment the pointer
read1:	ret	0
;
; readline - read a line and convert to lower case
;
; the echo options are implemented here
;
; uses:	r0
;
bytefun:	.byte	'Z','A'			; checkb instruction operand
erase1:		.byte	bs,' ',bs,0		; character erase string
input:		.byte	cr,bs,esc,lf,'@'	; magic character list
 inlen:		.equ	*-input
chardisp:	.byte	c_none-c_go
		.byte	c_at-c_go,rb1-c_go,c_esc-c_go,c_bs-c_go,c_cr-c_go
;
readline:
	save	[r1,r4]
	movxbd	-ibufsize,r7			; initialize loop counter
rb1:	bsr	ibyte				; get an 8-bit byte
	extsd	r0,r4,0,7			; put a 7-bit ascii # in r4
	movqd	inlen,r0			; get length of magic list
	addr	input,r1			; get addr of magic list
	skpsbu					; search magic char list
c_go:	caseb	chardisp[r0:b]			; branch on chars left in list
c_none:	cmpb	r4,' '				; not magic, check for (other)
	blt	rb1				; ...control chars (2B ignored)
rb2:	movb	r4,ibuffer+ibufsize[r7:b]	; install it in the line buffer
	bsr	echo1				; echo for the echo option
rb3:	acbd	1,r7,rb1			; at end of buffer?
	movqd	-1,r7				; don't overwrite the stack
	movb	cr,r4				; set up for cr echo
c_cr:	movqb	0,ibuffer+ibufsize[r7:b]	; add a nul for cr or ovrfl
	bsr	echo1				; might want to echo the cr
	tbitb	opt_lfecho,e_echo(fp)		; echo <lf> after <cr>??
	bfc	rl2				; f clear means no
	movzbd	lf,tos				; call writebyte with an lf
	bsr	writebyte			; what fun
rl2:	addr	ibuffer,r7			; done, set input pointer
	restore	[r1,r4]
	ret	0
c_esc:	br	restart				; an escape aborts the command
c_at:	movqw	0,e_echo(fp)			; disable echo for @lines
	br	rb1
c_bs:	cmpb	-ibufsize,r7			; check for empty buffer
	bge	rb1				; if empty, branch back to top
	tbitb	opt_echo,e_echo(fp)		; echoing??
	bfc	rb2a0				; ...if not, don't try erasing
	addr	erase1,tos			; otherwise, erase the byte
	bsr	wstrnul				; with the erase string
rb2a0:	addqd	-1,r7				; correct the buffer pointer
	br	rb1				; ...and try for another
;
; echo1: echo r4 if opt_echo enabled
;
echo1:	tbitb	opt_echo,e_echo(fp)		; echoing?
	bfc	echo2				; no
	movd	r4,tos				; echo r4
	bsr	writebyte
echo2:	ret	0
;
; wstrnul - write a string up to a null byte
;
; uses:	r0,r1
;
wstrnul:adjspb 	4			; write string until nul
	movmd	4(sp),tos,2		; ...make room for the len param
	movqd	-1,8(sp)		; ...add a large length and fall thru
;
; wstrlen - write a string up to a count or a null byte
;
; uses: r0,r1
;
wstrlen:movd	8(sp),r0		; r0 = count
	movd	4(sp),r1		; r1 = addr
wstr1:	cmpqb	0,0(r1)			; don't write past <nul>
	beq	wstr2			; just return if one is found
	movzbd	0(r1),tos		; get a byte
	addqd	1,r1			; select the next one
	bsr	writebyte		; what fun, write it out
	acbd	-1,r0,wstr1		; apply the count test
wstr2:	ret	8
;
; writebyte - write a byte
;
writebyte:
	tbitb	rxrdy,o_status(sio)	; first check the receiver
	bfc	wb1			; receiver is clear, continue
	movb	o_data(sio),tos		; not clear, grab the byte
	cmpb	dc3,0(sp)		; control/s?
	bne	wb0			; no
	bsr	skipdc1			; yes, wait for a control/q
wb0:	cmpb	esc,tos			; receiver ready!? check for <esc>
	beq	restart			; if <esc>, abort this command
wb1:	tbitb	txrdy,o_status(sio)	; otherwise, try the transmitter again
	bfc	writebyte		; ...and...
qqq:	movb	4(sp),o_data(sio)	; ...if ready, write the byte
	ret	4			; ...and get on with it
;
; interrupt entry vectors
;
nvi:	movqd	-7,tos
	br	l
nmi:	movqd	-6,tos
	br	l
abt:	movqd	-5,tos
	br	l
fpu:	movqd	-4,tos
	br	l
ill:	movqd	-3,tos
	br	l
svc:	movqd	-2,tos
	br	l
dvz:	movqd	-1,tos
	br	l
flg:	movqd	0,tos
	br	l
bpt:	movqd	1,tos
	br	l
trc:	movqd	2,tos
	br	l
und:	movqd	3,tos
l:	bicpsrw	1 shl psr_i		; turn off interrupts
	xord	0(sp),r0		; exchange r0 with tos
	xord	r0,tos			; ...
	xord	0(sp),r0		; now have r0=vector-##, tos=old_r0
	save	[r1,r2,r3,r4,r5,r6,r7]	; save the rest of the general reg's
	sprd	intbase,tos		; save the processor regs...
	sprw	psr,r1			; push the usp, sigh,	1. save psr
	bispsrw	1 shl psr_s		;			2. select usp
	addr	tos,r3			;			3. get level
	lprw	psr,r1			;			4. restore psr
	movd	r3,tos			;			5. push the usp
	sprd	fp,tos			; When returning,
	addr	-(e_fp-e_begin)(sp),r1	; ...the new registers are popped
	lprd	sp,r1			; ...off the stack and changes
	lprd	fp,r1			; ...take effect.
	movzwd	e_mod0(fp),e_mod32(fp)	; make a 32-bit copy
	addr	e_frame(fp),e_is(fp)	; calculate what the i-stack level was
	movd	0(e_mod32(fp)),e_sb(fp)	; get the sb out of the mod table
	movzwd	e_psr0(fp),e_psr32(fp)	; make a 32-bit copy
	movd	e_psr32(fp),e_fsr32(fp)	; make fake copy for address translate
	br	mstart

nonseq:	smr	msr,tos
	sbitb	msr_nt,0(sp)
	movd	tos,r4
	cbitb	psr_t,e_psr32(fp)	; (here to go) clear the trace bit
	br	ntby
mret:	cbitb	psr_t,e_psr32(fp)
sret:
; NOMMU		don't need these next 2 lines
	smr	msr,tos
	movd	tos,r4
ntby:	movw	e_psr32(fp),e_psr0(fp)
	movw	e_mod32(fp),e_mod0(fp)	; ...in case they've been changed
	addr	e_fp(fp),r1		;
	lprd	sp,r1			; it doesn't work to change the
	lprd	fp,tos			; ...sb or is
	sprw	psr,r1			; pop the usp, sigh,	1. save psr
	movd	tos,r3			;			2. get saved us
	bispsrw	1 shl psr_s		;			3. select usp
	lprd	sp,r3			;			4. load usp
	lprw	psr,r1			;			5. restore psr
	lprd	intbase,tos		; processor register changes in effect
; NOMMU		don't need next line
	lmr	msr,r4			; restore mmu for non seq trap
	restore	[r0,r1,r2,r3,r4,r5,r6,r7]; general register changes in effect
	rett	0			;   restart the user program
;
; various randomness
;
; case dispatch for commands
;
casefun:.word	quest-case		; r0=0, skpsbu `until' condition not met
	.word	exclaim-case		; '!' - comment
	.word	exclaim-case		; <0> - comment (from <cr>)
	.word	go-case			; 'g' - go
	.word	step-case		; 's' - single step
	.word	image-case		; 'i' - image (binary) load
	.word	xconfig-case		; 'x' - execute setcfg instruction
	.word	wmmu-case		; 'w' - write mmu register
	.word	rmmu-case		; 'r' - read mmu register
	.word	re1-case		; 'c' - change
	.word	re1-case		; 'p' - print
	.word	load-case		; 'l' - (hex ascii) load
	.word	dump-case		; 'd' - dump memory
	.word	fill-case		; 'f' - fill memory
	.word	all-case		; 'a' - print all registers
	.word	test-case		; 't' - test
	.word	move-case		; 'm' - move
	.word	verify-case		; 'v' - verify load
	.word	nonseq-case		; 'q' - set nonseq trap mmu
;
comtab:		.byte	'qvmtafdlpcrwxisg!',0; the command character table
 comsize:	.equ	*-comtab	; its size
;
atsign:		.byte	'E ',0		; entry message
questfun:	.byte	'?'		; question mark complaint
crlf:		.byte	lf,cr,0		; ...also used for cr,lf
crlfs:		.byte	lf,cr,'*',lf,cr,0; prompt message
wcrlfs:	addr	crlfs,tos		; prompt subroutine
	bsr	wstrnul			;
	ret	0
exclaim:br	restart			; null command for '!' and <cr>
;
; process commands
; ------- --------
;
; r7: input buffer byte pointer (ends at cr, changed to <nul>)
;
; on entry:
;
; r0: trap type index -7..+3
;
; here to process a command after an exception event
;
mstart:
	movqd	4,tos			; length of trap type message
	addr	trapt+7*4[r0:d],tos	; address of trap type message
	addr	atsign,tos		; do the E first (or lose r0)
	bsr	wstrnul			; write them both
	bsr	wstrlen
;
; here to process a command after a previous command (or fall thru from above)
;
; when restarting, the stack level is reset from fp
;
restart:
	addr	ibuffer,tos		; set stack level and allocate the...
	lprd	sp,tos			; ...input buffer
	bsr	wcrlfs			; write the prompt
	movw	0(options),e_echo(fp)	; copy the echo options in
	bsr	readline		; read a line
	bsr	readbyte		; obtain a command character
	checkb	r1,bytefun,r0		; is it in 'A'..'Z'??
	bfs	testn			; no, add back the 'A'
	addb	'a'-'A',r0		; yes, correct case
testn:  movd	r0,tos			; save the command character
	cmpb	r0,'n'			; see if it the load gpib command
	beq	namecase		; name command don't fold
	cmpb	r0,'h'			; see if it the home dir command
	beq	namecase		; name command don't fold
	movd	r7,r5			; get the readline string
nloop:	movzbd	0(r5),r3		; just grab the byte
	cmpqb	0,r3			; ...and if it's not
	beq	namecase		; stop end of string
	checkb	r1,bytefun,r3		; fold it
	bfs	testn2			; no, add back the 'A'
	addb	'a'-'A',r3		; yes, correct case
testn2:	movb	r3,0(r5)		; install it in the line buffer
	addqd	1,r5
	br	nloop
namecase:movd	r0,r4			; install the until condition
	movzbd	comsize,r0		; get the command table size
	addr	comtab,r1		; get the command table address
	skpsbu				; search: r0 will be the command index
case:	casew	casefun[r0:w]		; branch to it (r0=0 if com. not found)
;
; re1 is the entry point for print and change register/memory commands
;
re1:	bsr	readbyte		; read a register name or a memory
	movw	r0,tos			; ...context: rr or mb, mw, md
	bsr	readbyte		; these are two chars long
	movb	r0,1(sp)		; have the two-char string at tos now
	movzbd	eregt,r1		; get the register table ending address
print1:	cmpw	regt-3[r1:b],0(sp)	; search the reg table for an offset
	beq	print2			; the table will give an fp offset
	acbd	-3,r1,print1		; try the next
	movzbd	evmem,r1		; get the virtual table ending address
vprt1:	cmpw	vmem-3[r1:b],0(sp)	; search the table for an offset
	beq	vprt2			; the table will give an fp offset
	acbd	-3,r1,vprt1		; try the next
	br	quest			; **no name found**
vprt2:	cmpqw	0,tos			; pop the code off the stack
	movzbd	vmem-1[r1:b],r0		; get the fp offset out of the table
	cmpb	r0,e_frame-e_begin	; if >= e_frame, then it's v[bwd]
	bge	vmemfun			; ...so go print/change virtual memory
print2:	cmpqw	0,tos			; pop the code off the stack
	movzbd	regt-1[r1:b],r0		; get the fp offset out of the table
	cmpb	r0,e_frame-e_begin	; if >= e_frame, then it's m[bwd]
	bge	memfun			; ...so go print/change memory
	cmpb	'p',0(sp)		; print or change?
	bne	changer			; go change a register
	movd	0(fp)[r0:b],tos		; go print a register
	bsr	hexout
	br	next
changer:addr	0(fp)[r0:b],tos		; this is where the reg is saved
	bsr	eqhexin			; convert an =hex value
	movd	r0,0(0(sp))		; overwrite the saved register
	br	next			; restart will reset the stack level
;
memfun:	adjspb	16			; allocate tmp/src/dst/context
	subb	e_frame-e_begin,r0	; extract the context
	movxbd	r0,0(sp)		; specify context 0/1/2
	bsr	hexin			; get the address to print or change
	movd	r0,8(sp)		; assume it's a src address
	movqd	0,12(sp)		; clear tmp
	cmpb	'p',16(sp)		; printing?
	beq	mem2			; yes, do this a bit differently
	movd	8(sp),4(sp)		; (src assumption wrong) save dst
	addr	12(sp),8(sp)		; get the real source address
	bsr	eqhexin			; convert the new value
	movd	r0,12(sp)		; save it
	bsr	conmove			; move with context of 0(sp)
	cmpqd	0,tos			; clear source copy of tos
	br	next
mem2:	addr	12(sp),4(sp)		; specify dst as addr(tmp)
	bsr	conmove			; fetch it with the right context
	bsr	hexout			; output!
	br	next
;
vmemfun:adjspb	16			; allocate tmp/src/dst/context
	subb	e_frame-e_begin,r0	; extract the context
	movxbd	r0,0(sp)		; specify context 0/1/2
	bsr	hexin			; get the address to print or change
	movd	r0,8(sp)		; assume it's a src address
	movqd	0,12(sp)		; clear tmp
	cmpb	'p',16(sp)		; printing?
	beq	vmem2			; yes, do this a bit differently
	movd	8(sp),4(sp)		; (src assumption wrong) save dst
	addr	12(sp),8(sp)		; get the real source address
	bsr	eqhexin			; convert the new value
	movd	r0,12(sp)		; save it
	bsr	wvonmove		; move with context of 0(sp)
	cmpqd	0,tos			; clear source copy of tos
	br	next
vmem2:	addr	12(sp),4(sp)		; specify dst as addr(tmp)
	bsr	rvonmove		; fetch it with the right context
	bsr	hexout			; output!
	br	next
;
; conmove(context,dst,src) - move src to dst as per context
;
; uses: r0
;
conmove:movd	4(sp),r0		; get context
mgo:	caseb	movec[r0:b]		; select the appropriate,
movec:	.byte	bytem-mgo
	.byte	halfm-mgo	
	.byte	fullm-mgo
bytem:	movb	0(12(sp)),0(8(sp))
	ret	12
halfm:	movw	0(12(sp)),0(8(sp))	
	ret	12
fullm:	movd	0(12(sp)),0(8(sp))
	ret	12
;
; rvonmove(context,dst,vsrc) - move virtual src to dst as per context
;
;
rvonmove:
	smr	msr,tos
	movd	tos,r0
	tbitb	psr_u,e_fsr32(fp)
	bfc	rsupruser
	tbitb	msr_tu,r0
	bfc	conmove
	br	rdu
rsupruser:
	tbitb	msr_ts,r0
	bfc	conmove
	br	rmptb0
rdu:	tbitb	msr_ds,r0
	bfc	rmptb0
	smr	ptb1,tos
	br	rmptb
rmptb0:	smr	ptb0,tos
rmptb:	movd	tos,r5
;
	movd	4(sp),r0
rgo:	caseb	mover[r0:b]
mover:	.byte	byter-rgo
	.byte	halfr-rgo
	.byte	fullr-rgo
byter:	
	movqd	1,r3
bback:	movd	12(sp),r0
	bsr	physaddr	;return physical address
	cmpqd	0,r0		;check if successful
	bgt	rbadad	
	movd	r0,r2
	smr	msr,tos
	movd	tos,r0
	movd	r0,r4
	andd	h'fff8ffff,r4
	lmr	msr,r4
	movb	0(r2),r1
	lmr	msr,r0
	movb	r1,0(8(sp))
	addqd	1,8(sp)
	addqd	1,12(sp)
	acbd	-1,r3,bback
	ret	12
halfr:	
	movqd	2,r3
	br	bback
fullr:
	movqd	4,r3
	br	bback
rbadad:	movqd	-1,16(sp)
	ret	12
;
;
; wvonmove(context,vdst,src) - move src to virtual dst as per context
;
;
wvonmove:
	smr	msr,tos
	movd	tos,r0
	tbitb	psr_u,e_fsr32(fp)
	bfc	wsupruser
	tbitb	msr_tu,r0
	bfc	conmove
	br	wdu
wsupruser:
	tbitb	msr_ts,r0
	bfc	conmove
	br	wmptb0
wdu:	tbitb	msr_ds,r0
	bfc	wmptb0
	smr	ptb1,tos
	br	wmptb
wmptb0:	smr	ptb0,tos
wmptb:	movd	tos,r5
;
	movd	4(sp),r0
wgo:	caseb	movew[r0:b]
movew:	.byte	bytew-wgo
	.byte	halfw-wgo
	.byte	fullw-wgo
bytew:	
	movqd	1,r3
bbackw:	movd	8(sp),r0
	bsr	physaddr	;return physical address
	cmpqd	0,r0		;check if successful
	bgt	wbadad	
	movd	r0,r2
	movb	0(12(sp)),r1
	smr	msr,tos
	movd	tos,r0
	movd	r0,r4
	andd	h'fff8ffff,r4
	lmr	msr,r4
	movb	r1,0(r2)
	lmr	msr,r0
	addqd	1,8(sp)
	addqd	1,12(sp)
	acbd	-1,r3,bbackw
wbadad:	ret	12
halfw:	
	movqd	2,r3
	br	bbackw
fullw:
	movqd	4,r3
	br	bbackw
;
; physical to virtual address translation
;
physaddr:
	enter	[r1,r5],0
	movd	r0,r6
	cmpd	r6,16777216
	bls	vm1
	br	vmfail
vm1:
	movd	r6,r0
	andd	16711680,r0
	ashd	-16,r0		
	addr	@0[r0:d],r0
	addd	r0,r5
	movd	0(r5),r0
	andd	1,r0	
	cmpd	r0,0
	bne	vm2
	br	vmfail
vm2:
	movd	0(r5),r0
	andd	-512,r0	
	movd	r0,r5	
	movd	r6,r0	
	andd	65024,r0
	ashd	-9,r0
	addr	@0[r0:d],r0
	addd	r0,r5
	movd	0(r5),r0
	andd	1,r0
	cmpd	r0,0	
	bne	vm3
	br	vmfail
vm3:
	movd	0(r5),r0	
	andd	-512,r0
	movd	r6,r1
	andd	511,r1
	ord	r1,r0
	exit	[r1,r5]
	ret	0
vmfail: movqd	-1,r0
	exit	[r1,r5]
	ret	0
;
; next - skip to eol and go again
;
next:	br	restart
;
; eqhexin - convert =x...
; hexin   - convert x...
;
; conversion is halted by (a second) '=', or by a byte < '0'
;
; uses:	r1
; returns:
;	r0
;
eqhexin:bsr	skipblanks		; leading blanks allowed before numbers
	bsr	readbyte		; well, try a byte
	cmpb	'=',r0			; this entry wants an `='
	beq	hexin			; got it
	cmpqb	0,r0			; might be missing
	beq	quest			; so complain
	br	eqhexin			; 
hexin:	bsr	skipblanks		; do this `again', may have entered here
	movqd	0,r1			; entry point for simple conversion
hi1:	bsr	peek			; what's the next byte(?) are we done??
	cmpb	'=',r0			; terminate on `='
	beq	hi9			;
	cmpb	r0,'0'			; terminate on c < '0'
	blt	hi9
	bsr	readbyte		; otherwise, read the byte (again)
	bsr	hex_ins			; convert it and insert it in r1
	br	hi1			; continue
hi9:	movd	r1,r0
	ret	0
;
; hex_ins: convert the hex ascii digit in r0 and shift it into r1
;
lesshex:	.byte	'9','0'		; the operand for the checkb instruction
;
hex_ins:checkb	r0,lesshex,r0		; check for digits, subtract ord('0')
	bfc	hs2			; f clear, was a digit, is now binary
	subb	'a'-'0'-h'a,r0		; was probably alpha, adjust for binary
hs2:	lshd	4,r1			; make 4 bits of room
	inssb	r0,r1,0,4		; insert the new digit
	ret	0
;
;
; hexout - output ``=xxxxxxxx'' with no newline.
; hextwo - almost the same, but with no leading `='
; hexbyteout - convert only one byte, no leading `='
;
; uses:	r0,r1,r2
;
hexbytes:
	.byte	h'f,h'a			; operand for checkb instruction
;
hexout:	movzbd	'=',tos			; write an `='
	bsr	writebyte		;
hextwo:	movd	4(sp),r0		; get the parameter
	movxbd	-28,r1			; r1 is the shift count and loop var
hex1:	bsr	hex2			; do one hex digit
	acbb	4,r1,hex1		; repeat ... until (r1+=4)==0
	bsr	hex2			; do the least significant digit
	ret	4
hexbyteout:				; do only a single byte
	movd	4(sp),r0		; set it up
	movxbd	-4,r1			; need only one loop trip
	br	hex1			; head for the main loop
;
hex2:	movd	r0,r2			; output one hex digit
	lshd	r1,r2			; get digit in r2
	andb	h'f,r2			; interested in the low 4 bits
	checkb	r2,hexbytes,r2		; check for a-f
	bfs	hex3			; no, must be numeric
	addb	'A'-h'a-'0',r2		; yes, add difference between '0' & 'a'
hex3:	addb	h'a+'0',r2		; make it ascii
	movzbd	r2,tos			; what fun, write it out
	bsr	writebyte
	ret	0
;
; skip to dc1 (control/q)
;
skipdc1:
	movd	r0,tos			; save r0
sd2:	bsr	ibyte			; get the next transmitted byte
	cmpb	esc,r0			; we are interested in dc1 and esc
	beq	restart			; could halt it all right here, U know
	cmpb	dc1,r0			; dc1/control q: resume transmission
	bne	sd2			; not dc1, ignore it
	movd	tos,r0			; restore r0
	ret	0
;
; ibyte - read image (binary) byte
;
ibyte:	
;	tbitb	frame,o_status(sio)	; check for break
;	bfs	framed
	tbitb	rxrdy,o_status(sio)	; busy wait for a character
	bfc	ibyte			; ibyte is called by readline
	movb	o_data(sio),r0		; ...and the binary loader
	ret	0
;
; framed - note receipt of break/reset
;
framed:	addr	framedmsg,tos
	bsr	wstrnul
	br	restart
framedmsg:	.byte	cr,lf,'E FRAME',cr,lf,0
;
; write a question mark, skip to eol, branch back to the command loop
;
quest:	addr	questfun,tos
	bsr	wstrnul
	br	next
;
; process a load command:
;
; l addr 11223344cc	nn=hexa byte cc=checksum; first blank is optional
;
; r3: 	next address to load
; r4:	accumulator for checksum byte
;
load:	bsr	hexin			; get the address, with no '='
	movd	r0,r3			; 
	movqd	0,r4			;
	bsr	readbyte		; need a space after the address
	cmpb	' ',r0			; insist
	bne	quest			; ...on it
load1:	bsr	hexbyte			; read and convert a hex byte
	movd	r1,r2			;
	bsr	skipblanks		; skip blanks and peek at the next char
	cmpb	r0,'0'			; if not hex ascii, then we just read
	blt	load2			; ...the checksum, and we're done
	movb	r2,0(r3)		; otherwise, it's data, so store it
	addb	0(r3),r4		; get it back from mem for the checksum
	addqd	1,r3			; select the next address
	br	load1			; continue
load2:	cmpb	r2,r4			; done, compare checksum bytes
	beq	next			; if equal, then exit
	movzbd	r4,tos			; otherwise, need to write a message
	bsr	crcbotch		; ...and the computed check value
	br	next
;
; complain about a checksum
;
; crcbotch(badcheckvalue)...called from the hex and image loader
;
; calls hexout (r0,r1,r2)
;
crcbotch:
	addr	e_crc,tos		; print the crc message
	bsr	wstrnul
	movd	4(sp),tos		; print the computed check value
	bsr	hexout
	ret	4
;
e_crc:	.byte	lf,cr,'E CRC',cr,0	; the crc message
;
; hexbyte - read and covert a hex ascii byte
;
; uses:	r0
; returns:
;	r1
;
hexbyte:bsr	skipblanks		; skip any leading blanks
	movqd	0,r1			; assemble the value in r1
	bsr	readbyte		; get a byte
	bsr	hex_ins			; shift into r1
	bsr	readbyte		; and do it again
	bsr	hex_ins
	ret	0
;
; skipblanks - read over blanks
;
; returns:  r0 - result from peek
;
skipblanks:
	bsr	peek			; will the next byte read be
	cmpb	' ',r0			; ...a blank?
	bne	sb1			; if not, we're done
	bsr	readbyte		; otherwise, read it
	br	skipblanks		; ...and try for another
sb1:	ret	0
;
; step - single step once
;
step:	sbitb	psr_t,e_psr32(fp)	; set the t bit
	br	sret			; begin execution
;
; go - start execution
;
go:	br	mret			; restart with a clear t bit
;
;
;
verify:
	bsr	hexin			; get base address
	movd	r0,r4			; ...in r4
	bsr	hexin			; get length
	movd	r0,r5			; ...in r5
	movqd	0,r1			; accumulate checksum in r1
ver0:	movzbd	0(r4),r0		; get one byte
	addd	r0,r1			; add it in
	addqd	1,r4			; select the next byte
	acbd	-1,r5,ver0		
	movd	r1,tos			; print the computed checksum
	bsr	hexout
	br	next
;
; image - binary load
;
image:	bsr	asm32			; get the load address
	movd	r1,r2			; ...(in r2)
	bsr	asm32			; get the length (r1)
	movqd	0,r3			; initialize checksum
	addqd	1,r1			; do any zero length loads correctly
	br	i3			; ...by adding one and skipping one xfer
i2:	bsr	ibyte			; get the next byte
	movb	r0,0(r2)		; store it in memory
	addqd	1,r2			; select the next address
	addb	-1(r2),r3		; accumulate checksum from memory
i3:	acbd	-1,r1,i2		; loop fun
	bsr	ibyte			; get remote checksum
	cmpb	r0,r3			; compare with local checksum
	beq	i4			; if equal, we're done
	movzbd	r3,tos			; otherwise, should prob. say something
	bsr	crcbotch		; note the problem and the value
i4:	br	restart
;
; assemble a 32 bit word, least significant byte first
;
; uses:		r0
; returns:	r1
;
asm32:	movqd	0,r1			; accumulate value in r1
	movqw	4,tos			; it takes four bytes
asm32a:	bsr	ibyte			; get one of them
	movb	r0,r1			; put in in r1<7:0>
	rotd	-8,r1			; adjust significance of bytes
	acbw	-1,tos,asm32a		; loop fun
	cmpqw	0,tos			; pop off the loop counter
	ret	0			;
;
; the fill command
;
; r1 - fill byte
; r2 - beginning (next) address
; r3 - ending address
;
fill:	bsr	hexin			; get the start address
	movd	r0,r2
	bsr	hexin			; get the ending address
	movd	r0,r3
	bsr	hexin			; get the value to fill with
	movd	r0,r1			; ...now in r1
	bsr	setcontext		; determine context
	cmpqb	c_byte,r0		;
	bne	fill2			;
fill1:	cmpd	r2,r3			;
	bhi	next			;
	movb	r1,0(r2)		; store the byte
	addqd	1,r2			; select the next address
	br	fill1			;
fill2:	movd	r0,r4			;
	addr	0(r0)[r0:b],r5		; r5 <- context [i.e., 1/2] * 2
	movd	r1,tos			; tos = source
fill3:	cmpd	r2,r3			; at ending address?
	bhi	next
	addr	tos,tos			; fill w/d is src, at tos
	movd	r2,tos			; push dst
	movzbd	r4,tos			; push context
	bsr	conmove			; move
	addd	r5,r2			; select next item
	br	fill3
;
; move command
;
move:	
	bsr	hexin			; get source address
	movd	r0,tos			; save this for r1
	bsr	hexin			; get destination address
	movd	r0,r2			; in r2, for string2
	bsr	hexin			; get length
	movd	r0,r3
	movd	tos,r1			; src in r1
	bsr	setcontext		; allow b/w/d
	movd	r0,r4			; need r0 for length
	movd	r3,r0			; so put the length there
movego:	caseb	movelst[r4:b]		; select a move with the right context	
movelst:.byte	bm-movego,wm-movego,dm-movego
bm:	movsb				; move bytes
wm:	movsw				; move words
dm:	movsd				; move doubles
	br	next
;
; setcontext - read a byte, and return...
;
; r0	- c_byte, c_word, or c_dble as per nothing/'b', 'w', 'd'
;
;	  c_byte is known to be zero
;
bwdlist:	.byte	'dwb'
 bwdsize:	.equ	*-bwdlist
 bwdspec:	.byte	3,1
setcontext:
	save	[r1,r4]
	bsr	skipblanks		; skip to last parameter
	bsr	readbyte		; read it
	movb	r0,r4			; must be in 0/b/w/d
	addr	bwdlist,r1		;
	movqd	bwdsize,r0		;
	skpsbu				; search for b,w,d
	checkb	r0,bwdspec,r0		; normalize 1..3 to 0..2
	bfc	setc1			; if f=0, done
	movqd	0,r0			; end of line or error
setc1:	restore	[r1,r4]
	ret	0
;
; the dump command
;
; r3: starting (next) address
; r4: byte count remaining
; r5: checksum for the current line
;
dump:	bsr	hexin			; get the start address
	movd	r0,r3
	bsr	hexin			; get the byte count (alas, in hex)
	movd	r0,r4			; 
dump1:	movzbd	'L',tos
	bsr	writebyte
	movd	r3,tos			; write out the address
	bsr	hextwo			; (without an `=')
	movzbd	' ',tos			; do a space
	bsr	writebyte
	movqd	0,r5			; initialize the checksum byte
	addqd	1,r4			; want a parameter of zero to work
	br	dump3a			; so add one and jump to the test
dump2:	movzbd	0(r3),tos		; output the next byte
	bsr	hexbyteout
	addb	0(r3),r5		; factor it in to the checksum
	addqd	1,r3			; select the next address
dump2a:	extsb	r3,r0,0,2		; add a space if address<1:0>=0
	cmpqb	0,r0			; this can't
	bne	dump3			; ...possibly
	movzbd	' ',tos			; ...be worth
	bsr	writebyte		; ...the effort
dump3:	extsb	r3,r0,0,4		; get address<3:0>, check if zero
	cmpqb	0,r0			; if so, start a new line
	bne	dump3a			; otherwise, do another byte (if nec.)
	bsr	endline
	br	dump4			; be sure the line isn't ended twice
dump3a:	acbd	-1,r4,dump2		; here with no line termination
	bsr	endline
	br	next
dump4:	acbd	-1,r4,dump1		; here after a line is terminated
	br	next
endline:movd	r5,tos			; output the check sum byte and crlf
	bsr	hexbyteout		;
	movqd	0,r5			; reset the checksum byte
	addr	crlf,tos		; need a crlf
	bsr	wstrnul			; ...to terminate the line
	ret	0
;
; all - print all registers
;
all:	movzbd	ejregt,r3		; get the register table ending address
	movqd	0,r4			; keep display count in r4
	movd	h'aabb20,tos		; put template for name string on tos
all1:	movw	regt-3[r3:b],1(sp)	; insert register name to template
	addr	tos,tos			; push the template address
	bsr	wstrnul			; write the name, preceded by a blank
	movzbd	regt-1[r3:b],r0		; get the fp offset out of the table
	movd	0(fp)[r0:b],tos		; get the register off the stack
	bsr	hexout			; go print it
	addqd	1,r4			; update display count
	extsb	r4,r0,0,2		; get display count <1:0>
	cmpqb	0,r0			; if zero, end of the line
	bne	all2			; no, continue
	addr	crlf,tos		; yes, do a crlf
	bsr	wstrnul
all2:	acbd	-3,r3,all1		; display some more?
	br	restart
;
; setcfg prototype
;
xcfg0:	.byte	h'0e,h'0b,0
	jump	r0
 xclen:	.equ	*-xcfg0
;
; x setcfg
;
xconfig:adjspb	xclen			; make room for setcfg sequence
	movmb	xcfg0,tos,xclen		; move it to tos
	bsr	hexin			; obtain the ``new'' configuration
	inssd	r0,1(sp),7,4		; create the instruction
	addr	rmmu2,r0		; warm up the getaway car
	jump	tos			; configure
;
; wmmu - write mmu register
;
wmmcase:	.byte	wbpr0-wcase,wbpr1-wcase,wnop-wcase,wnop-wcase
		.byte	wpf0-wcase,wpf1-wcase,wnop-wcase,wnop-wcase,wsc-wcase
		.byte	wnop-wcase,wmsr-wcase,wbcnt-wcase,wptb0-wcase
		.byte	wptb1-wcase,wnop-wcase,weia-wcase
wmmu:	
	bsr	hexin			; get mmu register number
	movd	r0,r6
	bsr	eqhexin			; get the new register value (r0)
wcase:	caseb	wmmcase[r6:b]		; case to short store mmu inst
wbpr0:	lmr	bpr0,r0
	br	wmmu1
wbpr1:	lmr	bpr1,r0
	br	wmmu1
wpf0:	lmr	pf0,r0
	br	wmmu1
wpf1:	lmr	pf1,r0
	br	wmmu1
wsc:	lmr	sc,r0
	br	wmmu1
wmsr:	lmr	msr,r0
	br	wmmu1
wbcnt:	lmr	bcnt,r0
	br	wmmu1
wptb0:	lmr	ptb0,r0
	br	wmmu1
wptb1:	lmr	ptb1,r0
	br	wmmu1
weia:	lmr	eia,r0
	br	wmmu1
wnop:	
wmmu1:	br	restart			; restart will restore the stack level
;
; rmmu - read mmu register
;
rmmcase:	.byte	rbpr0-rcase,rbpr1-rcase,rnop-rcase,rnop-rcase
		.byte	rpf0-rcase,rpf1-rcase,rnop-rcase,rnop-rcase,rsc-rcase
		.byte	rnop-rcase,rmsr-rcase,rbcnt-rcase,rptb0-rcase
		.byte	rptb1-rcase,rnop-rcase,reia-rcase
rmmu:	
	bsr	hexin			; get the mmu register number
rcase:	caseb	rmmcase[r0:b]		; case to short store mmu inst
rbpr0:	smr	bpr0,tos
	br	rmmu1
rbpr1:	smr	bpr1,tos
	br	rmmu1
rpf0:	smr	pf0,tos
	br	rmmu1
rpf1:	smr	pf1,tos
	br	rmmu1
rsc:	smr	sc,tos
	br	rmmu1
rmsr:	smr	msr,tos
	br	rmmu1
rbcnt:	smr	bcnt,tos
	br	rmmu1
rptb0:	smr	ptb0,tos
	br	rmmu1
rptb1:	smr	ptb1,tos
	br	rmmu1
reia:	smr	eia,tos
	br	rmmu1
rnop:	movqd	0,tos
rmmu1:	bsr	hexout			; print it
rmmu2:	br	restart			; restart will restore the stack level
;
; The next two procedures (r3out and atest) are used by the memory
; probe command.
;
; r3out(string)
;
; print string, r3, crlf
;
r3out:	movd	4(sp),tos		; print the string
	bsr	wstrnul			; 
	movd	r3,tos			; print r3
	bsr	hextwo			; 
	addr	crlf,tos		; print crlf
	bsr	wstrnul
	ret	4
;
; test (double) 0(r3)
;
; uses:	r0
;
; returns:
;	z = 1	address can't be written
;	z = 0	address can be written with 1 and -1
;
atest:	movd	0(r3),r0		; get a double word
	movqd	1,0(r3)			; write 1
	cmpqd	1,0(r3)			;
	bne	atest1			; check 1
	movqd	-1,0(r3)		; write -1
	cmpqd	-1,0(r3)		; 
	bne	atest1			; check -1
	movd	r0,0(r3)		; put the word back
atest1:	ret	0
;
; test		- try various tests
;
; t 1		- probe ram configuration
; t 2 a l	- test `l' bytes of ram at address `a' 
;
testcase:	.byte	test1-tcase,ramtest-tcase
 testcount:	.equ	*-testcase
testcheck:	.byte	testcount,1
isram:		.byte	'ram begins: ',0
isnotram:	.byte	'ram ends:   ',0
etest:		.byte	'test ends:  ',0
test:	bsr	hexin			; get test number
	checkb	r0,testcheck,r0		; normalize test number
	bfs	quest			; an invalid test?
tcase:	caseb	testcase[r0:b]		; branch to test
test1:	movqd	0,r3			; test1 is simple, just look for ram
notram:	bsr	atest			; is 0(r3) writable?
	bne	notram2			; atest returns z
	addr	isram,tos		; this is ram
	bsr	r3out			; it is a transition, note it
	br	ram			; now look for the lack of ram
notram2:bsr	echeck			; no ram, check for end
	br	notram			; ...still looking
ram:	bsr	atest			; here for ram, look for not ram
	beq	ram2			;
	addr	isnotram,tos		; just the opposite of above
	bsr	r3out			;
	br	notram
ram2:	bsr	echeck			;
	br	ram
echeck:	addr	h'1000(r3),r3
	tbitd	21,r3			; test for up to 2 million bytes
	bfs	echeck1			;
	ret	0
echeck1:addr	etest,tos		; note the end of the test
	bsr	r3out
	br	next
;
; ramtest
;
; r1 - failure count
; r2 - pattern counter
; r3 - first addr
; r4 - length
; r5 - next byte
; r6 - byte counter
; r7 - test number
;
nextrt:	.byte	'cycle',0
ramtest:bsr	hexin			; get starting address
	movd	r0,r3			; it belongs in r3
	bsr	hexin			; get byte count to test
	movd	r0,r4			; belongs in r4
	bicb	3,r4			; tests are aligned
	cmpqd	7,r4			; and for > 7 bytes
	bge	quest
	movqd	0,r7			; high order word of r7 is loop count
retest:	lshd	-16,r7			; get the high ...
	addqd	1,r7			; count up the number of cycles
	addr	nextrt,tos		; print cycle message
	bsr	wstrnul			; 
	movd	r7,tos			; print cycle counter
	bsr	hexout			; (too bad it's in hex)
	addr	crlf,tos		; a newline would help
	bsr	wstrnul			;
	lshd	16,r7			; put counter back in 31:16
	movzbw	rtnum,r7		; test never ends, get loop counter
rtloop:	addr	@8,tos			; going to write 8 bytes
	negw	r7,r0			; -(test number) gets test name
	movxwd	r0,r0
	addr	rtlist+(rtnum*8)[r0:q],tos; get the name
	bsr	wstrlen			; write the test name
	addr	crlf,tos		; 
	bsr	wstrnul
	movqd	pcount,r2		; get the pattern count
rtl1:	movd	r3,r5			; r5 contains the current address
	movd	r4,r6			; r4 is the loop counter
	negw	r7,r0			;
	movxwd	r0,r0
rtgo:	casew	rtjump+(rtnum*2)[r0:w]
rtret:	acbd	-1,r2,rtl1		; try the next pattern
	acbw	-1,r7,rtloop		; try the next test
	br	retest
;		 12345678
rtlist:	.byte	'parity  '		; h
	.byte	'addr -> '		; f
	.byte	'addr <- '		; g
	.byte	'doubles '		; e
	.byte	'e. words'		; c
;	.byte	'o. words'		; d
	.byte	'e. bytes'		; a
;	.byte	'o. bytes'		; b
	.byte	'load    '		; i
rtjump:	.word	rth-rtgo		; address parity to each word, two pass
	.word	rtf-rtgo		; increasing addresses, two pass
	.word	rtg-rtgo		; decreasing addresses, two pass
	.word	rte-rtgo		; doubles
	.word	rtc-rtgo		; even words
;	.word	rtd-rtgo		; odd words
	.word	rta-rtgo		; even bytes
;	.word	rtb-rtgo		; odd bytes
	.word	rti-rtgo		; load test
 rtnum:	.equ	(*-rtjump)/2
rlist:	.double	h'39a7c736		; miscellaneous bits
	.double	h'eeddbb77		; a different low in each byte
	.double	h'11224488		; a different high in each byte
	.double	h'55555555		; adjacent bits different
	.double	h'aaaaaaaa		; adjacent bits different
	.double	h'ffffffff		; all ones
	.double	h'00000000		; all zeros
 pcount:.equ	(*-rlist)/4
rta:	movb	rlist-4[r2:d],r0	; even bytes
rta0:	movb	r0,0(r5)		; write the byte pattern
	addqd	2,r5			; get the next even byte
	acbd	-2,r6,rta0		; loop
	movd	r3,r5			; reinitialize
	movd	r4,r6
rta0b:	cmpb	0(r5),r0		; test it
	bne	rta1
	cmpb	0(r5),r0		; test it one more time
	beq	rta2
rta1:	bsr	rtfails			; failed
rta2:	addqd	2,r5			; get the next even byte
	acbd	-2,r6,rta0b		; loop
	br	rtret
rtb:	addqd	1,r3			; odd bytes
	movd	r3,r5			; reset address register
	br	rta
rtc:	movw	rlist-4[r2:d],r0	; even words
rtc0:	movw	r0,0(r5)		; write the word
	addqd	2,r5			; 
	acbd	-2,r6,rtc0		;
	movd	r3,r5			; reinitialize
	movd	r4,r6			;
rtc0b:	cmpw	0(r5),r0		; check it
	bne	rtc1
	cmpw	0(r5),r0		; check it again
	beq	rtc2
rtc1:	bsr	rtfails			; could fail...
rtc2:	addqd	2,r5			; ...or succeed
	acbd	-2,r6,rtc0b
	br	rtret
rtd:	addqd	1,r3			; odd words
	movd	r3,r5
	addqd	-2,r4			; have to check one fewer word
	movd	r4,r6
	br	rtc
rte:	movd	rlist-4[r2:d],r0	; double words
rte0:	movd	r0,0(r5)		; 
	addqd	4,r5
	acbd	-4,r6,rte0
	movd	r3,r5
	movd	r4,r6
rte0a:	cmpd	0(r5),r0		; try the usual two reads
	bne	rte1			; ...in a row
	cmpd	0(r5),r0
	beq	rte2
rte1:	bsr	rtfails
rte2:	addqd	4,r5
	acbd	-4,r6,rte0a
	br	rtret
rtf:	movd	rlist-4[r2:d],r1	; multiple write test
rtf0:	movd	r5,r0			; This test writes the address
	xord	r1,r0			; ...of each location into each
	movd	r0,0(r5)		; ...location. The address is xor'd
	addqd	4,r5			; ...with the usual patterns
	acbd	-4,r6,rtf0		; 
rtf0a:
	movd	r3,r5			; Now the second pass goes back
	movd	r4,r6			; ...and expects to read the same
rtf1:	movd	r5,r0			; ...addresses. If two addresses
	xord	r1,r0			; ...map into the same word, the
	cmpd	r0,0(r5)		; ...second pass won't be happy
	bne	rtf2
	cmpd	r0,0(r5)
	beq	rtf3
rtf2:	bsr	rtfails
	br	rtret			; This probably isn't necessary
rtf3:	addqd	4,r5			; ...but we want to avoid extra
	acbd	-4,r6,rtf1		; ...error messages
	br	rtret
rtg:	movd	rlist-4[r2:d],r1	; Here's the same multiple write
	addr	-4(r6)[r5:b],r5		; ...test but proceeding from
rtg0:	movd	r5,r0			; ...high addresses to low addresses
	xord	r1,r0			;
	movd	r0,0(r5)		; This test should catch lower words
	addqd	-4,r5			; that write onto higher addresses
	acbd	-4,r6,rtg0
	br	rtf0a
rth:	movd	r5,r0			; Ramtest h writes some variant of
	lshd	-16,r0			; ...the parity of each word's
	xorw	r5,r0			; ...address in all the bits of the
	movw	r0,r1			; ...word. In fact, a word of 1's
	lshw	-8,r0			; ...or zeroes is xor'd with the
	xorw	r1,r0			; ...current pattern and written in
	movb	r0,r1			; ...the first pass. The idea is
	lshb	-4,r0			; ...to catch errors that would
	xorb	r1,r0			; ...be hidden by the orderly
	movb	r0,r1			; ...progression of 1's and 0's in
	lshb	-2,r0			; ...the address test. The halves of
	xorb	r1,r0			; ...the address are xor'd together
	movb	r0,r1			; ...until only one bit (parity)
	lshb	-1,r0			; ...remains
	xorb	r1,r0			; ...and that's now...
	lshw	15,r0			; shift the parity into the sign bit
	ashw	-15,r0			; ...extend it through the register
	xorw	rlist-4[r2:d],r0	; xor in the pattern
	movw	r0,0(r5)		; put it in memory
	addqd	2,r5			; the end of first pass loop
	acbd	-2,r6,rth
	movd	r3,r5			; test h second pass
	movd	r4,r6
rth0:	movd	r5,r0			; do the  whole parity trip
	lshd	-16,r0			; ...again. This isn't a subroutine
	xorw	r5,r0			; ...because it seemed desireable
	movw	r0,r1			; ...for ramtest to run completely
	lshw	-8,r0			; ...out of rom.
	xorw	r1,r0			;
	movb	r0,r1			; actually, that isn't really
	lshb	-4,r0			; ...necessary, and it wastes
	xorb	r1,r0			; ...a lot of rom space here
	movb	r0,r1			;
	lshb	-2,r0			; it uses ram anyway if an error
	xorb	r1,r0			; ...is discovered
	movb	r0,r1
	lshb	-1,r0
	xorb	r1,r0
	lshw	15,r0			; sign extend the parity again
	ashw	-15,r0
	xorw	rlist-4[r2:d],r0	; include the pattern
	cmpw	0(r5),r0		; check memory
	bne	rth1
	cmpw	0(r5),r0		; give it the usual two chances
	beq	rth2			; ...to fail
rth1:	bsr	rtfails
rth2:	addqd	2,r5			; end of test h pass 2
	acbd	-2,r6,rth0
	br	rtret
;
; load test
;
rti:	addr	@20,r5			;
	movd	r2,r6
rti0:	movd	r4,r0			; do lots of transfers
	lshd	-2,r0			; (double word count in r0)
	movd	r3,r1			; this test trusts the parity
	movd	r3,r2			; logic to find any errors
	movsd				;
	acbb	-1,r5,rti0
	movd	r6,r2
	br	rtret
;
rtfails:save	[r0,r1,r2]		; output an error message
	movd	0(r5),tos		; memory	/ the guilty bits
	movd	r0,tos			; pattern	/ the modification
	movd	rlist-4[r2:d],tos
	movd	r5,tos			; address	/ the failing location
	addr	rtfailmsg,tos		; message	/ the complaint
	bsr	wstrnul
	bsr	hexout
	bsr	hexout
	bsr	hexout
	bsr	hexout
	addr	crlf,tos
	bsr	wstrnul
	restore	[r0,r1,r2]
	ret	0
rtfailmsg:	.byte	'Failure(=addr=xor=test=mem)',0
;
; these aren't used; there's no way to get imports without a reference
; ...that is, we use the link page of this module for the interrupt vector
; ...table and therefore we have to generate a reference to the imported
; ...entry points.
;
	cxp	nvi
	cxp	nmi
	cxp	abt
	cxp	fpu
	cxp	ill
	cxp	svc
	cxp	dvz
	cxp	flg
	cxp	bpt
	cxp	trc
	cxp	und
.static
;
; the static segment is actually constant; these pointers will be in ROM
; ...addressing the peripherals this way uses very small sb offsets,
; ...instead of many 4-byte absolute addressing constants
;
;
;
; The I/O configuration for the DB16000
;
sio:		.double	h'c00000	; serial I/O channel address
 o_data:	.equ	0		; offset of data register
 o_status:	.equ	2		; offset of status register
 o_siocmd:	.equ	2		; offset of command register
usfirst:.byte	siocom,h'4e,h'40,0,0,0	; usart initialization sequence
 uscount:	.equ	*-usfirst
options:	.double	h'c00030	; option switch register
.endseg
;
; this dsect defines the format of the stack as the monitor is entered
; a 32-bit entry is arranged for each registers, and then any register
; can be changed by looking its name up in a table and changing the stack
; copy. The change takes effect when the user program is restarted.
;
; It is a somewhat unfortunate thing: the assembler generates an absolute
; addressing mode when any of these things are used as address expressions.
; To get the literal offset, it's important to use: ``e_??-e_begin''.
;
.dsect				; (``CDW'' = ``changes don't work'')
	e_begin:
	e_echo:	.blkw	1		; echo flag
	e_fsr32:.blkd	1		; 32-bit fake copy of the psr
	e_psr32:.blkd	1		; 32-bit copy of the psr
	e_sb:	.blkd	1		; copy of mod table entry	CDW
	e_is:	.blkd	1		; calculated from stack level	CDW
	e_mod32:.blkd	1
	e_fp:	.blkd	1
	e_us:	.blkd	1
	e_intbase:.blkd	1
	e_r7:	.blkd	1
	e_r6:	.blkd	1
	e_r5:	.blkd	1
	e_r4:	.blkd	1
	e_r3:	.blkd	1
	e_r2:	.blkd	1
	e_r1:	.blkd	1
	e_r0:	.blkd	1
	e_pc:	.blkd	1
	e_mod0:	.blkw	1		; its original place on the stack
	e_psr0:	.blkw	1		; ""
	e_frame:
.endseg
;
; This table defines the register names with their offsets on the
; interrupt stack.  This way, a single subroutine is able to change
; any register. ``Sp'' is an alias for ``us''.
;
regt:	.byte	'ps',e_psr32
	.byte	'sb',e_sb
	.byte	'is',e_is
	.byte	'mo',e_mod32
	.byte	'fp',e_fp
	.byte	'us',e_us
	.byte	'sp',e_us
	.byte	'in',e_intbase
	.byte	'r7',e_r7
	.byte	'r6',e_r6
	.byte	'r5',e_r5
	.byte	'r4',e_r4
	.byte	'r3',e_r3
	.byte	'r2',e_r2
	.byte	'r1',e_r1
	.byte	'r0',e_r0
	.byte	'pc',e_pc
 ejregt:.equ	(*-regt)		; number of bytes of register data
	.byte	'fs',e_fsr32
	.byte	'mb',e_frame+0
	.byte	'mw',e_frame+1
	.byte	'md',e_frame+2
 eregt:	.equ	(*-regt)		; number of bytes of reg/mem data
 vmem:	.byte	'vb',e_frame+0
	.byte	'vw',e_frame+1
	.byte	'vd',e_frame+2
 evmem:	.equ	(*-vmem)
;
; here we have the entry message strings
;
trapt:	.byte	'NVI NMI ABT FPU ILL SVC DVZ FLG BPT TRC UND '
;		 ----====----====----====----====----====----
.endseg
.static
hello::	.byte	lf,cr,'National DB16000 Monitor (Rev. 1.1) (tsang) Thu Sep 22 17:16:02 PDT 1983',lf,cr,0
.endseg
