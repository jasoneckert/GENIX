;
; doprnt.s: version 1.15 of 9/26/83
; Mesa Unix C Library Source File
;
; @(#)doprnt.s	1.15 (NSC) 9/26/83
;

	.title	'Print conversions -- doprnt.S, version 1.15 (NSC) 9/26/83'
;;	.list
;;	.width	132

; Copyright (C) 1983 by National Semiconductor Corporation

;	National Semiconductor Corporation
; 	2900 Semiconductor Drive
; 	Santa Clara, California 95051

; All Rights Reserved

; This software is furnished under a license and may be used and copied only
; in accordance with the terms of such license and with the inclusion of the
; above copyright notice.  This software or any other copies thereof may not
; be provided or otherwise made available to any other person.  No title to
; or ownership of the software is hereby transferred.

; The information in this software is subject to change without notice
; and should not be construed as a commitment by National Semiconductor
; Corporation.

; National Semiconductor Corporation assumes no responsibility for the use
; or reliability of its software on equipment configurations that are not
; supported by National Semiconductor Corporation.
	.eject
	.program

	.export	__doprnt
	.import	__flsbuf

;===============================================================================
;	comment these out when not debugging!

;;	.export	strout, errdone, prdone, doprnt, loop, short, space
;;	.export	fmtbad, sharp, indir, plus, minus, dot, gnum, gnum0, decimal
;;	.export	capital, octal, unsigned, hex, binary, charac, remote, string
;;	.export	prstr, pad, bye, prnum, str1

;===============================================================================

ndfnd:	.equ	0		; flag bit positions
prec:	.equ	1
zfill:	.equ	2
minsgn:	.equ	3
plssgn:	.equ	4
numsgn:	.equ	5
caps:	.equ	6
blank:	.equ	7
gflag:	.equ	8
dpflag:	.equ	9
unsign:	.equ	10

t1:	.equ	r7		; general temporary
t2:	.equ	r6		; ditto
stbuf:	.equ	r5		; the temp string buf pointer

cnt:	.equ	0		; offset of the cnt field in file structure
ptr:	.equ	4		; offset of the ptr field in file structure
;------------------------------------------------

	.static

	.endseg

htab:	.byte	"0123456789abcdef"

strtab:	; translate table for detecting null and percent
	.byte	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
	.byte	 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
	.byte	" !""#$",  0, "&'()*+,-./"
	.byte	"0123456789:;<=>?"
	.byte	"@ABCDEFGHIJKLMNO"
	.byte	"PQRSTUVWXYZ[\]^_"
	.byte	"`abcdefghijklmno"
	.byte	"pqrstuvwxyz{|}~", 127
	.byte	128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143
	.byte	144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159
	.byte	160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175
	.byte	176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191
	.byte	192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207
	.byte	208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223
	.byte	224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239
	.byte	240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255

;------------------------------------------------
	
thissymbol:
strmore:
	movqb	1,r4			; set escape flag just in case
	movzbd	0(r1),r2		; peek ahead one char
	cmpqb	0,(r3)[r2:b]		; translate
	beq	stresc			; bad guy in disguise (outbuf is full)
	addqd	-1,r0			; str->len--
	addqd	1,r1

; enter here to force out r2; r0,r1 must be set

strout2:save	[r0,r1]		; save input descriptor
	movd	file,tos	; FILE
	movd	r2,tos		; the char
	cxp	__flsbuf	; please empty the buffer and handle 1 char
	adjspb	-8
	cmpqd	0,r0		; successful?
	ble	strm1		; yes
	sbitb	31,nchar	; turn on sign bit of nchar to signify err

strm1:	addqd	1,nchar		; count the char
	restore	[r0,r1]		; get input descriptor back

; enter via jsr with input descriptor = [r0, r1]

strout:
	addr	strtab,r3	; translation table address
	movqd	0,r4		; character to terminate move on
	movd	r0,t1		; save the input string length
	movd	cnt(file),r0	; r0 <- buffer space remaining
	cmpd	r0,t1		; r0 <- min(str.len,file.cnt)
	blt	str1
	movd	t1,r0		; string was smaller
 str1:
	movd	r0,t2		; save this length
	cmpqd	0,r0		; anything to do?
	bge	str2		; nope
	;subd	r0,cnt(file)	; intend to use this much buffer space
	;subd	r0,t1		; decrement string length
	;addd	r0,nchar	; intend to print this many chars
	movd	ptr(file),r2	; destination buffer address

	movstu			; move and translate
	sfsb	r4		; remember how we terminated
	movd	r2,ptr(file)	; update file ptr
	;addd	r0,cnt(file)	; return unused buffer space
	;subd	r0,nchar	; couldn't move this many chars
	subd	r0,t2		; # chars which were actually moved
	subd	t2,t1		; decrement string length
	subd	t2,cnt(file)	; and buffer space remaining
	addd	t2,nchar	; moved this many more chars
	cmpqb	1,r4		; was this an escape termination?
	beq	stresc		; yes...
	;negd	r0,r0		; no, get remaining string length
str2:
	movd	t1,r0		; into r0
	cmpqd	0,r0		; any more to move?
	blt	strmore		; yes, go do them
stresc:	
	movd	t1,r0		; return actual string length
	tbitb	0,r4		; no, set the flag bit for return
	ret	0		; if flag set, we terminated due to escape
errdone:sbitb	31,nchar	; set err bit

prdone:	movd	nchar,r0	; doprint exit point
	adjspw	-256		; scratch 256 bytes of space
	exit	[r3, r4, r5, r6, r7]
	rxp	0
;------------------------------------------------

__doprnt:.proc

					; from <stdio.h>:
					;   extern	struct	_iobuf {
					;	int	_cnt;
					;	char	*_ptr;
					;	char	*_base;
					;	short	_flag;
					;	char	_file;
					;	} _iob[_NFILE];
					;   #define	FILE	struct _iobuf
					;
file:	.blkd	1	; fp+14^	; arg 3: pointer to unix file descriptor
ap:	.blkd	1	; fp+10^	; arg 2: pointer to arg list 
fmt:	.blkd	1	; fp+c^		; arg 1: pointer to format string

	.var	[r3, r4, r5, r6, r7]

flags:	.blkd	1
llafx:	.blkd	1
lrafx:	.blkd	1
ndigit:	.blkd	1
width:	.blkd	1
nchar:	.blkd	1

exp:	.blkd	1
sexp:	.blkd	1
sign:	.blkd	1

;------------------------------------------------

	.begin


doprnt:
	adjspw	256		; 256 bytes of scratch space
	movqd	0,nchar		; number of chars transferred

loop:	movzwd	65535,r0	; pseudo length
	movd	fmt,r1		; fmt addr
	bsr	strout		; copy to output, stop at null or percent
	movd	r1,fmt		; next fmt char
	bfc	loop		; if no escape, then very long fmt
	cmpqb	0,0(r1)		; escape; null or percent?
	beq	prdone		; null means end of fmt
	addqd	1,fmt		; bump past %

	sprd	sp,stbuf	; reset output buffer pointer
	movqd	0,flags		; initialize all output flags
	movqd	0,llafx
	movqd	0,lrafx
	movqd	0,width

; we can ignore both of these distinctions

longorunsg:
short:
Z4a:	movzbd	0(fmt),r0		; so capital letters can tail merge
	addqd	1,fmt

Z4:
	checkb	r7,fmtbnds,r0		; in bounds?
	bfs	fmtbad			; nope...

Z5:	casew	Z6[r7:w]		; yep, go do it

fmtbnds: .byte "x "		; the format char bounds - "x" - " "

Z6:	.word	space-Z5			; space
	.word	fmtbad-Z5			; !
	.word	fmtbad-Z5			; "
	.word	sharp-Z5			; #
	.word	fmtbad-Z5			; $
	.word	fmtbad-Z5			; %
	.word	fmtbad-Z5			; &
	.word	fmtbad-Z5			; "
	.word	fmtbad-Z5			; (
	.word	fmtbad-Z5			; )
	.word	indir-Z5			; *
	.word	plus-Z5				; +
	.word	fmtbad-Z5			; ,
	.word	minus-Z5			; -
	.word	dot-Z5				; .
	.word	fmtbad-Z5			; /
	.word	gnum0-Z5			; 0
	.word	gnum-Z5				; 1
	.word	gnum-Z5				; 2
	.word	gnum-Z5				; 3
	.word	gnum-Z5				; 4
	.word	gnum-Z5				; 5
	.word	gnum-Z5				; 6
	.word	gnum-Z5				; 7
	.word	gnum-Z5				; 8
	.word	gnum-Z5				; 9
	.word	fmtbad-Z5			; :
	.word	fmtbad-Z5			; ;
	.word	fmtbad-Z5			; <
	.word	fmtbad-Z5			; =
	.word	fmtbad-Z5			; >
	.word	fmtbad-Z5			; ?
	.word	fmtbad-Z5			; @
	.word	fmtbad-Z5			; A
	.word	fmtbad-Z5			; B
	.word	fmtbad-Z5			; C
	.word	decimal-Z5			; D
	.word	capital-Z5			; E
	.word	fmtbad-Z5			; F
	.word	capital-Z5			; G
	.word	fmtbad-Z5			; H
	.word	fmtbad-Z5			; I
	.word	fmtbad-Z5			; J
	.word	fmtbad-Z5			; K
	.word	fmtbad-Z5			; L
	.word	fmtbad-Z5			; M
	.word	fmtbad-Z5			; N
	.word	octal-Z5			; O
	.word	fmtbad-Z5			; P
	.word	fmtbad-Z5			; Q
	.word	fmtbad-Z5			; R
	.word	fmtbad-Z5			; S
	.word	fmtbad-Z5			; T
	.word	unsigned-Z5			; U
	.word	fmtbad-Z5			; V
	.word	fmtbad-Z5			; W
	.word	hex-Z5				; X
	.word	fmtbad-Z5			; Y
	.word	fmtbad-Z5			; Z
	.word	fmtbad-Z5			; [
	.word	fmtbad-Z5			; \
	.word	fmtbad-Z5			; ]
	.word	fmtbad-Z5			; ^
	.word	fmtbad-Z5			; _
	.word	fmtbad-Z5			; `
	.word	fmtbad-Z5			; a
	.word	binary-Z5			; b
	.word	charac-Z5			; c
	.word	decimal-Z5			; d
	.word	scien-Z5			; e	<temp out>
	.word	float-Z5			; f	<temp out>
	.word	general-Z5			; g	<temp out>
	.word	short-Z5			; h
	.word	fmtbad-Z5			; i
	.word	fmtbad-Z5			; j
	.word	fmtbad-Z5			; k
	.word	longorunsg-Z5			; l
	.word	fmtbad-Z5			; m
	.word	fmtbad-Z5			; n
	.word	octal-Z5			; o
	.word	fmtbad-Z5			; p
	.word	fmtbad-Z5			; q
	.word	remote-Z5			; r
	.word	string-Z5			; s
	.word	fmtbad-Z5			; t
	.word	unsigned-Z5			; u
	.word	fmtbad-Z5			; v
	.word	fmtbad-Z5			; w
	.word	hex-Z5				; x

fmtbad:	movb	r0,0(stbuf)		; print the unfound character
	cmpqb	0,r0
	beq	errdone			; dumb users who end the format with a %
	addqd	1,stbuf
	br	prbuf

capital:sbitb	caps,flags		; note that it was capitalized
	xorb	"a" and "A",r0		; make it small
	br	Z4			; and try again

string:	movd	ndigit,r0
	tbitb	prec,flags		; max length was specified
	bfs	Z20
	movqd	-1,r0			; default max length

Z20:	movd	0(ap),r1		; addr first byte
	movd	r1,r2			; save start byte
	addqd	4,ap
	movqd	0,r4
	skpsbu
	movd	r1,stbuf		; addr last byte +1
	movd	r2,r1			; addr first byte
	br	prstr

binary:
	addqd	4,stbuf			; output slop
	movd	0(ap),r0		; r0 <- num to convert
	addqd	4,ap
	addr	@31,r2			; offset
 bin1:	extd	r2,r0,r1,1		; get the bits
	movb	htab[r1:b],0(stbuf)	; convert and store
	addqd	1,stbuf			; *out++
	addqd	-1,r2			; offset--
	cmpqd	0,r2			; done?
	ble	bin1			; nope
	movqd	1,r3			; note we're doing bin
	br	Z12

octal:
	addqd	4,stbuf			; output slop
	movd	0(ap),r0		; r0 <- num to convert
	addqd	4,ap
	addr	@30,r2			; offset
 	extd	r2,r0,r1,2		; get the first digit
	br	oct2			; (it is only 2 bits wide)

 oct1: 	extd	r2,r0,r1,3		; do remaining digits
 oct2:	movb	htab[r1:b],0(stbuf)	; convert and store
	addqd	1,stbuf			; *out++
	addqd	-3,r2			; offset--
	cmpqd	0,r2			; done?
	ble	oct1			; nope
	movqd	3,r3			; note we're doing octal
	br	Z12

hex:
	addqd	4,stbuf			; output slop
	movd	0(ap),r0		; r0 <- num to convert
	addqd	4,ap
	addr	@28,r2			; offset
 hex1:	extd	r2,r0,r1,4		; get the bits
	movb	htab[r1:b],0(stbuf)	; convert and store
	addqd	1,stbuf			; *out++
	addqd	-4,r2			; offset--
	cmpqd	0,r2			; done?
	ble	hex1			; nope
	movqd	4,r3			; note we're doing hex
				; fall into Z12...
Z12:
	movqd	0,llafx
	movqd	0,lrafx
	movqb	0,0(stbuf)		; terminate string
	addr	@31,r0			; max string len

Z12b:	addr	4(sp),r1		; string address
	movb	"0",r4			; char to skip
	skpsbw				; skip leading zeroes
	tbitb	numsgn,flags		; easy if no left affix
	bfc	prn3
	cmpqd	0,-4(ap)		; original value
	beq	prn3			; no affix on 0, for some reason
	cmpqd	4,r3			; were we doing hex or octal?
	bne	Z12a			; octal
	addqd	-1,r1
	movb	"x",0(r1)
	movqd	2,llafx			; leading 0x for hex is an affix

Z12a:	addqd	-1,r1
	movb	"0",0(r1); leading zero for octal is a digit, not an affix
	br	prn3			; omit sign (plus, blank) massaging
scien:
	movd	file,tos ; INSERTED
	addr	temp_fl,tos
	addr	control,tos
	movzbd	"e",tos
	movl 	0(ap),tos
	addd	8,ap
	movd	width,tos
	tbitd	prec,flags
	bfs	prec_found_e
	movqd	6,ndigit
prec_found_e:
	movd	ndigit,tos
	tbitd	minsgn,flags
	sfsd	tos
	tbitd	zfill,flags
	sfsd	tos
	bsr	_cnvf
	;movd	file,tos
	;addr	temp_fl,tos
	;addr	control,tos
	cxp	__doprnt
	adjspb	-12
	br 	loop

general:
	movd	file,tos ;INSERTED
	addr	temp_fl,tos
	addr	control,tos
	movzbd	"g",tos
	movl 	0(ap),tos
	addd	8,ap
	movd	width,tos
	tbitd	prec,flags
	bfs	prec_found_g
	movqd	6,ndigit
prec_found_g:
	cmpqd	0,ndigit
	bne	non_zero_prec
	movqd	1,ndigit
non_zero_prec:
	cmpqd	0,width
	bne	non_zero_width
	movqd	1,width
non_zero_width:
	movd	ndigit,tos
	tbitd	minsgn,flags
	sfsd	tos
	tbitd	zfill,flags
	sfsd	tos
	bsr	_cnvf
	;movd	file,tos
	;addr	temp_fl,tos
	;addr	control,tos
	cxp	__doprnt
	adjspb	-12
	br 	loop

float:
	movd	file,tos ;INSERTED
	addr	temp_fl,tos
	addr	control,tos
	movzbd	"f",tos
	movl 	0(ap),tos
	addd	8,ap
	movd	width,tos
	tbitd	prec,flags
	bfs	prec_found_f
	movqd	6,ndigit
prec_found_f:
	movd	ndigit,tos
	tbitd	minsgn,flags
	sfsd	tos
	tbitd	zfill,flags
	sfsd	tos
	bsr	_cnvf
	;movd	file,tos
	;addr	temp_fl,tos
	;addr	control,tos
	cxp	__doprnt
	adjspb	-12
	br 	loop

unsigned:
lunsigned:
	bicd	(1 shl plssgn) or (1 shl blank),flags
					; omit sign (plus, blank) massaging
	sbitb	unsign,flags		; is an unsigned item

decimal:
	movqd	0,r3			; flag for minus sign
	movd	0(ap),r1		; fetch value
	addqd	4,ap
	addr	12(stbuf),stbuf		; last addr + 1
	movd	stbuf,r7		; save it
	addr	@10,r2			; divisor
	tbitb	unsign,flags		; unsigned?
	bfs	dec1			; yep
	cmpqd	0,r1			; negative?
	ble	dec1			; nope
	addqd	1,llafx			; yes, "-" is left affix
	movqd	-1,r3			; needs a "-"
	negd	r1,r1
	br	dec1
 decl:
	deid	r2,r0
	addr	"0"(r0),r0		; r0 <- r0 + "0"
	movb	r0,-1(stbuf)		; save the char
	addqd	-1,stbuf		; stbuf--
 dec1:
	movd	r1,r0			; fetch quotient
	movqd	0,r1
	cmpqd	0,r0			; done?
	bne	decl			; nope
					; conversion done
	cmpqd	0,r3			; negative?
	beq	dec2			; nope
	addqd	-1,stbuf		; yes, include "-"
	movb	"-",0(stbuf)
 dec2:
	movd	stbuf,r1		; addr of first byte
	movd	r7,stbuf		; addr of last+1
	movqb	0,0(stbuf)		; terminate string

prnum:

; r1=addr first byte		r5=addr last byte +1
; llafx=size of signs		-1(r1) vacant, for forced sign

	cmpqd	0,llafx
	bne	prn3			; already some left affix, dont fuss
	tbitb	plssgn,flags
	bfc	prn2
	addqd	-1,r1
	movb	"+",0(r1)		; needs a plus sign
	br	prn4

prn2:	tbitb	blank,flags
	bfc	prn3
	addqd	-1,r1
	movb	" ",0(r1)		; needs a blank sign

prn4:	addqd	1,llafx

prn3:	tbitb	prec,flags
	bfs	prn1
	movqd	1,ndigit		; default precision is 1

prn1:	movd	stbuf,r0
	subd	r1,r0			; raw width
	subd	llafx,r0		; number of digits
	subd	r0,ndigit		; number of leading zeroes needed
	cmpqd	0,ndigit
	bge	prstr			; none
	addd	llafx,r1		; where current digits start
	addr	32(stbuf),r2		; check bounds on users who say %.300d
	addd	ndigit,r2
	addr	0(fp),r6
	subd	r6,r2
	cmpqd	0,r2
	bgt	prn5
	subd	r2,ndigit

;	pad with zeroes to the width specified by the precision
;	enter with:
;	r0 = # significant digits, r1 = *first addr, r5 = *lastbyte+1
;	set up r1 & r5 for prstr, etc

prn5:
	movd	r1,r7			; save first addr
	movd	ndigit,r2		; # leading zeroes needed
	addr	-1(stbuf),r1		; start at high end
	addr	(r1)[r2:b],r2		; move from high to low..
	addr	1(r2),r5		; r5 <- last addr+1
	movsbb				; open pad space

	movd	r7,r1			; r1 <- first addr
	movd	ndigit,r0		; # pad bytes
	br	prn7

prn6:
	movb	"0",0(r1)		; pad one
	addqd	1,r1			; buf++
	addqd	-1,r0			; padcount--
prn7:
	cmpqd	0,r0
	blt	prn6

	movd	r7,r1			; r1 <- first addr
	subd	llafx,r1		; include left affix length

;	r1=addr first byte	r5=addr last byte +1
;	width=minimum width	llafx=len. left affix
;	ndigit=<avail>

prstr:	movd	stbuf,ndigit
	subd	r1,ndigit		; raw width
	movd	width,r0
	subd	ndigit,r0		; pad length
	cmpqd	0,r0
	bge	padlno			; in particular, no left padding
	tbitb	minsgn,flags		; extension for %0 flag causing left zero padding
					;	to field width
	bfs	padlno
	tbitb	zfill,flags		; this call needed even if %0 flag extension is removed
	bfs	padlz
	bsr	padb			; blank pad on left
	br	padnlz

padlz:	movd	llafx,r0
	cmpqd	0,r0
	bge	padnlx		; left zero pad requires left affix first
	subd	r0,ndigit	; part of total length will be transferred
	subd	r0,width	; and will account for part of minimum width
	bsr	strout		; left affix

padnlx:	movd	width,r0
	subd	ndigit,r0		; pad length
	bsr	padz			; zero pad on left

padnlz: ; end of extension for left zero padding
padlno:	; remaining: root, possible right padding
	movd	ndigit,r0		; root length
	subd	r0,width		; root reduces minimum width

p1:	bsr	strout			; transfer to output buffer
p3:	bfc	padnpct			; percent sign (or null byte via %c) ?
	addqd	-1,r0			; yes; adjust count
	movzbd	0(r1),r2		; fetch byte
	addqd	1,r1
	movd	cnt(file),r4		; output buffer descriptor
	movd	ptr(file),stbuf
	addqd	-1,r4
	cmpqd	0,r4
	ble	p2			; room at the out [inn] ?
	bsr	strout2			; no; force it, then try rest
	br	p3			; here we go 'round the mullberry bush, ...

p2:	movb	r2,0(stbuf)		; hand-deposit the percent or null
	addqd	1,stbuf
	addqd	1,nchar			; count it
	movd	r4,cnt(file)		; store output descriptor
	movd	stbuf,ptr(file)
	br	p1			; what an expensive hiccup!

padnpct:movd	width,r0		; size of pad
	cmpqd	0,r0
	bge	loop
	bsr	padb
	br	loop

padz:	movb	"0",r2
	br	pad

padb:	movb	" ",r2

;
; pad -- create a pad string on the stack and print it.
; register args for pad:
;	r0	number of characters to pad
;	r1	next address after padding done
;	r2	character to pad with

pad:	cmpqd	0,r0			; failsafe check
	bge	pad1			; negative/zero pad width
	movd	r1,tos			; save non-pad addr
	subd	r0,width		; pad width decreases minimum width
	movd	r0,t1			; save width
	addqd	4,r0			; round width up to
	bicb	3,r0			; doublewords
	adjspd	r0			; allocate it
	movd	r0,tos			; save allocated size
	addr	4(sp),r1		; where to pad

	movb	r2,0(r1)		; move one pad
	addr	1(r1),r2		; where we will move to
	addqd	-1,r0			; one less
	movsb				; create pad

	movd	t1,r0			; recover string length
	addr	4(sp),r1		; string start
	bsr	strout			; print it
	negd	tos,r0			; fetch allocated length
	adjspd	r0			; deallocate the space
	movd	tos,r1			; recover r1
pad1:	ret	0			; done

charac:	movw	0(ap),r0		; word containing the char
	addqd	4,ap
	movb	r0,0(stbuf)		; one byte, that's all
	addqd	1,stbuf

prbuf:	sprd	sp,r1			; addr first byte
	br	prstr

space:	sbitb	blank,flags		; constant width e fmt, no plus sign
	br	Z4a

sharp:	sbitb	numsgn,flags		; 'self identifying', please
	br	Z4a

plus:	sbitb	plssgn,flags		; always print sign for floats
	br	Z4a

minus:	sbitb	minsgn,flags		; left justification, please
	br	Z4a

gnum0:	tbitb	ndfnd,flags
	bfs	gnum
	tbitb	prec,flags		; ignore when reading precision
	bfs	gnump
	sbitb	zfill,flags		; leading zero fill, please

gnum:	tbitb	prec,flags
	bfs	gnump
	movd	width,r7
	addr	(r7)[r7:d],r7		; width * 5
	addr	-"0"(r0)[r7:w],width	; width = 2*width + r0 - '0'
	sbitb	ndfnd,flags		; digit seen
	br	Z4a

gnump:	
	movd	ndigit,r7
	addr	(r7)[r7:d],r7		; ndigit *= 5
	addr	-"0"(r0)[r7:w],ndigit	; ndigit = 2*ndigit + r0 - '0'

gnumd:	sbitb	ndfnd,flags		; digit seen
	br	Z4a

dot:	movqd	0,ndigit		; start on the precision
	sbitb	prec,flags
	cbitb	ndfnd,flags
	br	Z4a

indir:	tbitb	prec,flags
	bfs	in1
	movd	0(ap),width		; width specified by parameter
	addqd	4,ap
	cmpqd	0,width
	ble	gnumd
	ibitb	minsgn,flags		; parameterized left adjustment
	negd	width,width
	br	gnumd

in1:	movd	0(ap),ndigit		; precision specified by parameter
	addqd	4,ap
	cmpqd	0,ndigit
	ble	gnumd
	negd	ndigit,ndigit
	br	gnumd

remote:
	movd	0(ap),r0
	addqd	4,ap
	movd	file,tos		; FILE arg
	addr	4(r0),tos		; arglist
	movd	0(r0),tos		; fmt string
	cxp	__doprnt
	adjspb	-12
	br	loop

	.endproc
	.static
temp_long:	.blkl
temp_fl:	.blkd 9
control:	.blkb 100
;	.import _cnvf
	.program
;        0 : longint.il                      
;        4 : longint.ih                      
_pchar.b:	; Level:      2
	enter   0
;Line 25 module _real
	movd    -12(r6),r1
	movd    36(r6),r0
	movb    8(fp),-1(r0)[r1:B]
	addqd   1,-12(r6)
	exit    
	ret     4
;	_c	8(fp)
_append.c:	; Level:      2
	enter   [r3],0
;Line 30 module _real
	movd    8(fp),r2
	movqd   0,r3
L23:
	cmpqd   0,r2
	blt     L24
L26:
	cmpqd   0,r3
	blt     L27
	exit    [r3]
	ret     4
;	_j	r3
;	_m	r2
;	_n	8(fp)
L27:
;Line 35 module _real
;Line 35 module _real
;;;;;;	movd    r6,tos
	movd    r3,r0
	movd    -112(r6)[r0:D],r0
	addd    48,r0
	movd    r0,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
	addqd   -1,r3
	br      L26
L24:
;Line 32 module _real
;Line 32 module _real
	addqd   1,r3
	movd    r3,r1
	movd    r2,r0
	modd    10,r0
	movd    r0,-112(r6)[r1:D]
	quod    10,r2
	br      L23
_percent_s.d:	; Level:      2
	enter   0
;Line 42 module _real
;;;;;;	movd    r6,tos
	addr    @37,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
;;;;;;	movd    r6,tos
	addr    @115,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
	movd    -16(r6),r1
	movd    40(r6),r0
	movd    8(fp),-4(r0)[r1:D]
	addqd   1,-16(r6)
	exit    
	ret     4
;	_x	8(fp)
_percent_d.e:	; Level:      2
	enter   0
;Line 46 module _real
	cmpqd   0,8(fp)
	bge     L34
;Line 48 module _real
;Line 48 module _real
;;;;;;	movd    r6,tos
	addr    @37,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
;;;;;;	movd    r6,tos
	addr    @48,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
;;;;;;	movd    r6,tos
	movd    8(fp),tos
	bsr     _append.c
;;;;;;	adjspb  -4
;;;;;;	movd    r6,tos
	addr    @100,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
	movd    -16(r6),r1
	movd    40(r6),r0
	movqd   0,-4(r0)[r1:D]
	addqd   1,-16(r6)
L34:
	exit    
	ret     4
;	_w	8(fp)
_percent_c.f:	; Level:      2
	enter   0
;Line 52 module _real
	cmpqd   0,8(fp)
	bge     L41
;Line 54 module _real
;Line 54 module _real
;;;;;;	movd    r6,tos
	addr    @37,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
;;;;;;	movd    r6,tos
	movd    8(fp),tos
	bsr     _append.c
;;;;;;	adjspb  -4
;;;;;;	movd    r6,tos
	addr    @99,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
	movd    -16(r6),r1
	movd    40(r6),r0
	addr    @32,-4(r0)[r1:D]
	addqd   1,-16(r6)
L41:
	exit    
	ret     4
;	_w	8(fp)
_putf.g:	; Level:      2
	enter   0
;Line 59 module _real
	cmpqd   0,-32(r6)
	bge     L54
;Line 59 module _real
;;;;;;	movd    r6,tos
	movd    -8(r6),tos
	bsr     _percent_s.d
;;;;;;	adjspb  -4
L54:
;;;;;;	movd    r6,tos
	movd    -28(r6),tos
	bsr     _percent_d.e
;;;;;;	adjspb  -4
	cmpqd   0,16(r6)
	beq     L48
;Line 62 module _real
;Line 62 module _real
;;;;;;	movd    r6,tos
	addr    @46,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
;;;;;;	movd    r6,tos
	movd    -24(r6),tos
	bsr     _percent_d.e
;;;;;;	adjspb  -4
	cmpqd   0,-20(r6)
	bge     L58
;Line 64 module _real
;;;;;;	movd    r6,tos
	movd    -4(r6),tos
	bsr     _percent_s.d
;;;;;;	adjspb  -4
L58:
;;;;;;	movd    r6,tos
	movd    -40(r6),tos
	bsr     _percent_d.e
;;;;;;	adjspb  -4
L48:
	exit    
	ret     0
_rounds.h:	; Level:      2
	enter   0
;Line 72 module _real
	movd    8(fp),r0
	addqd   1,r0
	movzbw  -69(r6)[r0:B],r0
	addqw   5,r0
	movw    r0,r1
	movd    8(fp),r0
	addqd   1,r0
	movb    r1,-69(r6)[r0:B]
	movd    8(fp),r2
	addqd   1,r2
L64:
	movd    r2,r0
	cmpb    57,-69(r6)[r0:B]
	blo     L65
	exit    
	ret     4
;	_ir	r2
;	_sz	8(fp)
L65:
;Line 75 module _real
;Line 75 module _real
	movd    r2,r0
	movb    48,-69(r6)[r0:B]
	addqd   -1,r2
	movd    r2,r0
	movd    r2,r1
	movb    -69(r6)[r0:B],r0
	addqb   1,r0
	movb    r0,-69(r6)[r1:B]
	br      L64
_pickw.i:	; Level:      2
	enter   [r3,r4],0
;Line 82 module _real
	cmpd    18,8(fp)
	bge     L73
;Line 83 module _real
;Line 83 module _real
	movd    8(fp),r0
	subd    18,r0
	movd    r0,-28(r6)
	addr    @18,8(fp)
L73:
	movd    -12(r6),r0
	movd    8(fp),r4
	movd    r0,r3
	cmpd    r3,r4
	bgt     L76
L75:
;Line 85 module _real
	movd    r3,r0
	movd    r3,r1
	subd    -12(r6),r1
	addqd   1,r1
	movd    -8(r6),r2
	movb    -69(r6)[r0:B],-1(r2)[r1:B]
	cmpd    r3,r4
	bge     L76
	addqd   1,r3
	br      L75
L76:
	movd    8(fp),r0
	subd    -12(r6),r0
	addqd   2,r0
	movd    -8(r6),r1
	movqb   0,-1(r1)[r0:B]
	movd    8(fp),r0
	subd    -12(r6),r0
	addqd   1,r0
	movd    r0,-32(r6)
	exit    [r3,r4]
	ret     4
;	_jp	r3
;	_toind	8(fp)
_pickf.j:	; Level:      2
	enter   [r3,r4,r5],0
;Line 93 module _real
	cmpd    -16(r6),8(fp)
	blt     L77
;Line 94 module _real
;Line 94 module _real
	movd    8(fp),r0
	subd    -36(r6),r0
	addqd   -1,r0
	movd    r0,-24(r6)
	movd    16(r6),r0
	subd    -24(r6),r0
	movd    r0,r1
	addr    @19,r0
	subd    8(fp),r0
	cmpd    r1,r0
	blt     L84
;Line 98 module _real
;Line 98 module _real
	addr    @19,r4
	subd    8(fp),r4
	br      L85
L84:
;Line 96 module _real
;Line 96 module _real
	movd    16(r6),r0
	subd    -24(r6),r0
	movd    r0,r4
L85:
	movd    r4,r5
	movqd   1,r3
	cmpd    r3,r5
	bgt     L89
L88:
;Line 100 module _real
	movd    r3,r0
	addqd   -1,r0
	addd    8(fp),r0
	movd    r3,r1
	movd    -4(r6),r2
	movb    -69(r6)[r0:B],-1(r2)[r1:B]
	cmpd    r3,r5
	bge     L89
	addqd   1,r3
	br      L88
L89:
	movd    r4,r0
	addqd   1,r0
	movd    r0,r1
	movd    -4(r6),r0
	movqb   0,-1(r0)[r1:B]
	movd    r4,-20(r6)
L77:
	exit    [r3,r4,r5]
	ret     4
;	_flen	r4
;	_jf	r3
;	_fromind	8(fp)
_cnvf:	; Level:      1
	enter   [r3,r4,r5,r6],108
	addr	0(fp),r6
;Line 107 module _real
	movqd   1,r4
	cmpl    24(fp),l'0000000000000000
	bge     L95
;Line 108 module _real
;Line 108 module _real
	movqd   0,r4
	negl    24(fp),24(fp)
L95:
	movqd   0,-36(fp)
	cmpl    24(fp),l'0000000000000000
	beq     L96
;Line 112 module _real
;Line 112 module _real
	addr    24(fp),tos
	addr    -36(fp),tos
	bsr     _changebase
	movl    24(fp),tos
	addr    -48(fp),tos
	bsr     _convert
	addr    @9,r1
	cmpqd   1,r1
	bgt     L102
L101:
;Line 115 module _real
;Line 115 module _real
	movd    -44(fp),r0
	modd    10,r0
	movd    r0,r2
	addd    48,r2
	movd    r1,r0
	movb    r2,-69(fp)[r0:B]
	quod    10,-44(fp)
	cmpqd   1,r1
	bge     L102
	addqd   -1,r1
	br      L101
L102:
	addr    @18,r1
	cmpd    10,r1
	bgt     L97
L104:
;Line 118 module _real
;Line 118 module _real
	movd    -48(fp),r0
	modd    10,r0
	movd    r0,r2
	addd    48,r2
	movd    r1,r0
	movb    r2,-69(fp)[r0:B]
	quod    10,-48(fp)
	cmpd    10,r1
	bge     L97
	addqd   -1,r1
	br      L104
	br      L97
L96:
;Line 110 module _real
	movmd   L99,-68(fp),4
	movmd   L99+16,-52(fp),1
L97:
	cmpd    103,32(fp)
	bne     L107
;Line 122 module _real
;Line 122 module _real
	movqd   1,r1
L108:
	movd    r1,r0
	cmpb    48,-69(fp)[r0:B]
	beq     L109
	movd    -36(fp),r0
	addd    18,r0
	addqd   1,r0
	subd    r1,r0
	movd    r0,r1
	cmpqd   -4,r1
	bge     L113
	cmpd    r1,16(fp)
	ble     L111
L113:
;Line 128 module _real
;Line 128 module _real
	addqd   -1,16(fp)
	addr    @101,32(fp)
	br      L107
L111:
;Line 126 module _real
;Line 126 module _real
	subd    r1,16(fp)
	addr    @102,32(fp)
L107:
	addr    49(36(fp)),r0
	movd    r0,-8(fp)
	cmpd    102,32(fp)
	beq     L115
;Line 170 module _real
;Line 170 module _real
	movb    46,1(-8(fp))
	movd    16(fp),r0
	addqd   3,r0
	movb    101,-1(-8(fp))[r0:B]
	movqd   1,r3
	movb    49,-50(fp)
L142:
	movd    r3,r0
	cmpb    48,-69(fp)[r0:B]
	beq     L143
	cmpd    19,r3
	beq     L145
;Line 180 module _real
;Line 180 module _real
	movd    r3,r0
	addd    16(fp),r0
	cmpd    17,r0
	blt     L152
;Line 180 module _real
;;;;;;	addr    0(fp),tos
	movd    r3,r0
	addd    16(fp),r0
	movd    r0,tos
	bsr     _rounds.h
;;;;;;	adjspb  -4
L152:
	movd    r3,r0
	addqd   -1,r0
	cmpb    48,-69(fp)[r0:B]
	beq     L154
;Line 181 module _real
	addqd   -1,r3
L154:
	movd    r3,r0
	movb    -69(fp)[r0:B],0(-8(fp))
	movd    16(fp),r5
	addqd   2,r5
	movqd   3,r2
	cmpd    r2,r5
	bgt     L146
L156:
;Line 184 module _real
	movd    r2,r0
	addqd   -2,r0
	addd    r3,r0
	cmpd    19,r0
	bgt     L158
;Line 185 module _real
	movd    r2,r0
	movb    48,-1(-8(fp))[r0:B]
	br      L159
L158:
;Line 184 module _real
	movd    r2,r0
	addqd   -2,r0
	addd    r3,r0
	movd    r2,r1
	movb    -69(fp)[r0:B],-1(-8(fp))[r1:B]
L159:
	cmpd    r2,r5
	bge     L146
	addqd   1,r2
	br      L156
	br      L146
L145:
;Line 176 module _real
;Line 176 module _real
	movb    48,0(-8(fp))
	movd    16(fp),r2
	addqd   2,r2
	movqd   3,r1
	cmpd    r1,r2
	bgt     L150
L149:
;Line 177 module _real
	movd    r1,r0
	movb    48,-1(-8(fp))[r0:B]
	cmpd    r1,r2
	bge     L150
	addqd   1,r1
	br      L149
L150:
	movqd   1,-36(fp)
L146:
	movd    -36(fp),r0
	subd    r3,r0
	movd    r0,-36(fp)
	addd    18,-36(fp)
	cmpqd   0,-36(fp)
	bgt     L161
;Line 189 module _real
	movd    16(fp),r0
	addqd   4,r0
	movb    43,-1(-8(fp))[r0:B]
	br      L162
L161:
;Line 188 module _real
;Line 188 module _real
	movd    16(fp),r0
	addqd   4,r0
	movb    45,-1(-8(fp))[r0:B]
	negd    -36(fp),-36(fp)
L162:
	movd    16(fp),r0
	addqd   7,r0
	movd    16(fp),r2
	addqd   5,r2
	movd    r0,r1
	cmpd    r2,r1
	bgt     L166
L165:
;Line 191 module _real
;Line 191 module _real
	movd    -36(fp),r0
	modd    10,r0
	movd    r0,r3
	addd    48,r3
	movd    r1,r0
	movb    r3,-1(-8(fp))[r0:B]
	quod    10,-36(fp)
	cmpd    r2,r1
	bge     L166
	addqd   -1,r1
	br      L165
L166:
	movd    16(fp),r0
	addd    8,r0
	movqb   0,-1(-8(fp))[r0:B]
	movqd   1,-32(fp)
L167:
	movd    -32(fp),r0
	cmpqb   0,-1(-8(fp))[r0:B]
	beq     L169
;Line 197 module _real
	addqd   1,-32(fp)
	br      L167
L169:
	addqd   -1,-32(fp)
	br      L116
L143:
;Line 174 module _real
	addqd   1,r3
	br      L142
L115:
;Line 133 module _real
;Line 133 module _real
	addr    74(36(fp)),r0
	movd    r0,-4(fp)
	addd    18,-36(fp)
	movqd   1,r1
	movb    49,-50(fp)
L118:
	movd    r1,r0
	cmpb    48,-69(fp)[r0:B]
	beq     L119
	movqd   0,-28(fp)
	movqd   0,-32(fp)
	movqd   0,-24(fp)
	movqd   0,-20(fp)
	cmpd    18,r1
	blt     L122
;Line 143 module _real
;Line 143 module _real
	movd    -36(fp),r3
	addd    16(fp),r3
	cmpqd   1,r3
	bgt     L124
	cmpd    18,r3
	ble     L124
;Line 145 module _real
;Line 145 module _real
;;;;;;	addr    0(fp),tos
	movd    r3,tos
	bsr     _rounds.h
;;;;;;	adjspb  -4
	movqd   1,r1
L126:
	movd    r1,r0
	cmpb    48,-69(fp)[r0:B]
	beq     L127
L124:
	movd    r3,-16(fp)
	cmpd    -36(fp),r1
	blt     L129
;Line 152 module _real
;Line 152 module _real
	movd    r1,-12(fp)
	cmpd    18,-36(fp)
	bgt     L132
;Line 155 module _real
;;;;;;	addr    0(fp),tos
	movd    -36(fp),tos
	bsr     _pickw.i
;;;;;;	adjspb  -4
	br      L130
L132:
;Line 154 module _real
;Line 154 module _real
;;;;;;	addr    0(fp),tos
	movd    -36(fp),tos
	bsr     _pickw.i
;;;;;;	adjspb  -4
;;;;;;	addr    0(fp),tos
	movd    -36(fp),r0
	addqd   1,r0
	movd    r0,tos
	bsr     _pickf.j
;;;;;;	adjspb  -4
	br      L122
L129:
;Line 150 module _real
;;;;;;	addr    0(fp),tos
	movd    r1,tos
	bsr     _pickf.j
;;;;;;	adjspb  -4
L130:
L122:
	movd    -28(fp),r0
	addd    -32(fp),r0
	cmpqd   0,r0
	bne     L136
;Line 159 module _real
	movqd   1,-28(fp)
L136:
	movqd   0,-40(fp)
	cmpqd   0,16(fp)
	beq     L137
;Line 163 module _real
;Line 163 module _real
	movd    -24(fp),r0
	addqd   1,r0
	movd    r0,r3
	addd    -20(fp),r3
	movd    16(fp),r0
	addqd   1,r0
	cmpd    r3,r0
	bge     L138
;Line 165 module _real
;Line 165 module _real
	movd    16(fp),r0
	addqd   1,r0
	movd    r0,-40(fp)
	subd    r3,-40(fp)
	movd    16(fp),r3
	addqd   1,r3
	br      L116
L137:
;Line 161 module _real
	movqd   0,r3
L138:
L116:
	movqd   1,-12(fp)
	movqd   1,-16(fp)
	cmpqd   0,12(fp)
	beq     L172
;Line 201 module _real
;Line 201 module _real
	cmpqd   0,r4
	bne     L174
;Line 201 module _real
;Line 201 module _real
;;;;;;	addr    0(fp),tos
	addr    @45,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
	addqd   -1,20(fp)
L174:
	cmpd    102,32(fp)
	beq     L175
;Line 206 module _real
;Line 206 module _real
;;;;;;	addr    0(fp),tos
	movd    -8(fp),tos
	bsr     _percent_s.d
;;;;;;	adjspb  -4
;;;;;;	addr    0(fp),tos
	movd    20(fp),r0
	subd    -32(fp),r0
	movd    r0,tos
	bsr     _percent_c.f
;;;;;;	adjspb  -4
	br      L171
L175:
;Line 203 module _real
;Line 203 module _real
;;;;;;	addr    0(fp),tos
	bsr     _putf.g
;;;;;;	adjspb  -4
;;;;;;	addr    0(fp),tos
	movd    -32(fp),r0
	addd    -28(fp),r0
	addd    r3,r0
	movd    20(fp),r1
	subd    r0,r1
	movd    r1,tos
	bsr     _percent_c.f
;;;;;;	adjspb  -4
	br      L171
L172:
;Line 210 module _real
;Line 210 module _real
	cmpqd   0,8(fp)
	beq     L180
;Line 211 module _real
;Line 211 module _real
	cmpqd   0,r4
	bne     L182
;Line 211 module _real
;Line 211 module _real
;;;;;;	addr    0(fp),tos
	addr    @45,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
	addqd   -1,20(fp)
L182:
	cmpd    102,32(fp)
	beq     L183
;Line 214 module _real
;;;;;;	addr    0(fp),tos
	movd    20(fp),r0
	subd    -32(fp),r0
	movd    r0,tos
	bsr     _percent_d.e
;;;;;;	adjspb  -4
	br      L179
L183:
;Line 213 module _real
;;;;;;	addr    0(fp),tos
	movd    -32(fp),r0
	addd    -28(fp),r0
	addd    r3,r0
	movd    20(fp),r1
	subd    r0,r1
	movd    r1,tos
	bsr     _percent_d.e
;;;;;;	adjspb  -4
	br      L179
L180:
;Line 216 module _real
;Line 216 module _real
	cmpqd   0,r4
	bne     L187
;Line 216 module _real
	addqd   -1,20(fp)
L187:
	cmpd    102,32(fp)
	beq     L188
;Line 219 module _real
;;;;;;	addr    0(fp),tos
	movd    20(fp),r0
	subd    -32(fp),r0
	movd    r0,tos
	bsr     _percent_c.f
;;;;;;	adjspb  -4
	br      L189
L188:
;Line 218 module _real
;;;;;;	addr    0(fp),tos
	movd    -32(fp),r0
	addd    -28(fp),r0
	movd    r0,r1
	addd    r3,r1
	movd    20(fp),r0
	subd    r1,r0
	movd    r0,tos
	bsr     _percent_c.f
;;;;;;	adjspb  -4
L189:
	cmpqd   0,r4
	bne     L179
;Line 221 module _real
;;;;;;	addr    0(fp),tos
	addr    @45,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
L179:
	cmpd    102,32(fp)
	beq     L193
;Line 225 module _real
;;;;;;	addr    0(fp),tos
	movd    -8(fp),tos
	bsr     _percent_s.d
;;;;;;	adjspb  -4
	br      L171
L193:
;Line 225 module _real
;;;;;;	addr    0(fp),tos
	bsr     _putf.g
;;;;;;	adjspb  -4
L171:
;;;;;;	addr    0(fp),tos
	movqd   0,tos
	bsr     _pchar.b
;;;;;;	adjspb  -4
	exit    [r3,r4,r5,r6]
	ret     28
;	_jt8	r3
;	_it7	r1
;	_it6	r2
;	_it5	r1
;	_jt4	r3
;	_it3	r1
;	_jt2	r1
;	_it1	r1
;	_frsz	r3
;	_signplus	r4
;	_zfill	8(fp)
;	_leftj	12(fp)
;	_prec	16(fp)
;	_width	20(fp)
;	_r	24(fp)
;	_sel	32(fp)
;	_control	36(fp)
;	_arglist	40(fp)
;	_outpf	-4(fp)
;	_outpw	-8(fp)
;	_ii	-12(fp)
;	_j	-16(fp)
;	_frst	-20(fp)
;	_frpr	-24(fp)
;	_whsc	-28(fp)
;	_whst	-32(fp)
;	_ex	-36(fp)
;	_frsc	-40(fp)
;	_li	-48(fp)
;	_st18	-68(fp)
;	_apx	-108(fp)
L99:
	.byte   '00000000000000000000',0
L127:
;Line 147 module _real
	addqd   1,r1
	br      L126
L119:
;Line 139 module _real
	addqd   1,r1
	br      L118
L109:
;Line 123 module _real
	addqd   1,r1
	br      L108
;--------------------------------------------------------------
 
; procedure changebase(var qud:real;  var ex:integer);
; (abs(res)<2**56) and (abs(res)>=2**52) and (qud * (10**ex) remains const)

qud:	.equ 0(12(fp)) 
ex:	.equ 0(8(fp))
ex_base:.equ 1023			; exponent shifted by ex_base

_changebase: enter [r3,r4],0
	tbitd 	31,qud+4		; store sign bit in r0
	bfs 	s1
	movqd 	0,r0
	br 	s2
s1:	movqd 	1,r0
s2:	cbitd 	31,qud+4		; clear sign bit
	addr	@20,r2
	extd 	r2,qud+4,r1,11		; extract exponent
	insd 	r2,ex_base+52,qud+4,11	; set actual base to 0
	cmpd 	r1,ex_base+52		; negative exponent?
	bge 	pos_base
	negd	r1,r1	
	addd	ex_base+52,r1		; qud = fraction * ( 2 ** -r1)
	movqd 	-1,r2
l0:	cmpqd	0,r1
	beq	done
	tbitd	0,r1			; is r1 odd?
	bfc	l3
	addr	tab_pw,r3		;multiply by the tab_pw[r2]
	movd	r2,r4
	muld	8,r4
	addd 	r4,r3
	mull	0(r3),qud
	addd	tab_ex[r2:d],ex		; update exponent
	cmpl	two_p_56,qud		; is new fraction >10.0
	bgt	l3
	addqd	1,ex			; increment exponent
	divl	ten,qud			; adjust qud
l3:	addqd	-1,r2			; next index
	lshd	-1,r1			; next bit of exponent
	br 	l0

pos_base:
	subd	ex_base+52,r1
	movqd 	1,r2
m0:	cmpqd	0,r1
	beq	done
	tbitd	0,r1
	bfc	m3
	addr	tab_pw,r3
	movd 	r2,r4
	muld	8,r4
	addd	r4,r3
	mull	0(r3),qud
	addd	tab_ex[r2:d],ex
	cmpl	two_p_56,qud
	bgt	m3
	addqd	1,ex
	divl	ten,qud
m3:	addqd	1,r2
	lshd	-1,r1
	br	m0
	
done:	cmpqd 	0,r0
	beq	d0
	mull	minusone,qud
d0:	exit	[r3,r4]
	ret 	8

ten:	.long 10.0
minusone: .long -1.0
two_p_56:
	.double h'00000000
	.double	h'43700000
	.long	5.56268464626800345772519	; -11
	.long	7.45834073120020674329071	; -10
	.long	8.63616855509444462538621	; -9
	.long	2.93873587705571876992182	; -8
	.long	5.42101086242752217003726	; -7
	.long	2.32830643653869628906250	; -6
	.long	1.52587890625			; -5
	.long	3.90625				; -4
	.long	6.25				; -3
	.long	2.5				; -2
	.long	5.0				; -1
tab_pw:	.long	1.0				; 0
	.long	2.0				; 1
	.long	4.0				; 2
	.long	1.6				; 3
	.long	2.56				; 4
	.long	6.5536				; 5
	.long	4.294967296			; 6
	.long	1.8446744073709551616		; 7
	.long	3.40282366920938463463374	; 8
	.long	1.15792089237316195423569	; 9
	.long	1.34078079299425970995735	; 10
	.long	1.79769313486231590772915	; 11

	.double	-309	;-11
	.double	-155	;-10
	.double	-78	;-9
	.double	-39	;-8
	.double	-20	;-7
	.double	-10	;-6
	.double	-5	;-5
	.double -3	;-4
	.double	-2	;-3
	.double -1	;-2
	.double -1	;-1
tab_ex:	.double 0	;0
	.double 0	;1
	.double 0	;2
	.double 1	;3
	.double 2	;4
	.double 4	;5
	.double 9	;6
	.double 19	;7
	.double 38	;8
	.double 77	;9
	.double 154	;10
	.double 308	;11
;--------------------------------------------------------------------------
; type longint=record il,ih :integer; ! 9 digits in each of them
; procedure convert(r:real; var i:longint);

r:	.equ 12(fp)
i:	.equ 0(8(fp))
p_10_9:	.equ 1000000000

_convert:
	enter	[r3],0
	addr	@20,r2
	extd	r2,r+4,r1,11	;extract exponent
	insd	r2,1,r+4,11	;insert 1 in exponent field. The hidden "1"
	movqd	0,i
	movqd 	0,i+4
	addr	@21,r0
lp1:	addqd	-1,r0
	cmpd	r0,0		; r0 <0 ?
	blt	ex1
	bsr	double
	tbitd	r0,r+4		;is bit 1
	bfc	lp1
	bsr	addone
	br	lp1
ex1:	addr	@32,r0
lp2:	addqd	-1,r0
	cmpd	r0,0
	blt	ex2
	bsr	double
	tbitd	r0,r
	bfc	lp2
	bsr 	addone
	br 	lp2
ex2:	cmpd	r1,ex_base+52
	ble	done1
	bsr	double	
	subd	1,r1
	br	ex2
done1:	exit	[r3]
	ret 	12

double:
	movd	0,r3
	addd	i,i
	cmpd	p_10_9,i
	bgt	dm
	movd	1,r3
	subd	p_10_9,i
dm:	addd	i+4,i+4
	addd	r3,i+4
	ret	0

addone:
	addqd	1,i
	cmpd	p_10_9,i
	bgt	af
	subd	p_10_9,i
	addqd	1,i+4
af:	ret	0
