/***************
 * MODUL:         sor
 * NAME:          sor.h
 * DESCRIPTION:   surface of revolution class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    20.03.97 ah    initial release
 ***************/

#ifndef SOR_H
#define SOR_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

typedef struct
{
	float r2;
	float h1,h2;
	float a,b,c,d;
} SOR_SPLINE;

class SOR : public OBJECT
{
	private:
		float height1, height2; // Min./Max. height
		float radius;           // Max. radius
		float base_radius;      // Radius**2 of the base plane
		float cap_radius;       // Radius**2 of the cap plane
		int segment;            // segment which was hit
		MATRIX trans;           // matrix for ray coord transformation
		SOR_SPLINE *spline;     // array of spline segments

	public:
		VECTOR pos;             // position
		VECTOR size;            // size
		VECTOR orient_x;        // orientation
		VECTOR orient_y;
		VECTOR orient_z;
		int number;             // amount of points

		SOR();
		~SOR();
		ULONG Identify() { return IDENT_SOR; };
		void Update(const float);
		void GenVoxel();
		BOOL RayIntersect(RAY *);
		void Normal(const VECTOR *, const VECTOR *, VECTOR *, VECTOR *);
		BOOL Compute(VECT2D*);
};

#endif /* SOR_H */
