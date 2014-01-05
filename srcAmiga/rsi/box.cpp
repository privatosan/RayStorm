/***************
 * MODUL:         box
 * NAME:          box.cpp
 * DESCRIPTION:   Functions for box object class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.01.97 ah    initial release
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

#ifndef BOX_H
#include "box.h"
#endif

/*************
 * DESCRIPTION:   Constructor of box (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
BOX::BOX()
{
	SetVector(&lowbounds, -1.f, -1.f, -1.f);
	SetVector(&highbounds, 1.f, 1.f, 1.f);
	SetVector(&orient_x, 1.f, 0.f, 0.f);
	SetVector(&orient_y, 0.f, 1.f, 0.f);
	SetVector(&orient_z, 0.f, 0.f, 1.f);
}

/*************
 * DESCRIPTION:   Generate Voxel
 * INPUT:         -
 * OUTPUT:        none
 *************/
void BOX::GenVoxel()
{
	AccumulateVoxel(&voxel, &lowbounds, &highbounds, &pos, &orient_x, &orient_y, &orient_z);
}

/*************
 * DESCRIPTION:   Update box parameters
 * INPUT:         time     actual time
 * OUTPUT:        none
 *************/
void BOX::Update(const float time)
{
	TIME t;

	if(actor)
	{
		if(time != this->time)
		{
			if((actor->time.begin != this->time) || (actor->time.end != time))
			{
				t.begin = this->time;
				t.end = time;
				actor->Animate(&t);
			}
			// animated box

			// animate position
			actor->invmatrix->MultVectMat(&pos);

			// animate orientation
			actor->rotmatrix->MultVectMat(&orient_x);
			actor->rotmatrix->MultVectMat(&orient_y);
			actor->rotmatrix->MultVectMat(&orient_z);
		}
	}
}

/*************
 * DESCRIPTION:   Ray/box intersection test
 * INPUT:         ray      Pointer to actual ray
 * OUTPUT:        TRUE if hit else FALSE
 *************/
BOOL BOX::RayIntersect(RAY *ray)
{
	float nearest, farest, lambda1, lambda2, help;
	VECTOR dir, start, v;
	BOOL enter;
	UWORD farnorm, nearnorm;

	STATISTIC(STAT_BOXTEST);

	// Translate ray to object space.
	VecSub(&ray->start, &pos, &v);
	start.x = dotp(&v, &orient_x);
	start.y = dotp(&v, &orient_y);
	start.z = dotp(&v, &orient_z);

	v = ray->dir;
	dir.x = dotp(&v, &orient_x);
	dir.y = dotp(&v, &orient_y);
	dir.z = dotp(&v, &orient_z);

	nearest = -INFINITY;
	farest = INFINITY;
	if(fabs(dir.x) > EPSILON)
	{
		help = 1.f/dir.x;
		lambda1 = (lowbounds.x - start.x)*help;
		lambda2 = (highbounds.x - start.x)*help;
		if(lambda1 > lambda2)
		{
			help = lambda1;
			lambda1 = lambda2;
			lambda2 = help;
		}

		if(lambda1 > nearest)
		{
			nearnorm = OBJECT_XNORMAL;
			nearest = lambda1;
		}
		if(lambda2 < farest)
		{
			farnorm = OBJECT_XNORMAL;
			farest = lambda2;
		}

		if(nearest > farest)
			return FALSE;
	}
	else
	{
		if((start.x < lowbounds.x) || (start.x > highbounds.x))
			return FALSE;
	}

	if(fabs(dir.y) > EPSILON)
	{
		help = 1.f/dir.y;
		lambda1 = (lowbounds.y - start.y)*help;
		lambda2 = (highbounds.y - start.y)*help;
		if(lambda1 > lambda2)
		{
			help = lambda1;
			lambda1 = lambda2;
			lambda2 = help;
		}

		if(lambda1 > nearest)
		{
			nearnorm = OBJECT_YNORMAL;
			nearest = lambda1;
		}
		if(lambda2 < farest)
		{
			farnorm = OBJECT_YNORMAL;
			farest = lambda2;
		}

		if(nearest > farest)
			return FALSE;
	}
	else
	{
		if((start.y < lowbounds.y) || (start.y > highbounds.y))
			return FALSE;
	}

	if(fabs(dir.z) > EPSILON)
	{
		help = 1.f/dir.z;
		lambda1 = (lowbounds.z - start.z)*help;
		lambda2 = (highbounds.z - start.z)*help;
		if(lambda1 > lambda2)
		{
			help = lambda1;
			lambda1 = lambda2;
			lambda2 = help;
		}

		if(lambda1 > nearest)
		{
			nearnorm = OBJECT_ZNORMAL;
			nearest = lambda1;
		}
		if(lambda2 < farest)
		{
			farnorm = OBJECT_ZNORMAL;
			farest = lambda2;
		}

		if(nearest > farest)
			return FALSE;
	}
	else
	{
		if((start.z < lowbounds.z) || (start.z > highbounds.z))
			return FALSE;
	}

	flags &= ~OBJECT_NORMALS;
	if(nearest < 0.f)
	{
		if(farest < EPSILON)
			return FALSE;

		lambda1 = farest;
		flags |= farnorm;
		enter = FALSE;
	}
	else
	{
		lambda1 = nearest;
		flags |= nearnorm;
		enter = TRUE;
	}

	if(lambda1 > ray->lambda)
		return FALSE;

	ray->lambda = lambda1;
	ray->enter = enter;
	ray->obj = this;

	STATISTIC(STAT_BOXHIT);

	return TRUE;
}

//#define equal1(a,b) (fabs((a)-(b))<1e-5)

/*************
 * DESCRIPTION:   Compute normal to box at pos
 * INPUT:         dir      direction vector
 *                hitpos   hitposition
 *                nrm      normal
 *                geon     geometric normal
 * OUTPUT:        nrm is the normal at pos
 *************/
void BOX::Normal(const VECTOR *dir, const VECTOR *hitpos, VECTOR *nrm, VECTOR *geon)
{
	switch(flags & OBJECT_NORMALS)
	{
		case OBJECT_XNORMAL:
			*geon = orient_x;
			break;
		case OBJECT_YNORMAL:
			*geon = orient_y;
			break;
		case OBJECT_ZNORMAL:
			*geon = orient_z;
			break;
	}

	if(dotp(dir, geon) > 0.f)
	{
		// flip normal
		SetVector(geon, -geon->x, -geon->y, -geon->z);
	}

	*nrm = *geon;
}
