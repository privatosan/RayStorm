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

BOOL InitNoise(short**);
float Noise(VECTOR *, short*);
void DNoise(VECTOR *v, VECTOR *r, short*);
float VLNoise(VECTOR *v, short *hashTable);
float Turbulence(VECTOR *, int, short*);
float fBm(VECTOR *,float,float,int, short*);
void VfBm(VECTOR *,float,float,int,VECTOR *, short*);
float VLfBm(VECTOR *vec, float H, float lacunarity, int octaves, short *hashTable);
void CleanupNoise(short*);

#ifdef __cplusplus
}
#endif

#endif // NOISE
