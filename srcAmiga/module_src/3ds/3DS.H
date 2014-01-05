/***************
 * MODULE:      3DS-object format handler
 * NAME:        3ds.h
 * DESCRIPTION: This module keeps function to read 3ds-files
 * AUTHORS:     Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.11.95  mh      initial version
 ***************/

#ifndef DDDS_H
#define DDDS_H

#ifndef TYPEDEFS_H
	#include "typedefs.h"
#endif

typedef struct
{
	UWORD p1;
	UWORD p2;
	UWORD p3;
	UWORD unknown;
} FACE3DS;

typedef struct
{
	UWORD id;
	ULONG end;
} CHUNK;

typedef FILE DDDSHandle;

// chunk ids
#define ID_PRIMARY      (0x4D4D)
#define ID_MESHBLOCK    (0x3D3D)
#define ID_PROPMATENTRY (0xAFFF)
#define ID_PROPNAME     (0xA000)
#define ID_AMBIENT      (0xA010)
#define ID_DIFFUSE      (0xA020)
#define ID_SPECULAR     (0xA030)
#define ID_SHININESS    (0xA040)
#define ID_TRANSPARENCY (0xA050)
#define ID_COLOR1       (0x0010)
#define ID_COLOR255     (0x0011)
#define ID_PERCENT100   (0x0030)
#define ID_PERCENT1     (0x0031)
#define ID_OBJECTDESC   (0x4000)
#define ID_TRIANGLE     (0x4100)
#define ID_POINTS       (0x4110)
#define ID_FACES        (0x4120)
#define ID_MSHMATGROUP  (0x4130)
#define ID_MAPPINGCOORS (0x4140)
#define ID_SMOOTHGROUP  (0x4150)
#define ID_TRANSMATRIX  (0x4160)

#endif
