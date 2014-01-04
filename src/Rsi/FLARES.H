/***************
 * MODULE:     flares.h
 * PROJECT:    RayStorm
 * DESCRIPTION:   
 * AUTHORS:    Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    13.04.96 mh    initial release
 ***************/

#ifndef FLARES_H
#define FLARES_H

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

enum
{
	FLARE_DISC,
	FLARE_POLY
};

enum
{
	FLARE_PLAIN,
	FLARE_EXP1,
	FLARE_RADIAL,
	FLARE_EXP2,
	FLARE_EXP3,
	FLARE_RAINBOW
};

class FLARE : public SLIST
{
public:
	int   type;
	int   func;
	float radius;
	float pos;
	COLOR color;
	int   edges;
	float tilt;
	float sr, phi, sinphi;

public:
	FLARE();
	void Update(int);
	void Draw(SMALL_COLOR *, int, int, int);
	void Free();
};

void FreeFlares(FLARE*);

#endif
