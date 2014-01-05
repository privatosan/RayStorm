/***************
 * MODUL:         marble
 * NAME:          marble.cpp
 * DESCRIPTION:   Marble texture
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    23.07.95 ah    initial release
 ***************/

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef IMTEXTURE_H
#include "imtextur.h"
#endif

// PROTOTYPES

#ifdef __AMIGA__
struct ExecBase *SysBase;
extern "C" void InitModules();
extern "C" void CleanupModules();
TTABLE *texture_init(LONG);
#else
TTABLE *texture_init(int, int);
#endif
void fwork(FRACT *, PATCH *, VECTOR *, FRACT *);

// INITIALIZATIONS

// infotext[][] is an array of strings that contain the texture name and
// all the text fields that appear in the texture requester.  The
// texture name file (infotext[0]) is not currently used, but we
// suggest supplying a name it in case it eventually is used.

UBYTE *infotext[17] =
{
	"Marble V1.0",
	"Color Red", "Color Green", "Color Blue",
	"Reflect Red", "Reflect Green", "Reflect Blue",
	"Transpar Red", "Transpar Green", "Transpar Blue",
	"Octave", "", "",
	"", "", "", "",
};

// infoflags[] is an array of TXTF_ (texture flags) for the data
// fields of the texture requester.  The the TXTF flags are as
// follows:
//      Bit 0 - alter the red gun in the requester color chip
//      Bit 1 - alter the green gun in the requester color chip
//      Bit 2 - alter the blue gun in the requester color chip
//      Bit 3 - auto-scale this data field when object is scaled
//      Bits 4 thru 7 - reserved

UBYTE   infoflags[16] = {
	 1, 2, 4,
	 1, 2, 4,
	 1, 2, 4,
	 0, 0, 0,
	 0, 0, 0, 0,
};

// fparams[] is an array of floats that are the initial defaults
// in the texture requester.  The values in this array can be
// modified by the user and are passed to the texture code by
// the floating point version of Imagine's renderer.

static float fparams[16] = {
	 0.0, 0.0, 255.0, 0.0,
	 0.0, 0.0, 0.0, 0.0,
	 0.0, 7.0, 0.0, 0.0,
	 0.0, 0.0, 0.0, 0.0,
};

// ftform[] is an array of floats that represent the initial TFORM data
// (size and position) of the texture axis.  The values in this array can
// be modified by the user (via edit axis) and are passed to the texture
// code by the floating point version of Imagine's renderer.

static float ftform[15] = {
	 0.0, 0.0, 0.0,      // position vector
	 1.0, 0.0, 0.0,      // X axis alignment vector
	 0.0, 1.0, 0.0,      // Y axis alignment vector
	 0.0, 0.0, 1.0,      // Z axis alignment vector
	 32.0, 32.0, 32.0,   // size of each axis
};

// this ttable{} structure is defined earlier and initialized here.

TTABLE ttable = {
	 TXT_VERS,       /* version identifier - this contstant must be used */
	 NULL,
	 NULL,
	 NULL,
	 infotext,
	 infoflags,
};

void Cleanup();

float *RTable;
short *hashTable;

/*************
 * DESCRIPTION:   This is the texture_init() module for the textures.
 * INPUT:         arg0        flags
 * OUTPUT:        pointer to texture, NULL if non found
 *************/
#ifdef __AMIGA__
// Amiga version
TTABLE *texture_init(LONG arg0)
{
	WORD vers, fp;

#ifdef __STORM__
	InitModules();
#endif
	SysBase = (*((struct ExecBase **) 4));
	vers = arg0 >> 16;      // parse out the two arguments
	fp = arg0 & 0xffff;
#else
// PC version
TTABLE *texture_init(int vers, int fp)
{
#endif
	if(vers != 0x60 || !fp) // look for wrong version number and integer version
		return 0L;

	// called from the floating point version
	ttable.work = fwork;
	ttable.cleanup = Cleanup;
	ttable.params = (APTR)fparams;
	ttable.tform = (APTR)ftform;

	if(!InitNoise(&RTable, &hashTable))
		return 0L;

	return &ttable;
}

void Cleanup()
{
	CleanupNoise(RTable, hashTable);
#ifdef __STORM__
	CleanupModules();
#endif
}

/*************
 * DESCRIPTION:   this is the floating point work function
 * INPUT:         params   pointer to user definable numbers in requester
 *                pt       pointer to patch structure - defined above
 *                v        hit position  -  relative to texture axis
 *                t        info about texture axis (TFORM array - 15 floats)
 * OUTPUT:        pointer to texture, NULL if non found
 *************/
void fwork(FRACT *params, PATCH *pt, VECTOR *v, FRACT *t)
{
	float val;

	v->x /= t[12];
	v->y /= t[13];
	v->z /= t[14];
	val = 1.f - Turbulence(v,(int)params[9], RTable, hashTable);

	pt->ptc_col[0] *= val;
	pt->ptc_col[1] *= val;
	pt->ptc_col[2] *= val;
	pt->ptc_ref[0] *= val;
	pt->ptc_ref[1] *= val;
	pt->ptc_ref[2] *= val;
	pt->ptc_tra[0] *= val;
	pt->ptc_tra[1] *= val;
	pt->ptc_tra[2] *= val;
	val = (1.f - val) * 3.921568627e-3f;
	pt->ptc_col[0] += val * params[0];
	pt->ptc_col[1] += val * params[1];
	pt->ptc_col[2] += val * params[2];
	pt->ptc_ref[0] += val * params[3];
	pt->ptc_ref[1] += val * params[4];
	pt->ptc_ref[2] += val * params[5];
	pt->ptc_tra[0] += val * params[6];
	pt->ptc_tra[1] += val * params[7];
	pt->ptc_tra[2] += val * params[8];
}

