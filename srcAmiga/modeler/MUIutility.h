/***************
 * PROGRAM:       Modeler
 * NAME:          MUIutility.h
 * DESCRIPTION:   MUI utility functions
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    31.01.97 ah    added rectangle icon
 *    21.02.97 ah    added edit points icon
 *    01.05.97 ah    added add points icon
 ***************/

#ifndef MUIUTILITY_H
#define MUIUTILITY_H

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef MUIDEFS_H
#include "muidefs.h"
#endif

extern struct Image
	newimage,
	loadimage,
	saveimage,
	frontimage,
	rightimage,
	topimage,
	perspimage,
	cameraimage,
	fourimage,
	boundingimage,
	wireimage,
	solidimage,
	rotateimage,
	moveimage,
	scaleimage,
	worldmoveimage,
	worldrotateimage,
	worldscaleimage,
	ximage,
	yimage,
	zimage,
	viewimage,
	rectangleimage,
	editpointsimage,
	addpointsimage,
	editedgesimage,
	addedgesimage,
	editfacesimage,
	addfacesimage,
	localimage,
	tapefirstimage,
	tapeprevimage,
	tapestopimage,
	tapeplayimage,
	tapenextimage,
	tapelastimage;

ULONG DoSuperNew(struct IClass*,Object*,ULONG,...);
#ifdef __PPC__
ULONG DoSuperNewPPC(struct IClass*, Object*, ...);
#else
ULONG DoSuperNew(struct IClass*, Object*, ...);
#endif
LONG xget(Object *,ULONG);
void Float2String(const float,char*);
void SetColorField(Object *, COLOR *);
void GetColorField(Object *, COLOR *);
void ViewPicture(Object *, char *);
void NewFreeBitMap(struct BitMap *);
struct BitMap *NewAllocBitMap(int, int, int);

/*************
 * DESCRIPTION:   set application to sleep
 * INPUT:         state    sleep state
 * OUTPUT:        -
 *************/
inline void SetSleep(BOOL state)
{
	SetAttrs(app, MUIA_Application_Sleep, state, TAG_DONE);
}

#endif

