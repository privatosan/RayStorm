/***************
 * MODUL:         cylinder
 * NAME:          cylinder.cpp
 * DESCRIPTION:   Functions for cylinder object class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    18.03.97 ah    initial release
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

#ifndef CYLINDER_H
#include "cylinder.h"
#endif

#define EPSILON_CYL  10.f*EPSILON

/*************
 * DESCRIPTION:   Constructor of cylinder (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
CYLINDER::CYLINDER()
{
	SetVector(&pos, 0.f, 0.f, 0.f);
	SetVector(&ox, 1.f, 0.f, 0.f);
	SetVector(&oy, 0.f, 1.f, 0.f);
	SetVector(&oz, 0.f, 0.f, 1.f);
	SetVector(&size, 1.f, 1.f, 1.f);
	flags |= OBJECT_CLOSETOP | OBJECT_CLOSEBOTTOM;
}

/*************
 * DESCRIPTION:   Generate Voxel
 * INPUT:         -
 * OUTPUT:        none
 *************/
void CYLINDER::GenVoxel()
{
	VECTOR v;

	SetVector(&v, -size.x, 0.f, -size.z);
	AccumulateVoxel(&voxel, &v, &size, &pos, &ox, &oy, &oz);
}

/*************
 * DESCRIPTION:   Update box parameters
 * INPUT:         time     actual time
 * OUTPUT:        none
 *************/
void CYLINDER::Update(const float time)
{
	TIME t;
	MATRIX tmp,tmp1,tmp2;

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
			// animated cylinder

			// animate position
			actor->invmatrix->MultVectMat(&pos);

			// up direction
			tmp.SetOMatrix(&ox, &oy, &oz);
			tmp1.MultMat(&tmp, actor->rotmatrix);
			SetVector(&ox, tmp1.m[5], tmp1.m[9], tmp1.m[13]);
			SetVector(&oy, tmp1.m[6], tmp1.m[10], tmp1.m[14]);
			SetVector(&oz, tmp1.m[7], tmp1.m[11], tmp1.m[15]);

			size.x *= actor->act_size.x / actor->axis_size.x;
			size.y *= actor->act_size.y / actor->axis_size.y;
			size.z *= actor->act_size.z / actor->axis_size.z;
		}
	}

	tmp.SetOTMatrix(&ox, &oy, &oz, &pos);
	tmp1.SetScaleMatrix(&size);
	tmp2.MultMat(&tmp1, &tmp);
	tmp2.InvMat(&trans);
}
// } <- needed by GoldED

/*************
 * DESCRIPTION:   Ray/cylinder intersection test
 * INPUT:         ray      Pointer to current ray
 * OUTPUT:        TRUE if hit else FALSE
 *************/
BOOL CYLINDER::RayIntersect(RAY *ray)
{
	float t1, t2, a, b, c, d, y, disc;
	float distfact, lambda;
	VECTOR ndir, npos;
	BOOL hit = FALSE;

	STATISTIC(STAT_CYLINDERTEST);

	/* Transform ray into canonical cylinder space */
	npos = ray->start;
	trans.MultVectMat(&npos);
	ndir = ray->dir;
	trans.MultDirMat(&ndir);
	distfact = ndir.x*ndir.x + ndir.y*ndir.y + ndir.z*ndir.z;
	if(fabs(distfact) > EPSILON)
	{
		distfact = (float)(1./sqrt(distfact));
		ndir.x *= distfact;
		ndir.y *= distfact;
		ndir.z *= distfact;
	}
	lambda = ray->lambda/distfact;

	a = ndir.x * ndir.x + ndir.z * ndir.z;
	if (a < EPSILON)
		return FALSE;

	b = -(ndir.x * npos.x + ndir.z * npos.z);
	c = npos.x*npos.x + npos.z*npos.z - 1.f;
	disc = b*b - a*c;
	if(disc < 0.f)
		return FALSE;
	disc = (float)sqrt(disc);

	a = 1.f/a;
	t1 = (b + disc) * a;

	if((t1 > EPSILON_CYL) && (t1 < lambda))
	{
		y = npos.y + t1 * ndir.y;
		if((y >= 0.f) && (y <= 1.f))
		{
			lambda = t1;
			ray->enter = FALSE;
			flags &= ~OBJECT_NORMALS1;
			flags |= OBJECT_SIDE;
			hit = TRUE;
		}
	}

	t2 = (b - disc) * a;

	if((t2 > EPSILON_CYL) && (t2 < lambda))
	{
		y = npos.y + t2 * ndir.y;
		if((y >= 0.f) && (y <= 1.f))
		{
			lambda = t2;
			ray->enter = TRUE;
			flags &= ~OBJECT_NORMALS1;
			flags |= OBJECT_SIDE;
			hit = TRUE;
		}
	}

	if(fabs(ndir.y) > EPSILON)
	{
		if(flags & OBJECT_CLOSETOP)
		{
			d = (1.f - npos.y) / ndir.y;
			a = (npos.x + d * ndir.x);
			b = (npos.z + d * ndir.z);

			if(((a*a + b*b) <= 1.f) && (d > EPSILON_CYL) && (d < lambda))
			{
				lambda = d;
				ray->enter = npos.y > 1.f ? TRUE : FALSE;
				flags &= ~OBJECT_NORMALS1;
				flags |= OBJECT_CAPS;
				hit = TRUE;
			}
		}

		if(flags & OBJECT_CLOSEBOTTOM)
		{
			d = -npos.y / ndir.y;
			a = (npos.x + d * ndir.x);
			b = (npos.z + d * ndir.z);

			if(((a*a + b*b) <= 1.f) && (d > EPSILON_CYL) && (d < lambda))
			{
				lambda = d;
				ray->enter = npos.y < 0.f ? TRUE : FALSE;
				flags &= ~OBJECT_NORMALS1;
				flags |= OBJECT_CAPS;
				hit = TRUE;
			}
		}
	}

	if(hit)
	{
		ray->obj = this;
		ray->lambda = lambda * distfact;
		STATISTIC(STAT_CYLINDERHIT);
	}

	return hit;
}

/*************
 * DESCRIPTION:   Compute normal to box at pos
 * INPUT:         dir      direction vector
 *                hitpos   hitposition
 *                nrm      normal
 *                geon     geometric normal
 * OUTPUT:        nrm is the normal at pos
 *************/
void CYLINDER::Normal(const VECTOR *dir, const VECTOR *hitpos, VECTOR *nrm, VECTOR *geon)
{
	if(flags & OBJECT_SIDE)
	{
		// Transform position into cylinder space
		*nrm = *hitpos;
		trans.MultVectMat(nrm);
		// The normal is equal to the point of intersection in cylinder
		// space, but with Y = 0.
		nrm->y = 0.f;
	}
	else
		SetVector(nrm, 0.f, 1.f, 0.f);

	/* Tranform normal back to world space */
	trans.MultNormalMat(nrm);

	if(dotp(dir, nrm) > 0.f)
	{
		// flip normal
		SetVector(nrm, -nrm->x, -nrm->y, -nrm->z);
	}

	*geon = *nrm;
}
