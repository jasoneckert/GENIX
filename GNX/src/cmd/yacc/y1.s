.module	y1.c
LL0:
.program
.endseg
.static
; _iob == undefined
; _ctype_ == undefined
; finput == undefined
; faction == undefined
; fdefine == undefined
; ftable == undefined
; ftemp == undefined
; foutput == undefined
; ntokens == undefined
; tokset == undefined
; toklev == undefined
; nnonter == undefined
; nontrst == undefined
; nprod == undefined
; prdptr == undefined
; levprd == undefined
; nstate == undefined
; pstate == undefined
; tystate == undefined
; defact == undefined
; tstates == undefined
; ntstates == undefined
; mstates == undefined
; lkst == undefined
; nolook == undefined
; wsets == undefined
; cwp == undefined
; mem0 == undefined
; mem == undefined
; amem == undefined
; memp == undefined
; indgo == undefined
; temp1 == undefined
; lineno == undefined
; zzgoent == undefined
; zzgobest == undefined
; zzacent == undefined
; zzexcp == undefined
; zzclose == undefined
; zzrrconf == undefined
; zzsrconf == undefined
; tbitset == undefined
; nlset == external
; 	.align	2
	.export	_nlset
_nlset:
	.double	0	; 14
; 	.align	2
	.export	_nolook
_nolook:
	.double	0	; 14
; clset == undefined
; 	.align	2
	.export	_nstate
_nstate:
	.double	0	; 14
; 	.align	2
	.export	_memp
_memp:
S0:	.blkb 4
 	;16a
; 	.align	2
	.export	_lineno
_lineno:
	.double	1	; 14
; fatfl == external
; 	.align	2
	.export	_fatfl
_fatfl:
	.double	1	; 14
; nerrors == external
; 	.align	2
	.export	_nerrors
_nerrors:
	.double	0	; 14
; pres == undefined
; pfirst == undefined
; pempty == undefined
.endseg
.program
.exportp	_main
_main::
	br 	L72	;bfcode
L73:
;_argc:	.EQU	12(fp)
;_argv:	.EQU	16(fp)
	;forarg 115d...
	movd	16(fp),tos	;59b
	;forarg 115d...
	movd	12(fp),tos	;59b
	cxp	_setup	;44a
	adjspb	-8
	;_ntokens intareg(nasl) 114b...
	movd	_ntokens,r0	;59b
	addr	32(r0),r0	;86e  r0 += 32
	quod	32,r0	;105r
	movd	r0,_tbitset	;59a
	cxp	_cpres	;44a
	cxp	_cempty	;44a
	cxp	_cpfir	;44a
	cxp	_stagen	;44a
	cxp	_output	;44a
	cxp	_go2out	;44a
	cxp	_hideprod	;44a
	cxp	_summary	;44a
	cxp	_callopt	;44a
	cxp	_others	;44a
	;forarg 115d...
	movqd	0,tos	;59a
	cxp	_exit	;44a
	adjspb	-4
L71:
	exit	
	rxp	0
L72:
	enter	0
	br 	L73
.endseg
.static
.endseg
.program
.exportp	_others
_others::
	br 	L88	;bfcode
L89:
;_c:	.EQU r7
;_i:	.EQU r6
;_j:	.EQU r5
	;unary and,forarg 115a...
	addr	L91,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	L90,tos	;setasg
;68a
	cxp	_fopen	;44a
	adjspb	-8
	movd	r0,_finput	;59a
	cmpqd	0,_finput	;35
	bne	L92
	;unary and,forarg 115a...
	addr	L95,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	L94,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-8
L92:
	;forarg 115d...
	movd	_nprod,tos	;59b
	;unary and,forarg 115a...
	addr	_levprd,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	L97,tos	;setasg
;68a
	cxp	_warray	;44a
	adjspb	-12
	;forarg 115d...
	movqd	0,tos	;59a
	;forarg 115d...
	movd	_nprod,tos	;59b
	;unary and,forarg 115a...
	addr	_temp1,tos	;setasg
;68a
	cxp	_aryfil	;44a
	adjspb	-12
	movqd	1,r6	;59a
L101:
	cmpd	r6,_nprod	;38
	bge	L100
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	r6,r0	;105t
	;_prdptr[r0:d] intareg(nasl) 114b...
	movd	_prdptr[r0:d],r0	;59b
	subd	_prdptr[r6:d],r0	;105r
	quod	4,r0	;105r
	addqd	-2,r0	;82
	movd	r0,_temp1[r6:d]	;59a
L99:
	addqd	1,r6	;82
	br	L101
L100:
	;forarg 115d...
	movd	_nprod,tos	;59b
	;unary and,forarg 115a...
	addr	_temp1,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	L102,tos	;setasg
;68a
	cxp	_warray	;44a
	adjspb	-12
	;forarg 115d...
	movxwd	-1000,tos	;59a
	;forarg 115d...
	movd	_nstate,tos	;59b
	;unary and,forarg 115a...
	addr	_temp1,tos	;setasg
;68a
	cxp	_aryfil	;44a
	adjspb	-12
	movqd	1,r6	;59a
L105:
	cmpd	r6,_ntokens	;38
	bgt	L104
	movd	_tstates[r6:d],r5	;59b
L108:
	cmpqd	0,r5	;35
	beq	L107
	;& _tokset intareg 114a...
	addr	_tokset,r0	;setasg
	;r6 intareg(nasl) 114b...
	movd	r6,r1	;59b
	addr	@0[r1:q],r1	;fast r1 <<= 3 ;49b
	addd	r1,r0	;105t
	movd	4(r0),_temp1[r5:d]	;59b
L106:
	movd	_mstates[r5:d],r5	;59b
	br	L108
L107:
L103:
	addqd	1,r6	;82
	br	L105
L104:
	movqd	0,r6	;59a
L111:
	cmpd	r6,_nnonter	;38
	bgt	L110
	movd	_ntstates[r6:d],r5	;59b
L114:
	cmpqd	0,r5	;35
	beq	L113
	negd	r6,_temp1[r5:d]	;emit_uop
;68a
L112:
	movd	_mstates[r5:d],r5	;59b
	br	L114
L113:
L109:
	addqd	1,r6	;82
	br	L111
L110:
	;forarg 115d...
	movd	_nstate,tos	;59b
	;unary and,forarg 115a...
	addr	_temp1,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	L115,tos	;setasg
;68a
	cxp	_warray	;44a
	adjspb	-12
	;forarg 115d...
	movd	_nstate,tos	;59b
	;unary and,forarg 115a...
	addr	_defact,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	L116,tos	;setasg
;68a
	cxp	_warray	;44a
	adjspb	-12
L118:
	;_finput intareg(nasl) 114b...
	movd	_finput,r0	;59b
	addqd	-1,0(r0)	;87c
	cmpqd	0,0(r0)	;35
	bgt	L9999
	;_finput intareg(nasl) 114b...
	movd	_finput,r0	;59b
	;intareg 114i...
	movd	4(r0),r1	;59b
	addqd	1,4(r0)	;83
	;0(r1) intareg(nasl) 114b...
	movxbd	0(r1),r0	;59b
	andd	255,r0	;105r
	br	L9998
L9999:
	;forarg 115d...
	movd	_finput,tos	;59b
	cxp	__filbuf	;44a
	adjspb	-4
L9998:
	movd	r0,r7	;59a
	cmpqd	-1,r0	;35
	beq	L119
	addr	@36,r0
	cmpd	r0,r7	;37a2
	bne	L120
	;_finput intareg(nasl) 114b...
	movd	_finput,r0	;59b
	addqd	-1,0(r0)	;87c
	cmpqd	0,0(r0)	;35
	bgt	L9997
	;_finput intareg(nasl) 114b...
	movd	_finput,r0	;59b
	;intareg 114i...
	movd	4(r0),r1	;59b
	addqd	1,4(r0)	;83
	;0(r1) intareg(nasl) 114b...
	movxbd	0(r1),r0	;59b
	andd	255,r0	;105r
	br	L9996
L9997:
	;forarg 115d...
	movd	_finput,tos	;59b
	cxp	__filbuf	;44a
	adjspb	-4
L9996:
	movd	r0,r7	;59a
	addr	@65,r1
	cmpd	r1,r0	;37a2
	beq	L121
	;_ftable intareg(nasl) 114b...
	movd	_ftable,r0	;59b
	addqd	-1,0(r0)	;87c
	cmpqd	0,0(r0)	;35
	bgt	L9995
	;_ftable intareg(nasl) 114b...
	movd	_ftable,r0	;59b
	;intareg 114i...
	movd	4(r0),r1	;59b
	addqd	1,4(r0)	;83
	movb	36,0(r1)	;59a
	;36 intareg(nasl) 114b...
	movxbd	36,r0	;59a
	br	L9994
L9995:
	;forarg 115d...
	movd	_ftable,tos	;59b
	;forarg 115d...
	addr	@36,tos	;59a
	cxp	__flsbuf	;44a
	adjspb	-8
L9994:
	br	L123
L121:
	;unary and,forarg 115a...
	addr	L125,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	L124,tos	;setasg
;68a
	cxp	_fopen	;44a
	adjspb	-8
	movd	r0,_faction	;59a
	cmpqd	0,_faction	;35
	bne	L126
	;unary and,forarg 115a...
	addr	L127,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-4
L126:
L128:
	;_faction intareg(nasl) 114b...
	movd	_faction,r0	;59b
	addqd	-1,0(r0)	;87c
	cmpqd	0,0(r0)	;35
	bgt	L9993
	;_faction intareg(nasl) 114b...
	movd	_faction,r0	;59b
	;intareg 114i...
	movd	4(r0),r1	;59b
	addqd	1,4(r0)	;83
	;0(r1) intareg(nasl) 114b...
	movxbd	0(r1),r0	;59b
	andd	255,r0	;105r
	br	L9992
L9993:
	;forarg 115d...
	movd	_faction,tos	;59b
	cxp	__filbuf	;44a
	adjspb	-4
L9992:
	movd	r0,r7	;59a
	cmpqd	-1,r0	;35
	beq	L129
	;_ftable intareg(nasl) 114b...
	movd	_ftable,r0	;59b
	addqd	-1,0(r0)	;87c
	cmpqd	0,0(r0)	;35
	bgt	L9991
	;_ftable intareg(nasl) 114b...
	movd	_ftable,r0	;59b
	;intareg 114i...
	movd	4(r0),r1	;59b
	addqd	1,4(r0)	;83
	movb	r7,0(r1)	;59b
	;0(r1) intareg(nasl) 114b...
	movxbd	0(r1),r0	;59b
	br	L9990
L9991:
	;forarg 115d...
	movd	_ftable,tos	;59b
	;forarg 115d...
	movd	r7,tos	;59b
	cxp	__flsbuf	;44a
	adjspb	-8
L9990:
	br	L128
L129:
	;forarg 115d...
	movd	_faction,tos	;59b
	cxp	_fclose	;44a
	adjspb	-4
	;unary and,forarg 115a...
	addr	L132,tos	;setasg
;68a
	cxp	_unlink	;44a
	adjspb	-4
	;_finput intareg(nasl) 114b...
	movd	_finput,r0	;59b
	addqd	-1,0(r0)	;87c
	cmpqd	0,0(r0)	;35
	bgt	L9989
	;_finput intareg(nasl) 114b...
	movd	_finput,r0	;59b
	;intareg 114i...
	movd	4(r0),r1	;59b
	addqd	1,4(r0)	;83
	;0(r1) intareg(nasl) 114b...
	movxbd	0(r1),r0	;59b
	andd	255,r0	;105r
	br	L9988
L9989:
	;forarg 115d...
	movd	_finput,tos	;59b
	cxp	__filbuf	;44a
	adjspb	-4
L9988:
	movd	r0,r7	;59a
L123:
L120:
	;_ftable intareg(nasl) 114b...
	movd	_ftable,r0	;59b
	addqd	-1,0(r0)	;87c
	cmpqd	0,0(r0)	;35
	bgt	L9987
	;_ftable intareg(nasl) 114b...
	movd	_ftable,r0	;59b
	;intareg 114i...
	movd	4(r0),r1	;59b
	addqd	1,4(r0)	;83
	movb	r7,0(r1)	;59b
	;0(r1) intareg(nasl) 114b...
	movxbd	0(r1),r0	;59b
	br	L9986
L9987:
	;forarg 115d...
	movd	_ftable,tos	;59b
	;forarg 115d...
	movd	r7,tos	;59b
	cxp	__flsbuf	;44a
	adjspb	-8
L9986:
	br	L118
L119:
	;forarg 115d...
	movd	_ftable,tos	;59b
	cxp	_fclose	;44a
	adjspb	-4
L87:
	exit	[r5,r6,r7]
	rxp	0
L88:
	enter	[r5,r6,r7],0
	br 	L89
.endseg
.static
.endseg
.program
.exportp	_chcopy
_chcopy::
	br 	L136	;bfcode
L137:
;_p:	.EQU	12(fp)
;_q:	.EQU	16(fp)
L138:
	;intareg 114i...
	movd	16(fp),r0	;59b
	addqd	1,16(fp)	;83
	movb	0(r0),0(12(fp))	;59b
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	cmpqb	0,0(r0)	;35
	beq	L139
	addqd	1,12(fp)	;83
	br	L138
L139:
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	br	L135
L135:
	exit	
	rxp	0
L136:
	enter	0
	br 	L137
.endseg
.static
.endseg
.program
.exportp	_writem
_writem::
	br 	L142	;bfcode
L143:
;_pp:	.EQU	12(fp)
;_i:	.EQU	-4(fp)
;_p:	.EQU	-8(fp)
; L144 ==  sarr
.endseg
.static
L144:
	.blkb	400
;_q:	.EQU	-12(fp)
.endseg
.program
	movd	12(fp),-8(fp)	;59b
L147:
	cmpqd	0,0(-8(fp))	;35
	bge	L146
L145:
	addqd	4,-8(fp)	;83
	br	L147
L146:
	;intareg 114h...
	negd	0(-8(fp)),r0	;emit_uop
	movd	_prdptr[r0:d],-8(fp)	;59b
	;intareg 114h...
	movd	0(-8(fp)),r0	;59b
	addd	-4096,r0	;105r
	;& _nontrst intareg 114a...
	addr	_nontrst,r1	;setasg
	addr	@0[r0:q],r0	;fast r0 <<= 3 ;49b
	addd	r1,r0	;105t
	;forarg 115d...
	movd	0(r0),tos	;59b
	;unary and,forarg 115a...
	addr	L144,tos	;setasg
;68a
	cxp	_chcopy	;44a
	adjspb	-8
	movd	r0,-12(fp)	;59a
	;unary and,forarg 115a...
	addr	L148,tos	;setasg
;68a
	;forarg 115d...
	movd	-12(fp),tos	;59b
	cxp	_chcopy	;44a
	adjspb	-8
	movd	r0,-12(fp)	;59a
L151:
	addqd	4,-8(fp)	;83
	cmpd	-8(fp),12(fp)	;38
	bne	L9985
	;95 intareg(nasl) 114b...
	movxbd	95,r0	;59a
	br	L9984
L9985:
	;32 intareg(nasl) 114b...
	movxbd	32,r0	;59a
L9984:
	movb	r0,0(-12(fp))	;59a
	addqd	1,-12(fp)	;83
	movqb	0,0(-12(fp))	;59a
	movd	0(-8(fp)),-4(fp)	;59b
	cmpqd	0,-4(fp)	;35
	blt	L152
	br	L150
L152:
	;forarg 115d...
	movd	-4(fp),tos	;59b
	cxp	_symnam	;44a
	adjspb	-4
	;forarg 115d...
	movd	r0,tos	;59a
	;forarg 115d...
	movd	-12(fp),tos	;59b
	cxp	_chcopy	;44a
	adjspb	-8
	movd	r0,-12(fp)	;59a
	;& L144+370 intareg 114a...
	addr	L144+370,r0	;setasg
	cmpd	r0,-12(fp)	;38
	bge	L153
	;unary and,forarg 115a...
	addr	L154,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-4
L153:
L149:
	br	L151
L150:
	movd	0(12(fp)),-4(fp)	;59b
	cmpqd	0,-4(fp)	;35
	ble	L155
	;unary and,forarg 115a...
	addr	L156,tos	;setasg
;68a
	;forarg 115d...
	movd	-12(fp),tos	;59b
	cxp	_chcopy	;44a
	adjspb	-8
	movd	r0,-12(fp)	;59a
	;intareg 114h...
	negd	-4(fp),r0	;emit_uop
	;forarg 115d...
	movd	r0,tos	;59a
	;unary and,forarg 115a...
	addr	L158,tos	;setasg
;68a
	;forarg 115d...
	movd	-12(fp),tos	;59b
	cxp	_sprintf	;44a
	adjspb	-12
L155:
	;& L144 intareg 114a...
	addr	L144,r0	;setasg
	br	L141
L141:
	exit	
	rxp	0
L142:
	enter	12
	br 	L143
.endseg
.static
.endseg
.program
.exportp	_symnam
_symnam::
	br 	L161	;bfcode
L162:
;_i:	.EQU	12(fp)
;_cp:	.EQU	-4(fp)
	addr	@4096,r0
	cmpd	r0,12(fp)	;37a2
	bgt	L9983
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	addd	-4096,r0	;105r
	;& _nontrst intareg 114a...
	addr	_nontrst,r1	;setasg
	addr	@0[r0:q],r0	;fast r0 <<= 3 ;49b
	addd	r1,r0	;105t
	;0(r0) intareg(nasl) 114b...
	movd	0(r0),r0	;59b
	br	L9982
L9983:
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	;& _tokset intareg 114a...
	addr	_tokset,r1	;setasg
	addr	@0[r0:q],r0	;fast r0 <<= 3 ;49b
	addd	r1,r0	;105t
	;0(r0) intareg(nasl) 114b...
	movd	0(r0),r0	;59b
L9982:
	movd	r0,-4(fp)	;59a
	cmpb	32,0(-4(fp))	;36
	bne	L163
	addqd	1,-4(fp)	;83
L163:
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r0	;59b
	br	L160
L160:
	exit	
	rxp	0
L161:
	enter	4
	br 	L162
.endseg
.static
; zzcwp == external
; 	.align	2
	.export	_zzcwp
_zzcwp:
S1:	.blkb 4
 	;16a
; 	.align	2
	.export	_zzgoent
_zzgoent:
	.double	0	; 14
; 	.align	2
	.export	_zzgobest
_zzgobest:
	.double	0	; 14
; 	.align	2
	.export	_zzacent
_zzacent:
	.double	0	; 14
; 	.align	2
	.export	_zzexcp
_zzexcp:
	.double	0	; 14
; 	.align	2
	.export	_zzclose
_zzclose:
	.double	0	; 14
; 	.align	2
	.export	_zzsrconf
_zzsrconf:
	.double	0	; 14
; zzmemsz == external
; 	.align	2
	.export	_zzmemsz
_zzmemsz:
S2:	.blkb 4
 	;16a
; 	.align	2
	.export	_zzrrconf
_zzrrconf:
	.double	0	; 14
.endseg
.program
.exportp	_summary
_summary::
	br 	L168	;bfcode
L169:
	cmpqd	0,_foutput	;35
	beq	L170
	;forarg 115d...
	addr	@300,tos	;59a
	;forarg 115d...
	movd	_nnonter,tos	;59b
	;forarg 115d...
	addr	@127,tos	;59a
	;forarg 115d...
	movd	_ntokens,tos	;59b
	;unary and,forarg 115a...
	addr	L172,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-24
	;forarg 115d...
	addr	@750,tos	;59a
	;forarg 115d...
	movd	_nstate,tos	;59b
	;forarg 115d...
	addr	@600,tos	;59a
	;forarg 115d...
	movd	_nprod,tos	;59b
	;unary and,forarg 115a...
	addr	L173,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-24
	;forarg 115d...
	movd	_zzrrconf,tos	;59b
	;forarg 115d...
	movd	_zzsrconf,tos	;59b
	;unary and,forarg 115a...
	addr	L174,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-16
	;forarg 115d...
	addr	@350,tos	;59a
	;forarg 115d...
	;& _wsets intareg 114a...
	addr	_wsets,r0	;setasg
	;_zzcwp intareg(nasl) 114b...
	movd	_zzcwp,r1	;59b
	subd	r0,r1	;105t
	quod	24,r1	;105r
	movd	r1,tos	;59a
	;unary and,forarg 115a...
	addr	L175,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-16
	;forarg 115d...
	addr	@12000,tos	;59a
	;forarg 115d...
	;& _amem intareg 114a...
	addr	_amem,r0	;setasg
	;_memp intareg(nasl) 114b...
	movd	_memp,r1	;59b
	subd	r0,r1	;105t
	quod	4,r1	;105r
	movd	r1,tos	;59a
	;forarg 115d...
	addr	@12000,tos	;59a
	;forarg 115d...
	;& _mem0 intareg 114a...
	addr	_mem0,r0	;setasg
	;_zzmemsz intareg(nasl) 114b...
	movd	_zzmemsz,r1	;59b
	subd	r0,r1	;105t
	quod	4,r1	;105r
	movd	r1,tos	;59a
	;unary and,forarg 115a...
	addr	L176,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-24
	;forarg 115d...
	addr	@600,tos	;59a
	;forarg 115d...
	movd	_nlset,tos	;59b
	;unary and,forarg 115a...
	addr	L177,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-16
	;forarg 115d...
	;_nstate intareg(nasl) 114b...
	movd	_nstate,r0	;59b
	addr	@0[r0:w],r0	;fast r0 <<= 1 ;49b
	;_zzclose intareg(nasl) 114b...
	movd	_zzclose,r1	;59b
	subd	r0,r1	;105t
	movd	r1,tos	;59a
	;unary and,forarg 115a...
	addr	L178,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-12
	;forarg 115d...
	movd	_zzexcp,tos	;59b
	;forarg 115d...
	movd	_zzacent,tos	;59b
	;unary and,forarg 115a...
	addr	L179,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-16
	;forarg 115d...
	movd	_zzgoent,tos	;59b
	;unary and,forarg 115a...
	addr	L180,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-12
	;forarg 115d...
	movd	_zzgobest,tos	;59b
	;unary and,forarg 115a...
	addr	L181,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-12
L170:
	cmpqd	0,_zzsrconf	;35
	bne	L9981
	cmpqd	0,_zzrrconf	;35
	beq	L182
L9981:
	;unary and,forarg 115a...
	addr	L183,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	__iob+16,tos	;setasg
;68a
	cxp	_fprintf	;44a
	adjspb	-8
	cmpqd	0,_zzsrconf	;35
	beq	L184
	;forarg 115d...
	movd	_zzsrconf,tos	;59b
	;unary and,forarg 115a...
	addr	L185,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	__iob+16,tos	;setasg
;68a
	cxp	_fprintf	;44a
	adjspb	-12
L184:
	cmpqd	0,_zzsrconf	;35
	beq	L186
	cmpqd	0,_zzrrconf	;35
	beq	L186
	;unary and,forarg 115a...
	addr	L187,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	__iob+16,tos	;setasg
;68a
	cxp	_fprintf	;44a
	adjspb	-8
L186:
	cmpqd	0,_zzrrconf	;35
	beq	L188
	;forarg 115d...
	movd	_zzrrconf,tos	;59b
	;unary and,forarg 115a...
	addr	L189,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	__iob+16,tos	;setasg
;68a
	cxp	_fprintf	;44a
	adjspb	-12
L188:
	;unary and,forarg 115a...
	addr	L190,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	__iob+16,tos	;setasg
;68a
	cxp	_fprintf	;44a
	adjspb	-8
L182:
	;forarg 115d...
	movd	_ftemp,tos	;59b
	cxp	_fclose	;44a
	adjspb	-4
	cmpqd	0,_fdefine	;35
	beq	L191
	;forarg 115d...
	movd	_fdefine,tos	;59b
	cxp	_fclose	;44a
	adjspb	-4
L191:
L167:
	exit	
	rxp	0
L168:
	enter	0
	br 	L169
.endseg
.static
.endseg
.program
.exportp	_error
_error::
	br 	L194	;bfcode
L195:
;_s:	.EQU	12(fp)
;_a1:	.EQU	16(fp)
	addqd	1,_nerrors	;83
	;unary and,forarg 115a...
	addr	L196,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	__iob+32,tos	;setasg
;68a
	cxp	_fprintf	;44a
	adjspb	-8
	;forarg 115d...
	movd	16(fp),tos	;59b
	;forarg 115d...
	movd	12(fp),tos	;59b
	;unary and,forarg 115a...
	addr	__iob+32,tos	;setasg
;68a
	cxp	_fprintf	;44a
	adjspb	-12
	;forarg 115d...
	movd	_lineno,tos	;59b
	;unary and,forarg 115a...
	addr	L197,tos	;setasg
;68a
	;unary and,forarg 115a...
	addr	__iob+32,tos	;setasg
;68a
	cxp	_fprintf	;44a
	adjspb	-12
	cmpqd	0,_fatfl	;35
	bne	L198
	br	L193
L198:
	cxp	_summary	;44a
	;forarg 115d...
	movqd	1,tos	;59a
	cxp	_exit	;44a
	adjspb	-4
L193:
	exit	
	rxp	0
L194:
	enter	0
	br 	L195
.endseg
.static
.endseg
.program
.exportp	_aryfil
_aryfil::
	br 	L201	;bfcode
L202:
;_v:	.EQU	12(fp)
;_n:	.EQU	16(fp)
;_c:	.EQU	20(fp)
;_i:	.EQU	-4(fp)
	movqd	0,-4(fp)	;59a
L205:
	cmpd	-4(fp),16(fp)	;38
	bge	L204
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r0	;59b
	movd	20(fp),0(12(fp))[r0:d]	;59b
L203:
	addqd	1,-4(fp)	;83
	br	L205
L204:
L200:
	exit	
	rxp	0
L201:
	enter	4
	br 	L202
.endseg
.static
;_a:	.EQU r7
;_b:	.EQU r6
.endseg
.program
.exportp	_setunion
_setunion::
	br 	L209	;bfcode
L210:
	movd	12(fp),r7
	movd	16(fp),r6
;_i:	.EQU r5
;_x:	.EQU r4
;_sub:	.EQU r3
	movqd	0,r3	;59a
	movqd	0,r5	;59a
L213:
	cmpd	r5,_tbitset	;38
	bge	L212
	movd	0(r7),r4	;59b
	;0(r6) intareg(nasl) 114b...
	movd	0(r6),r0	;59b
	ord	r4,r0	;105t
	movd	r0,0(r7)	;59a
	addqd	4,r6	;82
	;intareg 114i...
	movd	r7,r0	;59b
	addqd	4,r7	;82
	cmpd	r4,0(r0)	;38
	beq	L214
	movqd	1,r3	;59a
L214:
L211:
	addqd	1,r5	;82
	br	L213
L212:
	;r3 intareg(nasl) 114b...
	movd	r3,r0	;59b
	br	L208
L208:
	exit	[r3,r4,r5,r6,r7]
	rxp	0
L209:
	enter	[r3,r4,r5,r6,r7],0
	br 	L210
.endseg
.static
.endseg
.program
.exportp	_prlook
_prlook::
	br 	L218	;bfcode
L219:
;_p:	.EQU	12(fp)
;_j:	.EQU r7
;_pp:	.EQU r6
	movd	12(fp),r6	;59b
	cmpqd	0,r6	;35
	bne	L220
	;unary and,forarg 115a...
	addr	L221,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-8
	br	L222
L220:
	;unary and,forarg 115a...
	addr	L223,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-8
	movqd	1,r7	;59a
L226:
	cmpd	r7,_ntokens	;38
	bgt	L225
	;r7 intareg(nasl) 114b...
	movd	r7,r0	;59b
	ashd	-5,r0	;50b
	;r7 intareg(nasl) 114b...
	movd	r7,r1	;59b
	andd	31,r1	;105r
	;1 intareg(nasl) 114b...
	movqd	1,r2	;59a
	ashd	r1,r2	;50b
	;0(r6)[r0:d] intareg(nasl) 114b...
	movd	0(r6)[r0:d],r0	;59b
	andd	r2,r0	;105t
	cmpqd	0,r0	;35
	beq	L227
	;forarg 115d...
	movd	r7,tos	;59b
	cxp	_symnam	;44a
	adjspb	-4
	;forarg 115d...
	movd	r0,tos	;59a
	;unary and,forarg 115a...
	addr	L228,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-12
L227:
L224:
	addqd	1,r7	;82
	br	L226
L225:
	;unary and,forarg 115a...
	addr	L229,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-8
L222:
L217:
	exit	[r6,r7]
	rxp	0
L218:
	enter	[r6,r7],0
	br 	L219
.endseg
.static
.endseg
.program
.exportp	_cpres
_cpres::
	br 	L232	;bfcode
L233:
;_pmem:	.EQU r7
;_c:	.EQU r6
;_j:	.EQU r5
;_i:	.EQU r4
; L234 ==  pyield
.endseg
.static
; 	.align	2
L234:
	.blkb	2400
.endseg
.program
	addr	L234,r7	;setasg
;68a
	movqd	0,r4	;59a
L237:
	cmpd	r4,_nnonter	;38
	bgt	L236
	movd	r4,r6	;59b
	addr	4096(r6),r6	;86e  r6 += 4096
;68a
	movd	r7,_pres[r4:d]	;59b
	movqd	0,_fatfl	;59a
	movqd	0,r5	;59a
L240:
	cmpd	r5,_nprod	;38
	bge	L239
	;_prdptr[r5:d] intareg(nasl) 114b...
	movd	_prdptr[r5:d],r0	;59b
	cmpd	r6,0(r0)	;38
	bne	L241
	;4 intareg(nasl) 114b...
	movqd	4,r0	;59a
	addd	_prdptr[r5:d],r0	;105r
	movd	r0,0(r7)	;59a
	addqd	4,r7	;82
L241:
L238:
	addqd	1,r5	;82
	br	L240
L239:
	cmpd	r7,_pres[r4:d]	;38
	bne	L242
	;& _nontrst intareg 114a...
	addr	_nontrst,r0	;setasg
	;r4 intareg(nasl) 114b...
	movd	r4,r1	;59b
	addr	@0[r1:q],r1	;fast r1 <<= 3 ;49b
	addd	r0,r1	;105t
	;forarg 115d...
	movd	0(r1),tos	;59b
	;unary and,forarg 115a...
	addr	L243,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-8
L242:
L235:
	addqd	1,r4	;82
	br	L237
L236:
	movd	r7,_pres[r4:d]	;59b
	movqd	1,_fatfl	;59a
	cmpqd	0,_nerrors	;35
	beq	L244
	cxp	_summary	;44a
	;forarg 115d...
	movqd	1,tos	;59a
	cxp	_exit	;44a
	adjspb	-4
L244:
	;& L234 intareg 114a...
	addr	L234,r0	;setasg
	;_nprod intareg(nasl) 114b...
	movd	_nprod,r1	;59b
	addr	@0[r1:d],r1	;fast r1 <<= 2 ;49b
	addd	r1,r0	;105t
	cmpd	r7,r0	;38
	beq	L245
	;forarg 115d...
	;& L234 intareg 114a...
	addr	L234,r0	;setasg
	;_nprod intareg(nasl) 114b...
	movd	_nprod,r1	;59b
	addr	@0[r1:d],r1	;fast r1 <<= 2 ;49b
	addd	r1,r0	;105t
	;r7 intareg(nasl) 114b...
	movd	r7,r1	;59b
	subd	r0,r1	;105t
	quod	4,r1	;105r
	movd	r1,tos	;59a
	;unary and,forarg 115a...
	addr	L246,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-8
L245:
L231:
	exit	[r4,r5,r6,r7]
	rxp	0
L232:
	enter	[r4,r5,r6,r7],0
	br 	L233
.endseg
.static
; indebug == external
; 	.align	2
	.export	_indebug
_indebug:
	.double	0	; 14
.endseg
.program
.exportp	_cpfir
_cpfir::
	br 	L250	;bfcode
L251:
;_p:	.EQU r7
;_s:	.EQU r6
;_i:	.EQU r5
;_t:	.EQU r4
;_ch:	.EQU r3
;_changes:	.EQU	-4(fp)
	;& _wsets intareg 114a...
	addr	_wsets,r0	;setasg
	;_nnonter intareg(nasl) 114b...
	movd	_nnonter,r1	;59b
	muld	24,r1	;105r
	addd	r1,r0	;105t
	movd	r0,_zzcwp	;59a
	movqd	0,r5	;59a
L254:
	cmpd	r5,_nnonter	;38
	bgt	L253
	;forarg 115d...
	movqd	0,tos	;59a
	;forarg 115d...
	movd	_tbitset,tos	;59b
	;forarg 115d...
	;& _wsets intareg 114a...
	addr	_wsets,r0	;setasg
	;r5 intareg(nasl) 114b...
	movd	r5,r1	;59b
	muld	24,r1	;105r
	addd	r1,r0	;105t
	addr	8(r0),r0	;86e  r0 += 8
	movd	r0,tos	;59a
	cxp	_aryfil	;44a
	adjspb	-12
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	r5,r0	;105t
	movd	_pres[r0:d],r4	;59b
	movd	_pres[r5:d],r6	;59b
L257:
	cmpd	r6,r4	;38
	bge	L256
	movd	0(r6),r7	;59b
L260:
	movd	0(r7),r3	;59b
	cmpqd	0,r3	;35
	bge	L259
	addr	@4096,r0
	cmpd	r0,r3	;37a2
	ble	L261
	;r3 intareg(nasl) 114b...
	movd	r3,r0	;59b
	andd	31,r0	;105r
	;1 intareg(nasl) 114b...
	movqd	1,r1	;59a
	ashd	r0,r1	;50b
	;& _wsets intareg 114a...
	addr	_wsets,r0	;setasg
	;r5 intareg(nasl) 114b...
	movd	r5,r2	;59b
	muld	24,r2	;105r
	addd	r2,r0	;105t
	addr	8(r0),r0	;86e  r0 += 8
	;r3 intareg(nasl) 114b...
	movd	r3,r2	;59b
	ashd	-5,r2	;50b
	addr	@0[r2:d],r2	;fast r2 <<= 2 ;49b
	addd	r2,r0	;105t
	ord	r1,0(r0)	;105s
	br	L259
L261:
	;r3 intareg(nasl) 114b...
	movd	r3,r0	;59b
	addd	-4096,r0	;105r
	cmpqd	0,_pempty[r0:d]	;35
	bne	L262
	br	L259
L262:
L258:
	addqd	4,r7	;82
	br	L260
L259:
L255:
	addqd	4,r6	;82
	br	L257
L256:
L252:
	addqd	1,r5	;82
	br	L254
L253:
	movqd	1,-4(fp)	;59a
L263:
	cmpqd	0,-4(fp)	;35
	beq	L264
	movqd	0,-4(fp)	;59a
	movqd	0,r5	;59a
L267:
	cmpd	r5,_nnonter	;38
	bgt	L266
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	r5,r0	;105t
	movd	_pres[r0:d],r4	;59b
	movd	_pres[r5:d],r6	;59b
L270:
	cmpd	r6,r4	;38
	bge	L269
	movd	0(r6),r7	;59b
L273:
	movd	0(r7),r3	;59b
	addd	-4096,r3	;105r
	cmpqd	0,r3	;35
	bgt	L272
	;forarg 115d...
	;& _wsets intareg 114a...
	addr	_wsets,r0	;setasg
	;r3 intareg(nasl) 114b...
	movd	r3,r1	;59b
	muld	24,r1	;105r
	addd	r1,r0	;105t
	addr	8(r0),r0	;86e  r0 += 8
	movd	r0,tos	;59a
	;forarg 115d...
	;& _wsets intareg 114a...
	addr	_wsets,r0	;setasg
	;r5 intareg(nasl) 114b...
	movd	r5,r1	;59b
	muld	24,r1	;105r
	addd	r1,r0	;105t
	addr	8(r0),r0	;86e  r0 += 8
	movd	r0,tos	;59a
	cxp	_setunion	;44a
	adjspb	-8
	ord	r0,-4(fp)	;105s
	cmpqd	0,_pempty[r3:d]	;35
	bne	L274
	br	L272
L274:
L271:
	addqd	4,r7	;82
	br	L273
L272:
L268:
	addqd	4,r6	;82
	br	L270
L269:
L265:
	addqd	1,r5	;82
	br	L267
L266:
	br	L263
L264:
	movqd	0,r5	;59a
L277:
	cmpd	r5,_nnonter	;38
	bgt	L276
	;forarg 115d...
	;& _wsets intareg 114a...
	addr	_wsets,r0	;setasg
	;r5 intareg(nasl) 114b...
	movd	r5,r1	;59b
	muld	24,r1	;105r
	addd	r1,r0	;105t
	addr	8(r0),r0	;86e  r0 += 8
	movd	r0,tos	;59a
	cxp	_flset	;44a
	adjspb	-4
	movd	r0,_pfirst[r5:d]	;59a
L275:
	addqd	1,r5	;82
	br	L277
L276:
	cmpqd	0,_indebug	;35
	bne	L278
	br	L249
L278:
	cmpqd	0,_foutput	;35
	beq	L279
	movqd	0,r5	;59a
L282:
	cmpd	r5,_nnonter	;38
	bgt	L281
	;& _nontrst intareg 114a...
	addr	_nontrst,r0	;setasg
	;r5 intareg(nasl) 114b...
	movd	r5,r1	;59b
	addr	@0[r1:q],r1	;fast r1 <<= 3 ;49b
	addd	r0,r1	;105t
	;forarg 115d...
	movd	0(r1),tos	;59b
	;unary and,forarg 115a...
	addr	L283,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-12
	;forarg 115d...
	movd	_pfirst[r5:d],tos	;59b
	cxp	_prlook	;44a
	adjspb	-4
	;forarg 115d...
	movd	_pempty[r5:d],tos	;59b
	;unary and,forarg 115a...
	addr	L284,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-12
L280:
	addqd	1,r5	;82
	br	L282
L281:
L279:
L249:
	exit	[r3,r4,r5,r6,r7]
	rxp	0
L250:
	enter	[r3,r4,r5,r6,r7],4
	br 	L251
.endseg
.static
.endseg
.program
.exportp	_state
_state::
	br 	L288	;bfcode
L289:
;_c:	.EQU	12(fp)
;_size1:	.EQU	-4(fp)
;_size2:	.EQU	-8(fp)
;_i:	.EQU r7
;_p1:	.EQU	-12(fp)
;_p2:	.EQU	-16(fp)
;_k:	.EQU	-20(fp)
;_l:	.EQU	-24(fp)
;_q1:	.EQU	-28(fp)
;_q2:	.EQU	-32(fp)
	;_nstate intareg(nasl) 114b...
	movd	_nstate,r0	;59b
	movd	_pstate[r0:d],-12(fp)	;59b
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	_nstate,r0	;105r
	movd	_pstate[r0:d],-16(fp)	;59b
	cmpd	-12(fp),-16(fp)	;38
	bne	L290
	;0 intareg(nasl) 114b...
	movqd	0,r0	;59a
	br	L287
L290:
	;-8 intareg(nasl) 114b...
	movqd	-8,r0	;59a
	addd	-16(fp),r0	;105r
	movd	r0,-20(fp)	;59a
L293:
	cmpd	-20(fp),-12(fp)	;38
	ble	L292
	;-8 intareg(nasl) 114b...
	movqd	-8,r0	;59a
	addd	-20(fp),r0	;105r
	movd	r0,-24(fp)	;59a
L296:
	cmpd	-24(fp),-12(fp)	;38
	blt	L295
	cmpd	0(-24(fp)),0(-20(fp))	;38
	ble	L297
;_s:	.EQU	-36(fp)
;_ss:	.EQU	-40(fp)
	movd	0(-20(fp)),-36(fp)	;59b
	movd	0(-24(fp)),0(-20(fp))	;59b
	movd	-36(fp),0(-24(fp))	;59b
	movd	4(-20(fp)),-40(fp)	;59b
	movd	4(-24(fp)),4(-20(fp))	;59b
	movd	-40(fp),4(-24(fp))	;59b
L297:
L294:
	subd	8,-24(fp)	;105q
	br	L296
L295:
L291:
;68a
	subd	8,-20(fp)	;105q
	br	L293
L292:
	;-16(fp) intareg(nasl) 114b...
	movd	-16(fp),r0	;59b
	subd	-12(fp),r0	;105r
	quod	8,r0	;105r
	movd	r0,-4(fp)	;59a
	addr	@4096,r0
	cmpd	r0,12(fp)	;37a2
	bgt	L9980
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	addd	-4096,r0	;105r
	;_ntstates[r0:d] intareg(nasl) 114b...
	movd	_ntstates[r0:d],r0	;59b
	br	L9979
L9980:
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	;_tstates[r0:d] intareg(nasl) 114b...
	movd	_tstates[r0:d],r0	;59b
L9979:
	movd	r0,r7	;59a
L300:
	cmpqd	0,r7	;35
	beq	L299
	movd	_pstate[r7:d],-28(fp)	;59b
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	r7,r0	;105t
	movd	_pstate[r0:d],-32(fp)	;59b
	;-32(fp) intareg(nasl) 114b...
	movd	-32(fp),r0	;59b
	subd	-28(fp),r0	;105r
	quod	8,r0	;105r
	movd	r0,-8(fp)	;59a
	cmpd	-4(fp),-8(fp)	;38
	beq	L301
	br	L298
L301:
	movd	-12(fp),-20(fp)	;59b
	movd	-28(fp),-24(fp)	;59b
L304:
	cmpd	-24(fp),-32(fp)	;38
	bge	L303
	cmpd	0(-24(fp)),0(-20(fp))	;38
	beq	L305
	br	L303
L305:
	addd	8,-20(fp)	;105q
L302:
;68a
	addd	8,-24(fp)	;105q
	br	L304
L303:
	cmpd	-24(fp),-32(fp)	;38
	beq	L306
	br	L298
L306:
	;_nstate intareg(nasl) 114b...
	movd	_nstate,r0	;59b
	;1 intareg(nasl) 114b...
	movqd	1,r1	;59a
	addd	_nstate,r1	;105r
	movd	_pstate[r0:d],_pstate[r1:d]	;59b
	cmpqd	0,_nolook	;35
	beq	L307
	;r7 intareg(nasl) 114b...
	movd	r7,r0	;59b
	br	L287
L307:
	movd	-28(fp),-24(fp)	;59b
	movd	-12(fp),-20(fp)	;59b
L310:
	cmpd	-24(fp),-32(fp)	;38
	bge	L309
;_s:	.EQU	-36(fp)
	movqd	0,-36(fp)	;59a
L313:
	cmpd	-36(fp),_tbitset	;38
	bge	L312
	;-36(fp) intareg(nasl) 114b...
	movd	-36(fp),r0	;59b
	;-36(fp) intareg(nasl) 114b...
	movd	-36(fp),r1	;59b
	movd	-24(fp)+4[r0:d],_clset[r1:d]	;59b
L311:
	addqd	1,-36(fp)	;83
	br	L313
L312:
	;forarg 115d...
	movd	4(-20(fp)),tos	;59b
	;unary and,forarg 115a...
	addr	_clset,tos	;setasg
;68a
	cxp	_setunion	;44a
	adjspb	-8
	cmpqd	0,r0	;35
	beq	L314
	movqd	1,_tystate[r7:d]	;59a
	;unary and,forarg 115a...
	addr	_clset,tos	;setasg
;68a
	cxp	_flset	;44a
	adjspb	-4
	movd	r0,4(-24(fp))	;59a
L314:
L308:
	addd	8,-24(fp)	;105q
	addd	8,-20(fp)	;105q
	br	L310
L309:
	;r7 intareg(nasl) 114b...
	movd	r7,r0	;59b
	br	L287
L298:
	movd	_mstates[r7:d],r7	;59b
	br	L300
L299:
	cmpqd	0,_nolook	;35
	beq	L315
	;unary and,forarg 115a...
	addr	L316,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-4
L315:
	;2 intareg(nasl) 114b...
	movqd	2,r0	;59a
	addd	_nstate,r0	;105r
	movd	-16(fp),_pstate[r0:d]	;59b
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	_nstate,r0	;105r
	addr	@750,r1
	cmpd	r1,r0	;37a2
	bgt	L317
	;unary and,forarg 115a...
	addr	L318,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-4
L317:
	addr	@4096,r0
	cmpd	r0,12(fp)	;37a2
	bgt	L319
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	addd	-4096,r0	;105r
	;_nstate intareg(nasl) 114b...
	movd	_nstate,r1	;59b
	movd	_ntstates[r0:d],_mstates[r1:d]	;59b
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	addd	-4096,r0	;105r
	movd	_nstate,_ntstates[r0:d]	;59b
	br	L320
L319:
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	;_nstate intareg(nasl) 114b...
	movd	_nstate,r1	;59b
	movd	_tstates[r0:d],_mstates[r1:d]	;59b
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	movd	_nstate,_tstates[r0:d]	;59b
L320:
	;_nstate intareg(nasl) 114b...
	movd	_nstate,r0	;59b
	movqd	1,_tystate[r0:d]	;59a
	;_nstate intareg(nasl) 114b...
	movd	_nstate,r0	;59b
	addqd	1,_nstate	;83
	br	L287
L287:
	exit	[r7]
	rxp	0
L288:
	enter	[r7],40
	br 	L289
.endseg
.static
; pidebug == external
; 	.align	2
	.export	_pidebug
_pidebug:
	.double	0	; 14
.endseg
.program
.exportp	_putitem
_putitem::
	br 	L325	;bfcode
L326:
;_ptr:	.EQU	12(fp)
;_lptr:	.EQU	16(fp)
;_j:	.EQU r7
	cmpqd	0,_pidebug	;35
	beq	L327
	cmpqd	0,_foutput	;35
	beq	L327
	;forarg 115d...
	movd	_nstate,tos	;59b
	;forarg 115d...
	movd	12(fp),tos	;59b
	cxp	_writem	;44a
	adjspb	-4
	;forarg 115d...
	movd	r0,tos	;59a
	;unary and,forarg 115a...
	addr	L328,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-16
L327:
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	_nstate,r0	;105r
	movd	_pstate[r0:d],r7	;59b
	movd	12(fp),0(r7)	;59b
	cmpqd	0,_nolook	;35
	bne	L329
	;forarg 115d...
	movd	16(fp),tos	;59b
	cxp	_flset	;44a
	adjspb	-4
	movd	r0,4(r7)	;59a
L329:
	addr	8(r7),r7	;86e  r7 += 8
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	_nstate,r0	;105r
	movd	r7,_pstate[r0:d]	;59b
	cmpd	r7,_zzmemsz	;38
	ble	L330
	movd	r7,_zzmemsz	;59b
	;& _mem0+48000 intareg 114a...
	addr	_mem0+48000,r0	;setasg
	cmpd	r0,_zzmemsz	;38
	bgt	L331
	;unary and,forarg 115a...
	addr	L332,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-4
L331:
L330:
L324:
	exit	[r7]
	rxp	0
L325:
	enter	[r7],0
	br 	L326
.endseg
.static
.endseg
.program
.exportp	_cempty
_cempty::
	br 	L335	;bfcode
L336:
;_i:	.EQU r7
;_p:	.EQU r6
	;forarg 115d...
	movqd	0,tos	;59a
	;forarg 115d...
	;_nnonter intareg(nasl) 114b...
	movd	_nnonter,r0	;59b
	addqd	1,r0	;82
	movd	r0,tos	;59a
	;unary and,forarg 115a...
	addr	_pempty,tos	;setasg
;68a
	cxp	_aryfil	;44a
	adjspb	-12
L337:
	movqd	0,r7	;59a
L340:
	cmpd	r7,_nprod	;38
	bge	L339
	;_prdptr[r7:d] intareg(nasl) 114b...
	movd	_prdptr[r7:d],r0	;59b
	;0(r0) intareg(nasl) 114b...
	movd	0(r0),r0	;59b
	addd	-4096,r0	;105r
	cmpqd	0,_pempty[r0:d]	;35
	beq	L341
	br	L338
L341:
	movd	_prdptr[r7:d],r6	;59b
	addqd	4,r6	;82
;68a
L344:
	cmpqd	0,0(r6)	;35
	bgt	L343
	addr	@4096,r0
	cmpd	r0,0(r6)	;37a2
	bgt	L345
	;0(r6) intareg(nasl) 114b...
	movd	0(r6),r0	;59b
	addd	-4096,r0	;105r
	cmpqd	0,_pempty[r0:d]	;35
	bne	L345
	br	L343
L345:
L342:
	addqd	4,r6	;82
	br	L344
L343:
	cmpqd	0,0(r6)	;35
	ble	L346
	;_prdptr[r7:d] intareg(nasl) 114b...
	movd	_prdptr[r7:d],r0	;59b
	;0(r0) intareg(nasl) 114b...
	movd	0(r0),r0	;59b
	addd	-4096,r0	;105r
	movqd	1,_pempty[r0:d]	;59a
	br	L337
L346:
L338:
	addqd	1,r7	;82
	br	L340
L339:
	movqd	0,r7	;59a
L349:
	cmpd	r7,_nnonter	;38
	bgt	L348
	cmpqd	0,r7	;35
	bne	L350
	br	L347
L350:
	cmpqd	1,_pempty[r7:d]	;35
	beq	L351
	movqd	0,_fatfl	;59a
	;& _nontrst intareg 114a...
	addr	_nontrst,r0	;setasg
	;r7 intareg(nasl) 114b...
	movd	r7,r1	;59b
	addr	@0[r1:q],r1	;fast r1 <<= 3 ;49b
	addd	r0,r1	;105t
	;forarg 115d...
	movd	0(r1),tos	;59b
	;unary and,forarg 115a...
	addr	L352,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-8
L351:
L347:
	addqd	1,r7	;82
	br	L349
L348:
	cmpqd	0,_nerrors	;35
	beq	L353
	cxp	_summary	;44a
	;forarg 115d...
	movqd	1,tos	;59a
	cxp	_exit	;44a
	adjspb	-4
L353:
	;forarg 115d...
	movqd	0,tos	;59a
	;forarg 115d...
	;_nnonter intareg(nasl) 114b...
	movd	_nnonter,r0	;59b
	addqd	1,r0	;82
	movd	r0,tos	;59a
	;unary and,forarg 115a...
	addr	_pempty,tos	;setasg
;68a
	cxp	_aryfil	;44a
	adjspb	-12
L354:
	movqd	1,r7	;59a
L357:
	cmpd	r7,_nprod	;38
	bge	L356
	;_prdptr[r7:d] intareg(nasl) 114b...
	movd	_prdptr[r7:d],r0	;59b
	;0(r0) intareg(nasl) 114b...
	movd	0(r0),r0	;59b
	addd	-4096,r0	;105r
	cmpqd	0,_pempty[r0:d]	;35
	bne	L358
	movd	_prdptr[r7:d],r6	;59b
	addqd	4,r6	;82
;68a
L361:
	addr	@4096,r0
	cmpd	r0,0(r6)	;37a2
	bgt	L360
	;0(r6) intareg(nasl) 114b...
	movd	0(r6),r0	;59b
	addd	-4096,r0	;105r
	cmpqd	1,_pempty[r0:d]	;35
	bne	L360
L359:
	addqd	4,r6	;82
	br	L361
L360:
	cmpqd	0,0(r6)	;35
	ble	L362
	;_prdptr[r7:d] intareg(nasl) 114b...
	movd	_prdptr[r7:d],r0	;59b
	;0(r0) intareg(nasl) 114b...
	movd	0(r0),r0	;59b
	addd	-4096,r0	;105r
	movqd	1,_pempty[r0:d]	;59a
	br	L354
L362:
L358:
L355:
	addqd	1,r7	;82
	br	L357
L356:
L334:
	exit	[r6,r7]
	rxp	0
L335:
	enter	[r6,r7],0
	br 	L336
.endseg
.static
; gsdebug == external
; 	.align	2
	.export	_gsdebug
_gsdebug:
	.double	0	; 14
.endseg
.program
.exportp	_stagen
_stagen::
	br 	L366	;bfcode
L367:
;_i:	.EQU	-4(fp)
;_j:	.EQU	-8(fp)
;_c:	.EQU r7
;_p:	.EQU r6
;_q:	.EQU r5
	movqd	0,_nstate	;59a
	movd	_mem,_pstate+4	;59b
	movd	_pstate+4,_pstate	;59b
	;forarg 115d...
	movqd	0,tos	;59a
	;forarg 115d...
	movd	_tbitset,tos	;59b
	;unary and,forarg 115a...
	addr	_clset,tos	;setasg
;68a
	cxp	_aryfil	;44a
	adjspb	-12
	;unary and,forarg 115a...
	addr	_clset,tos	;setasg
;68a
	;forarg 115d...
	;_prdptr intareg(nasl) 114b...
	movd	_prdptr,r0	;59b
	addqd	4,r0	;82
	movd	r0,tos	;59a
	cxp	_putitem	;44a
	adjspb	-8
	movqd	1,_tystate	;59a
	movqd	1,_nstate	;59a
	movd	_pstate+4,_pstate+8	;59b
	;forarg 115d...
	movqd	0,tos	;59a
	;forarg 115d...
	addr	@12000,tos	;59a
	;unary and,forarg 115a...
	addr	_amem,tos	;setasg
;68a
	cxp	_aryfil	;44a
	adjspb	-12
L368:
	movqd	0,-4(fp)	;59a
L371:
	cmpd	-4(fp),_nstate	;38
	bge	L370
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r0	;59b
	cmpqd	1,_tystate[r0:d]	;35
	beq	L372
	br	L369
L372:
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r0	;59b
	movqd	0,_tystate[r0:d]	;59a
	;forarg 115d...
	movqd	0,tos	;59a
	;forarg 115d...
	;_nnonter intareg(nasl) 114b...
	movd	_nnonter,r0	;59b
	addqd	1,r0	;82
	movd	r0,tos	;59a
	;unary and,forarg 115a...
	addr	_temp1,tos	;setasg
;68a
	cxp	_aryfil	;44a
	adjspb	-12
	;forarg 115d...
	movd	-4(fp),tos	;59b
	cxp	_closure	;44a
	adjspb	-4
	addr	_wsets,r6	;setasg
;68a
L376:
	cmpd	r6,_cwp	;38
	bge	L375
	cmpqd	0,4(r6)	;35
	beq	L377
	br	L374
L377:
	movqd	1,4(r6)	;59a
	;0(r6) intareg(nasl) 114b...
	movd	0(r6),r0	;59b
	movd	0(r0),r7	;59b
	cmpqd	1,r7	;35
	blt	L378
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	-4(fp),r0	;105r
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r1	;59b
	;_pstate[r0:d] intareg(nasl) 114b...
	movd	_pstate[r0:d],r0	;59b
	subd	_pstate[r1:d],r0	;105r
	quod	8,r0	;105r
	;& _wsets intareg 114a...
	addr	_wsets,r1	;setasg
	;r6 intareg(nasl) 114b...
	movd	r6,r2	;59b
	subd	r1,r2	;105t
	quod	24,r2	;105r
	cmpd	r2,r0	;38
	blt	L379
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r0	;59b
	movqd	2,_tystate[r0:d]	;59a
L379:
	br	L374
L378:
	movd	r6,r5	;59b
L382:
	cmpd	r5,_cwp	;38
	bge	L381
	;0(r5) intareg(nasl) 114b...
	movd	0(r5),r0	;59b
	cmpd	r7,0(r0)	;38
	bne	L383
	;forarg 115d...
	;r5 intareg(nasl) 114b...
	movd	r5,r0	;59b
	addr	8(r0),r0	;86e  r0 += 8
	movd	r0,tos	;59a
	;forarg 115d...
	;0(r5) intareg(nasl) 114b...
	movd	0(r5),r0	;59b
	addqd	4,r0	;82
	movd	r0,tos	;59a
	cxp	_putitem	;44a
	adjspb	-8
	movqd	1,4(r5)	;59a
L383:
L380:
	addr	24(r5),r5	;86e  r5 += 24
	br	L382
L381:
	addr	@4096,r0
	cmpd	r0,r7	;37a2
	ble	L384
	;forarg 115d...
	movd	r7,tos	;59b
	cxp	_state	;44a
	adjspb	-4
	br	L385
L384:
	;forarg 115d...
	movd	r7,tos	;59b
	cxp	_state	;44a
	adjspb	-4
	;r7 intareg(nasl) 114b...
	movd	r7,r1	;59b
	addd	-4096,r1	;105r
	movd	r0,_temp1[r1:d]	;59a
L385:
L374:
	addr	24(r6),r6	;86e  r6 += 24
	br	L376
L375:
	cmpqd	0,_gsdebug	;35
	beq	L386
	cmpqd	0,_foutput	;35
	beq	L386
	;forarg 115d...
	movd	-4(fp),tos	;59b
	;unary and,forarg 115a...
	addr	L387,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-12
	movqd	0,-8(fp)	;59a
L390:
	cmpd	-8(fp),_nnonter	;38
	bgt	L389
	;-8(fp) intareg(nasl) 114b...
	movd	-8(fp),r0	;59b
	cmpqd	0,_temp1[r0:d]	;35
	beq	L391
	;-8(fp) intareg(nasl) 114b...
	movd	-8(fp),r0	;59b
	;forarg 115d...
	movd	_temp1[r0:d],tos	;59b
	;-8(fp) intareg(nasl) 114b...
	movd	-8(fp),r0	;59b
	;& _nontrst intareg 114a...
	addr	_nontrst,r1	;setasg
	addr	@0[r0:q],r0	;fast r0 <<= 3 ;49b
	addd	r1,r0	;105t
	;forarg 115d...
	movd	0(r0),tos	;59b
	;unary and,forarg 115a...
	addr	L392,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-16
L391:
L388:
	addqd	1,-8(fp)	;83
	br	L390
L389:
	;unary and,forarg 115a...
	addr	L393,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-8
L386:
	;forarg 115d...
	;_nnonter intareg(nasl) 114b...
	movd	_nnonter,r0	;59b
	addqd	-1,r0	;82
	movd	r0,tos	;59a
	;unary and,forarg 115a...
	addr	_temp1+4,tos	;setasg
;68a
	cxp	_apack	;44a
	adjspb	-8
	addqd	-1,r0	;82
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r1	;59b
	movd	r0,_indgo[r1:d]	;59a
	br	L368
L369:
	addqd	1,-4(fp)	;83
	br	L371
L370:
L365:
	exit	[r5,r6,r7]
	rxp	0
L366:
	enter	[r5,r6,r7],8
	br 	L367
.endseg
.static
; cldebug == external
; 	.align	2
	.export	_cldebug
_cldebug:
	.double	0	; 14
.endseg
.program
.exportp	_closure
_closure::
	br 	L398	;bfcode
L399:
;_i:	.EQU	12(fp)
;_c:	.EQU	-4(fp)
;_ch:	.EQU	-8(fp)
;_work:	.EQU	-12(fp)
;_k:	.EQU	-16(fp)
;_u:	.EQU r7
;_v:	.EQU r6
;_pi:	.EQU	-20(fp)
;_s:	.EQU	-24(fp)
;_t:	.EQU	-28(fp)
;_q:	.EQU	-32(fp)
;_p:	.EQU r5
	addqd	1,_zzclose	;83
	addr	_wsets,_cwp	;setasg
;68a
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	12(fp),r0	;105r
	movd	_pstate[r0:d],-32(fp)	;59b
	;12(fp) intareg(nasl) 114b...
	movd	12(fp),r0	;59b
	movd	_pstate[r0:d],r5	;59b
L402:
	cmpd	r5,-32(fp)	;38
	bge	L401
	;_cwp intareg(nasl) 114b...
	movd	_cwp,r0	;59b
	movd	0(r5),0(r0)	;59b
	;_cwp intareg(nasl) 114b...
	movd	_cwp,r0	;59b
	movqd	1,4(r0)	;59a
	movqd	0,-16(fp)	;59a
L405:
	cmpd	-16(fp),_tbitset	;38
	bge	L404
	;-16(fp) intareg(nasl) 114b...
	movd	-16(fp),r0	;59b
	addr	@0[r0:d],r0	;fast r0 <<= 2 ;49b
	addd	4(r5),r0	;105r
	;-16(fp) intareg(nasl) 114b...
	movd	-16(fp),r1	;59b
	;_cwp intareg(nasl) 114b...
	movd	_cwp,r2	;59b
	addr	8(r2),r2	;86e  r2 += 8
	movd	0(r0),0(r2)[r1:d]	;59b
L403:
	addqd	1,-16(fp)	;83
	br	L405
L404:
	addd	24,_cwp	;105q
L400:
	addr	8(r5),r5	;86e  r5 += 8
	br	L402
L401:
	movqd	1,-12(fp)	;59a
L406:
	cmpqd	0,-12(fp)	;35
	beq	L407
	movqd	0,-12(fp)	;59a
	addr	_wsets,r7	;setasg
;68a
L410:
	cmpd	r7,_cwp	;38
	bge	L409
	cmpqd	0,4(r7)	;35
	bne	L411
	br	L408
L411:
	;0(r7) intareg(nasl) 114b...
	movd	0(r7),r0	;59b
	movd	0(r0),-4(fp)	;59b
	addr	@4096,r0
	cmpd	r0,-4(fp)	;37a2
	ble	L412
	movqd	0,4(r7)	;59a
	br	L408
L412:
	;forarg 115d...
	movqd	0,tos	;59a
	;forarg 115d...
	movd	_tbitset,tos	;59b
	;unary and,forarg 115a...
	addr	_clset,tos	;setasg
;68a
	cxp	_aryfil	;44a
	adjspb	-12
	movd	r7,r6	;59b
L415:
	cmpd	r6,_cwp	;38
	bge	L414
	cmpqd	1,4(r6)	;35
	bne	L416
	movd	0(r6),-20(fp)	;59b
	;-20(fp) intareg(nasl) 114b...
	movd	-20(fp),r0	;59b
	cmpd	0(r0),-4(fp)	;38
	bne	L416
	movqd	0,4(r6)	;59a
	cmpqd	0,_nolook	;35
	beq	L417
	br	L413
L417:
L418:
	addqd	4,-20(fp)	;83
	;-20(fp) intareg(nasl) 114b...
	movd	-20(fp),r0	;59b
	movd	0(r0),-8(fp)	;59b
	cmpqd	0,-8(fp)	;35
	bge	L419
	addr	@4096,r0
	cmpd	r0,-8(fp)	;37a2
	ble	L420
	;-8(fp) intareg(nasl) 114b...
	movd	-8(fp),r0	;59b
	andd	31,r0	;105r
	;1 intareg(nasl) 114b...
	movqd	1,r1	;59a
	ashd	r0,r1	;50b
	;-8(fp) intareg(nasl) 114b...
	movd	-8(fp),r0	;59b
	ashd	-5,r0	;50b
	ord	r1,_clset[r0:d]	;105s
	br	L419
L420:
	;-8(fp) intareg(nasl) 114b...
	movd	-8(fp),r0	;59b
	addd	-4096,r0	;105r
	;forarg 115d...
	movd	_pfirst[r0:d],tos	;59b
	;unary and,forarg 115a...
	addr	_clset,tos	;setasg
;68a
	cxp	_setunion	;44a
	adjspb	-8
	;-8(fp) intareg(nasl) 114b...
	movd	-8(fp),r0	;59b
	addd	-4096,r0	;105r
	cmpqd	0,_pempty[r0:d]	;35
	bne	L421
	br	L419
L421:
	br	L418
L419:
	cmpqd	0,-8(fp)	;35
	blt	L422
	;forarg 115d...
	;r6 intareg(nasl) 114b...
	movd	r6,r0	;59b
	addr	8(r0),r0	;86e  r0 += 8
	movd	r0,tos	;59a
	;unary and,forarg 115a...
	addr	_clset,tos	;setasg
;68a
	cxp	_setunion	;44a
	adjspb	-8
L422:
L416:
L413:
	addr	24(r6),r6	;86e  r6 += 24
	br	L415
L414:
	subd	4096,-4(fp)	;105q
	;1 intareg(nasl) 114b...
	movqd	1,r0	;59a
	addd	-4(fp),r0	;105r
	movd	_pres[r0:d],-28(fp)	;59b
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r0	;59b
	movd	_pres[r0:d],-24(fp)	;59b
L425:
	cmpd	-24(fp),-28(fp)	;38
	bge	L424
	addr	_wsets,r6	;setasg
;68a
L428:
	cmpd	r6,_cwp	;38
	bge	L427
	cmpd	0(r6),0(-24(fp))	;38
	bne	L429
	cmpqd	0,_nolook	;35
	beq	L430
	br	L431
L430:
	;unary and,forarg 115a...
	addr	_clset,tos	;setasg
;68a
	;forarg 115d...
	;r6 intareg(nasl) 114b...
	movd	r6,r0	;59b
	addr	8(r0),r0	;86e  r0 += 8
	movd	r0,tos	;59a
	cxp	_setunion	;44a
	adjspb	-8
	cmpqd	0,r0	;35
	beq	L432
	movqd	1,-12(fp)	;59a
	movqd	1,4(r6)	;59a
L432:
	br	L431
L429:
L426:
	addr	24(r6),r6	;86e  r6 += 24
	br	L428
L427:
	;& _wsets intareg 114a...
	addr	_wsets,r0	;setasg
	;_cwp intareg(nasl) 114b...
	movd	_cwp,r1	;59b
	subd	r0,r1	;105t
	quod	24,r1	;105r
	addqd	1,r1	;82
	addr	@350,r0
	cmpd	r0,r1	;37a2
	bgt	L433
	;unary and,forarg 115a...
	addr	L434,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-4
L433:
	;-24(fp) intareg(nasl) 114b...
	movd	-24(fp),r0	;59b
	;_cwp intareg(nasl) 114b...
	movd	_cwp,r1	;59b
	movd	0(r0),0(r1)	;59b
	;_cwp intareg(nasl) 114b...
	movd	_cwp,r0	;59b
	movqd	1,4(r0)	;59a
	cmpqd	0,_nolook	;35
	bne	L435
	movqd	1,-12(fp)	;59a
	movqd	0,-16(fp)	;59a
L438:
	cmpd	-16(fp),_tbitset	;38
	bge	L437
	;-16(fp) intareg(nasl) 114b...
	movd	-16(fp),r0	;59b
	;-16(fp) intareg(nasl) 114b...
	movd	-16(fp),r1	;59b
	;_cwp intareg(nasl) 114b...
	movd	_cwp,r2	;59b
	addr	8(r2),r2	;86e  r2 += 8
	movd	_clset[r0:d],0(r2)[r1:d]	;59b
L436:
	addqd	1,-16(fp)	;83
	br	L438
L437:
L435:
	addd	24,_cwp	;105q
L431:
L423:
	addqd	4,-24(fp)	;83
	br	L425
L424:
L408:
	addr	24(r7),r7	;86e  r7 += 24
	br	L410
L409:
	br	L406
L407:
	cmpd	_cwp,_zzcwp	;38
	ble	L439
	movd	_cwp,_zzcwp	;59b
L439:
	cmpqd	0,_cldebug	;35
	beq	L440
	cmpqd	0,_foutput	;35
	beq	L440
	;forarg 115d...
	movd	_nolook,tos	;59b
	;forarg 115d...
	movd	12(fp),tos	;59b
	;unary and,forarg 115a...
	addr	L441,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-16
	addr	_wsets,r7	;setasg
;68a
L444:
	cmpd	r7,_cwp	;38
	bge	L443
	cmpqd	0,4(r7)	;35
	beq	L445
	;unary and,forarg 115a...
	addr	L446,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-8
L445:
	movqd	0,4(r7)	;59a
	;forarg 115d...
	movd	0(r7),tos	;59b
	cxp	_writem	;44a
	adjspb	-4
	;forarg 115d...
	movd	r0,tos	;59a
	;unary and,forarg 115a...
	addr	L447,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-12
	;forarg 115d...
	;r7 intareg(nasl) 114b...
	movd	r7,r0	;59b
	addr	8(r0),r0	;86e  r0 += 8
	movd	r0,tos	;59a
	cxp	_prlook	;44a
	adjspb	-4
	;unary and,forarg 115a...
	addr	L448,tos	;setasg
;68a
	;forarg 115d...
	movd	_foutput,tos	;59b
	cxp	_fprintf	;44a
	adjspb	-8
L442:
	addr	24(r7),r7	;86e  r7 += 24
	br	L444
L443:
L440:
L397:
	exit	[r5,r6,r7]
	rxp	0
L398:
	enter	[r5,r6,r7],32
	br 	L399
.endseg
.static
.endseg
.program
.exportp	_flset
_flset::
	br 	L451	;bfcode
L452:
;_p:	.EQU	12(fp)
;_q:	.EQU r7
;_j:	.EQU	-4(fp)
;_w:	.EQU	-8(fp)
;_u:	.EQU r6
;_v:	.EQU r5
	addr	_lkst,r7	;setasg
;68a
	;_nlset intareg(nasl) 114b...
	movd	_nlset,r0	;59b
	ashd	4,r0	;50b
	addd	r0,r7	;105t
;68a
L455:
	;& _lkst intareg 114a...
	addr	_lkst,r0	;setasg
	cmpd	r0,r7	;38
	subd	16,r7	;105r
	bge	L454
	movd	12(fp),r6	;59b
	movd	r7,r5	;59b
	;_tbitset intareg(nasl) 114b...
	movd	_tbitset,r0	;59b
	addr	@0[r0:d],r0	;fast r0 <<= 2 ;49b
	;r5 intareg(nasl) 114b...
	movd	r5,r1	;59b
	addd	r0,r1	;105t
	movd	r1,-8(fp)	;59a
L456:
	cmpd	r5,-8(fp)	;38
	bge	L457
	;intareg 114i...
	movd	r6,r0	;59b
	addqd	4,r6	;82
	;intareg 114i...
	movd	r5,r1	;59b
	addqd	4,r5	;82
	cmpd	0(r0),0(r1)	;38
	beq	L458
	br	L459
L458:
	br	L456
L457:
	;r7 intareg(nasl) 114b...
	movd	r7,r0	;59b
	br	L450
L459:
L453:
	br	L455
L454:
	addr	_lkst,r7	;setasg
;68a
	;_nlset intareg(nasl) 114b...
	movd	_nlset,r0	;59b
	ashd	4,r0	;50b
	addd	r0,r7	;105t
;68a
	addqd	1,_nlset	;83
	addr	@600,r0
	cmpd	r0,_nlset	;37a2
	bgt	L460
	;unary and,forarg 115a...
	addr	L461,tos	;setasg
;68a
	cxp	_error	;44a
	adjspb	-4
L460:
	movqd	0,-4(fp)	;59a
L464:
	cmpd	-4(fp),_tbitset	;38
	bge	L463
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r0	;59b
	;-4(fp) intareg(nasl) 114b...
	movd	-4(fp),r1	;59b
	movd	0(12(fp))[r0:d],0(r7)[r1:d]	;59b
L462:
	addqd	1,-4(fp)	;83
	br	L464
L463:
	;r7 intareg(nasl) 114b...
	movd	r7,r0	;59b
	br	L450
L450:
	exit	[r5,r6,r7]
	rxp	0
L451:
	enter	[r5,r6,r7],8
	br 	L452
.endseg
.static
	.importp _sprintf		;ejobcode
	.importp _fprintf		;ejobcode
	.importp _apack		;ejobcode
	.import	__iob		;ejobcode
	.importp _fopen		;ejobcode
	.import	_finput		;ejobcode
	.import	_faction		;ejobcode
	.import	_fdefine		;ejobcode
	.import	_ftable		;ejobcode
	.import	_ftemp		;ejobcode
	.import	_foutput		;ejobcode
	.import	_ntokens		;ejobcode
	.import	_tokset		;ejobcode
	.import	_nnonter		;ejobcode
	.import	_nontrst		;ejobcode
	.import	_nprod		;ejobcode
	.import	_prdptr		;ejobcode
	.import	_levprd		;ejobcode
	.comm	_pstate,3008		;ejobcode
	.comm	_tystate,3000		;ejobcode
	.import	_defact		;ejobcode
	.comm	_tstates,508		;ejobcode
	.comm	_ntstates,1200		;ejobcode
	.comm	_mstates,3000		;ejobcode
	.comm	_lkst,9600		;ejobcode
	.comm	_wsets,8400		;ejobcode
	.comm	_cwp,4		;ejobcode
	.import	_mem0		;ejobcode
	.import	_mem		;ejobcode
	.comm	_amem,48000		;ejobcode
	.comm	_indgo,3000		;ejobcode
	.comm	_temp1,4800		;ejobcode
	.comm	_tbitset,4		;ejobcode
	.comm	_clset,16		;ejobcode
	.comm	_pres,1208		;ejobcode
	.comm	_pfirst,1208		;ejobcode
	.comm	_pempty,1204		;ejobcode
	.importp _setup		;ejobcode
	.importp _output		;ejobcode
	.importp _go2out		;ejobcode
	.importp _hideprod		;ejobcode
	.importp _callopt		;ejobcode
	.importp _exit		;ejobcode
	.importp _warray		;ejobcode
	.importp __filbuf		;ejobcode
	.importp __flsbuf		;ejobcode
	.importp _fclose		;ejobcode
	.importp _unlink		;ejobcode
;NS16032 C compiler release 4.25 6/15/83 (JZ,jima)
L90:
	.byte	"/usr/lib/yaccpar",x'0
L91:
	.byte	"r",x'0
L94:
	.byte	"cannot find parser %s",x'0
L95:
	.byte	"/usr/lib/yaccpar",x'0
L97:
	.byte	"yyr1",x'0
L102:
	.byte	"yyr2",x'0
L115:
	.byte	"yychk",x'0
L116:
	.byte	"yydef",x'0
L124:
	.byte	"yacc.acts",x'0
L125:
	.byte	"r",x'0
L127:
	.byte	"cannot reopen action tempfile",x'0
L132:
	.byte	"yacc.acts",x'0
L148:
	.byte	" : ",x'0
L154:
	.byte	"item too big",x'0
L156:
	.byte	"    (",x'0
L158:
	.byte	"%d)",x'0
L172:
	.byte	x'a,"%d/%d terminals, %d/%d nonterminals",x'a,x'0
L173:
	.byte	"%d/%d grammar rules, %d/%d states",x'a,x'0
L174:
	.byte	"%d shift/reduce, %d reduce/reduce conflicts reported",x'a,x'0
L175:
	.byte	"%d/%d working sets used",x'a,x'0
L176:
	.byte	"memory: states,etc. %d/%d, parser %d/%d",x'a,x'0
L177:
	.byte	"%d/%d distinct lookahead sets",x'a,x'0
L178:
	.byte	"%d extra closures",x'a,x'0
L179:
	.byte	"%d shift entries, %d exceptions",x'a,x'0
L180:
	.byte	"%d goto entries",x'a,x'0
L181:
	.byte	"%d entries saved by goto default",x'a,x'0
L183:
	.byte	x'a,"conflicts: ",x'0
L185:
	.byte	"%d shift/reduce",x'0
L187:
	.byte	", ",x'0
L189:
	.byte	"%d reduce/reduce",x'0
L190:
	.byte	x'a,x'0
L196:
	.byte	x'a," fatal error: ",x'0
L197:
	.byte	", line %d",x'a,x'0
L221:
	.byte	x'9,"NULL",x'0
L223:
	.byte	" { ",x'0
L228:
	.byte	"%s ",x'0
L229:
	.byte	"}",x'0
L243:
	.byte	"nonterminal %s not defined!",x'0
L246:
	.byte	"internal Yacc error: pyield %d",x'0
L283:
	.byte	x'a,"%s: ",x'0
L284:
	.byte	" %d",x'a,x'0
L316:
	.byte	"yacc state/nolook error",x'0
L318:
	.byte	"too many states",x'0
L328:
	.byte	"putitem(%s), state %d",x'a,x'0
L332:
	.byte	"out of state space",x'0
L352:
	.byte	"nonterminal %s never derives any token string",x'0
L387:
	.byte	"%d: ",x'0
L392:
	.byte	"%s %d, ",x'0
L393:
	.byte	x'a,x'0
L434:
	.byte	"working set overflow",x'0
L441:
	.byte	x'a,"State %d, nolook = %d",x'a,x'0
L446:
	.byte	"flag set!",x'a,x'0
L447:
	.byte	x'9,"%s",x'0
L448:
	.byte	x'a,x'0
L461:
	.byte	"too many lookahead sets",x'0
	.endseg

; Initcode 
	.program
%initfun::
	addr _amem,S0
	addr _wsets,S1
	addr _mem0,S2
	rxp 0
	.endseg
