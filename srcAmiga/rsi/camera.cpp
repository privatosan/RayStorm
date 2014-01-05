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
}

/*************
 * DESCRIPTION:   Calculate ray direction
 * INPUT:         x     x position
 *                y     y position
 *                ray   ray structure
 *                  world world structure
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
* DESCRIPTION: create filter for smooting
* INPUT:       filter   array which receives the filter values
*                       (size must be samples*samples)
*              samples  width of gaussian filter matrix
*              fwidth   filter width
* OUTPUT:      none
*************/
void CreateGaussianFilter(float filter[], int samples, float fwidth)
{
	float upos,vpos,norm;
	int x,y;
	float sampledelta;

	// Init filter
	sampledelta = fwidth/samples;
	norm = 0.f;
	upos = -0.5f*fwidth + 0.5f*sampledelta;
	for (x = 0; x < samples; x++)
	{
		vpos = -0.5f*fwidth + 0.5f*sampledelta;
		for (y = 0; y < samples; y++)
		{
			filter[x + y*samples] = (float)exp(-0.5f*(upos*upos + vpos*vpos));
			norm += filter[x + y*samples];
			vpos += sampledelta;
		}
		upos += sampledelta;
	}
	norm = 1.f/norm;
	for (x = 0; x < samples; x++)
		for (y = 0; y < samples; y++)
			filter[x + y*samples] *= norm;
}

/*************
* DESCRIPTION: second method to make depth of field
*              How does it work?
*              During the render process, depth information was calculated
*              and saved within the info byte. Look after INFO_DEPTH (color.h)
*              to see how much bits are reserved for this information.
*              The constant DOF_DEPTHS (camera.h) keeps the number of different
*              gaussian filters which are used to blur the image. ApplyDepthOfField
*              goes through all pixels of the screen buffer and chooses an
*              appropriate filter according to the pixels' depth. The surrounding
*              region is scanned (the width of the region is DOF_WIDTH (camera.h))
*              and weighted with the gaussian filter matrix. Pixels in the region
*              which are outside the screen or don't have the same depth as the current
*              pixel are not be taken to compute the resulting color. Instead, the
*              corresponding weighting value from the gaussian filter matrix is added
*              to the "norm" variable. Finally the resulting color is multiplied with
+              the "norm" variable. This is necessary to keep the brightness constant.
*
* INPUT:       rc       render context
* OUTPUT:      none
*************/
void ApplyDepthOfField(RSICONTEXT *rc)
{
	int i, x, y, dx, dy, xx, yy, filternum;
	float percent, percent_per_row;
	float *filter[DOF_DEPTHS], *flt, norm, scale;
	SMALL_COLOR *line, scol;
	COLOR col;
	clock_t starttime;

	starttime = clock();
	percent_per_row = (float)1./(rc->camera.bottom - rc->camera.top);
	line = rc->scrarray + rc->camera.top*rc->camera.xres;

	// create gaussian filters for all depths
	for (i = 0; i < DOF_DEPTHS; i++)
	{
		filter[i] = new float[DOF_WIDTH*DOF_WIDTH];
		if (!filter[i])
			return;
		CreateGaussianFilter(filter[i], DOF_WIDTH, 3*DOF_WIDTH - rc->camera.aperture*(float(i + 1)/DOF_DEPTHS));
	}

	for (y = rc->camera.top; y < rc->camera.bottom; y++)
	{
		for (x = rc->camera.left; x < rc->camera.right; x++)
		{
			filternum = line[x].info & INFO_DEPTH;
			if (filternum != 0)
			{
				flt = filter[filternum - 1];

				col.r = col.g = col.b = .0f;
				norm = 1.f;
				for (dy = 0; dy < DOF_WIDTH; dy++)
				{
					for (dx = 0; dx < DOF_WIDTH; dx++)
					{
						xx = x + dx - (DOF_WIDTH >> 1);
						yy = y + dy - (DOF_WIDTH >> 1);
						scale = flt[dx + dy*DOF_WIDTH];

						// take only pixels within screen area and same depth
						if (xx >= rc->camera.left && xx <= rc->camera.right &&
							 yy >= rc->camera.top  && yy <= rc->camera.bottom)
						{
							if ((rc->scrarray[xx + yy*rc->camera.xres].info & INFO_DEPTH) == filternum)
							{
								scol = rc->scrarray[xx + yy*rc->camera.xres];
								col.r += scale*scol.r;
								col.g += scale*scol.g;
								col.b += scale*scol.b;
							}
							else
								norm += scale;
						}
						else
							norm += scale;
					}
				}
				line[x].r = int(norm * col.r);
				line[x].g = int(norm * col.g);
				line[x].b = int(norm * col.b);
			}
		}
		percent = (y - rc->camera.top) * percent_per_row;
		rc->UpdateStatus(rc, percent, float(clock() - starttime)/CLOCKS_PER_SEC, y, 1, line);
		rc->Cooperate(rc);

		line += rc->camera.xres;
	}

	for (i = 0; i < DOF_DEPTHS; i++)
		delete filter[i];
}
