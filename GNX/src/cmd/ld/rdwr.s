# rdwr.s: version 1.1 of 8/25/83
# 

# @(#)rdwr.s	1.1 (NSC) 8/25/83

#	Copyright (C) 1982 by National Semiconductor Corporation

#		National Semiconductor Corporation
#		2900 Semiconductor Drive
#		Santa Clara, California 95051

#	All Rights Reserved

#	This software is furnished under a license and may be used and copied only
#	in accordance with the terms of such license and with the inclusion of the
#	above copyright notice.  This software or any other copies thereof may not
#	be provided or otherwise made available to any other person.  No title to
#	or ownership of the software is hereby transferred.

#	The information in this software is subject to change without notice
#	and should not be construed as a commitment by National Semiconductor
#	Corporation.

#	National Semiconductor Corporation assumes no responsibility for the use
#	or reliability of its software on equipment configurations that are not
#	supported by National Semiconductor Corporation.

.set	std_cnt,0		# int
.set	std_ptr,4		# char *
.set	std_base,8		# char *
.set	std_flag,12		# short
.set	std_file,14		# char
 #
.set	ptr,4
.set	size,8
.set	count,0xc
.set	iop,0x10
.globl	_fastread
_fastread:
	.word	0x3e0		#
	movl	iop(ap),r6
	mull3	size(ap),count(ap),r9
	movl	ptr(ap),r8
0:	movl	r9,r7
	cmpl	(r6),r7		# cmp bytes-in-buffer, io-count
	bgeq	1f
	movl	(r6),r7
	beql	2f
1:	movc3	r7,*std_ptr(r6),(r8)	# load the bytes
	addl2	r7,std_ptr(r6)	# update the buffer pointer
	addl2	r7,r8		# update the I/O pointer
	subl2	r7,(r6)
	subl2	r7,r9
	bgtr	2f		# if count left, then we are not done
	movl	$1,r0
	ret			# we is done
2:	pushl	$1024
	pushl	std_base(r6)
	cvtbl	std_file(r6),-(sp)
	calls	$3,_read
	movl	std_base(r6),std_ptr(r6)
	movl	r0,(r6)
	bgtr	0b
	ret
#
.globl	_ftransfer
.set	newcount,4
.set	sfile,8
.set	dfile,12
_ftransfer:
	.word	0x1e0
	movl	sfile(ap),r7
	movl	dfile(ap),r8
0:	movl	newcount(ap),r6
	cmpl	(r7),r6		# is bytes-in-buffer > count
	bgtr	1f		# yes, otherwise...
	movl	(r7),r6		# ...take minimum
1:	cmpl	(r8),r6		# is free-bytes-left > count
	bgtr	1f		# yes, otherwise...
	movl	(r8),r6		# ...take minimum
1:	movc3	r6,*std_ptr(r7),*std_ptr(r8)# move it
	addl2	r6,std_ptr(r7)
	addl2	r6,std_ptr(r8)
	subl2	r6,(r7)
	subl2	r6,(r8)
	bgtr	1f
	pushl	r8
	calls	$1,_fflush
1:	subl2	r6,newcount(ap)
	bgtr	1f		# darn, not done
	ret
1:	tstl	(r7)		# bytes left in input?
	bgtr	0b
	pushl	$1024
	pushl	std_base(r7)
	cvtbl	std_file(r7),-(sp)
	calls	$3,_read
	movl	std_base(r7),std_ptr(r7)
	movl	r0,(r7)
	brw	0b
