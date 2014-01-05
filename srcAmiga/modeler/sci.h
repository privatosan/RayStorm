/***************
 * PROGRAM:       Scenario
 * NAME:          sci.h
 * DESCRIPTION:   interface definition for Scenario (tm) interface (sci)
 * AUTHORS:       Andreas Heumann & Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.02.97 ah    initial release
 *    03.05.97 ah    added sciCreateSurface(), sciFreeAll()
 *    12.05.97 ah    added sciLoadMaterial(), sciSaveMaterial()
 *    21.05.97 ah    added sciGetMaterialThumbNail()
 *    22.05.97 ah    added sciUpdateThumbNail(), sciBrowserClear()
 *    04.06.97 ah    added sciAssignSurface()
 *    29.07.97 ah    added sciSetActiveObjects()
 *    29.08.97 ah    added sciAssignSurfaceToSelectedObjects(), sciChangeSurface()
 *    23.09.97 ah    all menu calls now call sciDoMethod()
 ***************/

#ifndef SCI_H
#define SCI_H

#ifdef __AMIGA__
#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif
#else
class CCamView;
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifdef __AMIGA__
void sciInit(Object*, Object*, Object*, Object*);
void sciInitDisplay();
Object *sciGetMaterialThumbNail(SURFACE*);
void sciMaterialInitChange();
void sciMaterialExitChange();
BOOL sciRender();
#else
void sciInit(CCamView *);
#endif

void sciNew();
char *sciOpen(char*);
char *sciSave(char*);
void sciUndo();
void sciRedo();
void sciCut(CAMERA*);
void sciCopy();
void sciPaste();
void sciDelete(CAMERA *, ULONG);
void sciSelectAll();
void sciDeselectAll();
void sciSelectNext();
void sciSelectPrev();
BOOL sciSelectByName(char*,BOOL);
void sciRedraw();
void sciCamera2Viewer(VIEW*);
BOOL sciGroupObjects();
void sciUnGroupObjects();
BOOL sciLoadObject(char*);
BOOL sciSaveObject(char*);
BOOL sciCreateObject(int);
BOOL sciSavePrefs();
BOOL sciLoadPrefs();
void sciFocus(VIEW*, BOOL);
void sciZoomIn(VIEW*);
void sciZoomOut(VIEW*);
void sciBrowserSelect(OBJECT*);
void sciBrowserUpdate(OBJECT*);
void sciBrowserAddObject(OBJECT*);
void sciBrowserBuild();
void sciBrowserClear();
SURFACE *sciCreateSurface(char*);
void sciDeleteSurface(SURFACE*);
BOOL sciAssignSurface(SURFACE*, OBJECT*);
BOOL sciAssignSurfaceToSelectedObjects(SURFACE *surf);
void sciChangeSurface(SURFACE*, SURFACE*);
void sciAddMaterial(SURFACE*);
BOOL sciRemoveMaterial(SURFACE*, BOOL test_used=TRUE);
void sciFreeAll();
void sciOpenMaterialRequ(SURFACE*);
BOOL sciLoadMaterial(SURFACE*);
BOOL sciSaveMaterial(SURFACE*);
BOOL sciLoadLight(LIGHT*);
BOOL sciSaveLight(LIGHT*);
void sciUpdateThumbNail(SURFACE*);
BOOL sciTestForLight();
void sciSetActiveObjects(UNDO_TRANSFORM*, OBJECT*, VECTOR*, BOOL, VECTOR*, BOOL, VECTOR*, BOOL);
BOOL sciReadResFile(char***, ULONG**);
BOOL sciDoMethod(int);
char *sciRenderInit(void (*)(void*, char*), void (*)(void*, float, float, int, int, rsiSMALL_COLOR*),
	BOOL (*)(void*), void (*)(void*), void*, rsiCONTEXT**);
char *sciRender(rsiCONTEXT*);
BOOL sciScanPlugins();

enum
{
	SCIM_SETRENDERWINDOW,
	SCIM_SELECTBYBOX,
	SCIM_OBJECT2SCENE,
	SCIM_GRIDONOFF,
	SCIM_GRIDSNAP,
	SCIM_SHOWNAMES,
	SCIM_SHOWALL,
	SCIM_SHOWCAMERA,
	SCIM_SHOWBRUSHES,
	SCIM_SHOWTEXTURES,
	SCIM_SHOWLIGHT,
	SCIM_SHOWMESH,
	SCIM_SHOWSPHERE,
	SCIM_SHOWBOX,
	SCIM_SHOWCYLINDER,
	SCIM_SHOWCONE,
	SCIM_SHOWSOR,
	SCIM_SHOWPLANE,
	SCIM_SHOWCSG,
	SCIM_UNDO,
	SCIM_REDO,
	SCIM_CUT,
	SCIM_COPY,
	SCIM_PASTE,
	SCIM_DELETE,
	SCIM_EDIT_LOCAL,
	SCIM_EDITMOVEOBJECT,
	SCIM_EDITROTATEOBJECT,
	SCIM_EDITSCALEOBJECT,
	SCIM_EDITMOVEWORLD,
	SCIM_EDITROTATEWORLD,
	SCIM_EDITSCALEWORLD,
	SCIM_VIEWCAMERA,
	SCIM_VIEWFRONT,
	SCIM_VIEWRIGHT,
	SCIM_VIEWTOP,
	SCIM_VIEWPERSP,
	SCIM_VIEWQUAD,
	SCIM_SETACTIVEX,
	SCIM_SETACTIVEY,
	SCIM_SETACTIVEZ,
	SCIM_UNSETACTIVEX,
	SCIM_UNSETACTIVEY,
	SCIM_UNSETACTIVEZ,
	SCIM_DISPLAYBBOX,
	SCIM_DISPLAYWIRE,
	SCIM_DISPLAYSOLID,
	SCIM_EDITPOINTS,
	SCIM_ADDPOINTS,
	SCIM_EDITEDGES,
	SCIM_ADDEDGES,
	SCIM_EDITFACES,
	SCIM_ADDFACES,
	SCIM_SELALL,
	SCIM_DESELALL,
	SCIM_SELNEXT,
	SCIM_SELPREV,
	SCIM_REDRAW,
	SCIM_CAMERA2VIEWER,
	SCIM_GROUP,
	SCIM_UNGROUP,
	SCIM_ZOOMFIT,
	SCIM_FOCUS,
	SCIM_ZOOMIN,
	SCIM_ZOOMOUT,
	SCIM_CREATEBOX,
	SCIM_CREATESPHERE,
	SCIM_CREATECYLINDER,
	SCIM_CREATECONE,
	SCIM_CREATESOR,
	SCIM_CREATEPLANE,
	SCIM_CREATEPOINTLIGHT,
	SCIM_CREATESPOTLIGHT,
	SCIM_CREATEDIRECTIONALLIGHT,
	SCIM_CREATECAMERA,
	SCIM_CREATECSG,
	SCIM_CREATEAXIS,
	SCIM_MESHJOIN
};

#endif
