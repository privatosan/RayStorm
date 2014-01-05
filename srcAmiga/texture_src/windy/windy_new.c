/***************
 * MODUL:         windy
 * NAME:          windy.cpp
 * VERSION:       1.0 22.05.1995
 * DESCRIPTION:   Windy texture
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * BUGS:          none
 * TO DO:         all
 * HISTORY:       DATE		NAME	COMMENT
 *						22.05.95	ah		first release
 ***************/

#include "math.h"

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MATHDEFS_H
#include "mathdefs.h"
#endif

#ifndef IMTEXTURE_H
#include "imtextur.h"
#endif

// PROTOTYPES

#ifdef __AMIGA__
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
	"Windy V1.0",
	"Windscale",
	"Frequency",
	"Amplitude",
	"Offset",
	"Octaves","","",
	"", "", "", "",
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
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0, 0,
};

// fparams[] is an array of floats that are the initial defaults
// in the texture requester.  The values in this array can be
// modified by the user and are passed to the texture code by
// the floating point version of Imagine's renderer.

FLOAT   fparams[16] = {
    1.0, 0.1, 1.0, 0.3,
    7.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
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

/*************
 * FUNCTION:		texture_init
 * DESCRIPTION:	This is the texture_init() module for the textures.  This is the entry
 * module through which Imagine loads and calls the texture code.
 * texture_init() is called with a version number and a flag representing
 * whether this texture is being called from a floating point version or an
 * integer version of Imagine.  On the Amiga, these two (WORD) arguments
 * are packed into and passed as a single LONG which, in turn, is parsed
 * into the two arguments and interpreted within texture_init(). If all
 * goes well, texture_init() then initializes the ttable{} structure
 * (defined above) with the proper info and returns a pointer, for the
 * ttable{} structure, back to Imagine.
 * INPUT:			arg0			flags
 * OUTPUT:			pointer to texture, NULL if non found
 *************/
#ifdef __AMIGA__
// Amiga version
TTABLE *texture_init(LONG arg0)
{
	WORD vers, fp;
	
	vers = arg0 >> 16;      // parse out the two arguments
	fp = arg0 & 0xffff;
#else
// PC version
TTABLE *texture_init(int vers, int fp)
{
#endif
	if(vers != 0x60 || !fp)	// look for wrong version number and integer version
		return 0L;
	
	// called from the floating point version
	ttable.work = fwork;
	ttable.cleanup = CleanupNoise;
	ttable.params = (APTR)fparams;
	ttable.tform = (APTR)ftform;

	if(!InitNoise())
		return FALSE;

	return &ttable;
}

#ifdef __SASC
	void __stdargs _XCEXIT(long a) { }
#endif

/*************
 * FUNCTION:		fwork
 * DESCRIPTION:	this is the floating point work function
 * INPUT:			params	pointer to user definable numbers in requester
 *						pt			pointer to patch structure - defined above
 *						v			hit position  -  relative to texture axis
 *						t			info about texture axis (TFORM array - 15 floats)
 * OUTPUT:			pointer to texture, NULL if non found
 *************/
void fwork(FRACT *params, PATCH *pt, VECTOR *v, FRACT *t)
{
	float offset,turb,wind,f;
	VECTOR p,*norm;

	offset = (float)params[0] * fBm(v,1,2,(int)params[4]);
	p = *v;
	p.x *= params[1];
	p.y *= params[1];
	p.z *= params[1];
	turb = fBm(&p,.5,2,8);
	wind = (params[3] + params[2]*turb)*offset;

	norm = &pt->ptc_nor;
	norm->x += t[12] * wind;
	norm->y += t[13] * wind;
	norm->z += t[14] * wind;

	// Normalize
	f = sqrt(norm->x*norm->x + norm->y*norm->y + norm->z*norm->z);
	if (fabs(f) < EPSILON)
		return;
	f = 1/f;
	norm->x *= f;
	norm->y *= f;
	norm->z *= f;

/*	VECTOR spoint, tmp, res, *norm;
	float windfield, f, scalar;
	int octaves;

	spoint = *v;
	spoint.x *= params[0];
	spoint.y *= params[0];
	spoint.z *= params[0];
	if (params[1])
		windfield = params[1] * Turbulence(&spoint, 7);
	else
		windfield = 1.;
	
	DNoise(v, &tmp);
	res.x = params[2] * tmp.x;
	res.y = params[2] * tmp.y;
	res.z = params[2] * tmp.z;
	
	f = 1.;
	scalar = windfield;
	octaves = (int)params[3];
	if(octaves <= 1)
		octaves = 1;
	while(octaves--)
	{
		f *= params[4];
		spoint.x = f*v->x;
		spoint.y = f*v->y;
		spoint.z = f*v->z;
		DNoise(&spoint, &tmp);
		res.x += scalar*tmp.x;
		res.y += scalar*tmp.y;
		res.z += scalar*tmp.z;
		scalar *= params[5];
	}
	res.x *= windfield + params[6];
	res.y *= windfield + params[6];
	res.z *= windfield + params[6];

	norm = &pt->ptc_nor;
	norm->x += t[12] * res.x;
	norm->y += t[13] * res.y;
	norm->z += t[14] * res.z;

	// Normalize
	f = sqrt(norm->x*norm->x + norm->y*norm->y + norm->z*norm->z);
	if (fabs(f) < EPSILON)
		return;
	f = 1/f;
	norm->x *= f;
	norm->y *= f;
	norm->z *= f;*/
}

