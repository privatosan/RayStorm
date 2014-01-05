/***************
 * MODULE:        star.cpp
 * PROJECT:       RayStorm
 * DESCRIPTION:   star of light source
 * AUTHORS:       Mike Hesser & Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    30.10.96 mh    first release
 *    14.11.96 mh    destructor
 *    20.11.96 mh    default intensities
 *    27.11.96 ah    added inner halo, improved spikes
 *    14.12.96 ah    added spikewidth
 *    11.11.97 mh    new algorithm
 ***************/

#include <math.h>

#ifndef STAR_H
#include "star.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#define STAR_FALLOFF          1.0f
#define STAR_REDGLOWFALLOFF   1.5f

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
STAR::STAR()
{
	int i;

	starcolor.r = 1.0f;
	starcolor.g = 0.6f;
	starcolor.b = 0.6f;
	ringcolor.r = 0.156f;
	ringcolor.g = 0.0392f;
	ringcolor.b = 0.0196f;

	flags = STAR_ENABLE | STAR_HALOENABLE;
	spikes = 4;
	starradius = 0.3f;
	haloradius = 0.6f;
	innerhaloradius = 0.4f;
	noise = 5.0f;
	tilt = 0.0f;
	brightness = 40.0f;

	intensities = new float[spikes];
	if (!intensities)
		return;

	for(i = 0; i < spikes; i++)
		intensities[i] = 1.f;
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
STAR::~STAR()
{
	if (intensities)
		delete intensities;
}

/*************
 * DESCRIPTION:   precalculate some important variables
 * INPUT:         xres  horizontal render resolution
 * OUTPUT:        none
 *************/
void STAR::Update(int xres)
{
	// define frequently used values for acceleration
	acc_sr = xres*starradius*0.5f;
	acc_ihr = haloradius*innerhaloradius;
	acc_starcolor.r = (1.f - starcolor.r)*25.5f;
	acc_starcolor.g = (1.f - starcolor.g)*25.5f;
	acc_starcolor.b = (1.f - starcolor.b)*25.5f;
}

int clip(float a)
{
	 if (a >= 255.0f)
		 return 255;
	 return (int)floor(a);
}

// This random number generator really sucks
// but I couldn't find the other code I had...
#define rnd(i) (((i*37621)%2000)/2000.0f)

// Return bandlimited noise. Each integer value has a different
// random number. Values in-between use linear interpolation.
float Noise1D(float n)
{
	 int i;
	 float f;

	 i = (int)floor(n);
	 f = n - i;

	 return rnd(i + 1)*f + rnd(i)*(1.f - f);
}

/*************
 * DESCRIPTION:   draw star
 * INPUT:         line  address of current screen line
 *                col   color of flare (usually color of light source)
 *                sx    x coord. of center
 *                x,y   coord of pixel (relative to center of star !)
 * OUTPUT:        non
 *************/
void STAR::Draw(SMALL_COLOR *line, COLOR *col, int sx, int x, int y)
{
	float d, d2, intensity;
	float star;
	float Rf,Gf,Bf,redf,n, noiselevel;
	float R, G, B, r, dx;
	int h;

	intensity = 1.0f*255; // for future use
	noiselevel = 0.1f*noise;

	// Calc distance squared and distance, then
	// add d + d^2 to get the falloff function
	d2 = sqrt(x*x + y*y);
	d = ((d2 + d2*d2)*0.02f) / acc_sr;

	// The "Central Glow" falloff function is modified by the
	// "Red Outer Glow" in the inner part of the flare.
	redf = 1.0f/(1.0f + d*STAR_REDGLOWFALLOFF);
	Rf = STAR_FALLOFF + acc_starcolor.r*redf;
	Gf = STAR_FALLOFF + acc_starcolor.g*redf;
	Bf = STAR_FALLOFF + acc_starcolor.b*redf;
	R = 1.0f/(1.f + d*Rf);
	G = 1.0f/(1.f + d*Gf);
	B = 1.0f/(1.f + d*Bf);

	//R=G=B=0.0;
	if (flags & STAR_HALOENABLE)
	{
		// The red "Central Ring"
		if (d > acc_ihr && d < haloradius)
		{
			r = 2.0f*(d - acc_ihr)/(haloradius - acc_ihr);
			if (r > 1.0f)
				r = 2.0f - r;
			r = r*r*(3.f - 2.f*r);
			R += ringcolor.r*r;
			G += ringcolor.g*r;
			B += ringcolor.b*r;
		}
	}

	if (flags & STAR_ENABLE)
	{
		// Random streaks
		// Angle around flare in the range 0 to 2
#ifdef  __STORM__
		star = -newatan2(y, x);
#else
		star = -atan2(y, x);
#endif
		star -= tilt;
		if (star < 0.0f)
			star += 2.f*PI;
		h = (int)(((spikes*star)*INV_TWOPI) + 0.5f) % spikes;
		if (!(spikes % 2))
			star += PI/spikes;
		star = star*INV_PI;
		// Random noise with selectable frequency
		if (noise <= 1.0f)
			n = 1.0f;
		else
			n = Noise1D(star*noise);

		d *= 2.f/intensities[h];
		// Multiply by number of streaks and add in some noise
		star = star*spikes + n*noiselevel;
		// Get range 0 to 1 by reversing the range 1 to 2
		star = fmod(star, 2.0f);
		if (star >= 1.0f)
			star = 2.0f - star;
		// To get more sharply defined streaks, we square them
		// a couple of times
		star = star*star; star = star*star; //star = star*star;
		// Streaks are a maximum value divided by the falloff function,
		// but some noise is also multiplied in to lower the intensity
		// of some of them.
		star = star*(brightness*0.01f)/(1.f + d*2.0f)*n*n;
		if (star > 0.0f) // Until we fix the "negative" bug...
		{
			R += star;
			G += star;
			B += star;
		}
	}
	sx += x;

	dx = d2/acc_sr;
	if (dx < 1.0f)
	{
		dx *= dx;
		R = -R*dx + R;
		G = -G*dx + G;
		B = -B*dx + B;
	}
	else
		R = G = B = 0.0f;

	// Calculate and set RGB
	line[sx].r = clip(line[sx].r + intensity*R);
	line[sx].g = clip(line[sx].g + intensity*G);
	line[sx].b = clip(line[sx].b + intensity*B);
}
