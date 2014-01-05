/***************
 * PROGRAM:       Modeler
 * NAME:          mainwindow_class.h
 * DESCRIPTION:   definitions for main window class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    30.01.97 ah    added redo
 ***************/

#ifndef MAINWINDOW_CLASS_H
#define MAINWINDOW_CLASS_H

#define __VERS__ "2.3"

#ifndef PLUGINS_H
#include "plugins.h"
#endif // PLUGINS_H

// menus
enum
{
	MEN_NEW=1,
	MEN_OPEN,
	MEN_SAVE,
	MEN_SAVEAS,
	MEN_RENDERSETTINGS,
	MEN_RENDERWINDOW,
	MEN_RENDER,
	MEN_FILESEP,
	MEN_FILE1,
	MEN_FILE2,
	MEN_FILE3,
	MEN_FILE4,
#ifdef __STATISTICS__
	MEN_STATISTICS,
#endif
	MEN_ABOUT,
	MEN_QUIT,

	MEN_UNDO,
	MEN_REDO,
	MEN_CUT,
	MEN_COPY,
	MEN_PASTE,
	MEN_DELETE,
	MEN_SELALL,
	MEN_DESELALL,
	MEN_SELNEXT,
	MEN_SELPREV,
	MEN_MODE_MOVE,
	MEN_MODE_ROTATE,
	MEN_MODE_SCALE,
	MEN_MODE_WMOVE,
	MEN_MODE_WROTATE,
	MEN_MODE_WSCALE,
	MEN_MODE_WORLD,
	MEN_MODE_OBJECT,
	MEN_ACTIVE_X,
	MEN_ACTIVE_Y,
	MEN_ACTIVE_Z,

	MEN_REDRAW,
	MEN_FOCUS,
	MEN_ZOOMIN,
	MEN_ZOOMOUT,
	MEN_ZOOMFIT,
	MEN_VIEW_FRONT,
	MEN_VIEW_RIGHT,
	MEN_VIEW_TOP,
	MEN_VIEW_PERSP,
	MEN_VIEW_FOUR,
	MEN_VIEW_CAMERA,
	MEN_CAMERA2VIEWER,
	MEN_GRID,
	MEN_GRIDSNAP,
	MEN_GRIDSIZE,
	MEN_SHOWNAMES,
	MEN_SHOWCOORD,
	MEN_SHOWBRUSHES,
	MEN_SHOWTEXTURES,
	MEN_SHOWALL,
	MEN_SHOWCAMERA,
	MEN_SHOWLIGHT,
	MEN_SHOWMESH,
	MEN_SHOWSPHERE,
	MEN_SHOWBOX,
	MEN_SHOWCYLINDER,
	MEN_SHOWCONE,
	MEN_SHOWSOR,
	MEN_SHOWPLANE,
	MEN_SHOWCSG,
	MEN_GLOBAL,
	MEN_SHOWBROWSER,
	MEN_SHOWMATERIAL,

	MEN_ATTRIBUTES,
	MEN_SETTINGS,
	MEN_GROUP,
	MEN_UNGROUP,
	MEN_LOADOBJ,
	MEN_SAVEOBJ,
	MEN_SPHERE,
	MEN_BOX,
	MEN_CYLINDER,
	MEN_CONE,
	MEN_SOR,
	MEN_PLANE,
	MEN_POINTLIGHT,
	MEN_SPOTLIGHT,
	MEN_DIRECTIONALLIGHT,
	MEN_CAMERA,
	MEN_CSG,
	MEN_AXIS,
	MEN_MESHCUBE,
	MEN_MESHSPHERE,
	MEN_MESHPLANE,
	MEN_MESHTUBE,
	MEN_MESHCONE,
	MEN_MESHTORUS,
	MEN_ACTIVECAMERA,

	MEN_MESH_JOIN,

	MEN_MATERIAL_NEW,
	MEN_MATERIAL_LOADNEW,
	MEN_MATERIAL_REMOVEUNUSED,
	MEN_MATERIAL_REMOVEIDENTICAL,

	MEN_PREVIEW,

	MEN_PREFS,
	MEN_MUI,
	MEN_LOADPREFS,
	MEN_SAVEPREFS
};

// active gadgets (values vor MUIA_Active_Gadget)
enum
{
	GADGET_FRONT,GADGET_RIGHT,GADGET_TOP,GADGET_PERSP,GADGET_FOUR,
	GADGET_CAMERA,GADGET_MOVE,GADGET_ROTATE,GADGET_SCALE,GADGET_WMOVE,
	GADGET_WROTATE,GADGET_WSCALE,GADGET_SELBOX,GADGET_X_TOGGLE,
	GADGET_Y_TOGGLE,GADGET_Z_TOGGLE,GADGET_X,GADGET_Y,GADGET_Z
};

struct MainWindow_Data
{
	int filenum;
	Object *filesep;
	char file[4][256];
	char filemenu[4][24];

	Object *strip;
	Object *bnew,
		*bloa,
		*bsav,
		*beditpoints,
		*baddpoints,
		*beditedges,
		*baddedges,
		*beditfaces,
		*baddfaces,
		*bfro,
		*brig,
		*btop,
		*bper,
		*bcam,
		*bfor,
		*bbou,
		*bwir,
		*bsol,
		*brot,
		*bloc,
		*bmov,
		*bsca,
		*bwrot,
		*bwmov,
		*bwsca,
		*brect,
		*bx,
		*by,
		*bz;
	Object *drawarea,
		*middlegroup,
		*browsergroup, *browser,
		*materialgroup, *material,
		*animbar, *animgroup, *anim,
		*selobj,
		*camera,
		*cx,
		*cy,
		*cz;

	Object *globalwin, *rendsetwin, *gridsizewin;      // set if the windows are open
};

SAVEDS ASM ULONG MainWindow_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

struct MUIP_MainWindow_OpenPlugin
{
	ULONG MethodID;
	PLUGIN *plugin;
};


#endif
