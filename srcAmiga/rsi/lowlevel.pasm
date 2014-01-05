;  StormC Compiler V 3.93.35.PPC.Step3

	SECTION  ":0",Code

;  BOOL PointInVoxel(const VECTOR *point, const VOXEL *voxel)
	XDEF  PointInVoxel__PC06VECTORPC05VOXEL
PointInVoxel__PC06VECTORPC05VOXEL:
	mr r5,r3 ;dst=src
	lfs   f2,0(r4)
;     return(((voxel->min.x <= point->x) && (point->x <= voxel->max.x)) 
	addi  r3,r0,0
	lfs   f1,0(r5)
	fcmpu cr1,f2,f1
	bgt   cr1,L83
	lfs   f2,12(r4)
	fcmpu cr1,f1,f2
	bgt   cr1,L83
	lfs   f2,4(r4)
	lfs   f1,4(r5)
	fcmpu cr1,f2,f1
	bgt   cr1,L83
	lfs   f1,16(r4)
	fcmpu cr1,f1,f2
	bgt   cr1,L83
	lfs   f2,8(r4)
	lfs   f1,8(r5)
	fcmpu cr1,f2,f1
	bgt   cr1,L83
	lfs   f1,20(r4)
	fcmpu cr1,f1,f2
	bgt   cr1,L83
	addi  r3,r0,1
L83
	extsh r3,r3
	blr

	SECTION  "@_PointInVoxel__PC06VECTORPC05VOXEL:1",Data
	XDEF  @_PointInVoxel__PC06VECTORPC05VOXEL
@_PointInVoxel__PC06VECTORPC05VOXEL:
	dc.l  PointInVoxel__PC06VECTORPC05VOXEL

	END


