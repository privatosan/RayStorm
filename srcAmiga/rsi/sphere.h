/***************
 * MODUL:         sphere
 * NAME:          sphere.h
 * DESCRIPTION:   sphere class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    13.03.95 ah    added Update()
 *    11.10.95 ah    added Identify()
 ***************/

#ifndef SPHERE_H
#define SPHERE_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

/* Sphere */
class SPHERE : public OBJECT
{
	public:
		float r, rsq;     /* radius, radius squared */
		float ir, irsq;   /* inner radius, inner radius squared for hypertexture */
		float inv_delta_irsq;
		VECTOR pos;       /* position */

		SPHERE();
		ULONG Identify();
		void Update(const float);
		void GenVoxel();
		BOOL RayIntersect(RAY *);
		void Normal(const VECTOR *, const VECTOR *, VECTOR *, VECTOR *);
		float Density(const VECTOR *);
};

#endif /* SPHERE_H */
