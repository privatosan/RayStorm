/***************
 * MODUL:         world
 * NAME:          world.h
 * DESCRIPTION:   Global definition class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    15.02.95 ah    initial release
 *    08.04.95 ah    added quickrender option
 *    03.05.95 ah    added samples, filterwidth and depth
 *    08.08.95 ah    added ~WORLD()
 *    16.08.95 ah    added backdrop
 *    21.08.95 ah    added global fog
 *    27.08.95 ah    added time
 *    30.08.95 ah    added MOTIONBLUR
 *    01.09.95 ah    added distrib_samples
 *    02.09.95 ah    added WORLD::UnitCirclePointDistrib()
 *    28.09.95 ah    added gloabl reflection map
 *    02.11.95 ah    added DoGlobalFog()
 *    27.11.96 ah    added Cooperate(), CheckCancel(), Log() and UpdateStatus()
 *    13.12.96 ah    added minobjects
 ***************/

#ifndef WORLD_H
#define WORLD_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef INTERSECTION_H
#include "intersec.h"
#endif

class IMAGE;

#define WORLD_QUICK           1     // quick render: no shadows, no reflections, no transparence
#define WORLD_RANDOM_JITTER   2     // use random jitter else use normal jitter
#define WORLD_DOF             4     // use depth of field

class WORLD
{
	public:
		COLOR back;       // background color
		COLOR ambient;    // ambient color
		COLOR fog;        // global fog color
		float foglength;  // global fog length
		float fogheight;  // highest fog y-coordinate
		int flags;        // flags: quickrender, random jitter, dof
		ULONG samples;    // sqrt of samples of one pixel (antialias)
		float filterwidth;   // gaussian filter width
		float sampledelta;   // filterwidth/samples
		ULONG motionblur_samples;  // number of samples for motion blur
		ULONG softshadow_samples;  // number of samples for soft shadows
		COLOR contrast;   // minimum contrast for adaptive supersampling
		ULONG depth;       // maximum depth of octree
		char *brushpath;  // brush path
		char *texturepath;   // texture path
		char *objectpath; // object path
		char *backdrop;   // name of backdrop picture
		IMAGE *reflmap;   // global reflection map
		float time;       // actual global time
		float timelength; // intervall length
		ULONG minobjects; // minimum amount of objects for octree
		float step;       // hyper texture step
		BOOL validkeyfile;   // TRUE for a valid keyfile

		void ToDefaults();
		BOOL SetBackdropPic(char *);
		BOOL DoGlobalFog(RAY *, SMALL_COLOR *, COLOR *);
		void UnitCirclePoint(VECTOR *, int );
		void UnitCirclePointSoftShadow(VECTOR *, int );
		~WORLD();
		WORLD();
};

extern void (*Cooperate)();
extern BOOL (*CheckCancel)();
extern void (*Log)(char *);
extern void (*UpdateStatus)(float, float, int, int, SMALL_COLOR*);

#define UPDATE_INTERVAL .5    // time in seconds after the UpdateStatus() function is called

#endif /* WORLD_H */

