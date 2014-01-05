	XDEF _sqrt__r
	XDEF _estinvsqrt__r
	XDEF _estinv__r

	XDEF _sqrt
	XDEF _estinvsqrt
	XDEF _estinv

;   XREF _errno

	SECTION  ":0",Code

_estinvsqrt__r:
_estinvsqrt:
	frsqrte  f1,f1
	blr

_estinv__r:
_estinv:
	fres  f1,f1
	blr

_sqrt__r:
_sqrt:
	lw    r3,@__sqrt_data
	lwz   r4,32(r1)
	stwu  r1,-64(r1)
sqrt.b:
	li    r11,0
start_sqrt:
	li    r9,0
	mffs  f12
repeat:
	stfd  f1,32(r1)
	lwz   r4,32(r1)
	li    r5,511
	lfd   f11,1040(r3)
	addi  r6,r4,0
	rlwimi   r4,r5,21,0,10
	stw   r4,32(r1)
	lfd   f2,32(r1)
	mtfsf 255,f11
	rlwinm   r4,r4,21,22,28
	lfsux f3,r4,r3
	rlwinm   r5,r6,12,21,31
	cmpwi cr7,r6,0
	lfs   f4,4(r4)
	fnmsub   f5,f3,f3,f2
	cmpwi cr6,r5,0
	fadd  f6,f4,f4
	lwz   r8,36(r1)
	blt   cr7,arg_negative
	lfd   f7,1024(r3)
	cmpwi cr7,r5,2047
	fmadd f3,f4,f5,f3
	beq   cr6,zero_exp
	addi  r5,r5,1024
	add   r5,r5,r9
	rlwinm   r5,r5,19,1,11
	beq   cr7,max_exp
	stw   r5,32(r1)
	fnmsub   f8,f4,f3,f7
	li r5,0
	stw   r5,36(r1)
	lfd   f9,32(r1)
	fmadd f4,f8,f6,f4
	fnmsub   f5,f3,f3,f2
	stfd  f12,48(r1)
	fadd  f6,f4,f4
	fmadd f3,f4,f5,f3
	lwz   r10,52(r1)
	fmul  f0,f2,f9
	fnmsub   f8,f4,f3,f7
	fmadd f4,f8,f6,f4
	clrlwi   r10,r10,30
	fnmsub   f5,f3,f3,f2
	stw   r10,52(r1)
	fadd  f6,f4,f4
	cmpwi cr7,r11,0
	fmadd f3,f4,f5,f3
	lfd   f10,48(r1)
	fnmsub   f8,f4,f3,f7
	fmul  f1,f3,f9
	fmadd f4,f8,f6,f4
	bne   cr7,return_single
	fnmsub   f5,f3,f1,f0
	mtfsf 255,f10
	fmadd f1,f4,f5,f1
	mtfsf 255,f12
	addi  r1,r1,64
	blr
return_single:
	fnmsub  f5,f3,f1,f0
	mtfsf 255,f10
	fmadd f1,f4,f5,f1
	frsp  f1,f1
	mtfsf   255,f12
	addi r1,r1,64
	blr
arg_negative:
	clrlwi   r7,r6,1
	or. r7,r7,r8
	bne domain_error
negative_zero:
	mtfsf   255,f12
	addi  r1,r1,64
	blr
domain_error:
	rlwinm   r7,r6,12,21,31
	cmpwi  r7,2047
	beq max_exp
invalid_return:
;   lw    r5,@_errno
	lfd  f1,1032(r3)
;   li   r3,33
;   stw  r3,0(r5)
	mtfsf  255,f12
	addi   r1,r1,64
	blr
max_exp:
	clrlwi  r7,r6,12
	or.   r8,r7,r8
	cmpwi cr7,r8,0
	bne   its_a_nan
infinity:
	cmpwi  r6,0
	blt  invalid_return
	mtfsf   255,f12
	addi r1,r1,64
	blr
its_a_nan:
	rlwinm.  r7,r6,12,0,0
	bne its_a_qnan
	oris   r6,r6,8
	stw r6,32(r1)
	lfd   f1,32(r1)
;   lw    r5,@_errno
;   li r3,33
;   stw   r3,0(r5)
	mtfsf 255,f12
	addi  r1,r1,64
	blr
its_a_qnan:
	mtfsf   255,f12
	addi  r1,r1,64
	blr
zero_exp:
	or. r7,r6,r8
	bne  denorm
	mtfsf   255,f12
	addi r1,r1,64
	blr
denorm:
	lfs   f2,1048(r3)
	fmul   f1,f1,f2
	li  r9,-64
	b   repeat
	stwu  r1,-64(r1)
	li r11,1
	b  start_sqrt
	stwu  r1,-64(r1)
	lfd   f1,0(r3)
	li r11,0
	b  start_sqrt
	stwu  r1,-64(r1)
	lfs   f1,0(r3)
	li r11,1
	b  start_sqrt

	SECTION  ":1",Data

_sqrt_data:
	dc.l  $3f35b99e,$3f3450fc,$3f3720dd,$3f32ef41,$3f388560,$3f319589,$3f39e738,$3f304387,$3f3b4673,$3f2ef8f2,$3f3ca321
	dc.l  $3f2db587,$3f3dfd4e,$3f2c7904,$3f3f5509,$3f2b432a,$3f40aa5f,$3f2a13c0,$3f41fd5c,$3f28ea8c,$3f434e0d,$3f27c759
	dc.l  $3f449c7e,$3f26a9f4,$3f45e8b9,$3f25922c,$3f4732ca,$3f247fd3,$3f487abc,$3f2372bd,$3f49c098,$3f226abe,$3f4b046a
	dc.l  $3f2167af,$3f4c463a,$3f206968,$3f4d8613,$3f1f6fc4,$3f4ec3fc,$3f1e7aa0,$3f500000,$3f1d89d9,$3f513a26,$3f1c9d4e
	dc.l  $3f527278,$3f1bb4e1,$3f53a8fd,$3f1ad073,$3f54ddbc,$3f19efe6,$3f5610bf,$3f191320,$3f57420b,$3f183a05,$3f5871a9
	dc.l  $3f17647c,$3f599fa0,$3f16926c,$3f5acbf5,$3f15c3bc,$3f5bf6b1,$3f14f857,$3f5d1fd9,$3f143026,$3f5e4773,$3f136b13
	dc.l  $3f5f6d87,$3f12a90b,$3f609219,$3f11e9f9,$3f61b531,$3f112dca,$3f62d6d3,$3f10746d,$3f63f704,$3f0fbdce,$3f6515cc
	dc.l  $3f0f09dd,$3f66332e,$3f0e5889,$3f674f2f,$3f0da9c2,$3f6869d6,$3f0cfd79,$3f698327,$3f0c539f,$3f6a9b26,$3f0bac25
	dc.l  $3f6bb1d9,$3f0b06fd,$3f6cc744,$3f0a641a,$3f6ddb6b,$3f09c36d,$3f6eee53,$3f0924ec,$3f700000,$3f088889,$3f711076
	dc.l  $3f07ee37,$3f721fba,$3f0755ed,$3f732dcf,$3f06bf9e,$3f743aba,$3f062b40,$3f75467e,$3f0598c7,$3f76511e,$3f05082a
	dc.l  $3f775aa0,$3f04795f,$3f786305,$3f03ec5c,$3f796a52,$3f036117,$3f7a708b,$3f02d788,$3f7b75b1,$3f024fa6,$3f7c79ca
	dc.l  $3f01c967,$3f7d7cd8,$3f0144c4,$3f7e7ede,$3f00c1b4,$3f7f7fe0,$3f004030,$3f807fc0,$3eff017e,$3f817dc7,$3efd0d3e
	dc.l  $3f8279df,$3efb2452,$3f837413,$3ef9464e,$3f846c6f,$3ef772cb,$3f8562fc,$3ef5a969,$3f8657c5,$3ef3e9cb,$3f874ad3
	dc.l  $3ef23399,$3f883c2f,$3ef08681,$3f892be3,$3eeee232,$3f8a19f6,$3eed4660,$3f8b0672,$3eebb2c5,$3f8bf15e,$3eea271a
	dc.l  $3f8cdac3,$3ee8a31d,$3f8dc2a7,$3ee72691,$3f8ea912,$3ee5b139,$3f8f8e0c,$3ee442db,$3f90719a,$3ee2db41,$3f9153c4
	dc.l  $3ee17a36,$3f923491,$3ee01f87,$3f931406,$3edecb04,$3f93f229,$3edd7c7f,$3f94cf01,$3edc33cc,$3f95aa92,$3edaf0bf
	dc.l  $3f9684e4,$3ed9b330,$3f975dfa,$3ed87af7,$3f9835dc,$3ed747ee,$3f990c8c,$3ed619f2,$3f99e211,$3ed4f0df,$3f9ab66f
	dc.l  $3ed3cc93,$3f9b89ac,$3ed2acee,$3f9c5bcb,$3ed191d1,$3f9d2cd1,$3ed07b1d,$3f9dfcc2,$3ecf68b6,$3f9ecba4,$3ece5a7f
	dc.l  $3f9f9979,$3ecd505e,$3fa06646,$3ecc4a38,$3fa1320e,$3ecb47f4,$3fa1fcd7,$3eca497a,$3fa2c6a3,$3ec94eb3,$3fa38f75
	dc.l  $3ec85787,$3fa45753,$3ec763e2,$3fa51e3e,$3ec673ac,$3fa5e43b,$3ec586d3,$3fa6a94d,$3ec49d42,$3fa76d77,$3ec3b6e6
	dc.l  $3fa830bc,$3ec2d3ad,$3fa8f31f,$3ec1f383,$3fa9b4a4,$3ec11659,$3faa754d,$3ec03c1c,$3fab351d,$3ebf64bd,$3fabf417
	dc.l  $3ebe902c,$3facb23e,$3ebdbe58,$3fad6f95,$3ebcef34,$3fae2c1e,$3ebc22b1,$3faee7db,$3ebb58c0,$3fafa2d0,$3eba9154
	dc.l  $3fb05cff,$3eb9cc60,$3fb1166a,$3eb909d6,$3fb1cf13,$3eb849aa,$3fb286fe,$3eb78bd0,$3fb33e2c,$3eb6d03d,$3fb3f49f
	dc.l  $3eb616e4,$3fb4aa5a,$3eb55fba,$3fe00000,1,-$80000,0,$5f800000,0

@__sqrt_data:
	dc.l  _sqrt_data

;@_errno:
;   dc.l  _errno

