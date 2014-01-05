/***************
 * MODUL:         triangle
 * NAME:          triangle.h
 * DESCRIPTION:   This file includes the triangle class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.02.95 ah    initial release
 *    14.03.95 ah    added Update()
 *    11.10.95 ah    added Identify() and stuff for triangle blocks
 *    07.03.97 ah    added CreateMesh
 *    10.05.99 ah    added brush mapping coordinates
 ***************/

#ifndef TRIANGLE_H
#define TRIANGLE_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

class TRIANGLE : public OBJECT
{
	public:
		VECTOR p[3];      // vertices
		VECTOR *vnorm;    // pointer vertex normals
		VECT2D *brushcoord; // brush mapping coordinates
	private:
		VECTOR norm;      // normal
		VECTOR e[3];      // "edge" vectors (scaled)
		float d;          // plane constant
		float b[3];       // Array of barycentric coordinates

	public:
		TRIANGLE();
		~TRIANGLE();
		void Update(const float);
		void GenVoxel();
		BOOL RayIntersect(RAY *);
		void Normal(const VECTOR *, const VECTOR *, VECTOR *, VECTOR *);
		void BrushCoords(const VECTOR *, float *, float *);
		ULONG Identify() { return IDENT_TRIANGLE; };
};

typedef struct
{
	UWORD p[2];    // point numbers
	UWORD flags;
} EDGE;

#define EDGE_SHARP 1

typedef struct
{
	UWORD e[3];    // edge numbers
} MESH;

TRIANGLE *CreateMesh(RSICONTEXT*, SURFACE*, SURFACE **, ACTOR*, VECTOR*, EDGE*, int, MESH*, int, MATRIX*, MATRIX*, BOOL, VECT2D*);

#endif /* TRIANGLE_H */
