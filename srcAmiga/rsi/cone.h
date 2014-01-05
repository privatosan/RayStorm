/***************
 * MODUL:         cone
 * NAME:          cone.h
 * DESCRIPTION:   cone class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    23.03.97 ah    initial release
 ***************/

#ifndef CONE_H
#define CONE_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

class CONE : public OBJECT
{
		MATRIX trans;

	public:
		VECTOR pos;
		VECTOR ox, oy, oz;
		VECTOR size;

		CONE();
		ULONG Identify() { return IDENT_CONE; };
		void Update(const float);
		void GenVoxel();
		BOOL RayIntersect(RAY *);
		void Normal(const VECTOR *, const VECTOR *, VECTOR *, VECTOR *);
};

#endif /* CONE_H */
