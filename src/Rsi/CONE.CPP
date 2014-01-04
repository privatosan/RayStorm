/***************
 * MODUL:         cone
 * NAME:          cone.cpp
 * DESCRIPTION:   Functions for cone object class
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

#ifndef CONE_H
#include "cone.h"
#endif

/*************
 * DESCRIPTION:   Constructor of cone (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
CONE::CONE()
{
	SetVector(&pos, 0.f, 0.f, 0.f);
	SetVector(&ox, 1.f, 0.f, 0.f);
	SetVector(&oy, 0.f, 1.f, 0.f);
	SetVector(&oz, 0.f, 0.f, 1.f);
	SetVector(&size, 1.f, 1.f, 1.f);
	flags |= OBJECT_CLOSEBOTTOM;
}

/*************
 * DESCRIPTION:   Generate Voxel
 * INPUT:         -
 * OUTPUT:        none
 *************/
void CONE::GenVoxel()
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
void CONE::Update(const float time)
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
			// animated cone

			// animate position
			actor->matrix->MultVectMat(&pos);

			// up direction
			actor->rotmatrix->MultVectMat(&ox);
			actor->rotmatrix->MultVectMat(&oy);
			actor->rotmatrix->MultVectMat(&oz);

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
 * DESCRIPTION:   Ray/cone intersection test
 * INPUT:         ray      Pointer to current ray
 * OUTPUT:        TRUE if hit else FALSE
 *************/
BOOL CONE::RayIntersect(RAY *ray)
{
	float t1, t2, a, b, c, d, y, disc;
	float distfact, lambda;
	VECTOR nray, npos;
	BOOL hit = FALSE;

	STATISTIC(STAT_CONETEST);

	/* Transform ray into canonical cone space */
	npos = ray->start;
	trans.MultVectMat(&npos);
	npos.y = -npos.y+1.f;
	nray = ray->dir;
	trans.MultDirMat(&nray);
	nray.y = -nray.y;
	distfact = nray.x*nray.x + nray.y*nray.y + nray.z*nray.z;
	if(fabs(distfact) > EPSILON)
	{
		distfact = 1.f/(float)sqrt(distfact);
		nray.x *= distfact;
		nray.y *= distfact;
		nray.z *= distfact;
	}
	lambda = ray->lambda/distfact;

	a = nray.x*nray.x + nray.z*nray.z - nray.y*nray.y;
	b = -(nray.x*npos.x + nray.z*npos.z - nray.y*npos.y);
	c = npos.x*npos.x + npos.z*npos.z - npos.y*npos.y;

	if(fabs(a) < EPSILON)
	{
		if(fabs(b) > EPSILON)
		{
			// Only one intersection point...
			t1 = .5f*c / b;

			if((t1 > EPSILON) && (t1 < lambda))
			{
				y = npos.y + t1 * nray.y;
				if((y >= 0.f) && (y <= 1.f))
				{
					lambda = t1;
					ray->enter = TRUE;
					flags &= ~OBJECT_NORMALS1;
					flags |= OBJECT_SIDE;
					hit = TRUE;
				}
			}
		}
	}
	else
	{
		disc = b*b - a*c;
		if(disc < 0.f)
			return FALSE;
		disc = (float)sqrt(disc);

		a = 1.f/a;
		t1 = (b + disc) * a;

		if((t1 > EPSILON) && (t1 < lambda))
		{
			y = npos.y + t1 * nray.y;
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

		if((t2 > EPSILON) && (t2 < lambda))
		{
			y = npos.y + t2 * nray.y;
			if((y >= 0.f) && (y <= 1.f))
			{
				lambda = t2;
				ray->enter = TRUE;
				flags &= ~OBJECT_NORMALS1;
				flags |= OBJECT_SIDE;
				hit = TRUE;
			}
		}
	}

	if(fabs(nray.y) > EPSILON)
	{
		if(flags & OBJECT_CLOSEBOTTOM)
		{
			d = (1.f - npos.y) / nray.y;
			a = (npos.x + d * nray.x);
			b = (npos.z + d * nray.z);

			if((a*a + b*b) <= 1.f && (d > EPSILON) && (d < lambda))
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
		STATISTIC(STAT_CONEHIT);
	}

	return hit;
}

/*************
 * DESCRIPTION:   Compute normal to cone at pos
 * INPUT:         dir      direction vector
 *                hitpos   hitposition
 *                nrm      normal
 *                geon     geometric normal
 * OUTPUT:        nrm is the normal at pos
 *************/
void CONE::Normal(const VECTOR *dir, const VECTOR *hitpos, VECTOR *nrm, VECTOR *geon)
{
	if(flags & OBJECT_SIDE)
	{
		// Transform position into cone space
		*nrm = *hitpos;
		trans.MultVectMat(nrm);
		nrm->y = -nrm->y+1.f;
	}
	else
		SetVector(nrm, 0.f, -1.f, 0.f);

	/* Tranform normal back to world space */
	trans.MultNormalMat(nrm);
	VecNormalizeNoRet(nrm);

	if(dotp(dir, nrm) > 0.f)
	{
		// flip normal
		SetVector(nrm, -nrm->x, -nrm->y, -nrm->z);
	}

	*geon = *nrm;
}
