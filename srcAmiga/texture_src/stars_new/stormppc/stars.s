;	StormC Compiler V 3.93.35.PPC.Step3

	XREF	_floor__r
	XREF	@__floor__r
	XREF	_sqrt__r
	XREF	@__sqrt__r
	XREF	_atan2__r
	XREF	@__atan2__r
	XREF	_asin__r
	XREF	@__asin__r
	XREF	_cos__r
	XREF	@__cos__r
	XREF	op__new__Ui
	XREF	@_op__new__Ui
	XREF	_minVector
	XREF	@__minVector
	XREF	_maxVector
	XREF	@__maxVector

	SECTION	":0",Code

	XDEF	_INIT_8_stars_cpp
_INIT_8_stars_cpp:
;	DIALOG_ELEMENT dialog[] =
;	DIALOG_DATA default_data =
	blr

;	static void SAVEDS texture_cleanup(TEXTURE_INFO *tinfo)
texture_cleanup__P06_tinfo:
	XREF	_GetTOCReg
	mflr	r0
	stw	r0,8(r1)
	stw	r2,20(r1)
	bl	_GetTOCReg
	stwu	r1,-32(r1)
;	static void SAVEDS texture_cleanup(TEXTURE_INFO *tinfo)
	addi	r1,r1,32
	lwz	r0,8(r1)
	mtlr	r0
	lwz	r2,20(r1)
	blr

;	static void SAVEDS texture_work(STARS_INFO *info, DIALOG_DATA *param
texture_work__P10STARS_INFOP11DIALOG_DATAP13TEXTURE_PATCHP06VECTOR:
	stfd	f14,-8(r1)
	stw	r14,-12(r1)
	stw	r13,-16(r1)
	mflr	r0
	stw	r0,8(r1)
	stw	r2,20(r1)
	bl	_GetTOCReg
	addis	r0,r0,16286
	mr	r13,r4	;dst=src
	mr	r14,r5	;dst=src
;		f = (v->x*1.23456789f+98765.4321f) * (v->y*2.3456789f+87654.321f) 
	ori	r0,r0,+1618
	stwu	r1,-64(r1)
	lfs	f0,0(r6)
	stw	r0,-4(r1)
	addis	r0,r0,18368
	lfs	f13,-4(r1)
	fmuls	f0,f0,f13
	ori	r0,r0,-6473
	stw	r0,-4(r1)
	addis	r0,r0,16406
	lfs	f13,-4(r1)
	fadds	f1,f0,f13
	ori	r0,r0,+8090
	lfs	f0,4(r6)
	stw	r0,-4(r1)
	addis	r0,r0,18347
	lfs	f13,-4(r1)
	fmuls	f0,f0,f13
	ori	r0,r0,+13097
	stw	r0,-4(r1)
	addis	r0,r0,16477
	lfs	f13,-4(r1)
	fadds	f0,f0,f13
	fmuls	f1,f1,f0
	ori	r0,r0,+15367
	lfs	f0,8(r6)
	stw	r0,-4(r1)
	lfs	f13,-4(r1)
	fmuls	f0,f0,f13
	addis	r0,r0,18325
	ori	r0,r0,+32666
	stw	r0,-4(r1)
	lfs	f13,-4(r1)
	fadds	f0,f0,f13
	fmuls	f14,f1,f0
;		f -= floor(f);
	fmr	f1,f14
	bl	_floor__r
	frsp	f1,f1
	fsubs	f14,f14,f1
;		f = f < 0.f ? -f : f;
	addi	r0,r0,0
	stw	r0,-4(r1)
	lfs	f13,-4(r1)
	fcmpu	cr1,f14,f13
	bge	cr1,L28
	fneg	f14,f14
L28
L29
;		if(f < params->density)
	lfs	f0,48(r13)
	fcmpu	cr1,f14,f0
	bge	cr1,L36
;			if(params->diffuse_enabled)
	lwz	r3,12(r13)
	cmpli	cr0,r3,0
	beq	cr0,L32
;				patch->diffuse = params->diffuse;
	lwz	r0,0(r13)
	addi	r3,r14,12
	stw	r0,0(r3)
	lwz	r0,4(r13)
	stw	r0,4(r3)
	lwz	r0,8(r13)
	stw	r0,8(r3)
L32
;			if(params->reflect_enabled)
	lwz	r3,28(r13)
	cmpli	cr0,r3,0
	beq	cr0,L34
;				patch->reflect = params->reflect;
	addi	r4,r14,36
	addi	r3,r13,16
	lwz	r0,0(r3)
	stw	r0,0(r4)
	lwz	r0,4(r3)
	stw	r0,4(r4)
	lwz	r0,8(r3)
	stw	r0,8(r4)
L34
;			if(params->transpar_enabled)
	lwz	r3,44(r13)
	cmpli	cr0,r3,0
	beq	cr0,L36
;				patch->transpar = params->transpar;
	addi	r4,r14,48
	addi	r3,r13,32
	lwz	r0,0(r3)
	stw	r0,0(r4)
	lwz	r0,4(r3)
	stw	r0,4(r4)
	lwz	r0,8(r3)
	stw	r0,8(r4)
L36
	addi	r1,r1,64
	lfd	f14,-8(r1)
	lwz	r0,8(r1)
	lwz	r14,-12(r1)
	mtlr	r0
	lwz	r13,-16(r1)
	lwz	r2,20(r1)
	blr

;	extern "C" TEXTURE_INFO* SAVEDS texture_init(
	XDEF	_texture_init
_texture_init:
	mflr	r0
	stw	r0,8(r1)
	stw	r2,20(r1)
	bl	_GetTOCReg
;		info = new STARS_INFO;
	addi	r3,r0,24
	stwu	r1,-48(r1)
	bl	op__new__Ui
;		if(info)
	cmpli	cr0,r3,0
	beq	cr0,L41
;			info->tinfo.name = "Stars";
	lwz	r12,L37(r2)
	stw	r12,0(r3)
;			info->tinfo.dialog = dialog;
	lwz	r12,@__dialog(r2)
	stw	r12,4(r3)
;			info->tinfo.defaultdata = &default_data;
	lwz	r12,@__default_data(r2)
	stw	r12,8(r3)
;			info->tinfo.datasize = sizeof(DIALOG_DATA);
	addi	r12,r0,52
	stw	r12,12(r3)
;			info->tinfo.cleanup = texture_cleanup;
	lwz	r12,@_texture_cleanup__P06_tinfo(r2)
	stw	r12,16(r3)
;			info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH
	lwz	r12,@_texture_work__P10STARS_INFOP11DIALOG_DATAP13TEXTURE_PATCHP06VECTOR(r2)
	stw	r12,20(r3)
L41
	addi	r1,r1,48
	lwz	r0,8(r1)
	mtlr	r0
	lwz	r2,20(r1)
	blr

	SECTION	":1",Data
	XDEF	_dialog
_dialog:
	dc.l	2,3,5,L17,6,$b,5,L19,6,$b,5
	dc.l	L21,6,$b,3,2,2,5,L23,8,3,4
	XDEF	_default_data
_default_data:
	dc.l	$3f800000,$3f800000,$3f800000,1,0,0,0,0,0,0,0
	dc.l	0,$3c23d70a

L23:
	dc.b	"Density",0
L17:
	dc.b	"Diffuse",0
L19:
	dc.b	"Reflective",0
L38:
	dc.b	"Stars",0
L21:
	dc.b	"Transparent",0

	SECTION	"ToC_1:1",Data
L22:
	dc.l	L23
L16:
	dc.l	L17
L18:
	dc.l	L19
L37:
	dc.l	L38
L20:
	dc.l	L21

	SECTION	"@__default_data:1",Data
	XDEF	@__default_data
@__default_data:
	dc.l	_default_data

	SECTION	"@__dialog:1",Data
	XDEF	@__dialog
@__dialog:
	dc.l	_dialog

	SECTION	"@__texture_init:1",Data
	XDEF	@__texture_init
@__texture_init:
	dc.l	_texture_init

	SECTION	"@_texture_work__P10STARS_INFOP11DIALOG_DATAP13TEXTURE_PATCHP06VECTOR:1",Data
@_texture_work__P10STARS_INFOP11DIALOG_DATAP13TEXTURE_PATCHP06VECTOR:
	dc.l	texture_work__P10STARS_INFOP11DIALOG_DATAP13TEXTURE_PATCHP06VECTOR

	SECTION	"@_texture_cleanup__P06_tinfo:1",Data
@_texture_cleanup__P06_tinfo:
	dc.l	texture_cleanup__P06_tinfo

	END
