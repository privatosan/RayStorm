	XDEF _atan__r
	XDEF _atan2__r

	XDEF _atan
	XDEF _atan2

	SECTION  ":0",Code

_atan__r:
_atan:
	stwu r1,-80(r1)
	stfd f1,32(r1)
	mffs  f10
	lw    r11,@__atan_data
	lfd  f11,-48(r11)
	lwz  r10,32(r1)
	mtfsf   255,f11
	rlwinm  r9,r10,14,19,31
	addi r8,r9,-3972
	cmplwi  cr7,r8,112
	cmplwi  cr6,r8,200
	rlwinm  r7,r8,7,12,24
	fabs f3,f1
	add  r7,r7,r11
	bge  cr7,cases_abcdf
case_e:
	lfd   f13,-160(r11)
	fmul   f2,f1,f1
	lfd f12,-168(r11)
	lfd f11,-176(r11)
	lfd   f5,-184(r11)
	fmul  f3,f2,f2
	lfd   f9,-128(r11)
	lfd   f8,-136(r11)
	fmadd f6,f2,f13,f12
	lfd   f13,-144(r11)
	lfd   f12,-152(r11)
	fmadd f7,f2,f11,f5
	lfd   f11,-192(r11)
	fmul  f4,f3,f3
	fmadd f8,f2,f9,f8
	fmadd f12,f2,f13,f12
	fmadd f7,f3,f6,f7
	fmul  f5,f4,f3
	fmadd f12,f3,f8,f12
	fmul  f7,f3,f7
	fmadd f7,f5,f12,f7
	fmadd f11,f2,f11,f7
	fmadd f1,f1,f11,f1
	mtfsf 255,f10
	addi  r1,r1,80
	blr
cases_abcdf:
	cmplwi cr5,r8,332
	bge  cr6,cases_abcf
case_d:
	lfd   f0,-14336(r7)
	lfd f13,-14224(r7)
	fsub   f0,f3,f0
	lfd f12,-14232(r7)
	lfd   f11,-14320(r7)
	cmpwi cr7,r10,0
	lfd   f7,-14272(r7)
	lfd   f6,-14280(r7)
	fmul  f3,f0,f0
	lfd   f9,-14240(r7)
	fmul  f2,f0,f11
	lfd   f8,-14248(r7)
	fmadd f13,f0,f13,f12
	lfd   f5,-14328(r7)
	fmadd f7,f0,f7,f6
	lfd   f12,-14288(r7)
	fmsub f11,f0,f11,f2
	lfd   f6,-14296(r7)
	fmul  f1,f3,f3
	lfd   f4,-14256(r7)
	fmadd f9,f0,f9,f8
	fadd  f8,f5,f2
	fmadd f12,f0,f12,f6
	lfd   f6,-14264(r7)
	fmadd f13,f3,f13,f9
	lfd   f9,-14304(r7)
	fsub  f5,f8,f5
	fmadd f7,f3,f7,f12
	lfd   f12,-14312(r7)
	fmadd f4,f0,f4,f6
	fsub  f5,f2,f5
	fmul  f6,f1,f3
	fmadd f9,f0,f9,f12
	blt   cr7,atan_case_d_minus
	fadd  f5,f5,f11
	fmadd f13,f3,f13,f4
	fmadd f7,f3,f7,f9
	fmadd f13,f6,f13,f7
	fmadd f5,f3,f13,f5
	fadd  f1,f5,f8
	mtfsf 255,f10
	addi  r1,r1,80
	blr
atan_case_d_minus:
	fadd   f5,f5,f11
	fmadd   f13,f3,f13,f4
	fmadd   f7,f3,f7,f9
	fmadd   f13,f6,f13,f7
	fnmadd f5,f3,f13,f5
	fsub   f1,f5,f8
	mtfsf  255,f10
	addi   r1,r1,80
	blr
cases_abcf:
	cmpwi   r8,0
	cmplwi   cr7,r8,4216
	bge   cr5,cases_abf
case_c:
	 lfd   f4,-32(r11)
	fdiv   f3,f4,f1
	rlwinm r8,r10,5,27,27
	add r9,r8,r11
	lfd   f6,-112(r9)
	lfd   f5,-120(r9)
	fsub  f6,f6,f3
	fadd  f1,f5,f6
	mtfsf 255,f10
	addi  r1,r1,80
	blr
cases_abf:
	bge   cases_ab
case_f:
	mtfsf 255,f10
	addi   r1,r1,80
	blr
cases_ab:
	bge cr7,case_a
case_b:
	cmpwi cr7,r10,0
	lfd f1,-120(r11)
	bge+   cr7,atan_pi2_sign_now_ok
	fneg   f1,f1
atan_pi2_sign_now_ok:
	mtfsf  255,f10
	addi r1,r1,80
	blr
case_a:
	lwz   r7,36(r1)
	clrlwi r9,r10,12
	or. r8,r7,r9
	beq-   case_b
	lfd   f1,-40(r11)
	mtfsf 255,f10
	addi  r1,r1,80
	blr

_atan2__r:
_atan2:
	stwu   r1,-80(r1)
atan2_starts_here:
	fabs   f3,f1
	lw     r11,@__atan_data
	fabs f4,f2
	stfd   f2,32(r1)
	mffs   f10
	lfd f11,-48(r11)
	stfd   f1,48(r1)
	mtfsf  255,f11
	fcmpu  cr5,f4,f3
	lfd f13,-32(r11)
	fcmpu  cr6,f2,f11
	lwz r10,32(r1)
	fcmpu  cr7,f1,f11
	lwz r9,48(r1)
	blt cr5,atan2_y_gt_x
atan2_x_ge_y:
	bso   cr5,atan2_unordered
	beq cr7,atan2_x_ge_y_eq_0
atan2_recycle_x_ge_y:
	fdiv   f5,f13,f4
	rlwinm. r8,r10,12,21,31
	cmpwi   cr5,r8,2047
	rlwinm  r7,r9,12,21,31
	subfc  r6,r8,r7
	cmpwi  cr7,r6,-3
	beq atan2_x_is_denorm
	cmpwi  r6,-30
	beq cr5,atan2_x_is_infinite
atan2_inv_x_representable:
	cmpwi   cr5,r6,-1021
	clrlwi   r4,r10,12
	bgt+  cr7,atan2_cases_56
	clrlwi   r5,r9,12
	beq  cr7,atan2_x_ge_y_resolve
	bge+ atan2_cases_12
	blt  cr5,atan2_x_ge_y_prob_uflow
atan2_x_ge_y_one_term:
	bge   cr6,atan2_x_ge_y_direct
	cmpwi  cr7,r9,0
	lfd f8,-64(r11)
	lfd f9,-72(r11)
	fmsub f8,f3,f5,f8
	blt   cr7,atan2_when_minus
	fsub  f1,f9,f8
	mtfsf 255,f10
	addi  r1,r1,80
	blr
atan2_when_minus:
	fsub f1,f8,f9
	mtfsf 255,f10
	addi  r1,r1,80
	blr
atan2_x_ge_y_direct:
	fnmsub  f9,f4,f5,f13
	fmul  f8,f1,f5
	fmul  f8,f8,f9
	fmadd f1,f1,f5,f8
	mtfsf   255,f10
	addi r1,r1,80
	blr
atan2_x_ge_y_resolve:
	cmpw   cr7,r4,r5
	bge  cr7,atan2_cases_12
atan2_cases_56:
	addi   r8,r11,-1024
	blt  cr6,atan2_case_6
atan2_case_5:
	fmul  f1,f3,f5
	stfd   f10,40(r1)
	fmsub  f2,f4,f5,f13
	fmsub  f0,f3,f5,f1
	stfd  f1,56(r1)
	lwz   r7,56(r1)
	fnmsub   f0,f2,f1,f0
	rlwinm   r6,r7,22,18,24
	lfdx  f2,r8,r6
	add   r5,r8,r6
	lfd   f3,24(r5)
	fsub  f2,f1,f2
	lfd   f11,16(r5)
	fadd  f4,f3,f3
	lfd   f12,8(r5)
	fmul  f5,f11,f2
	lfd   f10,80(r5)
	fmadd f4,f4,f2,f11
	lfd   f9,72(r5)
	fmul  f1,f2,f2
	lfd   f13,64(r5)
	fmsub f6,f11,f2,f5
	lfd   f7,56(r5)
	lfd   f11,40(r5)
	cmpwi cr7,r9,0
	fmadd f10,f2,f10,f9
	lfd   f9,48(r5)
	fmadd f6,f0,f4,f6
	lfd   f4,32(r5)
	fadd  f0,f12,f5
	fmadd f13,f2,f13,f7
	fmadd f9,f2,f9,f11
	fsub  f7,f0,f12
	lfd   f11,40(r1)
	fmadd f4,f2,f4,f3
	fmul  f3,f1,f1
	fsub  f7,f7,f5
	fmadd f10,f1,f10,f13
	fmadd f4,f1,f9,f4
	fmadd f10,f3,f10,f4
	fsub  f6,f6,f7
	fnmadd   f10,f1,f10,f6
	blt   cr7,atan2_case_5_flip
	fsub  f1,f0,f10
	mtfsf 255,f11
	addi  r1,r1,80
	blr
atan2_case_5_flip:
	fsub   f1,f10,f0
	mtfsf   255,f11
	addi r1,r1,80
	blr
atan2_case_6:
	lfd   f8,-72(r11)
	lfd f7,-64(r11)
	fmul   f1,f3,f5
	stfd   f10,40(r1)
	fmsub f2,f4,f5,f13
	fmsub f0,f3,f5,f1
atan2_cases_67:
	stfd   f1,56(r1)
	lwz  r7,56(r1)
	fnmsub  f0,f2,f1,f0
	rlwinm  r6,r7,22,18,24
	lfdx   f2,r8,r6
	add r5,r8,r6
	lfd f3,24(r5)
	fsub   f2,f1,f2
	lfd f11,16(r5)
	fadd   f4,f3,f3
	lfd f12,8(r5)
	fmul   f5,f11,f2
	lfd f10,80(r5)
	fmadd  f4,f4,f2,f11
	lfd f9,72(r5)
	fmul   f1,f2,f2
	lfd f13,64(r5)
	fmsub  f6,f11,f2,f5
	lfd f11,40(r5)
	fnmsub f4,f0,f4,f7
	lfd f7,56(r5)
	fsub   f0,f8,f12
	cmpwi  cr7,r9,0
	fmadd  f10,f2,f10,f9
	lfd f9,48(r5)
	fsub   f6,f4,f6
	lfd f4,32(r5)
	fsub   f0,f0,f5
	fmadd  f13,f2,f13,f7
	fmadd  f9,f2,f9,f11
	fsub   f7,f0,f8
	lfd f11,40(r1)
	fmadd  f4,f2,f4,f3
	fmul   f3,f1,f1
	fadd   f7,f7,f12
	fmadd  f10,f1,f10,f13
	fmadd  f4,f1,f9,f4
	fadd   f7,f7,f5
	fmadd  f10,f3,f10,f4
	fsub   f6,f6,f7
	fmsub  f10,f1,f10,f6
	blt cr7,atan2_case_6_flip
	fsub   f1,f0,f10
	mtfsf  255,f11
	addi   r1,r1,80
	blr
atan2_case_6_flip:
	fsub   f1,f10,f0
	mtfsf   255,f11
	addi r1,r1,80
	blr
atan2_cases_12:
	blt cr6,atan2_case_2
atan2_case_1:
	lfd   f11,-200(r11)
	lfd f9,-208(r11)
	lfd f8,-216(r11)
	lfd f2,-224(r11)
	fmul  f6,f1,f5
	fmsub f0,f4,f5,f13
	fmul  f4,f6,f6
	fmsub f1,f1,f5,f6
	lfd   f5,-232(r11)
	fmadd f11,f4,f11,f9
	lfd   f9,-240(r11)
	fmul  f3,f4,f4
	fnmsub   f1,f6,f0,f1
	fmadd f8,f4,f8,f2
	fmadd f5,f4,f5,f9
	lfd   f0,-248(r11)
	fnmsub   f9,f6,f6,f13
	fmul  f2,f3,f4
	fmadd f11,f3,f11,f8
	fmadd f5,f4,f5,f0
	fmul  f9,f1,f9
	fmul  f0,f4,f6
	fmadd f11,f2,f11,f5
	fmadd f9,f0,f11,f9
	fadd  f1,f6,f9
	mtfsf 255,f10
	addi  r1,r1,80
	blr
atan2_case_2:
	lfd   f12,-72(r11)
	lfd f11,-200(r11)
	lfd f9,-208(r11)
	lfd f8,-216(r11)
	lfd   f2,-224(r11)
	fmul  f6,f3,f5
	fmsub f0,f4,f5,f13
	fmul  f4,f6,f6
	fmsub f1,f3,f5,f6
	fsub  f7,f12,f6
	lfd   f5,-232(r11)
	fmadd f11,f4,f11,f9
	lfd   f9,-240(r11)
	fmul  f3,f4,f4
	fnmsub   f1,f6,f0,f1
	fmadd f8,f4,f8,f2
	cmpwi cr7,r9,0
	fmadd f5,f4,f5,f9
	lfd   f0,-248(r11)
	fnmsub   f9,f6,f6,f13
	fmul  f2,f3,f4
	fsub  f12,f7,f12
	fmadd f11,f3,f11,f8
	lfd   f3,-64(r11)
	fmadd f5,f4,f5,f0
	fadd  f12,f12,f6
	fnmsub   f9,f1,f9,f3
	fmul  f0,f4,f6
	fmadd f11,f2,f11,f5
	fsub  f9,f9,f12
	fmsub f9,f0,f11,f9
	blt   cr7,atan2_case_2_flip
	fsub  f1,f7,f9
	mtfsf 255,f10
	addi  r1,r1,80
	blr
atan2_case_2_flip:
	fsub   f1,f9,f7
	mtfsf   255,f10
	addi r1,r1,80
	blr
atan2_x_ge_y_eq_0:
	cmpwi  cr6,r10,0
	bge  cr6,atan2_return_it
atan2_return_pi_sign_y:
	cmpwi   cr6,r9,0
	lfd   f1,-72(r11)
	bge+  cr6,atan2_return_it
	fneg  f1,f1
atan2_return_it:
	mtfsf 255,f10
	addi   r1,r1,80
	blr
atan2_y_gt_x:
	bso   cr5,atan2_unordered
	beq cr6,atan2_return_pi_2_sign_y
atan2_recycle_y_gt_x:
	fdiv   f5,f13,f3
	rlwinm. r8,r9,12,21,31
	cmpwi   cr5,r8,2047
	rlwinm  r7,r10,12,21,31
	subfc  r6,r8,r7
	cmpwi  cr7,r6,-3
	beq atan2_y_is_denorm
	cmpwi  r6,-30
	beq cr5,atan2_return_pi_2_sign_y
atan2_inv_y_representable:
	cmpwi   cr5,r6,-1021
	clrlwi   r4,r10,12
	bgt+  cr7,atan2_cases_78
	clrlwi   r5,r9,12
	beq  cr7,atan2_y_gt_x_resolve
	bge+ atan2_cases_34
	blt  cr5,atan2_return_pi_2_sign_y
atan2_y_gt_x_one_term:
	lfd   f8,-112(r11)
	cmpwi  cr7,r9,0
	lfd f9,-120(r11)
	fmsub  f8,f2,f5,f8
	blt   cr7,atan2_y_minus
	fsub  f1,f9,f8
	mtfsf 255,f10
	addi  r1,r1,80
	blr
atan2_y_minus:
	fsub f1,f8,f9
	mtfsf 255,f10
	addi  r1,r1,80
	blr
atan2_y_gt_x_resolve:
	cmpw   cr7,r4,r5
	ble  cr7,atan2_cases_34
atan2_cases_78:
	addi   r8,r11,-1024
	bgt  cr6,atan2_case_7
atan2_case_8:
	lfd   f8,-120(r11)
	lfd f7,-112(r11)
	fmul   f1,f4,f5
	stfd   f10,40(r1)
	fmsub f2,f3,f5,f13
	fmsub f0,f4,f5,f1
	stfd  f1,56(r1)
	lwz   r7,56(r1)
	fnmsub   f0,f2,f1,f0
	rlwinm   r6,r7,22,18,24
	lfdx  f2,r8,r6
	add   r5,r8,r6
	lfd   f3,24(r5)
	fsub  f2,f1,f2
	lfd   f11,16(r5)
	fadd  f4,f3,f3
	lfd   f12,8(r5)
	fmul  f5,f11,f2
	lfd   f10,80(r5)
	fmadd f4,f4,f2,f11
	lfd   f9,72(r5)
	fmul  f1,f2,f2
	lfd   f13,64(r5)
	fmsub f6,f11,f2,f5
	lfd   f11,40(r5)
	fmadd f4,f0,f4,f7
	lfd   f7,56(r5)
	fadd  f0,f8,f12
	cmpwi cr7,r9,0
	fmadd f10,f2,f10,f9
	lfd   f9,48(r5)
	fadd  f6,f4,f6
	lfd   f4,32(r5)
	fadd  f0,f0,f5
	fmadd f13,f2,f13,f7
	fmadd f9,f2,f9,f11
	fsub  f7,f0,f8
	lfd   f11,40(r1)
	fmadd f4,f2,f4,f3
	fmul  f3,f1,f1
	fsub  f7,f7,f12
	fmadd f10,f1,f10,f13
	fmadd f4,f1,f9,f4
	fsub  f7,f7,f5
	fmadd f10,f3,f10,f4
	fsub  f6,f6,f7
	fnmadd   f10,f1,f10,f6
	blt   cr7,atan2_case_8_flip
	fsub  f1,f0,f10
	mtfsf 255,f11
	addi  r1,r1,80
	blr
atan2_case_8_flip:
	fsub   f1,f10,f0
	mtfsf   255,f11
	addi r1,r1,80
	blr
atan2_case_7:
	lfd   f8,-120(r11)
	lfd f7,-112(r11)
	fmul   f1,f4,f5
	stfd   f10,40(r1)
	fmsub f2,f3,f5,f13
	fmsub f0,f4,f5,f1
	b  atan2_cases_67
atan2_cases_34:
	lfd f12,-120(r11)
	lfd  f11,-200(r11)
	lfd  f9,-208(r11)
	lfd  f8,-216(r11)
	lfd f4,-224(r11)
	fmul   f6,f2,f5
	fmsub  f0,f3,f5,f13
	fmul   f3,f6,f6
	fmsub  f1,f2,f5,f6
	fsub   f7,f12,f6
	lfd f5,-232(r11)
	fmadd  f11,f3,f11,f9
	lfd f9,-240(r11)
	fmul   f2,f3,f3
	fnmsub f1,f6,f0,f1
	fmadd  f8,f3,f8,f4
	cmpwi  cr7,r9,0
	fmadd  f5,f3,f5,f9
	lfd f0,-248(r11)
	fnmsub f9,f6,f6,f13
	fmul   f4,f2,f3
	fsub   f12,f7,f12
	fmadd  f11,f2,f11,f8
	lfd f2,-112(r11)
	fmadd  f5,f3,f5,f0
	fadd   f12,f12,f6
	fnmsub f9,f1,f9,f2
	fmul   f0,f3,f6
	fmadd  f11,f4,f11,f5
	fsub   f9,f9,f12
	fmsub  f9,f0,f11,f9
	blt cr7,atan2_case_34_flip
	fsub   f1,f7,f9
	mtfsf  255,f10
	addi   r1,r1,80
	blr
atan2_case_34_flip:
	fsub  f1,f9,f7
	mtfsf  255,f10
	addi   r1,r1,80
	blr
atan2_return_pi_2_sign_y:
	cmpwi cr6,r9,0
	lfd f1,-120(r11)
	bge+   cr6,atan2_return_as_is
	fneg   f1,f1
atan2_return_as_is:
	mtfsf 255,f10
	addi   r1,r1,80
	blr
atan2_y_is_denorm:
	clrlwi r4,r9,12
	lis  r5,4
	cmpw r4,r5
	bgt  atan2_inv_y_representable
	blt atan2_inv_y_overflows
	lwz r5,52(r1)
	cmplwi r5,0
	bgt atan2_inv_y_representable
atan2_inv_y_overflows:
	lfd   f7,-24(r11)
	fmul   f1,f1,f7
	fmul   f2,f2,f7
	fmul   f3,f3,f7
	stfd  f1,64(r1)
	fmul  f4,f4,f7
	stfd  f2,56(r1)
	lwz   r10,56(r1)
	lwz   r9,64(r1)
	b  atan2_recycle_y_gt_x
atan2_unordered:
	lfd f1,-40(r11)
	mtfsf 255,f10
	addi  r1,r1,80
	blr
atan2_x_is_infinite:
	cmpwi   cr5,r7,2047
	cmpwi cr7,r9,0
	beq   cr5,atan2_two_infinities
	blt-  cr6,atan2_return_pi_sign_y
atan2_return_zero_sign_y:
	fmr   f1,f11
	bge+   cr7,atan2_zero_sign_ok
	fneg   f1,f11
atan2_zero_sign_ok:
	mtfsf 255,f10
	addi   r1,r1,80
	blr
atan2_two_infinities:
	blt cr6,atan2_return_pi_34_sign_y
atan2_return_pi_4_sign_y:
	lfd   f1,-88(r11)
	bge+   cr7,atan2_return_pi_4
	fneg   f1,f1
atan2_return_pi_4:
	mtfsf  255,f10
	addi r1,r1,80
	blr
atan2_return_pi_34_sign_y:
	lfd  f1,-80(r11)
	bge+  cr7,atan2_return_3pi_4
	fneg  f1,f1
atan2_return_3pi_4:
	mtfsf 255,f10
	addi   r1,r1,80
	blr
atan2_x_is_denorm:
	clrlwi r4,r10,12
	lis  r5,4
	cmpw r4,r5
	bgt  atan2_inv_x_representable
	blt atan2_inv_x_overflows
	lwz r5,36(r1)
	cmplwi r5,0
	bgt atan2_inv_x_representable
atan2_inv_x_overflows:
	lfd   f7,-24(r11)
	fmul   f1,f1,f7
	fmul   f2,f2,f7
	fmul   f3,f3,f7
	stfd  f1,64(r1)
	fmul  f4,f4,f7
	stfd  f2,56(r1)
	lwz   r10,56(r1)
	lwz   r9,64(r1)
	b  atan2_recycle_x_ge_y
atan2_x_ge_y_prob_uflow:
	blt cr6,atan2_return_pi_sign_y
	b atan2_x_ge_y_direct
atan2_x_lt_y:
	lfd   f1,-56(r11)
	mtfsf  255,f10
	addi   r1,r1,80
	blr

	SECTION  ":1",Data

_atan_data:
	dc.l  -$402aaaab,$55555554,$3fc99999,-$66666e2b,-$403db6dc,-$6dc7a64e,$3fbc71c6,-$6019b37,-$4048ba42,-$76da258a,$3fb3ab82
	dc.l  $4d08e4b1,-$404fd5c8,-$33b814a7,-$402aaaab,$55555552,$3fc99999,-$6666726e,-$403db6dc,-$6dc14eaa,$3fbc71c7,$1388c02d
	dc.l  -$4048ba31,$4d573c8e,$3fb3b0ea,$743ef5d6,-$404ef63e,-$16f2d19a,$3fad4fb2,$20cbbd43,-$405b851a,$6cd209f9,$3ff921fb
	dc.l  $54442d18,$3c91a626,$33145c07,-$4006de05,$54442d18,-$436e59da,$33145c07,$3fe921fb,$54442d18,$4002d97c,$7f3321d2
	dc.l  $400921fb,$54442d18,$3ca1a626,$33145c07,-$45414542,-$45414542,0,0,$7ff80000,0,$3ff00000
	dc.l  0,$43300000,0,$57f00000,0,$27f00000,0

	dc.l  $3fd20043,$1111be,$3fd18c33,-$4640effd
	dc.l  $3feda76f,-$1123162d,-$4031154a,-$131dc0f9,-$40361ab8,-$6a67fc17,$3fc87315,-$212120a7,$3fa0cee2,$230ac9fd,-$403f11e1
	dc.l  $2544704e,$3fa32227,$7897919e,$3fb32b79,-$31b650b2,-$405071a8,-$aaf92dc,-$4064c370,$d5fec38,$3fae6123,$507cedf5
	dc.l  -$40805f83,-$59e4f450,-$405a24a6,-$2c14a9ec,0,0,$3fd6006f,$1f9b1,$3fd53110,-$21cb3a2f,$3fec9e2d
	dc.l  $d3d909a,-$402e6740,-$7d434911,-$403c4d63,$2318c7d,$3fc8d21b,$617534bf,-$4075c288,-$1d7aac9f,-$4044158d,-$1cbba729
	dc.l  $3fb0cee0,-$6c7fd9cc,$3fa35c99,$65e4bf50,-$404f047c,$1d10c8ec,$3f83c65f,-$5a73afe7,$3fa616c4,$493326ce,-$405fec1a
	dc.l  $6874fa66,-$4071b02e,$5cee0014,0,0,$3fda0070,$3ce7f6,$3fd8b2ad,$5b991fbc,$3feb7764,-$6838fb2e
	dc.l  -$402cd811,$5eea168a,-$4044c28e,-$689cb7a7,$3fc790fc,-$31c6a365,-$40577637,$3708048,-$404c2921,-$17c6c370,$3fb361cd
	dc.l  $455e8a65,$3f6e30d9,-$677bf6c6,-$4054f3af,-$2e26f50e,$3f9f5d41,-$5bac8ff0,$3f921aa4,$26fd17ef,-$405f076e,-$7d510cfb
	dc.l  $3f8153dc,-$63f8ab5a,0,0,$3fde0079,$36ef5c,$3fdc0e17,-$2f12564,$3fea3c1c,$4890475a,-$402bd5b2
	dc.l  $463c0355,-$404ffa4c,$70415d71,$3fc52689,$46a427bf,-$404ddeac,-$aa009de,-$4058fc74,$133a0b56,$3fb232be,-$799be3a
	dc.l  -$406b57f1,$5cb74b38,-$405fd932,$46c65ce2,$3fa1f284,$7322ff5b,-$4094b09a,-$36a95a2b,-$4069e42e,$37885589,$3f9200ac
	dc.l  $4a1b3e12,0,0,$3fe20072,$18f7ca,$3fe065d5,$2ddd0944,$3fe84eb6,-$3fa07d19,-$402b39cf,-$39c01b70
	dc.l  -$4081b89c,-$5b2315e6,$3fc06303,$14c35cb5,-$404a746d,$169aee78,-$408bf724,$565a54b,$3fa936e5,-$22a73a2d,-$405e9232
	dc.l  -$55789890,-$40957f53,$621d738e,$3f97bb3f,$238fea6,-$406f9f14,-$164aefae,-$409d9358,$72b87233,$3f8879ff,$6234c3b6
	dc.l  0,0,$3fe60069,$f3d8d,$3fe34637,$6926c3a7,$3fe5ba7a,$3414b2ad,-$402bb641,-$73908330,$3fa657d0
	dc.l  -$637bfe85,$3fb3ba80,-$4c417e53,-$404cb649,-$afdd0f0,$3f984f0e,$6160358a,$3f91f6e6,-$527595aa,-$4064f18f,-$469bf5ac
	dc.l  $3f89efdd,$61453ead,$3f6e65c8,$b9e50b7,-$407b23c6,$79777f87,$3f7ae282,$42f4bd70,$3f2aa4fa,$2e644215,0
	dc.l  0,$3fea0067,$18098d,$3fe5d5c7,-$6e6501e2,$3fe3463c,-$d6700ff,-$402d2232,-$21719945,$3fb2498a,-$2e62275f
	dc.l  $3fa29abf,$11ea83a7,-$405436ae,-$54559fd1,$3f9e63ab,$dd829bc,-$40a6997e,-$c66a6a1,-$40774357,$461b2aab,$3f8702ae
	dc.l  $d522fe1,-$408f4c12,$260acd9d,-$40a0bb5d,-$4805f8cd,$3f6ed2af,-$7c67b19d,-$409c460e,-$4b75b7f,0,0
	dc.l  $3fee0068,$28155,$3fe81a08,$10af4731,$3fe107c4,-$7e296ed6,-$402f0149,-$129aa144,$3fb50e79,$48a32e39,$3f82a366
	dc.l  -$7d03b83d,-$405ed524,$202714b8,$3f993f1e,-$631be6b5,-$407eea0a,$3db9fe9,-$409b0f21,-$12e0f372,$3f776eb5,-$43589e19
	dc.l  -$408f6987,$401df317,$3f53615c,-$6f7e0dbf,$3f47be69,$32f54363,-$40ac71a8,$601079bc,0,0,$3ff20074
	dc.l  $1ec81b,$3feb03b5,-$628a71a0,$3fdc3ec3,-$5e81d3e5,-$4033f330,$64046d22,$3fb48566,$5744ad08,-$4078c243,$358d2c77
	dc.l  -$4076fe2d,-$3c1ed4,$3f8b8889,-$17303c1,-$4080e471,-$3733cb99,$3f619bc0,-$3921cc8c,$3f473f8b,-$e01f3e7,-$40a92c72
	dc.l  $6f42e798,$3f506962,-$4c144f4c,-$40c667c7,$7aff4ab5,-$410dff03,-$1455c700,0,0,$3ff60020,$1cbcbc
	dc.l  $3fee24f3,$68c19e91,$3fd623d0,$5692c239,-$403aeff5,-$5c7c7b54,$3fb08153,$69797ed9,-$406e09fc,$73e3a932,-$40fec255
	dc.l  $63c91308,$3f70acfc,-$64961ee,-$4094ded9,$4694ceb3,$3f5bdc45,-$2e71a36f,-$40be1405,-$3354553c,-$413cd2c6,-$93bfdd8
	dc.l  $3f248d7b,-$50af413e,-$40ddfd41,-$b023e54,$3f134a0d,$25677b57,0,0,$3ffa0031,$1bf1ea,$3ff04e74
	dc.l  -$6300c8a2,$3fd19423,$714fd0fd,-$40409d86,$52de6d77,$3fa87afc,-$3d79a1fc,-$4070ebbe,-$165fb825,$3f683b2f,-$28b2b23c
	dc.l  $3f42f39e,-$63fb5420,-$40af3dd8,$151ecfec,$3f4646b2,-$6edfc19c,-$40caf7ca,$28e566b2,$3f1c6f48,$6bcb611f,-$410f527f
	dc.l  -$b38ce5e,-$4116427f,$66431a4c,$3eecabf1,-$2fb281e4,0,0,$3ffe0039,$387ec0,$3ff14b2a,$756528ab
	dc.l  $3fcc5840,-$b0c251f,-$40487625,$7a431628,$3fa1b1a8,$28c19c6d,-$4078c4c5,-$4480c089,$3f681ec2,-$3b1bdf6e,-$40c8367e
	dc.l  -$72066f7d,-$40d4d219,-$33b743bd,$3f2ba68e,-$64c167ed,-$40e08a7a,$451d98a6,$3f0b6e75,-$6b784cd6,-$410d6169,-$621981fb
	dc.l  $3ecfeb62,$16e77ee9,$3e9bb770,$c4c7920,0,0,$40020039,$c9b3c,$3ff27102,$2352b6f7,$3fc51c98
	dc.l  $430957de,-$4050a8f2,$61118abc,$3f95bb00,$c24e9c5,-$408449d4,-$3435bd43,$3f5f699e,-$575d8664,-$40c308b2,-$76fd3192
	dc.l  $3f0d0103,-$3c4370d6,$3ef3a801,$2a425822,-$410b9708,$1af25c5e,$3ee613df,-$51c3ef04,-$412d37ac,-$4e07b19a,$3ebb313e
	dc.l  $1dda46e0,-$41606399,-$ea38908,0,0,$40060035,$119c76,$3ff38d76,-$320a0c3a,$3fbde557,-$55656d8c
	dc.l  -$405ccc20,-$7739292f,$3f8794c7,$28a65af6,-$4094800d,$df3f160,$3f4e1639,-$92ccb77,-$40d1d679,$4eaa2250,$3f0a44eb
	dc.l  -$3e502b07,-$411f3659,-$295e974c,$3e54eb35,$71aa4599,$3ea8b3ee,$43c56d8a,-$41630546,$583d3c04,$3e892260,-$5947735d
	dc.l  -$418dfb87,$72192f6d,0,0,$400a002c,$43de3,$3ff45b5c,$1fc6625a,$3fb623b6,$2cdc239,-$40671bcb
	dc.l  -$6fb2209a,$3f7b1a32,$6a9af51d,-$40a383c0,-$14cf414c,$3f3cbd95,-$17d72c8,-$40e46c0f,$5a6fd4a7,$3ef8bf1d,-$4e9455ed
	dc.l  -$412bee3b,$40949887,$3eaaf645,-$3a5880b,-$418bb9c4,$4d4815dc,-$41ac6431,$2f9bfea0,$3e507c0d,$406657a6,-$41c3dddd
	dc.l  $60d32705,0,0,$400e0020,$2d6dff,$3ff4f692,$2a24f752,$3fb0fece,$373b9a16,-$406f1350,-$b0095c4
	dc.l  $3f707461,-$179ec10c,-$40b0d1b1,-$15ed769,$3f2cb536,$59508999,-$40f66ee6,-$44ad7de0,$3ee5dd77,-$25c74a0d,-$413e436a
	dc.l  $50b5f34a,$3e9aac43,$10ffb6b9,-$418e54ec,-$357b4db7,$3e419e83,$230ab65a,-$422fbfa8,$3f974b8f,-$4202644b,$5518108
	dc.l  0,0,$4012004d,$8a15c,$3ff5a25e,-$2fa9a819,$3fa81753,-$537c67a3,-$407b9839,-$62db2afb,$3f60ffc3
	dc.l  -$71994b1c,-$40c42ac4,-$7206fac6,$3f165cc3,$3bb66d04,-$410e669b,$4224b33c,$3ecb0ed7,$7f36fd4a,-$415bc466,-$41d58fa0
	dc.l  $3e7d3ed1,-$92e6c59,-$41abc8cd,-$3f277803,$3e2a3e2a,-$3f4732ae,-$4201159e,-$704a6e24,$3dccb8eb,-$13cd831e,0
	dc.l  0,$4016006f,$85eca,$3ff6415b,$790478a3,$3fa061ad,-$3289201c,-$4088efba,$42459a0b,$3f500e84,$6747e6c0
	dc.l  -$40d9e5c1,-$4f6397e5,$3efe0fbb,-$3220a7cf,-$412bd11d,$50b8dbbb,$3eaaba73,-$45407aa6,-$417e9249,-$5020dbe0,$3e56568c
	dc.l  $2076421b,-$41d3f1e9,$31f2ae89,$3e013291,-$1cd9f621,-$422b3269,$22f41593,$3da7c650,-$1f6bb928,0,0
	dc.l  $401a0074,$28900a,$3ff6b0c5,-$5d86d496,$3f97ac53,-$53a26179,-$40938a12,$741dc97b,$3f40f925,-$10fc034,-$40ebeb30
	dc.l  $307757d7,$3ee78f5c,-$7784c9fc,-$414499a0,-$280326ff,$3e8f9246,-$7b5e6c10,-$419dfdaa,$62aea81,$3e34533d,-$62541410
	dc.l  -$41f955b6,-$7051de93,$3dd8f0b0,-$787ccbef,-$42548ecd,-$aee03d2,$3d7d43f5,-$75fc04f4,0,0,$401e006f
	dc.l  $c76ae,$3ff70314,-$45756613,$3f91e26d,$31b14143,-$409d41ea,-$14608c5f,$3f33867f,$60407796,-$40fbc822,$3019b034
	dc.l  $3ed4ce61,$316a94a2,-$415ab9c7,-$41736262,$3e759bab,$43e53d8f,-$41ba34e5,$2f7546d4,$3e15d120,-$32bf6c6c,-$421a55c6
	dc.l  $291041ce,$3db55430,-$4b76ddfc,-$427aef88,-$7ec0a2a,$3d545dde,$12ef150f,0,0,$40220072,$314ac6
	dc.l  $3ff75cc5,-$e648624,$3f88f889,$1a909d63,-$40aa1393,-$33a91ad0,$3f232b3f,$7141b69b,-$410f4f34,$da4c6d9,$3ebcf134
	dc.l  -$14615299,-$417703ec,$7cb9ac26,$3e5578dc,-$6a05af5a,-$41dda1b6,$581c577e,$3def4682,$5d1ee8e4,-$42458613,$e3dd51a
	dc.l  $3d864d8d,$72de5a21,-$42ac9413,-$3f57bc86,$3d2045a1,-$109ef8cf,0,0,$4026006f,$15cfa4,$3ff7aeaa
	dc.l  -$2cb0102c,$3f80c8c9,$52dea0be,-$40b7c9d4,$6049ce05,$3f116a29,$5c8070a5,-$41270491,$3d6c8e79,$3ea1de61,-$7cefb74f
	dc.l  -$419682f3,$62f5bd80,$3e322040,-$8ebab29,-$42064bd3,-$3f194874,$3dc22b53,-$51bb7212,-$427666cf,-$25b2fe1c,$3d51fa22
	dc.l  -$30554796,-$42e624f8,$63a8426a,$3ce21e24,-$6ed479,0,0,$402a006c,$7537cd,$3ff7e78b,$3fa5b227
	dc.l  $3f781751,$1edac6de,-$40c28695,-$3bf196c4,$3f01fea1,$5895d15f,-$413a1248,-$7cbd0029,$3e8aab4b,$4d27a272,-$41afd0ca
	dc.l  -$33b9a273,$3e139a66,-$6891ec04,-$42284e32,-$5e7c01ea,$3d9c9446,$5e9e3532,-$429ecdf9,$6a4345f5,$3d24a5ea,-$709e0861
	dc.l  -$4316c47a,-$795d255d,$3cae449d,-$613a6ef0,0,0,$402e0061,$62f29a,$3ff81154,-$4270f3b,$3f721f42
	dc.l  -$2be0c056,-$40ccc198,$726c9a03,$3ef46794,$1b8832e2,-$414a6591,$e88566f,$3e76d68c,$560af034,-$41c7e499,$908e7c
	dc.l  $3df96860,$4cfd3d19,-$42454342,$41033212,$3d7c17a8,$50fee3b8,-$42c28886,-$338c47f0,$3cfedc1f,-$54dd2b74,-$433f9f58
	dc.l  -$4277e702,$3c812473,-$45db10ba,0,0,$4032002e,$3fb185,$3ff83eab,$316f42a3,$3f693446,$11744f23
	dc.l  -$40d9aa84,$4ea226d8,$3ee3c529,-$20619276,-$415e8471,$1a42f924,$3e5ee338,-$7b14b319,-$41e4be38,-$a043eab,$3dd80722
	dc.l  -$2a641da9,-$426ad749,$18360d8b,$3d529ce0,-$62584b6d,-$42efa81b,-$725338e9,$3cccaee4,-$6d9aaa7a,-$4375be93,-$6b806501
	dc.l  $3c4724a7,$7bdca60b,0,0,$40360020,$717e62,$3ff867ee,-$6032b342,$3f60e3d5,$4c33a306,-$40e77be5
	dc.l  $11f104a6,$3ed1c7ca,-$4e3abe63,-$41763a1e,$6d30d1b4,$3e42aa8d,$69d796b1,-$4204fb07,-$7ad4dcf5,$3db38aae,$699c0132
	dc.l  -$4293c0b8,-$378a172c,$3d2466a6,-$527e8db0,-$43228fee,-$26ec5a2c,$3c953a5d,-$24e7b150,-$43b081b9,-$1df0da9d,$3c06c738
	dc.l  $5451d407,0,0,$403a0020,$1a2389,$3ff88486,$1bb0a59d,$3f583331,-$334addfc,-$40f2428d,$14c0df51
	dc.l  $3ec243e2,-$791e80de,-$41899358,$4c2495ed,$3e2b8468,$6acda902,-$421f2005,$1f1535d3,$3d94afd1,$4a9e6fd,-$42b6a729
	dc.l  -$4cc2feed,$3cff0aa3,$3246fc5f,-$434d014e,-$4dc3d54d,$3c673cc5,$2fd026d7,-$43e2fdd3,$27fd0127,$3bd1c864,$1fbca0ea
	dc.l  0,0,$403e0041,$782b4d,$3ff89980,-$1cf6d38c,$3f522edb,-$5fc215f4,-$40fca07f,-$197b3073,$3eb4a223
	dc.l  $7a4cb241,-$419a0854,$eb8e408,$3e176110,-$4b0212dc,-$423720b0,$2bcfc47,$3d7a7372,$33de3a30,-$42d3e177,-$33a73927
	dc.l  $3cdde1b2,$27e6a73,-$437042b5,$29eb047,$3c40d9d7,-$7b40af47,-$440dd543,$71227af3,$3ba353d1,-$4d1ca7a3,0
	dc.l  0,$4042006c,$1f3298,$3ff8b03e,$5ed991ba,$3f494284,-$7b7dfff8,-$410990f1,$15a6a54b,$3ea3eb3a,$38b5bb23
	dc.l  -$41ae5198,$7b9437b4,$3dff61cd,$5a78f6e4,-$4254285a,-$14daa567,$3d58b213,$3dacce1,-$42fa19e0,-$43c29009,$3cb369f5
	dc.l  -$2abc2830,-$439ece88,$1be10772,$3c0e7583,-$3e85ff34,-$4443cd0e,-$5f59d1ab,$3b6928ab,-$e689744,0,0
	dc.l  $4046006c,$492d9c,$3ff8c4e9,-$ebc9a50,$3f40ea12,-$39edb5d8,-$41176970,-$21875589,$3e91de5f,$6c7e4e1c,-$41c60897
	dc.l  $1baf74fc,$3de2dd20,$6e0a273a,-$427498cf,-$f0aa562,$3d33e68c,-$22d2322,-$432319c6,$75cdb0f8,$3c84fadd,$20954401
	dc.l  -$43d18ddc,$7aaf5869,$3bd61784,$31a85a69,-$447f7e3d,$1cfe6df9,$3b281301,$4ac57be5,0,0,$404a0020
	dc.l  $ebeb19,$3ff8d339,$451b1719,$3f383a10,-$574a5c97,-$412231a8,-$7e82362c,$3e825531,-$22d5450d,-$41d9736e,-$7f32d748
	dc.l  $3dcbbb51,$4178be5e,-$428ef30b,-$712d2652,$3d14f6db,-$50fe6488,-$43463a17,-$1d9947a1,$3c5fae57,-$6271f4f,-$43fc88be
	dc.l  $3110a058,$3ba7eba8,$5332d88a,-$44b1fc65,-$718624a2,$3af27f4c,-$1373e04e,0,0,$404e0072,$a60f9b
	dc.l  $3ff8ddb9,-$4e7c3025,$3f323280,-$1b4db09b,-$412c9899,$10e60ef5,$3e74b04b,$446ce3ff,-$41e9f190,$37d90954,$3db7832c
	dc.l  $681ac3d4,-$42a6f015,$41fe42bf,$3cfab62e,$5f5b85bb,-$43638871,$60c67e73,$3c3e55bf,-$5ca86af7,-$441fd721,-$7ad40716
	dc.l  $3b81374f,$4d9ddfe0,-$44dd5e9e,$3e29ceec,$3ac3e7dc,-$64b95441,0,0,$40520069,$29c32e,$3ff8e91a
	dc.l  $18188ff,$3f29464a,-$6af94841,-$41398a41,-$30c505b3,$3e63f51e,-$6d86c777,-$41fe446d,$6a45f178,$3d9f8286,-$3df31e1b
	dc.l  -$42c4019f,-$7989fd8e,$3cd8de45,-$16ce61b5,-$4389e8d6,-$146bd6b9,$3c139f28,-$35781aff,-$444e95f4,$1d60c978,$3b4eec26
	dc.l  $1fb38b48,-$45134aca,-$52dd04bc,$3a89aef0,-$2bd8901e,0,0,$40560067,$52ba2f,$3ff8f371,$e867082
	dc.l  $3f20ebc7,-$3a4b8702,-$4147647e,$b6b3e3d,$3e51e458,$332d4dd3,-$4215fb98,-$a5eeed6,$3d82ea56,$7e866dcb,-$42e47f3a
	dc.l  -$708963f9,$3cb3fe6f,$6f5c8b41,-$43b2ee68,$6130d01f,$3be5215c,$59ddb3b0,-$44814acb,$4cd5060,$3b165113,-$3c66e9e3
	dc.l  -$454f4b88,$5b95c90a,$3a486830,-$5c106433,0,0,$405a0068,$b2a0e,$3ff8fa99,-$34d4d3b3,$3f183b42
	dc.l  -$339b80cd,-$41522e64,$6cfdbdaa,$3e4258bd,-$4390da21,-$42296ca4,$76bab1b5,$3d6bc74f,-$6d6feb11,-$42fee906,-$ad2617e
	dc.l  $3c9506ef,$658bdc2c,-$43d62117,-$132e00d3,$3bbfd445,-$8d5dee6,-$44ac6c8d,-$280a0344,$3ae814d4,$28063109,-$4581bf38
	dc.l  -$587a630,$3a12ade7,-$2f02577c,0,0,$405e0074,$69a68a,$3ff8ffd9,-$161edbc7,$3f123376,-$1fe3eff3
	dc.l  -$415c968c,-$33628d,$3e34b3f3,$63b113dd,-$4239ebb5,$86cf0f7,$3d578beb,$73f200b7,-$4316e3a4,$2bc56725,$3c7ac73e
	dc.l  $441412a1,-$43f371b2,-$6bcf237f,$3b9e7365,-$346b63d6,-$44cfc42a,-$850eb20,$3ac14f35,$4ce9aae4,-$45ad4044,-$6a9dbaab
	dc.l  $39e40d67,$533b0b28,0,0,$40620073,$49059a,$3ff9058a,$60d2fd52,$3f09471e,$15944ce3,-$416988bc
	dc.l  $36383972,$3e23f768,-$10425df1,-$424e4156,-$1d3fa2d5,$3d3f8a49,-$676eae1f,-$4333f85d,$228851be,$3c58e8e6,-$88e2e7b
	dc.l  -$4419dcfb,-$6ab62b8b,$3b73ac13,$7eced647,-$44fe882a,$65692740,$3a8f0920,$43053976,-$45e32ad0,$478caa20,$39a9d0d8
	dc.l  $6b63e11d,0,0,$40660020,$e80865,$3ff90ab5,-$4aec353c,$3f00eca0,$5fd047b0,-$41776258,-$57d65cfd
	dc.l  $3e11e6b9,-$3e32428f,-$4265f6ba,-$4f4a148f,$3d22ef0d,-$6ea6c1fa,-$4354766e,$435ae54d,$3c340667,-$1dc1e22a,-$4442e049
	dc.l  -$7c381b3f,$3b452da4,$7c5251cd,-$453135c3,$6750d766,$3a5662f7,$4e869590,-$461f3c12,-$22f31624,$39687da0,-$2246681f
	dc.l  0,0,$406a0072,$27f3b5,$3ff90e4a,$7977e89f,$3ef83b9e,$426e5a32,-$41822d77,$a78950,$3e0259b7
	dc.l  $67646c19,-$42796ace,$1d43855,$3d0bca83,$317aff77,-$436ee65f,-$11c480e2,$3c150b29,-$48d3e9cd,-$44661a8c,-$5303a0bf
	dc.l  $3b1fde29,$4cd6acc,-$455c6539,$74dbeefd,$3a281f82,$4c245028,-$4651af64,$46b69e13,$3932b99c,-$43a2fd0b,0
	dc.l  0,$406e0065,$bc23a4,$3ff910ea,-$7cdf4cf2,$3ef233c7,$3146896d,-$418c95ea,-$613ea98b,$3df4b508,$55536ed0
	dc.l  -$4289ea05,$d1e2475,$3cf78e64,-$416ef7a4,-$4386e02c,$2c3ca351,$3bfacbf2,-$78a29c35,-$44836b7b,-$43d9e034,$3afe7b70
	dc.l  -$640e8950,-$457fbf0c,$acf3125,$3a0155a0,$1d4e21d6,-$467d3829,-$416b68ff,$390416d2,-$6f15b7c0,0,0
	dc.l  $40720073,$4d63c6,$3ff913c2,-$30b035d1,$3ee94759,-$2b2d2d5,-$41998852,-$4dbb687b,$3de3f806,-$546164f7,-$429e4083
	dc.l  $2b165bf4,$3cdf8c54,-$1c381b1c,-$43a3f5f1,$3daaed82,$3bd8ebab,-$73dfa145,-$44a9d9e8,-$384b456d,$3ad3af6a,-$6a1d62c6
	dc.l  -$45ae849b,$8ff100a,$39cf1098,$3e56b959,-$46b32299,$7b372abf,$38c9d957,-$3fc06cc0,0,0,$40760065
	dc.l  $1db0f86,$3ff91658,-$5cfcfcb9,$3ee0ec51,$11a29687,-$41a762f1,$2ab30330,$3dd1e637,-$22af9466,-$42b5f784,-$873bf1b
	dc.l  $3cc2ee7b,-$6c6b9e5b,-$43c47733,-$7a3f1195,$3bb405ee,-$4170f3ff,-$44d2e0ca,-$5c5c8a86,$3aa52d74,-$6571bb52,-$45e135b7
	dc.l  -$19a2caf5,$3996632e,-$1bfdb661,-$46ef3b92,-$6fe68962,$388882ff,$28cb997f,0,0,$407a0072,$1a278eb
	dc.l  $3ff91822,-$1cdbbe00,$3ed83bb9,-$39693c1e,-$41b22d34,-$43983a0c,$3dc259fc,-$20b3c94c,-$42c96a4e,$40140ba0,$3cabcb60
	dc.l  $27039f80,-$43dee5a9,$37a37904,$3b950c48,-$6f9b09f8,-$44f618d3,-$e4a3dd7,$3a7fe0c0,-$40998129,-$460c634f,$649efbae
	dc.l  $3968224a,-$38a84ac8,-$4721ab48,$ef6c5ab,$3852bc8a,-$33729854,0,0,$407e0076,$1db7c6c,$3ff91972
	dc.l  -$11e4c22f,$3ed233c2,$170b3a23,-$41bc95ea,-$42bbe0a9,$3db4b514,$481fe344,-$42d9e9e6,-$507d8cfe,$3c978ea1,$555dce90
	dc.l  -$43f6dfc8,-$6be5fe7e,$3b7acc8b,$68275f32,-$45136a9f,-$3e14d718,$3a5e7ca0,-$45ee041b,-$462fbe41,-$2c85a0b2,$394156a8
	dc.l  -$14f62372,-$474d36cb,-$77b53eec,$3824192d,-$1ff3414b,0,0,$40820065,$1049630,$3ff91adf,$adb246f
	dc.l  $3ec94790,$47367ac1,-$41c98803,-$4521568d,$3da3f86c,$37caf984,-$42ee400a,-$3fe8eac3,$3c7f8d6a,-$11e3852a,-$4413f4be
	dc.l  -$11a89999,$3b58ecf7,-$3920804f,-$4539d888,-$56335c28,$3a33b0d9,$69373fda,-$465e8322,$4df3180d,$390f13a5,-$672fe80b
	dc.l  -$47831f65,-$5c277c38,$37e9db77,-$21e25f62,0,0,$40860064,$18fcf61,$3ff91c29,-$5651614,$3ec0ec59
	dc.l  $5244f7fb,-$41d762da,$d5cc748,$3d91e652,-$365306b6,-$4305f74a,-$7afe09e0,$3c62eeb5,$4f369a51,-$443476c4,$3a43af4f
	dc.l  $3b340655,$57669848,-$4562e011,-$28ff703d,$3a052e18,$3f1ae2c7,-$4691349b,-$105d0ec5,$38d66422,$4c191e8a,-$47bf3abc
	dc.l  $2b6ae395,$37a88456,-$177f9a07,0,0,$408a002e,$b7873c,$3ff91d0f,$e5cdfa0,$3eb83c3f,$6c305e23
	dc.l  -$41e22c39,-$4f1866ad,$3d825ace,$3f7f728f,-$43196907,-$4e0f1301,$3c4bcd4b,-$5e711e3b,-$444ee443,-$47450aeb,$3b150e48
	dc.l  -$3925c2bc,-$45861602,$286abf87,$39dfe4a8,-$745a25a5,-$46bc60a0,$46988679,$38a82602,-$430ac8a5,-$47f1a636,$7fb220e2
	dc.l  $3772bd46,$6be4b0f6,0,0,$408e0020,$1bb6e9b,$3ff91db7,$14a71c5d,$3eb2342e,$554fb07e,-$41ec953b
	dc.l  -$204d2c7,$3d74b610,$76f14b4c,-$4329e892,-$2898560e,$3c379059,-$42b0c8b5,-$4466dd9e,-$219bce3a,$3afacf36,-$4b4fdc59
	dc.l  -$45a36762,-$2c82371d,$39be8082,$979454d,-$46dfbbf4,-$2250f06a,$38815967,$282d4e26,-$481d3393,-$82279df,$374419c4
	dc.l  -$3fc0a577,0,0,$40920020,$1c788ec,$3ff91e6d,$21c21537,$3ea94855,-$2cd72eb9,-$41f986fa,-$39c65ca5
	dc.l  $3d63f9a8,$4c5d2cdf,-$433e3ea8,$1efdfb8a,$3c1f9061,$6aa6c663,-$4483f1a6,$4f16dbb1,$3ad8f020,-$677d1d5c,-$45c9d55a
	dc.l  $5c1e65eb,$3993b401,$4af54eca,-$470e8008,-$5be43ee2,$384f19f4,$34e02e25,-$48531938,-$1defeff5,$3709dbff,-$20c6f088
	dc.l  0,0,$40960054,$5260cd,$3ff91f12,-$5ac6c88a,$3ea0ec73,-$60f4ae01,-$420762a0,-$597d5982,$3d51e68c
	dc.l  -$35c85b00,-$4355f6de,$1f5121f3,$3c02ef15,$16a61cc5,-$44a4761f,-$103cdd4a,$3ab406e1,-$49add3c4,-$45f2df27,-$252f8f1c
	dc.l  $39652ed8,-$370af42d,-$47413361,-$7facf9c6,$38166525,$5605b602,-$488f39ea,-$7fdc07ae,$36c884ac,-$12fef5ee,0
	dc.l  0,$409a0061,$258913c,$3ff91f85,$3615bb73,$3e983be2,$fa85e6c,-$42122ce4,$65cf524f,$3d425a42,-$6ced27b2
	dc.l  -$436969dc,$2899c2e2,$3bebcc12,$3de6476e,-$44bee522,$26b21a88,$3a950d10,$4f99baaf,-$461617b0,$4aabe929,$393fe260
	dc.l  -$65c70b31,-$476c6229,-$6ab970be,$37e823ea,-$1fd44a1a,-$48c1a8fd,-$5637a122,$3692bd75,$41cda0d4,0,0
	dc.l  $409e0062,$2bd4393,$3ff91fd9,$391d6b8b,$3e9233df,$352ec730,-$421c95b9,-$162f5d79,$3d34b55d,-$1ae020d1,-$4379e97f
	dc.l  -$42f0517d,$3bd78f2b,-$7c348504,-$44d6df14,$57fa57e1,$3a7acd70,$105c8ac1,-$46336981,-$70101260,$391e7e00,-$8638fb2
	dc.l  -$478fbd6a,$63292100,$37c157ae,$2aebe12,-$48ed358f,-$7720ad3b,$366419ea,$6f117daf,0,0,$40a2006c
	dc.l  $10ae917,$3ff92034,$427eca57,$3e894781,$4c665ad3,-$42298815,-$2941957e,$3d23f859,$7b09eb02,-$438e401c,-$540ba29e
	dc.l  $3bbf8d4a,$3537f5fc,-$44f3f4d9,$ecdbb0a,$3a58ece1,-$6219a8ae,-$4659d898,-$57514a5a,$38f3b0cf,-$421114c0,-$47be8326
	dc.l  -$493a20e7,$378f13a3,$788af9b9,-$49231f53,$437978c,$3629dc21,-$d60f724,0,0,$40a60065,$309e93f
	dc.l  $3ff92086,-$2248094,$3e80ec59,-$20d50376,-$423762d8,-$38bb48e8,$3d11e656,-$16f6721c,-$43a5f740,-$4be12df9,$3ba2eec0
	dc.l  -$74e63b8b,-$451476ad,$4597b88b,$3a34066b,-$3265cbfe,-$4682dfe6,$2127dfb,$38c52e3e,-$4d39ad9c,-$47f13456,$679a3fff
	dc.l  $3756645d,-$3727f588,-$495f3a87,-$34a7e2b0,$35e884c2,$3ed2061a,0,0,$40aa0073,$201f8f7,$3ff920c0
	dc.l  $46052d5a,$3e783bc0,-$ef6efa7,-$42422d21,-$7a4b884c,$3d025a10,-$277dc678,-$43b96a29,-$22280b1,$3b8bcba2,$3fe012c1
	dc.l  -$452ee572,$1b0dc092,$3a150ca0,$3983cc28,-$46a6184b,-$348c4959,$389fe18e,$51b26a71,-$481c62b7,-$1b409821,$37282329
	dc.l  $3f36d294,-$4991a9fe,$cfed6cb,$35b2bd80,-$11d7de2,0,0,$40ae0020,$50de96f,$3ff920ea,$4456f5c8
	dc.l  $3e72342f,-$780d444f,-$424c9538,-$71b8912d,$3cf4b615,$7f7d1d0,-$43c9e88a,$284297fb,$3b779064,-$535508e2,-$4546dd8e
	dc.l  $6e6a5cb0,$39facf4c,$f5db7bc,-$46c36745,$4f709f36,$387e80a7,$2f3a5e7e,-$483fbbdc,-$5b753464,$37015985,$22ab87aa
	dc.l  -$49bd336c,$10e20c57,$358419f3,-$3898c969,0,0,$40b20067,$1ec00fd,$3ff92117,-$34de5fc0,$3e69478f
	dc.l  -$6f2f46a8,-$42598802,-$92e3223,$3ce3f870,$44d3d2ce,-$43de4002,$206a720b,$3b5f8d80,-$3377abbd,-$4563f4a1,-$80a19a8
	dc.l  $39d8ed1b,-$4428fe61,-$46e9d85d,$141f9d21,$3853b109,-$4d97ec6f,-$486e82ed,-$593f0814,$36cf1417,$2b1d10ad,-$49f31ee2
	dc.l  $391cada7,$3549dc2a,$61f32168,0,0,$40b60065,$818376,$3ff92141,$290f5e5f,$3e60ec59,-$216070c
	dc.l  -$426762d7,$1ddf2344,$3cd1e657,$4ff16481,-$43f5f73f,-$6e5efd39,$3b42eec1,$6a70f8ec,-$458476ac,-$c765564,$39b4066d
	dc.l  $5d4ba15d,-$4712dfe4,-$2a9d03fe,$38252e41,$333d464b,-$48a13452,-$3ecf41d2,$36966461,-$7cdf71fd,-$4a2f3a83,$1331ef63
	dc.l  $350884c7,-$3a5c9e77,0,0,$40ba006e,$5613249,$3ff9215d,-$32f9c4b6,$3e583bca,$582fbc9e,-$42722d10
	dc.l  -$dadb122,$3cc25a1f,$32285973,-$44096a12,$26a38e12,$3b2bcbc3,$19254bae,-$459ee55b,-$3940ef14,$39950cc1,-$5ba36c95
	dc.l  -$4736181c,$3bfcebdc,$37ffe1ce,$c1958a9,-$48cc628b,$2f852a5c,$36682364,-$3c049210,-$4a61a9ae,$17d1e302,$34d2bd83
	dc.l  -$34d6e60c,0,0,$40be0065,$64d22b3,$3ff92172,-$32788e56,$3e5233db,-$2568a5a8,-$427c95be,-$4a726821
	dc.l  $3cb4b556,-$46a75451,-$4419e988,$7c022163,$3b178f20,$1012133f,-$45b6df22,-$6b21e6aa,$397acd5f,-$1dccb821,-$47536994
	dc.l  -$23758313,$37de7deb,-$587b7ba9,-$48efbd76,$61884cae,$364157a0,$49b83f77,-$4a8d359e,$77fc9731,$34a419f6,$173467d4
	dc.l  0,0,$40c20072,$53ee2a1,$3ff92189,-$7007962a,$3e494770,-$4e6d0f61,-$4289882b,-$2b35b72b,$3ca3f83f
	dc.l  -$722bbf28,-$442e4038,$9ca3a5f,$3aff8d0d,$7e7a58df,-$45d3f518,$7ad9287c,$3958eca2,$6f890738,-$4779d8d7,-$21c73117
	dc.l  $37b3b092,$299a42b5,-$491e8362,-$1b2c1e0f,$360f132a,-$78e86d90,-$4ac31fc8,-$1e32e1c0,$3469dc2c,$3ee85879,0
	dc.l  0,$40c60061,$1d03540,$3ff9219e,$3e98d298,$3e40ec60,$29e97055,-$429762ca,-$66372d18,$3c91e664,$67c16fb2
	dc.l  -$4445f727,$6805355b,$3ae2eed6,$3f4ee92f,-$45f47688,$595f3937,$3934068a,-$2cb40bd5,-$47a2dfb3,$1b5786ae,$37852e68
	dc.l  $3f6e98a2,-$49513413,$4e0a091f,$35d66494,-$554a683c,-$4aff3a5b,-$5661fa44,$342884c9,$8eba4dd,0,0
	dc.l  $40ca0074,$e9be43a,$3ff921ac,-$6f488a7b,$3e383bbf,$1ec70227,-$42a22d24,$3e272d18,$3c825a0e,$391080d0,-$44596a2c
	dc.l  $fab3574,$3acbcb9c,-$66a190eb,-$460ee576,$28f0c4e4,$39150c9a,-$2b5b6d9c,-$47c61852,-$75385902,$375fe184,-$4d2dc952
	dc.l  -$497c62bd,-$6b41733a,$35a82320,-$3273a14b,-$4b31aa09,$368ffce5,$33f2bd84,$2951157f,0,0,$40ce0065
	dc.l  $ff11b2e,$3ff921b7,$10e5e076,$3e3233db,-$2d3962f4,-$42ac95be,-$54e15195,$3c74b556,-$528bdfaa,-$4469e988,$70141525
	dc.l  $3ab78f20,$5d39a52,-$4626df22,-$71a05960,$38facd5f,-$1e17bd2e,-$47e36994,-$1aa9de37,$373e7deb,-$4337d819,-$499fbd76
	dc.l  $745c3c3c,$358157a0,$6705e411,-$4b5d359e,-$5c81e8fd,$33c419f6,$536c11eb,0,0,$40d20064,$38779e7
	dc.l  $3ff921c2,$71f20614,$3e294798,$c07bc4c,-$42b987f6,$4dd80987,$3c63f87d,-$40fcab93,-$447e3ff3,$24948a98,$3a9f8da0
	dc.l  -$13ebcdc1,-$4643f47f,$5f947b0a,$38d8ed3d,-$3ce112de,-$4809d83b,$334f8312,$3713b12b,$76d7a1a7,-$49ce82cc,-$40caa35f
	dc.l  $354f145a,$5a9a5eb5,-$4b931ea1,-$4e1f3157,$3389dc2c,-$164056d4,0,0,$40d60020,$db3ed69,$3ff921cc
	dc.l  -$371aeaae,$3e20ecc4,$19b50cd1,-$42c761f0,-$5c4dbd7d,$3c51e737,-$2b06155b,-$4495f5a7,-$30bdba73,$3a82f025,-$47fa68ee
	dc.l  -$4664744f,-$60ff92db,$38b40863,-$5fec64c,-$4832dcad,$566855a2,$36e530d9,-$569633cb,-$4a01302b,-$2fd1f50b,$351667c6
	dc.l  $3420196c,-$4bcf37d4,-$277f9ac7,$334884c8,-$5b86e1a6,0,0,$40da0031,$e3848f,$3ff921d3,-$de7bf9e
	dc.l  $3e183c3c,$1ef047a4,-$42d22c3d,$1922daf,$3c425acb,-$7046e24d,-$44a96909,$57a31c63,$3a6bcd4a,-$36d6cdc1,-$467ee441
	dc.l  $a264aa,$38950e4d,$3bcbdb07,-$485615f8,$14b84ba,$36bfe4bb,$b898e41,-$4a2c6090,$309f3291,$34e8261b,-$b06a015
	dc.l  -$4c01a60c,$7607c2f8,$3312bd84,-$1cdcac4f,0,0,$40de002f,$8d5664e,$3ff921d9,$32578ea1,$3e12341d
	dc.l  $63b15b04,-$42dc9555,-$6d81bec0,$3c34b5eb,-$20f0a1e9,-$44b9e8c1,$5bf354ae,$3a57901e,-$5cd0e382,-$4696dde5,$6775bc16
	dc.l  $387acee2,$20d50ae2,-$487367c4,$61e3c44e,$369e8010,-$227cc87,-$4a4fbc34,-$55cdb19e,$34c1591d,$44242620,-$4c2d33e5
	dc.l  -$a5d28de,$32e419f6,-$4401e6d1,0,0,$40e20032,$d3884eb,$3ff921de,-$1d33d80a,$3e094824,$6247c26c
	dc.l  -$42e9873b,$68e14919,$3c23f95b,$7d1f925c,-$44ce3efd,-$7d6d82f4,$3a3f8fae,$7a007f9a,-$46b3f25e,$5cdeb0c4,$3858ef67
	dc.l  $601b3db9,-$4899d612,-$36d8e75e,$3673b34d,-$3c8cff2c,-$4a7e80b6,-$47f63d5f,$348f1895,$26b0c2,-$4c631a87,-$68305899
	dc.l  $32a9dc2c,$47727ab9,0,0,$40e60037,$3ca2b72,$3ff921e4,$eaceac8,$3e00eca0,-$399f953d,-$42f7623d
	dc.l  -$6e4e4d67,$3c11e6ed,$19b9fb6c,-$44e5f62f,-$f3e4665,$3a22efaf,$25c517be,-$46d47518,$6b1bbfa4,$383407bc,-$3fe0b6db
	dc.l  -$48c2ddbf,-$4b04cfb7,$36452ffc,-$5e553277,-$4ab1318c,$39a80dad,$345666a5,$465bb121,-$4c9f38b8,$2f422f04,$326884c9
	dc.l  $555df798,0,0,$40ea002f,$1cc8e7de,$3ff921e7,-$5cd33804,$3df83c3f,-$5a29cddb,-$43022c37,-$7b528ceb
	dc.l  $3c025ad0,-$18374b0d,-$44f96901,-$6f610503,$3a0bcd56,-$1188b5ba,-$46eee439,-$46c0b3e3,$38150e59,-$7f69bc0f,-$48e615e8
	dc.l  -$1594340,$361fe4d2,$47112673,-$4adc6081,-$150f2c84,$34282631,-$734315ba,-$4cd1a5ef,$51c21e03,$3232bd83,-$2b971768
	dc.l  0,0,$40ee0039,$1490c797,$3ff921ea,$435394bd,$3df23411,$330da5ba,-$430c9568,$11bb4d58,$3bf4b5d0
	dc.l  $229da021,-$4509e8e6,$61300376,$39f78fef,$4e5cc9ae,-$4706de20,-$7e761039,$37face9a,$55161710,-$4903681a,$3b588c89
	dc.l  $35fe7faa,-$18fc5c0a,-$4affbc70,-$38d95401,$340158d6,$7bf15161,-$4cfd3437,-$bfc44a2,$320419f6,$5716b65f,0
	dc.l  0,$40f20035,$de4c36d,$3ff921ed,$1b8a89c7,$3de9481b,-$ca810e9,-$43198746,$2a8704f7,$3be3f94e,$2a03cbee
	dc.l  -$451e3f0c,-$4b3ee7a2,$39df8f8e,-$1a4c1825,-$4723f27f,-$622d99f4,$37d8ef46,$1c9e72a7,-$4929d633,-$79b1277d,$35d3b32c
	dc.l  -$f200906,-$4b2e80d6,-$5ccfb94b,$33cf1853,-$a5a7f3f,-$4d331ac6,-$76c45e9b,$31c9dc2c,$42528682,0,0
	dc.l  $40f60020,$1f6bdc67,$3ff921ef,-$4e938f99,$3de0ecc3,-$1079e30,-$432761f0,$69be6c3a,$3bd1e737,-$62d956ce,-$4535f5a7
	dc.l  $6a6ee8c7,$39c2f025,$609e0eb0,-$4744744f,$bada0b7,$37b40863,-$7f9d88b7,-$4952dcae,-$6f3fb99e,$35a530d9,$ac9e4cc
	dc.l  -$4b61302c,-$2bbfab2a,$339667c5,$6dabc637,-$4d6f37d4,$37ef0e88,$318884c7,-$56710c60,0,0,$40fa0062
	dc.l  $1e59ab52,$3ff921f1,$7bcbcafb,$3dd83be0,-$6ef3ef0f,-$43322ce7,-$22f00d6,$3bc25a40,-$1c385cf1,-$454969de,$31d7e81
	dc.l  $39abcc0f,-$453be91d,-$475ee524,-$2e247116,$37950d0f,$1a36fa32,-$497617b1,-$764c680d,$357fe260,-$2b3bffd9,-$4b8c6228
	dc.l  -$69d7bd6f,$336823ed,$208bbdfd,-$4da1a8f8,-$4efae074,$3152bd83,-$38827cf8,0,0,$40fe0079,$7d6dfd8
	dc.l  $3ff921f2,-$3421eecc,$3dd233c3,-$66d96d61,-$433c95e5,-$163d9349,$3bb4b51f,-$6f298fb0,-$4559e9d2,-$90f0834,$39978ec1
	dc.l  -$2dc37a3,-$4776df97,-$7300f656,$377accd1,$43dde711,-$49936a3f,-$393f1958,$355e7d20,-$69b0ccb,-$4bafbded,-$7ad9e02c
	dc.l  $33415713,-$2cbab3f1,-$4dcd3641,-$453e23de,$312419f6,-$2ede8db5,0,0,$41020020,$16195fe,$3ff921f4
	dc.l  $37df0aca,$3dc94857,$13dcdef0,-$434986f8,-$260144,$3ba3f9ab,-$6835d52e,-$456e3ea4,-$7cdd37b0,$397f906c,$57be93c4
	dc.l  -$4793f199,$4172fb46,$3758f02f,$63b18036,-$49b9d54a,-$346f77d4,$3533b413,$257d2851,-$4bde7ff5,-$581ce95f,$330f1a1c
	dc.l  $22c8a105,-$4e03190c,-$31150619,$30e9dc2d,$41e8a65d,0,0,$41060052,$1f151b01,$3ff921f5,-$7d1a787d
	dc.l  $3dc0ec77,$1297834d,-$43576298,-$69b96302,$3b91e694,-$1eb98763,-$4585f6cf,-$73c2cf7d,$3962ef23,$2cfcbe96,-$47b47606
	dc.l  -$18e59477,$373406f7,$583e02f7,-$49e2df02,-$4defa782,$35052ef7,-$493600c8,-$4c11332e,-$2a5eefa3,$32d66550,$44b6320e
	dc.l  -$4e3f39c6,$3302d8d9,$30a884c7,-$4fc9350a,0,0,$410a006f,$feeebbf,$3ff921f6,$680a6f62,$3db83bc8
	dc.l  $707f8b01,-$43622d13,$73ac5bed,$3b825a1c,$58c3558f,-$45996a17,-$34506802,$394bcbbc,-$4aeebf80,-$47cee55f,$37c03038
	dc.l  $37150cbb,$45987944,-$4a061825,$78d96104,$34dfe1c2,$25667189,-$4c3c6293,$2f01d515,$32a82359,-$2492a016,-$4e71a9bd
	dc.l  -$645529bb,$3072bd84,$5a32baba,0,0,$410e0067,$1c4f295f,$3ff921f7,$100e92c6,$3db233d9,$57c6bf0d
	dc.l  -$436c95c2,-$4c2396e6,$3b74b551,$a952915,-$45a9e990,-$12589033,$39378f16,$6aeb721a,-$47e6df2e,-$635d353d,$36facd51
	dc.l  $551a5839,-$4a2369a5,$72d6e1c0,$34be7dd7,$13889593,-$4c5fbd82,$57be17d5,$32815792,$19685301,-$4e9d35ae,$1207c941
	dc.l  $304419f6,$1135ace6,0,0,$41120065,$8dd3eb5,$3ff921f7,-$39e0c1ca,$3da94795,$2f490e6f,-$437987fa
	dc.l  $7d7c8e90,$3b63f879,$3a9a25bd,-$45be3ff8,$20842896,$391f8d96,$3a871718,-$4803f48a,$4a7d3c27,$36d8ed32,-$7d24334c
	dc.l  -$4a49d847,-$b15dd24,$3493b120,$5fff32d4,-$4c8e82d7,-$15dc56d4,$324f1444,$6ad2b992,-$4ed31eb6,$6aa81d23,$3009dc2c
	dc.l  -$55243bd1,0,0,$41160072,$1b35143b,$3ff921f8,$6b9914e2,$3da0ec45,-$21d68540,-$43876303,$3d07f58d
	dc.l  $3b51e62c,-$3562f503,-$45d5f78c,$4f116be6,$3902ee7e,$8732767,-$4824771e,-$520150a6,$36b4060e,$728c11f3,-$4a72e07f
	dc.l  -$62315739,$34652dc3,-$1a5f5244,-$4cc1351a,$6df0bb87,$321663bd,-$1243ba11,-$4f0f3b05,-$645cbbda,$2fc884c7,-$12b34a01
	dc.l  0,0,$411a0020,$3ffb6195,$3ff921f8,-$21e027c0,$3d983c5b,$5b01a1d9,-$43922c04,-$54754497,$3b425afa
	dc.l  -$1fe40cd7,-$45e968c0,$2213a5ff,$38ebcdb6,$4acb4265,-$483ee3f4,$2fc1423e,$36950eb9,-$3322dc91,-$4a961562,$54053f35
	dc.l  $343fe588,-$6bb9e2ee,-$4cec6005,$4fe5964b,$31e826da,-$3ac4f12f,-$4f41a50d,-$5b0a2256,$2f92bd82,$73e0ec16,0
	dc.l  0,$411e0053,$2ab16d9c,$3ff921f9,$3227f4e7,$3d9233f1,-$74c5fa53,-$439c959b,$6c1d28d8,$3b34b588,$1b5817ee
	dc.l  -$45f9e946,$58731031,$38d78f74,$631f8584,-$4856deb9,-$742adba8,$367acddf,-$202d6675,-$4ab368fa,$7da5cc0d,$341e7ea1
	dc.l  -$3c795ebf,-$4d0fbd0b,$3dfffcee,$31c1581e,-$5a7bd0bb,-$4f6d350c,-$1053d192,$2f6419f5,-$75eab936,0,0
	dc.l  $4122006d,$127fe0e3,$3ff921f9,-$72cd7f29,$3d89477e,-$63bd2e3e,-$43a98818,$64cef9aa,$3b23f855,-$700d976e,-$460e4020
	dc.l  -$7fc02dc7,$38bf8d41,-$4ca4935e,-$4873f4e2,-$5c20dbd1,$3658ecd9,$79f5c40e,-$4ad9d8a0,-$120ec728,$33f3b0c8,-$7a540832
	dc.l  -$4d3e832c,$c754103,$318f1396,$5da29ed6,-$4fa31f5f,-$76a1cee3,$2f29dc2b,-$183014cc,0,0,$41260069
	dc.l  $1d4f3608,$3ff921f9,-$2011f72e,$3d80ec53,-$4cbaff7d,-$43b762e5,$6a7f6a15,$3b11e64a,$d681d74,-$4625f757,-$7e5751ae
	dc.l  $38a2eeac,$74dd1e91,-$489476cf,$73f47daa,$3634064f,-$155ff0d1,-$4b02e014,-$42a6c1fa,$33c52e1a,$6cd8cf3b,-$4d713490
	dc.l  -$27d54404,$3156642f,$6f0aead,-$4fdf3aab,$2c3e9371,$2ee884c7,-$339215e1,0,0,$412a0074,$312fb736
	dc.l  $3ff921fa,$1935fbd3,$3d783bbe,-$1f617ee2,-$43c22d25,-$332e79f4,$3b025a0d,-$22c38973,-$46396a2d,-$7bd08e35,$388bcb9b
	dc.l  -$31fe2e97,-$48aee577,-$672a1822,$36150c9a,$c9d9f72,-$4b261853,$7983e60c,$339fe183,$422dff5f,-$4d9c62be,-$61526c2a
	dc.l  $3128231f,-$7836ca9e,-$5011aa0b,-$7fe243b8,$2eb2bd83,$9bb85fa,0,0,$412e0068,$bc2e84d,$3ff921fa
	dc.l  $4336cf07,$3d7233d8,$35388753,-$43cc95c4,-$1d00389c,$3af4b54e,$757bf715,-$4649e993,$7c3d9fc2,$38778f12,$2c6b60e
	dc.l  -$48c6df33,$20ce3b74,$35facd4a,-$5a2258e1,-$4b4369ad,$6d794a5e,$337e7dcd,-$6db10f8f,-$4dbfbd88,-$3b9d4b29,$3101578b
	dc.l  -$7f8fc686,-$503d35b6,$6eb05a4d,$2e8419f6,-$52fb05d7,0,0,$3fc10043,$1588f0,$3fc0e6ef,-$5c48cba2
	dc.l  $3fef71fc,-$194d073b,-$403f9554,$3749f966,-$402cd40f,$32c36c1a,$3fbf2544,$606fb20,$3fc35ae8,-$3c2b8f29,-$40432cf5
	dc.l  -$311cf35a,-$404b474b,$4f335425,$3fb9f916,$58d5c641,$3fa39f66,-$225aaebb,0,0,0,0
	dc.l  0,0,0,0,0,0,$3fc3006f,$6df059,$3fc2dd2a,$4ebd084e,$3fef4f5c
	dc.l  $4bbedce5,-$403dcf16,-$32bb11f3,-$402d5698,-$443b7267,$3fc10801,$eab20a2,$3fc1f403,$3155acf6,-$404116c8,-$445e3858
	dc.l  -$404e9a3b,$18e29fe8,$3fbb1a24,-$47224b4d,$3f98f972,-$4c642d77,0,0,0,0,0
	dc.l  0,0,0,0,0,$3fc50070,$6f49c8,$3fc4d0f4,-$454e1cee,$3fef293e,-$2784011d
	dc.l  -$403c15c2,-$7fa886a1,-$402de44d,$5e9a3211,$3fc2604c,$3d904900,$3fc07668,$14cc3372,-$403fafc3,-$4e49e04d,-$405411fb
	dc.l  -$3458de9,$3fbbbadb,-$fba6e5c,$3f85a00c,$30fd788b,0,0,0,0,0,0
	dc.l  0,0,0,0,$3fc70079,$22f66c,$3fc6c249,-$3f43fed8,$3feeffbb,-$534eefe7,-$403a6a2e
	dc.l  $66d1af07,-$402e7c4e,$774cbca0,$3fc39a0d,$164581f5,$3fbdcce7,$513d7d3a,-$403f0496,-$ff05e06,-$405b07fc,$1b22a195
	dc.l  $3fbbddf3,-$3b33b5b2,-$4098eafe,-$1f1426d0,0,0,0,0,0,0,0
	dc.l  0,0,0,$3fc90072,$756c3a,$3fc8b0dc,-$4d794924,$3feed2f1,$4a4052dd,-$4038cd59,-$6cb591a3
	dc.l  -$402f1d99,$183b6da5,$3fc4b3f4,-$46e1098b,$3fba9191,$766c8f93,-$403e8a21,$b171cc5,-$4063eec9,-$531076a1,$3fbb8913
	dc.l  -$770c81aa,-$407066f1,-$145c189e,0,0,0,0,0,0,0,0
	dc.l  0,0,$3fcb0069,$71c056,$3fca9c87,-$5d7487d8,$3feea2fc,-$22a156bd,-$4037400a,-$452e3a66,-$402fc731
	dc.l  -$61da2723,$3fc5ad26,-$23a9a619,$3fb743d0,$42a0fb8a,-$403e3fcd,$5226e452,-$40730d8d,$64f33b04,$3fbac495,-$3f46b770
	dc.l  -$40645914,-$13e48da5,0,0,0,0,0,0,0,0,0
	dc.l  0,$3fcd0067,$45e284,$3fcc8521,-$7cc113db,$3fee6ffd,$4a5a3632,-$4035c2f5,-$3bcc71b8,-$4030f01f,-$56e04ba
	dc.l  $3fc6851b,-$2b5ca243,$3fb3ec7d,$5290f754,-$403e2442,-$32c8b4fe,-$40aa2eda,-$2f3dfc2f,$3fb99b0f,-$6b2a1ef5,-$405ce406
	dc.l  $56122dbc,0,0,0,0,0,0,0,0,0,0
	dc.l  $3fcf0068,$221903,$3fce6a76,-$3475887f,$3fee3a14,$5e44bd2,-$403456bf,$476f85fd,-$40325e52,-$1e0dd789,$3fc73b9d
	dc.l  $2839479,$3fb09436,-$166954a9,-$403e357a,-$17fd7bc9,$3f862748,-$5710aac8,$3fb818e5,$25ccf350,-$40585624,$675d8d95
	dc.l  0,0,0,0,0,0,0,0,0,0,$3fd10074
	dc.l  $3ce8ac,$3fd09e31,-$d484198,$3fede3fd,$2ab41664,-$403254ea,-$2b52d424,-$403496c6,-$53ad8315,$3fc80f1b,$4977db6a
	dc.l  $3fa73de3,$1f724454,-$403e9d6e,-$5f996b77,$3f9c261a,$4c45acd,$3fb54c02,$78d0b595,-$405301f4,-$460f4f56,0
	dc.l  0,0,0,0,0,0,0,0,0,$3fd30020,$367c52
	dc.l  $3fd27854,-$76ff5159,$3fed6872,-$562fae51,-$402ff3f4,-$27f239b8,-$4037a400,-$13b5fc63,$3fc8b766,$3d1e6738,$3f952325
	dc.l  -$58e24bb3,-$403fa7d8,-$1e66565b,$3fa7a3ff,-$20339603,$3fb0e266,-$91ec645,-$404f5f0c,-$4577ad0f,0,0
	dc.l  0,0,0,0,0,0,0,0,$3fd50031,$2aed5f,$3fd44ad0
	dc.l  $7395ff03,$3fece3ae,$e8c10b2,-$402ee238,-$151d75f9,-$403abf22,-$53a0f738,$3fc8e58b,-$6d208da1,-$409d8efd,-$76d65ff1
	dc.l  -$404250db,-$2957a324,$3faee48f,$5ed99a2b,$3fa82edf,-$2a42deb1,-$404ecb90,-$7fdb2f33,0,0,0
	dc.l  0,0,0,0,0,0,0,$3fd70039,$3b4898,$3fd614b6,$7e2da002
	dc.l  $3fec56f3,$169361f7,-$402df5cc,$468fef9f,-$403dd8e7,$7db89b4c,$3fc8a48f,-$506d73f6,-$4068c5a2,$159c1ddf,-$4045fb52
	dc.l  $2de9ab59,$3fb1de9a,-$504a7a88,$3f9d52ff,$3e461d05,-$404f8d0c,-$3eec0b79,0,0,0,0
	dc.l  0,0,0,0,0,0,$3fd90039,$60886,$3fd7d591,-$4071484c,$3febc36b
	dc.l  $25320e46,-$402d2e50,-$f6bfac,-$4041c884,$775e32a6,$3fc80222,-$74d87e7d,-$405b621d,-$16659f52,-$404a0cc4,-$67d514c7
	dc.l  $3fb32435,$63a6c61e,$3f877969,$6e93b0c5,-$4052a492,-$603bf014,0,0,0,0,0
	dc.l  0,0,0,0,0,$3fdb0035,$76f0,$3fd98d02,$4349e9ba,$3feb2a39,$361b40d9
	dc.l  -$402c8acd,-$56c98b49,-$4047abf8,-$64ae7b81,$3fc70d74,$5d22d3bd,-$4053f1a0,-$17ede859,-$404e47e7,$677b5659,$3fb36287
	dc.l  $531b9b5e,-$4094633e,-$2e049fad,-$40578272,-$1d7f9a5c,0,0,0,0,0,0
	dc.l  0,0,0,0,$3fdd002c,$96ab4,$3fdb3ab5,-$61234fbe,$3fea8c78,$eddb650,-$402c09d7
	dc.l  -$1b4ad3ef,-$404d4990,-$6d84801c,$3fc5d654,$7951cbe6,-$404f0b5f,-$133de770,-$4054ef6d,$4135f595,$3fb2c4eb,-$6690d005
	dc.l  -$40709edd,-$2c4c2bc5,-$405d06ad,$abbb1b9,0,0,0,0,0,0,0
	dc.l  0,0,0,$3fdf0020,$3047ba,$3fdcde6d,$2e8cb77a,$3fe9eb34,$7399e19b,-$402ba99b,-$584008ac
	dc.l  -$4055255c,-$3c683708,$3fc46c68,$773d54b0,-$404ce267,$5370b371,-$405ce2e2,$755b868a,$3fb17ca3,-$4859cf34,-$4067380b
	dc.l  -$2f7aad60,-$406539f5,-$353b70c1,0,0,0,0,0,0,0,0
	dc.l  0,0,$3fe1004d,$880b4,$3fdf4155,$256dcb62,$3fe8f4b1,$300440e2,-$402b51e4,$112d2a62,-$4067324f
	dc.l  -$7312556d,$3fc20e02,$2b8bd981,-$404af69c,$7bf0278a,-$406f0355,$3cac71da,$3fad7844,-$36935622,-$405fa2a2,-$1a7f7630
	dc.l  -$4078b6b1,$7fc7a9cc,0,0,0,0,0,0,0,0,0
	dc.l  0,$3fe3006f,$1337a9,$3fe125af,-$531301c1,$3fe7a879,-$581823e5,-$402b3a52,$526d5f39,$3f7fe4b0,-$256e1dbf
	dc.l  $3fbd66db,$72c6af8c,-$404a76ae,$2d76fda,$3f7442a9,-$e33ce60,$3fa4d70e,-$172108c7,-$405e9b6d,$7c49cb2d,$3f694530
	dc.l  $7c5d8888,0,0,0,0,0,0,0,0,0,0
	dc.l  $3fe50074,$10ab7a,$3fe295df,$6d80c9a8,$3fe65db8,-$3d117af3,-$402b7b03,$b69e906,$3fa1072d,-$66bc234d,$3fb6d353
	dc.l  -$20043c4a,-$404bad41,-$1aa94004,$3f939b9b,$4080d6fa,$3f992a78,$7ad4a154,-$406189db,-$517d0aa3,$3f8644a9,-$10a67770
	dc.l  0,0,0,0,0,0,0,0,0,0,$3fe7006f
	dc.l  $1ef11a,$3fe3f188,-$172470b5,$3fe51947,-$6e8c7b84,-$402c0051,$3ee6d879,$3faae811,-$36cd018c,$3fb0ced2,-$457de1d7
	dc.l  -$404deebd,$2219eab3,$3f9b8866,$7fe03836,$3f875547,$3496c438,-$4068b5e8,-$5d2097e6,$3f8b3c27,-$db7bd72,0
	dc.l  0,0,0,0,0,0,0,0,0,$3fe90072,$2697f
	dc.l  $3fe5393c,$45d05db1,$3fe3ded1,-$2d34ee18,-$402cb841,$7ae026ff,$3fb0fc02,-$41cb6d3b,$3fa72b15,$44f8feaf,-$40515c2d
	dc.l  $467d7c82,$3f9e5e40,-$19210f7e,$3f5ec49d,$1a3b43c9,-$40706eed,-$60ebd124,$3f89565e,$fc78313,0,0
	dc.l  0,0,0,0,0,0,0,0,$3feb006f,$8cabe,$3fe66da7
	dc.l  $fb0f615,$3fe2b109,-$753d24b1,-$402d9328,-$54a28634,$3fb351b1,$cd9f5ea,$3f9cf7a9,$311dbc38,-$40570969,-$75fc2084
	dc.l  $3f9dbb7d,-$68fbe743,-$408ed5bd,-$7fee8a2c,-$407d5465,-$3df87351,$3f843d50,-$503c048a,0,0,0
	dc.l  0,0,0,0,0,0,0,$3fed006c,$b150b,$3fe78fa7,$974a539
	dc.l  $3fe191ae,-$3f8cc63b,-$402e8400,-$3f5c75e0,$3fb4ad76,-$117188c5,$3f8e1fea,-$7409037f,-$405c6182,-$19cc858,$3f9b071d
	dc.l  $5d783aa,-$408154ed,$71255bb8,-$408e24d9,-$26e21246,$3f7cdf30,$18795224,0,0,0,0
	dc.l  0,0,0,0,0,0,$3fef0061,$10a462,$3fe8a028,$923f255,$3fe081cd
	dc.l  -$45626f66,-$402f8052,$603a3621,$3fb54488,$189e2b98,$3f7142ee,-$511646cb,-$4062382d,-$3023f3f6,$3f975216,$19a208a3
	dc.l  -$407dfcdf,$5a291984,-$40ad8514,-$31f8d3b3,$3f728c9e,-$8b5ddc9,0,0,0,0,0
	dc.l  0,0,0,0,0,$3fef0061,$10a462,$3fe8a028,$923f255,$3fe081cd,-$45626f66
	dc.l  -$402f8052,$603a3621,$3fb54488,$189e2b98,$3f7142ee,-$511646cb,-$4062382d,-$3023f3f6,$3f975216,$19a208a3,-$407dfcdf
	dc.l  $5a291984,-$40ad8514,-$31f8d3b3,$3f728c9e,-$8b5ddc9,0,0,0,0,0,0
	dc.l  0,0,0,0

@__atan_data:
	dc.l  _atan_data+62*4

