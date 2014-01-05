/***************
 * MODUL:         camera
 * NAME:          camera.h
 * DESCRIPTION:   camera-class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    11.03.95 ah    added new structure membeers and function
 *                   InitCamera()
 *    03.05.95 ah    changed CalcRay(int,int,ray*) to
 *                   CalcRay(float,float,ray*)
 *    18.07.95 ah    added aperture and focaldist
 *    27.08.95 ah    added actor
 *    11.09.95 ah    added values for field rendering
 *    13.09.95 ah    added Update()
 *    20.09.95 ah    added posactor and viewactor
 ***************/

#ifndef CAMERA_H
#define CAMERA_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef WORLD_H
#include "world.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

class ACTOR;
class RSICONTEXT;

#define DOF_WIDTH 7
#define DOF_DEPTHS 7
#define FASTDOF 1

class CAMERA
{
	public:
		VECTOR   firstray;   // direction of first ray
		VECTOR   scrnx;      // direction delta
		VECTOR   scrny;      // direction delta
		VECTOR   scrni,scrnj;
		float    focaldist;  // focal distance
		float    aperture;   // camera aperture
		VECTOR   pos;        // camera-position
		VECTOR   lookp;      // lookpoint
		VECTOR   vup;        // view-up-vector
		float    hfov,vfov;  // horizontal and vertical field of view
		int      xres,yres;  // Screen x and y resolution
		int      left,top,right,bottom;     // coordinates for field rendering
		float    time;       // actual time
		ACTOR    *posactor;  // pointer to position actor
		ACTOR    *viewactor; // pointer to view point actor
		int      flags;      // (fast Depth of Field,...)

		CAMERA();
		void ToDefaults();
		void Update(const float);
		void InitCamera();
		void CalcRay(float, float, RAY *, WORLD *, const float);
		void FocusBlurRay(RAY *, WORLD *);
};

void ApplyDepthOfField(RSICONTEXT*);

#endif /* CAMERA_H */
