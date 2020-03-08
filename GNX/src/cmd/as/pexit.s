
        .byte '@(#)pexit.s	3.31    9/20/83',0
	;@(#) Copyright (c) 1983 by National Semiconductor Corp.

_pexit1::	enter 0
		movqd	1,tos
		cxp	_exit
		exit
		rxp 0
_pexit2::	enter 0
		movqd	2,tos
		cxp	_exit
		exit
		rxp 0
	.import _exit
