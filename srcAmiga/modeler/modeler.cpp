/***************
 * PROGRAM:       Modeler
 * NAME:          modeler.cpp
 * DESCRIPTION:   main module
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    30.11.95 ah    initial release
 *    16.12.96 ah    added undo
 *    30.01.97 ah    added redo
 ***************/

#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <libraries/iffparse.h>
#include <libraries/gadtools.h>
#include <libraries/listtree_mcc.h>
#include <exec/execbase.h>
#include <wbstartup.h>

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#include <wbstartup.h>
#include <workbench/startup.h>
#include <pragma/exec_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/dos_lib.h>
#include <pragma/icon_lib.h>
#ifdef __MIXEDBINARY__
#include <pragma/powerpc_lib.h>
#include <powerpc/powerpc.h>
extern struct Library *PowerPCBase;
#endif

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MAINWINDOW_CLASS_H
#include "mainwindow_class.h"
#endif

#ifndef ATTRSWINDOW_CLASS_H
#include "attrswindow_class.h"
#endif

#ifndef GLOBALWINDOW_CLASS_H
#include "globalwindow_class.h"
#endif

#ifndef RENDERWINDOW_CLASS_H
#include "renderwindow_class.h"
#endif

#ifndef DRAW_CLASS_H
#include "draw_class.h"
#endif

#ifndef COLOR_CLASS_H
#include "color_class.h"
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

#ifndef PREFSWINDOW_CLASS_H
#include "prefswindow_class.h"
#endif

#ifndef SETTINGS_CLASS_H
#include "settings_class.h"
#endif

#ifndef MESHADDWINDOW_CLASS_H
#include "meshaddwindow_class.h"
#endif

#ifndef STATEWINDOW_CLASS_H
#include "statewindow_class.h"
#endif

#ifndef DISPWINDOW_CLASS_H
#include "dispwindow_class.h"
#endif

#ifndef RENDER_CLASS_H
#include "render_class.h"
#endif

#ifndef REQUVIEW_CLASS_H
#include "requview_class.h"
#endif

#ifndef NUMREQ_CLASS_H
#include "numreq_class.h"
#endif

#ifndef SLIDER_CLASS_H
#include "slider_class.h"
#endif

#ifndef DIALOG_CLASS_H
#include "Dialog_class.h"
#endif

#ifdef __STATISTICS__
#ifndef STATISTICS_CLASS_H
#include "statistics_class.h"
#endif
#endif

#ifndef MATERIAL_CLASS_H
#include "material_class.h"
#endif

#ifndef MATERIALITEM_CLASS_H
#include "materialitem_class.h"
#endif

#ifndef FLOAT_CLASS_H
#include "float_class.h"
#endif

#ifndef MSGHANDLER_CLASS_H
#include "msghandler_class.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef MUIUTILITY_H
#include "MUIutility.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef ERRORS_H
#include "errors.h"
#endif

#ifndef UNDO_H
#include "undo.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef AREXX_H
#include "arexx.h"
#endif

#ifndef PLUGINS_H
#include "plugins.h"
#endif

UBYTE vers[] = "\0$VER: RayStorm Scenario "__VERS__" ("__DATE__")";

struct Library *GfxBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *UtilityBase = NULL;
struct Library *IFFParseBase = NULL;
struct Library *IntuitionBase = NULL;
struct Library *AslBase = NULL;
struct Library *RayStormBase = NULL;
struct Library *KeyfileBase = NULL;
struct Library *CyberGfxBase = NULL;
extern struct ExecBase *SysBase;

#ifdef __MIXEDBINARY__
extern "C" struct Library *glppcBase;
extern "C" struct Library *gluppcBase;
extern "C" struct Library *glutppcBase;
#else // __MIXEDBINARY__
extern "C" struct Library *glBase;
extern "C" struct Library *gluBase;
extern "C" struct Library *glutBase;
#endif // __MIXEDBINARY__

struct NewMenu MainMenu[] =
{
	{ NM_TITLE, "Project"            , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "New"                , 0 ,0,0,(APTR)MEN_NEW},
	{ NM_ITEM , "Open..."            ,"O",0,0,(APTR)MEN_OPEN},
	{ NM_ITEM , "Save"               ,"S",NM_ITEMDISABLED,0,(APTR)MEN_SAVE},
	{ NM_ITEM , "Save as..."         , 0 ,0,0,(APTR)MEN_SAVEAS},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Render Settings..." , 0 ,0,0,(APTR)MEN_RENDERSETTINGS},
	{ NM_ITEM , "Set Render Window"  , 0 ,MENUTOGGLE|CHECKIT|NM_ITEMDISABLED,0,(APTR)MEN_RENDERWINDOW},
	{ NM_ITEM , "Render..."          ,"E",0,0,(APTR)MEN_RENDER},
#ifdef __STATISTICS__
	{ NM_ITEM , "Statistics..."      , 0 ,0,0,(APTR)MEN_STATISTICS},
#endif
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "About..."           ,"?",0,0,(APTR)MEN_ABOUT},
	{ NM_ITEM , "Quit"               ,"Q",0,0,(APTR)MEN_QUIT},

	{ NM_TITLE, "Edit"               , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Undo"               ,"Z",NM_ITEMDISABLED,0,(APTR)MEN_UNDO},
	{ NM_ITEM , "Redo"               ,"Y",NM_ITEMDISABLED,0,(APTR)MEN_REDO},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Cut"                ,"X",0,0,(APTR)MEN_CUT},
	{ NM_ITEM , "Copy"               ,"C",0,0,(APTR)MEN_COPY},
	{ NM_ITEM , "Paste"              ,"V",0,0,(APTR)MEN_PASTE},
	{ NM_ITEM , "Delete"             ,"D",0,0,(APTR)MEN_DELETE},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Select all"         , 0 ,0,0,(APTR)MEN_SELALL},
	{ NM_ITEM , "Deselect all"       , 0 ,0,0,(APTR)MEN_DESELALL},
	{ NM_ITEM , "Select next"        ,"N",0,0,(APTR)MEN_SELNEXT},
	{ NM_ITEM , "Select previous"    ,"P",0,0,(APTR)MEN_SELPREV},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Mode"               , 0 ,0,0,},
	{ NM_SUB  , "Move Object"        , 0 ,CHECKIT,        2|4|16|32|64,(APTR)MEN_MODE_MOVE},
	{ NM_SUB  , "Rotate Object"      , 0 ,CHECKIT,        1|4|16|32|64,(APTR)MEN_MODE_ROTATE},
	{ NM_SUB  , "Scale Object"       , 0 ,CHECKIT,        1|2|16|32|64,(APTR)MEN_MODE_SCALE},
	{ NM_SUB , NM_BARLABEL           , 0 ,0,0,(APTR)0},
	{ NM_SUB  , "Move World"         , 0 ,CHECKIT,        1|2|4|32|64,(APTR)MEN_MODE_WMOVE},
	{ NM_SUB  , "Rotate World"       , 0 ,CHECKIT|CHECKED,1|2|4|16|64,(APTR)MEN_MODE_WROTATE},
	{ NM_SUB  , "Zoom World"         , 0 ,CHECKIT,        1|2|4|16|32,(APTR)MEN_MODE_WSCALE},
	{ NM_ITEM , "Active"             , 0 ,0,0,},
	{ NM_SUB  , "X"                  , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_ACTIVE_X},
	{ NM_SUB  , "Y"                  , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_ACTIVE_Y},
	{ NM_SUB  , "Z"                  , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_ACTIVE_Z},

	{ NM_TITLE, "View"               , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Redraw"             ,"R",0,0,(APTR)MEN_REDRAW},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Focus"              , 0 ,0,0,(APTR)MEN_FOCUS},
	{ NM_ITEM , "Zoom in"            ,"+",0,0,(APTR)MEN_ZOOMIN},
	{ NM_ITEM , "Zoom out"           ,"-",0,0,(APTR)MEN_ZOOMOUT},
	{ NM_ITEM , "Zoom fit"           ,"F",0,0,(APTR)MEN_ZOOMFIT},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "View"               , 0 ,0,0,0},
	{ NM_SUB  , "Front"              , 0 ,CHECKIT,        2|4|8|16,(APTR)MEN_VIEW_FRONT},
	{ NM_SUB  , "Right"              , 0 ,CHECKIT,        1|4|8|16,(APTR)MEN_VIEW_RIGHT},
	{ NM_SUB  , "Top"                , 0 ,CHECKIT,        1|2|8|16,(APTR)MEN_VIEW_TOP},
	{ NM_SUB  , "Perspective"        , 0 ,CHECKIT|CHECKED,1|2|4|16,(APTR)MEN_VIEW_PERSP},
	{ NM_SUB  , "Quad"               , 0 ,CHECKIT,        1|2|4|8, (APTR)MEN_VIEW_FOUR},
	{ NM_SUB  , "Camera"             , 0 ,CHECKIT,        0,       (APTR)MEN_VIEW_CAMERA},
	{ NM_ITEM , "Camera to viewer"   , 0 ,0,0,(APTR)MEN_CAMERA2VIEWER},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Grid"               , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_GRID},
	{ NM_ITEM , "Snap to grid"       , 0 ,MENUTOGGLE|CHECKIT,0,(APTR)MEN_GRIDSNAP},
	{ NM_ITEM , "Gridsize..."        , 0 ,0,0,(APTR)MEN_GRIDSIZE},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Show"               , 0 ,0,0,},
	{ NM_SUB  , "Coordinates"        , 0 ,MENUTOGGLE|CHECKIT,0,(APTR)MEN_SHOWCOORD},
	{ NM_SUB  , "Names"              , 0 ,MENUTOGGLE|CHECKIT,0,(APTR)MEN_SHOWNAMES},
	{ NM_SUB  , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_SUB  , "All objects"        , 0 ,0,0,(APTR)MEN_SHOWALL},
	{ NM_SUB  , "Cameras"            , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWCAMERA},
	{ NM_SUB  , "Brushes"            , 0 ,MENUTOGGLE|CHECKIT,0,(APTR)MEN_SHOWBRUSHES},
	{ NM_SUB  , "Textures"           , 0 ,MENUTOGGLE|CHECKIT,0,(APTR)MEN_SHOWTEXTURES},
	{ NM_SUB  , "Lights"             , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWLIGHT},
	{ NM_SUB  , "Meshes"             , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWMESH},
	{ NM_SUB  , "Spheres"            , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWSPHERE},
	{ NM_SUB  , "Boxes"              , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWBOX},
	{ NM_SUB  , "Cylinders"          , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWCYLINDER},
	{ NM_SUB  , "Cones"              , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWCONE},
	{ NM_SUB  , "SOR"                , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWSOR},
	{ NM_SUB  , "Planes"             , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWPLANE},
	{ NM_SUB  , "CSG"                , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWCSG},
	{ NM_ITEM , "Global settings..." , 0 ,0,0,(APTR)MEN_GLOBAL},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Browser"            , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWBROWSER},
	{ NM_ITEM , "Material manager"   , 0 ,MENUTOGGLE|CHECKIT|CHECKED,0,(APTR)MEN_SHOWMATERIAL},

	{ NM_TITLE, "Objects"            , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Attributes..."      ,"A",0,0,(APTR)MEN_ATTRIBUTES},
	{ NM_ITEM , "Settings..."        ,"T",0,0,(APTR)MEN_SETTINGS},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Group"              ,"G",0,0,(APTR)MEN_GROUP},
	{ NM_ITEM , "Ungroup"            ,"U",0,0,(APTR)MEN_UNGROUP},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Load..."            ,"L",0,0,(APTR)MEN_LOADOBJ},
	{ NM_ITEM , "Save..."            ,"W",0,0,(APTR)MEN_SAVEOBJ},
	{ NM_ITEM , "Primitives"         , 0 ,0,0,0},
	{ NM_SUB  , "Sphere"             , 0 ,0,0,(APTR)MEN_SPHERE},
	{ NM_SUB  , "Box"                , 0 ,0,0,(APTR)MEN_BOX},
	{ NM_SUB  , "Cylinder"           , 0 ,0,0,(APTR)MEN_CYLINDER},
	{ NM_SUB  , "Cone"               , 0 ,0,0,(APTR)MEN_CONE},
	{ NM_SUB  , "SOR"                , 0 ,0,0,(APTR)MEN_SOR},
	{ NM_SUB  , "Plane"              , 0 ,0,0,(APTR)MEN_PLANE},
	{ NM_SUB  , "Camera"             , 0 ,0,0,(APTR)MEN_CAMERA},
	{ NM_SUB  , "CSG"                , 0 ,0,0,(APTR)MEN_CSG},
	{ NM_ITEM , "Lights"             , 0 ,0,0,0},
	{ NM_SUB  , "Directional light"  , 0 ,0,0,(APTR)MEN_DIRECTIONALLIGHT},
	{ NM_SUB  , "Point light"        , 0 ,0,0,(APTR)MEN_POINTLIGHT},
	{ NM_SUB  , "Spot light "        , 0 ,0,0,(APTR)MEN_SPOTLIGHT},
	{ NM_ITEM , "Meshes"             , 0 ,0,0,0},
	{ NM_SUB  , "Axis"               , 0 ,0,0,(APTR)MEN_AXIS},
	{ NM_SUB  , "Sphere"             , 0 ,0,0,(APTR)MEN_MESHSPHERE},
	{ NM_SUB  , "Box"                , 0 ,0,0,(APTR)MEN_MESHCUBE},
	{ NM_SUB  , "Cylinder"           , 0 ,0,0,(APTR)MEN_MESHTUBE},
	{ NM_SUB  , "Cone"               , 0 ,0,0,(APTR)MEN_MESHCONE},
	{ NM_SUB  , "Plane"              , 0 ,0,0,(APTR)MEN_MESHPLANE},
	{ NM_SUB  , "Torus"              , 0 ,0,0,(APTR)MEN_MESHTORUS},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Active Camera..."   , 0 ,0,0,(APTR)MEN_ACTIVECAMERA},

	{ NM_TITLE, "Mesh"               , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Join"               , 0 ,0,0,(APTR)MEN_MESH_JOIN},

	{ NM_TITLE, "Material"           , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "New"                , 0 ,0,0,(APTR)MEN_MATERIAL_NEW},
	{ NM_ITEM , "Load new..."        , 0 ,0,0,(APTR)MEN_MATERIAL_LOADNEW},
	{ NM_ITEM , "Remove unused"      , 0 ,0,0,(APTR)MEN_MATERIAL_REMOVEUNUSED},
	{ NM_ITEM , "Remove identical"   , 0 ,0,0,(APTR)MEN_MATERIAL_REMOVEIDENTICAL},

	{ NM_TITLE, "Plugins"            , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "-"                  , 0 ,NM_ITEMDISABLED,0,(APTR)MEN_PREVIEW},

	{ NM_TITLE, "Settings"           , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Prefs..."           , 0 ,0,0,(APTR)MEN_PREFS},
	{ NM_ITEM , "MUI..."             , 0 ,0,0,(APTR)MEN_MUI},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)0},
	{ NM_ITEM , "Load"               , 0 ,0,0,(APTR)MEN_LOADPREFS},
	{ NM_ITEM , "Save"               , 0 ,0,0,(APTR)MEN_SAVEPREFS},

	{ NM_END,NULL,0,0,0,(APTR)0 },
};

struct MUI_CustomClass *CL_AttrsWindow;
struct MUI_CustomClass *CL_MainWindow;
struct MUI_CustomClass *CL_PrefsWindow;
struct MUI_CustomClass *CL_Settings;
struct MUI_CustomClass *CL_GlobalWindow;
struct MUI_CustomClass *CL_RenderWindow;
struct MUI_CustomClass *CL_MeshAddWindow;
struct MUI_CustomClass *CL_StateWindow;
struct MUI_CustomClass *CL_DispWindow;
struct MUI_CustomClass *CL_NumReq;
struct MUI_CustomClass *CL_Draw;
struct MUI_CustomClass *CL_Color;
struct MUI_CustomClass *CL_Browser;
struct MUI_CustomClass *CL_BrowserTree;
struct MUI_CustomClass *CL_Render;
struct MUI_CustomClass *CL_RequView;
struct MUI_CustomClass *CL_StringSlider;
#ifdef __STATISTICS__
struct MUI_CustomClass *CL_Statistics;
#endif
struct MUI_CustomClass *CL_Material;
struct MUI_CustomClass *CL_MaterialItem;
struct MUI_CustomClass *CL_Float;
struct MUI_CustomClass *CL_MsgHandler;
struct MUI_CustomClass *CL_AnimTree;
struct MUI_CustomClass *CL_Dialog;
struct MUI_CustomClass *CL_AnimBar;
struct MUI_CustomClass *CL_PluginWindow;

Object *app, *msghandler;

/*************
 * FUNCTION:      Init
 * DESCRIPTION:   Open libs and screen
 * INPUT:         none
 * OUTPUT:        error code if failed else 0
 *************/
static int Init()
{
#ifdef __MIXEDBINARY__
	int err;
#endif // __MIXEDBINARY__
	rsiLIBINFO info;

#ifdef __MIXEDBINARY__
	// Open powerpc library.
	PowerPCBase = OpenLibrary((unsigned char*)"powerpc.library", 8L);
	if (!PowerPCBase)
		return ERR_POWERPCLIB;
	else
		CloseLibrary(PowerPCBase);
#endif // __MIXEDBINARY__
	// Open iffparse library.
	IFFParseBase = OpenLibrary((unsigned char*)"iffparse.library",0);
	if (!IFFParseBase)
		return ERR_IFFPARSELIB;
	// Open gfx library.
	GfxBase = OpenLibrary((unsigned char*)"graphics.library",36L);
	if (!GfxBase)
		return ERR_GRAPHICSLIB;
	// Open intuition library.
	IntuitionBase = OpenLibrary((unsigned char*)"intuition.library",0);
	if (!IntuitionBase)
		return ERR_INTUITIONLIB;
	// Open utility library.
	UtilityBase = OpenLibrary((unsigned char*)"utility.library",36L);
	if (!UtilityBase)
		return ERR_UTILITYLIB;
	// Open asl library.
	AslBase = OpenLibrary((unsigned char*)"asl.library",0);
	if (!AslBase)
		return ERR_ASLLIB;
	// Open raystorm library.
	RayStormBase = OpenLibrary(RAYSTORMNAME,2L);
	if(!RayStormBase)
		return ERR_RAYSTORMLIB;
	if(rsiGetLibInfo(&info) != rsiERR_NONE)
		return ERR_RAYSTORMLIB;
#ifndef __MIXEDBINARY__
	if(info.system != rsiSYSTEM_M68K)
	{
		CloseLibrary(RayStormBase);
		RayStormBase = NULL;
		return ERR_NON_M68K_RSILIB;
	}
	if(rsiInit() != rsiERR_NONE)
		return ERR_RAYSTORMLIB;
#else
	if(info.system != rsiSYSTEM_PPC_WarpOS)
	{
		CloseLibrary(RayStormBase);
		RayStormBase = NULL;
		return ERR_NON_PPC_RSILIB;
	}
	err = ppcRayStormLibOpen();
	if(err != ERR_NONE)
		return err;
#endif
	// Open keyfile library.
	KeyfileBase = OpenLibrary((unsigned char*)"s:RayStorm.key",0);
	CyberGfxBase = OpenLibrary((unsigned char*)"cybergraphics.library",0);

	/* init costum classes */
	CL_MainWindow = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct MainWindow_Data),MainWindow_Dispatcher);
	if(!CL_MainWindow)
		return ERR_MUICLASS;
	CL_Draw = MUI_CreateCustomClass(NULL,MUIC_Area,NULL,sizeof(struct Draw_Data),Draw_Dispatcher);
	if(!CL_Draw)
		return ERR_MUICLASS;
	CL_BrowserTree = MUI_CreateCustomClass(NULL,MUIC_Listtree,NULL,sizeof(struct BrowserTree_Data),BrowserTree_Dispatcher);
	if(!CL_BrowserTree)
		return ERR_MUICLASS;
	CL_Material = MUI_CreateCustomClass(NULL,MUIC_Virtgroup,NULL,sizeof(struct Material_Data),Material_Dispatcher);
	if(!CL_Material)
		return ERR_MUICLASS;
	CL_MaterialItem = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct MaterialItem_Data),MaterialItem_Dispatcher);
	if(!CL_MaterialItem)
		return ERR_MUICLASS;
	CL_AttrsWindow = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct AttrsWindow_Data),AttrsWindow_Dispatcher);
	if(!CL_AttrsWindow)
		return ERR_MUICLASS;
	CL_GlobalWindow = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct GlobalWindow_Data),GlobalWindow_Dispatcher);
	if(!CL_GlobalWindow)
		return ERR_MUICLASS;
	CL_RenderWindow = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct RenderWindow_Data),RenderWindow_Dispatcher);
	if(!CL_RenderWindow)
		return ERR_MUICLASS;
	CL_Color = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct Color_Data),Color_Dispatcher);
	if(!CL_Color)
		return ERR_MUICLASS;
	CL_Settings = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct Settings_Data),Settings_Dispatcher);
	if(!CL_Settings)
		return ERR_MUICLASS;
	CL_PrefsWindow = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct PrefsWindow_Data),PrefsWindow_Dispatcher);
	if(!CL_PrefsWindow)
		return ERR_MUICLASS;
	CL_MeshAddWindow = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct MeshAddWindow_Data),MeshAddWindow_Dispatcher);
	if(!CL_MeshAddWindow)
		return ERR_MUICLASS;
	CL_NumReq = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct NumReq_Data),NumReq_Dispatcher);
	if(!CL_NumReq)
		return ERR_MUICLASS;
	CL_Browser = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct Browser_Data),Browser_Dispatcher);
	if(!CL_Browser)
		return ERR_MUICLASS;
	CL_StateWindow = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct StateWindow_Data),StateWindow_Dispatcher);
	if(!CL_StateWindow)
		return ERR_MUICLASS;
	CL_DispWindow = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct DispWindow_Data),DispWindow_Dispatcher);
	if(!CL_DispWindow)
		return ERR_MUICLASS;
	CL_Render = MUI_CreateCustomClass(NULL,MUIC_Area,NULL,sizeof(struct Render_Data),Render_Dispatcher);
	if(!CL_Render)
		return ERR_MUICLASS;
	CL_RequView = MUI_CreateCustomClass(NULL,MUIC_Area,NULL,sizeof(struct RequView_Data),RequView_Dispatcher);
	if(!CL_RequView)
		return ERR_MUICLASS;
	CL_StringSlider = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct StringSlider_Data),StringSlider_Dispatcher);
	if(!CL_StringSlider)
		return ERR_MUICLASS;
#ifdef __STATISTICS__
	CL_Statistics = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct Statistics_Data),Statistics_Dispatcher);
	if(!CL_Statistics)
		return ERR_MUICLASS;
#endif
	CL_Float = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Float_Data), Float_Dispatcher);
	if(!CL_Float)
		return ERR_MUICLASS;
	CL_MsgHandler = MUI_CreateCustomClass(NULL,MUIC_Notify,NULL,sizeof(struct MsgHandler_Data),MsgHandler_Dispatcher);
	if(!CL_MsgHandler)
		return ERR_MUICLASS;
	CL_AnimTree = MUI_CreateCustomClass(NULL,MUIC_Listtree,NULL,sizeof(struct AnimTree_Data), AnimTree_Dispatcher);
	if(!CL_AnimTree)
		return ERR_MUICLASS;
	CL_Dialog = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Dialog_Data), Dialog_Dispatcher);
	if(!CL_Dialog)
		return ERR_MUICLASS;
	CL_AnimBar = MUI_CreateCustomClass(NULL,MUIC_Group, NULL, sizeof(struct AnimBar_Data), AnimBar_Dispatcher);
	if(!CL_AnimBar)
		return ERR_MUICLASS;
	CL_PluginWindow = MUI_CreateCustomClass(NULL, MUIC_Window, NULL, sizeof(struct PluginWindow_Data), PluginWindow_Dispatcher);
	if(!CL_PluginWindow)
		return ERR_MUICLASS;

	sciLoadPrefs();

	return 0;
}

/*************
 * FUNCTION:      Cleanup
 * DESCRIPTION:   close libs and windows
 * INPUT:         none
 * OUTPUT:        none
 *************/
void Cleanup()
{
	sciFreeAll();
	FreePlugins();

	if (CL_MainWindow)
		MUI_DeleteCustomClass(CL_MainWindow);
	if (CL_Draw)
		MUI_DeleteCustomClass(CL_Draw);
	if (CL_AttrsWindow)
		MUI_DeleteCustomClass(CL_AttrsWindow);
	if (CL_GlobalWindow)
		MUI_DeleteCustomClass(CL_GlobalWindow);
	if (CL_RenderWindow)
		MUI_DeleteCustomClass(CL_RenderWindow);
	if (CL_Color)
		MUI_DeleteCustomClass(CL_Color);
	if (CL_PrefsWindow)
		MUI_DeleteCustomClass(CL_PrefsWindow);
	if (CL_Settings)
		MUI_DeleteCustomClass(CL_Settings);
	if (CL_MeshAddWindow)
		MUI_DeleteCustomClass(CL_MeshAddWindow);
	if (CL_NumReq)
		MUI_DeleteCustomClass(CL_NumReq);
	if (CL_Browser)
		MUI_DeleteCustomClass(CL_Browser);
	if (CL_BrowserTree)
		MUI_DeleteCustomClass(CL_BrowserTree);
	if (CL_StateWindow)
		MUI_DeleteCustomClass(CL_StateWindow);
	if (CL_DispWindow)
		MUI_DeleteCustomClass(CL_DispWindow);
	if (CL_Render)
		MUI_DeleteCustomClass(CL_Render);
	if (CL_RequView)
		MUI_DeleteCustomClass(CL_RequView);
	if (CL_StringSlider)
		MUI_DeleteCustomClass(CL_StringSlider);
#ifdef __STATISTICS__
	if (CL_Statistics)
		MUI_DeleteCustomClass(CL_Statistics);
#endif
	if (CL_Material)
		MUI_DeleteCustomClass(CL_Material);
	if (CL_MaterialItem)
		MUI_DeleteCustomClass(CL_MaterialItem);
	if (CL_Float)
		MUI_DeleteCustomClass(CL_Float);
	if (CL_MsgHandler)
		MUI_DeleteCustomClass(CL_MsgHandler);
	if (CL_AnimTree)
		MUI_DeleteCustomClass(CL_AnimTree);
	if (CL_Dialog)
		MUI_DeleteCustomClass(CL_Dialog);
	if (CL_AnimBar)
		MUI_DeleteCustomClass(CL_AnimBar);
	if (CL_PluginWindow)
		MUI_DeleteCustomClass(CL_PluginWindow);

	if(CyberGfxBase)
		CloseLibrary(CyberGfxBase);
	if (KeyfileBase)
		CloseLibrary(KeyfileBase);
	if (RayStormBase)
	{
#ifndef __MIXEDBINARY__
		rsiExit();
#else
		ppcRayStormLibClose();
#endif // __MIXEDBINARY__
		CloseLibrary(RayStormBase);
	}
	if (AslBase)
		CloseLibrary(AslBase);
	if (IntuitionBase)
		CloseLibrary(IntuitionBase);
	if (IFFParseBase)
		CloseLibrary(IFFParseBase);
	if (UtilityBase)
		CloseLibrary(UtilityBase);
	if (MUIMasterBase)
		CloseLibrary(MUIMasterBase);
	if (GfxBase)
		CloseLibrary(GfxBase);
}

/*************
 * FUNCTION:      main
 * DESCRIPTION:   scenario main function
 * INPUT:         arguments
 * OUTPUT:        -
 *************/
void main(int argc, char *argv[])
{
	int error=0;
	ULONG sigs=0, open, iconified;
	Object *mainwin, *strip;
	BOOL done = FALSE;
	int len;
	struct DiskObject *dobj;
	SURFACE *surf;
	BOOL opengl_supported;

	// test processor version
#ifdef __020__
	if(!((SysBase->AttnFlags & AFF_68020) || (SysBase->AttnFlags & AFF_68030) || (SysBase->AttnFlags & AFF_68040)))
	{
		error = ERR_NO020;
	}
#endif
#ifdef __881__
	if(!(((SysBase->AttnFlags & AFF_68020) || (SysBase->AttnFlags & AFF_68030) || (SysBase->AttnFlags & AFF_68040))) &&
		 ((SysBase->AttnFlags & AFF_68881) || (SysBase->AttnFlags & AFF_68882)))
	{
		error = ERR_NO881;
	}
#endif
#ifdef __040__
	if(!((SysBase->AttnFlags & AFF_68040) &&
		 ((SysBase->AttnFlags & AFF_68881) || (SysBase->AttnFlags & AFF_68882) || (SysBase->AttnFlags & AFF_FPU40))))
	{
		error = ERR_NO040;
	}
#endif
#ifdef __060__
	if(!((SysBase->AttnFlags & AFF_68040) &&
		 ((SysBase->AttnFlags & AFF_68881) || (SysBase->AttnFlags & AFF_68882) || (SysBase->AttnFlags & AFF_FPU40))))
	{
		error = ERR_NO060;
	}
#endif
#ifdef __MIXEDBINARY__
	if(PowerPCBase)
	{
		if(!(GetCPU() & (CPUF_603 | CPUF_603E | CPUF_604 | CPUF_604E | CPUF_620)))
			error = ERR_NOPPC;
	}
	else
		error = ERR_NOPPC;
#endif
	// Open muimaster library.
	MUIMasterBase = OpenLibrary((unsigned char*)MUIMASTER_NAME,12L);
	if (!MUIMasterBase)
		error = ERR_MUIMASTERLIB;

	if(!error)
	{
		error = Init();
		if(!error)
		{
			// Test if StormMesa has been successfully opened
#ifdef __MIXEDBINARY__
			if(glppcBase && gluppcBase && glutppcBase)
#else // __MIXEDBINARY__
			if(glBase && gluBase && glutBase)
#endif // __MIXEDBINARY__
				opengl_supported = TRUE;
			else
				opengl_supported = TRUE;

			app = ApplicationObject,
				MUIA_Application_Title      , "RayStorm Scenario",
				MUIA_Application_Version    , &vers[25],
				MUIA_Application_Copyright  , "©1996-98 by Andreas Heumann and Mike Hesser",
				MUIA_Application_Author     , "Andreas Heumann",
				MUIA_Application_Description, "Modeler for RayStorm package",
				MUIA_Application_Base       , "SCENARIO",
				MUIA_Application_HelpFile   , "PROGDIR:docs/RayStorm.guide",
				MUIA_Application_Commands   , arexxcmds,
				MUIA_Application_DiskObject , dobj = GetDiskObject("PROGDIR:Scenario"),
				MUIA_Application_Window     ,
					mainwin = (Object *)NewObject(
					CL_MainWindow->mcc_Class, NULL,
					MUIA_Menustrip, strip,
					MUIA_OpenGL_Supported, opengl_supported,
					TAG_DONE),
				MUIA_Application_Menustrip  , strip = MUI_MakeObject(MUIO_MenustripNM,MainMenu,0),
			End;

			if(app)
			{
				// create the subtask message handler
				msghandler = (Object *)NewObject(CL_MsgHandler->mcc_Class,NULL,TAG_DONE);
				if(msghandler)
				{
					// scan for plugins
					ScanPlugins(strip, mainwin);
					// create the default surface
					surf = sciCreateSurface(SURF_DEFAULTNAME);
					if(surf)
					{
						surf->flags |= SURF_DEFAULT;

						SetAttrs(mainwin,MUIA_Window_Open,TRUE,TAG_DONE);
						GetAttr(MUIA_Window_Open,mainwin,&open);
						// if the window is not open, maybe it is iconified?
						GetAttr(MUIA_Application_Iconified, app, &iconified);
						if(!open && !iconified)
						{
							// it's not open and it's not iconfied then we have a big problem
							utility.Request("Failed to open main window");
						}
						else
						{
							// initialize utility functions
							utility.InitMainWin(mainwin, strip);
							sciInitDisplay();

							if(argc > 1)
							{
								// open project
								len = strlen(argv[1])-1;
								if(argv[1][len] == '/')
									argv[1][len] = 0;
								DoMethod(mainwin, MUIM_MainWindow_OpenQuiet, argv[1]);
							}
							else
								sciBrowserBuild();

							while(!done)
							{
								switch(DoMethod(app,MUIM_Application_NewInput,&sigs))
								{
									case MUIV_Application_ReturnID_Quit:
										if(GetProjectChanged())
										{
											if(MUI_Request(app,mainwin,0,NULL,"*_Yes|_No","Really quit\n(changes will be lost) ?"))
												done = TRUE;
										}
										else
											done = TRUE;
										break;
								}
								if(!done && sigs)
									sigs = Wait(sigs);
							}
						}
						MUI_DisposeObject(app);
						if(dobj)
							FreeDiskObject(dobj);
					}
					else
						error = ERR_MEM;
				}
				else
					error = ERR_MEM;
			}
			else
				error = ERR_OPENWINDOW;
		}
	}
	if(error)
	{
		if(MUIMasterBase)
			MUI_Request(app,NULL,0,NULL,"Ok",errors[error]);
		else
			cout << errors[error] << endl;
	}
	utility.InitMainWin(NULL, NULL);
	Cleanup();
}

#ifdef __STORM__
void wbmain(struct WBStartup *msg)
{
	BPTR lock;
	char name[256];
	char buffer[256];
	char *argv[2];

	// get a lock to the program directory
	lock = GetProgramDir();
	// make program directory to current directory
	lock = CurrentDir(lock);

	if(msg)
	{
		if(msg->sm_ArgList[0].wa_Lock)
			NameFromLock(msg->sm_ArgList[0].wa_Lock, name, 256);
		else
			strcpy(name, msg->sm_ArgList[0].wa_Name);

		argv[0] = name;

		if(msg->sm_NumArgs > 1)
		{
			if(msg->sm_ArgList[1].wa_Lock)
				NameFromLock(msg->sm_ArgList[1].wa_Lock, buffer, 256);
			else
				strcpy(buffer, msg->sm_ArgList[1].wa_Name);
			argv[1] = buffer;
			main(2, argv);
		}
		else
		{
			main(1, argv);
		}
	}
	else
		main(0, NULL);

	// change current dir to old dir
	lock = CurrentDir(lock);

	exit(0);
}
#endif
