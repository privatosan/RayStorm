/***************
 * PROGRAM:       Modeler
 * NAME:          attrswindow_class.cpp
 * DESCRIPTION:   attributes window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.12.95 ah    initial release
 *    02.05.97 ah    preview is now a thread
 ***************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <libraries/asl.h>
#include <exec/memory.h>

#include <pragma/asl_lib.h>
#include <pragma/dos_lib.h>
#include <pragma/exec_lib.h>
#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

extern struct GfxBase *GfxBase;
#ifndef __MIXEDBINARY__
// have to protect this Base pointer, StormC uses only global variables for libraries!
struct Library *TextureBase;
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef ATTRSWINDOW_CLASS_H
#include "attrswindow_class.h"
#endif

#ifndef RENDER_CLASS_H
#include "render_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef MSGHANDLER_H
#include "msghandler_class.h"
#endif

#ifndef PREVIEW_H
#include "preview.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef SUBTASK_H
#include "subtask.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef Dialog_CLASS_H
#include "dialog_class.h"
#endif

#ifndef TEXTURE_MODULE_H
#include "rtxt:texture_module.h"
#endif

static char *Pages[] =
{
	"Surface","Brush","RayStorm Texture","Imagine Texture",/*"Hypertexture",*/NULL
};

static const char *CYA_Colors[] =
{
	"Diffuse","Specular","Transparence","Reflectivity","Ambient",
	"Diffuse Transmission","Specular Transmission","Translucency",NULL
};

static const char *brush_type[] =
{
	"Color","Reflect","Filter","Altitude","Specular",NULL
};

static const char *brush_wrap[] =
{
	"Flat","WrapX","WrapY","WrapXY",NULL
};

static const char *hyper_type[] =
{
	"Explosion",NULL
};

// constants for index of refraction
static const char *index_values[] =
{
	"1.",
	"1.00029",
	"1.36",
	"1.329",
	"2.92",
	"1.66",
	"1.486",
	"1.63",
	"2.705",
	"2.705",
	"1.52",
	"2.00",
	"2.417",
	"1.57",
	"1.36",
	"1.434",
	"1.46",
	"1.89",
	"1.65",
	"1.5",
	"1.309",
	"3.34",
	"1.61",
	"1.575",
	"1.20",
	"1.55",
	"1.644",
	"1.553",
	"1.77",
	"1.77",
	"1.544",
	"1.644",
	"1.38",
	"1.49",
	"1.61",
	"1.333",
	"1.517"
};

static const char *index_names[] =
{
	"Vacuum",
	"Air (STP)",
	"Acetone",
	"Alcohol",
	"Amorphous Selenium",
	"Calspar1",
	"Calspar2",
	"Carbon Disulfide",
	"Chromium Oxide",
	"Copper Oxide",
	"Crown Glass",
	"Crystal",
	"Diamond",
	"Emerald",
	"Ethyl Alcohol",
	"Flourite",
	"Fused Quartz",
	"Heaviest Flint Glass",
	"Heavy Flint Glass",
	"Glass",
	"Ice",
	"Iodine Crystal",
	"Lapis Lazuli",
	"Light Flint Glass",
	"Liquid Carbon Dioxide",
	"Polystyrene",
	"Quartz 1",
	"Quartz 2",
	"Ruby",
	"Sapphire",
	"Sodium Chloride (Salt) 1",
	"Sodium Chloride (Salt) 2",
	"Sugar Solution (30%)",
	"Sugar Solution (80%)",
	"Topaz",
	"Water (20 C)",
	"Zinc Crown Glass"
};

// hooks for pupup
static SAVEDS ASM LONG StrObjFunc(REG(a2) Object *pop,REG(a1) Object *str)
{
	SetAttrs(pop,MUIA_List_Active,MUIV_List_Active_Off,TAG_DONE);
	return(TRUE);
}

static SAVEDS ASM VOID ObjStrFunc(REG(a2) Object *pop,REG(a1) Object *str)
{
	ULONG x;

	GetAttr(MUIA_List_Active,pop,&x);
	SetAttrs(str,MUIA_String_Contents,index_values[x],TAG_DONE);
}

static SAVEDS ASM VOID WindowFunc(REG(a2) Object *pop,REG(a1) Object *win)
{
	SetAttrs(win,MUIA_Window_DefaultObject,pop,TAG_DONE);
}

static const struct Hook StrObjHook = { { NULL,NULL },(HOOKFUNC)StrObjFunc,NULL,NULL };
static const struct Hook ObjStrHook = { { NULL,NULL },(HOOKFUNC)ObjStrFunc,NULL,NULL };
static const struct Hook WindowHook = { { NULL,NULL },(HOOKFUNC)WindowFunc,NULL,NULL };

// hooks for brushes
typedef struct
{
	BRUSH *brush;
	BOOL added;
	int type;
	int wrap;
	int flags;
} BRUSH_ITEM;

static SAVEDS ASM APTR BrushConstFunc(REG(a2) APTR pool, REG(a1) BRUSH_ITEM *bi)
{
	BRUSH_ITEM *nbi;

	nbi = new BRUSH_ITEM;
	if(!nbi)
		return NULL;

	*nbi = *bi;
	return (APTR)nbi;
}

static SAVEDS ASM void BrushDestFunc(REG(a2) APTR pool, REG(a1) BRUSH_ITEM *bi)
{
	if(bi->added)
		delete bi->brush;
	delete bi;
}

static SAVEDS ASM LONG BrushDispFunc(REG(a1) BRUSH_ITEM *bi, REG(a2) char **array)
{
	static char buf[256];

	strcpy(buf, bi->brush->file);
	*array = buf;

	return 0;
}

static const struct Hook BrushConstHook = { { NULL,NULL },(HOOKFUNC)BrushConstFunc,NULL,NULL };
static const struct Hook BrushDestHook = { { NULL,NULL },(HOOKFUNC)BrushDestFunc,NULL,NULL };
static const struct Hook BrushDispHook = { { NULL,NULL },(HOOKFUNC)BrushDispFunc,NULL,NULL };

enum
{
	RADIO_TYPE, RADIO_WRAP
};

struct MUIP_BrushRadio
{
	ULONG MethodID;
	int which;
};

struct MUIP_BrushFlags
{
	ULONG MethodID;
	int flags;
};

/////////////////////////////
// hooks for imagine textures
/////////////////////////////
static SAVEDS ASM APTR ITextureConstFunc(REG(a2) APTR pool, REG(a1) ITEXTURE_ITEM *ti)
{
	ITEXTURE_ITEM *nti;

	nti = new ITEXTURE_ITEM;
	if(!nti)
		return NULL;

	*nti = *ti;
	return (APTR)nti;
}

static SAVEDS ASM void ITextureDestFunc(REG(a2) APTR pool, REG(a1) ITEXTURE_ITEM *ti)
{
	int i;

	for(i=0; i<16; i++)
	{
		if(ti->infotext[i])
			delete ti->infotext[i];
	}
	if(ti->added)
		delete ti->texture;
	delete ti;
}

static SAVEDS ASM LONG ITextureDispFunc(REG(a1) ITEXTURE_ITEM *ti, REG(a2) char **array)
{
	static char buf[256];

	strcpy(buf, ti->texture->name);
	*array = buf;

	return 0;
}

static const struct Hook ITextureConstHook = { { NULL,NULL },(HOOKFUNC)ITextureConstFunc,NULL,NULL };
static const struct Hook ITextureDestHook = { { NULL,NULL },(HOOKFUNC)ITextureDestFunc,NULL,NULL };
static const struct Hook ITextureDispHook = { { NULL,NULL },(HOOKFUNC)ITextureDispFunc,NULL,NULL };

/////////////////////////////
// hooks for RayStorm textures
/////////////////////////////
static SAVEDS ASM APTR RTextureConstFunc(REG(a2) APTR pool, REG(a1) RTEXTURE_ITEM *ti)
{
	RTEXTURE_ITEM *nti;

	nti = new RTEXTURE_ITEM;
	if(!nti)
		return NULL;

	*nti = *ti;
	return (APTR)nti;
}

static SAVEDS ASM void RTextureDestFunc(REG(a2) APTR pool, REG(a1) RTEXTURE_ITEM *ti)
{
	if(ti->TextureBase)
	{
#ifdef __MIXEDBINARY__
		PPCRTextureCleanup(ti->tinfo->cleanup, ti->tinfo, ti->TextureBase);
#else
		ti->tinfo->cleanup(ti->tinfo);
		CloseLibrary(ti->TextureBase);
#endif
		delete ti->data;
	}
	if(ti->added)
		delete ti->texture;
	delete ti;
}

static SAVEDS ASM LONG RTextureDispFunc(REG(a1) RTEXTURE_ITEM *ti, REG(a2) char **array)
{
	static char buf[256];

	strcpy(buf, ti->texture->name);
	*array = buf;

	return 0;
}

static const struct Hook RTextureConstHook = { { NULL,NULL },(HOOKFUNC)RTextureConstFunc,NULL,NULL };
static const struct Hook RTextureDestHook = { { NULL,NULL },(HOOKFUNC)RTextureDestFunc,NULL,NULL };
static const struct Hook RTextureDispHook = { { NULL,NULL },(HOOKFUNC)RTextureDispFunc,NULL,NULL };

/////////////////////////////
// hooks for hyper textures
/////////////////////////////
static SAVEDS ASM APTR HTextureConstFunc(REG(a2) APTR pool, REG(a1) HTEXTURE_ITEM *ti)
{
	HTEXTURE_ITEM *nti;

	nti = new HTEXTURE_ITEM;
	if(!nti)
		return NULL;

	*nti = *ti;
	return (APTR)nti;
}

static SAVEDS ASM void HTextureDestFunc(REG(a2) APTR pool, REG(a1) HTEXTURE_ITEM *ti)
{
	if(ti->added)
		delete ti->texture;
	delete ti;
}

static SAVEDS ASM LONG HTextureDispFunc(REG(a1) HTEXTURE_ITEM *ti, REG(a2) char **array)
{
	static char buf[256];

	strcpy(buf, ti->texture->name);
	*array = buf;

	return 0;
}

static const struct Hook HTextureConstHook = { { NULL,NULL },(HOOKFUNC)HTextureConstFunc,NULL,NULL };
static const struct Hook HTextureDestHook = { { NULL,NULL },(HOOKFUNC)HTextureDestFunc,NULL,NULL };
static const struct Hook HTextureDispHook = { { NULL,NULL },(HOOKFUNC)HTextureDispFunc,NULL,NULL };

/////////////
// prototypes
/////////////
static BOOL ReadRTextureParams(RTEXTURE_ITEM *, char *);
static BOOL ReadITextureParams(ITEXTURE_ITEM *, char *);
static void GetParams(ITEXTURE_ITEM *,struct AttrsWindow_Data *);
static void SetValues(struct AttrsWindow_Data *, SURFACE *, BOOL added);

#define PREVIEW_WIDTH   64
#define PREVIEW_HEIGHT  64

#ifdef __PPC__
#undef WARPOS_PPC_FUNC
#define WARPOS_PPC_FUNC(a) a ## PPC
#endif // __PPC__

/*************
 * DESCRIPTION:   do initialations for attrs window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct AttrsWindow_Data *data,databuf;
	static char buffer[256];
	Object *preview, *surface, *brush, *rtexture, /**htexture,*/ *itexture;
	int i;

	databuf.st = NULL;
	databuf.sigbit = 0;
	databuf.previewdata = NULL;
	databuf.dialog = NULL;

	databuf.object = (OBJECT*)GetTagData(MUIA_AttrWindow_Object,0,msg->ops_AttrList);
	databuf.origsurf = (SURFACE*)GetTagData(MUIA_AttrWindow_Surface,0,msg->ops_AttrList);
	if(!databuf.origsurf)
		return 0;
	if(databuf.origsurf->flags & SURF_DEFAULT)
	{
		utility.Request("This object has no material assigned");
		return 0;
	}

	if(databuf.origsurf->name)
		sprintf(buffer, "Material %s", databuf.origsurf->name);
	else
		strcpy(buffer, "Material");

	// preview only for KS 3.0 and higher
	if(GfxBase->LibNode.lib_Version>=39)
	{
		preview =
			VGroup,
				Child, HGroup,
					Child, VGroup,
						Child, HGroup, GroupFrameT("Material file"),
							Child,
								databuf.b_load = KeyTextButton("Load",'l'),
								MUIA_ShortHelp, "Load material file",
							End,
							Child,
								databuf.b_save = KeyTextButton("Save",'s'),
								MUIA_ShortHelp, "Save material file",
							End,
						End,
						Child, VSpace(0),
					End,
					Child, ScrollgroupObject,
						MUIA_VertWeight, 300,
						MUIA_Scrollgroup_Contents, VirtgroupObject,
							VirtualFrame,
							Child, databuf.renderarea = (Object *)NewObject(CL_Render->mcc_Class,NULL,
								MUIA_Background, MUII_BACKGROUND,
								MUIA_Render_XRes, PREVIEW_WIDTH,
								MUIA_Render_YRes, PREVIEW_HEIGHT,
							End,
						End,
					End,
				End,
				Child, HGroup,
					Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE, End,
					Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE, End,
				End,
			End;
	}
	else
	{
		preview =
			HGroup,
				Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE, End,
				Child,
					databuf.b_load = KeyTextButton("Load",'l'),
					MUIA_ShortHelp, "Load material file",
				End,
				Child,
					databuf.b_save = KeyTextButton("Save",'s'),
					MUIA_ShortHelp, "Save material file",
				End,
				Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE, End,
			End;
	}

	surface =
		VGroup,
			Child, HGroup,
				Child, ColGroup(2),
					Child, Label2("Diffuse"),
					Child, databuf.cf_diff = Colorfield,
					Child, VSpace(0), Child, VSpace(0),
					Child, Label2("Specular"),
					Child, databuf.cf_spec = Colorfield,
					Child, VSpace(0), Child, VSpace(0),
					Child, Label2("Transparence"),
					Child, databuf.cf_transpar = Colorfield,
					Child, VSpace(0), Child, VSpace(0),
					Child, Label2("Reflectivity"),
					Child, databuf.cf_reflect = Colorfield,
					Child, VSpace(0), Child, VSpace(0),
					Child, FloatString(databuf.fs_sexp,"Reflection exponent",NULL), End,
					Child, VSpace(0), Child, VSpace(0),
					Child, FloatString(databuf.fs_texp,"Transmission exponent",NULL), End,
					Child, VSpace(0), Child, VSpace(0),
				End,
				Child, ColGroup(2),
					Child, Label2("Ambient"),
					Child, databuf.cf_ambient = Colorfield,
					Child, VSpace(0), Child, VSpace(0),
					Child, Label2("Diffuse Transmission"),
					Child, databuf.cf_transm = Colorfield,
					Child, VSpace(0), Child, VSpace(0),
					Child, Label2("Specular Transmission"),
					Child, databuf.cf_specm = Colorfield,
					Child, VSpace(0), Child, VSpace(0),
					Child, FloatString(databuf.fs_transluc,"Translucency",NULL), End,
					Child, VSpace(0), Child, VSpace(0),
					Child, Label2("Index of refraction"),
					Child, databuf.pop = PopobjectObject,
						MUIA_CycleChain,TRUE,
						MUIA_Popstring_String,
							databuf.fs_rind = StringObject,
								StringFrame,
								MUIA_CycleChain,TRUE,
								MUIA_String_Accept,  "0123456789eE-+.",
								MUIA_String_MaxLen  , 30,
							End,
						MUIA_Popstring_Button, PopButton(MUII_PopUp),
						MUIA_Popobject_StrObjHook, &StrObjHook,
						MUIA_Popobject_ObjStrHook, &ObjStrHook,
						MUIA_Popobject_WindowHook, &WindowHook,
						MUIA_Popobject_Object, databuf.plist = ListviewObject,
							MUIA_CycleChain,TRUE,
							MUIA_Listview_List, ListObject,
								InputListFrame,
								MUIA_List_SourceArray, index_names,
							End,
						End,
					End,
					Child, VSpace(0), Child, VSpace(0),
					Child, FloatString(databuf.fs_flen,"Fog length",NULL), End,
					Child, VSpace(0), Child, VSpace(0),
				End,
			End,
			Child, ColGroup(4),
				Child, KeyLabel2("Bright",'b'),
				Child, databuf.cm_bright = CheckMarkNew(databuf.origsurf->flags & SURF_BRIGHT,'b'),
				Child, HSpace(0), Child, HSpace(0),
			End,
		End;

	brush =
		VGroup,
			Child, databuf.lv_brush = ListviewObject,
				MUIA_CycleChain,TRUE,
				MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
				MUIA_Listview_List, ListObject,
					InputListFrame,
					MUIA_List_DragSortable, TRUE,
					MUIA_List_ConstructHook, &BrushConstHook,
					MUIA_List_DestructHook, &BrushDestHook,
					MUIA_List_DisplayHook, &BrushDispHook,
				End,
			End,
			Child, HGroup,
				Child, databuf.b_brushadd = KeyTextButton("Add",'a'), End,
				Child, databuf.b_brushchange = KeyTextButton("Change name", 'h'), MUIA_Disabled, TRUE, End,
				Child, databuf.b_brushview = KeyTextButton("View",'v'), MUIA_Disabled, TRUE, End,
				Child, databuf.b_brushremove = KeyTextButton("Remove",'r'), MUIA_Disabled, TRUE, End,
			End,
			Child, databuf.gr_brush = HGroup,
				MUIA_Disabled, TRUE,
				Child, databuf.rb_brushtype = RadioObject,
					MUIA_CycleChain,TRUE,
					MUIA_Radio_Entries,brush_type,
				End,
				Child, HSpace(0),
				Child, databuf.rb_brushwrap = RadioObject,
					MUIA_CycleChain,TRUE,
					MUIA_Radio_Entries,brush_wrap,
				End,
				Child, HSpace(0),
				Child, ColGroup(2),
					Child, KeyLabel2("Repeat",'p'),
					Child, databuf.cm_brushrepeat = CheckMarkNew(FALSE,'p'),
					Child, KeyLabel2("Mirror",'m'),
					Child, databuf.cm_brushmirror = CheckMarkNew(FALSE,'m'),
					Child, KeyLabel2("Soft",'s'),
					Child, databuf.cm_brushsoft = CheckMarkNew(TRUE,'s'),
				End,
			End,
		End;

	rtexture =
		VGroup,
			Child, databuf.lv_rtexture = ListviewObject,
				MUIA_Weight, 30,
				MUIA_CycleChain,TRUE,
				MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
				MUIA_Listview_List, ListObject,
					InputListFrame,
					MUIA_List_DragSortable, TRUE,
					MUIA_List_ConstructHook, &RTextureConstHook,
					MUIA_List_DestructHook, &RTextureDestHook,
					MUIA_List_DisplayHook, &RTextureDispHook,
				End,
			End,
			Child, databuf.g_rtxtdialog = VGroup,
				Child, HGroup,
					Child, RectangleObject, MUIA_Rectangle_HBar, TRUE, MUIA_FixHeight, 2, End,
					Child, databuf.t_rtxttitle = TextObject, MUIA_Text_PreParse, "\33c", End,
					Child, RectangleObject, MUIA_Rectangle_HBar, TRUE, MUIA_FixHeight, 2, End,
				 End,
			End,
			Child, HGroup,
				MUIA_Weight, 0,
				Child, HSpace(0),
			End,
			Child, HGroup,
				Child, databuf.b_rtextureadd = KeyTextButton("Add",'a'), End,
				Child, databuf.b_rtexturechange = KeyTextButton("Change name", 'h'), MUIA_Disabled, TRUE, End,
				Child, databuf.b_rtextureremove = KeyTextButton("Remove",'r'), MUIA_Disabled, TRUE, End,
			End,
		End;

#ifdef HTXT
	htexture =
		VGroup,
			Child, databuf.lv_htexture = ListviewObject,
				MUIA_CycleChain,TRUE,
				MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
				MUIA_Listview_List, ListObject,
					InputListFrame,
					MUIA_List_DragSortable, TRUE,
					MUIA_List_ConstructHook, &HTextureConstHook,
					MUIA_List_DestructHook, &HTextureDestHook,
					MUIA_List_DisplayHook, &HTextureDispHook,
				End,
			End,
			Child, HGroup,
				Child, databuf.b_htextureadd = KeyTextButton("Add",'a'), End,
				Child, databuf.b_htexturechange = KeyTextButton("Change name", 'h'), MUIA_Disabled, TRUE, End,
				Child, databuf.b_htextureremove = KeyTextButton("Remove",'r'), MUIA_Disabled, TRUE, End,
			End,
		End;
#endif

	itexture =
		VGroup,
			Child, databuf.lv_itexture = ListviewObject,
				MUIA_CycleChain,TRUE,
				MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
				MUIA_Listview_List, ListObject,
					InputListFrame,
					MUIA_List_DragSortable, TRUE,
					MUIA_List_ConstructHook, &ITextureConstHook,
					MUIA_List_DestructHook, &ITextureDestHook,
					MUIA_List_DisplayHook, &ITextureDispHook,
				End,
			End,
			Child, HGroup,
				Child, databuf.b_itextureadd = KeyTextButton("Add",'a'), End,
				Child, databuf.b_itexturechange = KeyTextButton("Change name", 'h'), MUIA_Disabled, TRUE, End,
				Child, databuf.b_itextureremove = KeyTextButton("Remove",'r'), MUIA_Disabled, TRUE, End,
			End,
			Child, HGroup,
				Child, ColGroup(2),
					Child, FloatString2(databuf.t_p[0],databuf.fs_p[0],"P1",NULL), End,
					Child, FloatString2(databuf.t_p[1],databuf.fs_p[1],"P2",NULL), End,
					Child, FloatString2(databuf.t_p[2],databuf.fs_p[2],"P3",NULL), End,
					Child, FloatString2(databuf.t_p[3],databuf.fs_p[3],"P4",NULL), End,
					Child, FloatString2(databuf.t_p[4],databuf.fs_p[4],"P5",NULL), End,
					Child, FloatString2(databuf.t_p[5],databuf.fs_p[5],"P6",NULL), End,
					Child, FloatString2(databuf.t_p[6],databuf.fs_p[6],"P7",NULL), End,
					Child, FloatString2(databuf.t_p[7],databuf.fs_p[7],"P8",NULL), End,
				End,
				Child, ColGroup(2),
					Child, FloatString2(databuf.t_p[8],databuf.fs_p[8],"P9",NULL), End,
					Child, FloatString2(databuf.t_p[9],databuf.fs_p[9],"P10",NULL), End,
					Child, FloatString2(databuf.t_p[10],databuf.fs_p[10],"P11",NULL), End,
					Child, FloatString2(databuf.t_p[11],databuf.fs_p[11],"P12",NULL), End,
					Child, FloatString2(databuf.t_p[12],databuf.fs_p[12],"P13",NULL), End,
					Child, FloatString2(databuf.t_p[13],databuf.fs_p[13],"P14",NULL), End,
					Child, FloatString2(databuf.t_p[14],databuf.fs_p[14],"P15",NULL), End,
					Child, FloatString2(databuf.t_p[15],databuf.fs_p[15],"P16",NULL), End,
				End,
			End,
		End;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title, buffer,
		MUIA_Window_ID, MAKE_ID('M','A','T','E'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, RegisterGroup(Pages),
				MUIA_Register_Frame, TRUE,
				Child, surface,
				Child, brush,
				Child, rtexture,
				Child, itexture,
#ifdef HTEXT
				Child, htexture,
#endif
			End,
			Child, preview,
		End,
		TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->originator = (Object *)WARPOS_PPC_FUNC(GetTagData)(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);
		data->surf = NULL;

		SetAttrs(obj, MUIA_Window_ActiveObject, data->b_ok, TAG_DONE);
		SetValues(data, data->origsurf, FALSE);

		// button methods
		DOMETHOD(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_AttrWindow_Ok);
		DOMETHOD(data->b_load,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_AttrWindow_Load);
		DOMETHOD(data->b_save,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_AttrWindow_Save);

		// close methods
		DOMETHOD(data->plist,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,
			data->pop, 2, MUIM_Popstring_Close, TRUE);
		DOMETHOD(data->b_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
			obj, 3, MUIM_AttrWindow_Close, obj, FALSE);
		DOMETHOD(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			obj, 3, MUIM_AttrWindow_Close, obj, FALSE);

		// open color window if someone click on colofields
		DOMETHOD(data->cf_diff,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_AttrWindow_OpenColor,data->cf_diff);
		DOMETHOD(data->cf_spec,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_AttrWindow_OpenColor,data->cf_spec);
		DOMETHOD(data->cf_transpar,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_AttrWindow_OpenColor,data->cf_transpar);
		DOMETHOD(data->cf_reflect,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_AttrWindow_OpenColor,data->cf_reflect);
		DOMETHOD(data->cf_ambient,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_AttrWindow_OpenColor,data->cf_ambient);
		DOMETHOD(data->cf_transm,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_AttrWindow_OpenColor,data->cf_transm);
		DOMETHOD(data->cf_specm,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_AttrWindow_OpenColor,data->cf_specm);

		// brush methods
		DOMETHOD(data->lv_brush,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,
			obj,1,MUIM_AttrWindow_BrushChanged);
		DOMETHOD(data->b_brushadd,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_AttrWindow_AddBrush);
		DOMETHOD(data->b_brushchange, MUIM_Notify, MUIA_Pressed, FALSE,
			obj, 1, MUIM_AttrWindow_ChangeBrush);
		DOMETHOD(data->b_brushview,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_AttrWindow_View);
		DOMETHOD(data->b_brushremove,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_AttrWindow_RemoveBrush);
		DOMETHOD(data->rb_brushtype,MUIM_Notify,MUIA_Radio_Active,MUIV_EveryTime,
			obj,2,MUIM_AttrWindow_BrushRadio,RADIO_TYPE);
		DOMETHOD(data->rb_brushwrap,MUIM_Notify,MUIA_Radio_Active,MUIV_EveryTime,
			obj,2,MUIM_AttrWindow_BrushRadio,RADIO_WRAP);
		DOMETHOD(data->cm_brushrepeat,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
			obj,2,MUIM_AttrWindow_BrushFlags,BRUSH_REPEAT);
		DOMETHOD(data->cm_brushmirror,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
			obj,2,MUIM_AttrWindow_BrushFlags,BRUSH_MIRROR);
		DOMETHOD(data->cm_brushsoft,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
			obj,2,MUIM_AttrWindow_BrushFlags,BRUSH_SOFT);

		// RayStorm texture methods
		DOMETHOD(data->lv_rtexture,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,
			obj,1,MUIM_AttrWindow_RTextureChanged);
		DOMETHOD(data->b_rtextureadd,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_AttrWindow_AddRTexture);
		DOMETHOD(data->b_rtexturechange, MUIM_Notify, MUIA_Pressed, FALSE,
			obj, 1, MUIM_AttrWindow_ChangeRTexture);
		DOMETHOD(data->b_rtextureremove,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_AttrWindow_RemoveRTexture);

		// Imagine texture methods
		DOMETHOD(data->lv_itexture,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,
			obj,1,MUIM_AttrWindow_ITextureChanged);
		DOMETHOD(data->b_itextureadd,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_AttrWindow_AddITexture);
		DOMETHOD(data->b_itexturechange, MUIM_Notify, MUIA_Pressed, FALSE,
			obj, 1, MUIM_AttrWindow_ChangeITexture);
		DOMETHOD(data->b_itextureremove,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_AttrWindow_RemoveITexture);

		// create preview sub task
		if(GfxBase->LibNode.lib_Version>=39)
		{
			data->sigbit = WARPOS_PPC_FUNC(AllocSignal)(-1);
			if(data->sigbit)
			{
				data->st = CreateSubTask(PreviewSubTask, data->sigbit, "Scenario Preview Task", -2, 4096);
				if(data->st)
				{
					data->previewdata = new struct PreviewData;
					if(data->previewdata)
					{
						data->previewdata->preview = NULL;
						// redraw preview if values change
						DOMETHOD(data->fs_sexp, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
							obj, 1, MUIM_AttrWindow_Preview);
						DOMETHOD(data->fs_texp, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
							obj, 1, MUIM_AttrWindow_Preview);
						DOMETHOD(data->fs_transluc, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
							obj, 1, MUIM_AttrWindow_Preview);
						DOMETHOD(data->fs_rind, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
							obj, 1, MUIM_AttrWindow_Preview);
						DOMETHOD(data->fs_flen, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
							obj, 1, MUIM_AttrWindow_Preview);
						DOMETHOD(data->cm_bright, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
							obj, 1, MUIM_AttrWindow_Preview);
						for(i=0; i<16; i++)
						{
							DOMETHOD(data->fs_p[i], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
								obj, 1, MUIM_AttrWindow_Preview);
						}

						// add render area to message handler, with that the messages which the
						// subtask sends are handled.
						DOMETHOD(msghandler, MUIM_MsgHandler_AddObject, data->renderarea);
					}
				}
			}
		}

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   OM_GET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG Get(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data *)INST_DATA(cl,obj);

	switch (msg->opg_AttrID)
	{
		case MUIA_AttrWindow_Surface:
			*(SURFACE**)(msg->opg_Storage) = data->origsurf;
			return TRUE;
	}

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   set window values
 * INPUT:         data     object data
 *                surf     surface
 *                added    TRUE brushes and textures are loaded, else FALSE
 * OUTPUT:        -
 *************/
static void SetValues(struct AttrsWindow_Data *data, SURFACE *surf, BOOL added)
{
	char buffer[30];
	BRUSH_ITEM bi;
	BRUSH *brush;
	ITEXTURE_ITEM iti;
	RTEXTURE_ITEM rti;
	TEXTURE *texture;
	int i;

	// set color field values
	SetColorField(data->cf_diff, &surf->diffuse);
	SetColorField(data->cf_spec, &surf->specular);
	SetColorField(data->cf_transpar, &surf->transpar);
	SetColorField(data->cf_reflect, &surf->reflect);
	SetColorField(data->cf_ambient, &surf->ambient);
	SetColorField(data->cf_transm, &surf->difftrans);
	SetColorField(data->cf_specm, &surf->spectrans);

	// set numeric fields
	Float2String(surf->refphong, buffer);
	SetAttrs(data->fs_sexp,MUIA_String_Contents,buffer,TAG_DONE);
	Float2String(surf->transphong, buffer);
	SetAttrs(data->fs_texp,MUIA_String_Contents,buffer,TAG_DONE);
	Float2String(surf->refrindex, buffer);
	SetAttrs(data->fs_rind,MUIA_String_Contents,buffer,TAG_DONE);
	Float2String(surf->foglength, buffer);
	SetAttrs(data->fs_flen,MUIA_String_Contents,buffer,TAG_DONE);
	Float2String(surf->translucency, buffer);
	SetAttrs(data->fs_transluc,MUIA_String_Contents,buffer,TAG_DONE);

	// add brushes to list
	brush = surf->brush;
	while(brush)
	{
		bi.brush = brush;
		bi.added = added;
		bi.type = brush->type;
		bi.wrap = brush->wrap;
		bi.flags = brush->flags;

		DOMETHOD(data->lv_brush, MUIM_List_InsertSingle, &bi, MUIV_List_Insert_Bottom);

		brush = (BRUSH*)brush->GetNext();
	}

	// add textures to list
	texture = surf->texture;
	while(texture)
	{
		switch(texture->GetType())
		{
			case TEXTURE_IMAGINE:
				iti.texture = (IMAGINE_TEXTURE*)texture;
				iti.added = added;

				ReadITextureParams(&iti, texture->GetName());

				for(i=0; i<16; i++)
					iti.params[i] = ((IMAGINE_TEXTURE*)texture)->params[i];

				DOMETHOD(data->lv_itexture, MUIM_List_InsertSingle, &iti, MUIV_List_Insert_Bottom);
				break;
			case TEXTURE_RAYSTORM:
				rti.texture = (RAYSTORM_TEXTURE*)texture;
				rti.added = added;

				ReadRTextureParams(&rti, texture->GetName());
				memcpy(rti.data, ((RAYSTORM_TEXTURE*)texture)->data, ((RAYSTORM_TEXTURE*)texture)->datasize);

				DOMETHOD(data->lv_rtexture, MUIM_List_InsertSingle, &rti, MUIV_List_Insert_Bottom);
				break;
		}

		texture = (TEXTURE*)texture->GetNext();
	}
	data->itextureentry = NULL;
	data->rtextureentry = NULL;
}

/*************
 * DESCRIPTION:   read the gadgets and update the surface values
 * INPUT:         data           object data
 *                surf           surface
 *                modify_list    modify MUI brush and texture list (FALSE for save and preview)
 * OUTPUT:        -
 *************/
static void UpdateValues(struct AttrsWindow_Data *data, SURFACE *surf, BOOL modify_list)
{
	char *s;
	int i;
	BRUSH_ITEM *bi;
	BRUSH *btmplist,*bnext,*brush;
	ITEXTURE_ITEM *iti;
	RTEXTURE_ITEM *rti;
	TEXTURE *ttmplist,*tnext,*texture;
	ULONG flags;

	// copy surface name
/* GetAttr(MUIA_String_Contents, data->name, (ULONG*)&s);
	surf->SetName(s); */

	// set surface colors
	GetColorField(data->cf_diff,&surf->diffuse);
	GetColorField(data->cf_spec,&surf->specular);
	GetColorField(data->cf_transpar,&surf->transpar);
	GetColorField(data->cf_reflect,&surf->reflect);
	GetColorField(data->cf_ambient,&surf->ambient);
	GetColorField(data->cf_transm,&surf->difftrans);
	GetColorField(data->cf_specm,&surf->spectrans);

	GetAttr(MUIA_Selected, data->cm_bright, &flags);
	if(flags)
		surf->flags |= SURF_BRIGHT;
	else
		surf->flags &= ~SURF_BRIGHT;

	// set surface values
	GetAttr(MUIA_String_Contents,data->fs_sexp,(ULONG*)&s);
	surf->refphong = atof(s);
	GetAttr(MUIA_String_Contents,data->fs_texp,(ULONG*)&s);
	surf->transphong = atof(s);
	GetAttr(MUIA_String_Contents,data->fs_rind,(ULONG*)&s);
	surf->refrindex = atof(s);
	GetAttr(MUIA_String_Contents,data->fs_flen,(ULONG*)&s);
	surf->foglength = atof(s);
	GetAttr(MUIA_String_Contents,data->fs_transluc,(ULONG*)&s);
	surf->translucency = atof(s);

	// read brushes from listview
	// build brush list in surface according to position in list view:
	// highest priority -> top of list -> last of brush list
	// lowest priority -> bottom of list -> first of brush list
	btmplist = surf->brush;
	surf->brush = NULL;
	for(i=0; ; i++)
	{
		// get entry from list
		DOMETHOD(data->lv_brush, MUIM_List_GetEntry, i, (ULONG*)&bi);
		if(!bi)
			break;

		if(modify_list)
		{
			// copy parameters
			bi->brush->type = bi->type;
			bi->brush->wrap = bi->wrap;
			bi->brush->flags = bi->flags;

			if(!bi->added)
			{
				// remove from temp list
				bi->brush->SLIST::DeChain((SLIST**)&btmplist);
			}

			// and add to surface brush list
			surf->AddBrush(bi->brush);

			// this must be set to FALSE to tell the Destructor of the list
			// not to delete the brush
			bi->added = FALSE;
		}
		else
		{
			brush = (BRUSH*)bi->brush->DupObj();
			if(!brush)
				break;

			brush->type = bi->type;
			brush->wrap = bi->wrap;
			brush->flags = bi->flags;

			surf->AddBrush(brush);
		}
	}

	// delete removed brushes
	while(btmplist)
	{
		bnext = (BRUSH*)btmplist->GetNext();
		delete btmplist;
		btmplist = bnext;
	}

	// set texture parameters
	if(data->itextureentry)
		GetParams(data->itextureentry, data);

	// read textures from listview
	ttmplist = (IMAGINE_TEXTURE*)surf->texture;
	surf->texture = NULL;
	for(i=0; ; i++)
	{
		// get entry from list
		DOMETHOD(data->lv_itexture, MUIM_List_GetEntry, i, (ULONG*)&iti);
		if(!iti)
			break;

		if(modify_list)
		{
			memcpy(iti->texture->params,iti->params,sizeof(float)*16);

			if(!iti->added)
			{
				// remove from temp list
				iti->texture->SLIST::DeChain((SLIST**)&ttmplist);
			}

			// and add to surface texture list
			surf->AddTexture(iti->texture);

			// this must be set to FALSE to tell the Destructor of the list
			// not to delete the texture
			iti->added = FALSE;
		}
		else
		{
			texture = (TEXTURE*)iti->texture->DupObj();
			if(!texture)
				break;

			memcpy(((IMAGINE_TEXTURE*)texture)->params,iti->params,sizeof(float)*16);

			surf->AddTexture(texture);
		}
	}
	for(i=0; ; i++)
	{
		// get entry from list
		DOMETHOD(data->lv_rtexture, MUIM_List_GetEntry, i, (ULONG*)&rti);
		if(!rti)
			break;

		if(modify_list)
		{
			memcpy(rti->texture->data, rti->data, rti->texture->datasize);

			if(!rti->added)
			{
				// remove from temp list
				rti->texture->SLIST::DeChain((SLIST**)&ttmplist);
			}

			// and add to surface texture list
			surf->AddTexture(rti->texture);

			// this must be set to FALSE to tell the Destructor of the list
			// not to delete the texture
			rti->added = FALSE;
		}
		else
		{
			texture = (TEXTURE*)rti->texture->DupObj();
			if(!texture)
				break;

			memcpy(((RAYSTORM_TEXTURE*)texture)->data, rti->data, rti->texture->datasize);

			surf->AddTexture(texture);
		}
	}

	// delete removed textures
	while(ttmplist)
	{
		tnext = (TEXTURE*)ttmplist->GetNext();
		delete ttmplist;
		ttmplist = tnext;
	}
}

/*************
 * DESCRIPTION:   this function is called when the user clicks on the preview button
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Preview(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);

	if(data->st)
	{
		SendSubTaskMsg(data->st, STC_STOP, NULL);
		SendSubTaskMsg(data->st, STC_CLEANUP, NULL);
		if(data->surf)
			delete data->surf;

		data->surf = data->origsurf->Duplicate();
		if(!data->surf)
			return 0;

		UpdateValues(data, data->surf, FALSE);

		data->previewdata->object = data->object;
		data->previewdata->surf = data->surf;
		data->previewdata->renderarea = data->renderarea;
		data->previewdata->width = PREVIEW_WIDTH;
		data->previewdata->height = PREVIEW_HEIGHT;
		data->previewdata->caller = NULL;
		data->previewdata->msghandler = msghandler;
		SendSubTaskMsg(data->st, STC_START, data->previewdata);
	}

	return 0;
}

/*************
 * DESCRIPTION:   this function is called when the user clicks on the ok button
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Ok(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);

	UpdateValues(data, data->origsurf, TRUE);

	// close the window
	DOMETHOD(obj, MUIM_AttrWindow_Close, obj, TRUE);

	return 0;
}

/*************
 * DESCRIPTION:   this function is called when the user clicks on the load button
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Load(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	SURFACE surf;
	BRUSH *btmplist, *bnext;
	IMAGINE_TEXTURE *ttmplist, *tnext;

	if(sciLoadMaterial(&surf))
	{
		// free all brushes and textures
		btmplist = data->origsurf->brush;
		while(btmplist)
		{
			bnext = (BRUSH*)btmplist->GetNext();
			delete btmplist;
			btmplist = bnext;
		}
		data->origsurf->brush = NULL;
		DOMETHOD(data->lv_brush, MUIM_List_Clear);

		ttmplist = (IMAGINE_TEXTURE*)data->origsurf->texture;
		while(ttmplist)
		{
			tnext = (IMAGINE_TEXTURE*)ttmplist->GetNext();
			delete ttmplist;
			ttmplist = tnext;
		}
		data->origsurf->texture = NULL;
		DOMETHOD(data->lv_itexture, MUIM_List_Clear);

		SetValues(data, &surf, TRUE);

		// don't delete brushes and textures with destructor
		surf.brush = NULL;
		surf.texture = NULL;
		DOMETHOD(obj, MUIM_AttrWindow_Preview);
	}

	return 0;
}

/*************
 * DESCRIPTION:   this function is called when the user clicks on the save button
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Save(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	SURFACE *surf;

	surf = data->origsurf->Duplicate();
	if(surf)
	{
		UpdateValues(data, surf, FALSE);

		sciSaveMaterial(surf);

		delete surf;
	}

	return 0;
}

/*************
 * DESCRIPTION:   open color pop up window
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG OpenColor(struct IClass *cl,Object *obj,struct MUIP_Object *msg)
{
	Object *newobj;

	// create a color window
	newobj = (Object*)NewObject(CL_Color->mcc_Class,NULL,
		MUIA_ChildWindow_Originator,obj,
		MUIA_Colorfield,msg->obj,
		TAG_DONE);
	if(newobj)
	{
		SetSleep(TRUE);

		DOMETHOD(app,OM_ADDMEMBER,newobj);
		SetAttrs(newobj,MUIA_Window_Open,TRUE,TAG_DONE);
	}
	return 0;
}

/*************
 * DESCRIPTION:   close color pop up window
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG CloseColor(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish* msg)
{
	Object *win;

	win = msg->win;
	SetAttrs(win,MUIA_Window_Open,FALSE,TAG_DONE);

	DOMETHOD(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);

	SetSleep(FALSE);

	DOMETHOD(obj, MUIM_AttrWindow_Preview);

	return 0;
}

/*************
 * DESCRIPTION:   this function is called everytime the user clicks on an entry of the brush list
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG BrushChanged(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	ULONG entry;
	BRUSH_ITEM *bi;

	GetAttr(MUIA_List_Active, data->lv_brush, &entry);
	if(entry == MUIV_List_Active_Off)
	{
		// no entry active -> disable buttons
		SetAttrs(data->gr_brush, MUIA_Disabled, TRUE, TAG_DONE);
		SetAttrs(data->b_brushchange, MUIA_Disabled, TRUE, TAG_DONE);
		SetAttrs(data->b_brushview, MUIA_Disabled, TRUE, TAG_DONE);
		SetAttrs(data->b_brushremove, MUIA_Disabled, TRUE, TAG_DONE);
		return 0;
	}
	DOMETHOD(data->lv_brush, MUIM_List_GetEntry, entry, (ULONG*)&bi);

	// enable brush flags
	SetAttrs(data->gr_brush,MUIA_Disabled,FALSE,TAG_DONE);

	// and view and remove buttons
	SetAttrs(data->b_brushchange, MUIA_Disabled, FALSE, TAG_DONE);
	SetAttrs(data->b_brushview, MUIA_Disabled, FALSE, TAG_DONE);
	SetAttrs(data->b_brushremove, MUIA_Disabled, FALSE, TAG_DONE);

	SetAttrs(data->rb_brushtype, MUIA_Radio_Active, bi->type, TAG_DONE);
	SetAttrs(data->rb_brushwrap, MUIA_Radio_Active, bi->wrap, TAG_DONE);

	SetAttrs(data->cm_brushrepeat, MUIA_Selected, (bi->flags & BRUSH_REPEAT) ? TRUE : FALSE, TAG_DONE);
	SetAttrs(data->cm_brushmirror, MUIA_Selected, (bi->flags & BRUSH_MIRROR) ? TRUE : FALSE, TAG_DONE);
	SetAttrs(data->cm_brushsoft, MUIA_Selected, (bi->flags & BRUSH_SOFT) ? TRUE : FALSE, TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:   add a brush to the brush list
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG AddBrush(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	char buffer[256];
	BRUSH_ITEM bi;

	if(utility.FileRequ(buffer, FILEREQU_BRUSH, 0))
	{
		// add to brush list
		bi.brush = new BRUSH;
		if(bi.brush)
		{
			if(bi.brush->SetName(buffer))
			{
				bi.added = TRUE;
				bi.type = bi.brush->type;
				bi.wrap = bi.brush->wrap;
				bi.flags = bi.brush->flags;

				DOMETHOD(data->lv_brush, MUIM_List_InsertSingle, &bi, MUIV_List_Insert_Bottom);

				// select new entry
				SetAttrs(data->lv_brush,MUIA_List_Active,MUIV_List_Active_Bottom, TAG_DONE);
			}
		}
	}

	return 0;
}

/*************
 * DESCRIPTION:   change the name of a brush
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG ChangeBrush(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	char buffer[256];
	ULONG entry;
	BRUSH_ITEM *bi;

	GetAttr(MUIA_List_Active, data->lv_brush, &entry);
	DOMETHOD(data->lv_brush, MUIM_List_GetEntry, entry, (ULONG*)&bi);
	strcpy(buffer, bi->brush->GetName());

	if(utility.FileRequ(buffer, FILEREQU_BRUSH, FILEREQU_INITIALFILEDIR))
	{
		bi->brush->SetName(buffer);
		DOMETHOD(data->lv_brush, MUIM_List_Redraw, MUIV_List_Redraw_Active);
	}

	return 0;
}

/*************
 * DESCRIPTION:   View a brush
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG View(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	ULONG entry;
	BRUSH_ITEM *bi;

	SetSleep(TRUE);

	GetAttr(MUIA_List_Active, data->lv_brush, &entry);
	DOMETHOD(data->lv_brush, MUIM_List_GetEntry, entry, (ULONG*)&bi);

	ViewPicture(obj, bi->brush->file);

	SetSleep(FALSE);

	return 0;
}

/*************
 * DESCRIPTION:   this function is called everytime the state of the a radio button changes
 *    (which radio button it is, is detemined with the field 'which' of the message)
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG BrushRadio(struct IClass *cl,Object *obj,struct MUIP_BrushRadio *msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	ULONG entry;
	BRUSH_ITEM *bi;

	GetAttr(MUIA_List_Active, data->lv_brush, &entry);
	DOMETHOD(data->lv_brush, MUIM_List_GetEntry, entry, (ULONG*)&bi);

	switch(msg->which)
	{
		case RADIO_TYPE:
			GetAttr(MUIA_Radio_Active, data->rb_brushtype, (ULONG*)&bi->type);
			break;
		case RADIO_WRAP:
			GetAttr(MUIA_Radio_Active, data->rb_brushwrap, (ULONG*)&bi->wrap);
			break;
	}

	return 0;
}

/*************
 * DESCRIPTION:   this function is called everytime the state of the repeat check mark changes
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG BrushFlags(struct IClass *cl,Object *obj,struct MUIP_BrushFlags *msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	ULONG entry,state;
	BRUSH_ITEM *bi;

	GetAttr(MUIA_List_Active, data->lv_brush, &entry);
	DOMETHOD(data->lv_brush, MUIM_List_GetEntry, entry, (ULONG*)&bi);

	switch(msg->flags)
	{
		case BRUSH_REPEAT:
			GetAttr(MUIA_Selected, data->cm_brushrepeat, &state);
			break;
		case BRUSH_MIRROR:
			GetAttr(MUIA_Selected, data->cm_brushmirror, &state);
			break;
		case BRUSH_SOFT:
			GetAttr(MUIA_Selected, data->cm_brushsoft, &state);
			break;
	}

	if(state)
		bi->flags |= msg->flags;
	else
		bi->flags &= ~msg->flags;

	return 0;
}

/*************
 * DESCRIPTION:   read parameters from float fields and store them
 * INPUT:         ti       pointer to texture item
 * OUTPUT:        none
 *************/
static void GetParams(ITEXTURE_ITEM *ti,struct AttrsWindow_Data *data)
{
	char *s;
	int i;

	for(i=0; i<16; i++)
	{
		if(ti->infotext[i])
		{
			GetAttr(MUIA_String_Contents, data->fs_p[i], (ULONG*)&s);
			ti->params[i] = atof(s);
		}
	}
}

/*************
 * DESCRIPTION:   this function is called everytime the user clicks on an entry of the RayStorm texture list
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG RTextureChanged(struct IClass *cl, Object *obj, Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	ULONG te;

	GetAttr(MUIA_List_Active, data->lv_rtexture, &te);
	if(te == MUIV_List_Active_Off)
	{
		// no entry active -> disable remove button and value fields
		SetAttrs(data->b_rtextureremove, MUIA_Disabled, TRUE, TAG_DONE);
		if(data->dialog)
		{
			if(DOMETHOD(data->g_rtxtdialog, MUIM_Group_InitChange))
			{
				DOMETHOD(data->g_rtxtdialog, OM_REMMEMBER, data->dialog);
				DOMETHOD(data->g_rtxtdialog, MUIM_Group_ExitChange);
				SetAttrs(data->t_rtxttitle, MUIA_Text_Contents, "", TAG_DONE);
				MUI_DisposeObject(data->dialog);
				data->dialog = NULL;
			}
		}
		data->rtextureentry = NULL;
		return 0;
	}
	DOMETHOD(data->lv_rtexture, MUIM_List_GetEntry, te, (ULONG*)&data->rtextureentry);
	if(!data->rtextureentry)
		return 0;

	// enable the remove button
	SetAttrs(data->b_rtexturechange, MUIA_Disabled, FALSE, TAG_DONE);
	SetAttrs(data->b_rtextureremove, MUIA_Disabled, FALSE, TAG_DONE);

	if(DOMETHOD(data->g_rtxtdialog, MUIM_Group_InitChange))
	{
		DOMETHOD(data->g_rtxtdialog, OM_REMMEMBER, data->dialog);
		MUI_DisposeObject(data->dialog);
		data->dialog =
			(Object *)NewObject(CL_Dialog->mcc_Class, NULL,
				MUIA_Dialog_Dialog, data->rtextureentry->tinfo->dialog,
				MUIA_Dialog_Data, data->rtextureentry->data,
			End;
		if(data->dialog)
			DOMETHOD(data->g_rtxtdialog, OM_ADDMEMBER, data->dialog);
		DOMETHOD(data->g_rtxtdialog, MUIM_Group_ExitChange);
		SetAttrs(data->t_rtxttitle, MUIA_Text_Contents, data->rtextureentry->tinfo->name, TAG_DONE);
	}

	return 0;
}

/*************
 * DESCRIPTION:   this function is called everytime the user clicks on an entry of the texture list
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG ITextureChanged(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	char buffer[30];
	ULONG te;
	int i;

	if(data->itextureentry)
		GetParams(data->itextureentry, data);

	GetAttr(MUIA_List_Active, data->lv_itexture, &te);
	if(te == MUIV_List_Active_Off)
	{
		// no entry active -> disable remove button and value fields
		SetAttrs(data->b_itextureremove, MUIA_Disabled, TRUE, TAG_DONE);

		for(i=0; i<16; i++)
		{
			sprintf(buffer, "P%d", i+1);
			SetAttrs(data->t_p[i], MUIA_Text_Contents, buffer, TAG_DONE);
			SetAttrs(data->fs_p[i], MUIA_Disabled, TRUE, TAG_DONE);
		}
		data->itextureentry = NULL;
		return 0;
	}
	DOMETHOD(data->lv_itexture, MUIM_List_GetEntry, te, (ULONG*)&data->itextureentry);
	if(!data->itextureentry)
		return 0;

	// enable the remove button
	SetAttrs(data->b_itexturechange, MUIA_Disabled, FALSE, TAG_DONE);
	SetAttrs(data->b_itextureremove, MUIA_Disabled, FALSE, TAG_DONE);

	for(i = 0; i<16; i++)
	{
		// Set parameter texts
		if(data->itextureentry->infotext[i])
		{
			SetAttrs(data->t_p[i], MUIA_Text_Contents, data->itextureentry->infotext[i], TAG_DONE);
			SetAttrs(data->fs_p[i], MUIA_Disabled, FALSE, TAG_DONE);
		}
		else
		{
			sprintf(buffer, "P%d", i+1);
			SetAttrs(data->t_p[i], MUIA_Text_Contents, buffer, TAG_DONE);
			SetAttrs(data->fs_p[i], MUIA_Disabled, TRUE, TAG_DONE);
		}
		// Set default parameters
		Float2String(data->itextureentry->params[i], buffer);
		SetAttrs(data->fs_p[i], MUIA_String_Contents, buffer, TAG_DONE);
	}

	return 0;
}

/*************
 * DESCRIPTION:   add a RayStorm texture to the texture list
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG AddRTexture(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	char buffer[256];
	RTEXTURE_ITEM ti;

	if(utility.FileRequ(buffer, FILEREQU_RTEXTURE, 0))
	{
		// add to texture list
		ti.texture = new RAYSTORM_TEXTURE;
		if(ti.texture)
		{
			if(ti.texture->SetName(buffer))
			{
				if(ReadRTextureParams(&ti, buffer))
				{
					ti.texture->data = new UBYTE[ti.tinfo->datasize];
					if(ti.texture->data)
					{
						ti.added = TRUE;

						// copy default parameters
						ti.texture->datasize = ti.tinfo->datasize;
						memcpy(ti.texture->data, ti.data, ti.tinfo->datasize);

						DOMETHOD(data->lv_rtexture, MUIM_List_InsertSingle, &ti, MUIV_List_Insert_Bottom);

						// select new entry
						SetAttrs(data->lv_rtexture, MUIA_List_Active, MUIV_List_Active_Bottom, TAG_DONE);

						DOMETHOD(obj, MUIM_AttrWindow_Preview);
					}
					else
					{
						delete ti.texture;
#ifdef __MIXEDBINARY__
						PPCRTextureCleanup(ti.tinfo->cleanup, ti.tinfo, ti.TextureBase);
#else
						ti.tinfo->cleanup(ti.tinfo);
						CloseLibrary(ti.TextureBase);
#endif
					}
				}
				else
					delete ti.texture;
			}
			else
				delete ti.texture;
		}
	}

	return 0;
}

/*************
 * DESCRIPTION:   add a Imagine texture to the texture list
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG AddITexture(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	char buffer[256];
	ITEXTURE_ITEM ti;
	int i;

	if(utility.FileRequ(buffer, FILEREQU_ITEXTURE, 0))
	{
		// add to texture list
		ti.texture = new IMAGINE_TEXTURE;
		if(ti.texture)
		{
			if(ti.texture->SetName(buffer))
			{
				if(ReadITextureParams(&ti, buffer))
				{
					ti.added = TRUE;

					// copy default parameters
					for(i=0; i<16; i++)
						ti.texture->params[i] = ti.params[i];

					DOMETHOD(data->lv_itexture, MUIM_List_InsertSingle, &ti, MUIV_List_Insert_Bottom);

					// select new entry
					SetAttrs(data->lv_itexture,MUIA_List_Active,MUIV_List_Active_Bottom, TAG_DONE);

					DOMETHOD(obj, MUIM_AttrWindow_Preview);
				}
				else
					delete ti.texture;
			}
			else
				delete ti.texture;
		}
	}

	return 0;
}

/*************
 * DESCRIPTION:   load a texture read the parameters and infotexts and store them in the itemlist
 * INPUT:         ti          pointer to texture item
 *                name        name of texture
 * OUTPUT:        FALSE if failed else TRUE
 *************/
static BOOL ReadITextureParams(ITEXTURE_ITEM *ti, char *name)
{
	BPTR seglist;     // segment list of Imagine texture file
	IM_TTABLE* (*texture_init)(LONG arg0);
	IM_TTABLE *ttable;
	int i;

	seglist = LoadSeg(name);

	if(!seglist)
	{
		for(i=0; i<16; i++)
			ti->infotext[i] = NULL;
		return FALSE;
	}

	*(ULONG*)(&texture_init) = 4*seglist+4;
	ttable = texture_init(0x60FFFF);
	if(!ttable)
	{
		UnLoadSeg(seglist);
		return FALSE;
	}

	// copy parameter names
	for(i=0; i<16; i++)
	{
		if(strlen(ttable->infotext[i+1]))
		{
			ti->infotext[i] = new char[strlen(ttable->infotext[i+1])+1];
			if(!ti->infotext[i])
			{
				UnLoadSeg(seglist);
				return FALSE;
			}
			strcpy(ti->infotext[i], ttable->infotext[i+1]);
		}
		else
			ti->infotext[i] = NULL;
	}

	// copy default parameters
	for(i=0; i<16; i++)
	{
		ti->params[i] = ttable->params[i];
	}

	// Set default geometry
// memcpy(&pos, texture->ttable->tform, 5*sizeof(VECTOR));

	if(ttable->cleanup)
		ttable->cleanup();

	UnLoadSeg(seglist);

	return TRUE;
}

/*************
 * DESCRIPTION:   load a raystorm texture, initialize it and get the texture info. Store them in the itemlist
 * INPUT:         ti          pointer to texture item
 *                name        name of texture
 * OUTPUT:        FALSE if failed else TRUE
 *************/
static BOOL ReadRTextureParams(RTEXTURE_ITEM *ti, char *name)
{
	TEXTURE_INFO *tinfo;

#ifdef __MIXEDBINARY__
	tinfo = PPC_STUB(texture_init)(name, &ti->TextureBase);
#else
	ti->TextureBase = OpenLibrary(name, 0);
	if(!ti->TextureBase)
		return FALSE;

	TextureBase = ti->TextureBase;

	tinfo = texture_init();
	if(!tinfo)
	{
		CloseLibrary(ti->TextureBase);
		return FALSE;
	}
#endif // __MIXEDBINARY__

	ti->data = new UBYTE[tinfo->datasize];
	if(!ti->data)
	{
#ifdef __MIXEDBINARY__
		PPCRTextureCleanup(tinfo->cleanup, tinfo, ti->TextureBase);
#else
		tinfo->cleanup(tinfo);
		CloseLibrary(ti->TextureBase);
#endif
		return FALSE;
	}

	// copy default parameters
	memcpy(ti->data, tinfo->defaultdata, tinfo->datasize);

	ti->tinfo = tinfo;

	return TRUE;
}

/*************
 * DESCRIPTION:   change the name of a raystorm texture
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG ChangeRTexture(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	char buffer[256];
	ULONG entry;
	RTEXTURE_ITEM *ti;

	GetAttr(MUIA_List_Active, data->lv_rtexture, &entry);
	DOMETHOD(data->lv_rtexture, MUIM_List_GetEntry, entry, (ULONG*)&ti);
	strcpy(buffer, ti->texture->GetName());

	if(utility.FileRequ(buffer, FILEREQU_RTEXTURE, FILEREQU_INITIALFILEDIR))
	{
		if(ti->texture->SetName(buffer))
		{
			if(ti->data)
				delete ti->data;
			if(ti->texture->data)
				delete ti->texture->data;
			if(ti->TextureBase)
				CloseLibrary(ti->TextureBase);
			if(ReadRTextureParams(ti, buffer))
			{
				ti->texture->data = new UBYTE[ti->tinfo->datasize];
				if(ti->texture->data)
				{
					// copy default parameters
					ti->texture->datasize = ti->tinfo->datasize;
					memcpy(ti->texture->data, ti->data, ti->tinfo->datasize);
				}
			}
			DOMETHOD(data->lv_rtexture, MUIM_List_Redraw, MUIV_List_Redraw_Active);
			DOMETHOD(obj, MUIM_AttrWindow_RTextureChanged);
			DOMETHOD(obj, MUIM_AttrWindow_Preview);
		}
	}

	return 0;
}

/*************
 * DESCRIPTION:   change the name of a imagine texture
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG ChangeITexture(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);
	char buffer[256];
	ULONG entry;
	ITEXTURE_ITEM *ti;
	int i;

	GetAttr(MUIA_List_Active, data->lv_itexture, &entry);
	DOMETHOD(data->lv_itexture, MUIM_List_GetEntry, entry, (ULONG*)&ti);
	strcpy(buffer, ti->texture->GetName());

	if(utility.FileRequ(buffer, FILEREQU_ITEXTURE, FILEREQU_INITIALFILEDIR))
	{
		if(ti->texture->SetName(buffer))
		{
			for(i=0; i<16; i++)
			{
				if(ti->infotext[i])
				{
					delete ti->infotext[i];
					ti->infotext[i] = NULL;
				}
			}
			if(ReadITextureParams(ti, buffer))
			{
				// copy default parameters
				for(i=0; i<16; i++)
					ti->texture->params[i] = ti->params[i];
			}
			DOMETHOD(data->lv_itexture, MUIM_List_Redraw, MUIV_List_Redraw_Active);
			DOMETHOD(obj, MUIM_AttrWindow_ITextureChanged);
			DOMETHOD(obj, MUIM_AttrWindow_Preview);
		}
	}

	return 0;
}

/*************
 * DESCRIPTION:   dispose window object
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Dispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);

	if(data->st)
	{
		SendSubTaskMsg(data->st, STC_STOP, NULL);
		SendSubTaskMsg(data->st, STC_CLEANUP, NULL);
		KillSubTask(data->st);
		delete data->previewdata;
	}

	// Remove render area from the list of vaild objects. Messages which
	// are in the application message buffer are ignored for this
	// render area object.
	DOMETHOD(msghandler, MUIM_MsgHandler_RemoveObject, data->renderarea);

	if(data->sigbit)
		WARPOS_PPC_FUNC(FreeSignal)(data->sigbit);

	if(data->surf)
		delete data->surf;

	return 0;
}

/*************
 * DESCRIPTION:   close window
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Close(struct IClass *cl,Object *obj, struct MUIP_ChildWindow_Finish *msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);

	if(data->st)
	{
		SendSubTaskMsg(data->st, STC_STOP, NULL);
		SendSubTaskMsg(data->st, STC_CLEANUP, NULL);
		KillSubTask(data->st);
		data->st = NULL;
	}
	if(data->sigbit)
	{
		WARPOS_PPC_FUNC(FreeSignal)(data->sigbit);
		data->sigbit = NULL;
	}

	if(data->surf)
	{
		delete data->surf;
		data->surf = NULL;
	}

	DOMETHOD(data->lv_brush, MUIM_List_Clear);
	DOMETHOD(data->lv_rtexture, MUIM_List_Clear);
	DOMETHOD(data->lv_itexture, MUIM_List_Clear);
//   DOMETHOD(data->lv_htexture, MUIM_List_Clear);

	DOMETHOD(app,MUIM_Application_PushMethod,data->originator,
		3, MUIM_AttrsWindow_Finish, msg->win, msg->ok);

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(AttrsWindow_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	struct AttrsWindow_Data *data = (struct AttrsWindow_Data*)INST_DATA(cl,obj);

	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
		case OM_GET:
			return(Get(cl,obj,(struct opGet*)msg));
		case OM_DISPOSE:
			return(Dispose(cl,obj,msg));
		case MUIM_Parent_Update:
		case MUIM_AttrWindow_Preview:
			return(Preview(cl,obj,msg));
		case MUIM_AttrWindow_Ok:
			return(Ok(cl,obj,msg));
		case MUIM_AttrWindow_Load:
			return(Load(cl,obj,msg));
		case MUIM_AttrWindow_Save:
			return(Save(cl,obj,msg));
		case MUIM_AttrWindow_OpenColor:
			return(OpenColor(cl,obj,(struct MUIP_Object *)msg));
		case MUIM_AttrWindow_BrushChanged:
			return(BrushChanged(cl,obj,msg));
		case MUIM_AttrWindow_AddBrush:
			return(AddBrush(cl,obj,msg));
		case MUIM_AttrWindow_ChangeBrush:
			return(ChangeBrush(cl,obj,msg));
		case MUIM_AttrWindow_View:
			return(View(cl,obj,msg));
		case MUIM_AttrWindow_RemoveBrush:
			DOMETHOD(data->lv_brush, MUIM_List_Remove, MUIV_List_Remove_Active);
			DOMETHOD(obj, MUIM_AttrWindow_Preview);
			return 0;
		case MUIM_AttrWindow_BrushRadio:
			return(BrushRadio(cl,obj,(struct MUIP_BrushRadio*)msg));
		case MUIM_AttrWindow_BrushFlags:
			return(BrushFlags(cl,obj,(struct MUIP_BrushFlags*)msg));
		case MUIM_AttrWindow_ITextureChanged:
			return(ITextureChanged(cl,obj,msg));
		case MUIM_AttrWindow_AddITexture:
			return(AddITexture(cl,obj,msg));
		case MUIM_AttrWindow_ChangeITexture:
			return(ChangeITexture(cl,obj,msg));
		case MUIM_AttrWindow_RemoveITexture:
			DOMETHOD(data->lv_itexture, MUIM_List_Remove, MUIV_List_Remove_Active);
			DOMETHOD(obj, MUIM_AttrWindow_Preview);
			return 0;
		case MUIM_AttrWindow_RTextureChanged:
			return(RTextureChanged(cl,obj,msg));
		case MUIM_AttrWindow_AddRTexture:
			return(AddRTexture(cl,obj,msg));
		case MUIM_AttrWindow_ChangeRTexture:
			return(ChangeRTexture(cl,obj,msg));
		case MUIM_AttrWindow_RemoveRTexture:
			DOMETHOD(data->lv_rtexture, MUIM_List_Remove, MUIV_List_Remove_Active);
			DOMETHOD(obj, MUIM_AttrWindow_Preview);
			return 0;
		case MUIM_Color_Finish:
			return(CloseColor(cl,obj,(struct MUIP_ChildWindow_Finish*)msg));
		case MUIM_AttrWindow_Close:
			return(Close(cl,obj,(struct MUIP_ChildWindow_Finish*)msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
