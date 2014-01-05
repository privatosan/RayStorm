/***************
 * MODUL:         light
 * NAME:          light.h
 * DESCRIPTION:   light class definitions
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    15.08.95 ah    added radius
 *    27.08.95 ah    added actor
 *    10.09.95 ah    added time, Update() and UpdateLights()
 *    12.10.95 ah    changed spotlight 'dir' to 'lookp';
 *       added look point actor to spotlight
 *    28.11.96 ah    added TestVisibility()
 *    27.02.97 ah    added shadowmap
 *    18.08.98 ah    added directional light
 ***************/

#ifndef LIGHT_H
#define LIGHT_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef WORLD_H
#include "world.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

#ifndef ACTOR_H
#include "actor.h"
#endif

#ifndef FLARES_H
#include "flares.h"
#endif

#ifndef STAR_H
#include "star.h"
#endif

#define LIGHT_SHADOW    1
#define LIGHT_FLARES    2
#define LIGHT_STAR      4
#define LIGHT_SHADOWMAP 8

class LIGHT : public LIST
{
	public:
		VECTOR pos;       // position of light
		float r;          // radius of light
		float falloff;    // distance where the brightness of the light is 0
		COLOR color;      // color of light
		ULONG flags;      // shadows/flares
		ACTOR *actor;     // pointer to actor
		float time;       // actual time
		FLARE *flares;    // lens flares
		STAR  *star;      // visible light star
		float *shadowmap; // shadowmap
		int   shadowmap_size;   // x-size of shadowmap

		LIGHT();
		virtual ~LIGHT();
		void Insert(RSICONTEXT*);
		virtual BOOL DoLight(VECTOR *, COLOR *) = 0;
		virtual void Update(const float) = 0;
		virtual BOOL TestVisibility(RSICONTEXT*, RAY *) = 0;
		virtual void GetLightDirection(VECTOR*, VECTOR*);

		float InterpolateShadowMap(float, float, float);
};

class POINT_LIGHT : public LIGHT
{
	public:
		virtual BOOL DoLight(VECTOR *, COLOR *);
		virtual void Update(const float);
		virtual BOOL TestVisibility(RSICONTEXT*, RAY *);
};

class SPOT_LIGHT : public LIGHT
{
	public:
		VECTOR lookp;           // spot look point
		ACTOR *lookp_actor;     // pointer to look point actor
		float angle;            // opening angle
		float falloff_radius;   // light intensity is full at falloff_radius*angle and zero at angle

		SPOT_LIGHT();
		virtual BOOL DoLight(VECTOR *, COLOR *);
		virtual void Update(const float);
		virtual BOOL TestVisibility(RSICONTEXT*, RAY *);
};

class DIRECTIONAL_LIGHT : public LIGHT
{
	public:
		VECTOR dir;

		DIRECTIONAL_LIGHT();
		virtual BOOL DoLight(VECTOR *, COLOR *);
		virtual void Update(const float);
		virtual BOOL TestVisibility(RSICONTEXT*, RAY *);
		virtual void GetLightDirection(VECTOR*, VECTOR*);
};

void UpdateLights(RSICONTEXT*, const float);
void InitShadowMap(RSICONTEXT*);
BOOL TestIfShadowMaps(RSICONTEXT*);
void DoLighting(RSICONTEXT *, OBJECT *, SURFACE *, VECTOR *, VECTOR *, RAY *, VECTOR *,
					 COLOR *, COLOR *, float, SMALL_COLOR *, int *);
void ApplyLensEffects(RSICONTEXT*);
void UpdateLensEffects(RSICONTEXT*, int);
BOOL TestIfLensEffects(RSICONTEXT*);

#endif /* LIGHT_H */

