/***************
 * MODUL:         triangle
 * NAME:          triangle.cpp
 * DESCRIPTION:   Functions for triangle object class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.02.95 ah    initial release
 *    14.03.95 ah    added Update()
 *    16.03.95 ah    bugfix Update(): changed voxel calculation
 *    27.08.95 ah    added Update() and GenVoxel()
 *    09.09.95 ah    added code for animated triangle
 *    11.10.95 ah    added Identify() and stuff for triangle blocks
 *    08.03.97 ah    added CreateMesh()
 *    10.05.99 ah    added brush mapping coordinates
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef ERROR_H
#include "error.h"
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

#ifndef TRIANGLE_H
#include "triangle.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
TRIANGLE::TRIANGLE()
{
	vnorm = NULL;
	brushcoord = NULL;
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
TRIANGLE::~TRIANGLE()
{
	if(vnorm)
		delete [ ] vnorm;
	if(brushcoord)
		delete [ ] brushcoord;
}

/*************
 * DESCRIPTION:   Generate Voxel
 * INPUT:         -
 * OUTPUT:        none
 *************/
void TRIANGLE::GenVoxel()
{
	float k;

	// generate voxel
	k = MIN(p[0].x, p[1].x);
	voxel.min.x = MIN(k, p[2].x);
	k = MIN(p[0].y, p[1].y);
	voxel.min.y = MIN(k, p[2].y);
	k = MIN(p[0].z, p[1].z);
	voxel.min.z = MIN(k, p[2].z);

	k = MAX(p[0].x, p[1].x);
	voxel.max.x = MAX(k, p[2].x);
	k = MAX(p[0].y, p[1].y);
	voxel.max.y = MAX(k, p[2].y);
	k = MAX(p[0].z, p[1].z);
	voxel.max.z = MAX(k, p[2].z);
}

/*************
 * DESCRIPTION:   Do the precalculations
 * INPUT:         time     actual time
 * OUTPUT:        none
 *************/
void TRIANGLE::Update(const float time)
{
	VECTOR ptmp, anorm;
	float k;
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
			// animated triangle
			actor->matrix->MultVectMat(&p[0]);
			actor->matrix->MultVectMat(&p[1]);
			actor->matrix->MultVectMat(&p[2]);

			actor->rotmatrix->MultVectMat(&vnorm[0]);
			actor->rotmatrix->MultVectMat(&vnorm[1]);
			actor->rotmatrix->MultVectMat(&vnorm[2]);
		}
	}

	VecSub(&p[1], &p[0], &e[0]);
	VecSub(&p[2], &p[1], &e[1]);
	VecSub(&p[0], &p[2], &e[2]);

	// Find plane normal
	VecCross(&e[0], &e[1], &ptmp);
	norm = ptmp;
	if (VecNormalize(&norm) == 0.f)
	{
		return;
	}

	d = dotp(&norm, &p[0]);

	if(vnorm)
	{
		if (VecNormalize(&vnorm[0]) == 0.f || VecNormalize(&vnorm[1]) == 0.f ||
			 VecNormalize(&vnorm[2]) == 0.f)
		{
			return;
		}
		if (dotp(&vnorm[0], &norm) < 0.f)
			VecScale(-1.f, &vnorm[0], &vnorm[0]);
		if (dotp(&vnorm[1], &norm) < 0.f)
			VecScale(-1.f, &vnorm[1], &vnorm[1]);
		if (dotp(&vnorm[2], &norm) < 0.f)
			VecScale(-1.f, &vnorm[2], &vnorm[2]);
#if 0
		if (dotp(&vnorm[0], &norm) < 0.f)
		{
			// Reverse direction of surface normal on Phong
			// triangle if the surface normal points "away"
			// from the first vertex normal.
			// Note that this means that we trust the vertex
			// normals rather than trust that the user gave the
			// vertices in the correct order.
			VecScale(-1.f, &norm, &norm);
			VecScale(-1.f, &ptmp, &ptmp);
			d = -d;
			VecScale(-1.f, &e[0], &e[0]);
			VecScale(-1.f, &e[1], &e[1]);
			VecScale(-1.f, &e[2], &e[2]);
		}
#endif
	}
	// Find "dominant" part of normal vector.
	anorm.x = (float)Abs(ptmp.x);
	anorm.y = (float)Abs(ptmp.y);
	anorm.z = (float)Abs(ptmp.z);

	// Scale edges by dominant part of normal.  This makes intersection
	// testing a bit faster.
	flags &= ~OBJECT_NORMALS;
	if (anorm.x > anorm.y && anorm.x > anorm.z)
	{
		flags |= OBJECT_XNORMAL;
		k = 1.f / ptmp.x;
	}
	else
	{
		if (anorm.y > anorm.z)
		{
			flags |= OBJECT_YNORMAL;
			k = 1.f / ptmp.y;
		}
		else
		{
			flags |= OBJECT_ZNORMAL;
			k = 1.f / ptmp.z;
		}
	}

	VecScale(k, &e[0], &e[0]);
	VecScale(k, &e[1], &e[1]);
	VecScale(k, &e[2], &e[2]);
}

/*************
 * DESCRIPTION:   Intersect ray with triangle.  This is an optimized
 *                version of the intersection routine from Snyder and
 *                Barr's '87 SIGGRAPH paper.
 * INPUT:         ray      pointer to ray
 * OUTPUT:        TRUE if intersection, else FALSE
 *************/
#if !defined LOWLEVEL
BOOL TRIANGLE::RayIntersect(RAY *ray)
{
	float qi1, qi2, d, b0, b1, b2;
	VECTOR pos, dir;

	STATISTIC(STAT_TRIANGLETEST);

	pos = ray->start;
	dir = ray->dir;
	// Plane intersection.
	d = dotp(&norm, &dir);
	if (Abs(d) < EPSILON)
		return FALSE;
	d = (this->d - dotp(&norm, &pos)) / d;
	if (d < EPSILON || d > ray->lambda)
		return FALSE;

	if (flags & OBJECT_XNORMAL)
	{
		qi1 = pos.y + d*dir.y;
		qi2 = pos.z + d*dir.z;
		b0 = e[1].y*(qi2 - p[1].z) - e[1].z*(qi1 - p[1].y);
		if (b0 < 0.f || b0 > 1.f)
			return FALSE;
		b1 = e[2].y*(qi2 - p[2].z) - e[2].z*(qi1 - p[2].y);
		if (b1 < 0.f || b1 > 1.f)
			return FALSE;
		b2 = e[0].y*(qi2 - p[0].z) - e[0].z*(qi1 - p[0].y);
		if (b2 < 0.f || b2 > 1.f)
			return FALSE;
	}
	else
	{
		if (flags & OBJECT_YNORMAL)
		{
			qi1 = pos.x + d*dir.x;
			qi2 = pos.z + d*dir.z;
			b0 = e[1].z*(qi1 - p[1].x) - e[1].x*(qi2 - p[1].z);
			if (b0 < 0.f || b0 > 1.f)
				return FALSE;
			b1 = e[2].z*(qi1 - p[2].x) - e[2].x*(qi2 - p[2].z);
			if (b1 < 0.f || b1 > 1.f)
				return FALSE;
			b2 = e[0].z*(qi1 - p[0].x) - e[0].x*(qi2 - p[0].z);
			if (b2 < 0.f || b2 > 1.f)
				return FALSE;
		}
		else
		{
			qi1 = pos.x + d*dir.x;
			qi2 = pos.y + d*dir.y;
			b0 = e[1].x*(qi2 - p[1].y) - e[1].y*(qi1 - p[1].x);
			if (b0 < 0.f || b0 > 1.f)
				return FALSE;
			b1 = e[2].x*(qi2 - p[2].y) - e[2].y*(qi1 - p[2].x);
			if (b1 < 0.f || b1 > 1.f)
				return FALSE;
			b2 = e[0].x*(qi2 - p[0].y) - e[0].y*(qi1 - p[0].x);
			if (b2 < 0.f || b2 > 1.f)
				return FALSE;
		}
	}

	b[0] = b0;
	b[1] = b1;
	b[2] = b2;

	if(d < ray->lambda && d > EPSILON)
	{
		ray->lambda = d;
		ray->obj = this;
		STATISTIC(STAT_TRIANGLEHIT);
		return TRUE;
	}
	return FALSE;
}
#endif

/*************
 * DESCRIPTION:   Compute normal to triangle at pos
 * INPUT:         dir      ray direction
 *                hitpos   hit position
 *                nrm      computed normal
 *                geon     geometric triangle normal
 * OUTPUT:        none
 *************/
void TRIANGLE::Normal(const VECTOR *dir, const VECTOR *hitpos, VECTOR *nrm, VECTOR *geon)
{
	*geon = norm;

	if(!vnorm)
	{
		*nrm = norm;
	}
	else
	{
		nrm->x = b[0]*vnorm[0].x + b[1]*vnorm[1].x + b[2]*vnorm[2].x;
		nrm->y = b[0]*vnorm[0].y + b[1]*vnorm[1].y + b[2]*vnorm[2].y;
		nrm->z = b[0]*vnorm[0].z + b[1]*vnorm[1].z + b[2]*vnorm[2].z;
		VecNormalizeNoRet(nrm);
	}

	if(dotp(dir, nrm) > 0.f)
	{
		// flip normal
		SetVector(nrm, -nrm->x, -nrm->y, -nrm->z);
		SetVector(geon, -geon->x, -geon->y, -geon->z);
	}
}

/*************
 * DESCRIPTION:   Get brush coordinates at pos
 * INPUT:         hitpos   hit position
 *                u,v      mapping coordinates
 * OUTPUT:        none
 *************/
void TRIANGLE::BrushCoords(const VECTOR *hitpos, float *u, float *v)
{
	if(!brushcoord)
	{
		*u = *v = 0.f;
	}
	else
	{
		*u = b[0]*brushcoord[0].x + b[1]*brushcoord[1].x + b[2]*brushcoord[2].x;
		*v = b[0]*brushcoord[0].y + b[1]*brushcoord[1].y + b[2]*brushcoord[2].y;
	}
}

/*************
 * DESCRIPTION:   generate normal for one triangle point
 * INPUT:         tnum     current triangle index
 *                n        calculated normal
 *                normals  flat normal array
 *                edgidx   index array which points to two triangles for each edge
 *                p        point to generate normal of
 *                edges    edge array
 *                trias    triangle array
 * OUTPUT:        -
 *************/
static void GenerateNormal(UWORD tnum, VECTOR *n, VECTOR *normals, UWORD *edgidx, UWORD p, EDGE *edges, MESH *trias)
{
	UWORD e[2], edge, edge2, oldtria;
	int j, i;
	MESH *tria, *t;

	tria = &trias[tnum];

	j = 0;
	if((p == edges[tria->e[0]].p[0]) || (p == edges[tria->e[0]].p[1]))
		e[j++] = tria->e[0];
	if((p == edges[tria->e[1]].p[0]) || (p == edges[tria->e[1]].p[1]))
		e[j++] = tria->e[1];

	if((p == edges[tria->e[2]].p[0]) || (p == edges[tria->e[2]].p[1]))
	{
		// degenerated triangle ?
		if(j==2)
			return;
		e[j++] = tria->e[2];
	}

	for(j=0; j<2; j++)
	{
		edge = e[j];
		oldtria = tnum;
		i = 0;
		while(!(edges[edge].flags & EDGE_SHARP) && i<50)
		{
			// get next triangle with same edge
			edge2 = edge*2;
			if(oldtria != edgidx[edge2])
			{
				oldtria = edgidx[edge2];
				t = &trias[oldtria];
			}
			else
			{
				oldtria = edgidx[edge2+1];
				if(oldtria == 0xffff)
					break;
				t = &trias[oldtria];
			}

			if(t == tria)
			{
				// avoid endless loops
				break;
			}

			// add normal
			if(dotp(n, &normals[oldtria]) < 0.f)
				VecSub(n, &normals[oldtria], n);
			else
				VecAdd(n, &normals[oldtria], n);

			// get new edge
			if((edge != t->e[0]) && ((p == edges[t->e[0]].p[0]) || (p == edges[t->e[0]].p[1])))
				edge = t->e[0];
			else
			{
				if((edge != t->e[1]) && ((p == edges[t->e[1]].p[0]) || (p == edges[t->e[1]].p[1])))
					 edge = t->e[1];
				else
				{
					 if((edge != t->e[2]) && ((p == edges[t->e[2]].p[0]) || (p == edges[t->e[2]].p[1])))
						 edge = t->e[2];
				}
			}
			// continue the search with the new edge

			i++;
		}
	}
	// Normalize
	VecNormalize(n);
}

/*************
 * DESCRIPTION:   add a mesh and calculate normals
 * INPUT:         srf         surface
 *                surfaces    array of surface pointer, if the mesh has different surfaces (NULL else)
 *                actor       actor
 *                points      mesh points
 *                edges       mesh edges
 *                edgecount   amount of edges
 *                mesh        mesh triangles
 *                triacount   amount of triangles
 *                matrix      transform matrix
 *                rotmatrix   rotate matrix
 *                nophong     disable phong shading
 *                brushcoords brush mapping coordinates
 * OUTPUT:        first created triangle or NULL if failed
 *************/
TRIANGLE *CreateMesh(RSICONTEXT *rc, SURFACE *srf, SURFACE **surfs, ACTOR *actor, VECTOR *points, EDGE *edges, int edgecount,
	MESH *mesh, int triacount, MATRIX *matrix, MATRIX *rotmatrix, BOOL nophong, VECT2D *brushcoords)
{
	int i;
	UWORD p1,p2,p3;
	UWORD *tmpedges, *te;
	VECTOR *tmpnorms, e1,e2;
	MESH *tria;
	TRIANGLE *triangle, *t;

	if(!nophong)
	{
		tmpnorms = new VECTOR[triacount];
		if (!tmpnorms)
			return NULL;

		tmpedges = new UWORD[edgecount*2];
		if (!tmpedges)
		{
			delete tmpnorms;
			return NULL;
		}

		memset(tmpedges, 0xFF, edgecount*sizeof(UWORD)*2);

		// calculate geometric normals
		// and generate edge index
		tria = mesh;
		for (i=0; i<triacount; i++)
		{
			// calculate normals of triangles
			p1 = edges[tria->e[0]].p[0];
			p2 = edges[tria->e[0]].p[1];
			if((p1 != edges[tria->e[1]].p[0]) &&
				(p2 != edges[tria->e[1]].p[0]))
			{
				p3 = edges[tria->e[1]].p[0];
			}
			else
			{
				p3 = edges[tria->e[1]].p[1];
			}

			VecSub(&points[p2], &points[p1], &e1);
			VecSub(&points[p3], &points[p1], &e2);
			VecNormCross(&e1, &e2, &tmpnorms[i]);

			te = &tmpedges[tria->e[0]*2];
			if(te[0] == 0xFFFF)
				te[0] = i;
			else
				te[1] = i;

			te = &tmpedges[tria->e[1]*2];
			if(te[0] == 0xFFFF)
				te[0] = i;
			else
				te[1] = i;

			te = &tmpedges[tria->e[2]*2];
			if(te[0] == 0xFFFF)
				te[0] = i;
			else
				te[1] = i;

			tria++;
		}
	}

#ifndef __STORM__
	// there is a bug in StormC which causes a crash if an array
	// is allocated when the destructor of the base class is virtual.
	// I have to test again if it works when a new version is available.
	// Do also change TriangleGetNext().
	triangle = new TRIANGLE[triacount];
	if(!triangle)
	{
		if(!nophong)
		{
			delete [ ] tmpedges;
			delete [ ] tmpnorms;
		}
		return NULL;
	}

	t = triangle;
#else
#endif
	for(i=0; i<triacount; i++)
	{
#ifdef __STORM__
		t = new TRIANGLE;
		if(!t)
		{
			if(!nophong)
			{
				delete [ ] tmpedges;
				delete [ ] tmpnorms;
			}
			return NULL;
		}
#endif

		if(surfs)
			t->surf = surfs[i];
		else
			t->surf = srf;

		t->actor = actor;
		t->flags |= OBJECT_INBLOCK;
		t->Insert(rc);
#ifndef __STORM__
		t++;
#endif
	}
#ifndef __STORM__
	triangle->flags |= OBJECT_FIRSTBLOCK;
#else
	triangle = t;
#endif

	tria = mesh;
	t = triangle;
	for (i=0; i<triacount; i++)
	{
		p1 = edges[tria->e[0]].p[0];
		p2 = edges[tria->e[0]].p[1];
		if((p1 != edges[tria->e[1]].p[0]) &&
			(p2 != edges[tria->e[1]].p[0]))
		{
			p3 = edges[tria->e[1]].p[0];
		}
		else
		{
			p3 = edges[tria->e[1]].p[1];
		}

		if(!nophong)
		{
			t->vnorm = new VECTOR[3];
			if(!t->vnorm)
			{
				delete triangle;
				delete [ ] tmpedges;
				delete [ ] tmpnorms;
				return NULL;
			}

			t->vnorm[0] = tmpnorms[i];
			t->vnorm[1] = tmpnorms[i];
			t->vnorm[2] = tmpnorms[i];

			GenerateNormal(i, &t->vnorm[0], tmpnorms, tmpedges, p1, edges, mesh);
			GenerateNormal(i, &t->vnorm[1], tmpnorms, tmpedges, p2, edges, mesh);
			GenerateNormal(i, &t->vnorm[2], tmpnorms, tmpedges, p3, edges, mesh);

			rotmatrix->MultVectMat(&t->vnorm[0]);
			rotmatrix->MultVectMat(&t->vnorm[1]);
			rotmatrix->MultVectMat(&t->vnorm[2]);
		}

		t->p[0] = points[p1];
		matrix->MultVectMat(&t->p[0]);
		t->p[1] = points[p2];
		matrix->MultVectMat(&t->p[1]);
		t->p[2] = points[p3];
		matrix->MultVectMat(&t->p[2]);

#ifdef __STORM__
		t = (TRIANGLE*)t->GetNext();
#else
		t++;
#endif
		tria++;
	}
	if(!nophong)
	{
		delete [ ] tmpedges;
		delete [ ] tmpnorms;
	}
	return triangle;
}
