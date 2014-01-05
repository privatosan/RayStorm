/***************
 * MODUL:         color
 * NAME:          color.h
 * DESCRIPTION:   Definitions and structures for colors
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * BUGS:          none
 * TO DO:         all
 * HISTORY:       DATE     NAME  COMMENT
 *                11.02.95 ah    initial release
 *                22.03.95 ah    added SMALL_COLOR structure
 *                19.04.95 ah    added ColorMultAdd() macro
 *                10.05.95 ah    added sampled flag to SMALL_COLOR
 *                11.03.97 mh    renamed sampled to info, INFO_ constants
 ***************/

#ifndef COLOR_H
#define COLOR_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

typedef struct
{
	float r, g, b;
} COLOR;

typedef struct
{
	UBYTE r, g, b;    // pixel color
	UBYTE info;       // information about z-depth, supersampling, glow...
} SMALL_COLOR;

#define INFO_SAMPLED       0x80
#define INFO_SAMPLED_MB    0x40
#define INFO_DEPTH         7

#define ColorScale(s,c,a)     (a)->r = (s)*(c)->r, \
					(a)->g = (s)*(c)->g, \
					(a)->b = (s)*(c)->b

#define ColorAddScaled(c,s,x,y) (c)->r += (x)->r*(s)*(y)->r, \
					(c)->g += (x)->g*(s)*(y)->g, \
					(c)->b += (x)->b*(s)*(y)->b

#define ColorMultAdd(c,s,a) (c)->r += (s)*(a)->r, \
					(c)->g += (s)*(a)->g, \
					(c)->b += (s)*(a)->b

#define ColorMultiply(x,y,a)     (a)->r = (x).r*(y).r, \
					(a)->g = (x).g*(y).g, \
					(a)->b = (x).b*(y).b

#define ColorAdd(x,y,a)       (a)->r = (x).r+(y).r, \
					(a)->g = (x).g+(y).g, \
					(a)->b = (x).b+(y).b

#endif /* COLOR_H */
