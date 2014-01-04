/***************
 * MODUL:         noise.lib
 * NAME:          noise.h
 * DESCRIPTION:   include file for all noise functions
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    25.07.97 ah    initial release
 ***************/

#ifndef NOISE_H
#define NOISE_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

BOOL InitNoise(float**, short**);
float Noise(VECTOR *, float*, short*);
void DNoise(VECTOR *v, VECTOR *r, float*, short*);
float Turbulence(VECTOR *, int, float*, short*);
float fBm(VECTOR *,float,float,int, float*, short*);
void VfBm(VECTOR *,float,float,int,VECTOR *, float*, short*);
void CleanupNoise(float*, short*);

#ifdef __cplusplus
}
#endif

#endif // NOISE
