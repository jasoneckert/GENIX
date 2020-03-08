;        0 : trec.stream                  
;        4 : trec.item_size               
;        8 : trec.buf                     
;       12 : trec.fs                      
;       13 : trec.ls                      
;       14 : trec.b                       
;       15 : trec.f_flushed               
_write.textstring2::	; Level:      1
	enter   [r3],0
;Line 49 module _all
	movqd   1,r3
L7:
	movd    r3,r0
	cmpqb   0,-1(12(fp))[r0:B]
	beq     L2
;Line 52 module _all
;Line 52 module _all
	movd    r3,r0
	movzbd  -1(12(fp))[r0:B],tos
	cxp     _putchar
	adjspb  -4
	addqd   1,r3
	br      L7
L2:
	exit    [r3]
	rxp     8
;	_i	r3
;	_t	12(fp)
;	_t1	16(fp)
_write.textstring3::	; Level:      1
	enter   [r3,r4,r5],0
;Line 64 module _all
	cmpqd   0,12(fp)
	bgt     L15
;Line 67 module _all
;Line 67 module _all
	movqd   1,r4
L19:
	cmpd    r4,12(fp)
	bgt     L21
	movd    r4,r0
	cmpqb   0,-1(16(fp))[r0:B]
	beq     L21
;Line 68 module _all
	addqd   1,r4
	br      L19
L21:
	movd    12(fp),r0
	subd    r4,r0
	movd    r0,r5
	addqd   1,r5
	movqd   1,r3
	cmpd    r3,r5
	bgt     L25
L24:
;Line 69 module _all
	addr    @32,tos
	cxp     _putchar
	adjspb  -4
	cmpd    r3,r5
	bge     L25
	addqd   1,r3
	br      L24
L25:
	addqd   -1,r4
	movqd   1,r3
	cmpd    r3,r4
	bgt     L16
L27:
;Line 70 module _all
	movd    r3,r0
	movzbd  -1(16(fp))[r0:B],tos
	cxp     _putchar
	adjspb  -4
	cmpd    r3,r4
	bge     L16
	addqd   1,r3
	br      L27
	br      L10
L15:
;Line 64 module _all
	movd    20(fp),tos
	addr    L18,tos
	cxp     _write.textstring2
L16:
L10:
	exit    [r3,r4,r5]
	rxp     12
;	_k	r4
;	_i	r3
;	_j	12(fp)
;	_t	16(fp)
;	_t1	20(fp)
L18:
	.byte   'error : Negative width specified for formatting.',0
_write.textchar2::	; Level:      1
	enter   0
;Line 79 module _all
	movd    12(fp),tos
	cxp     _putchar
	adjspb  -4
	exit    
	rxp     8
;	_c	12(fp)
;	_t1	16(fp)
_write.textchar3::	; Level:      1
	enter   [r3,r4,r5],0
;Line 90 module _all
	cmpqd   0,12(fp)
	bgt     L39
;Line 93 module _all
;Line 93 module _all
	movb    32,r5
	movd    12(fp),r4
	addqd   -1,r4
	movqd   1,r3
	cmpd    r3,r4
	bgt     L45
L44:
;Line 94 module _all
	movzbd  r5,tos
	cxp     _putchar
	adjspb  -4
	cmpd    r3,r4
	bge     L45
	addqd   1,r3
	br      L44
L45:
	movd    16(fp),tos
	cxp     _putchar
	adjspb  -4
	br      L34
L39:
;Line 90 module _all
	movd    20(fp),tos
	addr    L42,tos
	cxp     _write.textstring2
L34:
	exit    [r3,r4,r5]
	rxp     12
;	_i	r3
;	_ch	r5
;	_j	12(fp)
;	_c	16(fp)
;	_t1	20(fp)
L42:
	.byte   'error : Negative width specified for formatting.',0
_writeln.text::	; Level:      1
	enter   0
	movb    10,r0
;Line 107 module _all
	movzbd  r0,tos
	cxp     _putchar
	adjspb  -4
	exit    
	rxp     4
;	_lf	r0
;	_t1	12(fp)
_write.textinteger3::	; Level:      1
	enter   [r3,r4,r5],12
	addr    @10,r4
	movmd   L56,-12(fp),3
	absd    16(fp),r1
;Line 121 module _all
	cmpqd   0,12(fp)
	bgt     L57
;Line 124 module _all
;Line 124 module _all
	cmpd    -2147483648,16(fp)
	beq     L61
;Line 129 module _all
;Line 129 module _all
	movqd   0,r3
L67:
;Line 131 module _all
	movd    r1,r0
	modd    r4,r0
	movd    r0,r2
	addd    48,r2
	addr    @11,r0
	subd    r3,r0
	movb    r2,-13(fp)[r0:B]
	addqd   1,r3
	quod    r4,r1
	cmpqd   0,r1
	bne     L67
	cmpqd   0,16(fp)
	ble     L62
;Line 136 module _all
;Line 136 module _all
	addr    @11,r0
	subd    r3,r0
	movb    45,-13(fp)[r0:B]
	addqd   1,r3
	br      L62
L61:
;Line 125 module _all
;Line 125 module _all
	movmd   L64,-12(fp),3
	addr    @11,r3
L62:
	movqb   0,-1(fp)
	cmpd    12(fp),r3
	ble     L71
;Line 142 module _all
;Line 142 module _all
	movd    12(fp),r5
	subd    r3,r5
	movqd   1,r4
	cmpd    r4,r5
	bgt     L71
L73:
;Line 142 module _all
	addr    @32,tos
	cxp     _putchar
	adjspb  -4
	cmpd    r4,r5
	bge     L71
	addqd   1,r4
	br      L73
L71:
	addr    @12,r0
	subd    r3,r0
	movd    r0,r4
	cmpd    11,r4
	blt     L58
L76:
;Line 144 module _all
	movd    r4,r0
	movzbd  -13(fp)[r0:B],tos
	cxp     _putchar
	adjspb  -4
	cmpd    11,r4
	ble     L58
	addqd   1,r4
	br      L76
	br      L51
L57:
;Line 121 module _all
	movd    20(fp),tos
	addr    L60,tos
	cxp     _write.textstring2
L58:
L51:
	exit    [r3,r4,r5]
	rxp     12
;	_posn	r1
;	_radix	r4
;	_j	r4
;	_i	r3
;	_l	12(fp)
;	_n	16(fp)
;	_t1	20(fp)
;	_s	-12(fp)
L56:
	.byte   '            ',0
L60:
	.byte   'error : Negative width specified for formatting.',0
L64:
	.byte   '-2147483648 ',0
_write.textinteger2::	; Level:      1
	enter   0
;Line 155 module _all
	addr    @11,r0
	movd    16(fp),tos
	movd    12(fp),tos
	movd    r0,tos
	cxp     _write.textinteger3
	exit    
	rxp     8
;	_k	r0
;	_n	12(fp)
;	_t1	16(fp)
_write.textboolean2::	; Level:      1
	enter   0
;Line 164 module _all
	cmpqd   1,12(fp)
	bne     L90
;Line 164 module _all
	movd    16(fp),tos
	addr    L91,tos
	cxp     _write.textstring2
	br      L83
L90:
;Line 165 module _all
	movd    16(fp),tos
	addr    L92,tos
	cxp     _write.textstring2
L83:
	exit    
	rxp     8
;	_b	12(fp)
;	_t1	16(fp)
L91:
	.byte   'true',0
L92:
	.byte   'false',0
_write.textboolean3::	; Level:      1
	enter   0
;Line 173 module _all
	cmpqd   0,12(fp)
	bgt     L98
;Line 176 module _all
;Line 176 module _all
	cmpqd   1,16(fp)
	bne     L104
;Line 176 module _all
	movd    20(fp),tos
	addr    L105,tos
	movd    12(fp),tos
	cxp     _write.textstring3
	br      L99
L104:
;Line 177 module _all
	movd    20(fp),tos
	addr    L106,tos
	movd    12(fp),tos
	cxp     _write.textstring3
	br      L93
L98:
;Line 173 module _all
	movd    20(fp),tos
	addr    L101,tos
	cxp     _write.textstring2
L99:
L93:
	exit    
	rxp     12
;	_j	12(fp)
;	_b	16(fp)
;	_t1	20(fp)
L101:
	.byte   'error : Negative width specified for formatting.',0
L105:
	.byte   'true',0
L106:
	.byte   'false',0
_read.textchar2::	; Level:      1
	enter   0
;Line 186 module _all
	cxp     _getchar
	movb    r0,0(12(fp))
	movb    0(12(fp)),0(sb)
	exit    
	rxp     8
;	_c	12(fp)
;	_t1	16(fp)
_read.textstring3::	; Level:      1
	enter   [r3],0
;Line 197 module _all
	cmpqd   0,0(12(fp))
	bgt     L117
;Line 200 module _all
;Line 200 module _all
	movd    0(12(fp)),r0
	addqd   -1,r0
	movd    r0,r3
	movqd   1,0(12(fp))
	cxp     _getchar
	movb    r0,r1
	movd    0(12(fp)),r0
	movb    r1,-1(16(fp))[r0:B]
L121:
	movd    0(12(fp)),r0
	cmpqb   0,-1(16(fp))[r0:B]
	beq     L123
	cmpqd   0,r3
	bge     L123
;Line 205 module _all
;Line 205 module _all
	addqd   1,0(12(fp))
	cxp     _getchar
	movb    r0,r1
	movd    0(12(fp)),r0
	movb    r1,-1(16(fp))[r0:B]
	addqd   -1,r3
	br      L121
L123:
	movd    0(12(fp)),r0
	movb    -1(16(fp))[r0:B],0(sb)
	br      L112
L117:
;Line 197 module _all
	movd    20(fp),tos
	addr    L120,tos
	cxp     _write.textstring2
L112:
	exit    [r3]
	rxp     12
;	_bound	r3
;	_j	12(fp)
;	_t	16(fp)
;	_t1	20(fp)
L120:
	.byte   'error : Negative width specified for formatting.',0
_readln.text::	; Level:      1
	enter   0
;Line 219 module _all
L130:
	cmpb    13,0(sb)
	beq     L125
;Line 219 module _all
	cxp     _getchar
	movb    r0,0(sb)
	br      L130
L125:
	exit    
	rxp     4
;	_t1	12(fp)
_read.textinteger2::	; Level:      1
	enter   [r3,r4,r5],0
;Line 230 module _all
	movqb   0,r4
	movqb   1,r5
L140:
;Line 233 module _all
	cxp     _getchar
	movb    r0,r3
	cmpb    32,r3
	beq     L141
	cmpb    9,r3
	bne     L139
L141:
	cmpb    13,r3
	bne     L140
L139:
	cmpb    13,r3
	beq     L144
;Line 237 module _all
;Line 237 module _all
	cmpb    45,r3
	beq     L145
;Line 242 module _all
	cmpb    43,r3
	bne     L146
;Line 242 module _all
	cxp     _getchar
	movb    r0,r3
	br      L146
L145:
;Line 239 module _all
;Line 239 module _all
	movqb   0,r5
	cxp     _getchar
	movb    r0,r3
L146:
	movzbd  r3,r0
	subd    48,r0
	movd    r0,0(12(fp))
	cxp     _getchar
	movb    r0,r3
L150:
	cmpb    13,r3
	beq     L144
	cmpb    57,r3
	blo     L144
	cmpb    48,r3
	bhi     L144
	cmpqb   1,r4
	beq     L144
;Line 247 module _all
;Line 247 module _all
	cmpd    214748364,0(12(fp))
	bgt     L156
	cmpd    214748364,0(12(fp))
	bne     L158
	cmpqb   7,r3
	bhs     L156
L158:
;Line 253 module _all
	cmpd    214748364,0(12(fp))
	bne     L163
	br      L163
	cmpqb   1,r5
	bne     L161
L163:
;Line 261 module _all
;Line 261 module _all
	movd    16(fp),tos
	addr    L166,tos
	cxp     _write.textstring2
	movqb   1,r4
	br      L157
L161:
;Line 256 module _all
;Line 256 module _all
	movd    -2147483648,0(12(fp))
	movqb   1,r4
	movqb   1,r5
	br      L150
L156:
;Line 250 module _all
;Line 250 module _all
	movd    0(12(fp)),r0
	muld    10,r0
	movd    r0,r1
	movzbw  r3,r0
	subw    48,r0
	movxwd  r0,r0
	addd    r0,r1
	movd    r1,0(12(fp))
	cxp     _getchar
	movb    r0,r3
L157:
	br      L150
L144:
	movb    r3,0(sb)
	exit    [r3,r4,r5]
	rxp     8
;	_sign	r5
;	_flag	r4
;	_c	r3
;	_n	12(fp)
;	_t1	16(fp)
L166:
	.byte   'error : Too big integer for input.',0
_all::	; Level:      0
	enter   0
;Line 271 module _all
	exit    
	rxp     0
;	_endch	0(sb)
;	_input	2(sb)
;	_output	18(sb)
	.import _putchar
	.import _getchar
	.static 
_endch:
	.blkb   2
_input::
	.blkb   16
_output::
	.blkb   16
	.program
	.module pio.p
