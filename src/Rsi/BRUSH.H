/***************
 * MODUL:         brush
 * NAME:          brush.h
 * DESCRIPTION:   This file includes the definition of the brush-class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.03.95 ah    initial release
 *    03.04.95 ah    changed WRAP flags; added IMAGE-class
 *    07.08.95 ah    added IMAGE()
 *    08.09.95 ah    added actor, BRUSH() and Update()
 *    09.09.95 ah    added 'const float' for ApplyBrush()
 *    01.11.95 ah    added ColorInterpolate()
 *    09.11.95 ah    added 'IMAGE::Load()'
 *    21.07.96 ah    changed to new V2 brush handling
 ***************/

#ifndef BRUSH_H
#define BRUSH_H

#include <stdio.h>

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

class SURFACE;

#ifndef ACTOR_H
#include "actor.h"
#endif

#ifndef PICLOAD_H
#include "picload.h"
#endif

class RSICONTEXT;

class IMAGE : public SLIST     // image
{
	public:
		char *name;             // image name
		SMALL_COLOR *colormap;  // image data
		UWORD width,height;     // image width and height

		IMAGE();
		~IMAGE();
		void Insert(RSICONTEXT*);
		int Load(RSICONTEXT*, char*, char*);
		int Save(RSICONTEXT*, char*, void*);
		float ImageIndex(int , int , float , float );
		void ColorInterpolate(int , int , float , float , COLOR *);
#ifdef __AMIGA__
	private:
		int LoadAsDatatype(char *);
#endif

};

IMAGE* FindImage(RSICONTEXT*, char*);

class BRUSH : public SLIST     // brush
{
	public:
		IMAGE *image;           // pointer to image
		ULONG flags;            // brush flags
		VECTOR pos;             // position of axis
		VECTOR align;           // alignment of axis
		VECTOR orient_x;        // orientation of x-vector
		VECTOR orient_y;        // orientation of y-vector
		VECTOR orient_z;        // orientation of z-vector
		VECTOR size;            // size of axis
		ACTOR *actor;           // pointer to actor
		float time;             // actual time

#ifdef __MAXON__
		void *operator new(size_t);
		void operator delete(void*);
#endif

		BRUSH();
		void ApplyBrush(OBJECT *, VECTOR *, SURFACE *, VECTOR *, const float);
	private:
		void Update(const float);
};

#define MAP_COLOR          0x0000
#define MAP_REFLECTIFITY   0x0001
#define MAP_FILTER         0x0002
#define MAP_ALTITUDE       0x0003
#define MAP_REFLECTION     0x0004
#define MAP_SPECULAR       0x0005

#define MAP_MASK           0x0007

#define SOFT         0x000100    // Soft color interpolation

#define WRAP_X       0x010000    // cylindrical mapping around x axis
#define WRAP_Y       0x020000    // cylindrical mapping around y axis
#define WRAP_XY      0x030000    // spherical mapping
#define WRAP_COORD   0x040000    // use mapping coords

#define WRAP_MASK    0x070000

#define REPEAT       0x080000
#define MIRROR       0x100000

#endif // BRUSH_H
