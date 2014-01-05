/***************
 * MODUL:         Radial
 * NAME:          Radial.cpp
 * DESCRIPTION:   Radial Texture
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    28.11.95 ah    initial release
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

char *infotext[17] =
{
	"Radial V1.0",
	"Start radius",
	"End radius",
	"Color Red",
	"Color Green",
	"Color Blue",
	"Reflect Red",
	"Reflect Green",
	"Reflect Blue",
	"Filter Red",
	"Filter Green",
	"Filter Blue",
	"", "", "",
	"", ""
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
	0,
	0,
	TXTF_RED,
	TXTF_GRN,
	TXTF_BLU,
	TXTF_RED,
	TXTF_GRN,
	TXTF_BLU,
	TXTF_RED,
	TXTF_GRN,
	TXTF_BLU,
	0, 0, 0,
	0, 0
};

// fparams[] is an array of floats that are the initial defaults
// in the texture requester.  The values in this array can be
// modified by the user and are passed to the texture code by
// the floating point version of Imagine's renderer.

FLOAT   fparams[16] = {
	 0.0, 1.0, 0.0, 0.0,
	 0.0, 0.0, 0.0, 0.0,
	 0.0, 0.0, 0.0, 0.0,
	 0.0, 0.0, 0.0, 0.0
};

// ftform[] is an array of floats that represent the initial TFORM data
// (size and position) of the texture axis.  The values in this array can
// be modified by the user (via edit axis) and are passed to the texture
// code by the floating point version of Imagine's renderer.

FLOAT   ftform[15] = {
	 0.0, 0.0, 0.0,      // position vector
	 1.0, 0.0, 0.0,      // X axis alignment vector
	 0.0, 1.0, 0.0,      // Y axis alignment vector
	 0.0, 0.0, 1.0,      // Z axis alignment vector
	 32.0, 32.0, 32.0    // size of each axis
};

// this ttable{} structure is defined earlier and initialized here.

TTABLE ttable = {
	 TXT_VERS,       /* version identifier - this contstant must be used */
	 NULL,
	 NULL,
	 NULL,
	 (UBYTE **)infotext,
	 infoflags,
};

/*************
 * FUNCTION:      texture_init
 * DESCRIPTION:   This is the texture_init() module for the textures.  This is the entry
 * module through which Imagine loads and calls the texture code.
 * texture_init() is called with a version number and a flag representing
 * whether this texture is being called from a floating point version or an
 * integer version of Imagine.  On the Amiga, these two (WORD) arguments
 * are packed into and passed as a single LONG which, in turn, is parsed
 * into the two arguments and interpreted within texture_init(). If all
 * goes well, texture_init() then initializes the ttable{} structure
 * (defined above) with the proper info and returns a pointer, for the
 * ttable{} structure, back to Imagine.
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
#ifdef __STORM__
	ttable.cleanup = CleanupModules;
#endif
	ttable.params = (APTR)fparams;
	ttable.tform = (APTR)ftform;

	return &ttable;
}

#ifdef __STORM__
extern "C" void exit() {}
#endif

#ifdef __SASC
void __stdargs _XCEXIT(long a) { }
#endif

/*************
 * FUNCTION:      fwork
 * DESCRIPTION:   this is the floating point work function
 * INPUT:         params   pointer to user definable numbers in requester
 *                pt       pointer to patch structure - defined above
 *                v        hit position  -  relative to texture axis
 *                t        info about texture axis (TFORM array - 15 floats)
 * OUTPUT:        pointer to texture, NULL if non found
 *************/
void fwork(FRACT *params, PATCH *pt, VECTOR *v, FRACT *t)
{
	float d;

	d = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);

	if(d<=params[0])
		return;
	if(d>=params[1])
	{
		pt->ptc_col[0] = params[2]*3.921568627e-3;
		pt->ptc_col[1] = params[3]*3.921568627e-3;
		pt->ptc_col[2] = params[4]*3.921568627e-3;
		pt->ptc_ref[0] = params[5]*3.921568627e-3;
		pt->ptc_ref[1] = params[6]*3.921568627e-3;
		pt->ptc_ref[2] = params[7]*3.921568627e-3;
		pt->ptc_tra[0] = params[8]*3.921568627e-3;
		pt->ptc_tra[1] = params[9]*3.921568627e-3;
		pt->ptc_tra[2] = params[10]*3.921568627e-3;
		return;
	}

	d = 1. - (d - params[0])/(params[1] - params[0]);

	pt->ptc_col[0] *= d;
	pt->ptc_col[1] *= d;
	pt->ptc_col[2] *= d;
	pt->ptc_ref[0] *= d;
	pt->ptc_ref[1] *= d;
	pt->ptc_ref[2] *= d;
	pt->ptc_tra[0] *= d;
	pt->ptc_tra[1] *= d;
	pt->ptc_tra[2] *= d;
	d = (1-d) * 3.921568627e-3;
	pt->ptc_col[0] += d * params[2];
	pt->ptc_col[1] += d * params[3];
	pt->ptc_col[2] += d * params[4];
	pt->ptc_ref[0] += d * params[5];
	pt->ptc_ref[1] += d * params[6];
	pt->ptc_ref[2] += d * params[7];
	pt->ptc_tra[0] += d * params[8];
	pt->ptc_tra[1] += d * params[9];
	pt->ptc_tra[2] += d * params[10];
}

