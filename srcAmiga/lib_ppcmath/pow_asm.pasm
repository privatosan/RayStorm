	.globl _pow_ppc__r

	.section .text

_pow_ppc__r:
pow:
	stwu  r1,-80(r1)
	stfd  f1,48(r1)
	mffs  f4
	lis   r11,19264
pow_restart:
	stfd  f1,32(r1)
//  lis   r3,0
//     RELOC: 00000016 R_PPC_ADDR16_HA log_table_base
	lwz   r4,32(r1)
//  addi  r3,r3,0
//     RELOC: 0000001e R_PPC_ADDR16_LO log_table_base
	rlwinm   r5,r4,20,24,31
	lfd   f0,-1200(r3)
	clrlwi   r7,r4,12
	stfd  f2,56(r1)
	oris  r7,r7,16368
	stw   r7,32(r1)
	mtfsf 255,f0
	addi  r6,r5,1
	lfd   f9,32(r1)
	rlwinm   r6,r6,3,20,27
	lfdx  f8,r3,r6
	addi  r7,r5,-107
	lfd   f7,-1208(r3)
	rlwinm   r7,r7,1,31,31
	lfd   f6,-1216(r3)
	fmsub f13,f8,f9,f0
	rlwinm.  r8,r4,12,21,31
	stfd  f4,64(r1)
	lfd   f5,-1224(r3)
	fmul  f12,f8,f9
	subfc r9,r7,r8
	lfd   f3,-1248(r3)
	add   r5,r9,r11
	lfd   f4,-1232(r3)
	fmadd f0,f13,f7,f6
	beq   pow_x_denorm_zero
	addi  r7,r6,8
	lwz   r10,56(r1)
	stw   r5,40(r1)
	frsp  f12,f12
	cmpwi r8,2046
	lfs   f11,-1128(r3)
	fmul  f7,f13,f13
	rlwinm   r12,r10,12,21,31
	lfs   f10,40(r1)
	fmadd f0,f13,f0,f5
	addi  r12,r12,-960
	lfdx  f6,r3,r7
	fmsub f5,f8,f9,f12
	cmplwi   cr6,r12,127
	bgt   pow_x_inf_nan
	lfd   f8,-1240(r3)
	fmul  f9,f13,f3
	rlwinm   r6,r6,0,21,27
	lfd   f3,-1152(r3)
	fmadd f0,f13,f0,f4
	addi  r9,r9,-1022
	lfd   f4,-1160(r3)
	fsubs f10,f10,f11
	or.   r6,r6,r9
	lfd   f11,-1088(r3)
	fadd  f12,f12,f6
	lfd   f6,-1096(r3)
	fmadd f0,f13,f0,f8
	bge   cr6,pow_y_outside_range
	lfd   f8,-1080(r3)
	fmadd f5,f10,f3,f5
	lfd   f3,-1072(r3)
	fmadd f12,f10,f4,f12
	fmadd f0,f7,f0,f9
	beq   pow_special
pow_normal:
	fmadd f5,f13,f0,f5
	mr.   r4,r4
	lfd   f4,-520(r3)
	fmul  f9,f2,f5
	xor   r0,r0,r0
	fmadd f7,f2,f12,f9
	lfd   f1,-528(r3)
	fmadd f10,f7,f11,f6
	stfd  f7,72(r1)
	fmsub f0,f2,f12,f7
	b  pow_confluence
pow_special:
	fmul  f0,f0,f13
	mr.   r4,r4
	lfd   f4,-520(r3)
	fmul  f9,f2,f0
	xor   r0,r0,r0
	fmadd f7,f2,f13,f9
	lfd   f1,-528(r3)
	fmadd f10,f7,f11,f6
	stfd  f7,72(r1)
	fmsub f0,f2,f13,f7
pow_confluence:
	addi  r8,r3,-1064
	stfd  f10,32(r1)
	fsub  f10,f10,f6
	lwz   r5,72(r1)
	fadd  f9,f9,f0
	blt   pow_x_negative
pow_x_is_ok:
	rlwinm   r7,r5,12,21,31
	lwz   r6,36(r1)
	fmsub f12,f10,f8,f7
	addi  r7,r7,-970
	lfd   f5,-536(r3)
	rlwinm   r9,r6,4,23,27
	fmadd f9,f10,f3,f9
	cmplwi   cr7,r7,64
	lfdx  f8,r8,r9
	addi  r8,r8,8
	lfd   f2,-544(r3)
	fsub  f9,f9,f8
	lfd   f0,-552(r3)
	fsub  f3,f9,f12
	srawi r11,r6,5
	lfdx  f11,r8,r9
	fmul  f6,f3,f3
	bge   cr7,pow_ylogx_outside_range
	addi  r9,r11,1021
	fmadd f4,f3,f4,f1
	mr.   r0,r0
	fmadd f5,f3,f5,f2
	cmplwi   cr6,r9,2044
	fmul  f2,f6,f3
	lfd   f10,64(r1)
	fmadd f0,f6,f0,f9
	fmadd f5,f6,f4,f5
	beq+  pow_sign_now_ok
	fneg  f11,f11
pow_sign_now_ok:
	fmadd f0,f2,f5,f0
	fsub  f0,f0,f12
	fmadd f0,f0,f11,f11
	stfd  f0,32(r1)
	lwz   r5,32(r1)
	bge   cr6,pow_possible_over_under
	rlwinm   r6,r5,12,21,31
	add   r6,r11,r6
pow_result_ok:
	rlwimi   r5,r6,20,1,11
	stw   r5,32(r1)
	lfd   f1,32(r1)
	mtfsf 255,f10
	addi  r1,r1,80
	blr
pow_possible_over_under:
	rlwinm   r6,r5,12,21,31
	add.  r6,r11,r6
	addi  r7,r6,-1
	cmplwi   cr7,r7,2046
	blt-  cr7,pow_result_ok
	bgt+  pow_aix_overflow
	cmpwi cr6,r6,-52
	bgt   cr6,pow_aix_underflow
	mr.   r5,r5
	lfd   f1,-1176(r3)
	bge   pow_aix_return_zero
	fneg  f1,f1
pow_aix_return_zero:
	mtfsf 255,f10
	addi  r1,r1,80
	blr
pow_aix_underflow:
	addi  r6,r6,64
	lfd   f2,-1112(r3)
	rlwimi   r5,r6,20,1,11
	stw   r5,32(r1)
	lfd   f1,32(r1)
	fmul  f1,f1,f2
	b  pow_aix_range_returns
pow_aix_overflow:
	mr.   r5,r5
	lfd   f1,-1184(r3)
	bge   pow_aix_range_returns
	fneg  f1,f1
	b  pow_aix_range_returns
pow_x_denorm_zero:
	clrlwi   r9,r4,12
	lwz   r5,36(r1)
	or.   r5,r5,r9
	lfd   f4,-1120(r3)
	addi  r11,r11,-52
	fmul  f1,f1,f4
	lfd   f4,64(r1)
	mtfsf 255,f4
	bne   pow_restart
pow_x_zero:
	lwz   r10,56(r1)
	xor   r0,r0,r0
	lwz   r11,60(r1)
	rlwinm   r12,r10,12,21,31
	cmpwi cr7,r12,2047
	clrlwi   r9,r10,1
	or.   r9,r9,r11
	beq   cr7,pow_y_nan_inf
	cmpwi cr5,r12,1023
	cmpwi cr6,r12,1044
	beq+  pow_exact_one
	cmpwi cr7,r12,1075
	addi  r7,r12,-1011
	blt   cr5,pow_exact_zero_inf
	bge   cr6,pow_lower_word_y
	slw   r9,r10,r7
	or.   r9,r9,r11
	bne   pow_exact_zero_inf
	rlwnm r0,r10,r7,31,31
	b  pow_exact_zero_inf
pow_y_nan_inf:
	clrlwi   r9,r10,12
	or.   r9,r9,r11
	beq   pow_exact_zero_inf
	rlwinm.  r9,r10,13,31,31
	beq   pow_y_snan
pow_y_qnan:
	lfd   f1,56(r1)
	b  pow_aix_domain_returns
pow_y_snan:
	stw   r11,36(r1)
	oris  r9,r10,8
	stw   r9,32(r1)
	lfd   f1,32(r1)
	b  pow_aix_domain_returns
pow_lower_word_y:
	addi  r7,r7,-32
	bgt   cr7,pow_exact_zero_inf
	slw.  r9,r11,r7
	bne   pow_exact_zero_inf
	rlwnm r0,r11,r7,31,31
pow_exact_zero_inf:
	cmpwi cr7,r10,0
	blt   cr7,pow_dvz
	cmpwi cr6,r0,1
	beq   cr6,pow_return_exact
	fabs  f1,f1
	b  pow_return_exact
pow_aix_domain_returns:
// lis   r5,0
//    RELOC: 0000036a R_PPC_ADDR16_HA errno
// li r3,33
// stw   r3,0(r5)
//    RELOC: 00000372 R_PPC_ADDR16_LO errno
	lfd   f2,64(r1)
	mtfsf 255,f2
	addi  r1,r1,80
	blr
pow_exact_one:
	lfd   f1,-1200(r3)
pow_return_exact:
	lfd   f2,64(r1)
	mtfsf 255,f2
	addi  r1,r1,80
	blr
pow_dvz:
	lfd   f1,-1184(r3)
	rlwinm   r9,r4,1,31,31
	and.  r9,r9,r0
	beq   pow_aix_domain_returns
	fneg  f1,f1
	b  pow_aix_domain_returns
pow_x_negative:
	lwz   r11,60(r1)
	cmpwi r12,63
	cmpwi cr5,r12,84
	cmpwi cr6,r12,115
	addi  r7,r12,-51
	blt   pow_x_negative_error
	bge   cr5,pow_lower_word
	slw   r9,r10,r7
	or.   r9,r9,r11
	rlwnm r0,r10,r7,31,31
	beq   pow_x_is_ok
	b  pow_x_negative_error
pow_lower_word:
	addi  r7,r7,-32
	bgt-  pow_x_is_ok
	slw.  r9,r11,r7
	rlwnm r0,r11,r7,31,31
	beq   pow_x_is_ok
pow_x_negative_error:
	lfd   f1,-1168(r3)
	b  pow_aix_domain_returns
pow_y_outside_range:
	lwz   r11,60(r1)
	cmpwi cr7,r12,1087
	lfd   f4,-1200(r3)
	cmpwi cr6,r12,-960
	fabs  f5,f1
	clrlwi   r9,r10,12
	fcmpu cr5,f5,f4
	or.   r9,r9,r11
	bne   cr6,pow_y_not_zero_denorm
	beq   pow_exact_one
pow_inexact_one:
	mr.   r9,r4
	blt   pow_x_negative_error
pow_inexact_one_ok:
	lfd   f1,-1200(r3)
	lwz   r11,68(r1)
	lfd   f4,64(r1)
	mtfsf 255,f4
	addi  r1,r1,80
	blr
pow_y_not_zero_denorm:
	bne   cr7,pow_major_over_under
	beq   pow_y_is_infinity
	rlwinm.  r9,r10,13,31,31
	beq-  pow_y_snan
	b  pow_y_qnan
pow_y_is_infinity:
	lfd   f1,-1176(r3)
	beq   cr5,pow_domain_error
	mr.   r10,r10
	lfd   f2,-1184(r3)
	blt   cr5,pow_x_less
	blt   pow_return_exact
	fmr   f1,f2
	b  pow_return_exact
pow_domain_error:
	lfd   f1,-1168(r3)
	b  pow_aix_domain_returns
pow_x_less:
	bgt   pow_return_exact
	fmr   f1,f2
	b  pow_return_exact
pow_major_over_under:
	cmpwi cr7,r12,63
	mr.   r10,r10
	blt-  cr7,pow_inexact_one
	blt   pow_major_underflow
pow_major_overflow:
	lfd   f1,-1184(r3)
pow_aix_range_returns:
// lis   r5,0
//    RELOC: 000004a2 R_PPC_ADDR16_HA errno
// li r3,34
// stw   r3,0(r5)
//    RELOC: 000004aa R_PPC_ADDR16_LO errno
	lfd   f2,64(r1)
	mtfsf 255,f2
	addi  r1,r1,80
	blr
pow_major_underflow:
	lfd   f1,-1176(r3)
	b  pow_aix_range_returns
pow_ylogx_outside_range:
	lwz   r11,76(r1)
	clrlwi   r9,r5,12
	cmpwi cr7,r7,53
	cmpwi cr6,r5,0
	mr.   r9,r9
	cmpwi cr5,r0,0
	blt   cr7,pow_ylogx_small
	blt   cr6,pow_ylogx_unf
	lfd   f1,-1184(r3)
	beq   cr5,pow_ylogx_ovf_ok
	fneg  f1,f1
pow_ylogx_ovf_ok:
	b  pow_aix_range_returns
pow_ylogx_small:
	bne   pow_inexact_one_ok
	lfd   f1,-1200(r3)
	beq   cr5,pow_ylogx_small_ok
	fneg  f1,f1
pow_ylogx_small_ok:
	b  pow_return_exact
pow_ylogx_unf:
	lfd   f1,-1176(r3)
	beq   cr5,pow_ylogx_unf_ok
	fneg  f1,f1
pow_ylogx_unf_ok:
	b  pow_aix_range_returns
pow_x_inf_nan:
	lwz   r11,60(r1)
	clrlwi   r9,r4,12
	lwz   r6,36(r1)
	cmpwi cr7,r12,1087
	or.   r9,r9,r6
	clrlwi   r7,r10,12
	beq   pow_x_infinity
	rlwinm.  r9,r4,13,31,31
	bne   pow_x_is_qnan
	oris  r9,r4,8
	stw   r9,32(r1)
	lfd   f1,32(r1)
	b  pow_aix_domain_returns
pow_x_is_qnan:
	bne   cr7,pow_return_x_qnan
	or.   r7,r7,r11
	beq   pow_return_x_qnan
	rlwinm.  r9,r10,12,0,0
	beq-  pow_y_snan
pow_return_x_qnan:
	b  pow_aix_domain_returns
pow_x_infinity:
	bne   cr7,pow_infinitynumeric
	or.   r7,r7,r11
	beq   pow_infinityinfinity
	rlwinm.  r9,r10,13,31,31
	beq-  pow_y_snan
	b  pow_y_qnan
pow_infinityinfinity:
	mr.   r10,r10
	blt   pow_return_zero
	fabs  f1,f1
	b  pow_return_exact
pow_return_zero:
	lfd   f1,-1176(r3)
	b  pow_return_exact
pow_infinitynumeric:
	clrlwi   r9,r10,1
	or.   r9,r9,r11
	beq   pow_exact_one
	cmpwi r12,63
	xor   r0,r0,r0
	cmpwi cr5,r12,84
	cmpwi cr6,r12,115
	blt   pow_exact_inf_zero
	bge   cr5,pow_lower_word_zero
	addi  r7,r12,-51
	slw   r9,r10,r7
	or.   r9,r9,r11
	bne   pow_exact_inf_zero
	rlwnm r0,r10,r7,31,31
	b  pow_exact_inf_zero
pow_lower_word_zero:
	addi  r7,r12,-83
	bgt   cr6,pow_exact_inf_zero
	slw.  r9,r11,r7
	bne   pow_exact_inf_zero
	rlwnm r0,r11,r7,31,31
pow_exact_inf_zero:
	cmpwi cr7,r10,0
	blt   cr7,pow_give_a_zero
	cmpwi cr6,r0,1
	beq-  cr6,pow_return_exact
	fabs  f1,f1
	b  pow_return_exact
pow_give_a_zero:
	lfd   f1,-1176(r3)
	rlwinm   r9,r4,1,31,31
	and.  r9,r9,r0
	beq   pow_return_exact
	fneg  f1,f1
	b  pow_return_exact
	lfd   f1,-1104(r3)
	addi  r1,r1,80
	blr

