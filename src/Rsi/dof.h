/***************
* MODULE:		DoF.h
* PROJECT:		DoF class definition file
* DESCRIPTION:	Header-File
* AUTHORS:		Mike Hesser
***************/

#ifndef DOF_H
#define DOF_H

#include "typedefs.h"
#include "color.h"

class RSICONTEXT;

// error numbers
enum tDoFErr
{
	errOk = 0,
	errMemory,
	errOpenFileWrite,
	errWriteFile,
	errNoPicAvailable,
	errPicSize,
	errFieldOfView,
	errAperture,
	errFocalDistance,
	errSamples,
	errRenderField,
	errNoInit,
	errUnknown,
	_tErr
};

// commands to accelerate the raytracing process
enum tDoFAccelModes
{
	DoFNormal, DoFJump, DoFNoAntialias, DoFSampleBack
};

// flags to switch the acceleration options
enum tDoFFlags
{
	DoFHidden = 1,
	DoFAccelerate = 2,
};

typedef unsigned char uchar;

// information to accelerate calculation
typedef struct
{
	int action;
	int count;
} DOF_ACTION;

// we need floats to represent the rgb color values
typedef struct
{
	float r, g, b;
} PIXF;

typedef short DOF_PIXEL;
typedef float DOF_DEPTH;

// limit the amount of memory with segmentation
typedef struct
{
	PIXF *pLine;
	int fromLine, toLine; 
} DOF_SEGMENT;

// needed for spread table
typedef struct
{
	float x, y;
} DOF_APERTUREPOINT;

// one entry of the look-up-table
typedef struct
{
	float radius;
	int size;
	float intensity;
	float *pData;
} DOF_LUT_ENTRY;

// structure for Init() function
typedef struct
{
	int xres, yres;
	int left, top, right, bottom;
	float focaldist;
	float hfov, vfov;
	float aperture;
	float filterwidth;
	int samples;
	int flags;
} DOF_DATA;

class CDoF
{
	SMALL_COLOR *pSrc;				// pointer to the original picture data
	SMALL_COLOR *pDst;				// pointer to the resulting picture
	DOF_SEGMENT *aSegment;			// like pDst, but with floats
	DOF_PIXEL *CoCMap;				// table of LUT-values
	DOF_APERTUREPOINT *aAperture;	// aperture offsets
	DOF_LUT_ENTRY *aLUT;				// LU-table for CoCs (Circle of Confusion)
	float stepRadius;					// step value (used to calculate the LU-tables)
	float f, n;							// focal distance and aperture
	float Cmax;							// maximum diameter of a CoC
	float Clr;							// radius of the aperture
	int xres, yres;					// screen resolution
	int left, top, right, bottom; // render field
	int samples, samples2;			// number of samples for hidden area (..2 squared)
	float focaldist, hfd;			// distance of depth plane
	float hfov, vfov;					// horizontal and vertical field of fiew
	float scale;						// scale factor CoC <-> screen
	float Db;							// distance of image plane
	int cntAnti, cntJump;			// needed for acceleration
	bool bInit;							// Init() was called ?
	int flags;							// option flags
	int error;							// error number of the last error which occurred

public:
	CDoF();
	~CDoF();

	bool Init(DOF_DATA *);
	void Cleanup();
	bool CalculateDoF(RSICONTEXT *, void (*)(RSICONTEXT*, int, int, float, float, SMALL_COLOR*));
	void SetDepth(int, int, float, DOF_ACTION *);
	char *GetErrorMsg(int err = -1);

private:
	bool CalculateLUTs();
	void MarkHidden();
	bool Error(int);
};

#endif
