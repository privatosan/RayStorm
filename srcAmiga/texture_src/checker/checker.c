/***************
 * MODUL:         checker
 * NAME:          checker.cpp
 * DESCRIPTION:   Checker Texture
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    16.05.95 ah    initial release
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
extern "C" void fwork(FRACT *, PATCH *, VECTOR *, FRACT *);

// INITIALIZATIONS

// infotext[][] is an array of strings that contain the texture name and
// all the text fields that appear in the texture requester.  The
// texture name file (infotext[0]) is not currently used, but we
// suggest supplying a name it in case it eventually is used.

char *infotext[17] =
{
	 "Checker V1.0",
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
	 "", "", "", ""
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
	 0, 0, 0, 0,
};

// fparams[] is an array of floats that are the initial defaults
// in the texture requester.  The values in this array can be
// modified by the user and are passed to the texture code by
// the floating point version of Imagine's renderer.

FLOAT   fparams[16] = {
	 0.f, 0.f, 0.f, 0.f,
	 0.f, 0.f, 0.f, 0.f,
	 0.f, 0.f, 0.f, 0.f,
	 0.f, 0.f, 0.f, 0.f
};

// ftform[] is an array of floats that represent the initial TFORM data
// (size and position) of the texture axis.  The values in this array can
// be modified by the user (via edit axis) and are passed to the texture
// code by the floating point version of Imagine's renderer.

FLOAT   ftform[15] = {
	 0.f, 0.f, 0.f,      // position vector
	 1.f, 0.f, 0.f,      // X axis alignment vector
	 0.f, 1.f, 0.f,      // Y axis alignment vector
	 0.f, 0.f, 1.f,      // Z axis alignment vector
	 32.f, 32.f, 32.f    // size of each axis
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
	ttable.cleanup = CleanupModules;
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
extern "C" void fwork(FRACT *params, PATCH *pt, VECTOR *v, FRACT *t)
{
	float x,y,z;

	x = v->x/t[12];
	y = v->y/t[13];
	z = v->z/t[14];

	if ((int)((floor(x) + floor(y) + floor(z))) & 1)
	{ // check for even numbers
		pt->ptc_col[0] = params[0]*3.921568627e-3f;
		pt->ptc_col[1] = params[1]*3.921568627e-3f;
		pt->ptc_col[2] = params[2]*3.921568627e-3f;
		pt->ptc_ref[0] = params[3]*3.921568627e-3f;
		pt->ptc_ref[1] = params[4]*3.921568627e-3f;
		pt->ptc_ref[2] = params[5]*3.921568627e-3f;
		pt->ptc_tra[0] = params[6]*3.921568627e-3f;
		pt->ptc_tra[1] = params[7]*3.921568627e-3f;
		pt->ptc_tra[2] = params[8]*3.921568627e-3f;
	}
}
