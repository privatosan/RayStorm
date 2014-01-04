/***************
 * MODUL:         plane
 * NAME:          plane.h
 * DESCRIPTION:   plane class definition
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		16.02.95	ah		initial release
 *		14.03.95	ah		added Update()
 *		11.10.95	ah		added Identify()
 ***************/

#ifndef PLANE_H
#define PLANE_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

class PLANE : public OBJECT
{
	private:
		float d;				// plane constant
	public:
		VECTOR norm;		// normal
		VECTOR pos;			// position

		PLANE();
		ULONG Identify() { return IDENT_PLANE; };
		void Update(const float);
		void GenVoxel();
		BOOL RayIntersect(RAY *);
		void Normal(const VECTOR *, const VECTOR *, VECTOR *, VECTOR *);
};

#endif /* PLANE_H */
