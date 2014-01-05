	XDEF _log__r
	XDEF _log10__r

	SECTION  ":0",Code

;----------------------------------------- LOG

_log__r:
log:
	stwu  r1,-80(r1)
	stfd   f1,48(r1)
	mffs   f4
	lis r11,19264
log_restart:
	stfd   f1,32(r1)
	lis  r3,0
		RELOC: 000009ea R_PPC_ADDR16_HA log_table_base
	lwz  r4,32(r1)
	addi r3,r3,0
		RELOC: 000009f2 R_PPC_ADDR16_LO log_table_base
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
log10:
	stwu   r1,-80(r1)
	stfd f1,48(r1)
	mffs f4
	lis  r11,19264
log10_restart:
	stfd f1,32(r1)

	lis   r3,0
		RELOC: 00000b96 R_PPC_ADDR16_HA log_table_base
	lwz   r4,32(r1)
	addi  r3,r3,0
		RELOC: 00000b9e R_PPC_ADDR16_LO log_table_base
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

_pow_data:
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

	XDEF  @__pow_data
@__pow_data:
	dc.l  _pow_data+312*4

	END

 0d48 bfe00000 00000000 3fd55555 55555555  ........?.UUUUUU
 0d58 bfcfffff fff1f8c5 3fc99999 999449eb  ........?.....I.
 0d68 bfc55571 5ca0cd75 3fc24936 4add74ae  ..Uq\..u?.I6J.t.
 0d78 3ff00000 00000000 fff00000 00000000  ?...............
 0d88 7ff00000 00000000 00000000 00000000  ................
 0d98 7ff80000 00000000 3fe62e42 fefa3800  ........?..B..8.
 0da8 3d2ef357 93c76730 3fdbcb7b 1526e50e  =..W..g0?..{.&..
 0db8 3c695355 baaafad0 4b4003fe 00000000  <iSU....K@......
 0dc8 43300000 00000000 3bf00000 00000000  C0......;.......
 0dd8 7ff80000 00000000 43380000 00000000  ........C8......
 0de8 40471547 652b82fe 3f962e42 fefa39ef  @G.Ge+..?..B..9.
 0df8 bc2abc9e 3b39803e 00000000 00000000  .*..;9.>........
 0e08 3ff00000 00000000 3e343e0c bf000000  ?.......>4>.....
 0e18 3ff059b0 d4607dce 3e36f443 72000000  ?.Y..`}.>6.Cr...
 0e28 3ff0b558 6e790fe8 3e370dd7 8f000000  ?..Xny..>7......
 0e38 3ff11301 d19bfcd2 3e379ef6 0a000000  ?.......>7......
 0e48 3ff172b8 3e19759b 3e37207f 60000000  ?.r.>.u.>7 .`...
 0e58 3ff1d487 33051540 3e3696ca cf000000  ?...3..@>6......
 0e68 3ff2387a 7010f845 3e367af0 d8000000  ?.8zp..E>6z.....
 0e78 3ff29e9d f6c27190 3e3687e8 c4000000  ?.....q.>6......
 0e88 3ff306fe 0bde6ae7 3e374531 3b000000  ?.....j.>7E1;...
 0e98 3ff371a7 38ff2130 3e320557 87000000  ?.q.8.!0>2.W....
 0ea8 3ff3dea6 4d7845f8 3e331784 49000000  ?...MxE.>3..I...
 0eb8 3ff44e08 61e5314d 3e339287 63000000  ?.N.a.1M>3..c...
 0ec8 3ff4bfda d6cc47c9 3e339fb8 33000000  ?.....G.>3..3...
 0ed8 3ff5342b 583d6962 3e334309 ca000000  ?.4+X=ib>3C.....
 0ee8 3ff5ab07 dee9b256 3e32c7b1 47000000  ?......V>2..G...
 0ef8 3ff6247e b1da2c21 3e320f6a d0000000  ?.$~..,!>2.j....
 0f08 3ff6a09e 6817e3ca 3e34db2b 1e000000  ?...h...>4.+....
 0f18 3ff71f75 eacea079 3e34f716 bd000000  ?..u...y>4......
 0f28 3ff7a114 75da65a4 3e354fdb 77000000  ?...u.e.>5O.w...
 0f38 3ff82589 9b4f6aa5 3e34f0b6 63000000  ?.%..Oj.>4..c...
 0f48 3ff8ace5 442f5670 3e352ebd 4e000000  ?...D/Vp>5..N...
 0f58 3ff93737 b2e3e808 3e34f47b 5e000000  ?.77....>4.{^...
 0f68 3ff9c491 84bfe7b4 3e34c160 1b000000  ?.......>4.`....
 0f78 3ffa5503 b460ada1 3e3410c7 a9000000  ?.U..`..>4......
 0f88 3ffae89f 9b76c3ac 3e32c735 83000000  ?....v..>2.5....
 0f98 3ffb7f76 f4ffba79 3e320717 06000000  ?..v...y>2......
 0fa8 3ffc199b df7fe6cc 3e34927f 50000000  ?.......>4..P...
 0fb8 3ffcb720 df3e4db3 3e34ec9a 05000000  ?.. .>M.>4......
 0fc8 3ffd5818 df61a557 3e34308b ca000000  ?.X..a.W>40.....
 0fd8 3ffdfc97 35d906eb 3e32eafb af000000  ?...5...>2......
 0fe8 3ffea4af a4e845dc 3e320553 16000000  ?.....E.>2.S....
 0ff8 3fff5076 5da2944f 3fe00000 00000000  ?.Pv]..O?.......
 1008 3fc55555 55548f7c 3fa55555 55545d4e  ?.UUUT.|?.UUUT]N
 1018 3f811115 b7aa905e 3f56c172 8d739765  ?......^?V.r.s.e
 1028 3ff00000 00000000 00000000 00000000  ?...............
 1038 3ff059b0 d3158540 3d0a1d73 e2a475b4  ?.Y....@=..s..u.
 1048 3ff0b558 6cf98900 3ceec531 7256e308  ?..Xl...<..1rV..
 1058 3ff11301 d0125b40 3cf0a4eb bf1aed93  ?.....[@<.......
 1068 3ff172b8 3c7d5140 3d0d6e6f be462876  ?.r.<}Q@=.no.F(v
 1078 3ff1d487 3168b980 3d053c02 dc0144c8  ?...1h..=.<...D.
 1088 3ff2387a 6e756200 3d0c3360 fd6d8e0b  ?.8znub.=.3`.m..
 1098 3ff29e9d f51fdec0 3d009612 e8afad12  ?.......=.......
 10a8 3ff306fe 0a31b700 3cf52de8 d5a46306  ?....1..<.-...c.
 10b8 3ff371a7 373aa9c0 3ce54e28 aa05e8a9  ?.q.7:..<.N(....
 10c8 3ff3dea6 4c123400 3d011ada 0911f09f  ?...L.4.=.......
 10d8 3ff44e08 60618900 3d068189 b7a04ef8  ?.N.`a..=.....N.
 10e8 3ff4bfda d5362a00 3d038ea1 cbd7f621  ?....6*.=......!
 10f8 3ff5342b 569d4f80 3cbdf0a8 3c49d86a  ?.4+V.O.<...<I.j
 1108 3ff5ab07 dd485400 3d04ac64 980a8c8f  ?....HT.=..d....
 1118 3ff6247e b03a5580 3cd2c7c3 e81bf4b7  ?.$~.:U.<.......
 1128 3ff6a09e 667f3bc0 3ce92116 5f626cdd  ?...f.;.<.!._bl.
 1138 3ff71f75 e8ec5f40 3d09ee91 b8797785  ?..u.._@=....yw.
 1148 3ff7a114 73eb0180 3cdb5f54 408fdb37  ?...s...<._T@..7
 1158 3ff82589 994cce00 3cf28acf 88afab35  ?.%..L..<......5
 1168 3ff8ace5 422aa0c0 3cfb5ba7 c55a192d  ?...B*..<.[..Z.-
 1178 3ff93737 b0cdc5c0 3d027a28 0e1f92a0  ?.77....=.z(....
 1188 3ff9c491 82a3f080 3cf01c7c 46b071f3  ?.......<..|F.q.
 1198 3ffa5503 b23e2540 3cfc8b42 4491caf8  ?.U..>%@<..BD...
 11a8 3ffae89f 995ad380 3d06af43 9a68bb99  ?....Z..=..C.h..
 11b8 3ffb7f76 f2fb5e40 3cdbaa9e c206ad4f  ?..v..^@<......O
 11c8 3ffc199b dd855280 3cfc2220 cb12a092  ?.....R.<." ....
 11d8 3ffcb720 dcef9040 3d048a81 e5e8f4a5  ?.. ...@=.......
 11e8 3ffd5818 dcfba480 3cdc9768 16bad9b8  ?.X.....<..h....
 11f8 3ffdfc97 337b9b40 3cfeb968 cac39ed3  ?...3{.@<..h....
 1208 3ffea4af a2a490c0 3cf9858f 73a18f5e  ?.......<...s..^
 1218 3fff5076 5b6e4540 3c99d3ea 2dd8a18b  ?.Pv[nE@<...-...
 1228 3ff00000 00000000 bff00000 00000000  ?...............
 1238 3fefc07f 20278dc2 bfefc03f c991bc00  ?... '.....?....
 1248 3fef81f8 499299c5 bfef80fd 87f4f400  ?...I...........
 1258 3fef4465 e2a3afb7 bfef4237 6db4d800  ?.De......B7m...
 1268 3fef07c1 f7293018 bfef03eb 2e6eb800  ?....)0......n..
 1278 3feecc08 08497991 bfeec617 dcfc6c00  ?....Iy.......l.
 1288 3fee9132 1d5593bf bfee88ba e7526800  ?..2.U.......Rh.
 1298 3fee573b 0f20e307 bfee4bd2 72e31800  ?.W;. ....K.r...
 12a8 3fee1e1e 26deb2fc bfee0f5c fd4b8800  ?...&......\.K..
 12b8 3fede5d7 8d27dfa3 bfedd359 d7403c00  ?....'.....Y.@<.
 12c8 3fedae43 27a303e0 bfed97a5 73e8ac00  ?..C'.......s...
 12d8 3fed776f c0e71091 bfed5c52 733e6000  ?.wo......\Rs>`.
 12e8 3fed4170 d71cdd8d bfed2179 1932ac00  ?.Ap......!y.2..
 12f8 3fed0c79 640b3c0e bfece754 fd298800  ?..yd.<....T.)..
 1308 3fecd872 58123b28 bfecadd1 325c0000  ?..rX.;(....2\..
 1318 3feca469 9c164a32 bfec73e3 6a0c5400  ?..i..J2..s.j.T.
 1328 3fec7167 99e26f00 bfec3ab3 9ea4f800  ?.qg..o...:.....
 1338 3fec3f68 07ec37cb bfec0241 b29fa400  ?.?h..7....A....
 1348 3fec0e74 bf280f44 bfebca9d 89e0c800  ?..t.(.D........
 1358 3febdd20 770321b4 bfeb9228 935f3800  ?.. w.!....(._8.
 1368 3febac31 aced814f bfeb59c4 aeabf400  ?..1...O..Y.....
 1378 3feb7d39 2a66ce31 bfeb2345 831ccc00  ?.}9*f.1..#E....
 1388 3feb4e39 18ab5324 bfeaec60 0d44c400  ?.N9..S$...`.D..
 1398 3feb2034 b4d8f983 bfeab644 b46f7c00  ?. 4.......D.o|.
 13a8 3feaf22c 4510d007 bfea7fc8 7525bc00  ?..,E.......u%..
 13b8 3feac520 28b387ea bfea4a1c 76517400  ?.. (.....J.vQt.
 13c8 3fea984d 3b002b0b bfea145a a8255000  ?..M;.+....Z.%P.
 13d8 3fea6d4f 62931b7e bfe9e076 078b8000  ?.mOb..~...v....
 13e8 3fea4154 86e99b6d bfe9ab08 5c9ca800  ?.AT...m....\...
 13f8 3fea164f f1217166 bfe9766f 04308800  ?..O.!qf..vo.0..
 1408 3fe9ec52 82e20aee bfe942c3 8598ec00  ?..R......B.....
 1418 3fe9c24f 1b5dd371 bfe90ebc a55a6c00  ?..O.].q.....Zl.
 1428 3fe9994c 1f13fb38 bfe8dba1 345fc400  ?..L...8....4_..
 1438 3fe97041 5ded51b0 bfe8a829 dabae000  ?.pA].Q....)....
 1448 3fe9482c 5a0aae3e bfe87596 44b27400  ?.H,Z..>..u.D.t.
 1458 3fe92020 de2d4ff3 bfe842be 633db000  ?.  .-O...B.c=..
 1468 3fe8f949 61053187 bfe8111f e7849800  ?..Ia.1.........
 1478 3fe8d34e c43370f3 bfe7e050 bdf51c00  ?..N.3p....P....
 1488 3fe8ac43 fe20d229 bfe7add5 baf09c00  ?..C. .)........
 1498 3fe8862e f6823367 bfe77c4b 454c2c00  ?.....3g..|KEL,.
 14a8 3fe86120 70c6b991 bfe74bcc 3d18d800  ?.a p.....K.=...
 14b8 3fe83c20 1f470398 bfe71b16 0712e800  ?.< .G..........
 14c8 3fe81841 e438c2ea bfe6eb96 85038000  ?..A.8..........
 14d8 3fe7f46c d3454448 bfe6bbdc 57286000  ?..l.EDH....W(`.
 14e8 3fe7d06c 856112ff bfe68ba0 75c4ec00  ?..l.a......u...
 14f8 3fe7ad20 5c906cda bfe65c0e efcee000  ?.. \.l...\.....
 1508 3fe78a72 b36253e8 bfe62d0d c0fae800  ?..r.bS...-.....
 1518 3fe76769 dfdd8441 bfe5fd4a 7b4b2400  ?.gi...A...J{K$.
 1528 3fe74567 34a90aec bfe5cea8 060cc000  ?.Eg4...........
 1538 3fe72468 a4ee14ad bfe5a128 e8da8c00  ?.$h.......(....
 1548 3fe70274 f3c62f62 bfe57213 de0c0400  ?..t../b..r.....
 1558 3fe6e173 7f6bc840 bfe5440b f67a3c00  ?..s.k.@..D..z<.
 1568 3fe6c120 4ccded83 bfe516b6 8ba41400  ?.. L...........
 1578 3fe6a172 67830068 bfe4ea0a 4ca8a400  ?..rg..h....L...
 1588 3fe68165 466a9830 bff5756d 77e60000  ?..eFj.0..umw...
 1598 3fe66173 a0423f17 bff55ea7 7b989000  ?.as.B?...^.{...
 15a8 3fe64265 1060689e bff54864 24f45000  ?.Be.`h...Hd$.P.
 15b8 3fe62372 e170f5c8 bff53216 2b446600  ?.#r.p....2.+Df.
 15c8 3fe60576 8eaf3616 bff51c5b 9445d000  ?..v..6....[.E..
 15d8 3fe5e765 9ac585c4 bff50674 41022200  ?..e.......tA.".
 15e8 3fe5c964 660e7df7 bff4f07a 58a9e000  ?..df.}....zX...
 15f8 3fe5ac2e 3dd882da bff4daf8 013bfa00  ?...=........;..
 1608 3fe58e20 50674399 bff4c4b8 7200cc00  ?.. PgC.....r...
 1618 3fe57120 09cfbc4f bff4af23 03ac9c00  ?.q ...O...#....
 1628 3fe55554 700438f5 bff49a57 d8505e00  ?.UTp.8....W.P^.
 1638 3fe53961 f7d22bc1 bff48554 368d7200  ?.9a..+....T6.r.
 1648 3fe51d62 8f497e59 bff4702b 08d8dc00  ?..b.I~Y..p+....
 1658 3fe5016c c8529e42 bff45aed 0f733e00  ?..l.R.B..Z..s>.
 1668 3fe4e565 2b1e87f2 bff44585 14760200  ?..e+.....E..v..
 1678 3fe4ca20 423384c9 bff43096 2b1ce400  ?.. B3....0.+...
 1688 3fe4af67 835d8aba bff41bf8 26c6e000  ?..g.]......&...
 1698 3fe49565 54767468 bff407cd 5a0e2400  ?..eTvth....Z.$.
 16a8 3fe47a6e 49fd4d35 bff3f2c9 b6682200  ?.znI.M5.....h".
 16b8 3fe46065 6bafcd0a bff3de65 5d5ecc00  ?.`ek......e]^..
 16c8 3fe44672 3246bc95 bff3c9f7 fa4a4400  ?.Fr2F.......JD.
 16d8 3fe42d61 167b476d bff3b623 b2edd200  ?.-a.{Gm...#....
 16e8 3fe41474 4897712e bff3a253 a76db200  ?..tH.q....S.m..
 16f8 3fe3fb65 6d32a9e6 bff38e4f b0867c00  ?..em2.....O..|.
 1708 3fe3e264 90b74ef0 bff37a3d d33bec00  ?..d..N...z=.;..
 1718 3fe3c920 3480ee17 bff365dc 1445ec00  ?.. 4.....e..E..
 1728 3fe3b162 74763d5a bff3529d a6117000  ?..btv=Z..R...p.
 1738 3fe39979 0f69d11e bff33f24 55c2c200  ?..y.i....?$U...
 1748 3fe38120 c3e693ac bff32b38 3c469800  ?.. ......+8<F..
 1758 3fe36952 8dfd4b3d bff317a5 0823fe00  ?.iR..K=.....#..
 1768 3fe3526f 262a0b30 bff304bc 37b45600  ?.Ro&*.0....7.V.
 1778 3fe33a67 5dd6665e bff2f0c9 b069ee00  ?.:g].f^.....i..
 1788 3fe32365 eacb0d0d bff2dd99 825d2600  ?.#e.........]&.
 1798 3fe30d72 0a9148bf bff2cb34 9e02cc00  ?..r..H....4....
 17a8 3fe2f620 d0a5bd7b bff2b793 bc983800  ?.. ...{......8.
 17b8 3fe2e053 6a34bde0 bff2a523 716c1a00  ?..Sj4.....#ql..
 17c8 3fe2c96d 0f107e14 bff291ae 8d6ff600  ?..m..~......o..
 17d8 3fe2b469 ae331fa2 bff27fbf 1bcf9400  ?..i.3..........
 17e8 3fe29e74 95498567 bff26ceb c610dc00  ?..t.I.g..l.....
 17f8 3fe28868 4bacd192 bff259ee 2a6d0400  ?..hK.....Y.*m..
 1808 3fe2732e 5d171405 bff24790 59e77e00  ?.s.].....G.Y.~.
 1818 3fe25e20 dea98afa bff23544 11ba2000  ?.^ ......5D.. .
 1828 3fe24920 7befcfbb bff222ee 3cb66c00  ?.I {.....".<.l.
 1838 3fe23456 85499831 bff210b3 21d97600  ?.4V.I.1....!.v.
 1848 3fe21fb7 fa1bfdcc bff1fe89 7e6a7200  ?...........~jr.
 1858 3fe20b47 4d955617 bff1ec73 d1fc0c00  ?..GM.V....s....
 1868 3fe1f704 f6689315 bff1da72 e1ad6600  ?....h.....r..f.
 1878 3fe1e2f0 05ab88ba bff1c886 352e3200  ?...........5.2.
 1888 3fe1cf07 3e8252c8 bff1b6ad 0b03ea00  ?...>.R.........
 1898 3fe1bb4a a0715b0c bff1a4e7 bad16400  ?..J.q[.......d.
 18a8 3fe1a7b9 6efd4831 bff19335 f26b1e00  ?...n.H1...5.k..
 18b8 3fe19453 bbf93828 bff18198 1909e200  ?..S..8(........
 18c8 3fe18118 c0e8dbfd bff1700d d1038200  ?.........p.....
 18d8 3fe16e06 c4644714 bff15e95 db218600  ?.n..dG...^..!..
 18e8 3fe15b1e e1dea90e bff14d31 8e0b4a00  ?.[.......M1..J.
 18f8 3fe1485f 409a5169 bff13bdf 894bfc00  ?.H_@.Qi..;..K..
 1908 3fe135c8 953185c4 bff12aa0 c4f87a00  ?.5..1....*...z.
 1918 3fe12359 3d80af14 bff11974 0d7f8800  ?.#Y=......t....
 1928 3fe11111 2b4dfe58 bff10859 a3f30200  ?...+M.X...Y....
 1938 3fe0fef0 7a8739d4 bff0f751 f1598200  ?...z.9....Q.Y..
 1948 3fe0ecf5 cfaf3898 bff0e65b fcc29000  ?.....8....[....
 1958 3fe0db21 79918a4c bff0d578 5cecfa00  ?..!y..L...x\...
 1968 3fe0c971 b4bdbe75 bff0c4a5 b0e9d600  ?..q...u........
 1978 3fe0b7e7 e0463f8a bff0b3e5 9139da00  ?....F?......9..
 1988 3fe0a681 f073c485 bff0a336 53b0a800  ?....s.....6S...
 1998 3fe0953f aad0643d bff09298 074b7800  ?..?..d=.....Kx.
 19a8 3fe08421 84c0c5d3 bff0820b 64e9f800  ?..!........d...
 19b8 3fe07326 7181141c bff0718f b016de00  ?.s&q.....q.....
 19c8 3fe0624e 06d02a22 bff06124 c74a6c00  ?.bN..*"..a$.Jl.
 19d8 3fe05198 7eaec3c2 bff050cb 28cbb800  ?.Q.~.....P.(...
 19e8 3fe04104 10a3da07 bff04081 59c39800  ?.A.......@.Y...
 19f8 3fe03092 6cd400c3 bff03049 46d49200  ?.0.l.....0IF...
 1a08 3fe02040 c034a202 bff02020 699f4a00  ?. @.4....  i.J.
 1a18 3fe01010 d002e2ce bff01008 c48c3400  ?.............4.
 1a28 3fe00000 00000000 bff00000 00000000  ?...............

00000000 l     O .data  00000000 log_v2
00000008 l     O .data  00000000 log_v3
00000010 l     O .data  00000000 log_v4
00000018 l     O .data  00000000 log_v5
00000020 l     O .data  00000000 log_v6
00000028 l     O .data  00000000 log_v7
00000030 l     O .data  00000000 one
00000038 l     O .data  00000000 minus_infinity
00000040 l     O .data  00000000 infinity
00000048 l     O .data  00000000 zero
00000050 l     O .data  00000000 domain_qnan
00000058 l     O .data  00000000 log_2_hi
00000060 l     O .data  00000000 log_2_lo
00000068 l     O .data  00000000 log10_e_hi
00000070 l     O .data  00000000 log10_e_lo
00000078 l     O .data  00000000 b_plus_3fe
00000080 l     O .data  00000000 two_52
00000088 l     O .data  00000000 two_m_64
00000090 l     O .data  00000000 qnan
00000098 l     O .data  00000000 big
000000a0 l     O .data  00000000 q_inv
000000a8 l     O .data  00000000 q_bar
000000b0 l     O .data  00000000 alpha
000000b8 l     O .data  00000000 exp_vals
000002b8 l     O .data  00000000 exp_v2
000002c0 l     O .data  00000000 exp_v3
000002c8 l     O .data  00000000 exp_v4
000002d0 l     O .data  00000000 exp_v5
000002d8 l     O .data  00000000 exp_v6
000002e0 l     O .data  00000000 s_table
000004e0 l     O .data  00000000 log_table_base
000004e0 l     O .data  00000000 base
00000000         *UND*  00000000 errno
00000620 g     O .text  00000000 exp
00000b80 g     O .text  00000000 log10
000009d4 g     O .text  00000000 log
00000000 g     O .text  00000000 pow


RELOCATION RECORDS FOR [.text]:
OFFSET   TYPE              VALUE 
00000016 R_PPC_ADDR16_HA   log_table_base
0000001e R_PPC_ADDR16_LO   log_table_base
0000036a R_PPC_ADDR16_HA   errno
00000372 R_PPC_ADDR16_LO   errno
000004a2 R_PPC_ADDR16_HA   errno
000004aa R_PPC_ADDR16_LO   errno
0000062e R_PPC_ADDR16_HA   log_table_base
00000636 R_PPC_ADDR16_LO   log_table_base
0000075e R_PPC_ADDR16_HA   errno
00000766 R_PPC_ADDR16_LO   errno
00000812 R_PPC_ADDR16_HA   errno
0000081a R_PPC_ADDR16_LO   errno
0000083a R_PPC_ADDR16_HA   log_table_base
00000842 R_PPC_ADDR16_LO   log_table_base
00000906 R_PPC_ADDR16_HA   errno
0000090e R_PPC_ADDR16_LO   errno
00000922 R_PPC_ADDR16_HA   errno
0000092a R_PPC_ADDR16_LO   errno
000009ea R_PPC_ADDR16_HA   log_table_base
000009f2 R_PPC_ADDR16_LO   log_table_base
00000b1a R_PPC_ADDR16_HA   errno
00000b22 R_PPC_ADDR16_LO   errno
00000b96 R_PPC_ADDR16_HA   log_table_base
00000b9e R_PPC_ADDR16_LO   log_table_base
00000cde R_PPC_ADDR16_HA   errno
00000ce6 R_PPC_ADDR16_LO   errno


