/***************
 * MODUL:         sor
 * NAME:          sor.cpp
 * DESCRIPTION:   Functions for surface of revolution object class.
 *    The surface of revolution primitive is defined by a set of points
 *    in 2d space wich are interpolated by cubic splines. The resulting
 *    2d function is rotated about an axis to form the final object.
 *
 *    All calculations are done in the object's (u,v,w)-coordinate system
 *    with the (w)-axis being the rotation axis.
 *
 *    One spline segment in the (r,w)-plane is given by the equation
 *
 *       r^2 = f(w) = A * w * w * w + B * w * w + C * w + D.
 *
 *    To intersect a ray R = P + k * D transformed into the object's
 *    coordinate system with the surface of revolution, the equation
 *
 *       (Pu + k * Du)^2 + (Pv + k * Dv)^2 = f(Pw + k * Dw)
 *
 *    has to be solved for k (cubic polynomial in k).
 *
 *    Note that Pu, Pv, Pw and Du, Dv, Dw denote the coordinates
 *    of the vectors P and D.
 *
 * Ideas for the surface of revolution were taken from:
 *
 *    P. Burger and D. Gillies, "Rapid Ray Tracing of General Surfaces
 *    of Revolution", New Advances in Computer Graphics, Proceedings
 *    of CG International '89, R. A. Earnshaw, B. Wyvill (Eds.),
 *    Springer, ..., pp. 523-531
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    20.03.97 ah    initial release
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

#ifndef SOR_H
#include "sor.h"
#endif

#ifndef POLYSOLV_H
#include "polysolv.h"
#endif

/*************
 * DESCRIPTION:   Constructor of sor (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
SOR::SOR()
{
	flags |= OBJECT_CLOSETOP | OBJECT_CLOSEBOTTOM;
	number = 0;
	spline = NULL;
	SetVector(&pos, 0.f, 0.f, 0.f);
	SetVector(&size, 1.f, 1.f, 1.f);
	SetVector(&orient_x, 1.f, 0.f, 0.f);
	SetVector(&orient_y, 0.f, 1.f, 0.f);
	SetVector(&orient_z, 0.f, 0.f, 1.f);
}

/*************
 * DESCRIPTION:   Destructor of sor
 * INPUT:         none
 * OUTPUT:        none
 *************/
SOR::~SOR()
{
	if(spline)
		delete spline;
}

/*************
 * DESCRIPTION:   Generate Voxel
 * INPUT:         -
 * OUTPUT:        none
 *************/
void SOR::GenVoxel()
{
	VECTOR v,v1;
	float r;

	if(radius)
		r = (float)sqrt(radius);
	else
		r = 0.f;

	SetVector(&v, -r*size.x, height1*size.y, -r*size.z);
	SetVector(&v1, r*size.x, height2*size.y,  r*size.z);
	AccumulateVoxel(&voxel, &v, &v1, &pos, &orient_x, &orient_y, &orient_z);
}

/*************
 * DESCRIPTION:   Update sor parameters
 * INPUT:         time     actual time
 * OUTPUT:        none
 *************/
void SOR::Update(const float time)
{
	TIME t;
	MATRIX m;

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
			// animated sor

			// animate position
			actor->matrix->MultVectMat(&pos);
			// orientation
			actor->rotmatrix->MultNormalMat(&orient_x);
			actor->rotmatrix->MultNormalMat(&orient_y);
			actor->rotmatrix->MultNormalMat(&orient_z);
			// and size
			size.x *= actor->act_size.x / actor->axis_size.x;
			size.y *= actor->act_size.y / actor->axis_size.y;
			size.z *= actor->act_size.z / actor->axis_size.z;
		}
	}
	m.SetSOTMatrix(&size, &orient_x, &orient_y, &orient_z, &pos);
	m.InvMat(&trans);
}

/*************
 * FUNCTION:      SOR::RayIntersect
 * DESCRIPTION:   Ray/sor intersection test
 * INPUT:         ray      Pointer to actual ray
 * OUTPUT:        TRUE if hit else FALSE
 *************/
BOOL SOR::RayIntersect(RAY *ray)
{
	int i = 0, j, n;
	float a, b, u, v, r0, distfact, lambda;
	double k, l, x[4], y[3];
	VECTOR npos, ndir;
	SOR_SPLINE *entry;
	BOOL hit=FALSE, inside, enter;
	double npxz;
	double ndxz, ndy2;
	double ndy3, pd;

	STATISTIC(STAT_SORTEST);

	// Transform the ray into the surface of revolution space
	npos = ray->start;
	trans.MultVectMat(&npos);
	ndir = ray->dir;
	trans.MultDirMat(&ndir);
	distfact = ndir.x*ndir.x + ndir.y*ndir.y + ndir.z*ndir.z;
	if(fabs(distfact) > EPSILON)
	{
		distfact = 1.f/(float)sqrt(distfact);
		ndir.x *= distfact;
		ndir.y *= distfact;
		ndir.z *= distfact;
	}
	lambda = ray->lambda/distfact;

	// Get distance of the ray from rotation axis (= y axis)
	r0 = npos.x * ndir.z - npos.z * ndir.x;

	a = ndir.x*ndir.x + ndir.z*ndir.z;
	if(a > 0.f)
		r0 /= (float)sqrt(a);

	// Test if ray misses object's bounds
	if(r0 > radius)
		return FALSE;

	if(fabs(ndir.y) > EPSILON)
	{
		if(flags & (OBJECT_CLOSEBOTTOM | OBJECT_CLOSETOP))
		{
			// Test base plane
			if((flags & OBJECT_CLOSEBOTTOM) && (base_radius > EPSILON))
			{
				a = (height1 - npos.y) / ndir.y;
				if((a > EPSILON) && (a < lambda))
				{
					u = npos.x + a * ndir.x;
					v = npos.z + a * ndir.z;

					b = u * u + v * v;

					if(b <= base_radius)
					{
						hit = TRUE;
						lambda = a;
						enter = npos.y < height1 ? TRUE : FALSE;
						flags &= ~OBJECT_NORMALS1;
						flags |= OBJECT_CAPS;
					}
				}
			}

			// Test cap plane
			if((flags & OBJECT_CLOSETOP) && (cap_radius > EPSILON))
			{
				a = (height2 - npos.y) / ndir.y;
				if((a > EPSILON) && (a < lambda))
				{
					u = npos.x + a * ndir.x;
					v = npos.z + a * ndir.z;

					b = u * u + v * v;

					if(b <= cap_radius)
					{
						hit = TRUE;
						lambda = a;
						enter = npos.y > height2 ? TRUE : FALSE;
						flags &= ~OBJECT_NORMALS1;
						flags |= OBJECT_CAPS;
					}
				}
			}
		}
	}

	// precalculate some values
	ndxz = ndir.x*ndir.x + ndir.z*ndir.z;
	ndy2 = ndir.y*ndir.y;

	ndy3 = ndy2*ndir.y;

	npxz = npos.x*npos.x + npos.z*npos.z;

	pd = 2.f * (npos.x * ndir.x + npos.z * ndir.z);

	// Test all segments
	entry = spline;
	inside = FALSE;
	a = npos.x*npos.x + npos.z*npos.z;
	if(a != 0.f)
		a = (float)sqrt(a);

	for(j=0; j<number; j++, entry++)
	{
		if((npos.y < entry->h2) && (npos.y > entry->h1))
		{
			if(a < sqrt(npos.y * (npos.y * (npos.y * entry->a + entry->b) + entry->c) + entry->d))
				inside = TRUE;
		}

		// Test if ray misses segment's bounds
		if(((ndir.y >= 0.f) && (npos.y >  entry->h2)) ||
			((ndir.y <= 0.f) && (npos.y <  entry->h1)) ||
			((ndir.x >= 0.f) && (npos.x >  entry->r2)) ||
			((ndir.x <= 0.f) && (npos.x < -entry->r2)) ||
			((ndir.z >= 0.f) && (npos.z >  entry->r2)) ||
			((ndir.z <= 0.f) && (npos.z < -entry->r2)))
		{
			continue;
		}

		if(r0 > entry->r2)
			continue;

		// Cubic curve
		x[0] = entry->a * ndy3;
		x[1] = ndy2 * (3.f * entry->a * npos.y + entry->b) - ndxz;
		x[2] = ndir.y * (npos.y * (3.f * entry->a * npos.y + 2.f * entry->b) + entry->c) - pd;
		x[3] = npos.y * (npos.y * (entry->a * npos.y + entry->b) + entry->c) + entry->d - npxz;

		if(flags & OBJECT_SORSTURM)
			n = PolySolve(3, x, y);
		else
			n = SolveCubic(x, y);

		while (n--)
		{
			k = y[n];

			if((k > EPSILON) && (k < lambda))
			{
				l = npos.y + k * ndir.y;

				if((l >= entry->h1) && (l <= entry->h2))
				{
					hit = TRUE;
					lambda = (float)k;
					flags &= ~OBJECT_NORMALS1;
					flags |= OBJECT_SIDE;
					segment = j;
				}
			}
		}
	}

	if(hit)
	{
		if(flags & OBJECT_SIDE)
			ray->enter = !inside;
		else
			ray->enter = enter;

		ray->lambda = lambda * distfact;
		ray->obj = this;
		STATISTIC(STAT_SORHIT);
	}

	return hit;
}

/*************
 * DESCRIPTION:   Compute normal to SOR at pos
 * INPUT:         dir      direction vector
 *                hitpos   hitposition
 *                nrm      normal
 *                geon     geometric normal
 * OUTPUT:        nrm is the normal at pos
 *************/
void SOR::Normal(const VECTOR *dir, const VECTOR *hitpos, VECTOR *nrm, VECTOR *geon)
{
	float k;
	VECTOR p;
	SOR_SPLINE *entry;

	if(flags & OBJECT_SIDE)
	{
		// Transform the intersection point into the surface of revolution space
		p = *hitpos;
		trans.MultVectMat(&p);

		if(p.x*p.x + p.z*p.z > EPSILON)
		{
			entry = &spline[segment];

			k = 0.5f * (p.y * (3.f * entry->a * p.y + 2.f * entry->b) + entry->c);

			SetVector(nrm, p.x, -k, p.z);
		}
		else
			SetVector(nrm, 0.f, 1.f, 0.f);
	}
	else
		SetVector(nrm, 0.f, 1.f, 0.f);

	// Transform the normal out of the surface of revolution space
	trans.MultNormalMat(nrm);

	if(dotp(dir, nrm) > 0.f)
	{
		// flip normal
		SetVector(nrm, -nrm->x, -nrm->y, -nrm->z);
	}
	*geon = *nrm;
}

/*************
 * DESCRIPTION:   Calculate the spline segments of a surface of revolution
 *    from a set of points. Note that the number of points in the surface
 *    of revolution has to be set.
 * INPUT:         p     points which define the surface
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SOR::Compute(VECT2D *p)
{
	int i, n;
	double A, B, C, D, w, k[4];
	double x[4], xmax;
	double y[2], ymax, ymin;
	double c[3], r[2];
	MATRIX mat, m;

	// allocate segments.
	if(spline)
		delete spline;

	spline = new SOR_SPLINE[number];
	if(!spline)
		return FALSE;

	// We want to know the size of the overall bounding cylinder
	xmax = ymax = -INFINITY;
	ymin = INFINITY;

	// Calculate segments, i.e. cubic patches
	for(i=0; i<number; i++)
	{
		// Use cubic interpolation
		k[0] = p[i+1].x * p[i+1].x;
		k[1] = p[i+2].x * p[i+2].x;
		k[2] = (p[i+2].x - p[i].x) / (p[i+2].y - p[i].y);
		k[3] = (p[i+3].x - p[i+1].x) / (p[i+3].y - p[i+1].y);

		k[2] *= 2.f * p[i+1].x;
		k[3] *= 2.f * p[i+2].x;

		w = p[i+1].y;

		mat.m[2] = (float)w;
		mat.m[1] = mat.m[2] * (float)w;
		mat.m[0] = mat.m[1] * (float)w;
		mat.m[3] = 1.f;

		mat.m[9] = 2.f * (float)w;
		mat.m[8] = mat.m[9] * 1.5f * (float)w;
		mat.m[10] = 1.f;
		mat.m[11] = 0.f;

		w = p[i+2].y;

		mat.m[6] = (float)w;
		mat.m[5] = mat.m[6] * (float)w;
		mat.m[4] = mat.m[5] * (float)w;
		mat.m[7] = 1.f;

		mat.m[13] = 2.f * (float)w;
		mat.m[12] = mat.m[13] * 1.5f * (float)w;
		mat.m[14] = 1.f;
		mat.m[15] = 0.f;

		mat.InvMat(&m);

		// Calculate coefficients of cubic patch
		A = k[0] * m.m[0] + k[1] * m.m[1] + k[2] * m.m[2] + k[3] * m.m[3];
		B = k[0] * m.m[4] + k[1] * m.m[5] + k[2] * m.m[6] + k[3] * m.m[7];
		C = k[0] * m.m[8] + k[1] * m.m[9] + k[2] * m.m[10] + k[3] * m.m[11];
		D = k[0] * m.m[12] + k[1] * m.m[13] + k[2] * m.m[14] + k[3] * m.m[15];

		if(fabs(A) < EPSILON)
			A = 0.f;
		if(fabs(B) < EPSILON)
			B = 0.f;
		if(fabs(C) < EPSILON)
			C = 0.f;
		if(fabs(D) < EPSILON)
			D = 0.f;

		spline[i].a = (float)A;
		spline[i].b = (float)B;
		spline[i].c = (float)C;
		spline[i].d = (float)D;

		// Get maximum radius**2 in current segment
		y[0] = p[i+1].y;
		y[1] = p[i+2].y;

		x[0] = x[2] = p[i+1].x;
		x[1] = x[3] = p[i+2].x;

		c[0] = 3.f * A;
		c[1] = 2.f * B;
		c[2] = C;

		n = SolveQuadratic(c, r);

		while (n--)
		{
			if ((r[n] >= y[0]) && (r[n] <= y[1]))
			{
				x[n] = sqrt(r[n] * (r[n] * (r[n] * A + B) + C) + D);
			}
		}

		// Set current segment's bounding cylinder
		spline[i].h1 = (float)y[0];
		spline[i].h2 = (float)y[1];

		spline[i].r2 = (float)MAX(MAX(x[0], x[1]), MAX(x[2], x[3]));

		// Keep track of overall bounding cylinder
		xmax = MAX(xmax, spline[i].r2);

		ymin = MIN(ymin, spline[i].h1);
		ymax = MAX(ymax, spline[i].h2);
	}

	// Set overall bounding cylinder
	radius = (float)xmax;

	height1 = (float)ymin;
	height2 = (float)ymax;

	// Get cap and base radius
	w = spline[number-1].h2;
	cap_radius = ((spline[number-1].a * (float)w + spline[number-1].b) * (float)w + spline[number-1].c) * (float)w + spline[number-1].d;
	if(cap_radius < 0.f)
		cap_radius = 0.f;

	// Get base radius
	w = spline[0].h1;
	base_radius = ((spline[0].a * (float)w + spline[0].b) * (float)w + spline[0].c) * (float)w + spline[0].d;
	if(base_radius < 0.f)
		base_radius = 0.f;

	return TRUE;
}
