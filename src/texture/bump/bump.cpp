#include "stdafx.h"
#include "imdefs.h"
#include "txdefs.h"
#include "imtextur.h"
#include "noise.h"

#include <math.h>

#ifdef	__cplusplus
extern "C" {
#endif

LONG	(*pInit)();
void	(*pCleanup)();

char	szName[18] = "Bump";

BYTE	ubParamFlags[16] = {
	TXTF_SCL,
	TXTF_SCL,
	TXTF_SCL,
	0,0,0,0,
	TXTF_RED,	// TXTF_RED, etc., are ignored now.

	TXTF_GRN,
	TXTF_BLU,
	TXTF_RED,
	TXTF_GRN,
	TXTF_BLU,
	TXTF_RED,
	TXTF_GRN,
	TXTF_BLU,
};

float	fParams[16] = {
	1.0, 1.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0,
};

TFORM	tfAxes = {
	0.0, 0.0, 0.0,      // position vector
	1.0, 0.0, 0.0,      // X axis alignment vector
	0.0, 1.0, 0.0,      // Y axis alignment vector
	0.0, 0.0, 1.0,      // Z axis alignment vector
	32.0, 32.0, 32.0,   // size of each axis
};

void Work(float *params, PATCH *pt, VECTOR *v, FLOAT *t)
{
	VECTOR disp, *norm;
	float d;

	static int precalc = FALSE;
	static short *hashTable;

	if (!precalc)
	{
		// this could be done in an "Init()" routine
		InitNoise(&hashTable);
		precalc = TRUE;
	}
	
	v->x /= t[12];
	v->y /= t[13];
	v->z /= t[14];
	DNoise(v, &disp, hashTable);
	norm = (VECTOR*)&pt->ptc_nor;
	norm->x += disp.x * params[0];
	norm->y += disp.y * params[1];
	norm->z += disp.z * params[2];

	// Normalize
	d = sqrt(norm->x*norm->x + norm->y*norm->y + norm->z*norm->z);
	if (fabs(d) < EPSILON)
		return;
	d = 1.f/d;
	norm->x *= d;
	norm->y *= d;
	norm->z *= d;
}

#ifdef	__cplusplus
};
#endif
