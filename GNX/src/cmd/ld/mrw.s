
;*
;*	Mesa version of fastio for meld
;*

.importp	_read,_fflush

std_cnt:	.equ	0	; int
std_ptr:	.equ	4	; char *
std_base:	.equ	8	; char *
std_flag:	.equ	12	; short
std_file:	.equ	14	; char

ptr:		.equ	12
size:		.equ	16
count:		.equ	20
iop:		.equ	24

_fastread::
	enter	[r5,r6,r7],0
	movd	iop(fp),r7		; r7 <- iop
	movd	ptr(fp),r6		; r6 <- ptr
	movd	size(fp),r5		; r5 <- bytecount
	muld	count(fp),r5
rd0:	movd	r5,r0
	cmpd	0(r7),r0
	bge	rd1
	movd	0(r7),r0
	cmpqd	0,r0
	beq	rd2
rd1:	subd	r0,0(r7)		; update counts now
	subd	r0,r5
	movd	std_ptr(r7),r1		; src pointer
	movd	r6,r2			; dest ptr
	movsb				; move the data
	movd	r1,std_ptr(r7)		; update pointers
	movd	r2,r6
	cmpqd	0,r5			; if count left, not done
	blt	rd2
	movqd	1,r0			; done, return true
	exit	[r5,r6,r7]
	rxp	0
rd2:	movzwd	1024,tos
	movd	std_base(r7),tos
	movzbd	std_file(r7),tos
	cxp	_read
	adjspb	-12
	movd	std_base(r7),std_ptr(r7)
	movd	r0,0(r7)
	cmpqd	0,r0
	blt	rd0
	exit	[r5,r6,r7]
	rxp	0

newcount:	.equ	12
sfile:		.equ	16
dfile:		.equ	20

_ftransfer::
	enter	[r6,r7],0
	movd	sfile(fp),r7
	movd	dfile(fp),r6
ft0:	movd	newcount(fp),r0
	cmpd	0(r7),r0		; is bytes-in-buffer > count?
	bgt	ft1			; yes, otherwise...
	movd	0(r7),r0		; is free-bytes-left > count?
ft1:	cmpd	0(r6),r0		; yes, otherwise...
	bgt	ft2			; ...take minimum
	movd	0(r6),r0
ft2:	movd	std_ptr(r7),r1		; fetch pointers
	movd	std_ptr(r6),r2
	subd	r0,0(r7)		; update counts
	subd	r0,0(r6)
	subd	r0,newcount(fp)
	movsb				; move the data
	movd	r1,std_ptr(r7)		; update pointers
	movd	r2,std_ptr(r6)
	cmpqd	0,0(r6)			; dest buffer full?
	blt	ft3
	movd	r6,tos
	cxp	_fflush			; write the buffer out
	adjspb	-4
ft3:	cmpqd	0,newcount(fp)
	blt	ft4
	exit	[r6,r7]
	rxp	0
ft4:	cmpqd	0,0(r7)			; bytes left in input?
	blt	ft0
	movzwd	1024,tos		; nope, get some more
	movd	std_base(r7),tos
	movzbd	std_file(r7),tos
	cxp	_read
	adjspb	-12
	movd	std_base(r7),std_ptr(r7)
	movd	r0,0(r7)
	br	ft0

