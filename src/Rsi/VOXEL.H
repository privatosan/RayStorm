/***************
 * MODUL:         voxel
 * NAME:          voxel.h
 * DESCRIPTION:   voxel prototypes
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 ***************/

#ifndef VOXEL_H
#define VOXEL_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

typedef struct                /* voxel */
{
	VECTOR min, max;
} VOXEL;

void MinimizeMaximizeVoxel(VOXEL *, const VECTOR *);
void UnionVoxel(VOXEL *, const VOXEL *, const VOXEL *);
BOOL PointInVoxel(const VECTOR *, const VOXEL*);
BOOL RayVoxelIntersection(float *, const VECTOR *, const VECTOR *, const VOXEL *);
void AccumulateVoxel(VOXEL*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);

#endif /* VOXEL_H */

