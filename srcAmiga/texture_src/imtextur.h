/***************
 * MODUL:         imtexture
 * NAME:          imtexture.h
 * DESCRIPTION:   include file for all disk based textures
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    17.05.95 ah    initial release
 ***************/

#ifndef IMTEXTURE_H
#define IMTEXTURE_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

// constants to be used in infoflags[]
#define TXTF_RED    1
#define TXTF_GRN    2
#define TXTF_BLU    4
#define TXTF_SCL    8

// version number constant - if not correct, texture won't load
#ifdef __MAXON__
#define TXT_VERS    0x49545854   // Amiga version 'ITXT'
#else
#define TXT_VERS    0x54585449   // PC version 'TXTI'
#endif

/*********   DATA TYPES   *********/

#ifndef __AMIGA__
	typedef void *APTR;
	typedef float FLOAT;
#endif

// FRACT can be typedef'ed as a float without harm since this example is
// for the floating point version only.  See TDDD.DOC for an explanation
// of the FRACT data type.

typedef float               FRACT;

typedef struct _tform
{
	 VECTOR  r;      // texture axes position
	 VECTOR  a;      // texture's X axis alignment
	 VECTOR  b;      // texture's Y axis alignment
	 VECTOR  c;      // texture's Z axis alignment
	 VECTOR  s;      // length of each axis
}   TFORM;

// The PATCH structure contains all the (ray) hit point info.
// This structure is passed to a texture function each time a
// ray hits the object with a texture on it.

typedef struct _patch
{
	 VECTOR  ptc_pos;    // global hit point - read only
	 VECTOR  ptc_nor;    // surface normal (must be normalized)
	 FRACT   ptc_col[3]; // surface color (R,G,B) at hit point
	 FRACT   ptc_ref[3]; // surface color (R,G,B) at hit point
	 FRACT   ptc_tra[3]; // surface color (R,G,B) at hit point
	 FRACT   ptc_spc[3]; // surface color (R,G,B) at hit point - read only
	 UWORD   ptc_shp;    // copy of SHAP flags - read only - see TDDD.DOC
	 UWORD   ptc_shd;    // flag - obj can shadow itself - read only
	 FRACT   ptc_pc0;
	 FRACT   ptc_pc1;
	 VECTOR  *ptc_ray;   // position/exit direction of camera ray - read only
	 FRACT   raydist;
	 FRACT   foglen;     // surface foglength
}   PATCH;

// In the patch structure, ptc_ray[0] is the (camera) ray's base point
// position, and ptc_ray[1] is ray's (normalized) direction vector

// The ttable{} structure is used as a communication link between
// Imagine and the run-time loadable texture modules.  This structure is
// initialized and passed back to imagine by the entry module
// (texture_init()).

typedef struct ttable
{
	 LONG    id;             // version number identifier
	 void    (*init)();      // reserved - curently unused
	 void    (*cleanup)();   // reserved - curently unused
	 void    (*work)(FRACT *, PATCH *, VECTOR *, FRACT *);
									 // hook to the texture algorithm (the "guts")
	 UBYTE    **infotext;    // pointer to text fields for requester
	 UBYTE   *infoflags;     // pointer to data field flags for requester
	 APTR    params;         // pointer to data values for requester
	 APTR    tform;          // pointer to texture axis geometry info
} TTABLE;

#ifdef __cplusplus
extern "C"
{
#endif
BOOL InitNoise(float**, short**);
float Noise(VECTOR *, float*, short*);
void DNoise(VECTOR *v, VECTOR *r, float*, short*);
float Turbulence(VECTOR *, int, float*, short*);
float fBm(VECTOR *,float,float,int, float*, short*);
void VfBm(VECTOR *,float,float,int,VECTOR *, float*, short*);
void CleanupNoise(float*, short*);
#ifdef __cplusplus
};
#endif

#endif // IM_TEXTURE
