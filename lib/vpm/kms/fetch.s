/*	fetch.s 1.6 of 4/14/83
	@(#)fetch.s	1.6
*/

/*
 * VPM interpreter
 *
 * This version of the interpreter supports both BISYNC and HDLC.
 *
 * The desired protocol class is selected by the #define at line 1
 * of main.s
 *
 * Segment two
 *
 * This segment is entered from the main segment to fetch, decode,
 * and interpret the
 * VPM instruction pointed to by the simulated program counter (PC)
 */
	.org	2048
seg2:
/*
 * Jump tables go here
 */
#include	"opsw.s"
/*
 * Process state == READY:  Fetch next instruction
 */
fetch:
/*
 * If the hardware timer has expired then go to tickjmp2
 */
	mov	nprx,brg
	br4	tickjmp2
/*
 * Set mar = start + PC
 */
	mov	start,brg
	add	brg,r8,mar
	mov	%start,brg
	addc	brg,r9,%mar
/*
 * Set PC += 2
 */
	mov	2,brg
	add	brg,r8
	adc	r9
/*
 * Get the first byte of the instruction
 */
	mov	mem,brg|r1|mar++
/*
 * If the instruction is a jump or an instruction that
 * accesses an array element with a variable subscript,
 * then goto isjump
 */
	br7	isjump
/*
 * Validate the op code
 */
	mov	lastop-opsw,brg
	sub	brg,r1,-
	brc	illegal
/*
 * Calculate the address of the jump-table entry
 */
	mov	opsw,brg
	add	brg,r1
/*
 * Get the second byte of the instruction
 */
	mov	mem,r5|brg|mar
/*
 * Set the page register for a possible data reference
 */
	mov	r12,%mar
/*
 * Branch on the op code
 */
	br	(r1),%opsw
/*
 * Process an instruction in format one, or an
 * instruction that accesses an array element
 * specified by use of a variable index(subscript) 
 */
isjump:
/*
 * If the instruction accesses an array element
 * with a variable subscript, then goto dyn_arr_elt
 */
	asl	r1,brg
	br7	dyn_arr_elt
	mov	r1,brg
/*
 * Extract op code
 */
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	brg,r2
	mov	7,brg
	and	brg,r2
/*
 * Set r1 = high-order bits of jump address
 */
	mov	017,brg
	and	brg,r1
/*
 * Get the second byte of the instruction
 */
	mov	mem,r0
/*
 * Set the page register for a possible data reference
 */
	mov	r12,%mar
/*
 * Branch on op code
 */
	mov	jumpsw,brg
	br	(add,brg,r2),%jumpsw
/*
 * This code performs set up operations to permit access
 * to array elements with variable subscripts
 */
dyn_arr_elt:
/*
 * Mask off 2 high order bits of opcode
 */
	mov	077,brg
	and	brg,r1
/*
 * Validate the opcode by checking that it is a valid operation
 * to be performed with an array element with var. subscript
 */
	mov	last_arrop-opsw,brg
	sub	brg,r1,-
	brc	illegal
/*
 * Calculate the address of the jump-table entry
 */
	mov	opsw,brg
	add	brg,r1
/*
 * First set the page register for a possible data reference, then
 * get the second byte of the instruction and calculate the
 * correct offset into the array
 */
	mov	mem,r5
	mov	r12,%mar
	mov	r15,mar
	add	mem,r5,r5|brg|mar
/*
 * Branch on the opcode
 */
	br	(r1),%opsw
/*
 * LDA:  r15 = Y
 */
lda:
	mov	brg,r15
	br	fetch
/*
 * LDBYTE: MEM[r15] = Y
 *	 r15++
 */
ldbyte:
	mov	r15,mar
	mov	brg,mem
	inc	r15
	br	fetch
/*
 * MOV:  AC = MEM[Y]
 */
movd:
	mov	mem,r6
	br	fetch
/*
 * AND:  AC &= MEM[Y]
 */
andd:
	and	mem,r6
	br	fetch
/*
 * OR:  AC |= MEM[Y]
 */
ord:
	or	mem,r6
	br	fetch
/*
 * XOR:  AC ^= MEM[Y]
 */
xord:
	xor	mem,r6
	br	fetch
/*
 * BIC:  AC &= ~MEM[Y]
 */
bicd:
	mov	0,r0
	orn	mem,r0,brg
	and	brg,r6
	br	fetch
/*
 * ADD:  AC += MEM[Y]
 */
addd:
	add	mem,r6
	br	fetch
/*
 * SUB:  AC -= MEM[Y]
 */
subd:
	sub	mem,r6
	br	fetch
/*
 * LSH:  AC <<= MEM[Y]
 */
lshd:
	mov	mem,r5
	br	lshi
/*
 * RSH:  AC <<= MEM[Y]
 */
rshd:
	mov	mem,r5
	br	rshi
/*
 * TSTEQL:  AC = (AC==MEM[Y])
 */
tsteqld:
	addn	mem,r6
	brz	true
	br	false
/*
 * TSTNEQ:  AC = (AC!=MEM[Y])
 */
tstneqd:
	addn	mem,r6
	brz	false
	br	true
/*
 * TSTGTR:  AC = (AC>MEM[Y])
 */
tstgtrd:
	addn	mem,r6
	brc	true
	br	false
/*
 * TSTLSS:  AC = (AC<MEM[Y])
 */
tstlssd:
	sub	mem,r6
	brc	false
	br	true
/*
 * TSTGEQ:  AC = (AC>=MEM[Y])
 */
tstgeqd:
	sub	mem,r6
	brc	true
	br	false
/*
 * TSTLEQ:  AC = (AC<=MEM[Y])
 */
tstleqd:
	addn	mem,r6
	brc	false
	br	true
/*
 * MOVI:  AC = Y
 */
movi:
	mov	brg,r6
	br	fetch
/*
 * ANDI:  AC &= Y
 */
andi:
	and	brg,r6
	br	fetch
/*
 * ORI:  AC |= Y
 */
ori:
	or	brg,r6
	br	fetch
/*
 * XORI:  AC ^= Y
 */
xori:
	xor	brg,r6
	br	fetch
/*
 * BICI:  AC &= ~Y
 */
bici:
	mov	0,r0
	orn	brg,r0,brg
	and	brg,r6
	br	fetch
/*
 * ADDI:  AC += Y
 */
addi:
	add	brg,r6
	br	fetch
/*
 * SUBI:  AC -= Y
 */
subi:
	sub	brg,r6
	br	fetch
/*
 * LSHI:  AC <<= Y
 */
lshi:
	dec	r5
	brz	fetch
	asl	r6
	br	lshi
/*
 * RSHI:  AC <<= Y
 */
rshi:
	mov	r6,brg
1:
	dec	r5
	brz	1f
	mov	0,brg>>
	br	1b
1:
	mov	brg,r6
	br	fetch
/*
 * TSTEQLI:  AC = (AC==Y)
 */
tsteqli:
	addn	brg,r6
	brz	true
	br	false
/*
 * TSTNEQI:  AC = (AC!=Y)
 */
tstneqi:
	addn	brg,r6
	brz	false
	br	true
/*
 * TSTGTRI:  AC = (AC>Y)
 */
tstgtri:
	addn	brg,r6
	brc	true
	br	false
/*
 * TSTLSSI:  AC = (AC<Y)
 */
tstlssi:
	sub	brg,r6
	brc	false
	br	true
/*
 * TSTGEQI:  AC = (AC>=Y)
 */
tstgeqi:
	sub	brg,r6
	brc	true
	br	false
/*
 * TSTLEQI:  AC = (AC<=Y)
 */
tstleqi:
	addn	brg,r6
	brc	false
	br	true
/*
 * MOVM:  MEM[Y] = AC
 */
movm:
	mov	r6,mem
	br	fetch
/*
 * CLR:  MEM[Y] = 0
 */
clrm:
	mov	0,mem
	br	fetch
/*
 * ORM:  AC = MEM[Y] |= AC
 */
orm:
	or	mem,r6
	br	movm
/*
 * ANDM:  AC = MEM[Y] &= AC
 */
andm:
	and	mem,r6
	br	movm
/*
 * XORM:  AC = MEM[Y] ^= AC
 */
xorm:
	xor	mem,r6
	br	movm
/*
 * INC:  AC = ++MEM[Y]
 */
incm:
	mov	mem,r6
	inc	r6
	br	movm
/*
 * DEC:  AC = --MEM[Y]
 */
decm:
	mov	mem,r6
	dec	r6
	br	movm
/*
 * BICM:  AC = MEM[Y] &= ~AC
 */
bicm:
	mov	~0,brg
	xor	brg,r6		/* r6 = ~AC */
	and	mem,r6
	br	movm
/*
 * ADDM:  AC = MEM[Y] += AC
 */
addm:
	add	mem,r6
	br	movm
/*
 * SUBM:  AC = MEM[Y] -= AC
 */
subm:
	mov	r6,brg
	mov	mem,r6
	sub	brg,r6
	br	movm
/*
 * LSHM:  AC = MEM[Y] <<= AC
 */
lshm:
	mov	mem,r5
1:
	dec	r6
	brz	1f
	asl	r5
	br	1b
1:
	mov	r5,mem
	mov	mem,r6
	br	fetch
/*
 * RSHM:  AC = MEM[Y] >>= AC
 */
rshm:
	mov	mem,brg
1:
	dec	r6
	brz	1f
	mov	0,brg>>
	br	1b
1:
	mov	brg,mem
	mov	mem,r6
	br	fetch
/*
 * TESTOP:  Test if MEM[Y] has odd parity.
 */
testop:
	mov	mem,brg
	br	testopi
testopi:
/*
 * TESTOPI:  Set AC = 1 if MEM[Y] has odd parity, otherwise
 *		set AC = 0.
 */
	mov	brg,r6
	mov	0,brg>>
	xor	brg,r6,r6|brg
	mov	0,brg>>
	mov	0,brg>>
	xor	brg,r6,r6|brg
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	xor	brg,r6
	mov	1,brg
	and	brg,r6
	br	fetch
#ifdef	BISYNC
/*
 * CRCLOC:	Set crcsav = Y, then set MEM[Y] = MEM[Y+1] = 0.
 */
crcloc:
	mov	0,mem|mar++
	mov	0,mem
	mov	crcsav,mar
	mov	r5,mem
	br	fetch
/*
 * CRC16:  Combine the character in MEM[Y] with the CRC remainder
 *	   in MEM[crcsav], MEM[crcsav+1]
 */
crc16:
/*
 * Get the character
 */
	mov	mem,brg
/*
 * Merge with code for crc16i
 */
	br	crc16i
/*
 * CRC16I:  Combine Y with the CRC remainder
 *	   in MEM[CRC0], MEM[CRC0+1]
 */
crc16i:
/*
 * Note:  The following code calculates CRC-16
 *	 (x**0 + x**2 + x**15 + x**16).
 *
 *	 The algorithm processes one data byte in parallel.
 */
	mov	brg,r3
	mov	crcsav,mar
	mov	mem,brg|r5|mar
	dec	r5,-
	brz	crcerr
	xor	mem,r3
	mov	r3,mem
	asl	r3
	xor	mem,r3,r3|brg|mar++
	mov	brg,brg>>
	mov	brg,brg>>
	mov	3<<6,r0
	and	brg,r0
	xor	brg,r0,brg
	mov	brg,r4
	xor	brg,r3,brg
	mov	brg,r2
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	mov	0,brg>>
	xor	brg,r2
	mov	3,brg
	and	brg,r2,r2|brg
	mov	brg,brg>>
	mov	1,r1
	and	brg,r1
	mov	brg,brg>>
	xor	brg,r4
	xor	mem,r0,brg
	xor	brg,r1
	mov	r4,mem
	mov	r5,mar
	mov	r1,mem
	br	fetch
/*
 * RCV:  Set MEM[Y] = next received character; set AC = flags.
 *
 * If the RSILOFULL flag in flags2 is set
 *	reset RSILOFULL
 *	put contents of rsilo into memory addr. stored in ioadr
 *	branch to fetch
 * else
 *	save address passed in r5 into ioadr
 *	set virtual process state to INWAIT
 *	branch to deact (in this case the real work is done
 *			 following label 'inwait' in disp)
 */
rcv:
/*
 * If RSILOFULL is set then go to rcv2
 */
	mov	flags2,mar
	mov	mem,r0
	mov	~RSILOFULL,brg
	or	brg,r0,-
	brz	rcv2
/*
 * Put address to receive next byte into ioadr
 */
	mov	ioadr,mar
	mov	r5,mem
/*
 * Set process state = input wait
 */
	mov	state,mar
	mov	INWAIT,mem
/*
 * Go to deact
 */
	br	deact
rcv2:
/*
 * This code is executed if RSILOFULL is set
 * (i.e. if there is already a data byte waiting in rsilo)
 */
/*
 * Reset RSILOFULL
 */
	and	brg,r0,mem
/*
 * Move saved byte from rsilo to script data area
 */
	mov	rsilo,mar
	mov	mem,r0
	mov	r5,mar
	mov	r0,mem
/*
 * Go to fetch
 */
	br	fetch
/*
 * XMT:  Transmit MEM[Y]; set AC = flags.
 */
xmt:
	mov	mem,r5
	br	xmti
/*
 * XMTI:  Transmit Y; set AC = flags.
 *
 * If the XSILOFULL flag in flags2 is not set
 *	set XSILOFULL
 *	put data byte (passed in r5) into xsilo
 *	branch to fetch
 * else
 *	put data byte into ioaddr
 *	set process state to OUTWAIT
 *	branch to deact
 */
xmti:
/*
 * If XSILOFULL is set then go to xmti2
 */
	mov	flags2,mar
	mov	mem,r0
	mov	XSILOFULL,brg
	orn	brg,r0,-
	brz	xmti2
/*
 * This code executed if XSILOFULL is not set
 *  - first, set XSILOFULL.
 */
	or	brg,r0,mem
/*
 * Put byte into xmt silo
 */
	mov	xsilo,mar
	mov	r5,mem
/*
 * Go to fetch
 */
	br	fetch
xmti2:
/*
 * This code executed if XSILOFULL is set
 *  - first, put output byte into ioadr.
 */
	mov	ioadr,mar
	mov	r5,mem
/*
 * Set process state = output wait
 */
	mov	state,mar
	mov	OUTWAIT,mem
/*
 * Go to deact
 */
	br	deact
xsomi:
/*
 * This code is executed at the start of every bisync message transmission.
 * The code first determines if a transmitter shutdown is currently in
 * progress by examining the contents of lastbyte.
 * Operations performed:
 *	If the value stored in lastbytes == NIL (no shutdown in progress)
 *	   jump to execxsomi (where modem and line unit control performed)
 *	else (a shutdown is in progress)
 *	   set the virtual process state to XSOMWAITING
 *	   store the sync passed in r5 into ioadr
 *	   go to deact
 */
	mov	lastbytes,mar
	mov	mem,mem
	brz	1f
/*
 * The following code executed if a transmitter shutdown operation has
 * not yet completed (a previous shutdown operation has completed if
 * lastbytes == NIL):
 */
	mov	state,mar
	mov	XSOMWAITING,mem
	mov	ioadr,mar
	mov	r5,mem
	br	deact
/*
 * This code executed if no transmitter shutdown is in progress -
 *   jump to execxsomi
 */
1:
	SEGJMP(execxsomi);
/*
 * XEOMI:	Send Y, then shut down the transmitter.
 */
xeomi:
/*
 * This primitive signals the end of a message transmission. It does
 * this by loading, into the variable "lastbytes", the number of remaining
 * bytes that must be transmitted before shutting down the transmitter.
 * If XSILOFULL is set, then 2 is loaded into lastbytes; otherwise 1 is
 * loaded.
 */
	mov	flags2,mar
	mov	mem,r0
	mov	XSILOFULL,brg
	orn	brg,r0,-
	brz	1f
	mov	2,brg
	br	2f
1:
	mov	3,brg
2:
	mov	lastbytes,mar
	mov	brg,mem
/*
 * Merge with code for xmti
 */
	br	xmti
/*
 * RSOMI: the rsom primitive is invoked by a virtual process in order to
 *	  prompt the interpreter to start receiving a message. 
 *        The code first determines if a transmitter shutdown is
 *	  in progess.
 *	  If the value in lastbytes == NIL (shutdown not in progress)
 *	     then jump to execrsomi (where line unit control performed)
 *	  else (a transmitter shutdown is in progress)
 *	     set the virtual process's state to RSOMWAITING
 *	     move the sync character passed in r5 into ioadr
 *	     go to deact
 */
rsomi:
/*
 * Determine if the value stored in lastbytes == NIL
 */
	mov	lastbytes,mar
	mov	mem,mem
	brz	1f
/*
 * This code executed if a shutdown is in progress.
 */
	mov	state,mar
	mov	RSOMWAITING,mem
	mov	ioadr,mar
	mov	r5,mem
	br	deact
/* This code executed if a shutdown is not in progress -
 * - jump to execrsomi
 */
1:
	SEGJMP(execrsomi);
/*
 * GETXBUF: Open the transmit buffer.
 *	The code for this routine has been moved to report.s,
 *      therefore jump to xbufget in segment 3.
 */
getxbuf:
	SEGJMP(xbufget);
/*
 * GETRBUF: Open the receive buffer
 *	The code for this routine has been moved to report.s,
 *	therefore jump to rbufget in segment 3.
 */
getrbuf:
	SEGJMP(rbufget);
/*
 * GETBYTE:  Set MEM[Y] = next byte from transmit buffer;
 *	     set AC = flags.
 */
getbyte:
/*
 * If no current transmit buffer then error
 */
	mov	xmtbuf,mar
	mov	mem,r0
	brz	xbuferr2
/*
 * If there are no characters remaining then go to geterr1
 */
	mov	xmtnpc,mar
	mov	mem,r0|mar++
	mov	mem,r1
	mov	xmtsize,mar
	sub	mem,r0,mar++
	subc	mem,r1,-
	brc	geterr1
/*
 * Increment the count of fetched characters
 */
	mov	xmtnpc,mar
	inc	r0,mem|mar++
	adc	r1,mem
/*
 * If the count is odd then go to gettwo
 */
	mov	xmtnpc,mar
	mov	mem,brg
	br0	gettwo
/*
 * Copy the saved byte to the specified address
 */
	mov	xmtsave,mar
	mov	mem,r0
	mov	r5,mar
	mov	r0,mem
/*
 * Clear the AC, then go to fetch
 */
	br	false
/*
 * There is no saved byte from a previous fetch.  Therefore
 * we need to fetch two more characters from the host computer.
 */
gettwo:
/*
 * Wait for the previous unibus transfer (if any) to complete
 */
	BUSWAIT(buserr2)
/*
 * Set up a unibus request for the next two bytes
 */
	mov	xmtcp,mar
	mov	mem,ial|mar++
	mov	mem,iah|mar++
	mov	3,r3
	and	mem,r3
	asl	r3
	asl	r3
	mov	NRQ,brg
	or	brg,r3,brg
	mov	brg,npr
/*
 * Increment the buffer pointer
 */
	mov	xmtcp,mar
	mov	2,r2
	add	mem,r2,mem|mar++
	mov	mem,r0
	adc	r0,mem|mar++
	mov	mem,r0
	adc	r0,mem
/*
 * Wait for the unibus transfer to complete
 */
	BUSWAIT(buserr2)
/*
 * Save the high-order byte
 */
	mov	xmtsave,mar
	mov	idh,mem
/*
 * Store the low-order byte at the specified address
 */
	mov	r5,mar
	mov	idl,mem
/*
 * Clear the AC, then go to fetch
 */
	br	false
/*
 * No characters remaining
 */
geterr1:
	mov	NOMORE,brg
	mov	brg,r6
	br	fetch
/*
 * PUTBYTE:  Put MEM[Y] into the next available position
 *	     in the receive buffer
 */
putbyte:
	mov	mem,r5
	br	putbytei
/*
 * PUTBYTEI:  Put Y into the next available position
 *	      in the receive buffer
 */
putbytei:
/*
 * If no current receive buffer then error
 */
	mov	rcvbuf,mar
	mov	mem,r0
	brz	rbuferr2
/*
 * If there is no room for another character then go to puterr1
 */
	mov	rnrc,mar
	mov	mem,r0|mar++
	mov	mem,r1
	mov	rcvsize,mar
	sub	mem,r0,mar++
	subc	mem,r1,-
	brc	puterr1
/*
 * Increment the count of stored characters
 */
	mov	rnrc,mar
	inc	r0,mem|mar++
	adc	r1,mem
/*
 * If the count is odd then go to putlow
 */
	inc	r0,brg
	br0	putlow
/*
 * Wait for the previous unibus transfer (if any) to complete
 */
	BUSWAIT(buserr2)
/*
 * Set up a unibus request to move two bytes to the host computer
 */
	mov	rsave,mar
	mov	mem,odl
	mov	r5,brg
	mov	brg,odh
	mov	rcvcp,mar
	mov	mem,oal|mar++
	mov	mem,oah|mar++
	mov	3,r3
	and	mem,r3
	asl	r3
	asl	r3
	mov	VEC4,brg
	mov	nprx,r0
	and	brg,r0
	mov	r3,brg
	or	brg,r0,nprx
	mov	OUT|NRQ,brg
	mov	brg,npr
/*
 * Increment the buffer pointer
 */
	mov	rcvcp,mar
	mov	2,r2
	add	mem,r2,mem|mar++
	mov	mem,r0
	adc	r0,mem|mar++
	mov	mem,r0
	adc	r0,mem
/*
 * Clear the AC, then go to fetch
 */
	br	false
/*
 * Here we just save the specified byte for transfer later
 */
putlow:
/*
 * Copy the specified byte to the save area
 */
	mov	rsave,mar
	mov	r5,mem
/*
 * Clear the AC, then go to fetch
 */
	br	false
/*
 * No room for this character
 */
puterr1:
/*
 * Put error code in AC
 */
	mov	NOROOM,brg
	mov	brg,r6
/*
 * Go to fetch
 */
	br	fetch
/*
 * ATOE:	Translate MEM[Y] from ASCII to EBCDIC
 */
atoed:
	mov	mem,brg
	br	atoei
/*
 * ETOA:	Translate MEM[Y] from EBCDIC to ASCII
 */
etoad:
	mov	mem,brg
	br	etoai
/*
 * ATOEI:	Translate Y from ASCII to EBCDIC
 */
atoei:
	mov	brg,r6
	mov	atoetbl,brg
	add	brg,r6,mar
	mov	%atoetbl,%mar
	mov	mem,r6
	br	fetch
/*
 * ETOAI:	Translate Y from EBCDIC to ASCII
 */
etoai:
	mov	brg,r6
	mov	etoatbl,brg
	add	brg,r6,mar
	mov	%etoatbl,%mar
	mov	mem,r6
	br	fetch
/*
 * RTNXBUF:  Return a transmit buffer:
 *	     Set XTYPE = MEM[Y];
 *	     send a RTNXBUF signal to the driver
 */
rtnxbuf:
/*
 * Record the new values for the buffer parameters
 */
	mov	r5,mar
	mov	mem,r2|mar++
	mov	mem,r1|mar++
	mov	mem,r0
	mov	xbparam,mar
	mov	r2,mem|mar++
	mov	r1,mem|mar++
	mov	r0,mem
/*
 * If no current transmit buffer then error
 */
	mov	xmtbuf,mar
	mov	mem,r6
	brz	xbuferr2
/*
 * Indicate no current transmit buffer
 */
	mov	NIL,mem
/*
 * If this is not the buffer at the head of the
 * transmit-buffer-open queue then error
 */
	mov	xboq,mar
	addn	mem,r6,-
	brz	1f
	br	sequerr
1:
/*
 * If buffer state != 2 then error
 */
	mov	STATE,brg
	add	brg,r6,mar
	mov	mem,r0
	mov	2,brg
	addn	brg,r0,-
	brz	1f
	br	xbuferr2
1:
/*
 * Set buffer state = 3
 */
	mov	3,mem
/*
 * Remove the xbt entry at the head of the transmit-buffer-open
 * queue and attach it to the end of the transmit-buffer-return
 * queue
 */
	REMOVEQ(xboq,r5,sequerr)
	APPENDQ(xbrq,r5)
/*
 * Set up a unibus request to update the XTYPE parameter
 * in the host computer copy of the buffer descriptor.
 * This requires that 3 bytes be written to the host's
 * memory - it is accomplished by 2 npr's.  The first npr
 * writes 2 bytes, the second writes 1 byte.
 */
/*
 * First, wait for the previous unibus transfer (if any) to complete,
 * then update the first 2 bytes of the XTYPE parameter.
 */
	BUSWAIT(buserr2)
	mov	xbparam,mar
	mov	mem,odl|mar++
	mov	mem,odh
	mov	DESC,brg
	add	brg,r5,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	BDPARAM,brg
	add	brg,r0,brg
	mov	brg,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2,brg
	mov	3,r3
	and	brg,r3
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r3
	asl	r3,brg
	or	brg,r0,nprx
	mov	OUT|NRQ,brg
	mov	brg,npr
/*
 * Now write the third byte of the parameter to the host.
 * First, wait for the previous unibus transfer to complete.
 */
	BUSWAIT(buserr2)
	mov	xbparam+2,mar
	mov	mem,odh
	mov	mem,odl
	mov	DESC,brg
	add	brg,r5,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	BDPARAM+2,brg
	add	brg,r0,brg
	mov	brg,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2,brg
	mov	3,r3
	and	brg,r3
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r3
	asl	r3,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
/*
 * Schedule a report for this line
 */
	mov	0,brg
	br	rschd2a
/*
 * RTNRBUF:  Return a receive buffer:
 *	     Set RTYPE = MEM[Y];
 *	     send a RTNRBUF signal to the driver
 */
rtnrbuf:
/*
 * Record the new values for the buffer parameters
 */
	mov	r5,mar
	mov	mem,r2|mar++
	mov	mem,r1|mar++
	mov	mem,r0
	mov	rbparam,mar
	mov	r2,mem|mar++
	mov	r1,mem|mar++
	mov	r0,mem
/*
 * If no current receive buffer then error
 */
	mov	rcvbuf,mar
	mov	mem,r5
	brz	rbuferr2
/*
 * Indicate no current receive buffer
 */
	mov	NIL,mem
/*
 * If buffer state != 2 then error
 */
	mov	STATE,brg
	add	brg,r5,mar
	mov	mem,r0
	mov	2,brg
	addn	brg,r0,-
	brz	1f
	br	rbuferr2
1:
/*
 * Set buffer state = 5
 */
	mov	5,mem
/*
 * Get the count of stored characters
 * If the count is even then go to rtnrbuf2
 */
	mov	rnrc,mar
	mov	mem,brg
	br0	1f
	br	rtnrbuf2
1:
/*
 * Wait for the previous unibus transfer (if any) to complete
 */
	BUSWAIT(buserr2)
/*
 * Set up a unibus request to move one byte to the host computer
 */
	mov	rsave,mar
	mov	mem,odl
	mov	rcvcp,mar
	mov	mem,oal|mar++
	mov	mem,oah|mar++
	mov	3,r3
	and	mem,r3
	asl	r3
	asl	r3
	mov	VEC4,brg
	mov	nprx,r0
	and	brg,r0
	mov	r3,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
/*
 * Branch to here from above if count is even
 */
rtnrbuf2:
/*
 * Copy count to buffer descriptor
 *
 * Set up a unibus request to update the COUNT parameter
 * in the host computer copy of the buffer descriptor
 */
/*
 * First, wait for the previous unibus transfer (if any) to complete
 */
	BUSWAIT(buserr2)
	mov	rnrc,mar
	mov	mem,odl|mar++
	mov	mem,odh
	mov	DESC,brg
	add	brg,r5,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	4,brg
	add	brg,r0,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2,brg
	mov	3,r3
	and	brg,r3
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r3
	asl	r3,brg
	or	brg,r0,nprx
	mov	OUT|NRQ,brg
	mov	brg,npr
/*
 * Set up a unibus request to update the RTYPE parameter
 * in the host computer copy of the buffer descriptor.
 * Two npr's are performed -- the first moves 2 bytes to the host's
 *   memory, the second moves one byte.
 */
/*
 * First, wait for the previous unibus transfer (if any) to complete
 */
	BUSWAIT(buserr2)
	mov	rbparam,mar
	mov	mem,odl|mar++
	mov	mem,odh
	mov	DESC,brg
	add	brg,r5,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	BDPARAM,brg
	add	brg,r0,brg
	mov	brg,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2,brg
	mov	3,r3
	and	brg,r3
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r3
	asl	r3,brg
	or	brg,r0,nprx
	mov	OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the current unibus transfer to complete
 */
	BUSWAIT(buserr2)
/*
 * Ditto for the third parameter byte.
 */
	mov	rbparam+2,mar
	mov	mem,odh
	mov	mem,odl
	mov	DESC,brg
	add	brg,r5,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	BDPARAM+2,brg
	add	brg,r0,brg
	mov	brg,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2,brg
	mov	3,r3
	and	brg,r3
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r3
	asl	r3,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
/*
 * Append the rbt entry to the receive-buffer-return queue
 */
	APPENDQ(rbrq,r5)
/*
 * Schedule a report for this line
 */
	mov	0,brg
	br	rschd2a
#else
crc16:
xmt:
xmti:
rcv:
getbyte:
putbyte:
putbytei:
rtnrbuf:
rtnxbuf:
getxbuf:
getrbuf:
xeomi:
xsomi:
rsomi:
crc16i:
crcloc:
etoad:
atoed:
etoai:
atoei:
	br	illegal
#endif
#ifdef	HDLC
/*
 * getxfrm(s) - get the next available transmit frame and assign
 * it to sequence number <s>.
 *
 * If the sequence number <s> is out of range or does not have
 * a buffer attached then error.
 *
 * If a frame was obtained then return(1), otherwise return(0).
 */
getxfrm:
	mov	mem,brg
	br	getxfrmi
getxfrmi:
/*
 * Get the sequence number
 */
	mov	brg,r6
/*
 * If the sequence number is out of range then go to sequerr
 */
	mov	7,brg
	addn	brg,r6,-
	brc	sequerr
/*
 * If the sequence number is already in use then go to sequerr
 */
	mov	oxbt,brg
	add	brg,r6,mar
	mov	mem,r0
	brz	1f
	br	sequerr
1:
/*
 * If the transmit-buffer-ready queue is empty then go to true;
 * otherwise remove the xbt entry from the head of the queue
 */
	REMOVEQ(xbgq,r5,true)
/*
 * Set oxbt[s] = address of xbt entry
 */
	mov	oxbt,brg
	add	brg,r6,mar
	mov	r5,mem
/*
 * If STATE neq 1 then error
 */
	mov	STATE,brg
	add	brg,r5,mar
	mov	mem,r0
	mov	1,brg
	addn	brg,r0,-
	brz	1f
	br	xbuferr2
1:
/*
 * Set state = 2
 */
	mov	2,mem
/*
 * Attach the buffer to the end of the transmit-buffer-open
 * queue
 */
	APPENDQ(xboq,r5)
/*
 * Return(0)
 */
	br	false
/*
 * setctl(ac,n) - set control info for next transmit frame
 */
setctli:
/*
 * if n > 5 then error
 */
	mov	5,brg
	addn	brg,r6,-
	brc	xmterr
/*
 * Copy the control info from user space into scratch regs
 */
	mov	r5,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2|mar++
	mov	mem,r3|mar++
	mov	mem,r4
/*
 * If the transmitter is busy then error
 */
	mov	xmtstate,mar
	mov	mem,r5
	dec	r5,-
	brc	xmterr
/*
 * Save control info for use later
 */
	mov	xmtac,mar
	mov	r0,mem|mar++
	mov	r1,mem|mar++
	mov	r2,mem|mar++
	mov	r3,mem|mar++
	mov	r4,mem
/*
 * Save length of control info
 */
	mov	nxctl,mar
	mov	r6,mem
/*
 * return(0)
 */
	br	false
/*
 * xmtctl() - transmit the control information specified by
 *	      a previous <setctl> primitive
 */
xmtctl:
/*
 * If length == 0 then go to dispret2
 */
	mov	nxctl,mar
	mov	mem,r6
	dec	r6,-
	brz	xmterr
/*
 * If the transmitter is busy then error
 */
	mov	xmtstate,mar
	mov	mem,r0
	dec	r0,-
	brc	xmterr
/*
 * Set transmit state = 1
 */
	mov	1,mem
/*
 * Set flag count = 6
 */
	mov	flagcnt,mar
	mov	6,mem
/*
 * Initialize pointer to next control byte
 */
	mov	xctlptr,mar
	mov	0,mem
/*
 * Indicate no transmit buffer
 */
	mov	xmtbuf,mar
	mov	NIL,mem
/*
 * return(0)
 */
	br	false
/*
 * xmtfrm(s) - transmit the buffer which is currently assigned to
 * sequence number <s>, preceded by the control information specified
 * by the most recent <setctl> primitive.
 *
 * If the sequence number <s> is out of range or does not have
 * a buffer attached then error.
 *
 * If the transmitter is busy then return(1), otherwise return(0).
 */
xmtfrm:
	mov	mem,brg
	br	xmtfrmi
xmtfrmi:
/*
 * Get the sequence number
 */
	mov	brg,r6
/*
 * If the sequence number is out of range then go to sequerr
 */
	mov	7,brg
	addn	brg,r6,-
	brc	sequerr
/*
 * Set r6 = oxbt[s]; if no buffer is attached then go to sequerr
 */
	mov	oxbt,brg
	add	brg,r6,mar
	mov	mem,r6
	brz	sequerr
/*
 * If buffer state != 2 then error
 */
	mov	STATE,brg
	add	brg,r6,mar
	mov	mem,r0
	mov	2,brg
	addn	brg,r0,-
	brz	1f
	br	xbuferr2
1:
/*
 * If the transmit state is non-zero then return(1)
 */
	mov	xmtstate,mar
	mov	mem,r0
	dec	r0,-
	brc	true
/*
 * Set xmt state = 1
 */
	mov	1,mem
/*
 * Record the address of the xbt entry
 */
	mov	xmtbuf,mar
	mov	r6,mem
/*
 * Set flag count = 6
 */
	mov	flagcnt,mar
	mov	6,mem
/*
 * Initialize count of number of characters transmitted
 */
	mov	xmtnpc,mar
	mov	0,mem|mar++
	mov	0,mem
/*
 * Initialize pointer to next control byte to be transmitted
 */
	mov	xctlptr,mar
	mov	0,mem
/*
 * Get the descriptor address
 */
	mov	DESC,brg
	add	brg,r6,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
/*
 * Get buffer address, count, type, station, and device from buffer descriptor
 *
 * The following loop fetches the buffer descriptor from the host machine
 * (total of twelve bytes)
 */
	mov	BDLENGTH/2-1,brg
	mov	brg,r4
	mov	xmtdesc,mar
1:
	mov	r0,ial
	mov	r1,brg
	mov	brg,iah
	mov	r2,brg
	mov	3,r3
	and	brg,r3
	asl	r3
	asl	r3
	mov	NRQ,brg
	or	brg,r3,brg
	mov	brg,npr
	BUSWAIT(buserr2)
	mov	idl,mem|mar++
	mov	idh,mem|mar++
	mov	2,brg
	add	brg,r0
	adc	r1
	adc	r2
	dec	r4
	brc	1b
/*
 * Return(0)
 */
	br	false
/*
 * rtnxfrm(s) - return a transmit buffer
 */
rtnxfrm:
	mov	mem,brg
	br	rtnxfrmi
rtnxfrmi:
/*
 * Get the sequence number
 */
	mov	brg,r6
/*
 * If the sequence number is out of range then error
 */
	mov	7,brg
	addn	brg,r6,-
	brc	sequerr
/*
 * Set r6 = oxbt[s]; if no buffer is attached then error
 */
	mov	oxbt,brg
	add	brg,r6,mar
	mov	mem,r6
	brz	sequerr
	mov	NIL,mem
/*
 * If this is not the buffer at the head of the
 * transmit-buffer-open queue then error
 */
	mov	xboq,mar
	addn	mem,r6,-
	brz	1f
	br	sequerr
1:
/*
 * If buffer state != 2 then error
 */
	mov	STATE,brg
	add	brg,r6,mar
	mov	mem,r0
	mov	2,brg
	addn	brg,r0,-
	brz	1f
	br	xbuferr2
1:
/*
 * Set buffer state = 3
 */
	mov	3,mem
/*
 * Remove the xbt entry at the head of the transmit-buffer-open
 * queue and attach it to the end of the transmit-buffer-return
 * queue
 */
	REMOVEQ(xboq,r5,sequerr)
	APPENDQ(xbrq,r5)
/*
 * Schedule a RTNRBUF report
 */
	mov	0,brg
	br	rschd2a
/*
 * xmtbusy() - returns 1 if transmitter busy, otherwise returns 0
 */
xmtbusy:
/*
 * if the transmitter state is nonzero then return(1),
 * otherwise return(0)
 */
	mov	xmtstate,mar
	mov	mem,r6
	dec	r6,-
	brz	false
	br	true
/*
 * rsxmtq() - disassign (reset) all transmit sequence numbers
 *
 * Code for this primitive has been placed in segment 3 to
 * avoid overflowing instruction space in segment 1.
 */
rsxmtq:
	SEGJMP(rsxmtqc);
/*
 * abtxfrm() - abort the current transmission, if any.
 *
 * Code for this primitive has been placed in segment 3
 * to avoid overflowing instruction space in segment 1.
 */
abtxfrm:
	SEGJMP(abtxfrmc);
/*
 * rcvfrm(ac) - get a receive frame
 *
 * If there is already a current receive frame
 * then reset the current frame
 */
rcvfrm:
/*
 * Get the address of the rbt entry for the current receive frame.
 * If there is no current receive frame then go to rcvfrm1
 */
	mov	rcvbuf,mar
	mov	mem,r6
	brz	rcvfrm1
/*
 * Indicate no current receive frame
 */
	mov	NIL,mem
/*
 * Set buffer state = 1
 */
	mov	STATE,brg
	add	brg,r6,mar
	mov	1,mem
/*
 * Attach the current buffer to the end of the empty-receive-frame
 * queue
 */
	APPENDQ(erfq,r6)
rcvfrm1:
/*
 * Remove the rbt entry from the head of the completed-
 * receive-frame queue.
 * If the queue is empty then go to rcvfrm2
 */
	REMOVEQ(crfq,r6,rcvfrm2)
/*
 * Save entry address for use later
 */
	mov	rcvbuf,mar
	mov	r6,mem
/*
 * If buffer state neq 3 then error
 */
	mov	STATE,brg
	add	brg,r6,mar
	mov	mem,r0
	mov	3,brg
	addn	brg,r0,-
	brz	1f
	br	rbuferr2
1:
/*
 * Set buffer state = 4
 */
	mov	4,mem|mar++
/*
 * Copy control info into r0-r4
 */
	mov	RCTL,brg
	add	brg,r6,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2|mar++
	mov	mem,r3|mar++
	mov	mem,r4|mar++
/*
 * Get frame size (low byte in r6, high byte in r7)
 */
	mov	RCOUNT,brg
	add	brg,r6,mar
	mov	mem,r6|mar++
	mov	mem,r7|mar++
/*
 * Add length of control info + length of CRC
 */
	mov	4,brg
	add	brg,r6
	adc	r7
/*
 * If frame size > 15 then set AC = 15
 */
	mov	15,brg
	sub 	brg,r6,-
	mov	0,brg
	subc	brg,r7,-
	brc	1f
	br	2f
1:
	mov	15,brg
	mov	brg,r6
2:
/*
 * Copy control info into user space
 */
	mov	r5,mar
	mov	r0,mem|mar++
	mov	r1,mem|mar++
	mov	r2,mem|mar++
	mov	r3,mem|mar++
	mov	r4,mem
/*
 * Go to dispret2
 */
	br	dispret2
/*
 * The completed-receive-frame queue is empty--check for a control frame
 * in the staging area
 */
rcvfrm2:
	mov	flags,mar
	mov	mem,r0
	mov	~HAVCTL,brg
	or	brg,r0,-
	brz	1f
	br	false
1:
	and	brg,r0,mem
/*
 * If X25FLAG is set then set rnrc += nrctl-2
 */
	mov	rnrc,mar
	mov	mem,r0|mar++
	mov	mem,r1
	mov	flags,mar
	mov	mem,r2
	mov	X25FLAG,brg
	orn	brg,r2,-
	brz	1f
	br	2f
1:
	mov	nrctl,mar
	add	mem,r0
	adc	r1
	mov	2,brg
	sub	brg,r0
	mov	0,brg
	subc	brg,r1
	mov	rnrc,mar
	mov	r0,mem|mar++
	mov	r1,mem
2:
/*
 * Copy byte count to r6 and r7
 */
	mov	rnrc,mar
	mov	mem,r6|mar++
	mov	mem,r7
/*
 * Add length of control info + length of CRC
 */
	mov	4,brg
	add	brg,r6
	adc	r7
/*
 * If frame size > 15 then set AC = 15
 */
	mov	15,brg
	sub 	brg,r6,-
	mov	0,brg
	subc	brg,r7,-
	brc	1f
	br	2f
1:
	mov	15,brg
	mov	brg,r6
2:
/*
 * Set flag to indicate no buffer attached
 */
	mov	NOBUF,brg
	or	brg,r6
/*
 * Copy control info to scratch regs
 */
	mov	rcvac,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2|mar++
	mov	mem,r3|mar++
	mov	mem,r4
/*
 * Copy control info into user space
 */
	mov	r5,mar
	mov	r0,mem|mar++
	mov	r1,mem|mar++
	mov	r2,mem|mar++
	mov	r3,mem|mar++
	mov	r4,mem
/*
 * Go to dispret2
 */
	br	dispret2
/*
 * rsrfrm() - reset (release) the current receive frame
 *
 * The rbt entry is put back onto the list of empty receive
 * buffers
 */
rsrfrm:
/*
 * Get the address of the rbt entry for the current receive frame.
 * If there is no current receive frame then return
 * to the dispatcher.
 */
	mov	rcvbuf,mar
	mov	mem,r5
	brz	dispret2
/*
 * Indicate no buffer allocated
 */
	mov	NIL,mem
/*
 * Set buffer state = 1
 */
	mov	STATE,brg
	add	brg,r5,mar
	mov	1,mem
/*
 * Attach the current buffer to the end of the empty-receive-frame queue
 */
	APPENDQ(erfq,r5)
/*
 * Go to dispret2
 */
	br	dispret2
/*
 * rtnrfrm() - return the current receive buffer
 */
rtnrfrm:
/*
 * If no current receive buffer then error
 */
	mov	rcvbuf,mar
	mov	mem,r5
	brz	rbuferr2
/*
 * Indicate no current receive buffer
 */
	mov	NIL,mem
/*
 * If buffer state != 4 then error
 */
	mov	STATE,brg
	add	brg,r5,mar
	mov	mem,r0
	mov	4,brg
	addn	brg,r0,-
	brz	1f
	br	rbuferr2
1:
/*
 * Set state = 5
 */
	mov	5,mem
/*
 * Copy count to buffer descriptor
 *
 * Set up a unibus request to update the COUNT parameter
 * in the host computer copy of the buffer descriptor
 */
	mov	RCOUNT,brg
	add	brg,r5,mar
	mov	mem,odl|mar++
	mov	mem,odh
	mov	DESC,brg
	add	brg,r5,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	BDSIZE,brg
	add	brg,r0,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2,brg
	mov	3,r3
	and	mem,r3
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r3
	asl	r3,brg
	or	brg,r0,nprx
	mov	OUT|NRQ,brg
	mov	brg,npr
/*
 * Wait for the unibus transfer to complete
 */
	BUSWAIT(buserr2)
/*
 * If X25FLAG is set then copy three-byte packet header to buffer descriptor
 */
	mov	flags,mar
	mov	mem,r1
	mov	X25FLAG,brg
	orn	brg,r1,-
	brz	1f
	br	2f
1:
	mov	RCTL+2,brg
	add	brg,r5,mar
	mov	mem,odl|mar++
	mov	mem,odh
	mov	DESC,brg
	add	brg,r5,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	BDPARAM,brg
	add	brg,r0,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2,brg
	mov	3,r3
	and	brg,r3
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r3
	asl	r3,brg
	or	brg,r0,nprx
	mov	OUT|NRQ,brg
	mov	brg,npr
	BUSWAIT(buserr2)
	mov	RCTL+4,brg
	add	brg,r5,mar
	mov	mem,odl
	mov	DESC,brg
	add	brg,r5,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2
	mov	BDPARAM+2,brg
	add	brg,r0,oal
	adc	r1,brg
	mov	brg,oah
	adc	r2,brg
	mov	3,r3
	and	brg,r3
	mov	nprx,r0
	mov	VEC4,brg
	and	brg,r0
	asl	r3
	asl	r3,brg
	or	brg,r0,nprx
	mov	BYTE|OUT|NRQ,brg
	mov	brg,npr
	BUSWAIT(buserr2)
2:
/*
 * Append the rbt entry to the receive-buffer-return queue
 */
	APPENDQ(rbrq,r5)
/*
 * Schedule a RTNRBUF report
 */
	mov	0,brg
	br	rschd2a
/*
 * norbuf() - returns 1 (true) if the queue of empty
 * receive buffers is currently empty; otherwise returns 0 (false).
 */
norbuf:
	mov	erfq,mar
	mov	mem,-
	brz	true
	br	false
#else
rcvfrm:
rsrfrm:
rtnrfrm:
xmtbusy:
setctli:
xmtctl:
getxfrm:
getxfrmi:
xmtfrm:
xmtfrmi:
rtnxfrm:
rtnxfrmi:
rsxmtq:
abtxfrm:
norbuf:
	br	illegal
#endif
/*
 * getopt() - get interpreter options
 */
getopt:
	mov	vpmopts+1,mar
	mov	mem,r6
	br	fetch
/*
 * DSRWAIT:  Wait for modem-ready
 *
 * Code for this primitive has been placed in segment 3 to
 * avoid overflowing instruction space in segment 1.
 */
dsrwait:
	SEGJMP(dsrwaitc);
/*
 * EXIT:  Terminate execution and send an EXIT signal
 *	  to the host computer with MEM[Y] as a parameter.
 */
exit:
	mov	mem,r5
	br	exiti
/*
 * EXITI:  Terminate execution and send an EXIT signal
 *	   to the host computer with Y as a parameter.
 */
exiti:
	mov	errcode,mar
	mov	r5,mem
/*
	mov	0123,brg
	br	crash2
*/
/*
 * Load the reason for termination
 */
	mov	EXITINST,brg
/*
 * Go to errterm2
 */
	br	errterm2
/*
 * TRACE1:  Send a TRACE signal to the driver
 *	    with MEM[Y] as a parameter.
 */
trace1:
	mov	mem,r5
	br	trace1i
/*
 * TRACE1I:  Send a TRACE report to the driver
 *	    with Y as a parameter.
 */
trace1i:
	mov	traceval,mar
	mov	r5,mem|mar++
	mov	0,mem
	mov	TRACEOUT,brg
	br	rschd2a
/*
 * TRACE2:  Send a TRACE report to the driver
 *	    with MEM[Y] and AC as parameters.
 */
trace2:
	mov	mem,r5
	br	trace2i
/*
 * TRACE2I:  Send a TRACE report to the driver
 *	    with Y and AC as parameters.
 */
trace2i:
	mov	traceval,mar
	mov	r5,mem|mar++
	mov	r6,mem
	mov	TRACEOUT,brg
	br	rschd2a
/*
 * SNAP:  Send a SNAP report to the driver with MEM[Y],...,MEM[Y+3]
 * as parameters
 */
snap:
	mov	traceval,mar
	mov	r5,mem
	mov	SNAPOUT,brg
	br	rschd2a
/*
 * GETCMD:  If the driver has passed a command to the script, but
 * the script has not yet obtained it, copy the command parameters
 * into MEM[Y],...,MEM[Y+3] and return(1); otherwise return(0)
 */
getcmd:
	mov	4-1,brg
	add	brg,r5,-
	brc	arrayerr
	mov	flags,mar
	mov	mem,r0
	mov	~HAVCMD,brg
	or	brg,r0,-
	brz	1f
	br	false
1:
	and	brg,r0,mem
	mov	scrcmd,mar
	mov	mem,r0|mar++
	mov	mem,r1|mar++
	mov	mem,r2|mar++
	mov	mem,r3
	mov	r5,mar
	mov	r0,mem|mar++
	mov	r1,mem|mar++
	mov	r2,mem|mar++
	mov	r3,mem
	mov	1,brg
	mov	brg,r6
	mov	CMDACK,brg
	br	rschd2b
/*
 * RTNRPT:  Return a script report to the driver
 */
rtnrpt:
	mov	traceval,mar
	mov	r5,mem
	mov	RPTOUT,brg
	br	rschd2a
/*
 * PAUSE:
 */
pause:
/*
 * If the SAM bit is set in the page for this line then
 * clear the SAM bit and return(0)
 */
	mov	flags,mar
	mov	mem,r0
	mov	~SAM,brg
	or	brg,r0,-
	brz	1f
	br	2f
1:
	and	brg,r0,mem
	br	false
2:
/*
 * Set AC = 1
 */
	mov	1,brg
	mov	brg,r6
/*
 * Set state = PAUSING
 */
	mov	state,mar
	mov	PAUSING,mem
/*
 * Go to deact
 */
	br	deact
/*
 * timer(n) - if n>0 then reset the timer to expire in n*100
 *	      milliseconds and return(1); else if the timer has not
 *	      expired then return the current value of the timer;
 *	      else return(0).
 */
timerd:
	mov	mem,r5
timeri:
	mov	tcount,mar
	dec	r5,-
	brc	1f
	mov	mem,r6
	brz	false
	inc	r6
	br	fetch
1:
	dec	r5,mem
	br	true
/*
 * TIMEOUT:   If(Y==0) then cancel any previous time-out request;
 *	     else{
 *	     save stack pointer and frame pointer;
 *	     set TPC = address of next instruction;
 *	     set timer = Y;
 *	     set AC = 0;
 *	     }
 *	     When the timer expires, restore the saved
 *	     stack pointer and frame pointer, set AC = 1,
 *	     and set PC = TPC.
 *	     A return from the stack frame which was active at
 *	     the time the time-out request was made
 *	     will cancel the time-out request.
 *
 * Note:  The timeout parameter passed via the AC is the number
 *	  of 100-millisecond increments until the timeout occurs.
 */
timeout:
	mov	mem,r5
timeouti:
/*
 * If Y == 0 then go to cancel
 */
	dec	r5,-
	brz	cancel
/*
 * Point to the save area
 */
	mov	toutsave,mar
/*
 * Save the stack pointer and the frame pointer
 */
	mov	r10,mem|mar++
	mov	r11,mem|mar++
/*
 * Save the address of the next instruction
 */
	mov	r8,mem|mar++
	mov	r9,mem|mar++
/*
 * Put the timeout interval in clock1
 */
	mov	clock1,mar
	dec	r5,mem
/*
 * Clear the AC, then go to fetch
 */
	br	false
/*
 * Cancel the current timeout request, if any
 */
cancel:
	mov	clock1,mar
	mov	NIL,mem
/*
 * Clear the AC, then go to fetch
 */
	br	false
/*
 * CALL:  DPUSH(PC); JMP X
 */
pushj:
/*
 * If this will cause a stack overflow then error
 */
	mov	edata+2,brg
	sub	brg,r10,-
	brc	1f
	br	stackerr
1:
/*
 * Push PC
 */
	mov	2,brg
	sub	brg,r10,r10|mar
	mov	r8,mem|mar++
	mov	r9,mem
/*
 * Go to jump
 */
	br	jump
/*
 * RETURN:  DPOP(PC); AC = MEM[Y]
 */
popj:
/*
 * Put value to be returned in AC
 */
	mov	mem,brg
	br	popji
/*
 * RETURN:  DPOP(PC); AC = MEM[Y]
 */
popji:
/*
 * Put value to be returned in AC
 */
	mov	brg,r6
/*
 * If there is a stack underflow then error
 */
	mov	SSTACK-2,brg
	addn	brg,r10,-
	brc	stackerr
/*
 * Pop saved PC
 */
	mov	r10,mar
	mov	mem,r0|mar++
	mov	mem,r1
	mov	2,brg
	add	brg,r10
/*
 * If there is a timeout in progress in the routine
 * we are returning from then cancel the timeout 
 */
	mov	clock1,mar
	mov	mem,-
	brz	2f
	mov	toutsave,mar
	addn	mem,r10,-
	brc	1f
	br	2f
1:
	mov	clock1,mar
	mov	NIL,mem
2:
/*
 * Go to jump
 */
	br	jump
/*
 * JMPNEZ:  if(AC!=0)PC = X
 */
jumpnez:
	dec	r6,-
	brc	jump
	br	fetch
/*
 * JMPEQZ:  if(AC==0)PC = X
 */
jumpeqz:
	dec	r6,-
	brz	jump
	br	fetch
/*
 * JMP:  Set PC = X
 *
 * Entered with r0 = bits 7-0 of the jump address,
 * r1 = bits 11-8 of the jump address.
 */
jump:
/*
 * If the jump address is not even then error
 */
	mov	r0,brg
	br0	jumperr
/*
 * Set PC = jump address
 * (low-order bits in r8, page bits in r9)
 */
	mov	brg,r8
	mov	017,brg
	and	brg,r1,brg
	mov	brg,r9
/*
 * Go to fetch
 */
	br	fetch
/*
 * Set AC = TRUE
 */
true:
	mov	1,brg
	mov	brg,r6
	br	fetch
/*
 * Set AC = FALSE
 */
false:
	mov	0,brg
	mov	brg,r6
	br	fetch
/*
 * Schedule a report (flag in first byte)
 */
rschd2a:
	mov	brg,r0
	SCALL(rptschd);
	br 	dispret2
/*
 * Schedule a report (flag in second byte)
 */
rschd2b:
	mov	brg,r0
	SCALL(rptschdb);
	br	dispret2
/*
 * Undefined op code
 */
illegal:
	mov	ILLEGAL,brg
	br	errterm2
/*
 * PC value out of bounds
 */
fetcherr:
	mov	FETCHERR,brg
	br	errterm2
/*
 * Jump address is not even
 */
jumperr:
	mov	JMPERR,brg
	br	errterm2
/*
 * Stack overflow or underflow
 */
stackerr:
	mov	STACKERR,brg
	br	errterm2
/*
 * Invalid transmit sequence number (out of range or
 * no buffer attached)
 */
sequerr:
	mov	SEQUERR,brg
	br	errterm2
/*
 * setctl with transmit state non-zero
 * or user array too close to the end of user data space
 */
setxerr:
	mov	SETXERR,brg
	br	errterm2
/*
 * User array too close to the end of user data space
 */
arrayerr:
	mov	ARRAYERR,brg
	br	errterm2
/*
 * Transmit-buffer state error
 */
xbuferr2:
	mov	XBUFERR1,brg
	br	errterm2
/*
 * Receive-buffer state error
 */
rbuferr2:
	mov	RBUFERR1,brg
	br	errterm2
/*
 * Transmit error (e. g. attempt to transmit when transmitter
 * is already busy)
 */
xmterr:
	mov	XMTERR,brg
	br	errterm2
/*
 * UNIBUS error
 */
buserr2:
#ifdef BISYNC
	mov	~NEM,brg
	mov	nprx,r0
	and	brg,r0,nprx
#endif
	mov	BUSERR,brg
	br	errterm2
/*
 * Error loop for debugging
 */
crcerr:
	mov	CRCERR,brg
	br	errterm2
/*
 * Error stop for debugging
 *	this code is commented out because it is
 * 	no longer access from any code in fetch.s
hang2:
	mov	brg,r15
	mov	HANG,brg
	br	errterm2
*/
/*
 * The following code deactivates a line
 */
deact:
/*
 * If r12 does not contain a valid page number then go to
 * deacterr
 */
	LINECHK(deacterr);
/*
 * If the line is currently active then set actline = NIL and save
 * the virtual-machine registers
 */
	mov	actline,mar
	mov	%actline,%mar
	addn	mem,r12,-
	brz	1f
	br	deact2
1:
	mov	NIL,mem
	mov	r12,%mar
	mov	psave,mar
	mov	r6,mem|mar++
	mov	r7,mem|mar++
	mov	r8,mem|mar++
	mov	r9,mem|mar++
	mov	r10,mem|mar++
	mov	r11,mem|mar++
deact2:
	br	dispret2
/*
 * Deact called with invalid page number in r12
 */
deacterr:
	mov	0131,brg
	br	crash2
/*
 * Bridge to error-termination code in segment one
 */
errterm2:
	mov	brg,r0
	SEGJMP(errterm);
/*
 * Bridge to global-error-termination code in segment one
 */
crash2:
	mov	brg,r15
	SEGJMP(crash);
/*
 * Bridge to dispatcher loop in segment one
 */
dispret2:
	SEGJMP(disp);
/*
 * Bridge to timer-expiration sequence in segment zero
 */
tickjmp2:
	SEGJMP(tickc);
/*
 * end of segment two
 */
endseg2:
