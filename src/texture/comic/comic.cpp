/***************
 * MODUL:         comic
 * NAME:          comic.cpp
 * DESCRIPTION:   Comic texture
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		06.09.95	ah		initial release
 ***************/

#include "math.h"

#ifdef __GNUC__

#ifndef TYPEDEFS_H
#include "gnu:work/octree/typedefs.h"
#endif

#ifndef MATHDEFS_H
#include "gnu:work/octree/mathdefs.h"
#endif

#ifndef VECMATH_H
#include "gnu:work/octree/vecmath.h"
#endif

#ifndef IMTEXTURE_H
#include "gnu:work/octree/imtexture.h"
#endif

#else

#ifndef TYPEDEFS_H
#include "/typedefs.h"
#endif

#ifndef MATHDEFS_H
#include "/mathdefs.h"
#endif

#ifndef VECMATH_H
#include "/vecmath.h"
#endif

#ifndef IMTEXTURE_H
#include "/imtextur.h"
#endif

#endif

// PROTOTYPES

#ifdef __MAXON__
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
	"Comic V1.0",
	"",
	"",
	"",
	"", "", "",
	"", "", "",
	"", "", "",
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
    1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
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
#ifdef __MAXON__
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
	ttable.params = (APTR)fparams;
	ttable.tform = (APTR)ftform;

	return &ttable;
}

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
	VECTOR norm;
	float d;

	norm.x = pt->ptc_nor.x;
	norm.z = pt->ptc_nor.y;
	norm.y = pt->ptc_nor.z;
//	d = dotp(&pt->ptc_nor,&pt->ptc_ray[1]);
	d = fabs(dotp(&norm,&pt->ptc_ray[1]));
	if(d<.3)
	{
		pt->ptc_col[0] = 0;
		pt->ptc_col[1] = 0;
		pt->ptc_col[2] = 0;
	}
	else
	{
		if(d<.7)
		{
			pt->ptc_col[0] *= .5;
			pt->ptc_col[1] *= .5;
			pt->ptc_col[2] *= .5;
		}
	}
}

