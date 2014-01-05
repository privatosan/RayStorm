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
	XDEF  VecNormalize___r_P06VECTOR
	XDEF  VecNormalizeNoRet__P06VECTOR
	XDEF  VecNormCross___r_P06VECTORP06VECTORP06VECTOR
	XDEF  VecCross__P06VECTORP06VECTORP06VECTOR
	XDEF  MultVectMat__MATRIX__TP06VECTOR
	XDEF  MultMat__MATRIX__TP06MATRIXP06MATRIX

;**********
;* Structures
;**********

; VECTOR from vecmath.h

 RSRESET
VECTOR_x    rs.l  1
VECTOR_y    rs.l  1
VECTOR_z    rs.l  1
VECTOR_SIZEOF  rs.b 0

;**********
;* Constants
;**********

EPSILON  EQU.s 1e-6
INFINITY EQU.s 1e+17

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

VecNormalize___r_P06VECTOR
L9 EQU      $c
	fmove.x  fp6,-(a7)
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
	fcmp.d   #1e-6,fp0
	fbuge.w  L1
	fmove.s  #0,fp0
	fmove.x  (a7)+,fp6
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
	fmove.x  (a7)+,fp6
	rts

VecNormalizeNoRet__P06VECTOR
	move.l   4(a7),a0
	fmove.s  (a0),fp0
	fmul.x   fp0,fp0
	fmove.s  4(a0),fp1
	fmul.x   fp1,fp1
	fadd.x   fp1,fp0
	fmove.s  $8(a0),fp1
	fmul.x   fp1,fp1
	fadd.x   fp1,fp0
	fsqrt.x  fp0
	fcmp.d   #1e-6,fp0
	fbuge.w  L1
	rts
L2
	fmove.s  (a0),fp1
	fmul.x   fp0,fp1
	fmove.s  fp1,(a0)
	fmove.s  4(a0),fp1
	fmul.x   fp0,fp1
	fmove.s  fp1,4(a0)
	fmove.s  8(a0),fp1
	fmul.x   fp0,fp1
	fmove.s  fp1,8(a0)
	rts

VecNormCross___r_P06VECTORP06VECTORP06VECTOR
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
	bsr      VecNormalizeNoRet__P06VECTOR
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
	cmp.l #4,d0
	blt   VEL6
	movem.l  (a7)+,a2
	rts

