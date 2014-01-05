/***************
 * MODUL:         csg
 * NAME:          cgs.h
 * DESCRIPTION:   csg class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.01.97 ah    initial release
 ***************/

#ifndef CSG_H
#define CSG_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

class CSG : public OBJECT
{
	public:
		OBJECT *obj1, *obj2;
		OBJECT *prev;
		OBJECT *hitobj;

		CSG();
		~CSG();
		ULONG Identify() { return IDENT_CSG; };
		void Update(const float);
		void GenVoxel();
		BOOL RayIntersect(RAY *);
		void Normal(const VECTOR *, const VECTOR *, VECTOR *, VECTOR *);
		BOOL Duplicate();
		void DisableObject(RSICONTEXT*);
};

#endif /* CSG_H */
