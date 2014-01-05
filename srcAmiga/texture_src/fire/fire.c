/***************
 * MODUL:         Fire
 * NAME:          fire.cpp
 * DESCRIPTION:   Fire texture (from Texture & Modeling page 279)
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    04.12.96 ah    initial release
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
extern "C" void CleanupModules();
extern "C" void InitModules();
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
	"Fire V1.0",
	"Chaos Scale", "Chaos Offset", "Octaves",
	"Flameheight", "Flameamplitude",
	"Bottom Red", "Bottom Green", "Bottom Blue",
	"Middle Red", "Middle Green", "Middle Blue",
	"Top Red", "Top Green", "Top Blue",
	"Transparent Top",""
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
	 0, 0, 1,
	 2, 4, 1,
	 2, 4, 1,
	 2, 4, 0, 0,
};

// fparams[] is an array of floats that are the initial defaults
// in the texture requester.  The values in this array can be
// modified by the user and are passed to the texture code by
// the floating point version of Imagine's renderer.

static float fparams[16] = {
	 1.0, 0.0, 3.0, 1.0,
	 0.5, 255.0, 255.0, 255.0,
	 255.0, 255.0, 0.0, 255.0,
	 0.0, 0.0, 1.0, 0.0,
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

	SysBase = (*((struct ExecBase **) 4));
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
#endif // __STORM__
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
	VECTOR p;
	float chaos, val;

	p.x = v->x / t[12];
	p.y = v->y / t[13];
	p.z = exp(v->z / t[14]);

	chaos = fBm(&p, 1.f, 2.f, (int)params[2], RTable, hashTable);

	chaos = fabs(params[0]*chaos + params[1]);

	val = 0.85f*chaos + 0.8f*(params[4] - params[3]*(p.z - params[3]));

	if(val < 0.f)
	{
		// top color below 0
		pt->ptc_col[0] += params[11]*3.921568627e-3f;
		pt->ptc_col[1] += params[12]*3.921568627e-3f;
		pt->ptc_col[2] += params[13]*3.921568627e-3f;

		if(params[14] != 0.f)
		{
			// and fully transparent
			pt->ptc_tra[0] += 1.f;
			pt->ptc_tra[1] += 1.f;
			pt->ptc_tra[2] += 1.f;
		}
	}
	else
	{
		if(val > 1.f)
		{
			// bottom color above 1
			pt->ptc_col[0] += params[5]*3.921568627e-3f;
			pt->ptc_col[1] += params[6]*3.921568627e-3f;
			pt->ptc_col[2] += params[7]*3.921568627e-3f;
		}
		else
		{
			if(params[14] != 0.f)
			{
				pt->ptc_tra[0] += 1.f - val;
				pt->ptc_tra[1] += 1.f - val;
				pt->ptc_tra[2] += 1.f - val;
			}

			// top
			pt->ptc_col[0] += params[11]*(1.f - val)*3.921568627e-3f;
			pt->ptc_col[1] += params[12]*(1.f - val)*3.921568627e-3f;
			pt->ptc_col[2] += params[13]*(1.f - val)*3.921568627e-3f;

			// middle
			if(val > .5)
			{
				pt->ptc_col[0] += params[8]*(1.f - val)*2.f*3.921568627e-3f;
				pt->ptc_col[1] += params[9]*(1.f - val)*2.f*3.921568627e-3f;
				pt->ptc_col[2] += params[10]*(1.f - val)*2.f*3.921568627e-3f;
			}
			else
			{
				pt->ptc_col[0] += params[8]*val*2.f*3.921568627e-3f;
				pt->ptc_col[1] += params[9]*val*2.f*3.921568627e-3f;
				pt->ptc_col[2] += params[10]*val*2.f*3.921568627e-3f;
			}

			// bottom
			pt->ptc_col[0] += params[5]*val*3.921568627e-3f;
			pt->ptc_col[1] += params[6]*val*3.921568627e-3f;
			pt->ptc_col[2] += params[7]*val*3.921568627e-3f;
		}
	}
}

