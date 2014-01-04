/***************
 * MODULE:        star.h
 * PROJECT:       RayStorm
 * DESCRIPTION:   definition file for star
 * AUTHORS:       Mike Hesser & Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    13.04.96 mh    initial release
 *    14.11.96 mh    added destructor
 *    17.11.96 mh    added private members hr,sr,tg
 *    27.11.96 ah    added ihr and innerhaloradius
 *    14.12.96 ah    added spikewidth
 ***************/

#ifndef STAR_H
#define STAR_H

#ifndef COLOR_H
#include "color.h"
#endif

#define STAR_ENABLE     1
#define STAR_HALOENABLE 2

class STAR
{
public:
	COLOR starcolor, ringcolor;
	int   flags;
	float starradius;
	int   spikes;
	float haloradius, innerhaloradius;
	float *intensities;
	float brightness;
	float tilt;
	float noise;
	float acc_sr;

private:
	float acc_ihr;
	COLOR acc_starcolor;

public:
	STAR();
	~STAR();
	void Update(int);
	void Draw(SMALL_COLOR *, COLOR *, int, int, int);
};

#endif
