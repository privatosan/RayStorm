/***************
 * MODUL:         world
 * NAME:          world.cpp
 * DESCRIPTION:   Global definitons
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    15.02.95 ah    initial release
 *    08.04.95 ah    added quickrender
 *    03.05.95 ah    added samples, filterwidth and depth
 *    08.08.95 ah    added WORLD::~WORLD()
 *    15.08.95 ah    added random jitter
 *    16.08.95 ah    added backdrop
 *    21.08.95 ah    added global fog
 *    02.09.95 ah    added WORLD::UnitCirclePointDistrib()
 *    28.09.95 ah    added global reflection map
 *    02.11.95 ah    added DoGlobalFog()
 *    13.12.96 ah    added minobjects
 *    12.03.97 ah    added step
 ***************/

#include <stdlib.h>
#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef WORLD_H
#include "world.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
WORLD::WORLD()
{
	brushpath = NULL;
	texturepath = NULL;
	objectpath = NULL;
	backdrop = NULL;
	reflmap = NULL;
	ToDefaults();
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
WORLD::~WORLD()
{
	if(brushpath)
		delete [ ] brushpath;
	if(texturepath)
		delete [ ] texturepath;
	if(objectpath)
		delete [ ] objectpath;
	if(backdrop)
		delete [ ] backdrop;
	if(reflmap)
		delete reflmap;
}

/*************
 * DESCRIPTION:   Init world with defaults
 * INPUT:         none
 * OUTPUT:        none
 *************/
void WORLD::ToDefaults()
{
	back.r = back.g = back.b = 0.f;
	ambient.r = ambient.g = ambient.b = 0.f;
	fog.r = fog.g = fog.b = 1.f;
	foglength = 0.f;
	fogheight = 0.f;
	flags = 0;
	samples = 1;
	filterwidth = 1.3f;
	contrast.r = .2f;
	contrast.g = .15f;
	contrast.b = .3f;
	motionblur_samples = 1;
	softshadow_samples = 1;
	depth = 3;
	time = timelength = 0.f;
	minobjects = 3;
	hyper_steps = 20;
	hyper_max_density_step = 0.02f;
	hyper_min_density_step = 0.002f;
	validkeyfile = FALSE;

	if(brushpath)
	{
		delete [ ] brushpath;
		brushpath = NULL;
	}
	if(texturepath)
	{
		delete [ ] texturepath;
		texturepath = NULL;
	}
	if(objectpath)
	{
		delete [ ] objectpath;
		objectpath = NULL;
	}
	if(backdrop)
	{
		delete [ ] backdrop;
		backdrop = NULL;
	}
	if(reflmap)
	{
		delete reflmap;
		reflmap = NULL;
	}
}

/*************
 * DESCRIPTION:   Sets the globa backdrop pic
 * INPUT:         name     new picture name
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL WORLD::SetBackdropPic(char *name)
{
	if (backdrop)
		delete [ ] backdrop;
	backdrop = new char[strlen(name) + 1];
	if (!backdrop)
		return FALSE;

	strcpy(backdrop, name);

	return TRUE;
}

/*************
 * DESCRIPTION:   Find a point on a unit circle that is separated from other random
 *       points by some jitter spacing.
 *       It should do the above, but the temporary hack below just finds a
 *       jittered point in a unit square. (used for DOF)
 * INPUT:         pnt      pointer to vector
 *                sample
 * OUTPUT:        none
 *************/
void WORLD::UnitCirclePoint(VECTOR *pnt, int sample)
{
	/*
	 * This picks a random point on a -1 to 1 square.  The jitter stuff
	 * is correct enough to avoid excessive noise.  An extremely blurry
	 * bright highlight will look squarish, not roundish.  Sorry.
	 */
	float jit;

	jit = 2.f / samples;

	if(flags & WORLD_RANDOM_JITTER)
	{
		pnt->x = (float(rand())/RAND_MAX)*jit - 1.f +
			(sample % samples) * jit;
		pnt->y = (float(rand())/RAND_MAX)*jit - 1.f +
			(sample / samples) * jit;
	}
	else
	{
		pnt->x = (sample % samples) * jit - 1.f;
		pnt->y = (sample / samples) * jit - 1.f;
	}
	pnt->z = 0.f;
}

/*************
 * DESCRIPTION:   Find a point on a unit circle that is separated from other random
 *       points by some jitter spacing.
 *       It should do the above, but the temporary hack below just finds a
 *       jittered point in a unit square.
 * INPUT:         pnt      pointer to vector
 *                sample
 * OUTPUT:        none
 *************/
void WORLD::UnitCirclePointSoftShadow(VECTOR *pnt, int sample)
{
	/*
	 * This picks a random point on a -1 to 1 square.  The jitter stuff
	 * is correct enough to avoid excessive noise.  An extremely blurry
	 * bright highlight will look squarish, not roundish.  Sorry.
	 */
	float jit;

	jit = 2.f / softshadow_samples;

	if(flags & WORLD_RANDOM_JITTER)
	{
		pnt->x = (float(rand())/RAND_MAX)*jit - 1.f +
			(sample % softshadow_samples) * jit;
		pnt->y = (float(rand())/RAND_MAX)*jit - 1.f +
			(sample / softshadow_samples) * jit;
	}
	else
	{
		pnt->x = (sample % softshadow_samples) * jit - 1.f;
		pnt->y = (sample / softshadow_samples) * jit - 1.f;
	}
	pnt->z = 0.f;
}

/*************
 * DESCRIPTION:   Calculate global fog
 * INPUT:         ray      actual ray
 *                pixel    background color
 *                color    output fog color
 * OUTPUT:        TRUE if fog else FALSE
 *************/
BOOL WORLD::DoGlobalFog(RAY *ray, SMALL_COLOR *pixel, COLOR *color)
{
	float k;

	// Do global fog
	if (foglength > EPSILON)
	{
		// test if ray intersects global fog
		if (!(ray->dir.y > EPSILON && ray->start.y > fogheight))
		{
			if (ray->dir.y > EPSILON && ray->start.y < fogheight)
			{
				k = (fogheight - ray->start.y) / ray->dir.y;
				// Add fog to the given color.
				k = LNHALF * k/foglength;
				k = k < -10.f ? 0.f : (float)exp(k);
				color->r = float(pixel->r)/255*k + (1.f-k)*fog.r;
				color->g = float(pixel->g)/255*k + (1.f-k)*fog.g;
				color->b = float(pixel->b)/255*k + (1.f-k)*fog.b;
			}
			else
			{
				// endless fog
				*color = fog;
			}
			return TRUE;
		}
	}
	return FALSE;
}

