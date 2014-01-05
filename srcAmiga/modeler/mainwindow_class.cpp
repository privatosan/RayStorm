/***************
 * PROGRAM:       Modeler
 * NAME:          mainwindow_class.cpp
 * DESCRIPTION:   class for main window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    11.01.97 ah    added box command
 *    29.01.97 ah    added status bar
 *    30.01.97 ah    added redo command
 *    31.01.97 ah    added pick drag box mode
 *    25.03.97 ah    added cylinder command
 *    09.04.97 ah    added cone and SOR command
 ***************/

#include <string.h>
#include <stdio.h>

#include <intuition/intuition.h>
#include <intuition/imageclass.h>
#include <graphics/gfxbase.h>
#include <libraries/asl.h>
#include <libraries/listtree_mcc.h>
#include <libraries/gadtools.h>

extern struct GfxBase *GfxBase;

#include <pragma/dos_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef MAINWINDOW_CLASS_H
#include "mainwindow_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef BROWSER_CLASS_H
#include "browser_class.h"
#endif

#ifndef BROWSERTREE_CLASS_H
#include "browsertree_class.h"
#endif

#ifndef ANIMTREE_CLASS_H
#include "animtree_class.h"
#endif

#ifndef ANIMBAR_CLASS_H
#include "animbar_class.h"
#endif

#ifndef MESHADDWINDOW_CLASS_H
#include "meshaddwindow_class.h"
#endif

#ifndef MESH_H
#include "mesh.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef UNDO_H
#include "undo.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef KEYFILE_LIB_H
#include "pragma/keyfile_lib.h"
#endif

#ifndef IMAGE_H
#include "image.h"
#endif

#define NOTIFY_MENU(menu, method) DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,menu),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,obj,1,method)
#define DESEL_BUTTON(when, which) DOMETHOD(when,MUIM_Notify,MUIA_Selected,TRUE,which,3,MUIM_Set,MUIA_Selected,FALSE);
#define DISPATCH(func)  case MUIM_MainWindow_ ## func : return( func (cl,obj,msg));
#define MENU_ENABLE(strip, ID, state) SetAttrs((Object *)DOMETHOD(strip, MUIM_FindUData, ID), MUIA_Menuitem_Enabled, state, TAG_DONE);

typedef struct
{
	ULONG menu, method;
} NOTIFY_MSG;

static NOTIFY_MSG notify[] =
{
	{ MEN_NEW,                       MUIM_MainWindow_New },
	{ MEN_OPEN,                      MUIM_MainWindow_Open },
	{ MEN_SAVE,                      MUIM_MainWindow_Save },
	{ MEN_SAVEAS,                    MUIM_MainWindow_SaveAs },
	{ MEN_ABOUT,                     MUIM_MainWindow_About },
	{ MEN_RENDERSETTINGS,            MUIM_MainWindow_RenderSettings },
	{ MEN_RENDER,                    MUIM_MainWindow_Render },
#ifdef __STATISTICS__
	{ MEN_STATISTICS,                MUIM_MainWindow_Statistics }
#endif
	{ MEN_GRIDSIZE,                  MUIM_MainWindow_GridSize },
	{ MEN_SHOWCOORD,                 MUIM_MainWindow_ShowCoord },
	{ MEN_GLOBAL,                    MUIM_MainWindow_Global },
	{ MEN_ATTRIBUTES,                MUIM_MainWindow_Attributes },
	{ MEN_SETTINGS,                  MUIM_MainWindow_Settings },
	{ MEN_LOADOBJ,                   MUIM_MainWindow_LoadObj },
	{ MEN_SAVEOBJ,                   MUIM_MainWindow_SaveObj },
	{ MEN_MESHCUBE,                  MUIM_MainWindow_MeshCube },
	{ MEN_MESHSPHERE,                MUIM_MainWindow_MeshSphere },
	{ MEN_MESHPLANE,                 MUIM_MainWindow_MeshPlane },
	{ MEN_MESHTUBE,                  MUIM_MainWindow_MeshTube },
	{ MEN_MESHCONE,                  MUIM_MainWindow_MeshCone },
	{ MEN_MESHTORUS,                 MUIM_MainWindow_MeshTorus },
	{ MEN_ACTIVECAMERA,              MUIM_MainWindow_ActiveCamera },
	{ MEN_MATERIAL_NEW,              MUIM_MainWindow_MaterialNew },
	{ MEN_MATERIAL_LOADNEW,          MUIM_MainWindow_MaterialLoadNew },
	{ MEN_MATERIAL_REMOVEUNUSED,     MUIM_MainWindow_MaterialRemoveUnused },
	{ MEN_MATERIAL_REMOVEIDENTICAL,  MUIM_MainWindow_MaterialRemoveIdentical },
	{ MEN_PREFS,                     MUIM_MainWindow_Prefs },
	{ MEN_LOADPREFS,                 MUIM_MainWindow_LoadPrefs },
	{ MEN_SAVEPREFS,                 MUIM_MainWindow_SavePrefs },
};

static NOTIFY_MSG scinotify[] =
{
	{ MEN_UNDO,                      SCIM_UNDO },
	{ MEN_REDO,                      SCIM_REDO },
	{ MEN_CUT,                       SCIM_CUT },
	{ MEN_COPY,                      SCIM_COPY },
	{ MEN_PASTE,                     SCIM_PASTE },
	{ MEN_DELETE,                    SCIM_DELETE },
	{ MEN_GRID,                      SCIM_GRIDONOFF },
	{ MEN_GRIDSNAP,                  SCIM_GRIDSNAP },
	{ MEN_SHOWNAMES,                 SCIM_SHOWNAMES },
	{ MEN_SHOWBRUSHES,               SCIM_SHOWBRUSHES },
	{ MEN_SHOWTEXTURES,              SCIM_SHOWTEXTURES },
	{ MEN_SHOWALL,                   SCIM_SHOWALL },
	{ MEN_SHOWCAMERA,                SCIM_SHOWCAMERA },
	{ MEN_SHOWLIGHT,                 SCIM_SHOWLIGHT },
	{ MEN_SHOWMESH,                  SCIM_SHOWMESH },
	{ MEN_SHOWSPHERE,                SCIM_SHOWSPHERE },
	{ MEN_SHOWBOX,                   SCIM_SHOWBOX },
	{ MEN_SHOWCYLINDER,              SCIM_SHOWCYLINDER },
	{ MEN_SHOWCONE,                  SCIM_SHOWCONE },
	{ MEN_SHOWSOR,                   SCIM_SHOWSOR },
	{ MEN_SHOWPLANE,                 SCIM_SHOWPLANE },
	{ MEN_SHOWCSG,                   SCIM_SHOWCSG },
	{ MEN_SELALL,                    SCIM_SELALL },
	{ MEN_DESELALL,                  SCIM_DESELALL },
	{ MEN_SELNEXT,                   SCIM_SELNEXT },
	{ MEN_SELPREV,                   SCIM_SELPREV },
	{ MEN_REDRAW,                    SCIM_REDRAW },
	{ MEN_CAMERA2VIEWER,             SCIM_CAMERA2VIEWER },
	{ MEN_GROUP,                     SCIM_GROUP },
	{ MEN_UNGROUP,                   SCIM_UNGROUP },
	{ MEN_FOCUS,                     SCIM_FOCUS },
	{ MEN_ZOOMIN,                    SCIM_ZOOMIN },
	{ MEN_ZOOMOUT,                   SCIM_ZOOMOUT },
	{ MEN_ZOOMFIT,                   SCIM_ZOOMFIT },
	{ MEN_SPHERE,                    SCIM_CREATESPHERE },
	{ MEN_BOX,                       SCIM_CREATEBOX },
	{ MEN_CYLINDER,                  SCIM_CREATECYLINDER },
	{ MEN_CONE,                      SCIM_CREATECONE },
	{ MEN_SOR,                       SCIM_CREATESOR },
	{ MEN_PLANE,                     SCIM_CREATEPLANE },
	{ MEN_POINTLIGHT,                SCIM_CREATEPOINTLIGHT },
	{ MEN_SPOTLIGHT,                 SCIM_CREATESPOTLIGHT },
	{ MEN_DIRECTIONALLIGHT,          SCIM_CREATEDIRECTIONALLIGHT },
	{ MEN_CAMERA,                    SCIM_CREATECAMERA },
	{ MEN_CSG,                       SCIM_CREATECSG },
	{ MEN_AXIS,                      SCIM_CREATEAXIS },
	{ MEN_MESH_JOIN,                 SCIM_MESHJOIN },
};

/*************
 * DESCRIPTION:   sets the windowtitle with the project name. If NULL
 *    is specified as project the title is changed to 'untitled'.
 * INPUT:         obj      window object
 *                project  project name
 * OUTPUT:        none
 *************/
static void SetWindowTitle(Object *obj, char *project)
{
	static char buffer[256];

	sprintf(buffer, "RayStorm Scenario %s <", __VERS__);


	if(!project)
		strcat(buffer, "untitled>");
	else
	{
		strcat(buffer, project);
		strcat(buffer, ">");
	}
	SetAttrs(obj, MUIA_Window_Title, buffer, TAG_DONE);
}

/*************
 * DESCRIPTION:   sets the window buttons
 * INPUT:         system
 * OUTPUT:        none
 *************/
static void SetButtons(struct IClass *cl, Object *obj)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	DISPLAY *disp;

	GetAttr(MUIA_Draw_DisplayPtr, data->drawarea, (ULONG*)&disp);

	// update display, viewmode, grid, gridsnap
	SetAttrs(data->drawarea, MUIA_Draw_SetUpdate, FALSE, TAG_DONE);
	DOMETHOD(data->beditpoints, MUIM_NoNotifySet,
		MUIA_Selected, disp->editmode & EDIT_EDITPOINTS,
		MUIA_Disabled, !(disp->editmode & EDIT_MESH),
		TAG_DONE);
	DOMETHOD(data->baddpoints, MUIM_NoNotifySet,
		MUIA_Selected, disp->editmode & EDIT_ADDPOINTS,
		MUIA_Disabled, !(disp->editmode & EDIT_MESH),
		TAG_DONE);
	DOMETHOD(data->beditedges, MUIM_NoNotifySet,
		MUIA_Selected, disp->editmode & EDIT_EDITEDGES,
		MUIA_Disabled, !(disp->editmode & EDIT_MESH),
		TAG_DONE);
	DOMETHOD(data->baddedges, MUIM_NoNotifySet,
		MUIA_Selected, disp->editmode & EDIT_ADDEDGES,
		MUIA_Disabled, !(disp->editmode & EDIT_MESH),
		TAG_DONE);
	DOMETHOD(data->beditfaces, MUIM_NoNotifySet,
		MUIA_Selected, disp->editmode & EDIT_EDITFACES,
		MUIA_Disabled, !(disp->editmode & EDIT_MESH),
		TAG_DONE);
	DOMETHOD(data->baddfaces, MUIM_NoNotifySet,
		MUIA_Selected, disp->editmode & EDIT_ADDFACES,
		MUIA_Disabled, !(disp->editmode & EDIT_MESH),
		TAG_DONE);
	switch(disp->display)
	{
		case DISPLAY_BBOX:
			SetAttrs(data->bbou, MUIA_Selected, TRUE, TAG_DONE);
			break;
		case DISPLAY_WIRE:
			SetAttrs(data->bwir, MUIA_Selected, TRUE, TAG_DONE);
			break;
		case DISPLAY_SOLID:
			SetAttrs(data->bsol, MUIA_Selected, TRUE, TAG_DONE);
			break;
	}
	switch(disp->view->viewmode)
	{
		case VIEW_FRONT:
			SetAttrs(data->bfro, MUIA_Selected, TRUE, TAG_DONE);
			break;
		case VIEW_RIGHT:
			SetAttrs(data->brig, MUIA_Selected, TRUE, TAG_DONE);
			break;
		case VIEW_TOP:
			SetAttrs(data->btop, MUIA_Selected, TRUE, TAG_DONE);
			break;
		case VIEW_PERSP:
			SetAttrs(data->bper, MUIA_Selected, TRUE, TAG_DONE);
			break;
		case VIEW_CAMERA:
			SetAttrs(data->bcam, MUIA_Selected, TRUE, TAG_DONE);
			break;
	}
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWNAMES),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->name_disp, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWCOORD),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->coord_disp, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWCAMERA),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_CAMERA, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWLIGHT),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_LIGHT, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWMESH),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_MESH, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWSPHERE),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_SPHERE, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWBOX),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_BOX, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWCYLINDER),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_CYLINDER, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWCONE),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_CONE, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWSOR),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_SOR, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWPLANE),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_PLANE, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWCSG),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_CSG, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWBRUSHES),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_BRUSH, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData, MEN_SHOWTEXTURES),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked,disp->filter_flags & FILTER_TEXTURE, TAG_DONE);

	DOMETHOD(data->bfor, MUIM_NoNotifySet, MUIA_Selected, disp->multiview, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_GRID),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked, disp->grid, TAG_DONE);
	DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_GRIDSNAP),
		MUIM_NoNotifySet, MUIA_Menuitem_Checked, disp->gridsnap, TAG_DONE);

	SetAttrs(data->drawarea, MUIA_Draw_SetUpdate, TRUE, TAG_DONE);

	if(disp->view->viewmode == VIEW_CAMERA)
	{
		utility.EnableMenuItem(MENU_SETRENDERWINDOW, TRUE);
		disp->view->SetCamera(disp->camera);
	}
	else
	{
		utility.EnableMenuItem(MENU_SETRENDERWINDOW, FALSE);
		disp->view->SetVirtualCamera();
	}

	DeselectAll();
}

/*************
 * FUNCTION:
 * DESCRIPTION:   menu functions
 * INPUT:
 * OUTPUT:
 *************/
static ULONG New(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);

	if(GetProjectChanged())
	{
		if(!utility.AskRequest("Really (changes will be lost) ?"))
			return 0;
	}

	SetWindowTitle(obj,NULL);

	sciNew();

	MENU_ENABLE(data->strip, MEN_SAVE, FALSE);
	SetButtons(cl, obj);

	return 0;
}

/*************
 * DESCRIPTION:   Opens a project. Also inserts recent file menu item.
 * INPUT:         cl, obj
 *                dir      project to load
 * OUTPUT:        -
 *************/
static void OpenProject(struct IClass *cl,Object *obj, char *dir)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	char *err;
	Object *item;
	int len;
	int filenum;

	SetSleep(TRUE);

	err = sciOpen(dir);
	if(err)
		utility.Request(err);

	SetWindowTitle(obj, dir);

	MENU_ENABLE(data->strip, MEN_SAVE, TRUE);

	SetButtons(cl, obj);

	if(data->filenum == 0)
	{
		// first item -> make seperator
		data->filesep = MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, NULL, 0, MEN_FILESEP);
		DOMETHOD(data->strip, MUIM_Family_Insert, data->filesep, (Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_RENDER));
	}

	if(data->filenum == 4)
	{
		strcpy(data->file[3], data->file[2]);
		strcpy(data->filemenu[3], data->filemenu[2]);
		strcpy(data->file[2], data->file[1]);
		strcpy(data->filemenu[2], data->filemenu[1]);
		strcpy(data->file[1], data->file[0]);
		strcpy(data->filemenu[1], data->filemenu[0]);
	}

	filenum = 3-data->filenum;
	if(filenum < 0)
		filenum = 0;
	strcpy(data->file[filenum], dir);

	len = strlen(dir)-20;
	if(len>=0)
		sprintf(data->filemenu[filenum], "...%s", &dir[len]);
	else
		strcpy(data->filemenu[filenum], dir);

	if(data->filenum <= 3)
	{
		item = MUI_MakeObject(MUIO_Menuitem, data->filemenu[3-data->filenum], NULL, 0, MEN_FILE4-data->filenum);
		DOMETHOD(data->strip, MUIM_Family_Insert, item, data->filesep);
		NOTIFY_MENU(MEN_FILE4-data->filenum, MUIM_MainWindow_File4-data->filenum);
		data->filenum++;
	}

	SetSleep(FALSE);
}

static ULONG Open(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	char dir[256];

	if(GetProjectChanged())
	{
		if(!utility.AskRequest("Really open new project\n(changes will be lost) ?"))
			return 0;
	}

	strcpy(dir, "project.scn");
	if(utility.FileRequ(dir, FILEREQU_SCENE, FILEREQU_INITIALFILE))
	{
		OpenProject(cl, obj, dir);
	}

	return 0;
}

static ULONG Save(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	char *err, buffer[256];

	if(!IsTitled())
	{
		// no previously saved object -> call 'save as'
		DOMETHOD(obj, MUIM_MainWindow_SaveAs);
	}
	else
	{
		SetSleep(TRUE);

		// save project
		strcpy(buffer, GetProjectName());
		err = sciSave(buffer);
		if(err)
			utility.Request(err);

		SetSleep(FALSE);
	}

	return 0;
}

static ULONG SaveAs(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	char dir[256], *err;

	if(utility.FileRequ(dir, FILEREQU_SCENE, FILEREQU_SAVE))
	{
		SetSleep(TRUE);

		// save project
		err = sciSave(dir);
		if(err)
			utility.Request(err);

		MENU_ENABLE(data->strip, MEN_SAVE, TRUE);

		SetWindowTitle(obj,FilePart(dir));

		SetSleep(FALSE);
	}

	return 0;
}

static ULONG RenderSettings(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	DISPLAY *disp;

	if(data->rendsetwin)
	{
		// window already open -> pop it to front
		DOMETHOD(data->rendsetwin, MUIM_Window_ToFront);
		SetAttrs(data->rendsetwin, MUIA_Window_Activate, TRUE, TAG_DONE);
	}
	else
	{
		// window not open -> open it
		GetAttr(MUIA_Draw_DisplayPtr,data->drawarea,(ULONG*)&disp);

		// create render settings window
		data->rendsetwin = (Object*)NewObject(CL_RenderWindow->mcc_Class,NULL,
			MUIA_ChildWindow_Originator,obj,
			MUIA_RenderWindow_Camera, disp->camera,
			TAG_DONE);
		if(data->rendsetwin)
		{
			DOMETHOD(app, OM_ADDMEMBER, data->rendsetwin);
			SetAttrs(data->rendsetwin, MUIA_Window_Open, TRUE, TAG_DONE);
		}
	}
	return 0;
}

static ULONG Render(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	DISPLAY *disp;
	Object *newobj;

	if(!global.renderdpic)
	{
		utility.Request("Please specify a name for the output picture.");
		return FALSE;
	}

	if(!sciTestForLight())
	{
		if(!utility.AskRequest("There are no light sources in this scene.\nRender really?"))
			return FALSE;
	}

	GetAttr(MUIA_Draw_DisplayPtr,data->drawarea,(ULONG*)&disp);

	// create render window
	newobj = (Object*)NewObject(CL_StateWindow->mcc_Class, NULL,
		MUIA_ChildWindow_Originator, obj,
		MUIA_StateWindow_Display, disp, TAG_DONE);
	if(newobj)
	{
		DOMETHOD(app,OM_ADDMEMBER,newobj);
		SetAttrs(newobj,MUIA_Window_Open,TRUE, TAG_DONE);
		DOMETHOD(newobj,MUIM_StateWindow_Render);
	}

	return TRUE;
}

static ULONG File4(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	char buffer[256];

	strcpy(buffer, data->file[3]);
	OpenProject(cl, obj, buffer);
	return 0;
}

static ULONG File3(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	char buffer[256];

	strcpy(buffer, data->file[2]);
	OpenProject(cl, obj, buffer);
	return 0;
}

static ULONG File2(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	char buffer[256];

	strcpy(buffer, data->file[1]);
	OpenProject(cl, obj, buffer);
	return 0;
}

static ULONG File1(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	char buffer[256];

	strcpy(buffer, data->file[0]);
	OpenProject(cl, obj, buffer);
	return 0;
}

#ifdef __STATISTICS__
static ULONG Statistics(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	Object *newobj;

	// create statistics window
	newobj = (Object*)NewObject(CL_Statistics->mcc_Class, NULL,
		MUIA_ChildWindow_Originator, obj,TAG_DONE);
	if(newobj)
	{
		DOMETHOD(app,OM_ADDMEMBER,newobj);
		SetSleep(TRUE);
		SetAttrs(newobj,MUIA_Window_Open,TRUE, TAG_DONE);
	}

	return 0;
}
#endif

static ULONG About(struct IClass *cl,Object *obj,Msg msg)
{
	char buffer[256];
	char name[32];
	ULONG sernum;

#ifdef __020__
	sprintf(buffer, "\33b\33c%s 68020\33n\nVersion %s\n%s\nARexx-Port: %s\n\n",
		((char *)xget(app,MUIA_Application_Title)),
		((char *)xget(app,MUIA_Application_Version)),
		((char *)xget(app,MUIA_Application_Copyright)),
		((char *)xget(app,MUIA_Application_Base)));
#endif
#ifdef __881__
	sprintf(buffer, "\33c\33b%s 68020/881\33n\nVersion %s\n%s\nARexx-Port: %s\n\n",
		((char *)xget(app,MUIA_Application_Title)),
		((char *)xget(app,MUIA_Application_Version)),
		((char *)xget(app,MUIA_Application_Copyright)),
		((char *)xget(app,MUIA_Application_Base)));
#endif
#ifdef __040__
	sprintf(buffer, "\33b\33c%s 68040\33n\nVersion %s\n%s\nARexx-Port: %s\n\n",
		((char *)xget(app,MUIA_Application_Title)),
		((char *)xget(app,MUIA_Application_Version)),
		((char *)xget(app,MUIA_Application_Copyright)),
		((char *)xget(app,MUIA_Application_Base)));
#endif
#ifdef __060__
	sprintf(buffer, "\33b\33c%s 68060\33n\nVersion %s\n%s\nARexx-Port: %s\n\n",
		((char *)xget(app,MUIA_Application_Title)),
		((char *)xget(app,MUIA_Application_Version)),
		((char *)xget(app,MUIA_Application_Copyright)),
		((char *)xget(app,MUIA_Application_Base)));
#endif
#ifdef __MIXEDBINARY__
	sprintf(buffer, "\33b\33c%s PowerPC\33n\nVersion %s\n%s\nARexx-Port: %s\n\n",
		((char *)xget(app,MUIA_Application_Title)),
		((char *)xget(app,MUIA_Application_Version)),
		((char *)xget(app,MUIA_Application_Copyright)),
		((char *)xget(app,MUIA_Application_Base)));
#endif
	if(KeyfileBase)
	{
		keyGetInfo(name, &sernum);

		sprintf(buffer, "%sRegistered for %s\nSerial Number: %d\nThank you for purchasing RayStorm.",
			buffer,
			name,
			(int)sernum);
	}
	else
	{
		strcat(buffer, "Unregistered Version!");
	}

	utility.Request(buffer);

	return 0;
}

static ULONG GridSize(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	float size;

	if(data->gridsizewin)
	{
		// window already open -> pop it to front
		DOMETHOD(data->gridsizewin, MUIM_Window_ToFront);
		SetAttrs(data->gridsizewin, MUIA_Window_Activate, TRUE, TAG_DONE);
	}
	else
	{
		// window not open -> open it
		GetAttr(MUIA_Draw_GridSize, data->drawarea, (ULONG*)&size);

		data->gridsizewin = (Object*)NewObject(CL_NumReq->mcc_Class,NULL,
			MUIA_ChildWindow_Originator, obj,
			MUIA_NumReq_Text, "Grid size",
			MUIA_NumReq_Value, &size,
			TAG_DONE);
		if(data->gridsizewin)
		{
			DOMETHOD(app, OM_ADDMEMBER, data->gridsizewin);
			SetAttrs(data->gridsizewin, MUIA_Window_Open, TRUE, TAG_DONE);
		}
	}
	return 0;
}

static ULONG ShowCoord(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	ULONG t;
	Object *o;

	o = (Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_SHOWCOORD);
	GetAttr(MUIA_Menuitem_Checked,o,(ULONG*)&t);
	SetAttrs(data->drawarea,MUIA_Draw_CoordDisp,t, TAG_DONE);

	return 0;
}

static ULONG Global(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);

	if(data->globalwin)
	{
		// window already open -> pop it to front
		DOMETHOD(data->globalwin, MUIM_Window_ToFront);
		SetAttrs(data->globalwin, MUIA_Window_Activate, TRUE, TAG_DONE);
	}
	else
	{
		// window not open -> open it
		data->globalwin = (Object*)NewObject(CL_GlobalWindow->mcc_Class,NULL,
			MUIA_ChildWindow_Originator,obj,TAG_DONE);
		if(data->globalwin)
		{
			DOMETHOD(app, OM_ADDMEMBER, data->globalwin);
			SetAttrs(data->globalwin, MUIA_Window_Open, TRUE, TAG_DONE);
		}
	}
	return 0;
}

static ULONG Attributes(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	Object *newobj;
	OBJECT *object;

	if(GetActiveObjects(&object, TRUE))
	{
		// create browser window
		newobj = (Object*)NewObject(CL_Browser->mcc_Class,NULL,
			MUIA_ChildWindow_Originator,obj,
			MUIA_Browser_Mode,BROWSER_SELECTEDSURFACE,
			MUIA_Browser_DrawArea, data->drawarea,
			TAG_DONE);
		if(newobj)
		{
			SetSleep(TRUE);
			DOMETHOD(app,OM_ADDMEMBER,newobj);
			SetAttrs(newobj,MUIA_Window_Open,TRUE, TAG_DONE);
		}
	}
	else
	{
		if(!object || !object->surf)
		{
			DisplayBeep(_screen(obj));
			return 0;
		}
		if(object->surf->win)
		{
			// requester already open -> pop to front
			DOMETHOD(object->surf->win, MUIM_Window_ToFront);
			SetAttrs(object->surf->win, MUIA_Window_Activate, TRUE, TAG_DONE);
		}
		else
		{
			// create attributes window
			newobj = (Object*)NewObject(CL_AttrsWindow->mcc_Class,NULL,
				MUIA_ChildWindow_Originator, obj,
				MUIA_AttrWindow_Object, object,
				MUIA_AttrWindow_Surface, object->surf,
				TAG_DONE);
			if(newobj)
			{
				DOMETHOD(app, OM_ADDMEMBER, newobj);
				object->surf->win = newobj;
				SetAttrs(newobj, MUIA_Window_Open,TRUE, TAG_DONE);
				DOMETHOD(newobj, MUIM_AttrWindow_Preview);
			}
		}
	}

	return 0;
}

static ULONG OpenMaterialRequ(struct IClass *cl,Object *obj,MUIP_Material_Material *msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);

	if(msg->surf->win)
	{
			// requester already open -> pop to front
		DOMETHOD(msg->surf->win, MUIM_Window_ToFront);
		SetAttrs(msg->surf->win, MUIA_Window_Activate, TRUE, TAG_DONE);
	}
	else
	{
		// create attributes window
		msg->surf->win = (Object*)NewObject(CL_AttrsWindow->mcc_Class,NULL,
			MUIA_ChildWindow_Originator, obj,
			MUIA_AttrWindow_Surface, msg->surf,
			TAG_DONE);
		if(msg->surf->win)
		{
			DOMETHOD(app, OM_ADDMEMBER, msg->surf->win);
			SetAttrs(msg->surf->win, MUIA_Window_Open, TRUE, TAG_DONE);
			DOMETHOD(msg->surf->win, MUIM_AttrWindow_Preview);
		}
	}

	return 0;
}

static ULONG OpenPlugin(struct IClass *cl,Object *obj, MUIP_MainWindow_OpenPlugin *msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	Object *win;

	// create attributes window
	win = (Object*)NewObject(CL_PluginWindow->mcc_Class,NULL,
		MUIA_ChildWindow_Originator, obj,
		MUIA_PluginWindow_Info, msg->plugin->info,
		MUIA_PluginWindow_Data, msg->plugin->data,
		TAG_DONE);
	if(win)
	{
		DOMETHOD(app, OM_ADDMEMBER, win);
		SetAttrs(win, MUIA_Window_Open, TRUE, TAG_DONE);
	}

	return 0;
}

static ULONG Settings(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);

	DOMETHOD(data->drawarea,MUIM_Draw_OpenSettings);
	return 0;
}

static ULONG LoadObj(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	char buffer[256];

	if(utility.FileRequ(buffer, FILEREQU_OBJECT, 0))
	{
		SetSleep(TRUE);

		sciLoadObject(buffer);

		SetSleep(FALSE);
	}

	return 0;
}

static ULONG SaveObj(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	char buffer[256], *err;

	err = TestForSaveObject();
	if(err)
	{
		utility.Request(err);
		return 0;
	}

	if(utility.FileRequ(buffer, FILEREQU_OBJECT, FILEREQU_SAVE))
	{
		SetSleep(TRUE);

		sciSaveObject(buffer);

		SetSleep(FALSE);
	}

	return 0;
}

static ULONG MeshRequest(struct IClass *cl, Object *obj, int type)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	Object *newobj;

	// create mesh add window
	newobj = (Object*)NewObject(CL_MeshAddWindow->mcc_Class,NULL,
		MUIA_ChildWindow_Originator,obj,
		MUIA_MeshAddWindow_Type, type, TAG_DONE);
	if(newobj)
	{
		SetSleep(TRUE);
		DOMETHOD(app, OM_ADDMEMBER, newobj);
		SetAttrs(newobj, MUIA_Window_Open, TRUE, TAG_DONE);
	}

	return 0;
}

static ULONG MeshCube(struct IClass *cl,Object *obj,Msg msg)
{
	return MeshRequest(cl, obj, MESHADD_CUBE);
}

static ULONG MeshSphere(struct IClass *cl,Object *obj,Msg msg)
{
	return MeshRequest(cl, obj, MESHADD_SPHERE);
}

static ULONG MeshPlane(struct IClass *cl,Object *obj,Msg msg)
{
	return MeshRequest(cl, obj, MESHADD_PLANE);
}

static ULONG MeshTube(struct IClass *cl,Object *obj,Msg msg)
{
	return MeshRequest(cl, obj, MESHADD_TUBE);
}

static ULONG MeshCone(struct IClass *cl,Object *obj,Msg msg)
{
	return MeshRequest(cl, obj, MESHADD_CONE);
}

static ULONG MeshTorus(struct IClass *cl,Object *obj,Msg msg)
{
	return MeshRequest(cl, obj, MESHADD_TORUS);
}

static ULONG ActiveCamera(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	Object *newobj;

	// create browser window
	newobj = (Object*)NewObject(CL_Browser->mcc_Class,NULL,
		MUIA_ChildWindow_Originator,obj,
		MUIA_Browser_Mode,BROWSER_CAMERAS,
		MUIA_Browser_DrawArea, data->drawarea,
		TAG_DONE);
	if(newobj)
	{
		SetSleep(TRUE);
		DOMETHOD(app,OM_ADDMEMBER,newobj);
		SetAttrs(newobj,MUIA_Window_Open,TRUE, TAG_DONE);
	}
	return 0;
}

static ULONG MaterialNew(struct IClass *cl,Object *obj,Msg msg)
{
	SURFACE *surf;

	surf = sciCreateSurface(SURF_DEFAULTNAME);
	if(surf)
	{
		sciAddMaterial(surf);
		return TRUE;
	}

	return FALSE;
}

static ULONG MaterialLoadNew(struct IClass *cl,Object *obj,Msg msg)
{
	SURFACE *surf;

	surf = sciCreateSurface(SURF_DEFAULTNAME);
	if(surf)
	{
		if(sciLoadMaterial(surf))
		{
			sciAddMaterial(surf);
			return TRUE;
		}
		else
			sciDeleteSurface(surf);
	}

	return FALSE;
}

static ULONG MaterialRemoveUnused(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);

	return DOMETHOD(data->material, MUIM_Material_RemoveUnused);
}

static ULONG MaterialRemoveIdentical(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);

	return DOMETHOD(data->material, MUIM_Material_RemoveIdentical);
}

static ULONG Prefs(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	Object *newobj;
	DISPLAY *disp;

	GetAttr(MUIA_Draw_DisplayPtr,data->drawarea,(ULONG*)&disp);

	// create preference window
	newobj = (Object*)NewObject(CL_PrefsWindow->mcc_Class,NULL,
		MUIA_ChildWindow_Originator,obj,
		MUIA_PrefWindow_Display, disp,
		MUIA_PrefWindow_DrawArea, data->drawarea,
		TAG_DONE);
	if(newobj)
	{
		SetSleep(TRUE);
		DOMETHOD(app,OM_ADDMEMBER,newobj);
		SetAttrs(newobj,MUIA_Window_Open,TRUE, TAG_DONE);
	}
	return 0;
}

static ULONG LoadPrefs(struct IClass *cl,Object *obj,Msg msg)
{
	if(!sciLoadPrefs())
		utility.Request("Unable to load preferences");

	return 0;
}

static ULONG SavePrefs(struct IClass *cl,Object *obj,Msg msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	PREFS p;
	ULONG weight[4];

	GetAttr(MUIA_HorizWeight, data->drawarea, (ULONG*)&weight[0]);
	GetAttr(MUIA_HorizWeight, data->browsergroup, (ULONG*)&weight[1]);
	GetAttr(MUIA_VertWeight, data->middlegroup, (ULONG*)&weight[2]);
	GetAttr(MUIA_VertWeight, data->materialgroup, (ULONG*)&weight[3]);

	p.id = ID_WGHT;
	p.data = weight;
	p.length = 4*sizeof(ULONG);
	p.AddPrefs();

	p.data = NULL;

	if(!sciSavePrefs())
		utility.Request("Unable to save preferences");

	return 0;
}

/*************
 * DESCRIPTION:   do initializations for main window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct MainWindow_Data *data,databuf;
	PREFS p;
	Object *toolbar;
	int xres, yres;
	int flags, i;
	ULONG weight[4];
	IMAGE image;
	BOOL opengl_supported;

	p.id = ID_FLGS;
	if(p.GetPrefs())
		flags = *((int*)p.data);
	else
		flags = 0;

	p.id = ID_WGHT;
	if(p.GetPrefs())
	{
		weight[0] = ((ULONG*)p.data)[0];
		weight[1] = ((ULONG*)p.data)[1];
		weight[2] = ((ULONG*)p.data)[2];
		weight[3] = ((ULONG*)p.data)[3];
	}
	else
	{
		weight[0] = 100;
		weight[1] = 20;
		weight[2] = 100;
		weight[3] = 20;
	}

	p.data = NULL;

	opengl_supported = (BOOL)GetTagData(MUIA_OpenGL_Supported, 0, msg->ops_AttrList);

	if(GfxBase->LibNode.lib_Version<39 || !(flags & PREFFLAG_EXTIMAGES))
	{
		// use buildin images
		toolbar = HGroup,
			MUIA_Group_HorizSpacing, 2,
			Child, databuf.bnew = ImageButton1(newimage),
				MUIA_ShortHelp, "New project",
			End,
			Child, databuf.bloa = ImageButton1(loadimage),
				MUIA_ShortHelp, "Load Project",
			End,
			Child, databuf.bsav = ImageButton1(saveimage),
				MUIA_ShortHelp, "Save project",
			End,
			Child, HSpace(5),
			Child, VGroup,
				Child, HGroup,
					Child, databuf.beditpoints = ImageButton(editpointsimage),
						MUIA_InputMode, MUIV_InputMode_Toggle,
						MUIA_Disabled, TRUE,
						MUIA_ShortHelp, "Edit points of mesh objects",
					End,
					Child, databuf.beditedges = ImageButton(editedgesimage),
						MUIA_InputMode, MUIV_InputMode_Toggle,
						MUIA_Disabled, TRUE,
						MUIA_ShortHelp, "Edit edges of mesh objects",
					End,
					Child, databuf.beditfaces = ImageButton(editfacesimage),
						MUIA_InputMode, MUIV_InputMode_Toggle,
						MUIA_Disabled, TRUE,
						MUIA_ShortHelp, "Edit faces of mesh objects",
					End,
				End,
				Child, HGroup,
					Child, databuf.baddpoints = ImageButton(addpointsimage),
						MUIA_InputMode, MUIV_InputMode_Toggle,
						MUIA_Disabled, TRUE,
						MUIA_ShortHelp, "Add points to a mesh object",
					End,
					Child, databuf.baddedges = ImageButton(addedgesimage),
						MUIA_InputMode, MUIV_InputMode_Toggle,
						MUIA_Disabled, TRUE,
						MUIA_ShortHelp, "Add edges to a mesh object",
					End,
					Child, databuf.baddfaces = ImageButton(addfacesimage),
						MUIA_InputMode, MUIV_InputMode_Toggle,
						MUIA_Disabled, TRUE,
						MUIA_ShortHelp, "Add faces to a mesh object",
					End,
				End,
			End,
			Child, HSpace(5),
			Child, VGroup,
				Child, HGroup,
					Child, databuf.btop = ImageButton(topimage),
						MUIA_ShortHelp, "Top view",
					End,
					Child, databuf.bper = ImageButton(perspimage),
						MUIA_Selected,TRUE,
						MUIA_ShortHelp, "Perspective view",
					End,
					Child, databuf.bcam = ImageButton(cameraimage),
						MUIA_ShortHelp, "Camera view",
					End,
				End,
				Child, HGroup,
					Child, databuf.bfro = ImageButton(frontimage),
						MUIA_ShortHelp, "Front view",
					End,
					Child, databuf.brig = ImageButton(rightimage),
						MUIA_ShortHelp, "Right view",
					End,
					Child, databuf.bfor = ImageButton(fourimage),
						MUIA_InputMode, MUIV_InputMode_Toggle,
						MUIA_ShortHelp, "Quad view",
					End,
				End,
			End,
			Child, HSpace(10),
			Child, databuf.bbou = ImageButton(boundingimage),
				MUIA_ShortHelp, "Boundingbox display",
			End,
			Child, databuf.bwir = ImageButton(wireimage),
				MUIA_Selected,TRUE,
				MUIA_ShortHelp, "Wireframe display",
			End,
#ifdef __OPENGL__
			Child, databuf.bsol = ImageButton(solidimage),
				MUIA_ShortHelp, "Solid display",
				MUIA_Disabled, !opengl_supported,
			End,
#endif // __OPENGL__
			Child, HSpace(5),
			Child, databuf.bloc = ImageButton(localimage),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Edit local",
			End,
			Child, HSpace(5),
			Child, VGroup,
				Child, HGroup,
					Child, databuf.bmov = ImageButton(moveimage),
						MUIA_ShortHelp, "Move object",
					End,
					Child, databuf.brot = ImageButton(rotateimage),
						MUIA_ShortHelp, "Rotate object",
					End,
					Child, databuf.bsca = ImageButton(scaleimage),
						MUIA_ShortHelp, "Scale object",
					End,
				End,
				Child, HGroup,
					Child, databuf.bwmov = ImageButton(worldmoveimage),
						MUIA_ShortHelp, "Move World",
					End,
					Child, databuf.bwrot = ImageButton(worldrotateimage),
						MUIA_Selected, TRUE,
						MUIA_ShortHelp, "Rotate world",
					End,
					Child, databuf.bwsca = ImageButton(worldscaleimage),
						MUIA_ShortHelp, "Scale world",
					End,
				End,
			End,
			Child, HSpace(5),
			Child, databuf.brect = ImageButton(rectangleimage),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Select objects with drag box",
			End,
			Child, HSpace(5),
			Child, databuf.bx = ImageButton(ximage),
				MUIA_Selected,TRUE,
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Toggle active x-axis",
			End,
			Child, databuf.by = ImageButton(yimage),
				MUIA_Selected,TRUE,
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Toggle active y-axis",
			End,
			Child, databuf.bz = ImageButton(zimage),
				MUIA_Selected,TRUE,MUIA_InputMode,
				MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Toggle active z-axis",
			End,
			Child, HSpace(0),
		End;
	}
	else
	{
		if(image.Load("buttons/new.iff", NULL))
		{
			xres = image.width;
			yres = image.height;
		}
		else
		{
			xres = 16;
			yres = 16;
		}
		// load images
		toolbar = HGroup,
			MUIA_Group_HorizSpacing, 2,
			Child, databuf.bnew = ImageButtonExt("5:buttons/new.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_RelVerify,
				MUIA_ShortHelp, "New project",
			End,
			Child, databuf.bloa = ImageButtonExt("5:buttons/load.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_RelVerify,
				MUIA_ShortHelp, "Load Project",
			End,
			Child, databuf.bsav = ImageButtonExt("5:buttons/save.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_RelVerify,
				MUIA_ShortHelp, "Save project",
			End,
			Child, HSpace(5),
			Child, databuf.beditpoints = ImageButtonExt("5:buttons/editpoint.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_Disabled, TRUE,
				MUIA_ShortHelp, "Edit points of mesh objects",
			End,
			Child, databuf.baddpoints = ImageButtonExt("5:buttons/addpoint.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_Disabled, TRUE,
				MUIA_ShortHelp, "Add points to a mesh object",
			End,
			Child, databuf.beditedges = ImageButtonExt("5:buttons/editedge.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_Disabled, TRUE,
				MUIA_ShortHelp, "Edit edges of a mesh object",
			End,
			Child, databuf.baddedges = ImageButtonExt("5:buttons/addedge.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_Disabled, TRUE,
				MUIA_ShortHelp, "Add edges to a mesh object",
			End,
			Child, databuf.beditfaces = ImageButtonExt("5:buttons/editface.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_Disabled, TRUE,
				MUIA_ShortHelp, "Edit faces of a mesh object",
			End,
			Child, databuf.baddfaces = ImageButtonExt("5:buttons/addface.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_Disabled, TRUE,
				MUIA_ShortHelp, "Add faces to a mesh object",
			End,
			Child, HSpace(5),
			Child, databuf.bfro = ImageButtonExt("5:buttons/front.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Front view",
			End,
			Child, databuf.brig = ImageButtonExt("5:buttons/right.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Right view",
			End,
			Child, databuf.btop = ImageButtonExt("5:buttons/top.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Top view",
			End,
			Child, databuf.bper = ImageButtonExt("5:buttons/perspective.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_Selected,TRUE,
				MUIA_ShortHelp, "Perspective view",
			End,
			Child, databuf.bcam = ImageButtonExt("5:buttons/camera.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Camera view",
			End,
			Child, databuf.bfor = ImageButtonExt("5:buttons/quad.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Quad view",
			End,
			Child, HSpace(5),
			Child, databuf.bbou = ImageButtonExt("5:buttons/boundingbox.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Boundingbox display",
			End,
			Child, databuf.bwir = ImageButtonExt("5:buttons/wireframe.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_Selected,TRUE,
				MUIA_ShortHelp, "Wireframe display",
			End,
#ifdef __OPENGL__
			Child, databuf.bsol = ImageButtonExt("5:buttons/solid.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Solid display",
			End,
#endif // __OPENGL__
			Child, HSpace(5),
			Child, databuf.bloc = ImageButtonExt("5:buttons/local.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Edit local",
			End,
			Child, HSpace(5),
			Child, databuf.bmov = ImageButtonExt("5:buttons/move.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Move object",
			End,
			Child, databuf.brot = ImageButtonExt("5:buttons/rotate.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Rotate object",
			End,
			Child, databuf.bsca = ImageButtonExt("5:buttons/scale.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Scale object",
			End,
			Child, databuf.bwmov = ImageButtonExt("5:buttons/wmove.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Move World",
			End,
			Child, databuf.bwrot = ImageButtonExt("5:buttons/wrotate.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_Selected, TRUE,
				MUIA_ShortHelp, "Rotate world",
			End,
			Child, databuf.bwsca = ImageButtonExt("5:buttons/wscale.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Immediate,
				MUIA_ShortHelp, "Scale world",
			End,
			Child, HSpace(5),
			Child, databuf.brect = ImageButtonExt("5:buttons/dragbox.iff", xres, yres),
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Select objects with drag box",
			End,
			Child, HSpace(5),
			Child, databuf.bx = ImageButtonExt("5:buttons/x.iff", xres, yres),
				MUIA_Selected,TRUE,
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Toggle active x-axis",
			End,
			Child, databuf.by = ImageButtonExt("5:buttons/y.iff", xres, yres),
				MUIA_Selected,TRUE,
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Toggle active y-axis",
			End,
			Child, databuf.bz = ImageButtonExt("5:buttons/z.iff", xres, yres),
				MUIA_Selected,TRUE,
				MUIA_InputMode,MUIV_InputMode_Toggle,
				MUIA_ShortHelp, "Toggle active z-axis",
			End,
			Child, HSpace(0),
		End;
	}

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_ID, MAKE_ID('M','A','I','N'),
		WindowContents, VGroup,
			Child, toolbar,
			Child, RectangleObject, MUIA_Rectangle_HBar, TRUE, MUIA_FixHeight, 2, End,
			Child, databuf.middlegroup = HGroup,
				MUIA_VertWeight, weight[2],
				Child, VGroup,
					MUIA_HorizWeight, weight[0],
					Child, databuf.drawarea = (Object *)NewObject(CL_Draw->mcc_Class,NULL,
						MUIA_ContextMenu, TRUE,
						MUIA_Background,  MUII_BACKGROUND,
					End,
/*               Child,
						databuf.animbar = (Object *)NewObject(CL_AnimBar->mcc_Class,NULL,
					End,*/
				End,
				Child, BalanceObject, End,
				Child, databuf.browsergroup = ListviewObject,
					MUIA_HorizWeight, weight[1],
					MUIA_VertWeight, weight[2],
					MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
//             MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
					MUIA_Listview_List, databuf.browser = (Object *)NewObject(CL_BrowserTree->mcc_Class,NULL,End,
				End,
			End,
			Child, BalanceObject, End,
			Child, databuf.materialgroup = ScrollgroupObject,
				MUIA_VertWeight, weight[3],
				MUIA_Scrollgroup_FreeHoriz, FALSE,
				MUIA_Scrollgroup_Contents, databuf.material = (Object *)NewObject(CL_Material->mcc_Class,NULL,TAG_DONE),
			End,
			Child, BalanceObject, End,
			/*Child, databuf.animgroup = ListviewObject,
				MUIA_Listview_List, databuf.anim = (Object *)NewObject(CL_AnimTree->mcc_Class,NULL,End,
			End,*/
			Child, RectangleObject, MUIA_Rectangle_HBar, TRUE, MUIA_FixHeight, 2, End,
			Child, HGroup,
				Child, TextObject,
					MUIA_Text_Contents, "Selected",
					MUIA_Text_PreParse, MUIX_R,
					MUIA_Weight, 20,
				End,
				Child, databuf.selobj = TextObject,
					TextFrame,
					MUIA_Text_PreParse, MUIX_L,
					MUIA_ShortHelp, "Currently selected object",
					MUIA_Background, MUII_TextBack,
				End,
				Child, HSpace(10),
				Child, TextObject,
					MUIA_Text_Contents, "Camera",
					MUIA_Text_PreParse, MUIX_R,
					MUIA_Weight, 20,
				End,
				Child, databuf.camera = TextObject,
					TextFrame,
					MUIA_Text_PreParse, MUIX_L,
					MUIA_ShortHelp, "Active camera",
					MUIA_Weight, 50,
					MUIA_Background, MUII_TextBack,
				End,
				Child, TextObject,
					MUIA_Text_Contents, "X",
					MUIA_Text_PreParse, MUIX_R,
					MUIA_Weight, 20,
				End,
				Child, databuf.cx = TextObject,
					TextFrame,
					MUIA_Text_PreParse, MUIX_L,
					MUIA_Weight, 70,
					MUIA_Background, MUII_TextBack,
				End,
				Child, TextObject,
					MUIA_Text_Contents, "Y",
					MUIA_Text_PreParse, MUIX_R,
					MUIA_Weight, 20,
				End,
				Child, databuf.cy = TextObject,
					TextFrame,
					MUIA_Text_PreParse, MUIX_L,
					MUIA_Weight, 70,
					MUIA_Background, MUII_TextBack,
				End,
				Child, TextObject,
					MUIA_Text_Contents, "Z",
					MUIA_Text_PreParse, MUIX_R,
					MUIA_Weight, 20,
				End,
				Child, databuf.cz = TextObject,
					TextFrame,
					MUIA_Text_PreParse, MUIX_L,
					MUIA_Weight, 70,
					MUIA_Background, MUII_TextBack,
				End,
			End,
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (MainWindow_Data *)INST_DATA(cl,obj);
		*data = databuf;
		data->strip = (Object *)GetTagData(MUIA_Menustrip,0,msg->ops_AttrList);
		data->filenum = 0;
		data->globalwin = NULL;
		data->rendsetwin = NULL;
		data->gridsizewin = NULL;

		SetWindowTitle(obj,NULL);
		SetAttrs(data->drawarea, MUIA_Draw_Window, obj, TAG_DONE);
		SetAttrs(data->material, MUIA_Material_MainWindow, obj, TAG_DONE);

		SetAttrs(data->browser, MUIA_BrowserTree_DrawArea, data->drawarea, TAG_DONE);

		// initialize sci
		sciInit(data->drawarea, data->browser, data->material, obj);
		utility.InitDrawWin(data->drawarea);

		DOMETHOD(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			MUIV_Notify_Application,2,MUIM_Application_ReturnID,
			MUIV_Application_ReturnID_Quit);

		DOMETHOD(data->bnew,MUIM_Notify,MUIA_Pressed,FALSE,
			obj, 1, MUIM_MainWindow_New);

		DOMETHOD(data->bloa,MUIM_Notify,MUIA_Pressed,FALSE,
			obj, 1, MUIM_MainWindow_Open);

		DOMETHOD(data->bsav,MUIM_Notify,MUIA_Pressed,FALSE,
			obj, 1, MUIM_MainWindow_Save);

		DESEL_BUTTON(data->beditpoints, data->baddpoints);
		DESEL_BUTTON(data->beditpoints, data->beditedges);
		DESEL_BUTTON(data->beditpoints, data->baddedges);
		DESEL_BUTTON(data->beditpoints, data->beditfaces);
		DESEL_BUTTON(data->beditpoints, data->baddfaces);
		DOMETHOD(data->beditpoints, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_EDITPOINTS);

		DESEL_BUTTON(data->baddpoints, data->beditpoints);
		DESEL_BUTTON(data->baddpoints, data->beditedges);
		DESEL_BUTTON(data->baddpoints, data->baddedges);
		DESEL_BUTTON(data->baddpoints, data->beditfaces);
		DESEL_BUTTON(data->baddpoints, data->baddfaces);
		DOMETHOD(data->baddpoints, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_ADDPOINTS);

		DESEL_BUTTON(data->beditedges, data->beditpoints);
		DESEL_BUTTON(data->beditedges, data->baddpoints);
		DESEL_BUTTON(data->beditedges, data->baddedges);
		DESEL_BUTTON(data->beditedges, data->beditfaces);
		DESEL_BUTTON(data->beditedges, data->baddfaces);
		DOMETHOD(data->beditedges, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_EDITEDGES);

		DESEL_BUTTON(data->baddedges, data->beditpoints);
		DESEL_BUTTON(data->baddedges, data->baddpoints);
		DESEL_BUTTON(data->baddedges, data->beditedges);
		DESEL_BUTTON(data->baddedges, data->beditfaces);
		DESEL_BUTTON(data->baddedges, data->baddfaces);
		DOMETHOD(data->baddedges, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_ADDEDGES);

		DESEL_BUTTON(data->beditfaces, data->beditpoints);
		DESEL_BUTTON(data->beditfaces, data->baddpoints);
		DESEL_BUTTON(data->beditfaces, data->beditedges);
		DESEL_BUTTON(data->beditfaces, data->baddedges);
		DESEL_BUTTON(data->beditfaces, data->baddfaces);
		DOMETHOD(data->beditfaces, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_EDITFACES);

		DESEL_BUTTON(data->baddfaces, data->beditpoints);
		DESEL_BUTTON(data->baddfaces,data->baddpoints);
		DESEL_BUTTON(data->baddfaces,data->beditedges);
		DESEL_BUTTON(data->baddfaces,data->baddedges);
		DESEL_BUTTON(data->baddfaces,data->beditfaces);
		DOMETHOD(data->baddfaces, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_ADDFACES);

		DOMETHOD(data->bfro,MUIM_Notify,MUIA_Selected,TRUE,
			data->drawarea,3,MUIM_Set,MUIA_Draw_View,VIEW_FRONT);
		DESEL_BUTTON(data->bfro,data->brig);
		DESEL_BUTTON(data->bfro,data->btop);
		DESEL_BUTTON(data->bfro,data->bper);
		DESEL_BUTTON(data->bfro,data->bcam);

		DOMETHOD(data->brig,MUIM_Notify,MUIA_Selected,TRUE,
			data->drawarea,3,MUIM_Set,MUIA_Draw_View,VIEW_RIGHT);
		DESEL_BUTTON(data->brig,data->bfro);
		DESEL_BUTTON(data->brig,data->btop);
		DESEL_BUTTON(data->brig,data->bper);
		DESEL_BUTTON(data->brig,data->bcam);

		DOMETHOD(data->btop,MUIM_Notify,MUIA_Selected,TRUE,
			data->drawarea,3,MUIM_Set,MUIA_Draw_View,VIEW_TOP);
		DESEL_BUTTON(data->btop,data->bfro);
		DESEL_BUTTON(data->btop,data->brig);
		DESEL_BUTTON(data->btop,data->bper);
		DESEL_BUTTON(data->btop,data->bcam);

		DOMETHOD(data->bper,MUIM_Notify,MUIA_Selected,TRUE,
			data->drawarea,3,MUIM_Set,MUIA_Draw_View,VIEW_PERSP);
		DESEL_BUTTON(data->bper,data->bfro);
		DESEL_BUTTON(data->bper,data->brig);
		DESEL_BUTTON(data->bper,data->btop);
		DESEL_BUTTON(data->bper,data->bcam);

		DOMETHOD(data->bcam,MUIM_Notify,MUIA_Selected,TRUE,
			data->drawarea,3,MUIM_Set,MUIA_Draw_View,VIEW_CAMERA);
		DESEL_BUTTON(data->bcam,data->bfro);
		DESEL_BUTTON(data->bcam,data->brig);
		DESEL_BUTTON(data->bcam,data->btop);
		DESEL_BUTTON(data->bcam,data->bper);

		DOMETHOD(data->bfor, MUIM_Notify, MUIA_Selected, TRUE,
			data->drawarea, 3, MUIM_Set, MUIA_Draw_ViewFour, TRUE);
		DOMETHOD(data->bfor, MUIM_Notify, MUIA_Selected, FALSE,
			data->drawarea, 3, MUIM_Set, MUIA_Draw_ViewFour, FALSE);

		DOMETHOD(data->bbou,MUIM_Notify,MUIA_Selected,TRUE,
			data->drawarea,3,MUIM_Set,MUIA_Draw_Display,DISPLAY_BBOX);
		DESEL_BUTTON(data->bbou,data->bwir);
#ifdef __OPENGL__
		DESEL_BUTTON(data->bbou,data->bsol);
#endif // __OPENGL__

		DOMETHOD(data->bwir,MUIM_Notify,MUIA_Selected,TRUE,
			data->drawarea,3,MUIM_Set,MUIA_Draw_Display,DISPLAY_WIRE);
		DESEL_BUTTON(data->bwir,data->bbou);
#ifdef __OPENGL__
		DESEL_BUTTON(data->bwir,data->bsol);

		DOMETHOD(data->bsol,MUIM_Notify,MUIA_Selected,TRUE,
			data->drawarea,3,MUIM_Set,MUIA_Draw_Display,DISPLAY_SOLID);
		DESEL_BUTTON(data->bsol,data->bbou);
		DESEL_BUTTON(data->bsol,data->bwir);
#endif // __OPENGL__

		DOMETHOD(data->bloc,MUIM_Notify,MUIA_Selected, MUIV_EveryTime,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_EDIT_LOCAL);

		DOMETHOD(data->bmov,MUIM_Notify,MUIA_Selected,TRUE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_EDITMOVEOBJECT);
		DOMETHOD(data->bmov,MUIM_Notify,MUIA_Selected,TRUE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_MOVE),3,MUIM_Set,MUIA_Menuitem_Checked,TRUE);
		DESEL_BUTTON(data->bmov,data->brot);
		DESEL_BUTTON(data->bmov,data->bsca);
		DESEL_BUTTON(data->bmov,data->bwmov);
		DESEL_BUTTON(data->bmov,data->bwrot);
		DESEL_BUTTON(data->bmov,data->bwsca);

		DOMETHOD(data->brot,MUIM_Notify,MUIA_Selected,TRUE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_EDITROTATEOBJECT);
		DOMETHOD(data->brot,MUIM_Notify,MUIA_Selected,TRUE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_ROTATE),3,MUIM_Set,MUIA_Menuitem_Checked,TRUE);
		DESEL_BUTTON(data->brot,data->bmov);
		DESEL_BUTTON(data->brot,data->bsca);
		DESEL_BUTTON(data->brot,data->bwmov);
		DESEL_BUTTON(data->brot,data->bwrot);
		DESEL_BUTTON(data->brot,data->bwsca);

		DOMETHOD(data->bsca,MUIM_Notify,MUIA_Selected,TRUE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_EDITSCALEOBJECT);
		DOMETHOD(data->bsca,MUIM_Notify,MUIA_Selected,TRUE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_SCALE),3,MUIM_Set,MUIA_Menuitem_Checked,TRUE);
		DESEL_BUTTON(data->bsca,data->bmov);
		DESEL_BUTTON(data->bsca,data->brot);
		DESEL_BUTTON(data->bsca,data->bwmov);
		DESEL_BUTTON(data->bsca,data->bwrot);
		DESEL_BUTTON(data->bsca,data->bwsca);

		DOMETHOD(data->bwmov,MUIM_Notify,MUIA_Selected,TRUE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_EDITMOVEWORLD);
		DOMETHOD(data->bwmov,MUIM_Notify,MUIA_Selected,TRUE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_WMOVE),3,MUIM_Set,MUIA_Menuitem_Checked,TRUE);
		DESEL_BUTTON(data->bwmov,data->bmov);
		DESEL_BUTTON(data->bwmov,data->brot);
		DESEL_BUTTON(data->bwmov,data->bsca);
		DESEL_BUTTON(data->bwmov,data->bwrot);
		DESEL_BUTTON(data->bwmov,data->bwsca);

		DOMETHOD(data->bwrot,MUIM_Notify,MUIA_Selected,TRUE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_EDITROTATEWORLD);
		DOMETHOD(data->bwrot,MUIM_Notify,MUIA_Selected,TRUE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_WROTATE),3,MUIM_Set,MUIA_Menuitem_Checked,TRUE);
		DESEL_BUTTON(data->bwrot,data->bmov);
		DESEL_BUTTON(data->bwrot,data->brot);
		DESEL_BUTTON(data->bwrot,data->bsca);
		DESEL_BUTTON(data->bwrot,data->bwmov);
		DESEL_BUTTON(data->bwrot,data->bwsca);

		DOMETHOD(data->bwsca,MUIM_Notify,MUIA_Selected,TRUE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_EDITSCALEWORLD);
		DOMETHOD(data->bwsca,MUIM_Notify,MUIA_Selected,TRUE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_WSCALE),3,MUIM_Set,MUIA_Menuitem_Checked,TRUE);
		DESEL_BUTTON(data->bwsca,data->bmov);
		DESEL_BUTTON(data->bwsca,data->brot);
		DESEL_BUTTON(data->bwsca,data->bsca);
		DESEL_BUTTON(data->bwsca,data->bwmov);
		DESEL_BUTTON(data->bwsca,data->bwrot);

		DOMETHOD(data->brect,MUIM_Notify,MUIA_Selected,TRUE,
			data->drawarea,3,MUIM_Set,MUIA_Draw_SelectByBox,TRUE);
		DOMETHOD(data->brect,MUIM_Notify,MUIA_Selected,FALSE,
			data->drawarea,3,MUIM_Set,MUIA_Draw_SelectByBox,FALSE);

		DOMETHOD(data->bx,MUIM_Notify,MUIA_Selected,TRUE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_SETACTIVEX);
		DOMETHOD(data->bx,MUIM_Notify,MUIA_Selected,TRUE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_X),3,MUIM_Set,MUIA_Menuitem_Checked,TRUE);
		DOMETHOD(data->bx,MUIM_Notify,MUIA_Selected,FALSE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_UNSETACTIVEX);
		DOMETHOD(data->bx,MUIM_Notify,MUIA_Selected,FALSE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_X),3,MUIM_Set,MUIA_Menuitem_Checked,FALSE);

		DOMETHOD(data->by,MUIM_Notify,MUIA_Selected,TRUE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_SETACTIVEY);
		DOMETHOD(data->by,MUIM_Notify,MUIA_Selected,TRUE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_Y),3,MUIM_Set,MUIA_Menuitem_Checked,TRUE);
		DOMETHOD(data->by,MUIM_Notify,MUIA_Selected,FALSE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_UNSETACTIVEY);
		DOMETHOD(data->by,MUIM_Notify,MUIA_Selected,FALSE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_Y),3,MUIM_Set,MUIA_Menuitem_Checked,FALSE);

		DOMETHOD(data->bz,MUIM_Notify,MUIA_Selected,TRUE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_SETACTIVEZ);
		DOMETHOD(data->bz,MUIM_Notify,MUIA_Selected,TRUE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_Z),3,MUIM_Set,MUIA_Menuitem_Checked,TRUE);
		DOMETHOD(data->bz,MUIM_Notify,MUIA_Selected,FALSE,
			obj, 2, MUIM_MainWindow_sciDoMethod, SCIM_UNSETACTIVEZ);
		DOMETHOD(data->bz,MUIM_Notify,MUIA_Selected,FALSE,
			(Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_Z),3,MUIM_Set,MUIA_Menuitem_Checked,FALSE);

		for(i=sizeof(notify)/sizeof(NOTIFY_MSG)-1; i>=0; i--)
			NOTIFY_MENU(notify[i].menu, notify[i].method);
		for(i=sizeof(scinotify)/sizeof(NOTIFY_MSG)-1; i>=0; i--)
			DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,scinotify[i].menu), MUIM_Notify,MUIA_Menuitem_Trigger, MUIV_EveryTime, obj, 2, MUIM_MainWindow_sciDoMethod, scinotify[i].method);

		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_RENDERWINDOW),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->drawarea,3,MUIM_Set,MUIA_Draw_SetRenderWindow,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_RENDERWINDOW),MUIM_Notify,MUIA_Menuitem_Checked,FALSE,data->drawarea,3,MUIM_Set,MUIA_Draw_SetRenderWindow,FALSE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_QUIT),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_MOVE),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,data->bmov,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_ROTATE),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,data->brot,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_SCALE),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,data->bsca,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_WMOVE),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,data->bwmov,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_WROTATE),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,data->bwrot,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MODE_WSCALE),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,data->bwsca,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_X),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->bx,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_X),MUIM_Notify,MUIA_Menuitem_Checked,FALSE,data->bx,3,MUIM_Set,MUIA_Selected,FALSE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_Y),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->by,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_Y),MUIM_Notify,MUIA_Menuitem_Checked,FALSE,data->by,3,MUIM_Set,MUIA_Selected,FALSE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_Z),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->bz,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_ACTIVE_Z),MUIM_Notify,MUIA_Menuitem_Checked,FALSE,data->bz,3,MUIM_Set,MUIA_Selected,FALSE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_VIEW_FRONT),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->bfro,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_VIEW_RIGHT),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->brig,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_VIEW_TOP),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->btop,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_VIEW_PERSP),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->bper,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_VIEW_CAMERA),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->bcam,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_VIEW_FOUR),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->bfor,3,MUIM_Set,MUIA_Selected,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_SHOWBROWSER),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->browsergroup,3,MUIM_Set,MUIA_ShowMe,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_SHOWBROWSER),MUIM_Notify,MUIA_Menuitem_Checked,FALSE,data->browsergroup,3,MUIM_Set,MUIA_ShowMe,FALSE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_SHOWMATERIAL),MUIM_Notify,MUIA_Menuitem_Checked,TRUE,data->materialgroup,3,MUIM_Set,MUIA_ShowMe,TRUE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_SHOWMATERIAL),MUIM_Notify,MUIA_Menuitem_Checked,FALSE,data->materialgroup,3,MUIM_Set,MUIA_ShowMe,FALSE);
		DOMETHOD((Object *)DOMETHOD(data->strip,MUIM_FindUData,MEN_MUI  ),MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,2,MUIM_Application_OpenConfigWindow,0);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   OM_SET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG mSet(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct MainWindow_Data *data = (struct MainWindow_Data *)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;
	ULONG f;

	tags=msg->ops_AttrList;
	do
	{
		tag=NextTagItem(&tags);
		if(tag)
		{
			switch (tag->ti_Tag)
			{
				case MUIA_MainWindow_CoordX:
					SetAttrs(data->cx,MUIA_Text_Contents,(char*)(tag->ti_Data), TAG_DONE);
					break;
				case MUIA_MainWindow_CoordY:
					SetAttrs(data->cy,MUIA_Text_Contents,(char*)(tag->ti_Data), TAG_DONE);
					break;
				case MUIA_MainWindow_CoordZ:
					SetAttrs(data->cz,MUIA_Text_Contents,(char*)(tag->ti_Data), TAG_DONE);
					break;
				case MUIA_MainWindow_SelectedObject:
					SetAttrs(data->selobj,MUIA_Text_Contents,(char*)(tag->ti_Data), TAG_DONE);
					break;
				case MUIA_MainWindow_ActiveCamera:
					SetAttrs(data->camera,MUIA_Text_Contents,(char*)(tag->ti_Data), TAG_DONE);
					break;
				case MUIA_MainWindow_Active_Gadget:
					switch((int)tag->ti_Data)
					{
						case GADGET_FRONT:
							SetAttrs(data->bfro, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_RIGHT:
							SetAttrs(data->brig, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_TOP:
							SetAttrs(data->btop, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_PERSP:
							SetAttrs(data->bper, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_FOUR:
							GetAttr(MUIA_Selected, data->bfor, &f);
							SetAttrs(data->bfor, MUIA_Selected, !f, TAG_DONE);
							break;
						case GADGET_CAMERA:
							SetAttrs(data->bcam, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_MOVE:
							SetAttrs(data->bmov, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_ROTATE:
							SetAttrs(data->brot, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_SCALE:
							SetAttrs(data->bsca, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_WMOVE:
							SetAttrs(data->bwmov, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_WROTATE:
							SetAttrs(data->bwrot, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_WSCALE:
							SetAttrs(data->bwsca, MUIA_Selected, TRUE, TAG_DONE);
							break;
						case GADGET_SELBOX:
							GetAttr(MUIA_Selected, data->brect, &f);
							SetAttrs(data->brect,MUIA_Selected, !f, TAG_DONE);
							break;
						case GADGET_X_TOGGLE:
							GetAttr(MUIA_Selected, data->bx, &f);
							SetAttrs(data->bx,MUIA_Selected, !f, TAG_DONE);
							break;
						case GADGET_Y_TOGGLE:
							GetAttr(MUIA_Selected, data->by, &f);
							SetAttrs(data->by,MUIA_Selected, !f, TAG_DONE);
							break;
						case GADGET_Z_TOGGLE:
							GetAttr(MUIA_Selected, data->bz, &f);
							SetAttrs(data->bz,MUIA_Selected, !f, TAG_DONE);
							break;
						case GADGET_X:
							SetAttrs(data->bx,MUIA_Selected, TRUE, TAG_DONE);
							SetAttrs(data->by,MUIA_Selected, FALSE, TAG_DONE);
							SetAttrs(data->bz,MUIA_Selected, FALSE, TAG_DONE);
							break;
						case GADGET_Y:
							SetAttrs(data->bx,MUIA_Selected, FALSE, TAG_DONE);
							SetAttrs(data->by,MUIA_Selected, TRUE, TAG_DONE);
							SetAttrs(data->bz,MUIA_Selected, FALSE, TAG_DONE);
							break;
						case GADGET_Z:
							SetAttrs(data->bx,MUIA_Selected, FALSE, TAG_DONE);
							SetAttrs(data->by,MUIA_Selected, FALSE, TAG_DONE);
							SetAttrs(data->bz,MUIA_Selected, TRUE, TAG_DONE);
							break;
					}
					break;
				case MUIA_MainWindow_EnableEditMesh:
					SetAttrs(data->beditpoints, MUIA_Disabled, !(BOOL)(tag->ti_Data), TAG_DONE);
					SetAttrs(data->baddpoints, MUIA_Disabled, !(BOOL)(tag->ti_Data), TAG_DONE);
					SetAttrs(data->beditedges, MUIA_Disabled, !(BOOL)(tag->ti_Data), TAG_DONE);
					SetAttrs(data->baddedges, MUIA_Disabled, !(BOOL)(tag->ti_Data), TAG_DONE);
					SetAttrs(data->beditfaces, MUIA_Disabled, !(BOOL)(tag->ti_Data), TAG_DONE);
					SetAttrs(data->baddfaces, MUIA_Disabled, !(BOOL)(tag->ti_Data), TAG_DONE);
					break;
				case MUIA_MainWindow_EnableEditSOR:
					SetAttrs(data->beditpoints, MUIA_Disabled, !(BOOL)(tag->ti_Data), TAG_DONE);
					SetAttrs(data->baddpoints, MUIA_Disabled, !(BOOL)(tag->ti_Data), TAG_DONE);
					SetAttrs(data->beditedges, MUIA_Disabled, TRUE, TAG_DONE);
					SetAttrs(data->baddedges, MUIA_Disabled, TRUE, TAG_DONE);
					SetAttrs(data->beditfaces, MUIA_Disabled, TRUE, TAG_DONE);
					SetAttrs(data->baddfaces, MUIA_Disabled, TRUE, TAG_DONE);
					break;
			}
		}
	}
	while(tag);

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   child window will finish
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG ChildWindow_Finish(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish *msg)
{
	struct MainWindow_Data *data = (struct MainWindow_Data *)INST_DATA(cl,obj);
	Object *win;

	win = msg->win;
	SetAttrs(win,MUIA_Window_Open,FALSE, TAG_DONE);
	DOMETHOD(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);

	if(win == data->globalwin)
		data->globalwin = NULL;
	else
		SetSleep(FALSE);

	if(msg->ok)
		SetProjectChanged(TRUE);

	return 0;
}

/*************
 * DESCRIPTION:   attributes window will finish
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG AttrsWindow_Finish(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish *msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	SURFACE *surf;
	Object *win = msg->win;

	GetAttr(MUIA_AttrWindow_Surface, win, (ULONG*)&surf);

	if(msg->ok)
	{
		DOMETHOD(data->material, MUIM_Material_UpdateMaterial, surf);
		UpdateSurfaceAssignment(surf);
		DOMETHOD(data->drawarea, MUIM_Draw_RedrawDirty);
		SetProjectChanged(TRUE);
	}

	SetAttrs(win, MUIA_Window_Open, FALSE, TAG_DONE);
	DOMETHOD(app, OM_REMMEMBER, win);
	MUI_DisposeObject(win);
	surf->win = NULL;

	return 0;
}

/*************
 * DESCRIPTION:   numeric rquester finished
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG NumReq_Finish(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish *msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	float size;
	Object *win = msg->win;

	data->gridsizewin = NULL;

	if(msg->ok)
	{
		GetAttr(MUIA_NumReq_Value, win, (ULONG*)&size);
		SetAttrs(data->drawarea, MUIA_Draw_GridSize, &size, TAG_DONE);
		SetProjectChanged(TRUE);
	}

	SetAttrs(win, MUIA_Window_Open, FALSE, TAG_DONE);
	DOMETHOD(app, OM_REMMEMBER, win);
	MUI_DisposeObject(win);

	return 0;
}

/*************
 * DESCRIPTION:   browser finished
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Browser_Finish(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish *msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	Object *win, *newobj;
	OBJECT *object;
	ULONG mode;
	DISPLAY *disp;

	win = msg->win;

	GetAttr(MUIA_Browser_ActiveObject, win, (ULONG*)&object);
	SetAttrs(win,MUIA_Window_Open,FALSE, TAG_DONE);

	if(object && msg->ok)
	{
		GetAttr(MUIA_Browser_Mode, win, (ULONG*)&mode);

		switch(mode)
		{
			case BROWSER_SELECTEDSURFACE:
				if(!object->surf)
				{
					DisplayBeep(_screen(obj));
					break;
				}
				if(object->surf->win)
				{
					// requester already open -> pop to front
					DOMETHOD(object->surf->win, MUIM_Window_ToFront);
					SetAttrs(object->surf->win, MUIA_Window_Activate, TRUE, TAG_DONE);
				}
				else
				{
					// create attributes window
					newobj = (Object*)NewObject(CL_AttrsWindow->mcc_Class,NULL,
						MUIA_ChildWindow_Originator, obj,
						MUIA_AttrWindow_Surface, object->surf,
						MUIA_AttrWindow_Object, object,
						TAG_DONE);
					if(newobj)
					{
						object->surf->win = newobj;
						DOMETHOD(app,OM_ADDMEMBER,newobj);
						SetAttrs(newobj,MUIA_Window_Open,TRUE, TAG_DONE);
					}
				}
				break;
			case BROWSER_CAMERAS:
				DeselectAll();
				object->selected = TRUE;
				object->IsFirstSelected();
				object->flags |= OBJECT_DIRTY;
				GetAttr(MUIA_Draw_DisplayPtr,data->drawarea,(ULONG*)&disp);
				disp->camera = (CAMERA*)object;
				disp->view->SetCamera(disp->camera);
				if(disp->view->viewmode == VIEW_CAMERA)
				{
					// if camera is used to view scene do a general update
					MUI_Redraw(data->drawarea,MADF_DRAWUPDATE);
				}
				else
				{
					// only draw camera new
					DOMETHOD(data->drawarea,MUIM_Draw_RedrawDirty);
				}
				break;
			case BROWSER_ALL:
				DeselectAll();
				object->IsFirstSelected();
				object->SelectObject();
				DOMETHOD(data->drawarea,MUIM_Draw_RedrawDirty);
				break;
		}
	}

	DOMETHOD(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);
	SetSleep(FALSE);
	SetProjectChanged(TRUE);

	return 0;
}

/*************
 * DESCRIPTION:   mesh add window finished
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG MeshAddWindow_Finish(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish *msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	VECTOR size;
	MESH *mesh;
	ULONG type;
	float r, r1, height;
	int divs, slices;
	BOOL flag1, flag2;
	UNDO_CREATE *undo;
	Object *win = msg->win;

	SetAttrs(win, MUIA_Window_Open, FALSE, TAG_DONE);
	if(msg->ok)
	{
		GetAttr(MUIA_MeshAddWindow_Type, win, (ULONG*)&type);
		DeselectAll();

		mesh = new MESH;
		if(mesh)
		{
			mesh->selected = TRUE;
			mesh->surf = GetSurfaceByName(SURF_DEFAULTNAME);
			if(!mesh->surf)
			{
				mesh->surf = sciCreateSurface(SURF_DEFAULTNAME);
				if(!mesh->surf)
				{
					delete mesh;
					mesh = NULL;
				}
				sciAddMaterial(mesh->surf);
			}
			else
			{
				if(!sciAssignSurface(mesh->surf, mesh))
				{
					delete mesh;
					mesh = NULL;
				}
			}
			if(mesh)
			{
				switch(type)
				{
					case MESHADD_CUBE:
						GetAttr(MUIA_MeshAddWindow_Vector1, win, (ULONG*)&size);
						if(!mesh->CreateCube(&size))
						{
							delete mesh;
							mesh = NULL;
						}
						break;
					case MESHADD_SPHERE:
						GetAttr(MUIA_MeshAddWindow_Float1, win, (ULONG*)&r);
						GetAttr(MUIA_MeshAddWindow_Int1, win, (ULONG*)&divs);
						GetAttr(MUIA_MeshAddWindow_Int2, win, (ULONG*)&slices);
						if(!mesh->CreateSphere(r,divs,slices))
						{
							delete mesh;
							mesh = NULL;
						}
						break;
					case MESHADD_PLANE:
						GetAttr(MUIA_MeshAddWindow_Vector1, win, (ULONG*)&size);
						GetAttr(MUIA_MeshAddWindow_Int1, win, (ULONG*)&divs);
						GetAttr(MUIA_MeshAddWindow_Int2, win, (ULONG*)&slices);
						if(!mesh->CreatePlane(&size,divs,slices))
						{
							delete mesh;
							mesh = NULL;
						}
						break;
					case MESHADD_TUBE:
						GetAttr(MUIA_MeshAddWindow_Float1, win, (ULONG*)&r);
						GetAttr(MUIA_MeshAddWindow_Float2, win, (ULONG*)&height);
						GetAttr(MUIA_MeshAddWindow_Int1, win, (ULONG*)&divs);
						GetAttr(MUIA_MeshAddWindow_Int2, win, (ULONG*)&slices);
						GetAttr(MUIA_MeshAddWindow_Flag1, win, (ULONG*)&flag1);
						GetAttr(MUIA_MeshAddWindow_Flag2, win, (ULONG*)&flag2);
						if(!mesh->CreateTube(r, height, divs, slices, flag2, flag1))
						{
							delete mesh;
							mesh = NULL;
						}
						break;
					case MESHADD_CONE:
						GetAttr(MUIA_MeshAddWindow_Float1, win, (ULONG*)&r);
						GetAttr(MUIA_MeshAddWindow_Float2, win, (ULONG*)&height);
						GetAttr(MUIA_MeshAddWindow_Int1, win, (ULONG*)&divs);
						GetAttr(MUIA_MeshAddWindow_Int2, win, (ULONG*)&slices);
						GetAttr(MUIA_MeshAddWindow_Flag1, win, (ULONG*)&flag1);
						if(!mesh->CreateCone(r, height, divs, slices, flag1))
						{
							delete mesh;
							mesh = NULL;
						}
						break;
					case MESHADD_TORUS:
						GetAttr(MUIA_MeshAddWindow_Float1, win, (ULONG*)&r);
						GetAttr(MUIA_MeshAddWindow_Float2, win, (ULONG*)&r1);
						GetAttr(MUIA_MeshAddWindow_Int1, win, (ULONG*)&divs);
						GetAttr(MUIA_MeshAddWindow_Int2, win, (ULONG*)&slices);
						if(!mesh->CreateTorus(r, r1, divs, slices))
						{
							delete mesh;
							mesh = NULL;
						}
						break;
				}
				if(mesh)
				{
					mesh->Append();
					mesh->IsFirstSelected();

					undo = new UNDO_CREATE;
					if(undo)
					{
						if(undo->AddCreated(mesh))
							undo->Add();
						else
							delete undo;
					}
				}
			}
		}
		DOMETHOD(data->drawarea,MUIM_Draw_RedrawDirty);
	}

	DOMETHOD(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);
	SetSleep(FALSE);
	SetProjectChanged(TRUE);
	sciBrowserBuild();

	return 0;
}

/*************
 * DESCRIPTION:   render set window will finish
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG RenderWindow_Finish(struct IClass *cl,Object *obj,struct MUIP_RenderWindow_Finish *msg)
{
	MainWindow_Data *data = (MainWindow_Data *)INST_DATA(cl,obj);
	Object *win;
	DISPLAY *disp;

	win = msg->win;

	SetAttrs(win,MUIA_Window_Open,FALSE, TAG_DONE);
	DOMETHOD(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);
	data->rendsetwin = NULL;

	if(msg->redraw)
	{
		GetAttr(MUIA_Draw_DisplayPtr,data->drawarea,(ULONG*)&disp);
		if(disp->view->viewmode == VIEW_CAMERA)
			MUI_Redraw(data->drawarea,MADF_DRAWUPDATE);
	}
	if(msg->ok)
		SetProjectChanged(TRUE);

	return 0;
}

/*************
 * DESCRIPTI0ON:
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(MainWindow_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
#ifdef __STORMC_SWITCH_BUG__
	switch((int)msg->MethodID)
#else
	switch(msg->MethodID)
#endif
	{
		case OM_SET:
			return(mSet(cl,obj,(struct opSet*)msg));
		case OM_NEW:
			return(mNew(cl,obj,(struct opSet *)msg));
		case MUIM_ChildWindow_Finish:
			return(ChildWindow_Finish(cl,obj,(struct MUIP_ChildWindow_Finish *)msg));
		case MUIM_AttrsWindow_Finish:
			return(AttrsWindow_Finish(cl,obj,(struct MUIP_ChildWindow_Finish *)msg));
		case MUIM_NumReq_Finish:
			return(NumReq_Finish(cl,obj,(struct MUIP_ChildWindow_Finish *)msg));
		case MUIM_Browser_Finish:
			return(Browser_Finish(cl,obj,(struct MUIP_ChildWindow_Finish *)msg));
		case MUIM_MeshAddWindow_Finish:
			return(MeshAddWindow_Finish(cl,obj,(struct MUIP_ChildWindow_Finish *)msg));
		case MUIM_RenderWindow_Finish:
			return(RenderWindow_Finish(cl,obj,(struct MUIP_RenderWindow_Finish *)msg));
		case MUIM_MainWindow_OpenQuiet:
			OpenProject(cl,obj, ((struct MUIP_MainWindow_Open*)msg)->name);
			return 0;
		case MUIM_MainWindow_OpenMaterialRequ:
			return(OpenMaterialRequ(cl,obj,(struct MUIP_Material_Material *)msg));
		case MUIM_MainWindow_sciDoMethod:
			if(!sciDoMethod(((struct MUIP_MainWindow_sciDoMethod*)msg)->method))
				DisplayBeep(_screen(obj));
			return 0;
		case MUIM_MainWindow_OpenPlugin:
			return(OpenPlugin(cl,obj,(struct MUIP_MainWindow_OpenPlugin*)msg));
		DISPATCH(New);
		DISPATCH(Open);
		DISPATCH(Save);
		DISPATCH(SaveAs);
		DISPATCH(RenderSettings);
		DISPATCH(Render);
		DISPATCH(File1);
		DISPATCH(File2);
		DISPATCH(File3);
		DISPATCH(File4);
#ifdef __STATISTICS__
		DISPATCH(Statistics);
#endif
		DISPATCH(About);
		DISPATCH(Attributes);
		DISPATCH(Settings);
		DISPATCH(GridSize);
		DISPATCH(ShowCoord);
		DISPATCH(Global);
		DISPATCH(MeshCube);
		DISPATCH(MeshSphere);
		DISPATCH(MeshPlane);
		DISPATCH(MeshTube);
		DISPATCH(MeshCone);
		DISPATCH(MeshTorus);
		DISPATCH(LoadObj);
		DISPATCH(SaveObj);
		DISPATCH(ActiveCamera);
		DISPATCH(MaterialNew);
		DISPATCH(MaterialLoadNew);
		DISPATCH(MaterialRemoveUnused);
		DISPATCH(MaterialRemoveIdentical);
		DISPATCH(Prefs);
		DISPATCH(LoadPrefs);
		DISPATCH(SavePrefs);
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
