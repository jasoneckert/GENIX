
        .byte '@(#)fnew.s	3.31    9/20/83',0
	;@(#) Copyright (c) 1983 by National Semiconductor Corp.
  
	.import _new.def
_fnew::
	enter   0
	movd    12(fp),tos
	cxp     _new.def
	exit    
	rxp     4
