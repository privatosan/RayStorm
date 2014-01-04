/***************
 * MODUL:         csg
 * NAME:          csg.cpp
 * DESCRIPTION:   Functions for CSG class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.01.97 ah    initial release
 ***************/

#include <math.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef VOXEL_H
#include "voxel.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

#ifndef CSG_H
#include "csg.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

/*************
 * DESCRIPTION:   Constructor of CSG object (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
CSG::CSG()
{
	obj1 = NULL;
	obj2 = NULL;
	flags |= OBJECT_CSG_UNION;
}

/*************
 * DESCRIPTION:   Destructor of CSG object
 * INPUT:         none
 * OUTPUT:        none
 *************/
CSG::~CSG()
{
	if(obj1)
		delete obj1;
	if(obj2)
		delete obj2;
}

/*************
 * DESCRIPTION:   Generate Voxel
 * INPUT:         -
 * OUTPUT:        none
 *************/
void CSG::GenVoxel()
{
	obj1->GenVoxel();
	obj2->GenVoxel();

	switch(flags & OBJECT_CSG_MASK)
	{
		case OBJECT_CSG_UNION:
			// both voxels are needed for union
			UnionVoxel(&voxel, &obj1->voxel, &obj2->voxel);
			break;
		case OBJECT_CSG_INTERSECT:
			// for intersection we need the intersection of the object voxels
			voxel.min.x = MAX(obj1->voxel.min.x, obj2->voxel.min.x);
			voxel.min.y = MAX(obj1->voxel.min.y, obj2->voxel.min.y);
			voxel.min.z = MAX(obj1->voxel.min.z, obj2->voxel.min.z);
			voxel.max.x = MIN(obj1->voxel.max.x, obj2->voxel.max.x);
			voxel.max.y = MIN(obj1->voxel.max.y, obj2->voxel.max.y);
			voxel.max.z = MIN(obj1->voxel.max.z, obj2->voxel.max.z);
			break;
		case OBJECT_CSG_DIFFERENCE:
			// we only need the voxel of the first object for difference
			voxel = obj1->voxel;
			break;
	}
}

/*************
 * DESCRIPTION:   Update csg parameters
 * INPUT:         time     current time
 * OUTPUT:        none
 *************/
void CSG::Update(const float time)
{
	obj1->Update(time);
	obj2->Update(time);
}

/*************
 * DESCRIPTION:   Do union of csg-objects
 * INPUT:         ray      current ray
 *                ray1     first intersection
 *                ray2     second intersection
 *                hit1     TRUE if first object is hit
 *                hit2     TRUE if second object is hit
 *                obj1     first csg object
 *                obj2     second csg object
 * OUTPUT:        TRUE if valid intersection found
 *************/
static BOOL CSGUnion(RAY *ray, RAY *ray1, RAY *ray2, BOOL hit1, BOOL hit2, OBJECT *obj1, OBJECT *obj2)
{
	RAY newray, *tmpray;
	OBJECT *objtmp;
	BOOL hittmp;

	while(TRUE)
	{
		if(ray2->enter || !hit2)
		{
			if(ray1->lambda > ray->lambda)
				return FALSE;

			// return hit1
			*ray = *ray1;
			return TRUE;
		}
		else
		{
			newray = *ray1;
			VecAddScaled(&ray1->start, ray2->lambda*(1.f+EPSILON), &ray1->dir, &newray.start);
			newray.lambda = INFINITY;
			hit1 = obj1->RayIntersect(&newray);
			if(!hit1)
			{
				if(ray2->lambda > ray->lambda)
					return FALSE;

				// none of obj1 beyond, return hit2 (leaving)
//            ray2->enter = !ray2->enter;
				*ray = *ray2;
				return TRUE;
			}
			else
			{
				if(obj1->flags & OBJECT_INVERTED)
					ray1->enter = !newray.enter;
				else
					ray1->enter = newray.enter;
				ray1->obj = newray.obj;
				ray1->lambda = ray2->lambda*(1.f+EPSILON)+newray.lambda;
				// since hit1 is supposed to be the close one, swap them
				tmpray = ray1;
				ray1 = ray2;
				ray2 = tmpray;
				objtmp = obj1;
				obj1 = obj2;
				obj2 = objtmp;
				hittmp = hit1;
				hit1 = hit2;
				hit2 = hittmp;
			}
		}
	}
}

/*************
 * DESCRIPTION:   Do intersection of csg-objects
 * INPUT:         ray      current ray
 *                ray1     first intersection
 *                ray2     second intersection
 *                hit1     TRUE if first object is hit
 *                hit2     TRUE if second object is hit
 *                obj1     first csg object
 *                obj2     second csg object
 * OUTPUT:        TRUE if valid intersection found
 *************/
static BOOL CSGIntersection(RAY *ray, RAY *ray1, RAY *ray2, BOOL hit1, BOOL hit2, OBJECT *obj1, OBJECT *obj2)
{
	RAY newray, *tmpray;
	OBJECT *objtmp;

	while(TRUE)
	{
		if(!ray2->enter)
		{
			if(ray1->lambda > ray->lambda)
				return FALSE;

			// ray is leaving obj2, return hit1
			*ray = *ray1;
			return TRUE;
		}
		else
		{
			newray = *ray1;
			VecAddScaled(&ray1->start, ray2->lambda*(1.f+EPSILON), &ray1->dir, &newray.start);
			newray.lambda = INFINITY;
			hit1 = obj1->RayIntersect(&newray);
			if(!hit1)
			{
				// none of obj1 beyond, so return miss
				return FALSE;
			}
			else
			{
				if(obj1->flags & OBJECT_INVERTED)
					ray1->enter = !newray.enter;
				else
					ray1->enter = newray.enter;
				ray1->obj = newray.obj;
				ray1->lambda = ray2->lambda*(1.f+EPSILON)+newray.lambda;
				// since hit1 is supposed to be the close one, swap them
				tmpray = ray1;
				ray1 = ray2;
				ray2 = tmpray;
				objtmp = obj1;
				obj1 = obj2;
				obj2 = objtmp;
			}
		}
	}
	return FALSE;
}

/*************
 * DESCRIPTION:   Do difference of csg-objects
 * INPUT:         ray      result ray
 *                ray1     first intersection
 *                ray2     second intersection
 *                hit1     TRUE if first object is hit
 *                hit2     TRUE if second object is hit
 *                obj1     first csg object
 *                obj2     second csg object
 * OUTPUT:        TRUE if valid intersection found
 *************/
static BOOL CSGDifference(RAY *ray, RAY *ray1, RAY *ray2, BOOL hit1, BOOL hit2, OBJECT *obj1, OBJECT *obj2)
{
	RAY newray;

	while(TRUE)
	{
		if(ray1->lambda <= ray2->lambda)
		{
			if(ray2->enter || !hit2)
			{
				if(ray1->lambda > ray->lambda)
					return FALSE;

				// return hit1
				*ray = *ray1;
				return TRUE;
			}
			else
			{
				newray = *ray1;
				VecAddScaled(&ray1->start, ray2->lambda*(1.f+EPSILON), &ray1->dir, &newray.start);
				newray.lambda = INFINITY;
				hit1 = obj1->RayIntersect(&newray);
				if(!hit1)
				{
					// none of obj1 beyond, so return miss
					return FALSE;
				}
				ray1->enter = newray.enter;
				ray1->obj = newray.obj;
				ray1->lambda = ray2->lambda*(1.f+EPSILON)+newray.lambda;
			}
		}
		else
		{
			if(!hit1)
				return FALSE;
			if(!ray1->enter)
			{
				if(ray2->lambda > ray->lambda)
					return FALSE;

				// return hit2, but invert hit2 enter flag
				ray2->enter = !ray2->enter;
				*ray = *ray2;
				return TRUE;
			}
			else
			{
				newray = *ray2;
				VecAddScaled(&ray2->start, ray1->lambda*(1.f+EPSILON), &ray2->dir, &newray.start);
				newray.lambda = INFINITY;
				hit2 = obj2->RayIntersect(&newray);
				if(!hit2)
				{
					if(ray1->lambda > ray->lambda)
						return FALSE;

					// none of obj2 beyond, so return hit to obj1
					*ray = *ray1;
					return TRUE;
				}
				ray2->enter = newray.enter;
				ray2->obj = newray.obj;
				ray2->lambda = ray1->lambda+(1.f*EPSILON)+newray.lambda;
			}
		}
	}
	return FALSE;
}

/*************
 * DESCRIPTION:   Ray/csg intersection test
 * INPUT:         ray      Pointer to actual ray
 * OUTPUT:        TRUE if hit else FALSE
 *************/
BOOL CSG::RayIntersect(RAY *ray)
{
	BOOL hit1, hit2, hittmp;
	RAY ray1, ray2, raytmp;
	OBJECT *object1, *object2, *objtmp;
	ULONG operation;

	STATISTIC(STAT_CSGTEST);

	operation = flags & OBJECT_CSG_MASK;

	ray1 = *ray;
	hit1 = obj1->RayIntersect(&ray1);
	if(obj1->flags & OBJECT_INVERTED)
	{
		if(hit1)
		{
			ray1.enter = !ray1.enter;
		}
		else
		{
			if(operation == OBJECT_CSG_INTERSECT)
			{
				hit1 = TRUE;
				ray1.enter = FALSE;
			}
		}
	}

	if(!hit1 && ((operation == OBJECT_CSG_INTERSECT) || (operation == OBJECT_CSG_DIFFERENCE)))
	{
		/*
		 * Intersection and Difference cases: if we miss the first
		 * object, we missed the whole thing.
		 */
		return FALSE;
	}

	ray2 = *ray;
	hit2 = obj2->RayIntersect(&ray2);
	if(obj2->flags & OBJECT_INVERTED)
	{
		if(hit2)
		{
			ray2.enter = !ray2.enter;
		}
		else
		{
			if(operation == OBJECT_CSG_INTERSECT)
			{
				hit2 = TRUE;
				ray2.enter = FALSE;
			}
		}
	}

	if((!hit2 && ((operation == OBJECT_CSG_INTERSECT) || !hit1 && (operation == OBJECT_CSG_UNION))) ||
		((ray1.lambda > ray->lambda) && (ray2.lambda > ray->lambda)))
	{
		// Intersect case:  if we miss either object, we miss whole
		// Union case: if we miss both object, we miss whole
		// Both hits after previous hit
		return FALSE;
	}

	object1 = obj1;
	object2 = obj2;
	if((ray1.lambda > ray2.lambda) && (operation != OBJECT_CSG_DIFFERENCE))
	{
		// swap so 1 is closest (except in difference case)
		raytmp = ray2;
		ray2 = ray1;
		ray1 = raytmp;
		hittmp = hit2;
		hit2 = hit1;
		hit1 = hittmp;
		objtmp = object2;
		object2 = object1;
		object1 = objtmp;
	}

	// Call appropriate intersection method. If FALSE is returned,
	// no hit of any kind was found.
	switch(operation)
	{
		case OBJECT_CSG_UNION:
			if(!CSGUnion(ray, &ray1, &ray2, hit1, hit2, object1, object2))
				return FALSE;
			break;
		case OBJECT_CSG_INTERSECT:
			if(!CSGIntersection(ray, &ray1, &ray2, hit1, hit2, object1, object2))
				return FALSE;
			break;
		case OBJECT_CSG_DIFFERENCE:
			if(!CSGDifference(ray, &ray1, &ray2, hit1, hit2, object1, object2))
				return FALSE;
			break;
	}

	hitobj = ray->obj;

	STATISTIC(STAT_CSGHIT);

	return TRUE;
}

/*************
 * DESCRIPTION:   Compute normal to csg at pos
 * INPUT:         dir      direction vector
 *                hitpos   hitposition
 *                nrm      normal
 *                geon     geometric normal
 * OUTPUT:        nrm is the normal at pos
 *************/
void CSG::Normal(const VECTOR *dir, const VECTOR *hitpos, VECTOR *nrm, VECTOR *geon)
{
	hitobj->Normal(dir, hitpos, nrm, geon);
}

/*************
 * DESCRIPTION:   Duplicate csg object
 * INPUT:         -
 * OUTPUT:        FALSE if failed
 *************/
BOOL CSG::Duplicate()
{
	CSG *dup;

	dup = new CSG;
	if(!dup)
		return FALSE;

	*dup = *this;
	return TRUE;
}

/*************
 * DESCRIPTION:   Disable object so it is not tested by next intersection test
 * INPUT:         -
 * OUTPUT:        -
 *************/
void CSG::DisableObject(RSICONTEXT *rc)
{
	if(identification != rc->identification + 1)
	{
		identification = rc->identification + 1;

		if(prev)
			if(prev->identification != rc->identification + 1)
				prev->DisableObject(rc);

		if(obj1)
			obj1->DisableObject(rc);
		if(obj2)
			obj2->DisableObject(rc);
	}
}

