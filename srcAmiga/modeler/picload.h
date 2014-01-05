/***************
 * MODULE:        picload.h
 * PROJECT:       RayStorm
 * DESCRIPTION:   definition file for all picture loader modules
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		11.10.97	mh		initial release
 *		26.10.97	ah		made param a pointer to a structure to be more felxible with
 *			passed parameters. Also made extensions for thread-save modules.
 ***************/

#ifndef PICLOAD_H
#define PICLOAD_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

typedef struct
{
	char *name;             	// picture name
	SMALL_COLOR *colormap;  	// image data
	UWORD width,height;     	// image width and height
	void *caller;					// pointer to caller, this is passed back by the progress call back to identify
										// the caller for multi-threading environments
	void *param;					// additional parameter passed to loader module, structrues are defined below
										// (f.e. JPEG compression quality)
	void *(*malloc)(size_t);	// pointer to malloc
	void *(*Progress)(void*, float);	// pointer to progress routine (percent: 0 -> min, 1 -> max)
} PICTURE;

// parameter structures
typedef struct
{
	ULONG quality;					// JPEG compression quality
} JPEG_PARAM;

#endif