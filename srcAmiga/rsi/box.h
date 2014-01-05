/***************
 * MODUL:         box
 * NAME:          box.h
 * DESCRIPTION:   box class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.01.97 ah    initial release
 ***************/

#ifndef BOX_H
#define BOX_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

class BOX : public OBJECT
{
	public:
		VECTOR pos;
		VECTOR lowbounds, highbounds;
		VECTOR orient_x,orient_y,orient_z;

		BOX();
		ULONG Identify() { return IDENT_BOX; };
		void Update(const float);
		void GenVoxel();
		BOOL RayIntersect(RAY *);
		void Normal(const VECTOR *, const VECTOR *, VECTOR *, VECTOR *);
};

#endif /* BOX_H */
