;
; blt.s: version 3.5 of 3/1/83
; Mesa Unix Graphics Board Primitives
;
; @(#)blt.s	3.5 (NSC) 3/1/83
;

# if NBM > 0
# include "../h/devvm.h"

		.program
		.static

_vram::		.double	GB_VideoRAM		; video ram start
wram::		.double	GB_WindowRAM		; window ram start
crtcrs::	.double GB_CRTC_Addr		; crtc register select
crtcon::	.double GB_Control		; crt control register
tone::		.double GB_Tone			; tone generator
xmax:		.equ	608			; screen width in bits
datarg:		.equ	2			; crtc data register
;;
;;	The bits of the crt command register
;;
o_scrollon:	.equ	0
scrollon:	.equ	1 shl o_scrollon ;start scrolling operation
o_crton:	.equ	1
crton:		.equ	1 shl o_crton	; turn the display on/off
o_csron:	.equ	2
csron:		.equ	1 shl o_csron	; turn the hardware cursor on/off
o_reverse:	.equ	3
reverse:	.equ	1 shl o_reverse	; screen background
o_pag:		.equ	4
pag:		.equ	1 shl o_pag	; currently active display page
o_ad1:		.equ	5
ad1:		.equ	1 shl o_ad1	; A/D input select 1
o_ad0:		.equ	6
ad0:		.equ	1 shl o_ad0	; A/D input select 0
o_brdon:	.equ	7
brdon:		.equ	1 shl o_brdon	; SDLC subsystem master enable
o_cbinmap:	.equ	8
cbinmap:	.equ	1 shl o_cbinmap	; video memory addressing mode (command)
;;
;;	The bits of the crt status register
;;
o_busy:		.equ	0
busy:		.equ	1 shl o_busy	; scrolling in progress
o_sdisen:	.equ	1
sdisen:		.equ	1 shl o_sdisen	; scan line currently active
o_svsync:	.equ	2
svsync:		.equ	1 shl o_svsync	; vertical sync signal
o_shsync:	.equ	3
shsync:		.equ	1 shl o_shsync	; horizontal sync signal
;o_pag:		(same as above)
;pag:		(same as above)		; currently active display page
o_sbinmap:	.equ	5
sbinmap:	.equ	1 shl o_sbinmap	; video memory addressing mode (status)

;;
;;	Local copy of the command register
;;
_crtcom::	.word	crton+reverse

;;
;;	The bits of the window controller ram
;;
masked:		.equ	1 shl 7			; This block is masked
scrollable:	.equ	1 shl 6			; This block is scrollable
linked:		.equ	1 shl 5			; This block is linked
up:		.equ	1 shl 4			; Scroll direction is up
;;
;; Bits 0 thru 3 indicate amount of scroll....
;;
;;	The window controller ram initialization data
;;
winit:		.equ	scrollable+linked+up
wdata::		.double	(winit shl 16)+winit
		.endseg


;;
;;	Initialize the graphics board ram and registers.
;;	The two arguments specify respectively:
;;		a pattern to fill the display with, and
;;		normal/reverse video (0 <==> normal video).
;;
_ginit::
fillpattern:	.equ	12(fp)
screenrvid:	.equ	16(fp)
	enter	[r3,r4,r5,r6,r7],0
;;
;;	First initialize the crt controller internal registers.  The
;;	crtc register to be initialized must first be selected by
;;	writing the register number to the crtc register select register,
;;	followed by the initialization data.  The registers are initialized
;;	in decreasing order to use the register number as the loop index
;;	for the acbb instruction.  Since this instruction fails when the
;;	loop index goes to 0, register 0 is initialized outside the loop...
;;
	movqd	0,r0
	movb	regcount,r0		; # of regs to initialize
	addr	datarg(crtcrs),r1	; crtc data reg address
crtcinit:
	movb	r0,0(crtcrs)		; Select a crtc register
	movb	regval[r0:b],0(r1) 	; Send init value
	acbb	-1,r0,crtcinit		; ... continue
	movb	r0,0(crtcrs)		; init register 0
	movb	regval,0(r1)

;;
;;	Now we initialize the window ram to a known state. The steps
;;	are, initialize the window blocks to viewable,scrollable,
;;	linked, and scroll direction up, then unlink the top row
;;	of the window rams so they will not wrap around on up scrolls.
;;	The window rams are initialized 2 blocks at a time...
;;
	movqb	-1,0(tone)		; Turn bell on

	movd	wdata,r1
	movd	wram,r4
	addr	@(16*128),r5		; Row increment
	movb	38,r3			; x loop counter
cols:
	movb	25,r0			; r0 <- y loop counter
	movd	r4,r2			; r2 <- column address
rows:
	movd	r1,0(r2)
	addqd	4,r2			; Next row...
	acbb	-1,r0,rows
	addd	r5,r4			; Next column...
	acbb	-1,r3,cols

	movb	38,r0
	addr	(2*49)(wram),r1		; Top left row address
unlink:
	movqb	0,0(r1)			; Unlink top row of wram
	addd	r5,r1
	acbb	-1,r0,unlink

	movqb	0,0(tone)		; Turn bell off

	movd	screenrvid,r7		; Initialize normal/reverse video
	bsr	setvideo

	movd	fillpattern,r7		; Fill screen with initial pattern
	bsr	fill

	movw	_crtcom,0(crtcon)	; Now turn the crt on

	exit	[r3,r4,r5,r6,r7]
	rxp	0

;;
;;	Set screen state to normal or reverse video.
;;	An argument value of zero means normal video;
;;	nonzero means reverse video.  The C entry point
;;	writes to the control register to force the new
;;	state to take immediate effect; the internal entry
;;	point does not.
;;
_setvideo::
	enter	[r7],0
	movd	12(fp),r7
	bsr	setvideo
	movw	_crtcom,0(crtcon)	; make the new state take effect
	exit	[r7]
	rxp	0

setvideo:
	cmpqd	0,r7
	beq	normalvid
	orw	reverse,_crtcom
	ret	0
normalvid:
	andw	com reverse,_crtcom
	ret	0

_scroll::				; scroll(amt)
	enter	[r2,r3],0
	movd	12(fp),r0		; amt to scroll by
	cmpqb	0,r0
	bge	noscroll
	movd	wram,r1			; beginning wram address
	movb	38,r2			; x loop counter
	addr	@(16*128),r3		; row increment
	inssb	r0,wdata,0,4		; stick amt to scroll by in wram
crtbusy:
	tbitw	o_busy,0(crtcon)	; panic if already busy
.static
msg:	.byte	"nested scroll"
.endseg
.import	_panic
	bfc	slp
	addr	msg,tos
	cxp	_panic
slp:
	movw	wdata,0(r1)		; set new scroll amt
	addd	r3,r1
	acbb	-1,r2,slp
	movqd	1,r0
	orw	_crtcom,r0
	movw	r0,0(crtcon)		; start the scroll
noscroll:				; and leave....
	exit	[r2,r3]
	rxp	0

;;
;;	C entry point to fill.  The argument gives a pattern
;;	to be replicated over the screen, rotated one bit position
;;	each column.
;;
_clearscreen::
	enter	[r2,r3,r4,r5,r6,r7],0
	movd	12(fp),r7
	bsr	fill
	exit	[r2,r3,r4,r5,r6,r7]
	rxp	0

fill:
				; r7 contains fill pattern
	addr	@128,r5
	movd	_vram,r3
	addr	@xmax,r6
fill2:
	addr	@24,r0
	movd	r3,r1
	addr	4(r1),r2
	movd	r7,0(r1)		; fill first location
	movsd				; S  P  R  E  A  D    I  T
	addd	r5,r3			; next scan line
	rotd	1,r7			; rotate the pattern
	acbw	-1,r6,fill2
	ret	0

;;
;;	Goose the tone generator.  The first argument gives duration
;;	(crudely) and the second amplitude and frequency.  This routine
;;	should be replaced by a version with more knowledge of Unix
;;	guts, that uses timeout or sleep to go away until it's time
;;	to turn off the tone.
;;
_beep::
duration:	.equ	12(fp)
amp_freq:	.equ	16(fp)
	enter	[r7],0
	movd	duration,r7
	movb	amp_freq,0(tone)	; Turn on the tone generator
toneloop:				; Count down duration -- busy wait
	acbd	-1,r7,toneloop
	movqb	0,0(tone)		; Turn off the tone generator
	exit	[r7]
	rxp	0

;;
;;	Convert an x,y into a bit pointer.  The algorithm is...
;;	bitpointer := convert(x*xinc+y,baseaddress)
;;
xy2addr::			; Convert an x,y to a bit offset
	movd	x(r7),r0	; get x coordinate
	muld	xinc(r7),r0	; * #bits/column
	addd	y(r7),r0	; + y coordinate
	movd	base(r7),r2	; r2 <- base addr addr
	cvtp	r0,0(r2),r0	; convert to a bit pointer
	ret	0


;;
;; The function table contains the opcodes for the logical operations
;; on the bits manipulated by blt and clearbox. The function code parameter
;; is used to index the function table to extract the opcode, and replace the
;; opcode in the self modifying routine to do the appropriate action.
;; ... Gross but effective ...
;;
;;	This table is used by _blt, _copybox, and _clearbox.
;;
funtab:
	.byte	x'd7		; movd opcode
	.byte	x'cb		; bicd opcode
	.byte	x'db		; ord  opcode
	.byte	x'eb		; andd opcode
	.byte	x'fb		; xord opcode

funbounds:	.byte	4,0	; bounds checking for the opcode

;;
;; Offsets for accessing the form record:
;;	They are used in all the routines that follow.
;;
x:	.equ	0
y:	.equ	4
bw:	.equ	8
bh:	.equ	12
base:	.equ	16
xinc:	.equ	20


;;
;; Perform a bit-blt operation, using op, to fold mask into dst.
;;
_blt::
op:	.equ	12(fp)		; argument offsets from fp
mask:	.equ	16(fp)
dest:	.equ	20(fp)

	enter	[r3,r4,r5,r6,r7],4
	cmpqd	0,bh(dest)
	bge	blt_done
	cmpqd	0,bw(dest)
	bge	blt_done		; exit if h or w <= 0
	checkb	r0,funbounds,op
	flag				; error out if op is out of bounds
	movb	funtab[r0:b],function	; set the operation...
	movd	mask,r7			; r7 <- ^mask form
	bsr	xy2addr
	movd	r0,r1			; r1 <- mask bit pointer
	movd	xinc(r7),r6		; r6 <- mask bit pointer increment
	movd	dest,r7			; r7 <- ^dest form
	bsr	xy2addr			; r0 <- dest bit pointer
	movd	bw(r7),r5		; r5 <- box width
	; beginning of super kludge......
	movb	bh(r7),r4		; r4 <- bh
	movb	r4,dlen			; modify dest length (in extd ins)
	movb	r4,slen			; modify src len
	movb	r4,idlen		; modify dest length (in insd ins)
	movqd	0,r2			; base address for bit field ins.
	movd	xinc(r7),r7		; dest bit pointer increment
	addr	blt,-4(fp)		; the blt routine
	jsr	0(-4(fp))		; execute blt routine
blt_done:
	exit	[r3,r4,r5,r6,r7]
	rxp	0

;;
;; The actual blt routine. It is in the static base in order
;; to be able to modify the length fields....
;;
		.static
blt:		.byte	x'2e,x'c3,x'50,x'00	; extd r0,0(r2),r3,31
 dlen:		.byte	x'1f
		.byte	x'2e,x'0b,x'51,x'00	; extd r1,0(r2),r4,31
 slen:		.byte	x'1f
 function:	.word	x'20fb			; xord r4,r3
		.byte	x'ae,x'83,x'1a,x'00	; insd r0,r3,0(r2),31
 idlen:		.byte	x'1f
		.word	x'3803			; addd r7,r0
		.word	x'3043			; addd r6,r1
		.byte	x'cd,x'2f,x'6b		; acbw -1,r5,blt
		.word	x'0012			; ret  0
		.endseg


;;
;; Fill the destination box with a background
;; and lay a character on top of it.
;;
;;	This routine is intended to render characters onto
;;	the bit mapped display as efficiently as possible.
;;
;;	(Its name stands for Character-Background BLT.)
;;
;;	No error or bounds checking is performed.  Make sure
;;	arguments are correct and nondegenerate before calling!
;;
_cb_blt::
cb_bop:		.equ	12(fp)	; background operation
cb_chrf:	.equ	16(fp)	; character form
cb_dstf:	.equ	20(fp)	; destination form

cb_chr_xinc:	.equ	-4(fp)	; == xinc(cb_chrf)
cb_rmargin:	.equ	-8(fp)	; width of right margin of overall box
cb_loop_addr:	.equ	-12(fp)	; address of the part of this routine
				; that lives in the static data area

cb_bkgd:	.equ	r7	; background bit pattern (0 or -1)
cb_tmp:		.equ	r6	; vertical patches built up here
cb_dst_xinc:	.equ	r5	; == xinc(dstf)
cb_chrp:	.equ	r4	; bit address of next patch of character
cb_dstp:	.equ	r3	; bit address of next patch of destination
cb_bias:	.equ	r2	; offset from bottom of dst patch to bottom
				; of chr patch
cb_col_ctr:	.equ	r1	; loop counter
cb_null:	.equ	r0	; 0 (needed as such for extd/insd instructions)

	enter	[r3,r4,r5,r6,r7],12

	; Patch lengths into the insd and extd instructions:
	movd	bh(cb_dstf),r0
	movb	r0,cb_i1
	movb	r0,cb_e
	movb	r0,cb_i2
	movb	r0,cb_i3

	;
	; Initialize various quantities:
	;	The address calculation with xy2addr must come first,
	;	because xy2addr uses r0, r2, and r7.
	;
	;	We can't use xy2addr to get cb_chrf, because its x and y
	;	fields are nonzero to record where the character goes.
	;	But to get a source bit address for the character, they
	;	must be zero.  Hence, we do the work by hand.
	;	
	movd	cb_dstf,r7
	bsr	xy2addr
	movd	r0,cb_dstp

	movqd	0,cb_null

	movd	base(cb_chrf),r7
	cvtp	cb_null,0(r7),cb_chrp

	movd	xinc(cb_dstf),cb_dst_xinc
	movd	xinc(cb_chrf),cb_chr_xinc

	movd	y(cb_chrf),cb_bias
	subd	y(cb_dstf),cb_bias

	; This routine can be called with two possible values for cb_bop:
	; 0 (i.e., mov) and 1 (i.e., bic).  If 0, the character rendition
	; is reverse video, and cb_bkgd must be -1 to fill the background;
	; otherwise rendition is forward video and the background must be null.
	movqd	0,cb_bkgd
	cmpqd	0,cb_bop
	bne	cb_getwidth
	movqd	-1,cb_bkgd

cb_getwidth::
	; Set quantities used for loop control.
	;	Cb_col_ctr gets width of left margin,
	;	and cb_rmargin gets width of right margin.
	movd	x(cb_chrf),cb_col_ctr
	subd	x(cb_dstf),cb_col_ctr
	movd	bw(cb_dstf),cb_rmargin	; get total margin
	subd	bw(cb_chrf),cb_rmargin
	subd	cb_col_ctr,cb_rmargin	; subtract out left margin

	; Go off and do the actual work:
	addr	cb_blt,cb_loop_addr
	jsr	0(cb_loop_addr)

	exit	[r3,r4,r5,r6,r7]
	rxp	0

;;
;;	The actual guts of the routine.  It's in the static data area
;;	because we have to be able to patch the insd and extd instructions.
;;
	.static
cb_blt::
					; Fill in border patches to left of character:
	.word	x'081f			; cmpqd	0,cb_col_ctr
	.word	x'0eda			; bge	cb_char
					; Set background:
	.word	x'3997			; movd	cb_bkgd,cb_tmp
cb_left:
	.byte	x'ae,x'1b,x'32,x'00	; insd	cb_dstp,cb_tmp,0(cb_null),31
cb_i1:	.byte	x'1f
	.word	x'28c3			; addd	cb_dst_xinc,cb_dstp
	.byte	x'cd,x'0f,x'79		; acbw	-1,cb_col_ctr,cb_left
cb_char:
					; Fill in patches for the char itself:
					; (We assume > 0 width.)
	.byte	x'57,x'80,x'10,x'08	; movd	bw(cb_chrf),cb_col_ctr
cb_body:
	.byte	x'2e,x'a3,x'41,x'00	; extd	cb_chrp,0(cb_null),cb_tmp,31
cb_e:	.byte	x'1f
					; Slide char up to proper position w/in overall box:
	.byte	x'4e,x'97,x'11		; lshd	cb_bias,cb_tmp
					; Overlay background:
	.word	x'39bb			; xord	cb_bkgd,cb_tmp
	.byte	x'ae,x'1b,x'32,x'00	; insd	cb_dstp,cb_tmp,0(cb_null),31
cb_i2:	.byte	x'1f
					; Iterate:
	.word	x'28c3			; addd	cb_dst_xinc,cb_dstp
	.byte	x'03,x'c1,x'7c		; addd	cb_chr_xinc,cb_chrp
	.byte	x'cd,x'0f,x'6c		; acbw	-1,cb_col_ctr,cb_body

					; Fill in right border patches:
	.byte	x'57,x'c0,x'78		; movd	cb_rmargin,cb_col_ctr
	.word	x'081f			; cmpqd	0,cb_col_ctr
	.word	x'0eda			; bge	cb_out
	.word	x'3997			; movd	cb_bkgd,cb_tmp
cb_right:
	.byte	x'ae,x'1b,x'32,x'00	; insd	cb_dstp,cb_tmp,0(cb_null),31
cb_i3:	.byte	x'1f
	.word	x'28c3			; addd	cb_dst_xinc,cb_dstp
	.byte	x'cd,x'0f,x'79		; acbw	-1,cb_col_ctr,cb_right
cb_out:
	.word	x'0012			; ret	0
	.endseg


;;
;; Fill the box specified by the cl_dst form with cl_pat,
;; using cl_op to lay it in place.
;;
;;	This routine has been generalized to eliminate the former
;;	height restriction of 25 pixels.
;;
;;	The routine currently does not rotate the pattern at all.
;;	Eventually it should.
;;
_clearbox::
cl_op:	.equ	12(fp)		; argument offsets from fp
cl_pat:	.equ	16(fp)
cl_dst:	.equ	20(fp)

cl_loop_addr:	.equ	-4(fp)	; inner loop address
cl_swath_base:	.equ	-8(fp)	; bit address of bottom left corner of current
				; swath -- used to reset cl_cur_addr when outer
				; loop is iterated

cl_col_inc:	.equ	r7	; inter-column increment
cl_cur_addr:	.equ	r6	; bit address of next patch to be cleared
cl_col_ctr:	.equ	r5	; counter for inner loop
cl_reg_pat:	.equ	r4	; a register copy of cl_pat
cl_scr:		.equ	r3	; temporary in inner loop
cl_swath_width:	.equ	r2	; used to reset inner loop counter
cl_height:	.equ	r1	; remaining height to deal with -- decremented
				; in outer loop
cl_null:	.equ	r0	; 0 (needed as such for extd/insd instrs)

	enter	[r3,r4,r5,r6,r7],8

	; Verify that there's work to do:
	cmpqd	0,bh(cl_dst)
	bge	cl_done
	cmpqd	0,bw(cl_dst)
	bge	cl_done

	; Check operation validity and enter it into the real clear routine:
	checkb	r0,funbounds,cl_op
	flag				; error out if op is out of bounds
	movb	funtab[r0:b],cl_fn	; set the operation...

	; Set up various variables, using r7 temporarily as a form pointer:
	movd	cl_dst,r7
	bsr	xy2addr			; sets r0, trashes r2, uses r7
	movd	r0,cl_swath_base
	movd	bh(r7),cl_height
	movd	bw(r7),cl_swath_width
	movd	xinc(r7),cl_col_inc	; must come last -- trashes r7

	movd	cl_pat,cl_reg_pat
	movqd	0,cl_null

cl_hor_band_loop:
	; Does the outer loop require another iteration?
	cmpqd	0,cl_height
	bge	cl_done

	;
	; Set up the inner loop:
	;
	movd	cl_swath_base,cl_cur_addr
	movd	cl_swath_width,cl_col_ctr
	; Fill in heights:
	cmpd	cl_height,25
	ble	cl_short_swath
	movb	25,cl_el
	movb	25,cl_il
	br	cl_loop_go
cl_short_swath:
	movb	cl_height,cl_el
	movb	cl_height,cl_il

cl_loop_go:
	addr	cl_vert_patch_loop,cl_loop_addr
	jsr	0(cl_loop_addr)

	; Iterate the outer loop:
	addd	25,cl_swath_base
	subd	25,cl_height
	br	cl_hor_band_loop

cl_done:
	exit	[r3,r4,r5,r6,r7]
	rxp	0

;;
;;	The actual box clearing routine.  As with the actual blt routine,
;;	it lives in the static area to allow length fields to be filled in.
;;
	.static
cl_vert_patch_loop::
	.byte	x'2e,x'f3,x'40,x'00	; extd	cl_cur_addr,0(cl_null),cl_scr,31
cl_el:	.byte	x'1f
cl_fn:	.byte	x'd7			; movd	cl_reg_pat,cl_scr
	.byte	x'20
	.byte	x'ae,x'33,x'1a,x'00	; insd	cl_cur_addr,cl_scr,0(cl_null),31
cl_il:	.byte	x'1f
					; Iterate the inner loop:
	.word	x'3983			; addd	cl_col_inc,cl_cur_addr
	.byte	x'cd,x'2f,x'72		; acbw	-1,cl_col_ctr,cl_vert_patch_loop
	.word	x'0012			; ret 0
	.endseg


;;
;; Copy the rectangle specified by src into the rectangle specified by dst.
;;	Choose copying directions so that the copy is faithful;
;;	i.e., so that the destination rectangle contains exactly
;;	the original contents of the source rectangle.
;;
;;	We assume that both the source and destination boxes are part
;;	of the same form (raster), i.e., that they have identical
;;	xinc and base fields.
;;
_copybox::
cp_src_argp:	.equ	12(fp)		; these are form pointers
cp_dst_argp:	.equ	16(fp)

form_sz:	.equ	24		; in bytes
cp_src:		.equ	-(form_sz)(fp)
cp_dst:		.equ	-(2*form_sz)(fp)
cp_srcp:	.equ	-(2*form_sz+4)(fp)	; src starting bit addr
cp_dstp:	.equ	-(2*form_sz+8)(fp)	; dst starting bit addr
cp_slop_addr:	.equ	-(2*form_sz+12)(fp)	; addr of loop for slop row

cp_R_xinc:	.equ	r3			; inner loop incr: +-src.xinc
cp_R_yinc:	.equ	r1			; outer loop incr: +-25

	enter	[r3,r4,r5,r6,r7],2*form_sz+12

	; Create cp_src and cp_dst, modified versions of src and dst
	; that take copy direction into account so as to avoid
	; unnecessary loss of information.
	movqd	form_sz shr 2,r0	; string length (fits as quick opnd)
	addr	0(cp_src_argp),r1	; src addr (for move)
	addr	cp_src,r2		; dst addr (for move)
	movsd
	movqd	form_sz shr 2,r0
	addr	0(cp_dst_argp),r1
	addr	cp_dst,r2
	movsd

	movd	xinc+cp_src,cp_R_xinc
	cmpd	x+cp_src,x+cp_dst
	bge	cp_xskip
	; Reset origin to other side of box & reverse iteration direction:
	addd	bw+cp_src,x+cp_src	; src.r.x += bw - 1
	addqd	-1,x+cp_src
	addd	bw+cp_dst,x+cp_dst	; dst.r.x += bw - 1
	addqd	-1,x+cp_dst
	negd	cp_R_xinc,cp_R_xinc
cp_xskip:
	addr	@25,cp_R_yinc
	cmpd	y+cp_src,y+cp_dst
	bge	cp_yskip
	; Reset origin to other side of box & reverse iteration direction:
	addd	bh+cp_src,y+cp_src	; src.r.y += bh - 1
	addqd	-1,y+cp_src
	addd	bh+cp_dst,y+cp_dst	; dst.r.y += bh - 1
	addqd	-1,y+cp_dst
	negd	cp_R_yinc,cp_R_yinc
cp_yskip:

	; /*
	;  * The copy proceeds in horizontal bands whose heights are
	;  * determined by limitations of the ins and ext instructions.
	;  */
	; while (remaining_height > 25) {
	;	/* Copy the next 25-high row of pixels: */
	;	remaining_height -= 25
	; }

	; Get starting addresses for source and destination:
	addr	cp_src,r7
	bsr	xy2addr
	movd	r0,cp_srcp
	addr	cp_dst,r7
	bsr	xy2addr
	movd	r0,cp_dstp

	movqd	0,r0	; Zap, for subsequent insd & extd instructions

	; If cp_R_yinc < 0, we must bias addresses downward so that we name
	; the bottom of each patch to be moved rather than the top.
	cmpqd	0,cp_R_yinc
	ble	cp_hor_band_loop
	subd	24,cp_srcp
	subd	24,cp_dstp

cp_hor_band_loop:
	; See if the outer loop requires another pass.
	;	Note that initially, src.r.bh == dst.r.bh.
	cmpd	bh+cp_dst,25
	ble	cp_slop_band

	movd	cp_srcp,r7
	movd	cp_dstp,r6
	movd	bw+cp_dst,r5
cp_vert_patch_loop:

	; Do some actual work: copy a 25-pixel vertical patch from src to dst:
	extd	r7,0(r0),r4,25
	insd	r6,r4,0(r0),25

	; Iterate the inner loop:
	addd	cp_R_xinc,r7
	addd	cp_R_xinc,r6
	acbd	-1,r5,cp_vert_patch_loop

	; Iterate the outer loop:
	addd	cp_R_yinc,cp_srcp
	addd	cp_R_yinc,cp_dstp
	subd	25,bh+cp_dst
	br	cp_hor_band_loop

cp_slop_band:
	; Fill in the length specifiers for the static base-resident
	; extract and insert field instructions for moving the slop
	; remaining at the ends of columns.
	movd	cp_srcp,r7
	movd	cp_dstp,r6
	movd	bw+cp_dst,r5
	movb	bh+cp_dst,cp_el
	movb	bh+cp_dst,cp_il

	; If copying downward, we must rebias by removing old bias and
	; rebiasing by remaining-height - 1.
	;	r7 += 25 - remaining-height;
	;	r6 += 25 - remaining-height;
	cmpqd	0,cp_R_yinc
	ble	cp_slop_go
	addd	25,r7
	addd	25,r6
	subd	bh+cp_dst,r7
	subd	bh+cp_dst,r6

cp_slop_go:
	; Copy the final partial row of pixels:
	addr	cp_slop,cp_slop_addr
	jsr	0(cp_slop_addr)

	exit	[r3,r4,r5,r6,r7]
	rxp	0

	; The actual loop for copying the final partial row.
	; It lives in the static data area because lengths must
	; be patched into the insd and extd instructions before
	; running the loop.
	.static
cp_slop::
	.byte	x'2e,x'3b,x'41,x'00	; extd	r7,0(r0),r4,bh
cp_el:	.byte	x'00
	.byte	x'ae,x'33,x'22,x'00	; insd	r6,r4,0(r0),bh
cp_il:	.byte	x'00
	.word	x'19c3			; addd	cp_R_xinc,r7
	.word	x'1983			; addd	cp_R_xinc,r6
	.byte	x'cd,x'2f,x'72		; acbw	-1,r5,cp_slop
	.word	x'0012			; ret	0
	.endseg


;
; initialization table for crt controller
;
regval:	.byte	h'3b	; 00	horiz. total
	.byte	h'32	; 01	horiz. displayed
	.byte	h'33	; 02	horiz. sync. pos.
	.byte	h'64	; 03	VSYNC, HSYNC width
	.byte	h'13	; 04	vert. total
	.byte	h'00	; 05	vert. total adjust
	.byte	h'13	; 06	vert. displayed
	.byte	h'13	; 07	vert. sync pos.
	.byte	h'8b	; 08	mode control
	.byte	h'0f	; 09	scan line
	.byte	h'00	; 10	cursor start
	.byte	h'00	; 11	cursor end
	.byte	h'00	; 12	display start MSB
	.byte	h'00	; 13	display start LSB
	.byte	h'00	; 14	cursor pos. MSB
	.byte	h'00	; 15	cursor pos. LSB

 regcount:	.equ	*-regval	; size of crt initialization database

		.endseg
# endif NBM
