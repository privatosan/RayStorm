/***************
 * MODUL:         sphere
 * NAME:          sphere.cpp
 * DESCRIPTION:   Functions for sphere object class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    13.03.95 ah    added Update()
 *    27.08.95 ah    added animation
 *    11.10.95 ah    added Identify()
 *    14.04.96 ah    added code for normal flipping
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

#ifndef SPHERE_H
#include "sphere.h"
#endif

/*************
 * DESCRIPTION:   Constructor of sphere (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
SPHERE::SPHERE()
{
	ir = 0.f;
}

/*************
 * DESCRIPTION:   Returns identification for sphere
 * INPUT:         none
 * OUTPUT:        IDENT_SPHERE
 *************/
ULONG SPHERE::Identify()
{
	return IDENT_SPHERE;
}

/*************
 * DESCRIPTION:   Generate Voxel
 * INPUT:         -
 * OUTPUT:        none
 *************/
void SPHERE::GenVoxel()
{
	// generate voxel
	voxel.min.x = pos.x-r;
	voxel.min.y = pos.y-r;
	voxel.min.z = pos.z-r;
	voxel.max.x = pos.x+r;
	voxel.max.y = pos.y+r;
	voxel.max.z = pos.z+r;
}

/*************
 * DESCRIPTION:   Update sphere parameters
 * INPUT:         time     actual time
 * OUTPUT:        none
 *************/
void SPHERE::Update(const float time)
{
	TIME t;
	float f;

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
			// animated sphere

			// animate position
			actor->matrix->MultVectMat(&pos);

			// animate radius
			f = actor->act_size.x / actor->axis_size.x;
			r *= f;
			ir *= f;
		}
	}
	// square of radius for intersection test
	rsq = r*r;
	irsq = rsq*ir*ir;

	// inverted delta of radius squares for density function
	inv_delta_irsq = 1.f  / (irsq - rsq);
}

/*************
 * DESCRIPTION:   Ray/sphere intersection test
 * INPUT:         ray      Pointer to actual ray
 * OUTPUT:        TRUE if hit else FALSE
 *************/
#ifndef LOWLEVEL
BOOL SPHERE::RayIntersect(RAY *ray)
{
	VECTOR adj;
	float b, t, s;

	STATISTIC(STAT_SPHERETEST);

	// Translate ray origin to object space and negate everything.
	// (Thus, we translate the sphere into ray space, which saves
	// us a couple of negations below.)
	VecSub(&pos, &ray->start, &adj);

	// Solve quadratic equation.
	b = adj.x * ray->dir.x + adj.y * ray->dir.y + adj.z * ray->dir.z;
	t = b * b - adj.x * adj.x - adj.y * adj.y - adj.z * adj.z + rsq;
	if (t < 0.f)
		return FALSE;
	t = (float)sqrt(t);
	s = b - t;
	if(s > 0.f)
	{
		if(s < ray->lambda)
		{
			// Is our intersection nearer ?
			ray->lambda = s;
			ray->obj = this;
			ray->enter = TRUE;
			STATISTIC(STAT_SPHEREHIT);
			return TRUE;
		}
	}
	s = b + t;
	if(s > 0.f)
	{
		if(s < ray->lambda)
		{
			// Is our intersection nearer ?
			ray->lambda = s;
			ray->obj = this;
			ray->enter = FALSE;
			STATISTIC(STAT_SPHEREHIT);
			return TRUE;
		}
	}

	return FALSE;
}
#endif

/*************
 * DESCRIPTION:   Compute normal to sphere at pos
 * INPUT:         dir      direction vector
 *                hitpos   hitposition
 *                nrm      normal
 *                geon     geometric normal
 * OUTPUT:        nrm is the normal at pos
 *************/
void SPHERE::Normal(const VECTOR *dir, const VECTOR *hitpos, VECTOR *nrm, VECTOR *geon)
{
	float rr;

	rr = 1.f/r;
	VecSubScaled(hitpos, rr, &this->pos, nrm);
	VecNormalizeNoRet(nrm);

	if(dotp(dir, nrm) > 0.f)
	{
		// flip normal
		SetVector(nrm, -nrm->x, -nrm->y, -nrm->z);
	}
	*geon = *nrm;
}

/*************
 * DESCRIPTION:   compute density of sphere at position
 * INPUT:         hitpos     position to compute density at
 * OUTPUT:        computed density (0 (outside) ... 1 (inside))
 *************/
float SPHERE::Density(const VECTOR *hitpos)
{
	float t;
	VECTOR deltapos;

	VecSub(hitpos, &pos, &deltapos);
	t = deltapos.x*deltapos.x + deltapos.y*deltapos.y + deltapos.z*deltapos.z;

	if(t > rsq)
		return 0.f;

	if(t < irsq)
		return 1.f;

	return (t - rsq) * inv_delta_irsq;
}

