/***************
 * MODULE:        flares.cpp
 * PROJECT:       RayStorm
 * DESCRIPTION:   lens flares
 * AUTHORS:       Mike Hesser
 * HISTORY:       DATE     NAME  COMMENT
 *                12.10.96 mh    first release
 *                13.10.96 mh    polyeder
 *                24.11.96 mh    update
 ***************/

#include <math.h>

#ifndef FLARES_H
#include "flares.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
FLARE::FLARE()
{
	type = FLARE_DISC;
	func = FLARE_PLAIN;
	color.r = color.g = color.b = 0.2f;
	radius = 0.1f;
	pos = 0.5f;
	tilt = 0.f;
	edges = 5;
	sr = 0;
}

/*************
 * DESCRIPTION:   update flare data, initialize some helper variables
 *                to speed up the things
 * INPUT:         xres  horizontal render resolution
 * OUTPUT:        none
 *************/
void FLARE::Update(int xres)
{
	sr = radius * xres * 0.5f;

	if(edges < 3)
		edges = 3;

	phi = 0.5f*PI - PI/edges;
	sinphi = (float)sin(phi)*sr;
}

/*************
 * DESCRIPTION:   draws one horizontal line of a flare
 * INPUT:         line  pointer to current line
 *                fx    horizontal center of flare
 *                x,y   coord. of pixel
 * OUTPUT:        none
 *************/
void FLARE::Draw(SMALL_COLOR *line, int fx, int x, int y)
{
	float hr, dx, f, r, alpha;
	COLOR intensity;
	int h, xx;
	BOOL flag;

	r = (float)sqrt(float(x*x + y*y));

	hr = sr;

	if (type == FLARE_POLY)
	{
#ifdef __STORM__
		alpha = newatan2(y, x) + tilt;
#else
		alpha = (float)atan2(y, x) + tilt;
#endif
		if (alpha < 0.f)
			alpha += 2.f*PI;

		alpha = (float)fmod(alpha, 2.f*PI/edges);
		hr = sinphi/(float)sin(PI - phi - alpha);

		if (r > hr)
			return;
	}

	// some kind of antialiasing
	if (r > hr*0.93f)
	{
		dx = 10.f*(r - 0.93f*hr)/hr;
		r = hr*0.93f;
		flag = TRUE;
	}
	else
		flag = FALSE;
		
	switch (func)
	{
		case FLARE_PLAIN:
					intensity.r =
					intensity.g =
					intensity.b = 1.f;
					break;
		case FLARE_EXP1:
					f = (r - hr)/hr;
					intensity.r =
					intensity.g =
					intensity.b = (float)exp(-100.f*f*f);
					break;
		case FLARE_EXP2:
					f = (r - hr)/hr;
					intensity.r =
					intensity.g =
					intensity.b = (float)exp(-10.f*f*f);
					break;
		case FLARE_EXP3:
					f = (r - hr)/hr;
					intensity.r =
					intensity.g =
					intensity.b = (float)exp(-2.f*f*f);
					break;
		case FLARE_RADIAL:
					intensity.r =
					intensity.g =
					intensity.b = r/hr;
					break;
		case FLARE_RAINBOW:
					f = ((r - hr*.9f)/hr);
					intensity.r = (float)exp(-40.f*f*f);
					f = ((r - hr*.8f)/hr);
					intensity.g = (float)exp(-40.f*f*f);
					f = ((r - hr*.7f)/hr);
					intensity.b = (float)exp(-40.f*f*f);
					break;
	}
	
	if (flag)
	{
		dx *= dx;
		intensity.r = -intensity.r*dx + intensity.r;
		intensity.g = -intensity.g*dx + intensity.g;
		intensity.b = -intensity.b*dx + intensity.b;
	}
	
	xx = x + fx;
	h = int(line[xx].r) + int(255*color.r*intensity.r);
	if (h > 255)
		h = 255;
	line[xx].r = h;
		h = int(line[xx].g) + int(255*color.g*intensity.g);
	if (h > 255)
		h = 255;
	line[xx].g = h;
		h = int(line[xx].b) + int(255*color.b*intensity.b);
	if (h > 255)
		h = 255;
	line[xx].b = h;
}

/*************
 * DESCRIPTION:   delete flare and all flares which are linked to it
 * INPUT:         -
 * OUTPUT:        -
 *************/
void FreeFlares(FLARE *flare)
{
	FLARE *next;

	while (flare)
	{
		next = (FLARE *)flare->GetNext();
		delete flare;
		flare = next;
	}
}
