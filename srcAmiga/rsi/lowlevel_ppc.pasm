	.globl
	.section .text

;  BOOL SPHERE::RayIntersect(RAY *ray)
RayIntersect__SPHERE__TP03RAY:
	stfd  f14,-8(r1)
	stw   r14,-12(r1)
	stw   r13,-16(r1)
	mflr  r0
	mr r14,r3   ;dst=src
	mr r13,r4   ;dst=src
	stw   r0,8(r1)
	stwu  r1,-96(r1)
;     VecSub(&pos, &ray->start, &adj);
	addi  r5,r1,40
	addi  r3,r14,70
	lfs   f1,0(r3)
	mr r4,r13   ;dst=src
	lfs   f0,0(r4)
	fsubs f0,f1,f0
	stfs  f0,0(r5)
	lfs   f1,4(r3)
	lfs   f0,4(r4)
	fsubs f0,f1,f0
	stfs  f0,4(r5)
	lfs   f1,8(r3)
	addi  r3,r1,40
	lfs   f0,8(r4)
	fsubs f0,f1,f0
;     b = adj.x * ray->dir.x + adj.y * ray->dir.y + adj.z * ray->dir.z;
	stfs  f0,8(r5)
	lfs   f1,0(r3)
	addi  r3,r1,40
	lfs   f0,12(r13)
	fmuls f2,f1,f0
	lfs   f1,4(r3)
	addi  r3,r1,40
	lfs   f0,16(r13)
	fmadds   f2,f1,f0,f2
	lfs   f1,8(r3)
	addi  r3,r1,40
	lfs   f0,20(r13)
	fmadds   f14,f1,f0,f2
;     t = b * b - adj.x * adj.x - adj.y * adj.y - adj.z * adj.z + rsq;
	fmuls f2,f14,f14
	lfs   f1,0(r3)
	addi  r3,r1,40
	lfs   f0,0(r3)
	addi  r3,r1,40
	fmuls f0,f1,f0
	fsubs f2,f2,f0
	lfs   f1,4(r3)
	addi  r3,r1,40
	lfs   f0,4(r3)
	addi  r3,r1,40
	fmuls f0,f1,f0
	fsubs f2,f2,f0
	lfs   f1,8(r3)
	addi  r3,r1,40
	addi  r0,r0,0
	lfs   f0,8(r3)
	fmuls f0,f1,f0
	fsubs f1,f2,f0
	lfs   f0,54(r14)
	fadds f1,f1,f0
;     if (t < 0.f)
	stw   r0,-4(r1)
	lfs   f13,-4(r1)
	fcmpu cr1,f1,f13
	bge   cr1,L56
;        return FALSE;
	addi  r3,r0,0
	addi  r1,r1,96
	lwz   r0,8(r1)
	mtlr  r0
	lfd   f14,-8(r1)
	lwz   r14,-12(r1)
	lwz   r13,-16(r1)
	blr
L56
;     t = sqrt(t);
	bl _sqrt_ppc__r
	addi  r0,r0,0
	frsp  f1,f1
;     s = b - t;
	fsubs f0,f14,f1
;     if(s > 0.f)
	stw   r0,-4(r1)
	lfs   f13,-4(r1)
	fcmpu cr1,f0,f13
	ble   cr1,L59
;     
;        if(s < ray->lambda)
	lfs   f2,24(r13)
	fcmpu cr1,f0,f2
	bge   cr1,L59
;        
;           ray->lambda = s;
;           ray->obj = this;
;           ray->enter = TRUE;
	addi  r12,r0,1
	addi  r3,r0,1
	addi  r1,r1,96
	stfs  f0,24(r13)
;           return TRUE;
	stw   r14,40(r13)
	sth   r12,44(r13)
	lwz   r0,8(r1)
	mtlr  r0
	lfd   f14,-8(r1)
	lwz   r14,-12(r1)
	lwz   r13,-16(r1)
	blr
L59
;     s = b + t;
	addi  r0,r0,0
	fadds f0,f14,f1
;     if(s > 0.f)
	stw   r0,-4(r1)
	lfs   f13,-4(r1)
	fcmpu cr1,f0,f13
	ble   cr1,L62
;     
;        if(s < ray->lambda)
	lfs   f1,24(r13)
	fcmpu cr1,f0,f1
	bge   cr1,L62
;        
;           ray->lambda = s;
;           ray->obj = this;
;           ray->enter = FALSE;
	addi  r12,r0,0
	addi  r3,r0,1
	addi  r1,r1,96
	stfs  f0,24(r13)
;           return TRUE;
	stw   r14,40(r13)
	sth   r12,44(r13)
	lwz   r0,8(r1)
	mtlr  r0
	lfd   f14,-8(r1)
	lwz   r14,-12(r1)
	lwz   r13,-16(r1)
	blr
L62
;     return FALSE;
	addi  r3,r0,0
	addi  r1,r1,96
	lwz   r0,8(r1)
	mtlr  r0
	lfd   f14,-8(r1)
	lwz   r14,-12(r1)
	lwz   r13,-16(r1)
	blr

