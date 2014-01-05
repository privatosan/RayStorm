/***************
 * MODULE:        star.h
 * PROJECT:       Scenario
 * DESCRIPTION:   definition file for star.cpp
 * AUTHORS:       Mike Hesser & Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.10.96 mh    initial release
 *    19.11.96 ah    added Write() and ParseStar()
 *    27.11.96 ah    added innerhaloradius
 *    14.12.96 ah    added spikewidth
 *    18.12.96 ah    added GetSize()
 ***************/

#ifndef STAR_H
#define STAR_H

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#define STAR_ENABLE     1
#define STAR_HALOENABLE 2
#define STAR_RANDOM     4

class STAR
{
public:
	COLOR starcolor, ringcolor;
	int flags;
	float starradius;
	int spikes;
	float noise;
	float haloradius;
	float innerhaloradius;
	float *intensities;
	float brightness;
	float range;
	float tilt;

	STAR();
	~STAR();
	void SetSpikes(int);
	void Draw(DISPLAY *, int, int, int);
	STAR *Duplicate();
	rsiResult ToRSI(rsiCONTEXT*, void *);
	BOOL Write(struct IFFHandle*);
	int GetSize();
};

BOOL ParseStar(struct IFFHandle*, LIGHT *);

#endif
