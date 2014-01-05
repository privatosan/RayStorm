;***************
;* MODUL:         lowlevel routines
;* NAME:          lowlevel.asm
;* DESCRIPTION:   In this module are all the functions which are written
;*    in assembler to speed up the program; set '#define LOWLEVEL' to use
;*    this routines
;* AUTHORS:       Andreas Heumann, Mike Hesser
;* HISTORY:
;*    DATE     NAME  COMMENT
;*    03.08.95 ah    initial release
;***************

	fpu

	XDEF  RayIntersect__PLANE__TP03RAY
	XDEF  RayIntersect__SPHERE__TP03RAY
	XDEF  RayIntersect__TRIANGLE__TP03RAY
	XDEF  _VecSub
	XDEF  _VecAdd
	XDEF  _VecScale
	XDEF  _VecComb
	XDEF  _VecAddScaled
	XDEF  _VecSubScaled
	XDEF  _dotp__r
	XDEF  _Abs__r
	XDEF  _MinimizeVector
	XDEF  _MaximizeVector
	XDEF  _minVector
	XDEF  _maxVector
	IFD   __MAXON__
	XDEF  VecNormalize__r_P06VECTOR
	XDEF  VecNormCross__r_P06VECTORP06VECTORP06VECTOR
	ELSE
	XDEF  VecNormalize___r_P06VECTOR
	XDEF  VecNormCross___r_P06VECTORP06VECTORP06VECTOR
	ENDC
	XDEF  VecCross__P06VECTORP06VECTORP06VECTOR
	XDEF  MultVectMat__MATRIX__TP06VECTOR
	XDEF  MultMat__MATRIX__TP06MATRIXP06MATRIX
	XDEF  MinimizeMaximizeVoxel__P05VOXELP06VECTOR
	XDEF  UnionVoxel__P05VOXELPC05VOXELPC05VOXEL
	XDEF  PointInVoxel__PC06VECTORPC05VOXEL
	XDEF  RayVoxelIntersection__PfPC06VECTORPC06VECTORPC05VOXEL

;**********
;* Structures
;**********

; VECTOR from vecmath.h

 RSRESET
VECTOR_x    rs.l  1
VECTOR_y    rs.l  1
VECTOR_z    rs.l  1
VECTOR_SIZEOF  rs.b 0

; RAY from intersec.h

 RSRESET
RAY_start   rs.b  VECTOR_SIZEOF
RAY_dir     rs.b  VECTOR_SIZEOF
RAY_lambda  rs.l  1
RAY_depth   rs.l  1
RAY_sample  rs.l  1
RAY_time    rs.l  1
RAY_obj     rs.l  1
RAY_enter   rs.w  1
RAY_SIZEOF  rs.b  0

; VOXEL from voxel.h

 RSRESET
VOXEL_min   rs.b  VECTOR_SIZEOF
VOXEL_max   rs.b  VECTOR_SIZEOF
VOXEL_SIZEOF   rs.b  0

; LIST from list.h

 RSRESET
LIST_next   rs.l  1
OBJECT_VMT  rs.l  1
LIST_SIZEOF rs.b  0

; OBJECT from object.h

 RSRESET
OBJECT_LIST    rs.b  LIST_SIZEOF
OBJECT_voxel   rs.b  VOXEL_SIZEOF
OBJECT_flags   rs.l  1
OBJECT_identification   rs.l  1
OBJECT_surf    rs.l  1
OBJECT_actor   rs.l  1
OBJECT_time    rs.l  1
OBJECT_SIZEOF  rs.b  0

; PLANE from plane.h

 RSRESET
PLANE_OBJECT   rs.b  OBJECT_SIZEOF
PLANE_d        rs.l  1
PLANE_norm     rs.b  VECTOR_SIZEOF
PLANE_pos      rs.b  VECTOR_SIZEOF
PLANE_SIZEOF   rs.b  0

; SPHERE from sphere.h

 RSRESET
SPHERE_OBJECT  rs.b  OBJECT_SIZEOF
SPHERE_r       rs.l  1
SPHERE_rsq     rs.l  1
SPHERE_ir      rs.l  1
SPHERE_irsq    rs.l  1
SPHERE_inv_delta_irsq   rs.l  1
SPHERE_pos     rs.b  VECTOR_SIZEOF
SPHERE_SIZEOF  rs.b  0

; TRIANGLE from triangle.h

 RSRESET
TRIANGLE_OBJECT   rs.b  OBJECT_SIZEOF
TRIANGLE_p0    rs.b  VECTOR_SIZEOF
TRIANGLE_p1    rs.b  VECTOR_SIZEOF
TRIANGLE_p2    rs.b  VECTOR_SIZEOF
TRIANGLE_vnorm rs.l  1
TRIANGLE_norm  rs.b  VECTOR_SIZEOF
TRIANGLE_e0    rs.b  VECTOR_SIZEOF
TRIANGLE_e1    rs.b  VECTOR_SIZEOF
TRIANGLE_e2    rs.b  VECTOR_SIZEOF
TRIANGLE_d     rs.l  1
TRIANGLE_b0    rs.l  1
TRIANGLE_b1    rs.l  1
TRIANGLE_b2    rs.l  1
TRIANGLE_SIZEOF   rs.b  0

;**********
;* Constants
;**********

EPSILON  EQU.s 1e-6
INFINITY EQU.s 1e+17

OBJECT_XNORMAL EQU   $0020
OBJECT_YNORMAL EQU   $0040
OBJECT_ZNORMAL EQU   $0080

;***************
;* DESCRIPTION:   Assembler versions of functions for plane object class
;***************

	SECTION ":0",CODE
;BOOL PLANE::RayIntersect(RAY *ray)
RayIntersect__PLANE__TP03RAY
PLANE_stack EQU   12
PLANE_this  EQU   12+4
PLANE_ray   EQU   12+8
	fmove.x  fp7,-(a7)
;  d = dotp(&norm, &ray->dir);
	move.l   PLANE_ray(a7),a0
	fmove.s  RAY_dir+VECTOR_x(a0),fp7
	move.l   PLANE_this(a7),a1
	fmove.s  RAY_dir+VECTOR_y(a0),fp1
	fmul.s   PLANE_norm+VECTOR_x(a1),fp7
	fmul.s   PLANE_norm+VECTOR_y(a1),fp1
	fadd.x   fp1,fp7
	fmove.s  RAY_dir+VECTOR_z(a0),fp1
	fmul.s   PLANE_norm+VECTOR_z(a1),fp1
	fadd.x   fp1,fp7
	fmove.x  fp7,fp0
;  if (Abs(d) < EPSILON)
	fabs.x   fp0
	fcmp.s   #EPSILON,fp0
	fbuge.w  PL4
;     return FALSE;
	moveq    #0,d0
	fmove.x  (a7)+,fp7
	rts
PL4
;  d = (this->d - dotp(&norm, &ray->start)) / d;
	fmove.s  RAY_start+VECTOR_x(a0),fp0
	fmove.s  RAY_start+VECTOR_y(a0),fp1
	fmul.s   PLANE_norm+VECTOR_x(a1),fp0
	fmul.s   PLANE_norm+VECTOR_y(a1),fp1
	fadd.x   fp1,fp0
	fmove.s  RAY_start+VECTOR_z(a0),fp1
	fmul.s   PLANE_norm+VECTOR_z(a1),fp1
	fadd.x   fp1,fp0
	fmove.s  PLANE_d(a1),fp1
	fsub.x   fp0,fp1
	fdiv.x   fp7,fp1
;  if (d > EPSILON && d < ray->lambda)
	fcmp.s   #EPSILON,fp1
	fbule.w  PL5
	fcmp.s   RAY_lambda(a0),fp1
	fbuge.w  PL5
;  {
;     ray->lambda = d;
	fmove.s  fp1,RAY_lambda(a0)
;     ray->obj = this;
	move.l   PLANE_this(a7),RAY_obj(a0)
;     return TRUE;
	moveq    #1,d0
	fmove.x  (a7)+,fp7
	rts
;  }
PL5
;  return FALSE;
	moveq    #0,d0
	fmove.x  (a7)+,fp7
	rts

;***************
;* DESCRIPTION:   Assembler versions of functions for sphere object class
;***************

;BOOL SPHERE::RayIntersect(RAY *ray)
RayIntersect__SPHERE__TP03RAY
SP11  EQU   -36
	link     a5,#SP11+12
	fmove.x  fp7,-(a7)
;  VecSub(pos, ray->start, &adj);
	move.l   $C(a5),a1
	move.l   $8(a5),a0
	fmove.s  SPHERE_pos+VECTOR_x(a0),fp0
	fmove.s  SPHERE_pos+VECTOR_y(a0),fp1
	fsub.s   RAY_start+VECTOR_x(a1),fp0
	fsub.s   RAY_start+VECTOR_y(a1),fp1
	fmove.s  fp0,-$C(a5)
	fmove.s  fp1,-$8(a5)
	fmove.s  SPHERE_pos+VECTOR_z(a0),fp0
	fsub.s   RAY_start+VECTOR_z(a1),fp0
	fmove.s  fp0,-4(a5)
;  b = adj.x * ray->dir.x + adj.y * ray->dir.y + adj.z * ray->dir.z;
	fmove.s  -$C(a5),fp7
	fmove.s  -$8(a5),fp0
	fmul.s   RAY_dir+VECTOR_x(a1),fp7
	fmul.s   RAY_dir+VECTOR_y(a1),fp0
	fadd.x   fp0,fp7
	fmove.s  -4(a5),fp0
	fmul.s   RAY_dir+VECTOR_z(a1),fp0
	fadd.x   fp0,fp7
	fmove.x  fp7,fp1
;  t = b * b - adj.x * adj.x - adj.y * adj.y - adj.z * adj.z + rsq;
	fmul.x   fp7,fp7
	fmove.s  -$C(a5),fp0
	fmul.x   fp0,fp0
	fsub.x   fp0,fp7
	fmove.s  -$8(a5),fp0
	fmul.x   fp0,fp0
	fsub.x   fp0,fp7
	fmove.s  -4(a5),fp0
	fmul.x   fp0,fp0
	fsub.x   fp0,fp7
	fadd.s   SPHERE_rsq(a0),fp7
;  if (t < 0.)
	fcmp.s   #0,fp7
	fbuge.w  SP4
;     return FALSE;
	moveq    #0,d0
	fmove.x  (a7)+,fp7
	unlk     a5
	rts
SP4
;  t = (float)sqrt((double)t);
	fsqrt.x  fp7
;  s = b - t;
	fmove.x  fp1,fp0
	fsub.x   fp7,fp0
;  if(s>0.)
	fcmp.s   #0,fp0
	fbule.w  SP5
;  if(s<ray->lambda)
	fcmp.s   RAY_lambda(a1),fp0
	fbuge.w  SP5

;     ray->lambda = s;
	fmove.s  fp0,RAY_lambda(a1)
;     ray->obj = this;
	move.l   $8(a5),RAY_obj(a1)
;     ray->enter = TRUE;
	move.w   #1,RAY_enter(a1)
;     return TRUE;
	moveq    #1,d0
	fmove.x  (a7)+,fp7
	unlk     a5
	rts

SP5
;  s = b + t;
	fadd.x   fp7,fp1
;  if(s>0.)
	fcmp.s   #0,fp1
	fbule.w  SP6
;  if(s<ray->lambda)
	fcmp.s   RAY_lambda(a1),fp1
	fbuge.w  SP6
;     ray->lambda = s;
	fmove.s  fp1,RAY_lambda(a1)
;     ray->obj = this;
	move.l   $8(a5),RAY_obj(a1)
;     ray->enter = FALSE;
	move.w   #0,RAY_enter(a1)
;     return TRUE;
	moveq    #1,d0
	fmove.x  (a7)+,fp7
	unlk     a5
	rts

SP6
;  return FALSE;
	moveq    #0,d0
	fmove.x  (a7)+,fp7
	unlk     a5
	rts

;***************
;* DESCRIPTION:   Assembler versions of functions for triangle object class
;***************

;BOOL TRIANGLE::RayIntersect(RAY *ray)
RayIntersect__TRIANGLE__TP03RAY
TRIANGLE_stack EQU   32
TRIANGLE_this  EQU   32+4
TRIANGLE_ray   EQU   32+8
	movem.l  d2-d5/a2,-(a7)
	fmove.x  fp6,-(a7)
	move.l   TRIANGLE_ray(a7),a2
;  d = dotp(&norm, &dir);
	lea      RAY_dir(a2),a0
	move.l   TRIANGLE_this(a7),a1
	fmove.s  TRIANGLE_norm+VECTOR_x(a1),fp0
	fmul.s   (a0),fp0
	fmove.s  TRIANGLE_norm+VECTOR_y(a1),fp1
	fmul.s   4(a0),fp1
	fadd.x   fp1,fp0
	fmove.s  TRIANGLE_norm+VECTOR_z(a1),fp1
	fmul.s   8(a0),fp1
	fadd.x   fp1,fp0
	fmove.x  fp0,fp6
;  if (Abs(d) < EPSILON)
	fabs.x   fp0
	fcmp.s   #EPSILON,fp0
	fbuge.w  L16
;     return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L16
;  d = (this->d - dotp(&norm, &pos)) / d;
	lea      RAY_start(a2),a0
	fmove.s  TRIANGLE_norm+VECTOR_x(a1),fp0
	fmul.s   (a0),fp0
	fmove.s  TRIANGLE_norm+VECTOR_y(a1),fp1
	fmul.s   4(a0),fp1
	fadd.x   fp1,fp0
	fmove.s  TRIANGLE_norm+VECTOR_z(a1),fp1
	fmul.s   $8(a0),fp1
	fadd.x   fp1,fp0
	fmove.s  TRIANGLE_d(a1),fp1
	fsub.x   fp0,fp1
	fdiv.x   fp6,fp1
	fmove.s  fp1,d2
;  if (d < EPSILON || d > ray->lambda)
	fcmp.s   #EPSILON,fp1
	fbolt.w  L74
	fcmp.s   RAY_lambda(a2),fp1
	fbule.w  L17
L74
;     return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L17
;  if (flags & OBJECT_XNORMAL)
	move.l   OBJECT_flags(a1),d0
	and.l    #OBJECT_XNORMAL,d0
	beq      L21
;     qi1 = pos.y + d*dir.y;
;     qi2 = pos.z + d*dir.z;
	fmove.x  fp1,fp0
	fmul.s   RAY_dir+VECTOR_y(a2),fp1
	fmul.s   RAY_dir+VECTOR_z(a2),fp0
	fadd.s   RAY_start+VECTOR_y(a2),fp1
	fadd.s   RAY_start+VECTOR_z(a2),fp0
	fmove.s  fp1,d1
	fmove.s  fp0,d0
;     b0 = e[1].y*(qi2 - p[1].z) - e[1].z*(qi1 - p[1].y);
	fsub.s   TRIANGLE_p1+VECTOR_z(a1),fp0
	fsub.s   TRIANGLE_p1+VECTOR_y(a1),fp1
	fmul.s   TRIANGLE_e1+VECTOR_y(a1),fp0
	fmul.s   TRIANGLE_e1+VECTOR_z(a1),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,d5
;     if (b0 < 0. || b0 > 1.)
	fcmp.s   #0,fp0
	fbolt.w  L75
	fcmp.s   #1.,fp0
	fbule.w  L18
L75
;        return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L18
;     b1 = e[2].y*(qi2 - p[2].z) - e[2].z*(qi1 - p[2].y);
	fmove.s  d0,fp0
	fmove.s  d1,fp1
	fsub.s   TRIANGLE_p2+VECTOR_z(a1),fp0
	fsub.s   TRIANGLE_p2+VECTOR_y(a1),fp1
	fmul.s   TRIANGLE_e2+VECTOR_y(a1),fp0
	fmul.s   TRIANGLE_e2+VECTOR_z(a1),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,d4
;     if (b1 < 0. || b1 > 1.)
	fcmp.s   #0,fp0
	fbolt.w  L76
	fcmp.s   #1.,fp0
	fbule.w  L19
L76
;        return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L19
;     b2 = e[0].y*(qi2 - p[0].z) - e[0].z*(qi1 - p[0].y);
	fmove.s  d0,fp0
	fmove.s  d1,fp1
	fsub.s   TRIANGLE_p0+VECTOR_z(a1),fp0
	fsub.s   TRIANGLE_p0+VECTOR_y(a1),fp1
	fmul.s   TRIANGLE_e0+VECTOR_y(a1),fp0
	fmul.s   TRIANGLE_e0+VECTOR_z(a1),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,d3
;     if (b2 < 0. || b2 > 1.)
	fcmp.s   #0,fp0
	fbolt.w  L77
	fcmp.s   #1.,fp0
	fbule.w  L30
L77
;        return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L20
;        return
	bra   L30
L21
;     if (flags & OBJECT_YNORMAL)
	move.l   OBJECT_flags(a1),d0
	and.l    #OBJECT_YNORMAL,d0
	beq      L25
;        qi1 = pos.x + d*dir.x;
;        qi2 = pos.z + d*dir.z;
	fmove.x  fp1,fp0
	fmul.s   RAY_dir+VECTOR_x(a2),fp1
	fmul.s   RAY_dir+VECTOR_z(a2),fp0
	fadd.s   RAY_start+VECTOR_x(a2),fp1
	fadd.s   RAY_start+VECTOR_z(a2),fp0
	fmove.s  fp1,d1
	fmove.s  fp0,d0
;        b0 = e[1].z*(qi1 - p[1].x) - e[1].x*(qi2 - p[1].z);
	fsub.s   TRIANGLE_p1+VECTOR_x(a1),fp1
	fsub.s   TRIANGLE_p1+VECTOR_z(a1),fp0
	fmul.s   TRIANGLE_e1+VECTOR_z(a1),fp1
	fmul.s   TRIANGLE_e1+VECTOR_x(a1),fp0
	fsub.x   fp0,fp1
	fmove.s  fp1,d5
;        if (b0 < 0. || b0 > 1.)
	fcmp.s   #0,fp1
	fbolt.w  L78
	fcmp.s   #1.,fp1
	fbule.w  L22
L78
;           return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L22
;        b1 = e[2].z*(qi1 - p[2].x) - e[2].x*(qi2 - p[2].z);
	fmove.s  d1,fp0
	fmove.s  d0,fp1
	fsub.s   TRIANGLE_p2+VECTOR_x(a1),fp0
	fsub.s   TRIANGLE_p2+VECTOR_z(a1),fp1
	fmul.s   TRIANGLE_e2+VECTOR_z(a1),fp0
	fmul.s   TRIANGLE_e2+VECTOR_x(a1),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,d4
;        if (b1 < 0. || b1 > 1.)
	fcmp.s   #0,fp0
	fbolt.w  L79
	fcmp.s   #1.,fp0
	fbule.w  L23
L79
;           return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L23
;        b2 = e[0].z*(qi1 - p[0].x) - e[0].x*(qi2 - p[0].z);
	fmove.s  d1,fp0
	fmove.s  d0,fp1
	fsub.s   TRIANGLE_p0+VECTOR_x(a1),fp0
	fsub.s   TRIANGLE_p0+VECTOR_z(a1),fp1
	fmul.s   TRIANGLE_e0+VECTOR_z(a1),fp0
	fmul.s   TRIANGLE_e0+VECTOR_x(a1),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,d3
;        if (b2 < 0. || b2 > 1.)
	fcmp.s   #0,fp0
	fbolt.w  L80
	fcmp.s   #1.,fp0
	fbule.w  L30
L80
;           return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L24
;           return
	bra   L30
L25
;        qi1 = pos.x + d*dir.x;
;        qi2 = pos.y + d*dir.y;
	fmove.x  fp1,fp0
	fmul.s   RAY_dir+VECTOR_x(a2),fp1
	fmul.s   RAY_dir+VECTOR_y(a2),fp0
	fadd.s   RAY_start+VECTOR_x(a2),fp1
	fadd.s   RAY_start+VECTOR_y(a2),fp0
	fmove.s  fp1,d1
	fmove.s  fp0,d0
;        b0 = e[1].x*(qi2 - p[1].y) - e[1].y*(qi1 - p[1].x);
	fsub.s   TRIANGLE_p1+VECTOR_y(a1),fp0
	fsub.s   TRIANGLE_p1+VECTOR_x(a1),fp1
	fmul.s   TRIANGLE_e1+VECTOR_x(a1),fp0
	fmul.s   TRIANGLE_e1+VECTOR_y(a1),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,d5
;        if (b0 < 0. || b0 > 1.)
	fcmp.s   #0,fp0
	fbolt.w  L81
	fcmp.s   #1.,fp0
	fbule.w  L26
L81
;           return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L26
;        b1 = e[2].x*(qi2 - p[2].y) - e[2].y*(qi1 - p[2].x);
	fmove.s  d0,fp0
	fmove.s  d1,fp1
	fsub.s   TRIANGLE_p2+VECTOR_y(a1),fp0
	fsub.s   TRIANGLE_p2+VECTOR_x(a1),fp1
	fmul.s   TRIANGLE_e2+VECTOR_x(a1),fp0
	fmul.s   TRIANGLE_e2+VECTOR_y(a1),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,d4
;        if (b1 < 0. || b1 > 1.)
	fcmp.s   #0,fp0
	fbolt.w  L82
	fcmp.s   #1.,fp0
	fbule.w  L27
L82
;           return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L27
;        b2 = e[0].x*(qi2 - p[0].y) - e[0].y*(qi1 - p[0].x);
	fmove.s  d0,fp0
	fmove.s  d1,fp1
	fsub.s   TRIANGLE_p0+VECTOR_y(a1),fp0
	fsub.s   TRIANGLE_p0+VECTOR_x(a1),fp1
	fmul.s   TRIANGLE_e0+VECTOR_x(a1),fp0
	fmul.s   TRIANGLE_e0+VECTOR_y(a1),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,d3
;        if (b2 < 0. || b2 > 1.)
	fcmp.s   #0,fp0
	fbolt.w  L83
	fcmp.s   #1.,fp0
	fbule.w  L30
L83
;           return
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L28
;           return
L29
;           return
L30
;  b[0] = b0;
	move.l   d5,TRIANGLE_b0(a1)
;  b[1] = b1;
	move.l   d4,TRIANGLE_b1(a1)
;  b[2] = b2;
	move.l   d3,TRIANGLE_b2(a1)
;  if(d < ray->lambda && d > EPSILON)
	fmove.s  d2,fp0
	fcmp.s   RAY_lambda(a2),fp0
	fbuge.w  L31
	fcmp.s   #EPSILON,fp0
	fbule.w  L31
;
;     ray->lambda = d;
	move.l   d2,RAY_lambda(a2)
;     ray->obj = this;
	move.l   TRIANGLE_this(a7),RAY_obj(a2)
;     return TRUE;
	moveq    #1,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts
L31
;  return FALSE;
	moveq    #0,d0
	fmove.x  (a7)+,fp6
	movem.l  (a7)+,d2-d5/a2
	rts

_dotp__r
	move.l   $8(a7),a0
	move.l   4(a7),a1
	fmove.s  VECTOR_x(a1),fp0
	fmove.s  VECTOR_y(a1),fp1
	fmul.s   VECTOR_x(a0),fp0
	fmul.s   VECTOR_y(a0),fp1
	fadd.x   fp1,fp0
	fmove.s  VECTOR_z(a1),fp1
	fmul.s   VECTOR_z(a0),fp1
	fadd.x   fp1,fp0
	rts

_VecAbs__r
	move.l   4(a7),a0
	fmove.s  VECTOR_x(a0),fp0
	fmove.s  VECTOR_y(a0),fp1
	fmul.x   fp0,fp0
	fmul.x   fp1,fp1
	fadd.x   fp1,fp0
	fmove.s  VECTOR_z(a0),fp1
	fmul.x   fp1,fp1
	fadd.x   fp1,fp0
	fsqrt.x  fp0
	rts

_VecSub
	move.l   a2,-(a7)
	move.l   8(a7),a0
	move.l   $C(a7),a1
	move.l   $10(a7),a2
	fmove.s  VECTOR_x(a0),fp0
	fmove.s  VECTOR_y(a0),fp1
	fsub.s   VECTOR_x(a1),fp0
	fsub.s   VECTOR_y(a1),fp1
	fmove.s  fp0,VECTOR_x(a2)
	fmove.s  fp1,VECTOR_y(a2)
	fmove.s  VECTOR_z(a0),fp0
	fsub.s   VECTOR_z(a1),fp0
	fmove.s  fp0,VECTOR_z(a2)
	move.l   (a7)+,a2
	rts

_VecAdd
	move.l   a2,-(a7)
	move.l   8(a7),a0
	move.l   $C(a7),a1
	move.l   $10(a7),a2
	fmove.s  VECTOR_x(a0),fp0
	fmove.s  VECTOR_y(a0),fp1
	fadd.s   VECTOR_x(a1),fp0
	fadd.s   VECTOR_y(a1),fp1
	fmove.s  fp0,VECTOR_x(a2)
	fmove.s  fp1,VECTOR_y(a2)
	fmove.s  VECTOR_z(a0),fp0
	fadd.s   VECTOR_z(a1),fp0
	fmove.s  fp0,VECTOR_z(a2)
	move.l   (a7)+,a2
	rts

_VecScale
	fmove.s  4(a7),fp0
	move.l   8(a7),a0
	move.l   12(a7),a1
	fmove.x  fp0,fp1
	fmul.s   VECTOR_x(a0),fp1
	fmove.s  fp1,VECTOR_x(a1)
	fmove.x  fp0,fp1
	fmul.s   VECTOR_y(a0),fp1
	fmove.s  fp1,VECTOR_y(a1)
	fmove.x  fp0,fp1
	fmul.s   VECTOR_z(a0),fp1
	fmove.s  fp1,VECTOR_z(a1)
	rts

_VecComb
MA13  EQU   28
	fmovem.x fp6/fp7,-(a7)
	move.l   a2,-(a7)
	fmove.s  MA13+4(a7),fp0
	move.l   MA13+8(a7),a0
	fmove.s  MA13+12(a7),fp1
	move.l   MA13+16(a7),a1
	move.l   MA13+20(a7),a2
	fmove.x  fp0,fp7
	fmul.s   VECTOR_x(a0),fp7
	fmove.x  fp1,fp6
	fmul.s   VECTOR_x(a1),fp6
	fadd.x   fp6,fp7
	fmove.s  fp7,VECTOR_x(a2)
	fmove.x  fp0,fp7
	fmul.s   VECTOR_y(a0),fp7
	fmove.x  fp1,fp6
	fmul.s   VECTOR_y(a1),fp6
	fadd.x   fp6,fp7
	fmove.s  fp7,VECTOR_y(a2)
	fmove.x  fp0,fp7
	fmul.s   VECTOR_z(a0),fp7
	fmove.x  fp1,fp6
	fmul.s   VECTOR_z(a1),fp6
	fadd.x   fp6,fp7
	fmove.s  fp7,VECTOR_z(a2)
	move.l   (a7)+,a2
	fmovem.x (a7)+,fp6/fp7
	rts

_VecAddScaled
MA15  EQU   4
	move.l   a2,-(a7)
	move.l   MA15+4(a7),a0
	fmove.s  MA15+8(a7),fp0
	move.l   MA15+12(a7),a1
	move.l   MA15+16(a7),a2
	fmove.x  fp0,fp1
	fmul.s   VECTOR_x(a1),fp1
	fadd.s   VECTOR_x(a0),fp1
	fmove.s  fp1,VECTOR_x(a2)
	fmove.x  fp0,fp1
	fmul.s   VECTOR_y(a1),fp1
	fadd.s   VECTOR_y(a0),fp1
	fmove.s  fp1,VECTOR_y(a2)
	fmove.x  fp0,fp1
	fmul.s   VECTOR_z(a1),fp1
	fadd.s   VECTOR_z(a0),fp1
	fmove.s  fp1,VECTOR_z(a2)
	move.l   (a7)+,a2
	rts

_VecSubScaled
MA17  EQU   4
	move.l   a2,-(a7)
	move.l   MA17+4(a7),a0
	fmove.s  MA17+8(a7),fp0
	move.l   MA17+12(a7),a1
	move.l   MA17+16(a7),a2
	fmove.s  VECTOR_x(a0),fp1
	fsub.s   VECTOR_x(a1),fp1
	fmul.x   fp0,fp1
	fmove.s  fp1,VECTOR_x(a2)
	fmove.s  VECTOR_y(a0),fp1
	fsub.s   VECTOR_y(a1),fp1
	fmul.x   fp0,fp1
	fmove.s  fp1,VECTOR_y(a2)
	fmove.s  VECTOR_z(a0),fp1
	fsub.s   VECTOR_z(a1),fp1
	fmul.x   fp0,fp1
	fmove.s  fp1,VECTOR_z(a2)
	move.l   (a7)+,a2
	rts

_Abs__r
	fmove.s  4(a7),fp0
	fabs.x   fp0
	rts

_MinimizeVector
MA23  EQU   $4 ;$14
	movem.l  a2,-(a7)
	move.l   MA23+$C(a7),a0
	move.l   MA23+$8(a7),a1
	move.l   MA23+4(a7),a2
	fmove.s  VECTOR_x(a1),fp0
	fcmp.s   VECTOR_x(a0),fp0
	fbuge.w  MA25
	move.l   VECTOR_x(a1),VECTOR_x(a2)
	bra.b MA26
MA25
	move.l   VECTOR_x(a0),VECTOR_x(a2)
MA26
	fmove.s  VECTOR_y(a1),fp0
	fcmp.s   VECTOR_y(a0),fp0
	fbuge.w  MA27
	move.l   VECTOR_y(a1),VECTOR_y(a2)
	bra.b MA28
MA27
	move.l   VECTOR_y(a0),VECTOR_y(a2)
MA28
	fmove.s  VECTOR_z(a1),fp0
	fcmp.s   VECTOR_z(a0),fp0
	fbuge.w  MA29
	move.l   VECTOR_z(a1),VECTOR_z(a2)
	bra.b MA30
MA29
	move.l   VECTOR_z(a0),VECTOR_z(a2)
MA30
	movem.l  (a7)+,a2
	rts

_MaximizeVector
MA31  EQU   $4 ;$14
	movem.l  a2,-(a7)
	move.l   MA31+$C(a7),a0
	move.l   MA31+$8(a7),a1
	move.l   MA31+4(a7),a2
	fmove.s  VECTOR_x(a1),fp0
	fcmp.s   VECTOR_x(a0),fp0
	fbule.w  MA33
	move.l   VECTOR_x(a1),VECTOR_x(a2)
	bra.b MA34
MA33
	move.l   VECTOR_x(a0),VECTOR_x(a2)
MA34
	fmove.s  VECTOR_y(a1),fp0
	fcmp.s   VECTOR_y(a0),fp0
	fbule.w  MA35
	move.l   VECTOR_y(a1),VECTOR_y(a2)
	bra.b MA36
MA35
	move.l   VECTOR_y(a0),VECTOR_y(a2)
MA36
	fmove.s  VECTOR_z(a1),fp0
	fcmp.s   VECTOR_z(a0),fp0
	fbule.w  MA37
	move.l   VECTOR_z(a1),VECTOR_z(a2)
	bra.b MA38
MA37
	move.l   VECTOR_z(a0),VECTOR_z(a2)
MA38
	movem.l  (a7)+,a2
	rts

	SECTION ":1",DATA

_minVector
	dc.s  -1e17
	dc.s  -1e17
	dc.s  -1e17
_maxVector
	dc.s  1e17
	dc.s  1e17
	dc.s  1e17

	SECTION ":0",CODE

	IFD   __MAXON__
VecNormalize__r_P06VECTOR
	ELSE
VecNormalize___r_P06VECTOR
	ENDC
L9 EQU      $c
	fmove.x fp6,-(a7)
	move.l   L9+4(a7),a0
	fmove.s  (a0),fp0
	fmul.x   fp0,fp0
	fmove.s  4(a0),fp6
	fmul.x   fp6,fp6
	fadd.x   fp6,fp0
	fmove.s  $8(a0),fp6
	fmul.x   fp6,fp6
	fadd.x   fp6,fp0
	fsqrt.x  fp0
	fabs.x   fp0
	fcmp.d   #1e-6,fp0
	fbuge.w  L1
	fmove.s  #0,fp0
	fmove.x (a7)+,fp6
	rts
L1
	fmove.x  fp0,fp1
	fmove.x  #1,fp1
	fdiv.x   fp0,fp1
	fmove.s  (a0),fp6
	fmul.x   fp1,fp6
	fmove.s  fp6,(a0)
	fmove.s  4(a0),fp6
	fmul.x   fp1,fp6
	fmove.s  fp6,4(a0)
	fmove.s  8(a0),fp6
	fmul.x   fp1,fp6
	fmove.s  fp6,8(a0)
	fmove.x (a7)+,fp6
	rts

	IFD   __MAXON__
VecNormCross__r_P06VECTORP06VECTORP06VECTOR
	ELSE
VecNormCross___r_P06VECTORP06VECTORP06VECTOR
	ENDC
L11   EQU   -4
	link     a5,#L11+4
	move.l   a2,-(a7)
	move.l   $10(a5),a2
	move.l   a2,-(a7)
	move.l   $C(a5),-(a7)
	move.l   $8(a5),-(a7)
	jsr      VecCross__P06VECTORP06VECTORP06VECTOR
	lea      $C(a7),a7
	move.l   a2,-(a7)
	IFD      __MAXON__
	bsr      VecNormalize__r_P06VECTOR
	ELSE
	bsr      VecNormalize___r_P06VECTOR
	ENDC
	addq.l   #4,a7
	move.l   (a7)+,a2
	unlk     a5
	rts

VecCross__P06VECTORP06VECTORP06VECTOR
L13   EQU   $4 ;$20
L14   EQU   $1400
	movem.l  a2,-(a7)
	move.l   L13+$8(a7),a0
	move.l   L13+4(a7),a2
	move.l   L13+$C(a7),a1
	fmove.s  4(a2),fp0
	fmul.s   $8(a0),fp0
	fmove.s  $8(a2),fp1
	fmul.s   4(a0),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,(a1)
	fmove.s  $8(a2),fp0
	fmul.s   (a0),fp0
	fmove.s  (a2),fp1
	fmul.s   $8(a0),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,4(a1)
	fmove.s  (a2),fp0
	fmul.s   4(a0),fp0
	fmove.s  4(a2),fp1
	fmul.s   (a0),fp1
	fsub.x   fp1,fp0
	fmove.s  fp0,$8(a1)
	movem.l  (a7)+,a2
	rts

;BOOL MATRIX::MultVectMat(VECTOR *v)
MultVectMat__MATRIX__TP06VECTOR
VEL36 EQU   $30
	fmovem.x fp2/fp3/fp6/fp7,-(a7)
	move.l   VEL36+$8(a7),a0
	move.l   VEL36+4(a7),a1
;  x = v->x;
	fmove.s  (a0),fp0
;  y = v->y;
	fmove.s  4(a0),fp1
;  z = v->z;
	fmove.s  $8(a0),fp2
;  w = m[0] + x*m[4] + y*m[8] + z*m[12];
	fmove.x  fp0,fp7
	fmul.s   16(a1),fp7
	fmove.s  (a1),fp3
	fadd.x   fp7,fp3
	fmove.x  fp1,fp7
	fmul.s   32(a1),fp7
	fadd.x   fp7,fp3
	fmove.x  fp2,fp7
	fmul.s   48(a1),fp7
	fadd.x   fp7,fp3
;  if (Abs(w) < EPSILON)
	fmove.x  fp3,fp6
	fabs.x   fp6
	fcmp.s   #1e-6,fp6
	fbuge.w  VEL9
;     return
	moveq #0,d0
	fmovem.x (a7)+,fp2/fp3/fp6/fp7
	movem.l  (a7)+,d2
	rts
VEL9
;  w = 1/w;
	fmove.s  #1,fp6
	fdiv.x   fp3,fp6
	fmove.x  fp6,fp3
;  v->x = (m[1] + x*m[5] + y*m[9] + z*m[13])*w;
	fmove.x  fp0,fp6
	fmul.s   20(a1),fp6
	fmove.s  4(a1),fp7
	fadd.x   fp7,fp6
	fmove.x  fp1,fp7
	fmul.s   36(a1),fp7
	fadd.x   fp7,fp6
	fmove.x  fp2,fp7
	fmul.s   52(a1),fp7
	fadd.x   fp7,fp6
	fmul.x   fp3,fp6
	fmove.s  fp6,(a0)
;  v->y = (m[2] + x*m[6] + y*m[10] + z*m[14])*w;
	fmove.x  fp0,fp6
	fmul.s   24(a1),fp6
	fmove.s  8(a1),fp7
	fadd.x   fp7,fp6
	fmove.x  fp1,fp7
	fmul.s   40(a1),fp7
	fadd.x   fp7,fp6
	fmove.x  fp2,fp7
	fmul.s   56(a1),fp7
	fadd.x   fp7,fp6
	fmul.x   fp3,fp6
	fmove.s  fp6,4(a0)
;  v->z = (m[3] + x*m[7] + y*m[11] + z*m[15])*w;
	fmove.x  fp0,fp6
	fmul.s   28(a1),fp6
	fmove.s  12(a1),fp7
	fadd.x   fp7,fp6
	fmove.x  fp1,fp7
	fmul.s   44(a1),fp7
	fadd.x   fp7,fp6
	fmove.x  fp2,fp7
	fmul.s   60(a1),fp7
	fadd.x   fp7,fp6
	fmul.x   fp3,fp6
	fmove.s  fp6,$8(a0)
;  return
	moveq #1,d0
	fmovem.x (a7)+,fp2/fp3/fp6/fp7
	rts
;  return

;void MATRIX::MultMat(MATRIX *ma,MATRIX *mb)
MultMat__MATRIX__TP06MATRIXP06MATRIX
VEL22 EQU   $4
	movem.l  a2,-(a7)
	move.l   VEL22+$C(a7),a0
	move.l   VEL22+4(a7),a1
	move.l   VEL22+$8(a7),a2
;  for(i=0;
	moveq #0,d0
	bra   VEL7
VEL6
;     m[i] = ma->m[0] * mb->m[i] + ma->m[1] * mb->m[4+i] +
	fmove.s  (a2),fp1
	fmul.s   (a0),fp1
	fmove.s  4(a2),fp0
	fmul.s   16(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  8(a2),fp0
	fmul.s   32(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  12(a2),fp0
	fmul.s   48(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  fp1,0(a1)
;     m[4+i] = ma->m[4] * mb->m[i] + ma->m[5] * mb->m[4+i] +
	fmove.s  16(a2),fp1
	fmul.s   (a0),fp1
	fmove.s  20(a2),fp0
	fmul.s   16(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  24(a2),fp0
	fmul.s   32(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  28(a2),fp0
	fmul.s   48(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  fp1,16(a1)
;     m[8+i] = ma->m[8] * mb->m[i] + ma->m[9] * mb->m[4+i] +
	fmove.s  32(a2),fp1
	fmul.s   (a0),fp1
	fmove.s  36(a2),fp0
	fmul.s   16(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  40(a2),fp0
	fmul.s   32(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  44(a2),fp0
	fmul.s   48(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  fp1,32(a1)
;     m[12+i] = ma->m[12] * mb->m[i] + ma->m[13] * mb->m[4+i] +
	fmove.s  48(a2),fp1
	fmul.s   (a0),fp1
	fmove.s  52(a2),fp0
	fmul.s   16(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  56(a2),fp0
	fmul.s   32(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  60(a2),fp0
	fmul.s   48(a0),fp0
	fadd.x   fp0,fp1
	fmove.s  fp1,48(a1)
VEL8
	addq.l   #1,d0
	add.l    #4,a0
	add.l    #4,a1
VEL7
	cmp.l    #4,d0
	blt      VEL6
	movem.l  (a7)+,a2
	rts

;***************
;* NAME:          voxel
;***************

;void MinimizeMaximizeVoxel(VOXEL *voxel, VECTOR *vector)
MinimizeMaximizeVoxel__P05VOXELP06VECTOR
VO22  EQU   $8
	movem.l  a2/a3,-(a7)
	move.l   VO22+$4(a7),a2
	move.l   VO22+$8(a7),a3
;  MinimizeVector(&voxel->min, &voxel->min, vector);
	move.l   a3,-(a7)
	pea      (a2)
	pea      (a2)
	jsr      _MinimizeVector
	lea      $C(a7),a7
;  MaximizeVector(&voxel->max, &voxel->max, vector);
	move.l   a3,-(a7)
	pea      $C(a2)
	pea      $C(a2)
	jsr      _MaximizeVector
	lea      $C(a7),a7
	movem.l  (a7)+,a2/a3
	rts

;void UnionVoxel(VOXEL *result, const VOXEL *voxel1, const VOXEL *vox
UnionVoxel__P05VOXELPC05VOXELPC05VOXEL
VO24  EQU   8
	movem.l  a2/a3,-(a7)
	move.l   VO24+$c(a7),a2
	move.l   VO24+$8(a7),a3
;  MinimizeVector(&result->min, &voxel1->min, &voxel2->min);
	pea      (a2)
	pea      (a3)
	move.l   VO24+$c(a7),a0
	pea      (a0)
	jsr      _MinimizeVector
	lea      $C(a7),a7
;  MaximizeVector(&result->max, &voxel1->max, &voxel2->max);
	pea      $C(a2)
	pea      $C(a3)
	move.l   VO24+$c(a7),a0
	pea      $C(a0)
	jsr      _MaximizeVector
	lea      $C(a7),a7
	movem.l  (a7)+,a2/a3
	rts

;BOOL PointInVoxel(const VECTOR *point, const VOXEL *voxel)
PointInVoxel__PC06VECTORPC05VOXEL
	move.l   4(a7),a1
	fmove.s  VECTOR_x(a1),fp0
	move.l   $8(a7),a0
	fcmp.s   VOXEL_min+VECTOR_x(a0),fp0
	fbule.w  VO29
	fcmp.s   VOXEL_max+VECTOR_x(a0),fp0
	fbugt.w  VO29
	fmove.s  VECTOR_y(a1),fp1
	fcmp.s   VOXEL_min+VECTOR_y(a0),fp1
	fbule.w  VO29
	fcmp.s   VOXEL_max+VECTOR_y(a0),fp1
	fbugt.w  VO29
	fmove.s  VECTOR_z(a1),fp0
	fcmp.s   VOXEL_min+VECTOR_z(a0),fp0
	fbule.w  VO29
	fcmp.s   VOXEL_max+VECTOR_z(a0),fp0
	fbole.w  VO28
VO29
	moveq #0,d0
	rts
VO28
	moveq #1,d0
	rts

;BOOL RayVoxelIntersection(float *lambda, const VECTOR *start, const
RayVoxelIntersection__PfPC06VECTORPC06VECTORPC05VOXEL
VO32  EQU   52
	move.l   a2,-(a7)
	fmovem.x fp2-fp5,-(a7)
	move.l   VO32+$10(a7),a0
	move.l   VO32+$C(a7),a1
	move.l   VO32+$8(a7),a2
;  nearest = -INFINITY;
	fmove.s  #-INFINITY,fp5
;  farest = INFINITY;
	fmove.s  #INFINITY,fp4
;  if(fabs(ray->x) > EPSILON)
	fmove.s  (a1),fp0
	fabs.x   fp0
	fcmp.s   #EPSILON,fp0
	fbule.w  VO5
;     help = 1/ray->x;
	fmove.s  #1,fp1
	fdiv.s   (a1),fp1
;     lambda1 = (voxel->min.x-start->x)*help;
	fmove.s  (a0),fp3
	fsub.s   (a2),fp3
	fmul.x   fp1,fp3
;     lambda2 = (voxel->max.x-start->x)*help;
	fmove.s  $C(a0),fp2
	fsub.s   (a2),fp2
	fmul.x   fp1,fp2
;     if(lambda1 > lambda2)
	fcmp.x   fp2,fp3
	fbule.w  VO1
;        help = lambda1;
	fmove.x  fp3,fp0
;        lambda1 = lambda2;
	fmove.x  fp2,fp3
;        lambda2 = help;
	fmove.x  fp0,fp2
;        lambda2 = help;
VO1
;     if(lambda1 > nearest)
	fcmp.x   fp5,fp3
	fbule.w  VO2
;        nearest = lambda1;
	fmove.x  fp3,fp5
VO2
;     if(lambda2 < farest)
	fcmp.x   fp4,fp2
	fbuge.w  VO3
;        farest = lambda2;
	fmove.x  fp2,fp4
VO3
;     if((nearest > farest) || (farest < 0.0))
	fcmp.s   #0,fp4
	fbult.w  VO7
	fcmp.x   fp4,fp5
	fbole.w  VO7
VO36
	moveq #0,d0
	fmovem.x (a7)+,fp2-fp5
	move.l   (a7)+,a2
	rts
VO5
;     if((start->x < voxel->min.x) || (start->x > voxel->max.x))
	fmove.s  (a2),fp0
	fcmp.s   (a0),fp0
	fbolt.w  VO37
	fcmp.s   $C(a0),fp0
	fbule.w  VO7
VO37
;        return
	moveq #0,d0
	fmovem.x (a7)+,fp2-fp5
	move.l   (a7)+,a2
	rts
VO6
VO7
;  if(fabs(ray->y) > EPSILON)
	fmove.s  4(a1),fp0
	fabs.x   fp0
	fcmp.s   #EPSILON,fp0
	fbule.w  VO12
;     help = 1/ray->y;
	fmove.s  #1,fp1
	fdiv.s   4(a1),fp1
;     lambda1 = (voxel->min.y-start->y)*help;
	fmove.s  4(a0),fp3
	fsub.s   4(a2),fp3
	fmul.x   fp1,fp3
;     lambda2 = (voxel->max.y-start->y)*help;
	fmove.s  $10(a0),fp2
	fsub.s   4(a2),fp2
	fmul.x   fp1,fp2
;     if(lambda1 > lambda2)
	fcmp.x   fp2,fp3
	fbule.w  VO8
;
;        help = lambda1;
	fmove.x  fp3,fp0
;        lambda1 = lambda2;
	fmove.x  fp2,fp3
;        lambda2 = help;
	fmove.x  fp0,fp2
VO8
;     if(lambda1 > nearest)
	fcmp.x   fp5,fp3
	fbule.w  VO9
;        nearest = lambda1;
	fmove.x  fp3,fp5
VO9
;     if(lambda2 < farest)
	fcmp.x   fp4,fp2
	fbuge.w  VO10
;        farest = lambda2;
	fmove.x  fp2,fp4
VO10
;     if((nearest > farest) || (farest < 0.0))
	fcmp.s   #0,fp4
	fbult.w  VO14
	fcmp.x   fp4,fp5
	fbole.w  VO14
VO40
;        return
	moveq    #0,d0
	fmovem.x (a7)+,fp2-fp5
	move.l   (a7)+,a2
	rts
VO12
;     if((start->y < voxel->min.y) || (start->y > voxel->max.y))
	fmove.s  4(a2),fp0
	fcmp.s   4(a0),fp0
	fbolt.w  VO41
	fcmp.s   $10(a0),fp0
	fbule.w  VO14
VO41
;        return
	moveq    #0,d0
	fmovem.x (a7)+,fp2-fp5
	move.l   (a7)+,a2
	rts
VO13
VO14
;  if(fabs(ray->z) > EPSIVOON)
	fmove.s  8(a1),fp0
	fabs.x   fp0
	fcmp.s   #1e-6,fp0
	fbule.w  VO19
;     help = 1/ray->z;
	fmove.s  #1,fp1
	fdiv.s   8(a1),fp1
;     lambda1 = (voxel->min.z-start->z)*help;
	fmove.s  $8(a0),fp3
	fsub.s   $8(a2),fp3
	fmul.x   fp1,fp3
;     lambda2 = (voxel->max.z-start->z)*help;
	fmove.s  $14(a0),fp2
	fsub.s   $8(a2),fp2
	fmul.x   fp1,fp2
;     if(lambda1 > lambda2)
	fcmp.x   fp2,fp3
	fbule.w  VO15
;        help = lambda1;
	fmove.x  fp3,fp0
;        lambda1 = lambda2;
	fmove.x  fp2,fp3
;        lambda2 = help;
	fmove.x  fp0,fp2
VO15
;     if(lambda1 > nearest)
	fcmp.x   fp5,fp3
	fbule.w  VO16
;        nearest = lambda1;
	fmove.x  fp3,fp5
VO16
;     if(lambda2 < farest)
	fcmp.x   fp4,fp2
	fbuge.w  VO17
;        farest = lambda2;
	fmove.x  fp2,fp4
VO17
;     if((nearest > farest) || (farest < 0.0))
	fcmp.s   #0,fp4
	fbult.w  VO21
	fcmp.x   fp4,fp5
	fbole.w  VO21
VO44
;        return
	moveq    #0,d0
	fmovem.x (a7)+,fp2-fp5
	move.l   (a7)+,a2
	rts
VO19
;
;     if((start->z < voxel->min.z) || (start->z > voxel->max.z))
	fmove.s  $8(a2),fp0
	fcmp.s   $8(a0),fp0
	fbolt.w  VO45
	fcmp.s   $14(a0),fp0
	fbule.w  VO21
VO45
;        return
	moveq    #0,d0
	fmovem.x (a7)+,fp2-fp5
	move.l   (a7)+,a2
	rts
VO20
VO21
;  *lambda = nearest;
	move.l   VO32+4(a7),a0
	fmove.s  fp5,(a0)
;  return
	moveq    #1,d0
	fmovem.x (a7)+,fp2-fp5
	move.l   (a7)+,a2
	rts

		END

