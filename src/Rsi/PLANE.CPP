/***************
 * MODUL:         plane
 * NAME:          plane.cpp
 * DESCRIPTION:   Functions for plane object
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    16.02.95 ah    initial release
 *    14.03.95 ah    added Update()
 *    27.08.95 ah    added Update()
 *    11.10.95 ah    added Identify()
 *    03.11.95 ah    plane is not animateable
 ***************/

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

#ifndef PLANE_H
#include "plane.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
PLANE::PLANE()
{
	SetVector(&pos, 0.f, 0.f, 0.f);
	SetVector(&norm, 0.f, 1.f, 0.f);
	flags |= OBJECT_UNLIMITED;
}

/*************
 * DESCRIPTION:   Generate Voxel
 * INPUT:         -
 * OUTPUT:        none
 *************/
void PLANE::GenVoxel()
{
	// PLANE is unlimited -> no voxel
}

/*************
 * DESCRIPTION:   Update plane parameters
 * INPUT:         time     actual time
 * OUTPUT:        none
 *************/
void PLANE::Update(const float time)
{
	TIME t;
	MATRIX r;

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
			// animated plane
			actor->matrix->MultVectMat(&pos);

			actor->rotmatrix->InvMat(&r);
			r.MultVectMat(&norm);
		}
	}
	d = dotp(&norm, &pos);
}

/*************
 * DESCRIPTION:   Ray/plane intersection test
 * INPUT:         ray      pointer to ray
 * OUTPUT:        TRUE if intersection, else FALSE
 *************/
#ifndef LOWLEVEL
BOOL PLANE::RayIntersect(RAY *ray)
{
	float d;

	STATISTIC(STAT_PLANETEST);

	d = dotp(&norm, &ray->dir);
	if (Abs(d) < EPSILON)
		return FALSE;
	d = (this->d - dotp(&norm, &ray->start)) / d;

	if (d > EPSILON && d < ray->lambda)
	{ // Is our intersection nearer ?
		ray->lambda = d;
		ray->obj = this;
		STATISTIC(STAT_PLANEHIT);
		return TRUE;
	}
	return FALSE;
}
#endif

/*************
 * DESCRIPTION:   Compute normal to plane at pos
 * INPUT:         dir      ray direction
 *                hitpos   hit position
 *                nrm      computed normal
 *                geon     geometric normal
 * OUTPUT:        none
 *************/
void PLANE::Normal(const VECTOR *dir, const VECTOR *hitpos, VECTOR *nrm, VECTOR *geon)
{
	*nrm = norm;
	if(dotp(dir, &norm) > 0.f)
	{ // flip normal
		nrm->x = -nrm->x;
		nrm->y = -nrm->y;
		nrm->z = -nrm->z;
	}
	*geon = *nrm;
}
