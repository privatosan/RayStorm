/***************
 * MODUL:         voxel
 * NAME:          voxel.cpp
 * DESCRIPTION:   Functions for voxels
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 ***************/

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef VOXEL_H
#include "voxel.h"
#endif

/*************
 * DESCRIPTION:
 * INPUT:         voxel    pointer to voxel
 *                vecotor  pointer to vector
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
void MinimizeMaximizeVoxel(VOXEL *voxel, VECTOR *vector)
{
	MinimizeVector(&voxel->min, &voxel->min, vector);
	MaximizeVector(&voxel->max, &voxel->max, vector);
}
#endif

/*************
 * DESCRIPTION:   Union two voxels
 * INPUT:         result      result
 *                voxel1      first voxel
 *                voxel2      second voxel
 * OUTPUT:        none
 *************/
#ifndef LOWLEVEL
void UnionVoxel(VOXEL *result, const VOXEL *voxel1, const VOXEL *voxel2)
{
	MinimizeVector(&result->min, &voxel1->min, &voxel2->min);
	MaximizeVector(&result->max, &voxel1->max, &voxel2->max);
}
#endif

/*************
 * DESCRIPTION:   Test if point is in voxel
 * INPUT:         point       point to test
 *                voxel       voxel to test with
 * OUTPUT:        TRUE if intersection, else FALSE
 *************/
#ifndef LOWLEVEL
BOOL PointInVoxel(const VECTOR *point, const VOXEL *voxel)
{
	return(((voxel->min.x <= point->x) && (point->x <= voxel->max.x)) &&
		((voxel->min.y <= point->y) && (point->y <= voxel->max.y)) &&
		((voxel->min.z <= point->z) && (point->z <= voxel->max.z)));
}
#endif

/*************
 * DESCRIPTION:   Test if ray intersects voxel
 * INPUT:         lambda      distance factor
 *                start       startposition
 *                ray         pointer to ray direction
 *                voxel       pointer to voxel
 * OUTPUT:        TRUE if intersection, else FALSE
 *************/
#ifndef LOWLEVEL
BOOL RayVoxelIntersection(float *lambda, const VECTOR *start, const VECTOR *ray, const VOXEL *voxel)
{
	float nearest, farest, lambda1, lambda2, help;

	nearest = -INFINITY;
	farest = INFINITY;
	if((ray->x > 0.f ? ray->x : - ray->x) > EPSILON)
	{
		help = 1.f/ray->x;
		lambda1 = (voxel->min.x-start->x)*help;
		lambda2 = (voxel->max.x-start->x)*help;
		if(lambda1 > lambda2)
		{
			help = lambda1;
			lambda1 = lambda2;
			lambda2 = help;
		}
		if(lambda1 > nearest)
		{
			nearest = lambda1;
		}
		if(lambda2 < farest)
		{
			farest = lambda2;
		}
		if((nearest > farest) || (farest < 0.f))
		{
			return(FALSE);
		}
	}
	else
	{
		if((start->x < voxel->min.x) || (start->x > voxel->max.x))
		{
			return(FALSE);
		}
	}
	if((ray->y > 0.f ? ray->y : - ray->y) > EPSILON)
	{
		help = 1.f/ray->y;
		lambda1 = (voxel->min.y-start->y)*help;
		lambda2 = (voxel->max.y-start->y)*help;
		if(lambda1 > lambda2)
		{
			help = lambda1;
			lambda1 = lambda2;
			lambda2 = help;
		}
		if(lambda1 > nearest)
		{
			nearest = lambda1;
		}
		if(lambda2 < farest)
		{
			farest = lambda2;
		}
		if((nearest > farest) || (farest < 0.f))
		{
			return(FALSE);
		}
	}
	else
	{
		if((start->y < voxel->min.y) || (start->y > voxel->max.y))
		{
			return(FALSE);
		}
	}
	if((ray->z > 0.f ? ray->z : - ray->z) > EPSILON)
	{
		help = 1.f/ray->z;
		lambda1 = (voxel->min.z-start->z)*help;
		lambda2 = (voxel->max.z-start->z)*help;
		if(lambda1 > lambda2)
		{
			help = lambda1;
			lambda1 = lambda2;
			lambda2 = help;
		}
		if(lambda1 > nearest)
		{
			nearest = lambda1;
		}
		if(lambda2 < farest)
		{
			farest = lambda2;
		}
		if((nearest > farest) || (farest < 0.f))
		{
			return(FALSE);
		}
	}
	else
	{
		if((start->z < voxel->min.z) || (start->z > voxel->max.z))
		{
			return(FALSE);
		}
	}
	*lambda = nearest;
	return TRUE;
}
#endif

/*************
 * DESCRIPTION:   Accumulate Point
 * INPUT:         voxel
 *                v           voxel is extended so that this vector fits inside
 *                pos         position
 *                ox,oy,oz    orientation
 * OUTPUT:        none
 *************/
static void AccumulatePoint(VOXEL *voxel, VECTOR *v, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz)
{
	VECTOR v1;

	v1.x = dotp(v, ox);
	v1.y = dotp(v, oy);
	v1.z = dotp(v, oz);

	VecAdd(&v1, pos, &v1);

	if(v1.x < voxel->min.x)
		voxel->min.x = v1.x;
	if(v1.x > voxel->max.x)
		voxel->max.x = v1.x;
	if(v1.y < voxel->min.y)
		voxel->min.y = v1.y;
	if(v1.y > voxel->max.y)
		voxel->max.y = v1.y;
	if(v1.z < voxel->min.z)
		voxel->min.z = v1.z;
	if(v1.z > voxel->max.z)
		voxel->max.z = v1.z;
}

/*************
 * DESCRIPTION:   Accumulate Voxel
 * INPUT:         voxel
 *                min,max
 *                pos         position
 *                ox,oy,oz    orientation
 * OUTPUT:        none
 *************/
void AccumulateVoxel(VOXEL *voxel, VECTOR *min, VECTOR *max, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz)
{
	VECTOR v;

	SetVector(&voxel->min, INFINITY, INFINITY, INFINITY);
	SetVector(&voxel->max, -INFINITY, -INFINITY, -INFINITY);

	// generate voxel
	v = *min;
	AccumulatePoint(voxel, &v, pos, ox, oy, oz);
	SetVector(&v, min->x, min->y, max->z);
	AccumulatePoint(voxel, &v, pos, ox, oy, oz);
	SetVector(&v, min->x, max->y, min->z);
	AccumulatePoint(voxel, &v, pos, ox, oy, oz);
	SetVector(&v, min->x, max->y, max->z);
	AccumulatePoint(voxel, &v, pos, ox, oy, oz);
	SetVector(&v, max->x, min->y, min->z);
	AccumulatePoint(voxel, &v, pos, ox, oy, oz);
	SetVector(&v, max->x, min->y, max->z);
	AccumulatePoint(voxel, &v, pos, ox, oy, oz);
	SetVector(&v, max->x, max->y, min->z);
	AccumulatePoint(voxel, &v, pos, ox, oy, oz);
	v = *max;
	AccumulatePoint(voxel, &v, pos, ox, oy, oz);
}

