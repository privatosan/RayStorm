	XDEF _exp__r
	XDEF _pow__r
	XDEF _log__r
	XDEF _log10__r

	XDEF _exp
	XDEF _pow
	XDEF _log
	XDEF _log10

	SECTION  ":0",Code

;----------------------------------------- EXP

_exp__r:
_exp:
	stwu  r1,-80(r1)
	stfd   f1,32(r1)
	mffs   f9
	lw    r3,@__log_table_base
	lwz   r10,32(r1)
	lfd   f0,-1200(r3)
	rlwinm   r12,r10,12,21,31
	lfd   f11,-1088(r3)
	addi  r9,r12,-970
	lfd   f6,-1096(r3)
	mtfsf 255,f0
	cmplwi   cr6,r9,64
	lfd   f8,-1080(r3)
	bge   cr6,exp_no_computation
	addi  r8,r3,-1064
	lfd   f3,-1072(r3)
	fmadd f10,f1,f11,f6
	lfd   f4,-520(r3)
	lfd   f7,-528(r3)
	stfd  f10,32(r1)
	fsub  f10,f10,f6
	lwz   r6,36(r1)
	fnmsub   f12,f10,f8,f1
	rlwinm   r9,r6,4,23,27
	lfd   f5,-536(r3)
	srawi r11,r6,5
	lfdx  f8,r8,r9
	addi  r8,r8,8
	lfd   f2,-544(r3)
	fmsub f10,f10,f3,f8
	lfd   f0,-552(r3)
	fadd  f3,f12,f10
	lfdx  f11,r8,r9
	fmul  f6,f3,f3
	addi  r9,r11,1021
	fmadd f4,f3,f4,f7
	cmplwi   cr6,r9,2044
	fmadd f5,f3,f5,f2
	fmul  f2,f6,f3
	fmadd f0,f6,f0,f10
	fmadd f5,f6,f4,f5
	fmadd f0,f2,f5,f0
	fadd  f0,f0,f12
	fmadd f0,f0,f11,f11
	stfd  f0,32(r1)
	lwz   r5,32(r1)
	bge   cr6,exp_possible_over_under
	rlwinm   r6,r5,12,21,31
	add   r6,r11,r6
exp_result_ok:
	rlwimi  r5,r6,20,1,11
	stw   r5,32(r1)
	lfd   f1,32(r1)
	mtfsf 255,f9
	addi r1,r1,80
	blr
exp_possible_over_under:
	rlwinm r6,r5,12,21,31
	add. r6,r11,r6
	addi r7,r6,-1
	cmplwi  cr7,r7,2046
	blt-   cr7,exp_result_ok
	bgt+   exp_aix_overflow
	cmpwi  cr6,r6,-52
	bgt cr6,exp_aix_denorm
	mr. r5,r5
	lfd f1,-1176(r3)
	bge exp_aix_return_zero
	fneg   f1,f1
exp_aix_return_zero:
	mtfsf   255,f9
	addi  r1,r1,80
	blr
exp_aix_denorm:
	addi   r6,r6,64
	lfd  f2,-1112(r3)
	rlwimi  r5,r6,20,1,11
	stw  r5,32(r1)
	lfd f1,32(r1)
	fmul   f1,f1,f2
	b   exp_aix_range_returns
exp_aix_overflow:
	lfd  f1,-1184(r3)
exp_aix_range_returns:
	lfd f2,64(r1)
	mtfsf 255,f9
	addi  r1,r1,80
	blr
exp_no_computation:
	cmpwi cr6,r9,0
	stfd   f9,64(r1)
	lwz r11,36(r1)
	cmpwi  cr7,r12,2047
	lwz   r8,68(r1)
	cmpwi cr5,r10,0
	bge   cr6,exp_inf_large_nan
exp_zero_or_small:
	clrlwi r9,r10,1
	or.  r9,r9,r11
	blt+ exp_zero
exp_small:
	fmr   f1,f0
	mtfsf  255,f9
	addi   r1,r1,80
	blr
exp_zero:
	fmr f1,f0
exp_exact:
	mtfsf 255,f9
	addi   r1,r1,80
	blr
exp_inf_large_nan:
	beq cr7,exp_inf_nan
	blt  cr5,exp_unf
exp_ovf:
	lfd  f1,-1184(r3)
	b  exp_aix_range_returns
exp_unf:
	fsub f1,f0,f0
	b  exp_aix_range_returns
exp_inf_nan:
	clrlwi r9,r10,12
	or.  r9,r9,r11
	bne  exp_nan
exp_inf:
	bge  cr5,exp_exact
	fsub  f1,f0,f0
	b  exp_exact
exp_nan:
	rlwinm. r9,r10,0,12,12
	beq   exp_snan
exp_qnan:
	b   exp_aix_domain_returns
exp_snan:
	stw r11,36(r1)
	oris r9,r10,8
	stw  r9,32(r1)
	lfd  f1,32(r1)
	b   exp_aix_domain_returns
exp_aix_domain_returns:
	lfd   f2,64(r1)
	mtfsf   255,f2
	addi r1,r1,80
	blr
	stwu r1,-80(r1)
	stfd f1,32(r1)
	mffs f9
	lwz  r10,32(r1)
	lfd  f0,-1200(r3)
	rlwinm  r12,r10,12,21,31
	lfd  f11,-1088(r3)
	addi r9,r12,-999
	lfd  f6,-1096(r3)
	mtfsf   255,f0
	cmplwi  cr6,r9,32
	lfd  f8,-1080(r3)
	bge  cr6,sexp_no_computation
	addi r8,r3,-1064
	lfd  f3,-1072(r3)
	fmadd   f10,f1,f11,f6
	lfd  f4,-520(r3)
	lfd  f7,-528(r3)
	stfd f10,32(r1)
	fsub f10,f10,f6
	lwz  r6,36(r1)
	fnmsub  f12,f10,f8,f1
	rlwinm  r9,r6,4,23,27
	lfd  f5,-536(r3)
	srawi   r11,r6,5
	lfdx f8,r8,r9
	addi r8,r8,8
	lfd  f2,-544(r3)
	fmsub   f10,f10,f3,f8
	lfd  f0,-552(r3)
	fadd f3,f12,f10
	lfdx f11,r8,r9
	fmul f6,f3,f3
	fmadd   f4,f3,f4,f7
	fmadd   f5,f3,f5,f2
	fmul f2,f6,f3
	fmadd   f0,f6,f0,f10
	fmadd   f5,f6,f4,f5
	fmadd   f0,f2,f5,f0
	fadd f0,f0,f12
	fmadd   f0,f0,f11,f11
	stfd f0,32(r1)
	lwz  r5,32(r1)
	rlwinm  r6,r5,12,21,31
	add  r6,r11,r6
	rlwimi  r5,r6,20,1,11
	stw  r5,32(r1)
	lfd  f1,32(r1)
	frsp f1,f1
	mtfsf   255,f9
	addi r1,r1,80
	blr
sexp_aix_domain_returns:
	lfd  f2,64(r1)
	mtfsf  255,f2
	addi   r1,r1,80
	blr
sexp_aix_range_returns:
	lfd   f2,64(r1)
	mtfsf   255,f9
	addi r1,r1,80
	blr
sexp_no_computation:
	cmpwi   cr6,r9,0
	stfd  f9,64(r1)
	lwz   r11,36(r1)
	cmpwi cr7,r12,2047
	lwz  r8,68(r1)
	cmpwi   cr5,r10,0
	bge  cr6,sexp_inf_large_nan
sexp_zero_or_small:
	clrlwi   r9,r10,1
	or. r9,r9,r11
	blt+   sexp_zero
sexp_small:
	fmr  f1,f0
	mtfsf 255,f9
	addi  r1,r1,80
	blr
sexp_zero:
	fmr   f1,f0
sexp_exact:
	mtfsf   255,f9
	addi  r1,r1,80
	blr
sexp_inf_large_nan:
	beq   cr7,sexp_inf_nan
	blt cr5,sexp_unf
sexp_ovf:
	lfd f1,-1184(r3)
	b sexp_aix_range_returns
sexp_unf:
	fsub   f1,f0,f0
	b sexp_aix_range_returns
sexp_inf_nan:
	clrlwi   r9,r10,12
	or. r9,r9,r11
	bne sexp_nan
sexp_inf:
	bge cr5,sexp_exact
	fsub f1,f0,f0
	b sexp_exact
sexp_nan:
	rlwinm.   r9,r10,0,12,12
	beq  sexp_snan
sexp_qnan:
	b  sexp_aix_domain_returns
sexp_snan:
	stw   r11,36(r1)
	oris   r9,r10,8
	stw r9,32(r1)
	lfd f1,32(r1)
	b  sexp_aix_domain_returns


;----------------------------------------- POW

_pow__r:
_pow:
	stwu  r1,-80(r1)
	stfd  f1,48(r1)
	mffs  f4
	lis   r11,19264
pow_restart:
	stfd  f1,32(r1)
	lw    r3,@__log_table_base
	lwz   r4,32(r1)
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

;----------------------------------------- LOG

_log__r:
_log:
	stwu  r1,-80(r1)
	stfd   f1,48(r1)
	mffs   f4
	lis r11,19264
log_restart:
	stfd   f1,32(r1)
	lw    r3,@__log_table_base
	lwz  r4,32(r1)
	rlwinm r5,r4,20,24,31
	lfd f0,-1200(r3)
	clrlwi r7,r4,12
	oris   r7,r7,16368
	stw r7,32(r1)
	mtfsf  255,f0
	addi   r6,r5,1
	lfd f9,32(r1)
	rlwinm r6,r6,3,20,27
	lfdx   f8,r3,r6
	addi   r7,r5,-107
	lfd f7,-1208(r3)
	rlwinm r7,r7,1,31,31
	lfd f6,-1216(r3)
	fmsub  f13,f8,f9,f0
	rlwinm.   r8,r4,12,21,31
	stfd   f4,64(r1)
	lfd f5,-1224(r3)
	cmpwi  cr6,r4,0
	fmul   f12,f8,f9
	subfc  r9,r7,r8
	lfd f3,-1248(r3)
	add r5,r9,r11
	lfd f4,-1232(r3)
	fmadd  f0,f13,f7,f6
	beq log_x_denorm_zero
	addi   r7,r6,8
	stw r5,40(r1)
	frsp   f12,f12
	cmpwi  r8,2046
	lfs f11,-1128(r3)
	fmul   f7,f13,f13
	blt cr6,log_x_negative
	rlwinm r6,r6,0,21,27
	lfs f10,40(r1)
	fmadd  f0,f13,f0,f5
	addi   r9,r9,-1022
	lfdx   f6,r3,r7
	fmsub  f5,f8,f9,f12
	bgt log_x_inf_nan
	lfd f8,-1240(r3)
	fmul   f9,f13,f3
	or. r6,r6,r9
	lfd f3,-1152(r3)
	fmadd  f0,f13,f0,f4
	lfd f4,-1160(r3)
	fsubs  f10,f10,f11
	fadd   f12,f12,f6
	lfd f6,64(r1)
	fmadd  f0,f13,f0,f8
	fmadd  f5,f10,f3,f5
	fmadd  f12,f10,f4,f12
	fmadd  f0,f7,f0,f9
	beq log_special
log_normal:
	fmadd   f5,f13,f0,f5
	fadd  f1,f12,f5
	mtfsf 255,f6
	addi  r1,r1,80
	blr
log_special:
	fmadd  f1,f13,f0,f13
	mtfsf   255,f6
	addi r1,r1,80
	blr
log_x_denorm_zero:
	clrlwi r9,r4,12
	lwz  r5,36(r1)
	or.  r5,r5,r9
	lfd  f4,-1120(r3)
	addi   r11,r11,-52
	fmul   f1,f1,f4
	lfd f4,64(r1)
	mtfsf  255,f4
	bne log_restart
log_x_zero:
	lfd  f1,-1192(r3)
log_aix_domain_returns:
;   lis  r5,0
;      RELOC: 00000b1a R_PPC_ADDR16_HA errno
;   li r3,33
;   stw   r3,0(r5)
;      RELOC: 00000b22 R_PPC_ADDR16_LO errno
	lfd   f2,64(r1)
	mtfsf   255,f2
	addi r1,r1,80
	blr
log_x_negative:
	lfd f1,-1168(r3)
	b log_aix_domain_returns
log_x_inf_nan:
	lwz  r6,36(r1)
	clrlwi   r9,r4,12
	or.   r9,r9,r6
	beq+  log_x_infinity
	rlwinm. r9,r4,13,31,31
	bne  log_x_is_qnan
	oris r9,r4,8
	stw  r9,32(r1)
	lfd  f1,32(r1)
	b log_aix_domain_returns
log_x_is_qnan:
	b log_aix_domain_returns
log_x_infinity:
	mr. r4,r4
	blt- log_x_negative
	lfd  f2,64(r1)
	mtfsf   255,f2
	addi   r1,r1,80
	blr

;----------------------------------------- LOG10

_log10__r:
_log10:
	stwu   r1,-80(r1)
	stfd f1,48(r1)
	mffs f4
	lis  r11,19264
log10_restart:
	stfd f1,32(r1)
	lw    r3,@__log_table_base
	lwz   r4,32(r1)
	rlwinm  r5,r4,20,24,31
	lfd  f0,-1200(r3)
	clrlwi  r7,r4,12
	oris r7,r7,16368
	stw  r7,32(r1)
	mtfsf   255,f0
	addi r6,r5,1
	lfd  f9,32(r1)
	rlwinm  r6,r6,3,20,27
	lfdx f8,r3,r6
	addi r7,r5,-107
	lfd  f7,-1208(r3)
	rlwinm  r7,r7,1,31,31
	lfd  f6,-1216(r3)
	fmsub   f13,f8,f9,f0
	rlwinm. r8,r4,12,21,31
	stfd f4,64(r1)
	lfd  f5,-1224(r3)
	cmpwi   cr6,r4,0
	fmul f12,f8,f9
	subfc   r9,r7,r8
	lfd  f3,-1248(r3)
	add  r5,r9,r11
	lfd  f4,-1232(r3)
	fmadd   f0,f13,f7,f6
	beq  log10_x_denorm_zero
	addi r7,r6,8
	stw  r5,40(r1)
	frsp f12,f12
	cmpwi   r8,2046
	lfs  f11,-1128(r3)
	fmul f7,f13,f13
	blt  cr6,log10_x_negative
	rlwinm  r6,r6,0,21,27
	lfs  f10,40(r1)
	fmadd   f0,f13,f0,f5
	addi r9,r9,-1022
	lfdx f6,r3,r7
	fmsub   f5,f8,f9,f12
	bgt  log10_x_inf_nan
	lfd  f8,-1240(r3)
	fmul f9,f13,f3
	or.  r6,r6,r9
	lfd  f3,-1152(r3)
	fmadd   f0,f13,f0,f4
	lfd  f4,-1160(r3)
	fsubs   f10,f10,f11
	fadd f12,f12,f6
	lfd  f6,64(r1)
	fmadd   f0,f13,f0,f8
	lfd  f2,-1136(r3)
	fmadd   f5,f10,f3,f5
	fmadd   f12,f10,f4,f12
	lfd  f11,-1144(r3)
	fmadd   f0,f7,f0,f9
	beq  log10_special
log10_normal:
	fmadd f5,f13,f0,f5
	fmul   f7,f2,f12
	fmadd  f7,f11,f5,f7
	fmadd  f1,f11,f12,f7
	mtfsf 255,f6
	addi  r1,r1,80
	blr
log10_special:
	fmadd   f8,f11,f0,f2
	fmul  f8,f13,f8
	fmadd f1,f13,f11,f8
	mtfsf 255,f6
	addi r1,r1,80
	blr
log10_x_denorm_zero:
	clrlwi  r9,r4,12
	lwz   r5,36(r1)
	or.   r5,r5,r9
	lfd   f4,-1120(r3)
	addi r11,r11,-52
	fmul f1,f1,f4
	lfd  f4,64(r1)
	mtfsf   255,f4
	bne  log10_restart
log10_x_zero:
	lfd   f1,-1192(r3)
log10_aix_domain_returns:
;   lis   r5,0
;      RELOC: 00000cde R_PPC_ADDR16_HA errno
;   li  r3,33
;   stw r3,0(r5)
;      RELOC: 00000ce6 R_PPC_ADDR16_LO errno
	lfd f2,64(r1)
	mtfsf 255,f2
	addi  r1,r1,80
	blr
log10_x_negative:
	lfd  f1,-1168(r3)
	b  log10_aix_domain_returns
log10_x_inf_nan:
	lwz   r6,36(r1)
	clrlwi r9,r4,12
	or. r9,r9,r6
	beq+   log10_x_infinity
	rlwinm.  r9,r4,13,31,31
	bne   log10_x_is_qnan
	oris  r9,r4,8
	stw   r9,32(r1)
	lfd   f1,32(r1)
	b  log10_aix_domain_returns
log10_x_is_qnan:
	b  log10_aix_domain_returns
log10_x_infinity:
	mr.  r4,r4
	blt-  log10_x_negative
	lfd   f2,64(r1)
	mtfsf 255,f2
	addi r1,r1,80
	blr

;-------------------------------- DATA

	SECTION  ":1",Data

_log_table_base:
	dc.l  -$40200000,0,$3fd55555,$55555555,-$40300001,-$e073b,$3fc99999,-$666bb615,-$403aaa8f,$5ca0cd75,$3fc24936
	dc.l  $4add74ae,$3ff00000,0,-$100000,0,$7ff00000,0,0,0,$7ff80000,0
	dc.l  $3fe62e42,-$105c800,$3d2ef357,-$6c3898d0,$3fdbcb7b,$1526e50e,$3c695355,-$45550530,$4b4003fe,0,$43300000
	dc.l  0,$3bf00000,0,$7ff80000,0,$43380000,0,$40471547,$652b82fe,$3f962e42,-$105c611
	dc.l  -$43d54362,$3b39803e,0,0,$3ff00000,0,$3e343e0c,-$41000000,$3ff059b0,-$2b9f8232,$3e36f443
	dc.l  $72000000,$3ff0b558,$6e790fe8,$3e370dd7,-$71000000,$3ff11301,-$2e64032e,$3e379ef6,$a000000,$3ff172b8,$3e19759b
	dc.l  $3e37207f,$60000000,$3ff1d487,$33051540,$3e3696ca,-$31000000,$3ff2387a,$7010f845,$3e367af0,-$28000000,$3ff29e9d
	dc.l  -$93d8e70,$3e3687e8,-$3c000000,$3ff306fe,$bde6ae7,$3e374531,$3b000000,$3ff371a7,$38ff2130,$3e320557,-$79000000
	dc.l  $3ff3dea6,$4d7845f8,$3e331784,$49000000,$3ff44e08,$61e5314d,$3e339287,$63000000,$3ff4bfda,-$2933b837,$3e339fb8
	dc.l  $33000000,$3ff5342b,$583d6962,$3e334309,-$36000000,$3ff5ab07,-$21164daa,$3e32c7b1,$47000000,$3ff6247e,-$4e25d3df
	dc.l  $3e320f6a,-$30000000,$3ff6a09e,$6817e3ca,$3e34db2b,$1e000000,$3ff71f75,-$15315f87,$3e34f716,-$43000000,$3ff7a114
	dc.l  $75da65a4,$3e354fdb,$77000000,$3ff82589,-$64b0955b,$3e34f0b6,$63000000,$3ff8ace5,$442f5670,$3e352ebd,$4e000000
	dc.l  $3ff93737,-$4d1c17f8,$3e34f47b,$5e000000,$3ff9c491,-$7b40184c,$3e34c160,$1b000000,$3ffa5503,-$4b9f525f,$3e3410c7
	dc.l  -$57000000,$3ffae89f,-$64893c54,$3e32c735,-$7d000000,$3ffb7f76,-$b004587,$3e320717,$6000000,$3ffc199b,-$20801934
	dc.l  $3e34927f,$50000000,$3ffcb720,-$20c1b24d,$3e34ec9a,$5000000,$3ffd5818,-$209e5aa9,$3e34308b,-$36000000,$3ffdfc97
	dc.l  $35d906eb,$3e32eafb,-$51000000,$3ffea4af,-$5b17ba24,$3e320553,$16000000,$3fff5076,$5da2944f,$3fe00000,0
	dc.l  $3fc55555,$55548f7c,$3fa55555,$55545d4e,$3f811115,-$48556fa2,$3f56c172,-$728c689b,$3ff00000,0,0
	dc.l  0,$3ff059b0,-$2cea7ac0,$3d0a1d73,-$1d5b8a4c,$3ff0b558,$6cf98900,$3ceec531,$7256e308,$3ff11301,-$2feda4c0
	dc.l  $3cf0a4eb,-$40e5126d,$3ff172b8,$3c7d5140,$3d0d6e6f,-$41b9d78a,$3ff1d487,$3168b980,$3d053c02,-$23febb38,$3ff2387a
	dc.l  $6e756200,$3d0c3360,-$29271f5,$3ff29e9d,-$ae02140,$3d009612,-$175052ee,$3ff306fe,$a31b700,$3cf52de8,-$2a5b9cfa
	dc.l  $3ff371a7,$373aa9c0,$3ce54e28,-$55fa1757,$3ff3dea6,$4c123400,$3d011ada,$911f09f,$3ff44e08,$60618900,$3d068189
	dc.l  -$485fb108,$3ff4bfda,-$2ac9d600,$3d038ea1,-$342809df,$3ff5342b,$569d4f80,$3cbdf0a8,$3c49d86a,$3ff5ab07,-$22b7ac00
	dc.l  $3d04ac64,-$67f57371,$3ff6247e,-$4fc5aa80,$3cd2c7c3,-$17e40b49,$3ff6a09e,$667f3bc0,$3ce92116,$5f626cdd,$3ff71f75
	dc.l  -$1713a0c0,$3d09ee91,-$4786887b,$3ff7a114,$73eb0180,$3cdb5f54,$408fdb37,$3ff82589,-$66b33200,$3cf28acf,-$775054cb
	dc.l  $3ff8ace5,$422aa0c0,$3cfb5ba7,-$3aa5e6d3,$3ff93737,-$4f323a40,$3d027a28,$e1f92a0,$3ff9c491,-$7d5c0f80,$3cf01c7c
	dc.l  $46b071f3,$3ffa5503,-$4dc1dac0,$3cfc8b42,$4491caf8,$3ffae89f,-$66a52c80,$3d06af43,-$65974467,$3ffb7f76,-$d04a1c0
	dc.l  $3cdbaa9e,-$3df952b1,$3ffc199b,-$227aad80,$3cfc2220,-$34ed5f6e,$3ffcb720,-$23106fc0,$3d048a81,-$1a170b5b,$3ffd5818
	dc.l  -$23045b80,$3cdc9768,$16bad9b8,$3ffdfc97,$337b9b40,$3cfeb968,-$353c612d,$3ffea4af,-$5d5b6f40,$3cf9858f,$73a18f5e
	dc.l  $3fff5076,$5b6e4540,$3c99d3ea,$2dd8a18b,$3ff00000,0,-$40100000,0,$3fefc07f,$20278dc2,-$40103fc1
	dc.l  -$366e4400,$3fef81f8,$499299c5,-$40107f03,-$780b0c00,$3fef4465,-$1d5c5049,-$4010bdc9,$6db4d800,$3fef07c1,-$8d6cfe8
	dc.l  -$4010fc15,$2e6eb800,$3feecc08,$8497991,-$401139e9,-$23039400,$3fee9132,$1d5593bf,-$40117746,-$18ad9800,$3fee573b
	dc.l  $f20e307,-$4011b42e,$72e31800,$3fee1e1e,$26deb2fc,-$4011f0a4,-$2b47800,$3fede5d7,-$72d8205d,-$40122ca7,-$28bfc400
	dc.l  $3fedae43,$27a303e0,-$4012685b,$73e8ac00,$3fed776f,-$3f18ef6f,-$4012a3ae,$733e6000,$3fed4170,-$28e32273,-$4012de87
	dc.l  $1932ac00,$3fed0c79,$640b3c0e,-$401318ac,-$2d67800,$3fecd872,$58123b28,-$4013522f,$325c0000,$3feca469,-$63e9b5ce
	dc.l  -$40138c1d,$6a0c5400,$3fec7167,-$661d9100,-$4013c54d,-$615b0800,$3fec3f68,$7ec37cb,-$4013fdbf,-$4d605c00,$3fec0e74
	dc.l  -$40d7f0bc,-$40143563,-$761f3800,$3febdd20,$770321b4,-$40146dd8,-$6ca0c800,$3febac31,-$53127eb1,-$4014a63c,-$51540c00
	dc.l  $3feb7d39,$2a66ce31,-$4014dcbb,-$7ce33400,$3feb4e39,$18ab5324,-$401513a0,$d44c400,$3feb2034,-$4b27067d,-$401549bc
	dc.l  -$4b908400,$3feaf22c,$4510d007,-$40158038,$7525bc00,$3feac520,$28b387ea,-$4015b5e4,$76517400,$3fea984d,$3b002b0b
	dc.l  -$4015eba6,-$57dab000,$3fea6d4f,$62931b7e,-$40161f8a,$78b8000,$3fea4154,-$79166493,-$401654f8,$5c9ca800,$3fea164f
	dc.l  -$ede8e9a,-$40168991,$4308800,$3fe9ec52,-$7d1df512,-$4016bd3d,-$7a671400,$3fe9c24f,$1b5dd371,-$4016f144,-$5aa59400
	dc.l  $3fe9994c,$1f13fb38,-$4017245f,$345fc400,$3fe97041,$5ded51b0,-$401757d7,-$25452000,$3fe9482c,$5a0aae3e,-$40178a6a
	dc.l  $44b27400,$3fe92020,-$21d2b00d,-$4017bd42,$633db000,$3fe8f949,$61053187,-$4017eee1,-$187b6800,$3fe8d34e,-$3bcc8f0d
	dc.l  -$40181fb0,-$420ae400,$3fe8ac43,-$1df2dd7,-$4018522b,-$450f6400,$3fe8862e,-$97dcc99,-$401883b5,$454c2c00,$3fe86120
	dc.l  $70c6b991,-$4018b434,$3d18d800,$3fe83c20,$1f470398,-$4018e4ea,$712e800,$3fe81841,-$1bc73d16,-$4019146a,-$7afc8000
	dc.l  $3fe7f46c,-$2cbabbb8,-$40194424,$57286000,$3fe7d06c,-$7a9eed01,-$40197460,$75c4ec00,$3fe7ad20,$5c906cda,-$4019a3f2
	dc.l  -$10312000,$3fe78a72,-$4c9dac18,-$4019d2f3,-$3f051800,$3fe76769,-$20227bbf,-$401a02b6,$7b4b2400,$3fe74567,$34a90aec
	dc.l  -$401a3158,$60cc000,$3fe72468,-$5b11eb53,-$401a5ed8,-$17257400,$3fe70274,-$c39d09e,-$401a8ded,-$21f3fc00,$3fe6e173
	dc.l  $7f6bc840,-$401abbf5,-$985c400,$3fe6c120,$4ccded83,-$401ae94a,-$745bec00,$3fe6a172,$67830068,-$401b15f6,$4ca8a400
	dc.l  $3fe68165,$466a9830,-$400a8a93,$77e60000,$3fe66173,-$5fbdc0e9,-$400aa159,$7b989000,$3fe64265,$1060689e,-$400ab79c
	dc.l  $24f45000,$3fe62372,-$1e8f0a38,-$400acdea,$2b446600,$3fe60576,-$7150c9ea,-$400ae3a5,-$6bba3000,$3fe5e765,-$653a7a3c
	dc.l  -$400af98c,$41022200,$3fe5c964,$660e7df7,-$400b0f86,$58a9e000,$3fe5ac2e,$3dd882da,-$400b2508,$13bfa00,$3fe58e20
	dc.l  $50674399,-$400b3b48,$7200cc00,$3fe57120,$9cfbc4f,-$400b50dd,$3ac9c00,$3fe55554,$700438f5,-$400b65a9,-$27afa200
	dc.l  $3fe53961,-$82dd43f,-$400b7aac,$368d7200,$3fe51d62,-$70b681a7,-$400b8fd5,$8d8dc00,$3fe5016c,-$37ad61be,-$400ba513
	dc.l  $f733e00,$3fe4e565,$2b1e87f2,-$400bba7b,$14760200,$3fe4ca20,$423384c9,-$400bcf6a,$2b1ce400,$3fe4af67,-$7ca27546
	dc.l  -$400be408,$26c6e000,$3fe49565,$54767468,-$400bf833,$5a0e2400,$3fe47a6e,$49fd4d35,-$400c0d37,-$4997de00,$3fe46065
	dc.l  $6bafcd0a,-$400c219b,$5d5ecc00,$3fe44672,$3246bc95,-$400c3609,-$5b5bc00,$3fe42d61,$167b476d,-$400c49dd,-$4d122e00
	dc.l  $3fe41474,$4897712e,-$400c5dad,-$58924e00,$3fe3fb65,$6d32a9e6,-$400c71b1,-$4f798400,$3fe3e264,-$6f48b110,-$400c85c3
	dc.l  -$2cc41400,$3fe3c920,$3480ee17,-$400c9a24,$1445ec00,$3fe3b162,$74763d5a,-$400cad63,-$59ee9000,$3fe39979,$f69d11e
	dc.l  -$400cc0dc,$55c2c200,$3fe38120,-$3c196c54,-$400cd4c8,$3c469800,$3fe36952,-$7202b4c3,-$400ce85b,$823fe00,$3fe3526f
	dc.l  $262a0b30,-$400cfb44,$37b45600,$3fe33a67,$5dd6665e,-$400d0f37,-$4f961200,$3fe32365,-$1534f2f3,-$400d2267,-$7da2da00
	dc.l  $3fe30d72,$a9148bf,-$400d34cc,-$61fd3400,$3fe2f620,-$2f5a4285,-$400d486d,-$4367c800,$3fe2e053,$6a34bde0,-$400d5add
	dc.l  $716c1a00,$3fe2c96d,$f107e14,-$400d6e52,-$72900a00,$3fe2b469,-$51cce05e,-$400d8041,$1bcf9400,$3fe29e74,-$6ab67a99
	dc.l  -$400d9315,-$39ef2400,$3fe28868,$4bacd192,-$400da612,$2a6d0400,$3fe2732e,$5d171405,-$400db870,$59e77e00,$3fe25e20
	dc.l  -$21567506,-$400dcabc,$11ba2000,$3fe24920,$7befcfbb,-$400ddd12,$3cb66c00,$3fe23456,-$7ab667cf,-$400def4d,$21d97600
	dc.l  $3fe21fb7,-$5e40234,-$400e0177,$7e6a7200,$3fe20b47,$4d955617,-$400e138d,-$2e03f400,$3fe1f704,-$9976ceb,-$400e258e
	dc.l  -$1e529a00,$3fe1e2f0,$5ab88ba,-$400e377a,$352e3200,$3fe1cf07,$3e8252c8,-$400e4953,$b03ea00,$3fe1bb4a,-$5f8ea4f4
	dc.l  -$400e5b19,-$452e9c00,$3fe1a7b9,$6efd4831,-$400e6ccb,-$d94e200,$3fe19453,-$4406c7d8,-$400e7e68,$1909e200,$3fe18118
	dc.l  -$3f172403,-$400e8ff3,-$2efc7e00,$3fe16e06,-$3b9bb8ec,-$400ea16b,-$24de7a00,$3fe15b1e,-$1e2156f2,-$400eb2cf,-$71f4b600
	dc.l  $3fe1485f,$409a5169,-$400ec421,-$76b40400,$3fe135c8,-$6ace7a3c,-$400ed560,-$3b078600,$3fe12359,$3d80af14,-$400ee68c
	dc.l  $d7f8800,$3fe11111,$2b4dfe58,-$400ef7a7,-$5c0cfe00,$3fe0fef0,$7a8739d4,-$400f08af,-$ea67e00,$3fe0ecf5,-$3050c768
	dc.l  -$400f19a5,-$33d7000,$3fe0db21,$79918a4c,-$400f2a88,$5cecfa00,$3fe0c971,-$4b42418b,-$400f3b5b,-$4f162a00,$3fe0b7e7
	dc.l  -$1fb9c076,-$400f4c1b,-$6ec62600,$3fe0a681,-$f8c3b7b,-$400f5cca,$53b0a800,$3fe0953f,-$552f9bc3,-$400f6d68,$74b7800
	dc.l  $3fe08421,-$7b3f3a2d,-$400f7df5,$64e9f800,$3fe07326,$7181141c,-$400f8e71,-$4fe92200,$3fe0624e,$6d02a22,-$400f9edc
	dc.l  -$38b59400,$3fe05198,$7eaec3c2,-$400faf35,$28cbb800,$3fe04104,$10a3da07,-$400fbf7f,$59c39800,$3fe03092,$6cd400c3
	dc.l  -$400fcfb7,$46d49200,$3fe02040,-$3fcb5dfe,-$400fdfe0,$699f4a00,$3fe01010,-$2ffd1d32,-$400feff8,-$3b73cc00,$3fe00000
	dc.l  0,-$40100000,0

@__log_table_base:
	dc.l  _log_table_base+312*4

	END

