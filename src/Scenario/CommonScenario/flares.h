/***************
 * MODULE:        flares.h
 * PROJECT:       Scenario
 * DESCRIPTION:   definition file for flares.cpp
 * AUTHORS:       Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.10.96 mh    initial release
 *    19.11.96 ah    added Write() and ParseFlare()
 *    18.12.96 ah    added GetSize()
 ***************/

#ifndef FLARES_H
#define FLARES_H

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

enum
{
	FLARE_DISC,
	FLARE_POLY
};

enum
{
	FLARE_FUNC_PLAIN,
	FLARE_FUNC_EXP1,
	FLARE_FUNC_RADIAL,
	FLARE_FUNC_EXP2,
	FLARE_FUNC_EXP3
};

class FLARE : public SLIST
{
public:
	int type;
	int func;
	float pos;
	float radius;
	int edges;
	float tilt;

	COLOR color;
	BOOL selected;

	FLARE();
	void Draw(DISPLAY *, RECTANGLE *, int, int, BOOL);
	FLARE *Duplicate();
	rsiResult ToRSI(rsiCONTEXT*, void *);
	BOOL Write(struct IFFHandle *);
	int GetSize();
};

BOOL ParseFlare(struct IFFHandle *, LIGHT *);
int SelectedFlare(FLARE *, int, int, int, int, int, int);

#endif
