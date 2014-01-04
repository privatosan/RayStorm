/***************
 * MODULE:        camera
 * NAME:          camera.cpp
 * DESCRIPTION:   Functions for camera
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    11.03.95 ah    added InitCamera()
 *    03.05.95 ah    changed CalcRay(int,int,ray*) to
 *       CalcRay(float,float,ray*)
 *    11.09.95 ah    added parameters for field rendering
 *    13.09.95 ah    added Update()
 *    02.03.97 mh    added ApplyDepthOfField
 ***************/

#include <time.h>
#include <math.h>

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

#ifndef ACTOR_H
#include "actor.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
CAMERA::CAMERA()
{
	ToDefaults();
}

/*************
 * DESCRIPTION:   Init camera with defaults
 * INPUT:         none
 * OUTPUT:        none
 *************/
void CAMERA::ToDefaults()
{
	// camera-position
	SetVector(&pos, 0.f, 0.f, -10.f);
	// lookpoint
	SetVector(&lookp, 0.f, 0.f, 0.f);
	// view-up-vector
	SetVector(&vup, 0.f, 1.f, 0.f);
	// horizontal and vertical field of view
	hfov = 45.f;
	vfov = 45.f;
	// Screen x and y resolution
	xres = 128;
	yres = 128;
	left = top = 0;
	right = bottom = 127;
	// depth of field
	focaldist = 1.f;
	aperture = 0.f;
	flags = 0;
	// animation parameters
	time = 0;
	posactor = NULL;
	viewactor = NULL;
}

/*************
 * DESCRIPTION:   Initialize Camera (MUST be called if parameters changed)
 * INPUT:         none
 * OUTPUT:        none
 *************/
void CAMERA::InitCamera()
{
	float lookdist, magnitude;
	VECTOR dir;

	VecSub(&lookp, &pos, &dir);
	firstray = dir;

	lookdist = VecNormalize(&dir);
	VecNormCross(&dir, &vup, &scrni);
	VecNormCross(&scrni, &dir, &scrnj);

	magnitude = -2.f*lookdist * (float)tan(0.5f*hfov*PI_180)/xres;

	VecScale(magnitude, &scrni, &scrnx);
	magnitude = 2.f*lookdist * (float)tan(0.5f*vfov*PI_180)/yres;

	VecScale(magnitude, &scrnj, &scrny);

	firstray.x -= 0.5f*yres*scrny.x + 0.5f*xres*scrnx.x;
	firstray.y -= 0.5f*yres*scrny.y + 0.5f*xres*scrnx.y;
	firstray.z -= 0.5f*yres*scrny.z + 0.5f*xres*scrnx.z;

	DoFData.xres = xres;
	DoFData.yres = yres;
	DoFData.left = left;
	DoFData.top = top;
	DoFData.right = right;
	DoFData.bottom = bottom;
	DoFData.focaldist = focaldist;
	DoFData.hfov = hfov*PI/180.0f;
	DoFData.vfov = vfov*PI/180.0f;
	DoFData.aperture = 2*aperture;
	DoFData.samples = 7;

	DoFData.flags = 0;
	if ((flags & HIDDENAREA) == HIDDENAREA)
		DoFData.flags = DoFHidden;
	if ((flags & ACCELERATE) == ACCELERATE)
		DoFData.flags |= DoFAccelerate;

	DoF.Init(&DoFData);
}

/*************
 * DESCRIPTION:   Calculate ray direction
 * INPUT:         x     x position
 *                y     y position
 *                ray   ray structure
 *                world world structure
 *                time  actual time
 * OUTPUT:        none
 *************/
void CAMERA::CalcRay(float x, float y, RAY *ray, WORLD *world, const float time)
{
	// update camera
	if((posactor || viewactor) && this->time != time)
	{
		Update(time);
		InitCamera();
	}

	// Set startpoint to eye-point
	ray->start = pos;

	// Calculate ray direction.
	ray->dir.x = firstray.x + x*scrnx.x + y*scrny.x;
	ray->dir.y = firstray.y + x*scrnx.y + y*scrny.y;
	ray->dir.z = firstray.z + x*scrnx.z + y*scrny.z;

	VecNormalizeNoRet(&ray->dir);

	if(fabs(ray->dir.x) < EPSILON)
		ray->dir.x = 0.f;
	if(fabs(ray->dir.y) < EPSILON)
		ray->dir.y = 0.f;
	if(fabs(ray->dir.z) < EPSILON)
		ray->dir.z = 0.f;

	// do only depth of field if aperture > 0, supersampling on and fastDOF off
	if((aperture > 0.f) && (world->samples > 1) && !(flags & FASTDOF))
		this->FocusBlurRay(ray, world);

	ray->ray_length = 0.f;
}

/*************
 * DESCRIPTION:   Adjust the initial ray to account for an aperture and a focal
 *       distance.  The ray argument is assumed to be an initial ray, and
 *       always reset to the eye point.  It is assumed to be unit length.
 * INPUT:         ray      pointer to ray structure
 *                world    pointer to world structure
 * OUTPUT:        none
 *************/
void CAMERA::FocusBlurRay(RAY *ray, WORLD *world)
{
	VECTOR circle_point, aperture_inc;

	/*
	 * Find a point on a unit circle and scale by aperture size.
	 * This simulates rays passing thru different parts of the aperture.
	 * Treat the point as a vector and rotate it so the circle lies
	 * in the plane of the screen. Add the aperture increment to the
	 * starting position of the ray. Stretch the ray to be focaldist
	 * in length. Subtract the aperture increment from the end of the
	 * long ray. This insures that the ray heads toward a point at
	 * the specified focus distance, so that point will be in focus.
	 * Normalize the ray, and that's it. Really.
	 */

	world->UnitCirclePoint(&circle_point, ray->sample);
	VecComb(aperture * circle_point.x, &scrni,
			  aperture * circle_point.y, &scrnj,
			  &aperture_inc);
	VecAdd(&aperture_inc, &pos, &(ray->start));
	VecScale(focaldist, &ray->dir, &(ray->dir));
	VecSub(&ray->dir, &aperture_inc, &(ray->dir));
	VecNormalizeNoRet(&ray->dir);
}

/*************
 * DESCRIPTION:   Update camera parameters
 * INPUT:         time     current time
 * OUTPUT:        none
 *************/
void CAMERA::Update(const float time)
{
	TIME t;

	if(posactor)
	{
		if(time != this->time)
		{
			if((posactor->time.begin != this->time) || (posactor->time.end != time))
			{
				t.begin = this->time;
				t.end = time;
				posactor->Animate(&t);
			}
			posactor->matrix->MultVectMat(&pos);
		}
	}
	if(viewactor)
	{
		if(time != this->time)
		{
			if((viewactor->time.begin != this->time) || (viewactor->time.end != time))
			{
				t.begin = this->time;
				t.end = time;
				viewactor->Animate(&t);
			}
			viewactor->matrix->MultVectMat(&lookp);
		}
	}
	this->time = time;
}

/*************
 * DESCRIPTION: second method to make depth of field
 * INPUT:       rc       render context
 * OUTPUT:      none
 *************/
void ApplyDepthOfField(RSICONTEXT *rc)
{
	rc->camera.DoF.CalculateDoF(rc, SamplePixel);
}
