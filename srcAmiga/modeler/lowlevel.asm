	opt	l+	;Linkable code
	opt	c+	;Case sensitive
	opt	d-	;No debugging information
	opt	m+	;Expand macros in listing
	opt	o+	;Optimisation on

;***************
;* PROGRAM:       Modeler
;* NAME:          lowlevel.asm
;* VERSION:       1.0 26.05.1995
;* DESCRIPTION:   This module includes all the stuff which is written in assembler for
;*		a higher speed
;*	AUTHORS:			Andreas Heumann
;* HISTORY:
;*		DATE		NAME	COMMENT
;*		30.11.95	ah		initial release
;***************

	incdir	"acom:include"
	include	lvo/graphics.i
	include	graphics/rastport.i

	xref	_GfxBase

ditherData
	dc.w	$ffff,$ffff,$ffff,$ffff
	dc.w	$ffff,$ffff,$ffff,$7777
	dc.w	$ffff,$dddd,$ffff,$7777
	dc.w	$ffff,$dddd,$ffff,$5555
	dc.w	$ffff,$5555,$ffff,$5555
	dc.w	$ffff,$5555,$bbbb,$5555
	dc.w	$eeee,$5555,$bbbb,$5555
	dc.w	$eeee,$5555,$aaaa,$5555
	dc.w	$aaaa,$5555,$aaaa,$5555
	dc.w	$aaaa,$5555,$aaaa,$1111
	dc.w	$aaaa,$4444,$aaaa,$1111
	dc.w	$aaaa,$4444,$aaaa,$0000
	dc.w	$aaaa,$0000,$aaaa,$0000
	dc.w	$aaaa,$0000,$2222,$0000
	dc.w	$8888,$0000,$2222,$0000
	dc.w	$8888,$0000,$0000,$0000
	dc.w	$0000,$0000,$0000,$0000

;************************************************************************
;* Span draws a shaded line betweeen x1 and x2 at height z. If ltor is TRUE
;* the line is interpolatet left to right else right to left
;*
;*	Input:	a5.l -> rport		Pointer to rastport
;*				a2.l -> zptr		Pointer to zbuffer
;*				a3.w -> left		left window border
;*				a4.w -> right		right window border
;*				d0.b -> ltor		TRUE -> left to right; FLASE -> right to left
;*				d1.l -> dither		dither value
;*				d2.w -> z			z position
;*				d3.w -> x1			begin of span
;*				d4.w -> x2			end of span
;*				d5.l -> y			y value
;*				d6.l -> dy			y delta
;************************************************************************/

save	equ	44

	xdef	_span
_span
	movem.l	d2-d6/a2-a6,-(a7)
	move.l	_GfxBase,a6

	move.l	save(a7),a5
	move.l	save+4(a7),a2
	move.w	save+8(a7),a3
	move.w	save+10(a7),a4
	move.l	save+14(a7),d1
	move.w	save+18(a7),d2
	move.w	save+20(a7),d3
	move.w	save+22(a7),d4
	move.l	save+24(a7),d5
	move.l	save+28(a7),d6
	lsl.w		#3,d1
	lea		ditherData,a0
	adda.l	d1,a0
	moveq		#0,d1
	move.w	d2,d1
	and.w		#3,d1
	lsl.w		#1,d1
	adda.l	d1,a0
	move.w	(a0),rp_LinePtrn(a5)	; rport->Mask = ditherData[dither][z & 3]

	moveq		#1,d1		; move = TRUE

	tst.w		save+12(a7)
	beq.w		rtol

ltor						; draw left to right
	cmp.w		d3,d4
	ble.w		done
	cmp.w		a4,d3
	bgt.w		done		; while((x1 <= x2) && (x1 < right))

	cmp.l		(a2),d5
	bgt.b		dont_draw
	cmp.w		a3,d3
	blt.b		dont_draw	; if((y < *zptr) && (x1 > left))

	move.l	d5,(a2)	; *zptr = y

	tst.b		d1
	beq.b		nodraw	; if (move)
	movea.l	a5,a1
	move.w	d3,d0
	move.w	d2,d1
	jsr		_LVOMove(a6)	; Move(rport,x1,z)
	moveq		#0,d1		; move = FALSE
	bra.b		nodraw
							; endif
dont_draw
							; else
	tst.b		d1
	bne.b		nodraw	; if (!move)
	movea.l	a5,a1
	move.w	d3,d0
	subq.w	#1,d0
	move.w	d2,d1
	jsr		_LVODraw(a6)	; Draw(rport,x1-1,z)
	moveq		#1,d1		; move = TRUE
							; endif
							; endelse
nodraw
	addq.w	#1,d3		; x1++
	adda.l	#4,a2		; zptr++
	add.l		d6,d5		; y += dy

	bra.b		ltor

rtol						; draw right to left
	cmp.w		d3,d4
	bgt.b		done
	cmp.w		a3,d3
	blt.b		done		; while((x1 >= x2) && (x1 > left))

	cmp.l		(a2),d5
	bgt.b		dont_draw1
	cmp.w		a4,d3
	bgt.b		dont_draw1	; if((y < *zptr) && (x1 < right))

	move.l	d5,(a2)	; *zptr = y

	tst.b		d1
	beq.b		nodraw1	; if (move)
	move.l	a5,a1
	move.w	d3,d0
	move.w	d2,d1
	jsr		_LVOMove(a6)	; Move(rport,x1,z)
	moveq		#0,d1		; move = FALSE
	bra.b		nodraw1
							; endif
dont_draw1
							; else
	tst.b		d1
	bne.b		nodraw1	; if (!move)
	move.l	a5,a1
	move.w	d3,d0
	addq.w	#1,d0
	move.w	d2,d1
	jsr		_LVODraw(a6)	; Draw(rport,x1+1,z)
	moveq		#1,d1		; move = TRUE
							; endif
							; endelse
nodraw1
	subq.w	#1,d3		; x1--
	suba.l	#4,a2		; zptr--
	sub.l		d6,d5		; y -= dy

	bra.b		rtol

done
	tst.w		d1			; if (!move)
	bne.b		done1
	move.l	a5,a1
	move.w	d3,d0
	move.w	d2,d1
	jsr		_LVODraw(a6)	; Draw(rport,x2,z)
done1
	movem.l	(a7)+,d2-d6/a2-a6
	rts

	end

