/***************
 * MODUL:         Hypertexture
 * NAME:          htexture.h
 * DESCRIPTION:   Here is the class definition for hypertextures
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.03.97 ah    initial release
 ***************/

#ifndef HTEXTURE_H
#define HTEXTURE_H

class HYPERTEXTURE;

#ifndef LIST_H
#include "list.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef NOISE_H
#include "noise.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

class HYPERTEXTURE : public SLIST
{
	public:
		VECTOR pos;
		VECTOR orient_x;
		VECTOR orient_y;
		VECTOR orient_z;
		VECTOR size;
		float p1,p2,p3;
		ACTOR *actor;
		float time;

		HYPERTEXTURE();
		float ApplyTextures(RSICONTEXT *rc, const VECTOR*, OBJECT*, COLOR*, float);
		void Update(const float);
		virtual float Density(RSICONTEXT*, const VECTOR*, float) = 0;
		virtual void Color(float, COLOR*) = 0;
};

class EXPLOSION : public HYPERTEXTURE
{
	public:
		EXPLOSION();
		float Density(RSICONTEXT*, const VECTOR *, float);
		void Color(float, COLOR*);
};

#endif /* TEXTURE_H */

