	XDEF _sin__r
	XDEF _cos__r

	XDEF _sin
	XDEF _cos

	SECTION  ":0",Code

_sin__r:
_sin:
	stwu  r1,-64(r1)
	lw    r3,@__trig_data
sin.b:
	stfd  f1,32(r1)
	mffs  f4
	li r12,0
common:
	lwz   r4,32(r1)
	rlwinm   r5,r4,12,21,31
	lfd   f3,120(r3)
	addi  r6,r5,-992
	lfd   f5,104(r3)
	cmplwi   cr5,r6,71
	lfd   f6,112(r3)
	mtfsf 255,f3
	cmplwi   cr6,r6,30
	lfd   f7,96(r3)
	bge   cr5,other_cases
	li r6,0
	lfd   f9,40(r3)
	blt+  cr6,reduce_done
reduce:
	fmadd f0,f1,f5,f3
	stfd  f0,48(r1)
	fsub  f2,f0,f3
	fnmsub   f1,f2,f6,f1
	lwz   r6,52(r1)
reduce_done:
	add   r11,r6,r12
	fmul  f3,f1,f1
	clrlwi.  r7,r11,31
	lfd   f8,88(r3)
	rlwinm   r8,r11,0,30,30
	fmul  f6,f3,f3
	lfd   f10,32(r3)
	cmpwi cr7,r8,0
	lfd   f11,80(r3)
	lfd   f13,24(r3)
	beq   actual_sin
actual_cos:
	lfd   f0,72(r3)
	lfd   f2,64(r3)
	lfd   f12,56(r3)
	lfd   f10,48(r3)
	fmadd f7,f3,f7,f8
	fmadd f11,f3,f11,f0
	fmadd f12,f3,f2,f12
	lfd   f9,144(r3)
	fmul  f2,f3,f6
	fmadd f7,f6,f7,f11
	fmadd f0,f3,f12,f10
	fmadd f0,f2,f7,f0
	fmadd f1,f3,f0,f9
	beq   cr7,actual_cos_ok
	fneg  f1,f1
actual_cos_ok:
	mtfsf 255,f4
	addi  r1,r1,64
	blr
actual_sin:
	lfd   f5,16(r3)
	lfd   f2,8(r3)
	lfd   f8,0(r3)
	fmadd f7,f3,f9,f10
	fmadd f11,f3,f13,f5
	fmadd f8,f3,f2,f8
	fmul  f5,f1,f3
	fmadd f7,f6,f7,f11
	fmadd f0,f6,f7,f8
	fmadd f1,f5,f0,f1
	beq   cr7,actual_sin_ok
	fneg  f1,f1
actual_sin_ok:
	mtfsf 255,f4
	addi  r1,r1,64
	blr
other_cases:
	cmpwi cr6,r5,992
	cmpwi cr5,r5,2047
	bgt   cr6,multiple_nan_inf
	lfd   f2,152(r3)
	fcmpo cr6,f5,f1
	lfd   f3,144(r3)
	clrlwi.  r11,r12,31
	beq   zero_sin
zero_cos:
	fmr   f1,f3
zero_sin:
	mtfsf 255,f4
	addi  r1,r1,64
	blr
multiple_nan_inf:
	addi  r7,r5,-1013
	mulli r7,r7,1311
	beq   cr5,return_invalid
	rlwinm   r7,r7,16,16,31
	mulli r8,r7,50
	subfc r8,r8,r5
	lfd   f8,88(r3)
	rlwimi   r4,r8,20,1,11
	stw   r4,32(r1)
	mfctr r11
	lfd   f1,32(r1)
	mtctr r7
	lfd   f2,136(r3)
reduce_loop:
	fmadd f10,f5,f1,f3
	fsub  f10,f10,f3
	fnmsub   f1,f6,f10,f1
	fmul  f1,f1,f2
	bdnz  reduce_loop
	mtctr r11
	b  reduce
return_invalid:
	lfd   f1,128(r3)
	mtfsf 255,f4
	addi  r1,r1,64
	blr

_cos__r:
_cos:
	stwu  r1,-64(r1)
	lw    r3,@__trig_data
cos.b:
	stfd  f1,32(r1)
	mffs  f4
	li r12,1
	b  common

;------------------------------ DATA

	SECTION  ":1",Data
_trig_data:
	dc.b  $bf,$c5,$55,$55,$55,$55,$55,$5a,$3f,$81,$11
	dc.b  $11,$11,$11,$d,$97,$bf,$2a,1,$a0,$19,$e4
	dc.b  $a9,$ac,$3e,$c7,$1d,$e3,$72,$62,$ed,$56,$be
	dc.b  $5a,$e5,$f9,$33,$56,$9b,$98,$3d,$e5,$de,$23
	dc.b  $ad,$24,$95,$f2,$bf,$e0,0,0,0,0,0
	dc.b  0,$3f,$a5,$55,$55,$55,$55,$55,$4c,$bf,$56
	dc.b  $c1,$6c,$16,$c1,$52,$21,$3e,$fa,1,$a0,$19
	dc.b  $cb,$f6,$71,$be,$92,$7e,$4f,$81,$2b,$49,$5b
	dc.b  $3e,$21,$ee,$9f,$14,$cd,$c5,$90,$bd,$a8,$fb
	dc.b  $12,$ba,$f5,$9d,$4b,$3f,$e4,$5f,$30,$6d,$c9
	dc.b  $c8,$83,$3f,$f9,$21,$fb,$54,$44,$2d,$18,$43
	dc.b  $38,0,0,0,0,0,0,$7f,$f8,0,0
	dc.b  0,0,0,0,$43,$10,0,0,0,0,0
	dc.b  0,$3f,$f0,0,0,0,0,0,0,0,0
	dc.b  0,0,0,0,0,0

@__trig_data:
	dc.l  _trig_data

