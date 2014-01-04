/***************
 * MODUL:         param
 * NAME:          param.h
 * DESCRIPTION:   Definitions for parsing variable parameter lists
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    13.06.96 ah    initial release
 *    08.10.96 ah    added 'Orient'-parameters for brushes and textures
 *    26.11.96 ah    added rsiTStarInnerHaloRadius
 *    13.12.96 ah    added rsiTWorldMinObjects
 *    14.12.96 ah    added rsiTStarSpikeWidht
 *    10.01.97 ah    added parameters for box object
 *    12.01.97 ah    added parameters for csg object
 *    05.03.97 ah    added rsiTLightShadowMapSize
 *    07.03.97 ah    added rsiTMeshActor and rsiTMeshNoPhong
 *    18.03.97 ah    added rsiTSphereFlags, rsiTCSGFlags and rsiTBoxFlags
 *    19.03.97 ah    added parameters for cylinder object
 *    21.03.97 ah    added parameters for sor object
 ***************/

#ifndef PARAM_H
#define PARAM_H

#include <stdarg.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

typedef struct
{
	int tag_id;             // ID of tag
	int type;               // Type of parameter
	void *data;             // Pointer to parameter data
} PARAM;

enum
{
	rsiTypeInt,
	rsiTypeFloat,
	rsiTypePointer
};

BOOL GetParams(va_list *, PARAM *);

#endif /* PARAM_H */
