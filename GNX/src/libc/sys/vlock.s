;
; vlock.S: version 1.1 of 9/3/82
; Mesa Unix C Library Source File
;
; @(#)vlock.S	1.1 (NSC) 9/3/82
;

; C library - vlock
; Lock/unlock a user page in memory.  Requires super-user privileges.
;	physaddr = vlock(func, useraddr);
; func/		function (0 = unlock page, 1 = lock page)
; useraddr/	user virtual address to lock or unlock
; physaddr/	physical location locked into, or -1 on errors

vlock: 	.equ	49	;System call table (sysent.c) index

	.import	cerror 

_vlock::
	addr	@vlock,r0
	addr	8(sp),r1
	svc
	bfc	noerror  
	cxp     cerror
noerror:
	rxp     0
