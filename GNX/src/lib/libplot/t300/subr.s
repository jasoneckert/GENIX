.module	subr.c
LL0:
.program
.endseg
.static
; _iob == undefined
; xnow == undefined
; ynow == undefined
; OUTF == undefined
; ITTY == undefined
; PTTY == undefined
; HEIGHT == undefined
; WIDTH == undefined
; OFFSET == undefined
; xscale == undefined
; xoffset == undefined
; yscale == undefined
; botx == undefined
; boty == undefined
; obotx == undefined
; oboty == undefined
; scalex == undefined
; scaley == undefined
.endseg
.program
.exportp	_abval
_abval::
	br 	L37	;bfcode
L38:
;_q:	.EQU	12(fp)
	cmpqd	0,12(fp)	;35
	bgt	L9999
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	br	L9998
L9999:
	;intareg 114h...
	negd	12(fp),r0	;emit_uop
L9998:
	br	L36
L36:
	exit	
	rxp	0
L37:
	enter	0
	br 	L38
.endseg
.static
.endseg
.program
.exportp	_xconv
_xconv::
	br 	L42	;bfcode
L43:
;_xp:	.EQU	12(fp)
	addd	2048,12(fp)	;105q
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	quod	_xscale,r0	;105r
	addd	_xoffset,r0	;105r
	br	L41
L41:
	exit	
	rxp	0
L42:
	enter	0
	br 	L43
.endseg
.static
.endseg
.program
.exportp	_yconv
_yconv::
	br 	L47	;bfcode
L48:
;_yp:	.EQU	12(fp)
	addd	2048,12(fp)	;105q
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	quod	_yscale,r0	;105r
	br	L46
L46:
	exit	
	rxp	0
L47:
	enter	0
	br 	L48
.endseg
.static
.endseg
.program
.exportp	_inplot
_inplot::
	br 	L52	;bfcode
L53:
	;unary and,forarg 115a...
	addr	_PTTY,tos	;setasg
;68a
	;forarg 115d...
	movd	_OUTF,tos	;59b
	cxp	_stty	;44a
	adjspb	-8
	;forarg 115d...
	movqd	6,tos	;59a
	cxp	_spew	;44a
	adjspb	-4
L51:
	exit	
	rxp	0
L52:
	enter	0
	br 	L53
.endseg
.static
.endseg
.program
.exportp	_outplot
_outplot::
	br 	L59	;bfcode
L60:
	;forarg 115d...
	addr	@27,tos	;59a
	cxp	_spew	;44a
	adjspb	-4
	;forarg 115d...
	movqd	6,tos	;59a
	cxp	_spew	;44a
	adjspb	-4
	;unary and,forarg 115a...
	addr	__iob+16,tos	;setasg
;68a
	cxp	_fflush	;44a
	adjspb	-4
	;unary and,forarg 115a...
	addr	_ITTY,tos	;setasg
;68a
	;forarg 115d...
	movd	_OUTF,tos	;59b
	cxp	_stty	;44a
	adjspb	-8
L58:
	exit	
	rxp	0
L59:
	enter	0
	br 	L60
.endseg
.static
.endseg
.program
.exportp	_spew
_spew::
	br 	L64	;bfcode
L65:
;_ch:	.EQU	12(fp)
	addr	@11,r0
	cmpd	r0,12(fp)	;37a2
	bne	L66
	addqd	-1,__iob+16	;87c
	cmpqd	0,__iob+16	;35
	bgt	L9997
	;intareg 114i...
	movd	__iob+20,r0	;59b
	addqd	1,__iob+20	;83
	movb	27,0(r0)	;59a
	;27 intareg(nasl) 114b...
	movxbd	27,r0	;59a
	br	L9996
L9997:
	;unary and,forarg 115a...
	addr	__iob+16,tos	;setasg
;68a
	;forarg 115d...
	addr	@27,tos	;59a
	cxp	__flsbuf	;44a
	adjspb	-8
L9996:
L66:
	addqd	-1,__iob+16	;87c
	cmpqd	0,__iob+16	;35
	bgt	L9995
	;intareg 114i...
	movd	__iob+20,r0	;59b
	addqd	1,__iob+20	;83
	movb	12(fp),0(r0)	;59b
	;0(r0) intareg(nasl) 114b...
	movxbd	0(r0),r0	;59b
	br	L9994
L9995:
	;unary and,forarg 115a...
	addr	__iob+16,tos	;setasg
;68a
	;forarg 115d...
	movd	12(fp),tos	;59b
	cxp	__flsbuf	;44a
	adjspb	-8
L9994:
L63:
	exit	
	rxp	0
L64:
	enter	0
	br 	L65
.endseg
.static
.endseg
.program
.exportp	_tobotleft
_tobotleft::
	br 	L71	;bfcode
L72:
	;forarg 115d...
	movxwd	-2048,tos	;59a
	;forarg 115d...
	movxwd	-2048,tos	;59a
	cxp	_move	;44a
	adjspb	-8
L70:
	exit	
	rxp	0
L71:
	enter	0
	br 	L72
.endseg
.static
.endseg
.program
.exportp	_reset
_reset::
	br 	L77	;bfcode
L78:
	cxp	_outplot	;44a
	cxp	_exit	;44a
L76:
	exit	
	rxp	0
L77:
	enter	0
	br 	L78
.endseg
.static
.endseg
.program
.exportp	_dist2
_dist2::
	br 	L83	;bfcode
L84:
;_x1:	.EQU	12(fp)
;_y1:	.EQU	16(fp)
;_x2:	.EQU	20(fp)
;_y2:	.EQU	24(fp)
;_t:	.EQU	-4(fp)
;_v:	.EQU	-8(fp)
	;20(fp) intareg(nasl) 114b...
	movd	20(fp),r0	;59b
	subd	12(fp),r0	;105r
	movdf	r0,-4(fp)	;setasg
	;16(fp) intareg(nasl) 114b...
	movd	16(fp),r0	;59b
	subd	24(fp),r0	;105r
	movdf	r0,-8(fp)	;setasg
	;-4(fp) intbreg(nbsl) 114c...
	movfl	-4(fp),f0	;59b
	movfl	-4(fp),f2	;109
	mull	f2,f0
	;-8(fp) intbreg(nbsl) 114c...
	movfl	-8(fp),f2	;59b
	movfl	-8(fp),f4	;109
	mull	f4,f2
	addl	f2,f0	;111
	movlf	f0,f0
	movfl	f0,f0	;2c
	br	L82
L82:
	movl	tos,f4
	exit	
	rxp	0
L83:
	enter	8
	movl	f4,tos
	br 	L84
.endseg
.static
.endseg
.program
.exportp	_swap
_swap::
	br 	L88	;bfcode
L89:
;_pa:	.EQU	12(fp)
;_pb:	.EQU	16(fp)
;_t:	.EQU	-4(fp)
	movd	0(12(fp)),-4(fp)	;59b
	movd	0(16(fp)),0(12(fp))	;59b
	movd	-4(fp),0(16(fp))	;59b
L87:
	exit	
	rxp	0
L88:
	enter	4
	br 	L89
.endseg
.static
.endseg
.program
.exportp	_movep
_movep::
	br 	L93	;bfcode
L94:
;_xg:	.EQU	12(fp)
;_yg:	.EQU	16(fp)
;_i:	.EQU	-4(fp)
;_ch:	.EQU	-8(fp)
	cmpd	12(fp),_xnow	;38
	bne	L95
	cmpd	16(fp),_ynow	;38
	bne	L95
	br	L92
L95:
	;_xnow intareg(nasl) 114b...
	movd	_xnow,r0	;59b
	quod	2,r0	;105r
	cmpd	r0,12(fp)	;38
	ble	L96
	;forarg 115d...
	addr	@13,tos	;59a
	cxp	_spew	;44a
	adjspb	-4
	movqd	0,_xnow	;59a
L96:
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	subd	_xnow,r0	;105r
	;intbreg 114g...
	movdl	r0,f0	;setasg
	divl	l'4018000000000000,f0	;111
	truncld	f0,-4(fp)	;setasg
	cmpd	_xnow,12(fp)	;38
	bge	L97
	addr	@32,-8(fp)	;59a
	br	L98
L97:
	addr	@8,-8(fp)	;59a
L98:
	;intbreg 114g...
	movdl	-4(fp),f0	;setasg
	mull	l'4018000000000000,f0	;111
	;intareg 114h...
	truncld	f0,r0	;setasg
	addd	r0,_xnow	;105s
	;forarg 115d...
	movd	-4(fp),tos	;59b
	cxp	_abval	;44a
	adjspb	-4
	movd	r0,-4(fp)	;59a
L99:
	;intareg 114j...
	movd	-4(fp),r0	;59b
	addqd	-1,-4(fp)	;87c
	cmpqd	0,r0	;35
	beq	L100
	;forarg 115d...
	movd	-8(fp),tos	;59b
	cxp	_spew	;44a
	adjspb	-4
	br	L99
L100:
	;forarg 115d...
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	subd	_xnow,r0	;105r
	movd	r0,tos	;59a
	cxp	_abval	;44a
	adjspb	-4
	movd	r0,-4(fp)	;59a
	cxp	_inplot	;44a
L101:
	;intareg 114j...
	movd	-4(fp),r0	;59b
	addqd	-1,-4(fp)	;87c
	cmpqd	0,r0	;35
	beq	L102
	;forarg 115d...
	movd	-8(fp),tos	;59b
	cxp	_spew	;44a
	adjspb	-4
	br	L101
L102:
	cxp	_outplot	;44a
	;16(fp) intareg(nasl) 114b...
	movd	16(fp),r0	;59b
	subd	_ynow,r0	;105r
	;intbreg 114g...
	movdl	r0,f0	;setasg
	divl	l'4020000000000000,f0	;111
	truncld	f0,-4(fp)	;setasg
	cmpd	_ynow,16(fp)	;38
	bge	L103
	addr	@11,-8(fp)	;59a
	br	L104
L103:
	addr	@10,-8(fp)	;59a
L104:
	;intbreg 114g...
	movdl	-4(fp),f0	;setasg
	mull	l'4020000000000000,f0	;111
	;intareg 114h...
	truncld	f0,r0	;setasg
	addd	r0,_ynow	;105s
	;forarg 115d...
	movd	-4(fp),tos	;59b
	cxp	_abval	;44a
	adjspb	-4
	movd	r0,-4(fp)	;59a
L105:
	;intareg 114j...
	movd	-4(fp),r0	;59b
	addqd	-1,-4(fp)	;87c
	cmpqd	0,r0	;35
	beq	L106
	;forarg 115d...
	movd	-8(fp),tos	;59b
	cxp	_spew	;44a
	adjspb	-4
	br	L105
L106:
	;forarg 115d...
	;16(fp) intareg(nasl) 114b...
	movd	16(fp),r0	;59b
	subd	_ynow,r0	;105r
	movd	r0,tos	;59a
	cxp	_abval	;44a
	adjspb	-4
	movd	r0,-4(fp)	;59a
	cxp	_inplot	;44a
L107:
	;intareg 114j...
	movd	-4(fp),r0	;59b
	addqd	-1,-4(fp)	;87c
	cmpqd	0,r0	;35
	beq	L108
	;forarg 115d...
	movd	-8(fp),tos	;59b
	cxp	_spew	;44a
	adjspb	-4
	br	L107
L108:
	cxp	_outplot	;44a
	movd	12(fp),_xnow	;59b
	movd	16(fp),_ynow	;59b
L92:
	exit	
	rxp	0
L93:
	enter	8
	br 	L94
.endseg
.static
.endseg
.program
.exportp	_xsc
_xsc::
	br 	L112	;bfcode
L113:
;_xi:	.EQU	12(fp)
;_xa:	.EQU	-4(fp)
	;intbreg 114g...
	movdl	12(fp),f0	;setasg
	movfl	_obotx,f2	;109
	subl	f2,f0
	movfl	_scalex,f2	;109
	mull	f2,f0
	movfl	_botx,f2	;109
	addl	f2,f0
	truncld	f0,-4(fp)	;setasg
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r0	;59b
	br	L111
L111:
	exit	
	rxp	0
L112:
	enter	4
	br 	L113
.endseg
.static
.endseg
.program
.exportp	_ysc
_ysc::
	br 	L117	;bfcode
L118:
;_yi:	.EQU	12(fp)
;_ya:	.EQU	-4(fp)
	;intbreg 114g...
	movdl	12(fp),f0	;setasg
	movfl	_oboty,f2	;109
	subl	f2,f0
	movfl	_scaley,f2	;109
	mull	f2,f0
	movfl	_boty,f2	;109
	addl	f2,f0
	truncld	f0,-4(fp)	;setasg
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r0	;59b
	br	L116
L116:
	exit	
	rxp	0
L117:
	enter	4
	br 	L118
.endseg
.static
	.import	__iob		;ejobcode
	.import	_xnow		;ejobcode
	.import	_ynow		;ejobcode
	.import	_OUTF		;ejobcode
	.import	_ITTY		;ejobcode
	.import	_PTTY		;ejobcode
	.import	_xscale		;ejobcode
	.import	_xoffset		;ejobcode
	.import	_yscale		;ejobcode
	.import	_botx		;ejobcode
	.import	_boty		;ejobcode
	.import	_obotx		;ejobcode
	.import	_oboty		;ejobcode
	.import	_scalex		;ejobcode
	.import	_scaley		;ejobcode
	.importp _stty		;ejobcode
	.importp _fflush		;ejobcode
	.importp __flsbuf		;ejobcode
	.importp _move		;ejobcode
	.importp _exit		;ejobcode
;NS16032 C compiler release 4.27 8/25/83 (JZ,jima)
	.endseg
