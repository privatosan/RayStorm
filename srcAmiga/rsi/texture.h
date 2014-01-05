/***************
 * MODUL:         texture
 * NAME:          texture.h
 * DESCRIPTION:   This file includes the texture class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    23.02.95 ah    initial release
 *    15.03.95 ah    texture is now a list
 *    08.04.95 ah    added texture orientation and lenght
 *    11.04.95 ah    added linear texture
 *    19.04.95 ah    added Imagine texture definitions
 *          (only AMIGA)
 *    17.05.95 ah    removed checker and linear
 *    08.09.95 ah    added actor and Update()
 *    21.06.97 ah    changed to new V2 texture handling
 ***************/

#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef __AMIGA__
	#ifndef DOS_DOS_H
	#include <dos/dos.h>
	#endif
#else
	#include "windef.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

#ifndef WORLD_H
#include "world.h"
#endif

#ifndef ACTOR_H
#include "actor.h"
#endif

#ifndef TEXTURE_MOD_H
#ifdef __STORM__
#include "rtxt:rtexture.h"
#else
#include "rtexture.h"
#endif
#endif // TEXTURE_MOD_H

// The PATCH structure contains all the (ray) hit point info.
// This structure is passed to a texture function each time a
// ray hits the object with a texture on it.
typedef struct
{
	VECTOR   ptc_pos;    // global hit point - read only
	VECTOR   ptc_nor;    // surface normal (must be normalized)
	COLOR    ptc_col;    // surface color (R,G,B) at hit point
	COLOR    ptc_ref;    // surface color (R,G,B) at hit point
	COLOR    ptc_tra;    // surface color (R,G,B) at hit point
	COLOR    ptc_spc;    // surface color (R,G,B) at hit point - read only
	UWORD    ptc_shp;    // copy of SHAP flags - read only - see TDDD.DOC
	UWORD    ptc_shd;    // flag - obj can shadow itself - read only
	float    ptc_pc0;
	float    ptc_pc1;
	VECTOR   *ptc_ray;   // position/exit direction of camera ray - read only
	float    raydist;
	float    foglen;     // surface foglength
} IM_PATCH;

// The IM_TTABLE{} structure is used as a communication link between
// Imagine and the run-time loadable texture modules.  This structure is
// initialized and passed back to imagine by the entry module
// (texture_init()).
typedef struct
{
	LONG     id;            // version number identifier
	void     (*init)();     // reserved - curently unused
	void     (*cleanup)();  // reserved - curently unused
#ifdef __AMIGA__
	void  (*work)(float*, IM_PATCH*, VECTOR*, float*);
#else
#ifdef __WATCOMC__
		void __cdecl (*work)(float*, IM_PATCH*, VECTOR*, float*);
#else
		void (*work)(float*, IM_PATCH*, VECTOR*, float*);
#endif
#endif                        // hook to the texture algorithm (the "guts")
	BYTE     **infotext;    // pointer to text fields for requester
	UBYTE    *infoflags;    // pointer to data field flags for requester
	APTR     params;        // pointer to data values for requester
	APTR     tform;         // pointer to texture axis geometry info
} IM_TTABLE;

class IM_TEXT : public LIST
{
	public:
		char     *name;      // texture name
#ifdef __AMIGA__
		BPTR     seglist;    // segment list of Imagine texture file
#else
		HINSTANCE hInst;
#endif /* __AMIGA__ */
		IM_TTABLE *ttable;   // pointer to ttable-structure

		IM_TEXT();
		~IM_TEXT();
		void Insert(RSICONTEXT*);
		IM_TEXT* FindIMTexture(char *);
};

class TEXTURE : public LIST
{
	public:
		VECTOR pos;          // Position of texture
		VECTOR orient_x;     // orientation of x-vector
		VECTOR orient_y;     // orientation of y-vector
		VECTOR orient_z;     // orientation of z-vector
		VECTOR size;         // size of texture
		ACTOR *actor;        // pointer to actor
		float time;          // current time

		TEXTURE();
		virtual ~TEXTURE() { };
		void Update(const float);
		virtual void Apply(VECTOR *, SURFACE *, const VECTOR *, RAY *) = 0;
		virtual int Load(RSICONTEXT*, char*, char*) = 0;
		virtual TEXTURE *Duplicate() = 0;
};

class IMAGINE_TEXTURE : public TEXTURE
{
	public:
		IM_TEXT *texture;    // pointer to texture
		float param[16];     // texture parameters

		IMAGINE_TEXTURE();
		int Load(RSICONTEXT*, char*, char*);
		void Apply(VECTOR *, SURFACE *, const VECTOR *, RAY *);
		TEXTURE *Duplicate();
};

class RAYSTORM_TEXTURE : public TEXTURE
{
	public:
		void *data;
#ifdef __AMIGA__
		struct Library *Base;
#endif // __AMIGA__
		TEXTURE_INFO *tinfo;

		RAYSTORM_TEXTURE();
		~RAYSTORM_TEXTURE();
		int Load(RSICONTEXT*, char*, char*);
		void Apply(VECTOR *, SURFACE *, const VECTOR *, RAY *);
		TEXTURE *Duplicate();
};

#endif /* TEXTURE_H */
