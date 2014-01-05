/*************
 * MODULE:      RAW-object format handler
 * NAME:        raw.h
 * PROJECT:     RayStorm
 * DESCRIPTION: definitions for raw.cpp
 * AUTHORS:     Mike Hesser, Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    13.03.96 mh    first release version
 *    11.02.97 ah    added TRIANGLE
 *************/

#ifndef RAW_H
#define RAW_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

typedef struct
{
	UWORD begin;            // point numbers of first
	UWORD end;              // and second edge point
} EDGE;

class TRIA : public LIST
{
	public:
		VECTOR points[3];
};

#endif
