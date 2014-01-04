/***************
 * PROGRAM:       Scenario
 * NAME:          sci.cpp
 * DESCRIPTION:   interface to Scenario (tm) modeler
 * AUTHORS:       Andreas Heumann & Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.02.97 ah    initial release
 *    03.05.97 ah    added sciCreateSurface(), sciFreeAll()
 *    12.05.97 ah    added sciLoadMaterial(), sciSaveMaterial(), sciDeleteSurface()
 *    21.05.97 ah    added sciGetMaterialThumbNail()
 *    22.05.97 ah    added sciUpdateThumbNail(), sciBrowserClear()
 *    04.06.97 ah    added sciAssignSurface()
 *    29.07.97 ah    added sciSetActiveObjects()
 *    29.08.97 ah    added sciAssignSurfaceToSelectedObjects(), sciChangeSurface()
 *    17.09.97 ah    added sciReadResFile()
 ***************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __AMIGA__
#include <pragma/dos_lib.h>
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef UNDO_H
#include "undo.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef SPHERE_H
#include "sphere.h"
#endif

#ifndef BOX_H
#include "box.h"
#endif

#ifndef PLANE_H
#include "plane.h"
#endif

#ifndef CYLINDER_H
#include "cylinder.h"
#endif

#ifndef CONE_H
#include "cone.h"
#endif

#ifndef CSG_H
#include "csg.h"
#endif

#ifndef SOR_H
#include "sor.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef MESH_H
#include "mesh.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef MOUSE_H
#include "mouse.h"
#endif

#ifndef __AMIGA__
#ifndef CAMVIEW_H
#include "CamView.h"
#endif
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

static DISPLAY *display;

#ifdef __AMIGA__
	#ifndef MUIUTILITY_H
	#include "muiutility.h"
	#endif

	#ifdef __MIXEDBINARY__
	void WriteLog(void*, char*);
	#endif // __MIXEDBINARY__

	static Object *drawarea, *browser, *material, *main;
	#define REDRAW() MUI_Redraw(drawarea, MADF_DRAWUPDATE);
	#define CLEAR_SELECTED() DoMethod(drawarea, MUIM_Draw_RemoveSelected);
	#define REDRAW_SELECTED() DoMethod(drawarea,MUIM_Draw_RedrawSelected);
	#define REDRAW_DIRTY() DoMethod(drawarea,MUIM_Draw_RedrawDirty);
	#define REDRAW_CURRENTVIEW() DoMethod(drawarea,MUIM_Draw_RedrawCurrentView);
#else
	extern char szWorkingDirectory[256];
	static CCamView *pCamView;
	#define REDRAW() pCamView->Redraw(REDRAW_ALL, FALSE);
	#define CLEAR_SELECTED() pCamView->Redraw(REDRAW_REMOVE_SELECTED, FALSE);
	#define REDRAW_SELECTED() pCamView->Redraw(REDRAW_SELECTED, FALSE);
	#define REDRAW_DIRTY() pCamView->Redraw(REDRAW_DIRTY, FALSE);
	#define REDRAW_CURRENTVIEW() pCamView->Redraw(REDRAW_CURRENT_VIEW, FALSE);
#endif

#ifdef __AMIGA__
/*************
 * DESCRIPTION:   call this function before you use the interface
 * INPUT:         d        drawarea
 *                b        browser
 *                mm       material manager
 *                mw       main window
 * OUTPUT:        -
 *************/
void sciInit(Object *d, Object *b, Object *mm, Object *mw)
{
	ASSERT(d && b && mm && mw);

	drawarea = d;
	browser = b;
	material = mm;
	main = mw;
}

/*************
 * DESCRIPTION:   call this function after the window is opened
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciInitDisplay()
{
	ASSERT(drawarea);
	GetAttr(MUIA_Draw_DisplayPtr, drawarea, (ULONG*)&display);
}

/*************
 * DESCRIPTION:   Get a thumb nail image for the specified surface
 * INPUT:         surf
 * OUTPUT:        image object or NULL if none found
 *************/
Object *sciGetMaterialThumbNail(SURFACE *surf)
{
	return (Object*)DoMethod(material, MUIM_Material_GetThumbNail, surf);
}

/*************
 * DESCRIPTION:   Avoid the material manager from updating its display
 *    while adding items. Call this function is you want to add many
 *    material items. This avoids the display from flickering. After you
 *    have added the materials you HAVE to call sciMaterialExitChange().
 * INPUT:         none
 * OUTPUT:        none
 *************/
void sciMaterialInitChange()
{
	DoMethod(material, MUIM_Material_InitChange);
}

/*************
 * DESCRIPTION:   Start rendering
 * INPUT:         none
 * OUTPUT:        FALSE if failed
 *************/
BOOL sciRender()
{
	return(DoMethod(main, MUIM_MainWindow_Render));
}

/*************
 * DESCRIPTION:   Let the material manager update its display after
 *    adding items
 * INPUT:         none
 * OUTPUT:        none
 *************/
void sciMaterialExitChange()
{
	DoMethod(material, MUIM_Material_ExitChange);
}
#else
/*************
 * DESCRIPTION:   call this function before you use the interface
 * INPUT:         v        camera view
 * OUTPUT:        -
 *************/
void sciInit(CCamView *pView)
{
	pCamView = pView;
	display = pCamView->pDisplay;
}
#endif

/*************
 * DESCRIPTION:   Reset scene to default
 * INPUT:         display     current display
 * OUTPUT:        -
 *************/
void sciNew()
{
	SURFACE *surf;

	// set title to 'untitled'
	SetProjectName("untitled");
	sciBrowserClear();

	// free undo buffer
	FreeUndo();
	// remove all objects
	FreeObjects();
	// free all surfaces
	FreeSurfaces();
	// and create the default surface
	surf = sciCreateSurface(SURF_DEFAULTNAME);
	surf->flags |= SURF_DEFAULT;
#ifdef __AMIGA__
	DoMethod(material, MUIM_Material_FreeMaterials);
#else
	pCamView->pMatView->FreeMaterials();
#endif

	global.ToDefaults();
	display->ToDefaults();
	display->view->ToDefaults();

	SetProjectChanged(FALSE);

	REDRAW();
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Open a new scene
 * INPUT:         name        name of scene to load
 * OUTPUT:        error string or NULL if all went ok
 *************/
char *sciOpen(char *name)
{
	char *err;
	SURFACE *surf;

	// set the new project name
	SetProjectName(name);
	sciBrowserClear();

	// free undo buffer
	FreeUndo();
	// then remove all objects
	FreeObjects();
	// free all surfaces
	FreeSurfaces();
	// and create the default surface
	surf = sciCreateSurface(SURF_DEFAULTNAME);
	surf->flags |= SURF_DEFAULT;
#ifdef __AMIGA__
	DoMethod(material, MUIM_Material_FreeMaterials);
	sciMaterialInitChange();
#else
	pCamView->pMatView->FreeMaterials();
#endif

	// and load new project
	err = ProjectLoad(name, display);
#ifdef __AMIGA__
	sciMaterialExitChange();
#endif
	if(err)
		return err;

	DeselectAll();

	display->SetMultiView(display->multiview);
	REDRAW();

	sciBrowserBuild();

	return NULL;
}

/*************
 * DESCRIPTION:   Save the current scene
 * INPUT:         name        name of scene to save
 * OUTPUT:        error string or NULL if all went ok
 *************/
char *sciSave(char *name)
{
	char *err, buffer[256];

	// save project
	err = ProjectSave(name, display);
	if(err)
		return err;

	// set the new project name
	SetProjectName(name);

	// set output picture name if no is set
	if(!global.renderdpic)
	{
		strcpy(buffer, name);
#ifdef __AMIGA__
		*PathPart(buffer) = 0;
#else
		DirPart(buffer, buffer, 255);
#endif
		AddPart(buffer, "pic0001", 255);
		global.SetRenderdPic(buffer);
	}

	return NULL;
}

/*************
 * DESCRIPTION:   Undo the last operation
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciUndo()
{
	switch(Undo())
	{
		case UNDO_NOUNDO:
			utility.Request("Nothing to undo");
			break;
		case UNDO_FAILED:
			utility.Request("Undo failed. Not enough memory.");
			break;
		case UNDO_OK_REDRAW:
			REDRAW();
			break;
	}

	SetProjectChanged(TRUE);
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Redo the last operation
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciRedo()
{
	switch(Redo())
	{
		case UNDO_NOREDO:
			utility.Request("Nothing to redo");
			break;
		case UNDO_FAILED:
			utility.Request("Redo failed. Not enough memory.");
			break;
		case UNDO_OK_REDRAW:
			REDRAW();
			break;
	}

	SetProjectChanged(TRUE);
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Cut selected objects to paste buffer (active camera is excluded)
 * INPUT:         camera   active camera
 * OUTPUT:        -
 *************/
void sciCut(CAMERA *camera)
{
	if(!Cut(camera))
		utility.Request("Not enough memory");

	CLEAR_SELECTED();

	// redraw the camera if it was cleared
	if(camera->selected)
	{
		camera->flags |= OBJECT_DIRTY;
		REDRAW_DIRTY();
	}

	SetProjectChanged(TRUE);
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Copy selected objects to paste buffer
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciCopy()
{
	if(!Copy())
		utility.Request("Not enough memory");
}

/*************
 * DESCRIPTION:   Paste objects from paste buffer
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciPaste()
{
	DeselectAll();

	if(!Paste())
		utility.Request("Not enough memory");

	REDRAW();
	SetProjectChanged(TRUE);
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Delete selected objects (active camera is excluded)
 * INPUT:         camera   active camera
 *                mode     mode (copy of display->editmode)
 * OUTPUT:        -
 *************/
void sciDelete(CAMERA *camera, ULONG mode)
{
	DeleteSelected(camera, mode);

	CLEAR_SELECTED();

	// redraw the camera if it was deleted
	if(camera->selected)
		camera->flags |= OBJECT_DIRTY;

	REDRAW_DIRTY();

	SetProjectChanged(TRUE);
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Select all objects
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciSelectAll()
{
	SelectAll();
	REDRAW_DIRTY();
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Deselect all selected objects
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciDeselectAll()
{
	DeselectAll();
	REDRAW_DIRTY();
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Select next object
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciSelectNext()
{
	SelectNext();
	REDRAW_DIRTY();
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Select previous object
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciSelectPrev()
{
	SelectPrev();
	REDRAW_DIRTY();
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Select an object by it's name
 * INPUT:         name
 *                multi    if TRUE multiselect is enabled
 * OUTPUT:        TRUE if found, else FALSE
 *************/
BOOL sciSelectByName(char *name, BOOL multi)
{
	OBJECT *obj;

	obj = GetObjectByName(name);
	if(obj)
	{
		if(!multi)
			DeselectAll();
		obj->Select();
		return TRUE;
	}
	return FALSE;
}

/*************
 * DESCRIPTION:   Redraw
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciRedraw()
{
	REDRAW();
}

/*************
 * DESCRIPTION:   Redraw current view
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciRedrawCurrentView()
{
	REDRAW_CURRENTVIEW();
}

/*************
 * DESCRIPTION:   Set active camera to position of virtual camera
 * INPUT:         view  current view
 * OUTPUT:        -
 *************/
void sciCamera2Viewer(VIEW *view)
{
	view->Camera2Viewer(display->camera);
	REDRAW();
	SetProjectChanged(TRUE);
}

/*************
 * DESCRIPTION:   Group currently selected objects
 * INPUT:         -
 * OUTPUT:        FALSE if failed (less than two objects selected) else TRUE
 *************/
BOOL sciGroupObjects()
{
	if(!GroupObjects())
		return FALSE;

	REDRAW_DIRTY();
	SetProjectChanged(TRUE);
	sciBrowserBuild();

	return TRUE;
}

/*************
 * DESCRIPTION:   UnGroup currently selected objects
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciUnGroupObjects()
{
	UnGroupObjects();

	CLEAR_SELECTED();
	REDRAW_DIRTY();

	SetProjectChanged(TRUE);
	sciBrowserBuild();
}

/*************
 * DESCRIPTION:   Load an object
 * INPUT:         name     file name
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL sciLoadObject(char *name)
{
	char *err;

	DeselectAll();
	err = LoadObject(name);
	if(err)
	{
		utility.Request("%s\n%s", err, GetLoadError());
		return FALSE;
	}

	REDRAW_DIRTY();
	SetProjectChanged(TRUE);
	sciBrowserBuild();

	return TRUE;
}

/*************
 * DESCRIPTION:   Save selected object(s)
 * INPUT:         name     file name
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL sciSaveObject(char *name)
{
	FILE *f;
	char *err;

	f = fopen(name,"r");
	if(f)
	{
		fclose(f);
		if(!utility.AskRequest("File '%s' already exists. Overwrite?", name))
			return FALSE;
	}

	err = SaveObject(name);
	if(err)
	{
		utility.Request("%s\n%s", err, GetLoadError());
		return FALSE;
	}

	REDRAW_DIRTY();
	SetProjectChanged(TRUE);
	sciBrowserBuild();

	return TRUE;
}

/*************
 * DESCRIPTION:   Create a new object
 * INPUT:         type     type of object to create (OBJECT_xxx)
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL sciCreateObject(int type)
{
	OBJECT *obj;
	UNDO_CREATE *undo;

	switch(type)
	{
		case OBJECT_SPHERE:
			obj = new SPHERE;
			break;
		case OBJECT_BOX:
			obj = new BOX;
			break;
		case OBJECT_PLANE:
			obj = new PLANE;
			break;
		case OBJECT_POINTLIGHT:
			obj = new POINT_LIGHT;
			break;
		case OBJECT_SPOTLIGHT:
			obj = new SPOT_LIGHT;
			break;
		case OBJECT_DIRECTIONALLIGHT:
			obj = new DIRECTIONAL_LIGHT;
			break;
		case OBJECT_CAMERA:
			obj = new CAMERA;
			break;
		case OBJECT_MESH:
			obj = new MESH;
			break;
		case OBJECT_CYLINDER:
			obj = new CYLINDER;
			break;
		case OBJECT_CONE:
			obj = new CONE;
			break;
		case OBJECT_CSG:
			obj = new CSG;
			break;
		case OBJECT_SOR:
			obj = new SOR;
			break;
	}
	if(!obj)
		return FALSE;

	// create surface for visible objects
	if((type == OBJECT_SPHERE) ||
		(type == OBJECT_BOX) ||
		(type == OBJECT_PLANE) ||
		(type == OBJECT_MESH) ||
		(type == OBJECT_CYLINDER) ||
		(type == OBJECT_CONE) ||
		(type == OBJECT_SOR))
	{
		obj->surf = GetSurfaceByName(SURF_DEFAULTNAME);
		if(!obj->surf)
		{
			obj->surf = sciCreateSurface(SURF_DEFAULTNAME);
			if(!obj->surf)
			{
				delete obj;
				return FALSE;
			}
			sciAddMaterial(obj->surf);
		}
		else
		{
			if(!sciAssignSurface(obj->surf, obj))
			{
				delete obj;
				return FALSE;
			}
		}
	}

	obj->selected = TRUE;
	DeselectAll();
	obj->Append();
	obj->IsFirstSelected();

	undo = new UNDO_CREATE;
	if(undo)
	{
		if(undo->AddCreated(obj))
			undo->Add();
		else
			delete undo;
	}

	REDRAW_DIRTY();
	SetProjectChanged(TRUE);
	sciBrowserAddObject(obj);
	return TRUE;
}

/*************
 * DESCRIPTION:   Load preferences
 * INPUT:         -
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL sciLoadPrefs()
{
	BOOL success;

	FreePrefs();
#ifdef __AMIGA__
	success = PrefsLoad("prefs/Scenario.prf");
#else
	char szBuf[256];
	sprintf(szBuf, "%s\\prefs\\Scenario.prf", szWorkingDirectory);
	success = PrefsLoad(szBuf);
#endif
	if(success)
		GetColorPrefs();

	return success;
}

/*************
 * DESCRIPTION:   Save preferences
 * INPUT:         -
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL sciSavePrefs()
{
	SetColorPrefs();
#ifdef __AMIGA__
	return PrefsSave("prefs/Scenario.prf");
#else
	char szBuf[256];
	sprintf(szBuf, "%s\\prefs\\Scenario.prf", szWorkingDirectory);
	return PrefsSave(szBuf);
#endif
}

/*************
 * DESCRIPTION:   focus objects
 * INPUT:         view     current view
 *                all      TRUE: whole scene FALSE: only selected objects
 * OUTPUT:        -
 *************/
void sciFocus(VIEW *view, BOOL all)
{
	if(Focus(view, all))
		REDRAW_CURRENTVIEW();
}

/*************
 * DESCRIPTION:   zooms in one step
 * INPUT:         view     current view
 * OUTPUT:        -
 *************/
void sciZoomIn(VIEW *view)
{
	ZoomIn(view);
	REDRAW_CURRENTVIEW();
}

/*************
 * DESCRIPTION:   zooms out one step
 * INPUT:         view     current view
 * OUTPUT:        -
 *************/
void sciZoomOut(VIEW *view)
{
	ZoomOut(view);
	REDRAW_CURRENTVIEW();
}

/*************
 * DESCRIPTION:   update
 * INPUT:         obj   object to be updated
 * OUTPUT:        -
 *************/
void sciBrowserUpdate(OBJECT *obj)
{
#ifdef __AMIGA__
	DoMethod(browser, MUIM_BrowserTree_Update, obj);
#else
	pCamView->pBrowserView->Build();
#endif
}

/*************
 * DESCRIPTION:   add a new created object to the end of the list
 * INPUT:         obj   object to be added
 * OUTPUT:        -
 *************/
void sciBrowserAddObject(OBJECT *obj)
{
#ifdef __AMIGA__
	DoMethod(browser, MUIM_BrowserTree_AddObject, obj);
#else
	pCamView->pBrowserView->Build();
#endif
}

/*************
 * DESCRIPTION:   build complete browser hierarchie
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciBrowserBuild()
{
#ifdef __AMIGA__
	DoMethod(browser, MUIM_BrowserTree_Build);
#else
	pCamView->pBrowserView->Build();
#endif
}

/*************
 * DESCRIPTION:   clear browser
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciBrowserClear()
{
#ifdef __AMIGA__
	DoMethod(browser, MUIM_BrowserTree_Clear);
#else
	pCamView->pBrowserView->Clear();
#endif
}

/*************
 * DESCRIPTION:   Create a new surface.
 * INPUT:         name     name of surface
 * OUTPUT:        created surface
 *************/
SURFACE *sciCreateSurface(char *name)
{
	SURFACE *surf;

	surf = new SURFACE;
	if(!surf)
		return NULL;

	if(!surf->SetName(name))
	{
		delete surf;
		return NULL;
	}

	surf->Add();

	return surf;
}

/*************
 * DESCRIPTION:   Delete a surface.
 * INPUT:         surf     surface to delete
 * OUTPUT:        -
 *************/
void sciDeleteSurface(SURFACE *surf)
{
	surf->Remove();
}

/*************
 * DESCRIPTION:   Assign a surface to an object. Local brushes and textures
 *    are created.
 * INPUT:         surf     surface to assign
 *                obj      obj to assign surface to
 * OUTPUT:        FALSE if failed
 *************/
BOOL sciAssignSurface(SURFACE *surf, OBJECT *obj)
{
	BRUSH *brush;
	BRUSH_OBJECT *brushobj;
	TEXTURE *texture;
	TEXTURE_OBJECT *textureobj;
	int type;

	type = obj->GetType();
	if((type == OBJECT_CSG) || (type == OBJECT_CAMERA) || (type == OBJECT_POINTLIGHT) || (type == OBJECT_SPOTLIGHT) || (type == OBJECT_DIRECTIONALLIGHT))
		return TRUE;

	// remove old brushes
	obj->RemoveBrushes();

	// create brushes
	brush = surf->brush;
	while(brush)
	{
		brushobj = new BRUSH_OBJECT;
		if(!brushobj)
			return FALSE;

		brushobj->brush = brush;
		obj->AddBrushTop(brushobj);
		brush = (BRUSH*)brush->GetNext();
	}

	// remove old textures
	obj->RemoveTextures();

	// create textures
	texture = surf->texture;
	while(texture)
	{
		textureobj = new TEXTURE_OBJECT;
		if(!textureobj)
			return FALSE;

		textureobj->texture = texture;
		obj->AddTextureTop(textureobj);
		texture = (TEXTURE*)texture->GetNext();
	}

	obj->surf = surf;

	sciBrowserUpdate(obj);

	return TRUE;
}

/*************
 * DESCRIPTION:   Assign a surface to all selected objects.
 * INPUT:         surf     surface to assign
 * OUTPUT:        FALSE if failed
 *************/
typedef struct
{
	SURFACE *surf;
	BOOL success;
} AssignSurfaceToSelectedObjectsData;

static void AssignSurfaceToSelectedObjectsCB(OBJECT *obj, void *data)
{
	if(obj)
	{
		if(obj->surf)
		{
			if(!sciAssignSurface(((AssignSurfaceToSelectedObjectsData*)data)->surf, obj))
				((AssignSurfaceToSelectedObjectsData*)data)->success = FALSE;
		}
	}
}

BOOL sciAssignSurfaceToSelectedObjects(SURFACE *surf)
{
	AssignSurfaceToSelectedObjectsData data;

	data.surf = surf;
	data.success = TRUE;

	GetSelectedObjects(AssignSurfaceToSelectedObjectsCB, &data, 0);
	return data.success;
}

/*************
 * DESCRIPTION:   Change the surface of objects with a specified surface.
 * INPUT:         from     objects with this surface are changed
 *                to       this surface is assigned to the object
 * OUTPUT:        -
 *************/
typedef struct
{
	SURFACE *from, *to;
} ChangeSurfaceData;

static void ChangeSurfaceCB(OBJECT *obj, void *data)
{
	if(obj)
	{
		if(obj->surf == ((ChangeSurfaceData*)data)->from)
			sciAssignSurface(((ChangeSurfaceData*)data)->to, obj);
	}
}

void sciChangeSurface(SURFACE *from, SURFACE *to)
{
	ChangeSurfaceData data;

	data.from = from;
	data.to = to;
	GetObjects(ChangeSurfaceCB, &data, 0);
}

/*************
 * DESCRIPTION:   Add a surface to the material manager.
 * INPUT:         surf     surface
 * OUTPUT:        -
 *************/
void sciAddMaterial(SURFACE *surf)
{
	// add new created surface to material manager
#ifdef __AMIGA__
	DoMethod(material, MUIM_Material_AddMaterial, surf);
#else
	pCamView->pMatView->AddMaterial(surf);
#endif
	SetProjectChanged(TRUE);
}

/*************
 * DESCRIPTION:   Remove a surface from the material manager. It's tested before if the
 *    surface is used by an object.
 * INPUT:         surf     surface
 *                test_used   TRUE -> test if surface is used and refuse deleting when used
 * OUTPUT:        FALSE if failed
 *************/
BOOL sciRemoveMaterial(SURFACE *surf, BOOL test_used)
{
	OBJECT *obj;

	if(test_used)
	{
		obj = IsSurfaceUsed(surf);
		if(obj)
		{
			utility.Request("Can't delete Surface because it is used by %s.", obj->GetName());
			return FALSE;
		}
	}

	// is requester open?
	if(surf->win)
	{
#ifdef __AMIGA__
		// close requester
		SetAttrs(surf->win, MUIA_Window_Open, FALSE, TAG_DONE);
		DoMethod(app, OM_REMMEMBER, surf->win);
		MUI_DisposeObject(surf->win);
		surf->win = NULL;
#else
// Mike: Dialog schließen
#endif
	}

#ifdef __AMIGA__
	DoMethod(material, MUIM_Material_RemoveMaterial, surf);
#endif
	SetProjectChanged(TRUE);

	return TRUE;
}

/*************
 * DESCRIPTION:   free all scenario data
 * INPUT:         -
 * OUTPUT:        -
 *************/
void sciFreeAll()
{
	FreeUndo();
	FreeObjects();
	FreePrefs();
	FreeSurfaces();
}

/*************
 * DESCRIPTION:   open material requester
 * INPUT:         surf     surface to open requester on
 * OUTPUT:        -
 *************/
void sciOpenMaterialRequ(SURFACE *surf)
{
#ifdef __AMIGA__
	DoMethod(main, MUIM_MainWindow_OpenMaterialRequ, surf);
#else
	pCamView->MaterialDlg(surf, NULL);
#endif
}

/*************
 * DESCRIPTION:   pop up a file requester and ask for a material file and
 *    load it
 * INPUT:         surf     surface to store loaded material to
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL sciLoadMaterial(SURFACE *surf)
{
	char *err;
	char file[256];
	BRUSH *brush, *bnext;
	TEXTURE *texture, *tnext;

	if(utility.FileRequ(file, FILEREQU_MATERIAL, 0))
	{
		// free all brushes and textures
		brush = surf->brush;
		while(brush)
		{
			bnext = (BRUSH*)brush->GetNext();
			delete brush;
			brush = bnext;
		}
		surf->brush = NULL;

		texture = surf->texture;
		while(texture)
		{
			tnext = (TEXTURE*)texture->GetNext();
			delete texture;
			texture = tnext;
		}
		surf->texture = NULL;

		err = surf->Load(file);
		if(err)
		{
			utility.Request(err);
			return FALSE;
		}
		SetProjectChanged(TRUE);
	}
	else
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   pop up a file requester and ask for a material file name and
 *     save it
 * INPUT:         surf     surface to store
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL sciSaveMaterial(SURFACE *surf)
{
	char *err;
	char file[256];

	if(utility.FileRequ(file, FILEREQU_MATERIAL, FILEREQU_SAVE))
	{
		err = surf->Save(file);
		if(err)
		{
			utility.Request(err);
			return FALSE;
		}
	}
	else
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   pop up a file requester and ask for a light file and
 *    load it
 * INPUT:         light    light to store data to
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL sciLoadLight(LIGHT *light)
{
	char *err;
	char file[256];

	if(utility.FileRequ(file, FILEREQU_LIGHT, 0))
	{
		err = light->Load(file);
		if(err)
		{
			utility.Request(err);
			return FALSE;
		}
		SetProjectChanged(TRUE);
	}
	else
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   pop up a file requester and ask for a light file name and
 *     save it
 * INPUT:         light    light to store
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL sciSaveLight(LIGHT *light)
{
	char *err;
	char file[256];

	if(utility.FileRequ(file, FILEREQU_LIGHT, FILEREQU_SAVE))
	{
		err = light->Save(file);
		if(err)
		{
			utility.Request(err);
			return FALSE;
		}
	}
	else
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   Update the thumb nail gfx of a surface of a browser item
 * INPUT:         surf     surface
 * OUTPUT:        -
 *************/
void sciUpdateThumbNail(SURFACE *surf)
{
#ifdef __AMIGA__
	DoMethod(browser, MUIM_BrowserTree_UpdateThumbNail, surf);
#endif
}

/*************
 * DESCRIPTION:   test if there is a light source in the scene
 * INPUT:         none
 * OUTPUT:        TRUE if a light is found
 *************/
static void TestForLight(OBJECT *obj, void *lights)
{
	ASSERT(lights);
	if(obj)
	{
		switch(obj->GetType())
		{
			case OBJECT_POINTLIGHT:
			case OBJECT_SPOTLIGHT:
			case OBJECT_DIRECTIONALLIGHT:
				*((BOOL*)lights) = TRUE;
				break;
		}
	}
}

BOOL sciTestForLight()
{
	BOOL lights;

	lights = FALSE;
	GetObjects(TestForLight, &lights, 0);
	return lights;
}

/*************
 * DESCRIPTION:   set the position, size and alignment of the selected object(s)
 * INPUT:         undo        undo object with all objects to set
 *                object      primary object
 *                newpos
 *                poslocal    if TRUE position change is in model coordinates
 *                newsize
 *                sizelocal   if TRUE size change is in model coordinates
 *                newalign
 *                alignlocal  if TRUE alignment change is in model coordinates
 * OUTPUT:        -
 *************/
void sciSetActiveObjects(UNDO_TRANSFORM *undo, OBJECT *object,
	VECTOR *newpos, BOOL poslocal, VECTOR *newsize, BOOL sizelocal, VECTOR *newalign, BOOL alignlocal)
{
	VECTOR pos, orient_x, orient_y, orient_z;
	BOOL oldposlocal, oldalignlocal, oldsizelocal;
	MATRIX m;

	ASSERT(undo && object && newpos && newsize && newalign);

	oldposlocal = display->localpos;
	display->localpos = poslocal;
	oldalignlocal = display->localalign;
	display->localalign = alignlocal;
	oldsizelocal = display->localsize;
	display->localsize = sizelocal;

	// update the axis values in the undo object
	// (maybe another requester changed the object after
	// we opened our requester)
	undo->UpdateObjects();

	object->GetObjectMatrix(&m);
	m.GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	// change position
	if(poslocal)
		display->pos = *newpos;
	else
		VecSub(newpos, &pos, &display->pos);

	// change size
	SetVector(&display->size,
		newsize->x / object->size.x,
		newsize->y / object->size.y,
		newsize->z / object->size.z);

	// change alignment
	display->align.x = newalign->x - object->align.x;
	display->align.y = newalign->y - object->align.y;
	display->align.z = newalign->z - object->align.z;
	newalign->y = -newalign->y;
	CalcOrient(newalign, &display->orient_x, &display->orient_y, &display->orient_z);
	if(!alignlocal)
		InvOrient(&display->orient_x, &display->orient_y, &display->orient_z, &display->orient_x, &display->orient_y, &display->orient_z);
	undo->SetObjects(display);

	display->localpos = oldposlocal;
	display->localalign = oldalignlocal;
	display->localsize = oldsizelocal;
}

/*************
 * DESCRIPTION:   Read the resolutions from the a preset file. This routine allocates an array of
 *    description strings and resolution values and fills in the values it reads from a ASCII file.
 *    The last string in the string array is followed by a NULL-pointer. After you dont't need the
 *    strings any more you have to free each single string AND the array which holds the pointers AND
 *    the array with the resolution values.
 * INPUT:         resolutions    pointer to a pointer to a array with pointers to resolution strings (WOW!)
 *                res            pointer to pointer to a array of resolution values (x-y pairs)
 * OUTPUT:        FALSE if failed
 *************/
BOOL sciReadResFile(char ***resolutions, ULONG **res)
{
	FILE *f;
	int amount, cur, i;
	char buffer[256], *s;

	ASSERT(resolutions && res);

#ifdef __AMIGA__
	f = fopen("prefs/res.dat","rb");
#else
	f = fopen("prefs\\res.dat","rb");
#endif
	if(!f)
		return FALSE;

	amount = 0;
	while(fgets(buffer, 256, f))
		amount++;

	*resolutions = new char*[amount+1];
	if(!resolutions)
	{
		fclose(f);
		return FALSE;
	}
	*res = new ULONG[2*amount];
	if(!*res)
	{
		delete resolutions;
		fclose(f);
		return FALSE;
	}

	fseek(f, 0, SEEK_SET);
	cur = 0;
	while(fgets(buffer, 256, f))
	{
		(*resolutions)[cur] = new char[strlen(buffer)+1];
		if(!(*resolutions)[cur])
		{
			for(i=0; i<cur; i++)
				delete resolutions[i];
			delete resolutions;
			delete res;
			fclose(f);
			return FALSE;
		}
		strtok(buffer, ",");
		strcpy((*resolutions)[cur], buffer);
		s = strtok(NULL, ",");
		(*res)[cur*2] = atol(s);
		s = strtok(NULL, ",");
		(*res)[cur*2+1] = atol(s);
		cur++;
	}
	(*resolutions)[amount] = NULL;
	fclose(f);

	return TRUE;
}

/*************
 * DESCRIPTION:   Join selected mesh objects
 * INPUT:         -
 * OUTPUT:        FALSE if failed
 *************/
typedef struct
{
	OBJECT *to;
} MeshJoinData;

static void MeshJoinCB(OBJECT *obj, void *data)
{
	if(obj)
	{
		if((obj != ((MeshJoinData*)data)->to) && (obj->selected) && (obj->GetType() == OBJECT_MESH))
			((MESH*)(((MeshJoinData*)data)->to))->Join(((MESH*)obj));
	}
}

BOOL sciMeshJoin()
{
	OBJECT *selobj = NULL;
	MeshJoinData data;

	GetActiveObjects(&selobj);
	if(!selobj)
	{
		utility.Request("No object selected.");
		return FALSE;
	}

	if(selobj->GetType() != OBJECT_MESH)
	{
		utility.Request("First selected object has to be a mesh.");
		return FALSE;
	}

	data.to = selobj;
	GetObjects(MeshJoinCB, &data, 0);

	REDRAW();

	return TRUE;
}

/*************
 * DESCRIPTION:   all menu functions come through this function
 * INPUT:         method
 * OUTPUT:        -
 *************/
BOOL sciDoMethod(int method)
{
	OBJECT *active;

	ASSERT(display);

	switch(method)
	{
		case SCIM_SETRENDERWINDOW:
			if(display->editmode & EDIT_SELBOX)
				display->editmode &= ~EDIT_SELBOX;
			else
			{
				display->editmode |= EDIT_SELBOX;
				display->boxmode = BOXMODE_RENDERWINDOW;
			}
			break;
		case SCIM_SELECTBYBOX:
			if(display->editmode & EDIT_SELBOX)
				display->editmode &= ~EDIT_SELBOX;
			else
			{
				utility.CheckMenuItem(MENU_SETRENDERWINDOW, FALSE);
				display->editmode |= EDIT_SELBOX;
				display->boxmode = BOXMODE_SELECT;
			}
			break;
		case SCIM_OBJECT2SCENE:
			GetActiveObjects(&active, FALSE);
			if(active)
			{
				if(active->GetType() == OBJECT_MESH)
				{
					((MESH*)active)->ToScene();
					delete active;
					sciBrowserBuild();
					REDRAW();
				}
			}
			break;
		case SCIM_GRIDONOFF:
			display->grid = !display->grid;
			REDRAW();
			SetProjectChanged(TRUE);
			break;
		case SCIM_GRIDSNAP:
			display->gridsnap = !display->gridsnap;
			SetProjectChanged(TRUE);
			break;
		case SCIM_SHOWNAMES:
			display->name_disp = !display->name_disp;
			REDRAW();
			break;
		case SCIM_SHOWALL:
			display->filter_flags = FILTER_ALL;
			REDRAW();
			utility.CheckMenuItem(MENU_SHOWCAMERA, TRUE);
			utility.CheckMenuItem(MENU_SHOWBRUSHES, TRUE);
			utility.CheckMenuItem(MENU_SHOWTEXTURES, TRUE);
			utility.CheckMenuItem(MENU_SHOWLIGHT, TRUE);
			utility.CheckMenuItem(MENU_SHOWMESH, TRUE);
			utility.CheckMenuItem(MENU_SHOWSPHERE, TRUE);
			utility.CheckMenuItem(MENU_SHOWBOX, TRUE);
			utility.CheckMenuItem(MENU_SHOWCYLINDER, TRUE);
			utility.CheckMenuItem(MENU_SHOWCONE, TRUE);
			utility.CheckMenuItem(MENU_SHOWSOR, TRUE);
			utility.CheckMenuItem(MENU_SHOWPLANE, TRUE);
			utility.CheckMenuItem(MENU_SHOWCSG, TRUE);
			break;
		case SCIM_SHOWBRUSHES:
			display->filter_flags ^= FILTER_BRUSH;
			if(!(display->filter_flags & FILTER_BRUSH))
				DeselectBrushes();
			REDRAW();
			break;
		case SCIM_SHOWTEXTURES:
			display->filter_flags ^= FILTER_TEXTURE;
			if(!(display->filter_flags & FILTER_TEXTURE))
				DeselectTextures();
			REDRAW();
			break;
		case SCIM_SHOWCAMERA:
			display->filter_flags ^= FILTER_CAMERA;
			REDRAW();
			break;
		case SCIM_SHOWLIGHT:
			display->filter_flags ^= FILTER_LIGHT;
			REDRAW();
			break;
		case SCIM_SHOWMESH:
			display->filter_flags ^= FILTER_MESH;
			REDRAW();
			break;
		case SCIM_SHOWSPHERE:
			display->filter_flags ^= FILTER_SPHERE;
			REDRAW();
			break;
		case SCIM_SHOWBOX:
			display->filter_flags ^= FILTER_BOX;
			REDRAW();
			break;
		case SCIM_SHOWCYLINDER:
			display->filter_flags ^= FILTER_CYLINDER;
			REDRAW();
			break;
		case SCIM_SHOWCONE:
			display->filter_flags ^= FILTER_CONE;
			REDRAW();
			break;
		case SCIM_SHOWSOR:
			display->filter_flags ^= FILTER_SOR;
			REDRAW();
			break;
		case SCIM_SHOWPLANE:
			display->filter_flags ^= FILTER_PLANE;
			REDRAW();
			break;
		case SCIM_SHOWCSG:
			display->filter_flags ^= FILTER_CSG;
			REDRAW();
			break;
		case SCIM_UNDO:
			sciUndo();
			break;
		case SCIM_REDO:
			sciRedo();
			break;
		case SCIM_CUT:
			sciCut(display->camera);
			break;
		case SCIM_COPY:
			sciCopy();
			break;
		case SCIM_PASTE:
			sciPaste();
			break;
		case SCIM_DELETE:
			sciDelete(display->camera, display->editmode);
			break;
		case SCIM_EDITMOVEOBJECT:
			display->editmode &= ~EDIT_MASK;
			display->editmode |= EDIT_MOVE | EDIT_OBJECT;
			break;
		case SCIM_EDITROTATEOBJECT:
			display->editmode &= ~EDIT_MASK;
			display->editmode |= EDIT_ROTATE | EDIT_OBJECT;
			break;
		case SCIM_EDITSCALEOBJECT:
			display->editmode &= ~EDIT_MASK;
			display->editmode |= EDIT_SCALE | EDIT_OBJECT;
			break;
		case SCIM_EDITMOVEWORLD:
			display->editmode &= ~(EDIT_MASK | EDIT_OBJECT);
			display->editmode |= EDIT_MOVE;
			break;
		case SCIM_EDITROTATEWORLD:
			display->editmode &= ~(EDIT_MASK | EDIT_OBJECT);
			display->editmode |= EDIT_ROTATE;
			break;
		case SCIM_EDITSCALEWORLD:
			display->editmode &= ~(EDIT_MASK | EDIT_OBJECT);
			display->editmode |= EDIT_SCALE;
			break;
		case SCIM_VIEWCAMERA:
			display->view->viewmode = VIEW_CAMERA;
			display->view->SetCamera(display->camera);
			REDRAW_CURRENTVIEW();
			// enable Set Render Window menu item
			utility.EnableMenuItem(MENU_SETRENDERWINDOW, TRUE);
			utility.CheckMenuItem(MENU_VIEWCAMERA, TRUE);
			break;
		case SCIM_VIEWFRONT:
			display->view->viewmode = VIEW_FRONT;
			REDRAW_CURRENTVIEW();
			// disable Set Render Window menu item
			utility.EnableMenuItem(MENU_SETRENDERWINDOW, FALSE);
			utility.CheckMenuItem(MENU_VIEWFRONT, TRUE);
			break;
		case SCIM_VIEWRIGHT:
			display->view->viewmode = VIEW_RIGHT;
			REDRAW_CURRENTVIEW();
			// disable Set Render Window menu item
			utility.EnableMenuItem(MENU_SETRENDERWINDOW, FALSE);
			utility.CheckMenuItem(MENU_VIEWRIGHT, TRUE);
			break;
		case SCIM_VIEWTOP:
			display->view->viewmode = VIEW_TOP;
			REDRAW_CURRENTVIEW();
			// disable Set Render Window menu item
			utility.EnableMenuItem(MENU_SETRENDERWINDOW, FALSE);
			utility.CheckMenuItem(MENU_VIEWTOP, TRUE);
			break;
		case SCIM_VIEWPERSP:
			display->view->viewmode = VIEW_PERSP;
			display->view->SetVirtualCamera();
			REDRAW_CURRENTVIEW();
			// disable Set Render Window menu item
			utility.EnableMenuItem(MENU_SETRENDERWINDOW, FALSE);
			utility.CheckMenuItem(MENU_VIEWPERSP, TRUE);
			break;
		case SCIM_VIEWQUAD:
			display->SetMultiView(!display->multiview);
			REDRAW();
			utility.CheckMenuItem(MENU_VIEWQUAD, display->multiview);
			break;
		case SCIM_TOGGLEACTIVEX:
			display->active ^= ACTIVE_X;
			break;
		case SCIM_TOGGLEACTIVEY:
			display->active ^= ACTIVE_Y;
			break;
		case SCIM_TOGGLEACTIVEZ:
			display->active ^= ACTIVE_Z;
			break;
		case SCIM_SETACTIVEX:
			display->active |= ACTIVE_X;
			break;
		case SCIM_SETACTIVEY:
			display->active |= ACTIVE_Y;
			break;
		case SCIM_SETACTIVEZ:
			display->active |= ACTIVE_Z;
			break;
		case SCIM_UNSETACTIVEX:
			display->active &= ~ACTIVE_X;
			break;
		case SCIM_UNSETACTIVEY:
			display->active &= ~ACTIVE_Y;
			break;
		case SCIM_UNSETACTIVEZ:
			display->active &= ~ACTIVE_Z;
			break;
		case SCIM_DISPLAYBBOX:
			display->SetDisplayMode(DISPLAY_BBOX);
			REDRAW();
			break;
		case SCIM_DISPLAYWIRE:
			display->SetDisplayMode(DISPLAY_WIRE);
			REDRAW();
			break;
		case SCIM_DISPLAYSOLID:
			display->SetDisplayMode(DISPLAY_SOLID);
			REDRAW();
			break;
		case SCIM_EDIT_LOCAL:
			display->localpos = !display->localpos;
			display->localalign = !display->localalign;
			display->localsize = !display->localsize;
			break;
		case SCIM_EDITPOINTS:
			if(display->editmode & EDIT_EDITPOINTS)
			{
				OBJECT *selobj;

				GetActiveObjects(&selobj, TRUE);
				if(selobj)
				{
					// free selected point buffers
					selobj->FreeSelectedPoints();
				}
				display->editmode &= ~(EDIT_EDITPOINTS | EDIT_MESH);
			}
			else
			{
				display->editmode |= EDIT_EDITPOINTS | EDIT_MESH;
			}
			REDRAW_SELECTED();
			break;
		case SCIM_ADDPOINTS:
			if(display->editmode & EDIT_ADDPOINTS)
			{
				OBJECT *selobj;

				GetActiveObjects(&selobj, TRUE);
				if(selobj)
					selobj->FreeSelectedPoints();
				display->editmode &= ~(EDIT_ADDPOINTS | EDIT_MESH);
			}
			else
				display->editmode |= EDIT_ADDPOINTS | EDIT_MESH;
			REDRAW_SELECTED();
			break;
		case SCIM_EDITEDGES:
			if(display->editmode & EDIT_EDITEDGES)
			{
				OBJECT *selobj;

				GetActiveObjects(&selobj, TRUE);
				if(selobj)
				{
					// free selected point buffers
					selobj->FreeSelectedPoints();
				}
				display->editmode &= ~(EDIT_EDITEDGES | EDIT_MESH);
			}
			else
			{
				display->editmode |= EDIT_EDITEDGES | EDIT_MESH;
			}
			REDRAW_SELECTED();
			break;
		case SCIM_ADDEDGES:
			if(display->editmode & EDIT_ADDEDGES)
			{
				OBJECT *selobj;

				GetActiveObjects(&selobj, TRUE);
				if(selobj)
					selobj->FreeSelectedPoints();
				display->editmode &= ~(EDIT_ADDEDGES | EDIT_MESH);
			}
			else
			{
				display->editmode |= EDIT_ADDEDGES | EDIT_MESH;
			}
			REDRAW_SELECTED();
			break;
		case SCIM_EDITFACES:
			if(display->editmode & EDIT_EDITFACES)
			{
				OBJECT *selobj;

				GetActiveObjects(&selobj, TRUE);
				if(selobj)
				{
					// free selected point buffers
					selobj->FreeSelectedPoints();
				}
				display->editmode &= ~(EDIT_EDITFACES | EDIT_MESH);
			}
			else
			{
				display->editmode |= EDIT_EDITFACES | EDIT_MESH;
			}
			REDRAW_SELECTED();
			break;
		case SCIM_ADDFACES:
			if(display->editmode & EDIT_ADDFACES)
			{
				OBJECT *selobj;

				GetActiveObjects(&selobj, TRUE);
				if(selobj)
					selobj->FreeSelectedPoints();
				display->editmode &= ~(EDIT_ADDFACES | EDIT_MESH);
			}
			else
			{
				display->editmode |= EDIT_ADDFACES | EDIT_MESH;
			}
			REDRAW_SELECTED();
			break;
		case SCIM_SELALL:
			sciSelectAll();
			break;
		case SCIM_DESELALL:
			sciDeselectAll();
			break;
		case SCIM_SELNEXT:
			sciSelectNext();
			break;
		case SCIM_SELPREV:
			sciSelectPrev();
			break;
		case SCIM_REDRAW:
			sciRedraw();
			break;
		case SCIM_REDRAW_CURRENT_VIEW:
			sciRedrawCurrentView();
			break;
		case SCIM_CAMERA2VIEWER:
			sciCamera2Viewer(display->view);
			break;
		case SCIM_GROUP:
			return sciGroupObjects();
			break;
		case SCIM_UNGROUP:
			sciUnGroupObjects();
			break;
		case SCIM_ZOOMFIT:
			sciFocus(display->view, TRUE);
			break;
		case SCIM_FOCUS:
			sciFocus(display->view, FALSE);
			break;
		case SCIM_ZOOMIN:
			sciZoomIn(display->view);
			break;
		case SCIM_ZOOMOUT:
			sciZoomOut(display->view);
			break;
		case SCIM_CREATEBOX:
			return sciCreateObject(OBJECT_BOX);
			break;
		case SCIM_CREATESPHERE:
			return sciCreateObject(OBJECT_SPHERE);
			break;
		case SCIM_CREATECYLINDER:
			return sciCreateObject(OBJECT_CYLINDER);
			break;
		case SCIM_CREATECONE:
			return sciCreateObject(OBJECT_CONE);
			break;
		case SCIM_CREATESOR:
			return sciCreateObject(OBJECT_SOR);
			break;
		case SCIM_CREATEPLANE:
			return sciCreateObject(OBJECT_PLANE);
			break;
		case SCIM_CREATEPOINTLIGHT:
			return sciCreateObject(OBJECT_POINTLIGHT);
			break;
		case SCIM_CREATESPOTLIGHT:
			return sciCreateObject(OBJECT_SPOTLIGHT);
			break;
		case SCIM_CREATEDIRECTIONALLIGHT:
			return sciCreateObject(OBJECT_DIRECTIONALLIGHT);
			break;
		case SCIM_CREATECAMERA:
			return sciCreateObject(OBJECT_CAMERA);
			break;
		case SCIM_CREATECSG:
			return sciCreateObject(OBJECT_CSG);
			break;
		case SCIM_CREATEAXIS:
			return sciCreateObject(OBJECT_MESH);
			break;
		case SCIM_MESHJOIN:
			return sciMeshJoin();
		default:
			return FALSE;
	}
	return TRUE;
}

static char *ErrorMessage(rsiCONTEXT *rc, rsiResult err)
{
	static char buffer[256];

	if(rc)
	{
		PPC_STUB(rsiGetErrorMsg)(CTXT, buffer, err);
		PPC_STUB(rsiCleanup)(CTXT);
		PPC_STUB(rsiSetLogCB)(CTXT, (void (*)(void*, char*))NULL);
		PPC_STUB(rsiSetUpdateStatusCB)(CTXT, (void (*)(void*, float, float, int, int, rsiSMALL_COLOR*))NULL);
		PPC_STUB(rsiSetCheckCancelCB)(CTXT, (BOOL (*)(void*))NULL);
		PPC_STUB(rsiSetCooperateCB)(CTXT, (void (*)(void*))NULL);
		PPC_STUB(rsiFreeContext)(CTXT);
		return buffer;
	}
	return NULL;
}

/*************
 * DESCRIPTION:   transfer the data and initialize the render context
 * INPUT:         LogCB
 *                UpdateStatusCB
 *                CheckCancelCB
 *                CooperateCB
 *                data
 *                rc
 * OUTPUT:        error string
 *************/
char *sciRenderInit(void (*LogCB)(void*, char*), void (*UpdateStatusCB)(void*, float, float, int, int, rsiSMALL_COLOR*),
	BOOL (*CheckCancelCB)(void*), void (*CooperateCB)(void*), void *data, rsiCONTEXT **rc)
{
	rsiResult err;
	PREFS p;

	*rc = NULL;
	err = PPC_STUB(rsiCreateContext)(CTXT);
	if(err)
		return ErrorMessage(*rc, err);

	err = PPC_STUB(rsiSetUserData)(*rc, data);
	if(err)
		return ErrorMessage(*rc, err);

	LogCB(*rc, "Transfering data");

#ifdef __MIXEDBINARY__
	err = ppcSetCallbacks(*rc);
	if(err)
		return ErrorMessage(*rc, err);
#else
	err = PPC_STUB(rsiSetLogCB)(*rc, LogCB);
	if(err)
		return ErrorMessage(*rc, err);
	err = PPC_STUB(rsiSetUpdateStatusCB)(*rc, UpdateStatusCB);
	if(err)
		return ErrorMessage(*rc, err);
	err = PPC_STUB(rsiSetCheckCancelCB)(*rc, CheckCancelCB);
	if(err)
		return ErrorMessage(*rc, err);
	err = PPC_STUB(rsiSetCooperateCB)(*rc, CooperateCB);
	if(err)
		return ErrorMessage(*rc, err);
#endif // __MIXEDBINARY__

	err = global.ToRSI(*rc);
	if(err)
		return ErrorMessage(*rc, err);

	p.id = ID_OBJP;
	if(p.GetPrefs())
	{
		err = PPC_STUB(rsiSetObjectPath)(*rc, (char*)p.data);
		if(err)
		{
			p.data = NULL;
			return ErrorMessage(*rc, err);
		}
	}
	p.id = ID_BRSP;
	if(p.GetPrefs())
	{
		err = PPC_STUB(rsiSetBrushPath)(*rc, (char*)p.data);
		if(err)
		{
			p.data = NULL;
			return ErrorMessage(*rc, err);
		}
	}
	p.id = ID_TXTP;
	if(p.GetPrefs())
	{
		err = PPC_STUB(rsiSetTexturePath)(*rc, (char*)p.data);
		if(err)
		{
			p.data = NULL;
			return ErrorMessage(*rc, err);
		}
	}
	p.data = NULL;

	err = ToRSI(*rc, display->camera);
	if(err)
		return ErrorMessage(*rc, err);

	if(global.enablearea)
	{
		err = PPC_STUB(rsiSetRenderField)(*rc, (int)(global.xres * global.xmin),
			(int)(global.yres * global.ymin),
			(int)(global.xres * global.xmax)-1,
			(int)(global.yres * global.ymax)-1);
		if(err)
			return ErrorMessage(*rc, err);
	}
	return NULL;
}

/*************
 * DESCRIPTION:   render the scene
 * INPUT:         rc
 * OUTPUT:        error string
 *************/
char *sciRender(rsiCONTEXT *rc)
{
	ULONG flags, quality;
	PREFS p;
	rsiResult err;
	UBYTE *scr;

	flags = 0;
	if(global.quick)
		flags |= rsiFRenderQuick;
	if(global.randjit)
		flags |= rsiFRenderRandomJitter;

	err = PPC_STUB(rsiRender)(CTXT, &scr,
		rsiTRenderFlags,flags,
		rsiTDone);
	if(err)
	{
		PPC_STUB(rsiCleanup)(CTXT);
		return ErrorMessage(rc, err);
	}

	p.id = ID_JPEG;
	if(p.GetPrefs())
		quality = *(ULONG*)(p.data);
	else
		quality = 70;
	p.data = NULL;

	if(global.renderdpic)
	{
		if(global.renderdpic[0])
		{
			err = PPC_STUB(rsiSavePicture)(CTXT, global.renderdpic,
				rsiTPicType, global.picformat,
				rsiTPicJPEGQuality, quality,
				rsiTDone);
		}
	}
	if(err)
		return ErrorMessage(rc, err);

	err = PPC_STUB(rsiCleanup)(CTXT);
	if(err)
		return ErrorMessage(rc, err);

	// reset callback functions
	PPC_STUB(rsiSetLogCB)(CTXT, (void (*)(void*, char*))NULL);
	PPC_STUB(rsiSetUpdateStatusCB)(CTXT, (void (*)(void*, float, float, int, int, rsiSMALL_COLOR*))NULL);
	PPC_STUB(rsiSetCheckCancelCB)(CTXT, (BOOL (*)(void*))NULL);
	PPC_STUB(rsiSetCooperateCB)(CTXT, (void (*)(void*))NULL);

	PPC_STUB(rsiFreeContext)(CTXT);

	return NULL;
}

