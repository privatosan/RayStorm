/***************
 * MODUL:         cylinder
 * NAME:          cylinder.h
 * DESCRIPTION:   cylinder class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    18.03.97 ah    initial release
 ***************/

#ifndef CYLINDER_H
#define CYLINDER_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

class CYLINDER : public OBJECT
{
		MATRIX trans;

	public:
		VECTOR pos;
		VECTOR ox, oy, oz;
		VECTOR size;

		CYLINDER();
		ULONG Identify() { return IDENT_CYLINDER; };
		void Update(const float);
		void GenVoxel();
		BOOL RayIntersect(RAY *);
		void Normal(const VECTOR *, const VECTOR *, VECTOR *, VECTOR *);
};

#endif /* CYLINDER_H */
