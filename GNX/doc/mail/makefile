#
# makefile: version 1.1 of 12/29/82
# 
#
# @(#)makefile	1.1 (NSC) 12/29/82
#

vpr:
	/bin/csh /usr/ucb/vtroff -me mail?.nr

lpr:
	nroff -me -Tlpr mail?.nr | lpr

typeset:
	/usr/ucb/nettroff -me mail?.nr
	/usr/ucb/nettroff -me addenda.nr
