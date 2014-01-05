/***************
 * MODUL:         intersec
 * NAME:          intersec.h
 * DESCRIPTION:   defines RAY-structure
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE		NAME	COMMENT
 *		11.02.95	ah		first release
 *		03.05.95	ah		added depth
 *		18.07.95	ah		added sample
 *		01.09.95	ah		added time
 **************/

#ifndef INTERSECTION_H
#define INTERSECTION_H

class OBJECT;

typedef struct
{
	VECTOR start, dir;		// start (startpoint) and ray (direction)
	float lambda;				// start+lambda*ray (hitpoint)
	int depth;					// current ray depth
	int sample;					// current sample #
	float time;					// current time of ray
	OBJECT *obj;				// hited object
	BOOL enter;
} RAY;

#define MAXRAYDEPTH 6

#endif // INTERSECTION_H
