
        .byte '@(#)ptime.s	3.31    9/20/83',0
	;@(#) Copyright (c) 1983 by National Semiconductor Corp.

	.routine _times
_ptime::
	enter   16
	addr	-16(fp),tos
	cxp	_times	
	adjspb	-4
	movd	-16(fp),0(12(fp))
	movd	-12(fp),0(16(fp))
	exit    
	rxp     8
