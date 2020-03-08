; @(#)Typerite_10.s	3.1	8/11/83
; @(#)Copyright (C) 1983 by National Semiconductor Corp.


; This file was generated from a Mesa font file with the following commands:
; 
;     fconvert /v/mesaux/lib/vfont/Typewrite.10
;     mfont _Typerite_10 newfont
;     mv font.data Typerite_10.s

	.static

; Font header.

_Typerite_10::	.word	287,1838,0,130,0

; Character dispatch table

	.word	0,14		; char <?> : 0
	.byte	11,3,0,7
	.word	14,14		; char <?> : 1
	.byte	11,3,0,7
	.word	28,14		; char <?> : 2
	.byte	11,3,0,7
	.word	42,14		; char <?> : 3
	.byte	11,3,0,7
	.word	56,14		; char <?> : 4
	.byte	11,3,0,7
	.word	70,14		; char <?> : 5
	.byte	11,3,0,7
	.word	84,14		; char <?> : 6
	.byte	11,3,0,7
	.word	98,14		; char <?> : 7
	.byte	11,3,0,7
	.word	112,14		; char <?> : 8
	.byte	11,3,0,7
	.word	126,14		; char <?> : 9
	.byte	11,3,0,7
	.word	140,14		; char <?> : 10
	.byte	11,3,0,7
	.word	154,14		; char <?> : 11
	.byte	11,3,0,7
	.word	168,14		; char <?> : 12
	.byte	11,3,0,7
	.word	182,14		; char <?> : 13
	.byte	11,3,0,7
	.word	196,14		; char <?> : 14
	.byte	11,3,0,7
	.word	210,14		; char <?> : 15
	.byte	11,3,0,7
	.word	224,14		; char <?> : 16
	.byte	11,3,0,7
	.word	238,14		; char <?> : 17
	.byte	11,3,0,7
	.word	252,14		; char <?> : 18
	.byte	11,3,0,7
	.word	266,14		; char <?> : 19
	.byte	11,3,0,7
	.word	280,14		; char <?> : 20
	.byte	11,3,0,7
	.word	294,14		; char <?> : 21
	.byte	11,3,0,7
	.word	308,14		; char <?> : 22
	.byte	11,3,0,7
	.word	322,14		; char <?> : 23
	.byte	11,3,0,7
	.word	336,14		; char <?> : 24
	.byte	11,3,0,7
	.word	350,14		; char <?> : 25
	.byte	11,3,0,7
	.word	364,14		; char <?> : 26
	.byte	11,3,0,7
	.word	378,14		; char <?> : 27
	.byte	11,3,0,7
	.word	392,14		; char <?> : 28
	.byte	11,3,0,7
	.word	406,14		; char <?> : 29
	.byte	11,3,0,7
	.word	420,14		; char <?> : 30
	.byte	11,3,0,7
	.word	434,14		; char <?> : 31
	.byte	11,3,0,7
	.word	448,14		; char < > : 32
	.byte	1,0,0,7
	.word	462,14		; char <!> : 33
	.byte	10,0,0,7
	.word	476,14		; char <"> : 34
	.byte	10,-5,0,7
	.word	490,14		; char <#> : 35
	.byte	10,0,0,7
	.word	504,14		; char <$> : 36
	.byte	11,1,0,7
	.word	518,14		; char <%> : 37
	.byte	10,0,0,7
	.word	532,14		; char <&> : 38
	.byte	10,0,0,7
	.word	546,14		; char <'> : 39
	.byte	10,-5,0,7
	.word	560,14		; char <(> : 40
	.byte	11,2,0,7
	.word	574,14		; char <)> : 41
	.byte	11,2,0,7
	.word	588,14		; char <*> : 42
	.byte	8,-1,0,7
	.word	602,14		; char <+> : 43
	.byte	8,-1,0,7
	.word	616,14		; char <,> : 44
	.byte	2,3,0,7
	.word	630,14		; char <-> : 45
	.byte	5,-4,0,7
	.word	644,14		; char <.> : 46
	.byte	2,0,0,7
	.word	658,14		; char </> : 47
	.byte	10,0,0,7
	.word	672,14		; char <0> : 48
	.byte	10,0,0,7
	.word	686,14		; char <1> : 49
	.byte	10,0,0,7
	.word	700,14		; char <2> : 50
	.byte	10,0,0,7
	.word	714,14		; char <3> : 51
	.byte	10,0,0,7
	.word	728,14		; char <4> : 52
	.byte	10,0,0,7
	.word	742,14		; char <5> : 53
	.byte	10,0,0,7
	.word	756,14		; char <6> : 54
	.byte	10,0,0,7
	.word	770,14		; char <7> : 55
	.byte	10,0,0,7
	.word	784,14		; char <8> : 56
	.byte	10,0,0,7
	.word	798,14		; char <9> : 57
	.byte	10,0,0,7
	.word	812,14		; char <:> : 58
	.byte	9,-2,0,7
	.word	826,14		; char <;> : 59
	.byte	6,3,0,7
	.word	840,14		; char <<> : 60
	.byte	9,0,0,7
	.word	854,14		; char <=> : 61
	.byte	7,-4,0,7
	.word	868,14		; char <>> : 62
	.byte	9,0,0,7
	.word	882,14		; char <?> : 63
	.byte	10,0,0,7
	.word	896,14		; char <@> : 64
	.byte	10,0,0,7
	.word	910,14		; char <A> : 65
	.byte	10,0,0,7
	.word	924,14		; char <B> : 66
	.byte	10,0,0,7
	.word	938,14		; char <C> : 67
	.byte	10,0,0,7
	.word	952,14		; char <D> : 68
	.byte	10,0,0,7
	.word	966,14		; char <E> : 69
	.byte	10,0,0,7
	.word	980,14		; char <F> : 70
	.byte	10,0,0,7
	.word	994,14		; char <G> : 71
	.byte	10,0,0,7
	.word	1008,14		; char <H> : 72
	.byte	10,0,0,7
	.word	1022,14		; char <I> : 73
	.byte	10,0,0,7
	.word	1036,14		; char <J> : 74
	.byte	10,0,0,7
	.word	1050,14		; char <K> : 75
	.byte	10,0,0,7
	.word	1064,14		; char <L> : 76
	.byte	10,0,0,7
	.word	1078,14		; char <M> : 77
	.byte	10,0,0,7
	.word	1092,14		; char <N> : 78
	.byte	10,0,0,7
	.word	1106,14		; char <O> : 79
	.byte	10,0,0,7
	.word	1120,14		; char <P> : 80
	.byte	10,0,0,7
	.word	1134,14		; char <Q> : 81
	.byte	10,2,0,7
	.word	1148,14		; char <R> : 82
	.byte	10,0,0,7
	.word	1162,14		; char <S> : 83
	.byte	10,0,0,7
	.word	1176,14		; char <T> : 84
	.byte	10,0,0,7
	.word	1190,14		; char <U> : 85
	.byte	10,0,0,7
	.word	1204,14		; char <V> : 86
	.byte	10,0,0,7
	.word	1218,14		; char <W> : 87
	.byte	10,0,0,7
	.word	1232,14		; char <X> : 88
	.byte	10,0,0,7
	.word	1246,14		; char <Y> : 89
	.byte	10,0,0,7
	.word	1260,14		; char <Z> : 90
	.byte	10,0,0,7
	.word	1274,14		; char <[> : 91
	.byte	11,2,0,7
	.word	1288,14		; char <\> : 92
	.byte	10,0,0,7
	.word	1302,14		; char <]> : 93
	.byte	11,2,0,7
	.word	1316,14		; char <^> : 94
	.byte	10,-5,0,7
	.word	1330,14		; char <_> : 95
	.byte	1,0,0,7
	.word	1344,14		; char <`> : 96
	.byte	10,-5,0,7
	.word	1358,14		; char <a> : 97
	.byte	7,0,0,7
	.word	1372,14		; char <b> : 98
	.byte	10,0,0,7
	.word	1386,14		; char <c> : 99
	.byte	7,0,0,7
	.word	1400,14		; char <d> : 100
	.byte	10,0,0,7
	.word	1414,14		; char <e> : 101
	.byte	7,0,0,7
	.word	1428,14		; char <f> : 102
	.byte	10,0,0,7
	.word	1442,14		; char <g> : 103
	.byte	7,2,0,7
	.word	1456,14		; char <h> : 104
	.byte	10,0,0,7
	.word	1470,14		; char <i> : 105
	.byte	10,0,0,7
	.word	1484,14		; char <j> : 106
	.byte	10,3,0,7
	.word	1498,14		; char <k> : 107
	.byte	10,0,0,7
	.word	1512,14		; char <l> : 108
	.byte	10,0,0,7
	.word	1526,14		; char <m> : 109
	.byte	7,0,0,7
	.word	1540,14		; char <n> : 110
	.byte	7,0,0,7
	.word	1554,14		; char <o> : 111
	.byte	7,0,0,7
	.word	1568,14		; char <p> : 112
	.byte	7,3,0,7
	.word	1582,14		; char <q> : 113
	.byte	7,3,0,7
	.word	1596,14		; char <r> : 114
	.byte	7,0,0,7
	.word	1610,14		; char <s> : 115
	.byte	7,0,0,7
	.word	1624,14		; char <t> : 116
	.byte	9,0,0,7
	.word	1638,14		; char <u> : 117
	.byte	7,0,0,7
	.word	1652,14		; char <v> : 118
	.byte	7,0,0,7
	.word	1666,14		; char <w> : 119
	.byte	7,0,0,7
	.word	1680,14		; char <x> : 120
	.byte	7,0,0,7
	.word	1694,14		; char <y> : 121
	.byte	7,2,0,7
	.word	1708,14		; char <z> : 122
	.byte	7,0,0,7
	.word	1722,14		; char <{> : 123
	.byte	11,2,0,7
	.word	1736,14		; char <|> : 124
	.byte	11,1,0,7
	.word	1750,14		; char <}> : 125
	.byte	11,2,0,7
	.word	1764,14		; char <~> : 126
	.byte	10,-7,0,7
	.word	1778,14		; char <?> : 127
	.byte	11,3,0,7
	.word	1792,14		; char <?> : 128
	.byte	11,3,0,7
	.word	1806,16		; char <?> : 129
	.byte	11,4,0,8
	.word	1822,16		; char <?> : 130
	.byte	0,1,0,8

; Bit maps.

	.word	h'3f80, h'1000, h'087f, h'0401
	.word	h'3f81, h'0001, h'0001, h'1900
	.word	h'2480, h'24ff, h'2488, h'1308
	.word	h'0008, h'007f, h'1900, h'2480
	.word	h'24e3, h'2494, h'1308, h'0014
	.word	h'0063, h'3f80, h'2480, h'24e3
	.word	h'2494, h'2088, h'0014, h'0063
	.word	h'3f80, h'2480, h'24c0, h'24c0
	.word	h'20ff, h'0040, h'0040, h'3f80
	.word	h'2480, h'24be, h'24c1, h'20c5
	.word	h'0042, h'003d, h'0180, h'0e00
	.word	h'327f, h'0e08, h'0194, h'0022
	.word	h'0041, h'3f80, h'2480, h'24ff
	.word	h'2481, h'1b01, h'0001, h'0001
	.word	h'3f80, h'2480, h'24b2, h'24c9
	.word	h'1b49, h'0049, h'0026, h'3f80
	.word	h'0400, h'0440, h'0440, h'3fff
	.word	h'0040, h'0040, h'3f80, h'0080
	.word	h'00ff, h'00c8, h'00c8, h'0048
	.word	h'0040, h'3000, h'0e00, h'01c0
	.word	h'0e40, h'307f, h'0040, h'0040
	.word	h'3f80, h'2400, h'247f, h'2448
	.word	h'2048, h'0048, h'0040, h'1f00
	.word	h'2080, h'20ff, h'20c8, h'114c
	.word	h'004a, h'0031, h'1900, h'2480
	.word	h'24be, h'24c1, h'1341, h'0041
	.word	h'003e, h'1900, h'2480, h'24c1
	.word	h'24c1, h'137f, h'0041, h'0041
	.word	h'3f80, h'2080, h'20ff, h'2081
	.word	h'1f01, h'0001, h'0001, h'3f80
	.word	h'2080, h'2091, h'20a1, h'1f7f
	.word	h'0001, h'0001, h'3f80, h'2080
	.word	h'20a1, h'20c3, h'1f45, h'0049
	.word	h'0031, h'3f80, h'2080, h'20a2
	.word	h'20c1, h'1f49, h'0049, h'0036
	.word	h'3f80, h'2080, h'208c, h'2094
	.word	h'1f24, h'007f, h'0004, h'3f80
	.word	h'1000, h'087f, h'0408, h'3f94
	.word	h'0022, h'0041, h'1900, h'2480
	.word	h'24e0, h'2490, h'130f, h'0010
	.word	h'0060, h'3f80, h'2480, h'24ff
	.word	h'24c9, h'20c9, h'0049, h'0036
	.word	h'1f00, h'2080, h'20ff, h'20a0
	.word	h'1110, h'0008, h'007f, h'3f80
	.word	h'2480, h'24ff, h'24a0, h'2098
	.word	h'0020, h'007f, h'1900, h'2480
	.word	h'24ff, h'24c9, h'1349, h'0049
	.word	h'0036, h'3f80, h'2480, h'24be
	.word	h'24c1, h'20c1, h'0041, h'0022
	.word	h'3f80, h'2400, h'2432, h'2449
	.word	h'2049, h'0049, h'0026, h'1f00
	.word	h'2080, h'24b2, h'24c9, h'1749
	.word	h'0049, h'0026, h'3f80, h'2400
	.word	h'2632, h'2549, h'18c9, h'0049
	.word	h'0026, h'3f00, h'0080, h'00b2
	.word	h'00c9, h'3f49, h'0049, h'0026
	.word	h'0000, h'0000, h'0000, h'0000
	.word	h'0000, h'0000, h'0000, h'0000
	.word	h'0000, h'03f3, h'03f3, h'0000
	.word	h'0000, h'0000, h'0000, h'001b
	.word	h'001e, h'0000, h'001b, h'001e
	.word	h'0000, h'0004, h'00bf, h'03e4
	.word	h'0084, h'009f, h'03f4, h'0080
	.word	h'038e, h'06c4, h'0442, h'0fff
	.word	h'0422, h'0236, h'071c, h'0180
	.word	h'0243, h'018e, h'0238, h'02e6
	.word	h'0389, h'0006, h'000e, h'01d3
	.word	h'0221, h'01d9, h'0026, h'0039
	.word	h'0022, h'0000, h'0000, h'001b
	.word	h'001e, h'0000, h'0000, h'0000
	.word	h'0000, h'0000, h'03f8, h'0e0e
	.word	h'1803, h'0000, h'0000, h'0000
	.word	h'0000, h'1803, h'0e0e, h'03f8
	.word	h'0000, h'0000, h'0008, h'005d
	.word	h'006b, h'001c, h'006b, h'005d
	.word	h'0008, h'0008, h'0008, h'0008
	.word	h'007f, h'0008, h'0008, h'0008
	.word	h'0000, h'0000, h'001b, h'001e
	.word	h'0000, h'0000, h'0000, h'0001
	.word	h'0001, h'0001, h'0001, h'0001
	.word	h'0001, h'0001, h'0000, h'0000
	.word	h'0003, h'0003, h'0000, h'0000
	.word	h'0000, h'0001, h'0007, h'001c
	.word	h'0070, h'01c0, h'0300, h'0000
	.word	h'00fc, h'0186, h'0303, h'0201
	.word	h'0303, h'0186, h'00fc, h'0000
	.word	h'0081, h'0181, h'03ff, h'0001
	.word	h'0001, h'0000, h'0183, h'0306
	.word	h'020e, h'021b, h'0211, h'0331
	.word	h'01e3, h'038e, h'020b, h'0221
	.word	h'0241, h'02e1, h'03b3, h'031e
	.word	h'0018, h'0028, h'00c9, h'0109
	.word	h'03ff, h'0009, h'0009, h'03ee
	.word	h'024b, h'0241, h'0241, h'0241
	.word	h'0263, h'033e, h'00fe, h'01b3
	.word	h'0321, h'0221, h'0221, h'02b3
	.word	h'019e, h'0380, h'0100, h'0200
	.word	h'020f, h'0338, h'01e0, h'0300
	.word	h'01de, h'0373, h'0221, h'0221
	.word	h'0221, h'0373, h'01de, h'01e6
	.word	h'0335, h'0211, h'0211, h'0213
	.word	h'0336, h'01fc, h'0000, h'0000
	.word	h'0063, h'0063, h'0000, h'0000
	.word	h'0000, h'0000, h'0000, h'019b
	.word	h'019e, h'0000, h'0000, h'0000
	.word	h'0010, h'0038, h'006c, h'00c6
	.word	h'0183, h'0101, h'0000, h'0005
	.word	h'0005, h'0005, h'0005, h'0005
	.word	h'0005, h'0005, h'0101, h'0183
	.word	h'00c6, h'006c, h'0038, h'0010
	.word	h'0000, h'0180, h'0300, h'0200
	.word	h'021b, h'0230, h'0360, h'01c0
	.word	h'01fe, h'0303, h'0279, h'0285
	.word	h'0279, h'030b, h'01f6, h'0001
	.word	h'000f, h'00f9, h'0388, h'00f9
	.word	h'000f, h'0001, h'0201, h'03ff
	.word	h'0221, h'0221, h'0221, h'0373
	.word	h'01de, h'00fc, h'0186, h'0303
	.word	h'0201, h'0201, h'0103, h'0386
	.word	h'0201, h'03ff, h'0201, h'0201
	.word	h'0201, h'0303, h'01fe, h'0201
	.word	h'03ff, h'0221, h'0221, h'0271
	.word	h'0201, h'0387, h'0201, h'03ff
	.word	h'0221, h'0220, h'0270, h'0200
	.word	h'0380, h'00fc, h'0186, h'0303
	.word	h'0201, h'0211, h'0113, h'039e
	.word	h'0201, h'03ff, h'0221, h'0020
	.word	h'0221, h'03ff, h'0201, h'0000
	.word	h'0201, h'0201, h'03ff, h'0201
	.word	h'0201, h'0000, h'0002, h'0003
	.word	h'0201, h'0201, h'03fe, h'0200
	.word	h'0200, h'0201, h'03ff, h'0231
	.word	h'0048, h'0285, h'0303, h'0201
	.word	h'0201, h'03ff, h'0201, h'0001
	.word	h'0001, h'0001, h'0007, h'0201
	.word	h'03ff, h'00c1, h'0038, h'00c1
	.word	h'03ff, h'0201, h'0201, h'03ff
	.word	h'00c1, h'0030, h'020c, h'03ff
	.word	h'0200, h'01fe, h'0303, h'0201
	.word	h'0201, h'0201, h'0303, h'01fe
	.word	h'0201, h'03ff, h'0221, h'0220
	.word	h'0220, h'0360, h'01c0, h'07f8
	.word	h'0c0c, h'0804, h'0806, h'0807
	.word	h'0c0d, h'07f8, h'0201, h'03ff
	.word	h'0221, h'0230, h'022d, h'0363
	.word	h'01c1, h'01c7, h'0362, h'0221
	.word	h'0231, h'0211, h'011b, h'038e
	.word	h'0380, h'0201, h'0201, h'03ff
	.word	h'0201, h'0201, h'0380, h'0200
	.word	h'03fe, h'0203, h'0001, h'0203
	.word	h'03fe, h'0200, h'0200, h'03c0
	.word	h'027c, h'0007, h'027c, h'03c0
	.word	h'0200, h'0200, h'03f0, h'021f
	.word	h'00f0, h'021f, h'03f0, h'0200
	.word	h'0201, h'0303, h'02cd, h'0030
	.word	h'02cd, h'0303, h'0201, h'0200
	.word	h'03c0, h'0271, h'001f, h'0271
	.word	h'03c0, h'0200, h'0387, h'020d
	.word	h'0219, h'0231, h'0261, h'02c1
	.word	h'0387, h'0000, h'0000, h'1fff
	.word	h'1001, h'1001, h'0000, h'0000
	.word	h'0300, h'01c0, h'0070, h'001c
	.word	h'0007, h'0001, h'0000, h'0000
	.word	h'0000, h'1001, h'1001, h'1fff
	.word	h'0000, h'0000, h'0003, h'0006
	.word	h'000c, h'0018, h'000c, h'0006
	.word	h'0003, h'0001, h'0001, h'0001
	.word	h'0001, h'0001, h'0001, h'0001
	.word	h'0000, h'0000, h'0000, h'001e
	.word	h'001b, h'0000, h'0000, h'0026
	.word	h'0049, h'0049, h'0051, h'0052
	.word	h'003f, h'0001, h'0200, h'03fe
	.word	h'0023, h'0041, h'0041, h'0063
	.word	h'003e, h'003e, h'0063, h'0041
	.word	h'0041, h'0063, h'0022, h'0000
	.word	h'003e, h'0063, h'0041, h'0041
	.word	h'0222, h'03ff, h'0001, h'003e
	.word	h'006b, h'0049, h'0049, h'0069
	.word	h'0039, h'0000, h'0041, h'0041
	.word	h'01ff, h'0241, h'0241, h'0100
	.word	h'0000, h'0002, h'00ef, h'01b5
	.word	h'0115, h'01b5, h'00e7, h'0102
	.word	h'0201, h'03ff, h'0021, h'0040
	.word	h'0041, h'003f, h'0001, h'0000
	.word	h'0041, h'0041, h'027f, h'0001
	.word	h'0001, h'0000, h'0000, h'0006
	.word	h'0003, h'0201, h'0203, h'13fe
	.word	h'0000, h'0201, h'03ff, h'0009
	.word	h'0058, h'0065, h'0043, h'0001
	.word	h'0000, h'0201, h'0201, h'03ff
	.word	h'0001, h'0001, h'0000, h'0041
	.word	h'007f, h'0041, h'003f, h'0041
	.word	h'003f, h'0001, h'0041, h'007f
	.word	h'0021, h'0040, h'0041, h'003f
	.word	h'0001, h'003e, h'0063, h'0041
	.word	h'0041, h'0063, h'003e, h'0000
	.word	h'0201, h'03ff, h'0111, h'0208
	.word	h'0208, h'0318, h'01f0, h'01f0
	.word	h'0318, h'0208, h'0208, h'0111
	.word	h'03ff, h'0001, h'0041, h'007f
	.word	h'0021, h'0040, h'0040, h'0060
	.word	h'0020, h'0033, h'004a, h'0049
	.word	h'0049, h'0029, h'0066, h'0000
	.word	h'0000, h'0040, h'0040, h'01fe
	.word	h'0041, h'0042, h'0000, h'0040
	.word	h'007e, h'0001, h'0001, h'0042
	.word	h'007f, h'0001, h'0040, h'0070
	.word	h'004c, h'0003, h'004c, h'0070
	.word	h'0040, h'0040, h'0078, h'0047
	.word	h'0018, h'0047, h'0078, h'0040
	.word	h'0041, h'0063, h'0055, h'0008
	.word	h'0055, h'0063, h'0041, h'0101
	.word	h'01c1, h'0132, h'000c, h'0130
	.word	h'01c0, h'0100, h'0063, h'0047
	.word	h'004d, h'0059, h'0071, h'0063
	.word	h'0000, h'0000, h'0000, h'0040
	.word	h'0fbe, h'1803, h'1001, h'0000
	.word	h'0000, h'0000, h'0000, h'0fff
	.word	h'0000, h'0000, h'0000, h'0000
	.word	h'1001, h'1803, h'0fbe, h'0040
	.word	h'0000, h'0000, h'0003, h'0006
	.word	h'0004, h'0007, h'0001, h'0003
	.word	h'0006, h'3f80, h'2080, h'20ff
	.word	h'20c9, h'1f49, h'0049, h'0041
	.word	h'3fff, h'3fff, h'3fff, h'3fff
	.word	h'3fff, h'3fff, h'3fff, h'7fff
	.word	h'7fff, h'7fff, h'7fff, h'7fff
	.word	h'7fff, h'7fff, h'7fff, h'0001
	.word	h'0001, h'0001, h'0001, h'0001
	.word	h'0001, h'0001, h'0001, 0
	.endseg
