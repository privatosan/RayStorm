/***************
 * PROGRAM:       Modeler
 * NAME:          MUIdefs.h
 * DESCRIPTION:   definitions for MUI
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    11.01.97 ah    added box message
 *    30.01.97 ah    added redo
 *    06.03.97 ah    added ImageButtonExt
 *    22.03.97 ah    added BrowserTree and BodyChunk
 *    25.10.97 ah    added AnimTree
 *    22.11.98 ah    added AnimBar
 ***************/

#ifndef MUIDEFS_H
#define MUIDEFS_H

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#include <pragma/muimaster_lib.h>
#include <clib/alib_protos.h>

#ifdef __PPC__
#include <clib/powerpc_protos.h>
ULONG DoMethodPPC(Object *obj, Tag tag, ...);
ULONG DoMethodAPPC(Object *obj, Msg message);
ULONG DoSuperMethodAPPC(struct IClass *cl, Object *obj, Msg message);
#define DOMETHOD DoMethodPPC
#define DOMETHODA DoMethodAPPC
#define DOSUPERMETHODA DoSuperMethodAPPC
#else
#define DOMETHOD DoMethod
#define DOMETHODA DoMethodA
#define DOSUPERMETHODA DoSuperMethodA
#endif // __PPC__

extern struct MUI_CustomClass *CL_AttrsWindow;
extern struct MUI_CustomClass *CL_MainWindow;
extern struct MUI_CustomClass *CL_PrefsWindow;
extern struct MUI_CustomClass *CL_Settings;
extern struct MUI_CustomClass *CL_GlobalWindow;
extern struct MUI_CustomClass *CL_RenderWindow;
extern struct MUI_CustomClass *CL_MeshAddWindow;
extern struct MUI_CustomClass *CL_StateWindow;
extern struct MUI_CustomClass *CL_DispWindow;
extern struct MUI_CustomClass *CL_NumReq;
extern struct MUI_CustomClass *CL_Draw;
extern struct MUI_CustomClass *CL_Color;
extern struct MUI_CustomClass *CL_Browser;
extern struct MUI_CustomClass *CL_BrowserTree;
extern struct MUI_CustomClass *CL_Render;
extern struct MUI_CustomClass *CL_RequView;
extern struct MUI_CustomClass *CL_StringSlider;
#ifdef __STATISTICS__
extern struct MUI_CustomClass *CL_Statistics;
#endif
extern struct MUI_CustomClass *CL_Material;
extern struct MUI_CustomClass *CL_MaterialItem;
extern struct MUI_CustomClass *CL_Float;
extern struct MUI_CustomClass *CL_MsgHandler;
extern struct MUI_CustomClass *CL_AnimTree;
extern struct MUI_CustomClass *CL_Dialog;
extern struct MUI_CustomClass *CL_AnimBar;
extern struct MUI_CustomClass *CL_PluginWindow;

extern Object *app;

enum
{
	MUIA_MainWindow_Active_Gadget = TAG_USER,
	MUIA_MainWindow_CoordX,
	MUIA_MainWindow_CoordY,
	MUIA_MainWindow_CoordZ,
	MUIA_MainWindow_SelectedObject,
	MUIA_MainWindow_ActiveCamera,
	MUIA_MainWindow_EnableEditMesh,
	MUIA_MainWindow_EnableEditSOR,
	MUIM_MainWindow_New,
	MUIM_MainWindow_Open,
	MUIM_MainWindow_OpenQuiet,
	MUIM_MainWindow_Save,
	MUIM_MainWindow_SaveAs,
	MUIM_MainWindow_Render,
	MUIM_MainWindow_SetRenderWindow,
	MUIM_MainWindow_File1,
	MUIM_MainWindow_File2,
	MUIM_MainWindow_File3,
	MUIM_MainWindow_File4,
#ifdef __STATISTICS__
	MUIM_MainWindow_Statistics,
#endif
	MUIM_MainWindow_RenderSettings,
	MUIM_MainWindow_About,
	MUIM_MainWindow_Attributes,
	MUIM_MainWindow_Settings,
	MUIM_MainWindow_GridSize,
	MUIM_MainWindow_ShowCoord,
	MUIM_MainWindow_Global,
	MUIM_MainWindow_ShowBrowser,
	MUIM_MainWindow_ShowMaterial,
	MUIM_MainWindow_MeshCube,
	MUIM_MainWindow_MeshSphere,
	MUIM_MainWindow_MeshPlane,
	MUIM_MainWindow_MeshTube,
	MUIM_MainWindow_MeshCone,
	MUIM_MainWindow_MeshTorus,
	MUIM_MainWindow_LoadObj,
	MUIM_MainWindow_SaveObj,
	MUIM_MainWindow_ActiveCamera,
	MUIM_MainWindow_MaterialNew,
	MUIM_MainWindow_MaterialLoadNew,
	MUIM_MainWindow_MaterialRemoveUnused,
	MUIM_MainWindow_MaterialRemoveIdentical,
	MUIM_MainWindow_Prefs,
	MUIM_MainWindow_LoadPrefs,
	MUIM_MainWindow_SavePrefs,
	MUIM_MainWindow_OpenMaterialRequ,
	MUIM_MainWindow_sciDoMethod,
	MUIM_MainWindow_OpenPlugin,

	MUIA_ChildWindow_Originator,
	MUIA_Menustrip,
	MUIA_OpenGL_Supported,
	MUIM_ChildWindow_Finish,
	MUIM_Settings_Finish,
	MUIM_AttrsWindow_Finish,

	MUIA_Draw_Window,
	MUIA_Draw_ViewFour,
	MUIA_Draw_View,
	MUIA_Draw_Display,
	MUIA_Draw_GridSize,
	MUIA_Draw_CoordDisp,
	MUIA_Draw_DisplayPtr,
	MUIA_Draw_SetUpdate,
	MUIA_Draw_SelectByBox,
	MUIA_Draw_SetRenderWindow,
	MUIM_Draw_RedrawDirty,
	MUIM_Draw_RedrawSelected,
	MUIM_Draw_RedrawCurrentView,
	MUIM_Draw_OpenSettings,
	MUIM_Draw_SetFinish,
	MUIM_Draw_RemoveSelected,

	MUIA_AttrWindow_Object,
	MUIA_AttrWindow_Surface,
	MUIM_AttrWindow_Ok,
	MUIM_AttrWindow_Load,
	MUIM_AttrWindow_Save,
	MUIM_AttrWindow_OpenColor,
	MUIM_AttrWindow_BrushChanged,
	MUIM_AttrWindow_AddBrush,
	MUIM_AttrWindow_ChangeBrush,
	MUIM_AttrWindow_View,
	MUIM_AttrWindow_RemoveBrush,
	MUIM_AttrWindow_BrushRadio,
	MUIM_AttrWindow_BrushFlags,
	MUIM_AttrWindow_RTextureChanged,
	MUIM_AttrWindow_AddRTexture,
	MUIM_AttrWindow_ChangeRTexture,
	MUIM_AttrWindow_RemoveRTexture,
	MUIM_AttrWindow_ITextureChanged,
	MUIM_AttrWindow_AddITexture,
	MUIM_AttrWindow_ChangeITexture,
	MUIM_AttrWindow_RemoveITexture,
	MUIM_AttrWindow_Preview,
	MUIM_AttrWindow_Close,

	MUIA_Colorfield,

	MUIA_PopUp_List,
	MUIA_PopUp_Active,

	MUIM_Color_Set,
	MUIM_Color_Finish,

	MUIA_PrefWindow_Display,
	MUIA_PrefWindow_DrawArea,
	MUIM_PrefWindow_Ok,
	MUIM_PrefWindow_ColorChanged,
	MUIM_PrefWindow_ColorListChanged,

	MUIA_Settings_DrawArea,
	MUIA_Settings_Display,
	MUIA_Settings_Type,
	MUIA_Settings_PosLocal,
	MUIA_Settings_AlignLocal,
	MUIA_Settings_SizeLocal,
	MUIA_Settings_Pos,
	MUIA_Settings_Align,
	MUIA_Settings_Size,
	MUIA_Settings_Name,
	MUIA_Settings_Object,
	MUIA_Settings_FlareNumber,
	MUIA_Settings_Undo,
	MUIM_Settings_PopTrack,
	MUIM_Settings_StringTrack,
	MUIM_Settings_SetLens,
	MUIM_Settings_OpenColor,
	MUIM_Settings_Ok,
	MUIM_Settings_Cancel,
	MUIM_Settings_EnableFlares,
	MUIM_Settings_AddFlare,
	MUIM_Settings_RemoveFlare,
	MUIM_Settings_FlareNumber,
	MUIM_Settings_FlareUpdate,
	MUIM_Settings_EnableStar,
	MUIM_Settings_StarUpdate,
	MUIM_Settings_LoadLight,
	MUIM_Settings_SaveLight,
	MUIM_Settings_PosLocal,
	MUIM_Settings_AlignLocal,
	MUIM_Settings_SizeLocal,

	MUIM_GlobalWindow_Ok,
	MUIM_GlobalWindow_OpenColor,
	MUIM_GlobalWindow_View,

	MUIA_RenderWindow_Camera,
	MUIM_RenderWindow_Ok,
	MUIM_RenderWindow_SetRes,
	MUIM_RenderWindow_View,
	MUIM_RenderWindow_Finish,

	MUIA_StateWindow_Display,
	MUIM_StateWindow_Render,
	MUIM_StateWindow_Ok,
	MUIM_StateWindow_Show,
	MUIM_StateWindow_RenderCleanup,

	MUIA_MeshAddWindow_Type,
	MUIA_MeshAddWindow_Vector1,
	MUIA_MeshAddWindow_Float1,
	MUIA_MeshAddWindow_Float2,
	MUIA_MeshAddWindow_Int1,
	MUIA_MeshAddWindow_Int2,
	MUIA_MeshAddWindow_Flag1,
	MUIA_MeshAddWindow_Flag2,
	MUIM_MeshAddWindow_Ok,
	MUIM_MeshAddWindow_Finish,

	MUIA_NumReq_Value,
	MUIA_NumReq_Text,
	MUIM_NumReq_Set,
	MUIM_NumReq_Finish,

	MUIA_Browser_Mode,
	MUIA_Browser_DrawArea,
	MUIA_Browser_ActiveObject,
	MUIA_Browser_DontSelect,
	MUIM_Browser_Ok,
	MUIM_Browser_Finish,
	MUIM_Browser_Cancel,
	MUIM_Browser_Build,

	MUIA_BrowserTree_DrawArea,
	MUIA_BrowserTree_Mode,
	MUIA_BrowserTree_DontSelect,
	MUIA_BrowserTree_ActiveObject,
	MUIM_BrowserTree_UpdateThumbNail,
	MUIM_BrowserTree_Show,
	MUIM_BrowserTree_Select,
	MUIM_BrowserTree_Build,
	MUIM_BrowserTree_Update,
	MUIM_BrowserTree_Clear,
	MUIM_BrowserTree_AddObject,

	MUIM_DispWindow_Render,

	MUIA_Render_XRes,
	MUIA_Render_YRes,
	MUIA_Render_Data,
	MUIA_Render_Lines,
	MUIA_Render_SubTask,
	MUIA_Render_BitMap,
	MUIM_Render_ResetColors,
	MUIM_Render_DrawUpdate,

	MUIA_RequView_Display,
	MUIA_RequView_Flares,
	MUIA_RequView_Star,

	MUIA_StringSlider_Min,
	MUIA_StringSlider_Max,
	MUIA_StringSlider_Value,
	MUIA_StringSlider_LowerBound,
	MUIA_StringSlider_UpperBound,
	MUIA_StringSlider_Float,
	MUIM_StringSlider_StringAck,

	MUIA_Material_MainWindow,
	MUIM_Material_AddMaterial,
	MUIM_Material_FreeMaterials,
	MUIM_Material_UpdateMaterial,
	MUIM_Material_RemoveMaterial,
	MUIM_Material_GetThumbNail,
	MUIM_Material_SubTaskFinished,
	MUIM_Material_RecalcItem,
	MUIM_Material_RemoveRecalcItem,
	MUIM_Material_InitChange,
	MUIM_Material_ExitChange,
	MUIM_Material_StopTask,
	MUIM_Material_RemoveIdentical,
	MUIM_Material_RemoveUnused,

	MUIA_MaterialItem_Surface,
	MUIA_MaterialItem_SubTask,
	MUIM_MaterialItem_Recalc,
	MUIM_MaterialItem_Name,
	MUIM_MaterialItem_GetThumbNail,
	MUIM_MaterialItem_StopTask,
	MUIM_MaterialItem_Pressed,

	MUIM_SubTask_Finished,

	MUIA_Float_Label,
	MUIA_Float_ControlChar,
	MUIA_Float_Min,
	MUIA_Float_Max,
	MUIA_Float_Value,
	MUIA_Float_LowerBound,
	MUIA_Float_UpperBound,
	MUIA_Float_LowerInside,
	MUIA_Float_UpperInside,
	MUIM_Float_StringAck,

	MUIM_MsgHandler_AddObject,
	MUIM_MsgHandler_RemoveObject,
	MUIM_MsgHandler_PushMessage,

	MUIA_Dialog_Dialog,
	MUIA_Dialog_Data,
	MUIM_Dialog_OpenColor,
	MUIM_Dialog_SliderUpdate,
	MUIM_Dialog_FloatUpdate,
	MUIM_Dialog_IntUpdate,
	MUIM_Dialog_CheckBoxUpdate,

	MUIA_PluginWindow_Info,
	MUIA_PluginWindow_Data,
	MUIM_PluginWindow_Close,

	MUIM_Parent_Update,
};

struct MUIP_ChildWindow_Finish
{
	ULONG MethodID;
	Object *win;
	ULONG ok;
};

struct MUIP_RenderWindow_Finish
{
	ULONG MethodID;
	Object *win;
	ULONG ok;
	ULONG redraw;
};

struct MUIP_Popup_Finish
{
	ULONG MethodID;
	Object *win;
	Object *list;
};

struct MUIP_Object
{
	ULONG MethodID;
	Object *obj;
};

struct MUIP_OBJECT
{
	ULONG MethodID;
	OBJECT *obj;
};

struct ModeMsg
{
	ULONG MethodID;
	ULONG newmode;
};

struct MUIP_DispWindow_Line
{
	ULONG MethodID;
	int y;
	int lines;
	rsiSMALL_COLOR *line;
};

struct MUIP_MainWindow_Open
{
	ULONG MethodID;
	char *name;
};

struct MUIP_MainWindow_sciDoMethod
{
	ULONG MethodID;
	int method;
};

struct MUIP_Material_Material
{
	ULONG MethodID;
	SURFACE *surf;
};

struct MUIP_Render_DrawUpdate
{
	ULONG MethodID;
	int line;
};

struct MUIP_StateWindow_RenderCleanup
{
	ULONG MethodID;
	char *err;
};

#define ImageButton(data)\
	ImageObject,\
		ImageButtonFrame,\
		MUIA_Image_OldImage, &data,\
		MUIA_Background, MUII_ButtonBack,\
		MUIA_InnerLeft,0,\
		MUIA_InnerRight,0,\
		MUIA_InnerTop,0,\
		MUIA_InnerBottom,0,\
		MUIA_InputMode,MUIV_InputMode_Immediate,\
		MUIA_CycleChain,TRUE

#define ImageButton1(data)\
	ImageObject,\
		ImageButtonFrame,\
		MUIA_Image_OldImage, &data,\
		MUIA_Background, MUII_ButtonBack,\
		MUIA_InnerLeft,0,\
		MUIA_InnerRight,0,\
		MUIA_InnerTop,0,\
		MUIA_InnerBottom,0,\
		MUIA_InputMode,MUIV_InputMode_RelVerify,\
		MUIA_CycleChain,TRUE

#define ImageButtonExt(name,xres,yres)\
	ImageObject,\
		ImageButtonFrame,\
		MUIA_Image_Spec, name,\
		MUIA_Background, MUII_ButtonBack,\
		MUIA_InnerLeft,0,\
		MUIA_InnerRight,0,\
		MUIA_InnerTop,0,\
		MUIA_InnerBottom,0,\
		MUIA_Image_FreeVert, TRUE,\
		MUIA_Image_FreeHoriz, TRUE,\
		MUIA_FixWidth, xres,\
		MUIA_FixHeight, yres,\
		MUIA_CycleChain,TRUE

#define KeyTextButton(name,key)\
	TextObject,\
		ButtonFrame,\
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , key,\
		MUIA_ControlChar  , key,\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_CycleChain,TRUE

#define TextButton(name)\
	TextObject,\
		ButtonFrame,\
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_CycleChain,TRUE

#define FloatString(obj,label,contents)\
	MUI_MakeObject(MUIO_Label,label,MUIO_Label_DoubleFrame),\
	Child,obj = StringObject,\
		StringFrame,\
		MUIA_String_MaxLen  , 30,\
		MUIA_String_Contents, contents,\
		MUIA_String_Accept,  "0123456789eE-+.",\
		MUIA_String_AdvanceOnCR, TRUE,\
		MUIA_CycleChain,TRUE

#define KeyFloatString(obj,label,contents,key)\
	KeyLabel2(label,key),\
	Child,obj = StringObject,\
		StringFrame,\
		MUIA_String_MaxLen  , 30,\
		MUIA_String_Contents, contents,\
		MUIA_String_Accept,  "0123456789eE-+.",\
		MUIA_CycleChain,TRUE,\
		MUIA_String_AdvanceOnCR, TRUE,\
		MUIA_ControlChar,key

#define FloatString2(tobj,sobj,label,contents)\
	tobj = TextObject,\
		MUIA_Text_Contents,label,\
	End,\
	Child,sobj = StringObject,\
		StringFrame,\
		MUIA_Weight, 30,\
		MUIA_Disabled, TRUE,\
		MUIA_String_MaxLen  , 30,\
		MUIA_String_Contents, contents,\
		MUIA_String_Accept,  "0123456789eE-+.",\
		MUIA_String_AdvanceOnCR, TRUE,\
		MUIA_CycleChain,TRUE

#define NumericString(obj,label,contents)\
	MUI_MakeObject(MUIO_Label,label,MUIO_Label_DoubleFrame),\
	Child,obj=StringObject,\
		StringFrame,\
		MUIA_String_MaxLen  , 30,\
		MUIA_String_Contents, contents,\
		MUIA_String_Accept,  "-0123456789",\
		MUIA_String_AdvanceOnCR, TRUE,\
		MUIA_CycleChain,TRUE

#define KeyNumericString(obj,label,contents,key)\
	KeyLabel2(label,key),\
	Child,obj=StringObject,\
		StringFrame,\
		MUIA_String_MaxLen  , 30,\
		MUIA_String_Contents, contents,\
		MUIA_String_Accept,  "-0123456789",\
		MUIA_String_AdvanceOnCR, TRUE,\
		MUIA_CycleChain,TRUE,\
		MUIA_ControlChar,key

#define Colorfield \
	ColorfieldObject,ButtonFrame,\
		MUIA_InputMode,MUIV_InputMode_RelVerify,\
		MUIA_CycleChain,TRUE,\
	End

#define CheckMarkNew(selected,key)\
	ImageObject,\
		ImageButtonFrame,\
		MUIA_InputMode, MUIV_InputMode_Toggle,\
		MUIA_Image_Spec, MUII_CheckMark,\
		MUIA_Image_FreeVert, TRUE,\
		MUIA_Selected, selected,\
		MUIA_Background, MUII_ButtonBack,\
		MUIA_ShowSelState, FALSE,\
		MUIA_CycleChain,TRUE,\
		MUIA_ControlChar, key,\
	End

#undef Slider
#define Slider(min,max,level)\
	SliderObject,\
		MUIA_Numeric_Min  , min,\
		MUIA_Numeric_Max  , max,\
		MUIA_Numeric_Value, level,\
		MUIA_CycleChain, TRUE, \
	End

#undef KeyString
#define KeyString(contents,maxlen,controlchar)\
	StringObject,\
		StringFrame,\
		MUIA_ControlChar    , controlchar,\
		MUIA_String_MaxLen  , maxlen,\
		MUIA_String_Contents, contents,\
		MUIA_String_AdvanceOnCR, TRUE,\
		MUIA_CycleChain, TRUE

#define BodyChunk(body, palette, width, height, depth)\
	BodychunkObject,\
		MUIA_FixWidth, width,\
		MUIA_FixHeight, height,\
		MUIA_Bitmap_Width, width,\
		MUIA_Bitmap_Height, height,\
		MUIA_Bodychunk_Depth, depth,\
		MUIA_Bodychunk_Body, body,\
		MUIA_Bodychunk_Compression, 0,\
		MUIA_Bodychunk_Masking, 0,\
		MUIA_Bitmap_SourceColors, palette,\
		MUIA_Bitmap_Transparent, 0

#endif
