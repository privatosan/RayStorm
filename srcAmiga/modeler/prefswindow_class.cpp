/***************
 * PROGRAM:       Modeler
 * NAME:          prefswindow_class.cpp
 * DESCRIPTION:   preferences window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.02.96 ah    initial release
 *    06.03.97 ah    added external toolbar images flag
 *    28.01.98 ah    render task priority and stack
 *    31.01.98 ah    colors page
 ***************/

#include <string.h>
#include <libraries/asl.h>
#include <libraries/gadtools.h>
#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef PREFSWINDOW_CLASS_H
#include "prefswindow_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

// hooks for color list
typedef struct
{
	ULONG index;
	COLOR color;
} COLOR_ITEM;

static SAVEDS ASM APTR ColorConstFunc(REG(a2) APTR pool, REG(a1) COLOR_ITEM *item)
{
	COLOR_ITEM *nitem;

	nitem = new COLOR_ITEM;
	if(!nitem)
		return NULL;

	*nitem = *item;
	return (APTR)nitem;
}

static SAVEDS ASM void ColorDestFunc(REG(a2) APTR pool, REG(a1) COLOR_ITEM *item)
{
	delete item;
}

static SAVEDS ASM LONG ColorDispFunc(REG(a1) COLOR_ITEM *item, REG(a2) char **array)
{
	*array = GetColorString(item->index);

	return 0;
}

static const struct Hook ColorConstHook = { { NULL,NULL },(HOOKFUNC)ColorConstFunc,NULL,NULL };
static const struct Hook ColorDestHook = { { NULL,NULL },(HOOKFUNC)ColorDestFunc,NULL,NULL };
static const struct Hook ColorDispHook = { { NULL,NULL },(HOOKFUNC)ColorDispFunc,NULL,NULL };

static char *Pages[] =
{
	"Paths","Flags","Misc","Colors","OpenGL",NULL
};

/*************
 * DESCRIPTION:   do initialations for preferences window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct PrefsWindow_Data *data,databuf;
	PREFS p;
	float value, quality, priority, stack;
	Object *paths, *flags, *misc, *colors, *opengl;
	int i;
	COLOR_ITEM item;

	paths =
		ColGroup(2),
			Child, KeyLabel2("Picture viewer:",'r'),
			Child, databuf.pf_view = PopaslObject,
				MUIA_CycleChain,TRUE,
				MUIA_Popstring_String, KeyString(NULL,256,'r'), End,
				MUIA_Popstring_Button, PopButton(MUII_PopFile),
				ASLFR_TitleText  , "Please select a viewer...",
				MUIA_ShortHelp, "Name of viewer",
			End,
			Child, KeyLabel2("Project path:",'o'),
			Child, databuf.pf_project = PopaslObject,
				MUIA_CycleChain,TRUE,
				MUIA_Popstring_String, KeyString(NULL,256,'o'), End,
				MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
				ASLFR_TitleText  , "Please select a drawer...",
				ASLFR_DrawersOnly, TRUE,
				MUIA_ShortHelp, "Path for projects",
			End,
			Child, KeyLabel2("Object path:",'o'),
			Child, databuf.pf_object = PopaslObject,
				MUIA_CycleChain,TRUE,
				MUIA_Popstring_String, KeyString(NULL,256,'o'), End,
				MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
				ASLFR_TitleText  , "Please select a drawer...",
				ASLFR_DrawersOnly, TRUE,
				MUIA_ShortHelp, "Path for objects",
			End,
			Child, KeyLabel2("Texture path:",'t'),
			Child, databuf.pf_texture = PopaslObject,
				MUIA_CycleChain,TRUE,
				MUIA_Popstring_String, KeyString(NULL,256,'t'), End,
				MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
				ASLFR_TitleText  , "Please select a drawer...",
				ASLFR_DrawersOnly, TRUE,
				MUIA_ShortHelp, "Path for textures",
			End,
			Child, KeyLabel2("Brush path:",'b'),
			Child, databuf.pf_brush = PopaslObject,
				MUIA_CycleChain,TRUE,
				MUIA_Popstring_String, KeyString(NULL,256,'b'), End,
				MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
				ASLFR_TitleText  , "Please select a drawer...",
				ASLFR_DrawersOnly, TRUE,
				MUIA_ShortHelp, "Path for brushes",
			End,
			Child, KeyLabel2("Materials path:",'m'),
			Child, databuf.pf_material = PopaslObject,
				MUIA_CycleChain,TRUE,
				MUIA_Popstring_String, KeyString(NULL,256,'m'), End,
				MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
				ASLFR_TitleText  , "Please select a drawer...",
				ASLFR_DrawersOnly, TRUE,
				MUIA_ShortHelp, "Path for materials",
			End,
			Child, KeyLabel2("Lights path:",'m'),
			Child, databuf.pf_light = PopaslObject,
				MUIA_CycleChain,TRUE,
				MUIA_Popstring_String, KeyString(NULL,256,'l'), End,
				MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
				ASLFR_TitleText  , "Please select a drawer...",
				ASLFR_DrawersOnly, TRUE,
				MUIA_ShortHelp, "Path for lights",
			End,
		End;

	flags =
		ColGroup(2),
			Child, KeyLabel2("Always show coordinates",'c'),
			Child, databuf.cm_showcoord = CheckMarkNew(FALSE,'c'),
			Child, KeyLabel2("External toolbar images",'e'),
			Child, databuf.cm_extimages = CheckMarkNew(FALSE,'e'),
		End;

	opengl =
		ColGroup(2),
			Child, KeyLabel2("Direct rendering",'d'),
			Child, databuf.cm_directrender = CheckMarkNew(FALSE,'d'),
		End;

	misc =
		ColGroup(2),
			Child, Label2("Undo memory kB"),
			Child, databuf.ls_undomem = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
				MUIA_StringSlider_Min, 1,
				MUIA_StringSlider_Max, 200,
				MUIA_StringSlider_LowerBound, TRUE,
				MUIA_StringSlider_Float, FALSE,
			End,
			Child, Label2("JPEG quality"),
			Child, databuf.ls_jpegquality = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
				MUIA_StringSlider_Min, 1,
				MUIA_StringSlider_Max, 100,
				MUIA_StringSlider_LowerBound, TRUE,
				MUIA_StringSlider_UpperBound, TRUE,
				MUIA_StringSlider_Float, FALSE,
			End,
			Child, Label2("Render task priority"),
			Child, databuf.ls_renderprio = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
				MUIA_StringSlider_Min, -10,
				MUIA_StringSlider_Max, 10,
				MUIA_StringSlider_Float, FALSE,
			End,
			Child, Label2("Render task stack kB"),
			Child, databuf.ls_renderstack = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
				MUIA_StringSlider_Min, 4,
				MUIA_StringSlider_Max, 128,
				MUIA_StringSlider_LowerBound, TRUE,
				MUIA_StringSlider_Float, FALSE,
			End,
		End;

	colors =
		HGroup,
			Child, VGroup,
				Child, databuf.lv_colors = ListviewObject,
					MUIA_CycleChain,TRUE,
					MUIA_Listview_List, ListObject,
						InputListFrame,
						MUIA_List_ConstructHook, &ColorConstHook,
						MUIA_List_DestructHook, &ColorDestHook,
						MUIA_List_DisplayHook, &ColorDispHook,
					End,
				End,
				Child, HGroup,
					Child, databuf.cm_usesurfacecolors = CheckMarkNew(FALSE,'s'),
					Child, KeyLabel2("Use surface colors",'s'),
					Child, HSpace(0),
				End,
			End,
			Child, databuf.coloradjust = ColoradjustObject,
				MUIA_CycleChain,TRUE,
			End,
		End;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title, "Preferences",
		MUIA_Window_ID, MAKE_ID('P','R','E','F'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, RegisterGroup(Pages),
				MUIA_Register_Frame, TRUE,
				Child, paths,
				Child, flags,
				Child, misc,
				Child, colors,
				Child, opengl,
			End,
			Child, HGroup,
				Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE, End,
				Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE, End,
			End,
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct PrefsWindow_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->originator = (Object *)WARPOS_PPC_FUNC(GetTagData)(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);
		data->disp = (DISPLAY *)WARPOS_PPC_FUNC(GetTagData)(MUIA_PrefWindow_Display,0,msg->ops_AttrList);
		data->drawarea = (Object *)WARPOS_PPC_FUNC(GetTagData)(MUIA_PrefWindow_DrawArea,0,msg->ops_AttrList);

		SetAttrs(obj, MUIA_Window_ActiveObject, data->b_ok, TAG_DONE);

		// close methods
		DOMETHOD(data->b_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_ChildWindow_Finish, obj, FALSE);
		DOMETHOD(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_ChildWindow_Finish, obj, FALSE);

		// other methods
		DOMETHOD(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_PrefWindow_Ok);

		// initialize string contents
		p.id = ID_VIEW;
		if(p.GetPrefs())
			SetAttrs(data->pf_view,MUIA_String_Contents,p.data, TAG_DONE);
		p.id = ID_PRJP;
		if(p.GetPrefs())
			SetAttrs(data->pf_project,MUIA_String_Contents,p.data, TAG_DONE);
		p.id = ID_OBJP;
		if(p.GetPrefs())
			SetAttrs(data->pf_object,MUIA_String_Contents,p.data, TAG_DONE);
		p.id = ID_TXTP;
		if(p.GetPrefs())
			SetAttrs(data->pf_texture,MUIA_String_Contents,p.data, TAG_DONE);
		p.id = ID_BRSP;
		if(p.GetPrefs())
			SetAttrs(data->pf_brush,MUIA_String_Contents,p.data, TAG_DONE);
		p.id = ID_MATP;
		if(p.GetPrefs())
			SetAttrs(data->pf_material,MUIA_String_Contents,p.data, TAG_DONE);
		p.id = ID_LIGP;
		if(p.GetPrefs())
			SetAttrs(data->pf_light,MUIA_String_Contents,p.data, TAG_DONE);
		p.id = ID_FLGS;
		if(p.GetPrefs())
		{
			SetAttrs(data->cm_showcoord, MUIA_Selected, *((ULONG*)p.data) & PREFFLAG_SHOWCOORD, TAG_DONE);
			SetAttrs(data->cm_extimages, MUIA_Selected, *((ULONG*)p.data) & PREFFLAG_EXTIMAGES, TAG_DONE);
			SetAttrs(data->cm_usesurfacecolors, MUIA_Selected, *((ULONG*)p.data) & PREFFLAG_USESURFACECOLORS, TAG_DONE);
			SetAttrs(data->cm_directrender, MUIA_Selected, *((ULONG*)p.data) & PREFFLAG_OPENGL_DIRECTRENDER, TAG_DONE);
		}
		else
		{
			SetAttrs(data->cm_showcoord, MUIA_Selected, FALSE, TAG_DONE);
			SetAttrs(data->cm_extimages, MUIA_Selected, FALSE, TAG_DONE);
			SetAttrs(data->cm_usesurfacecolors, MUIA_Selected, TRUE, TAG_DONE);
			SetAttrs(data->cm_directrender, MUIA_Selected, TRUE, TAG_DONE);
		}
		p.id = ID_UNDO;
		if(p.GetPrefs())
			value = (float)(*(ULONG*)(p.data))*0.001;
		else
			value = 100.f;
		SetAttrs(data->ls_undomem,
			MUIA_NoNotify, TRUE,
			MUIA_StringSlider_Value, &value,
			TAG_DONE);

		p.id = ID_JPEG;
		if(p.GetPrefs())
			quality = (float)(*(ULONG*)(p.data));
		else
			quality = 70.f;
		SetAttrs(data->ls_jpegquality,
			MUIA_NoNotify, TRUE,
			MUIA_StringSlider_Value, &quality,
			TAG_DONE);

		p.id = ID_RPRI;
		if(p.GetPrefs())
			priority = (float)(*(LONG*)(p.data));
		else
			priority = -1.f;
		SetAttrs(data->ls_renderprio,
			MUIA_NoNotify, TRUE,
			MUIA_StringSlider_Value, &priority,
			TAG_DONE);

		p.id = ID_STCK;
		if(p.GetPrefs())
			stack = (float)(*(LONG*)(p.data));
		else
			stack = 32.f;
		SetAttrs(data->ls_renderstack,
			MUIA_NoNotify, TRUE,
			MUIA_StringSlider_Value, &stack,
			TAG_DONE);

		p.data = NULL;

		// add color names
		for(i=0; i<COLOR_MAX; i++)
		{
			GetColorValue(i, &item.color);
			item.index = i;
			DOMETHOD(data->lv_colors, MUIM_List_InsertSingle, &item, MUIV_List_Insert_Bottom);
		}
		SetAttrs(data->lv_colors, MUIA_List_Active, MUIV_List_Active_Top, TAG_DONE);
		DOMETHOD(data->lv_colors, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
			obj, 1, MUIM_PrefWindow_ColorListChanged);
		DOMETHOD(data->coloradjust, MUIM_Notify, MUIA_Coloradjust_RGB, MUIV_EveryTime,
			obj, 1, MUIM_PrefWindow_ColorChanged);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   ok button pressed -> accept new preferences
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Ok(struct IClass *cl,Object *obj,Msg msg)
{
	struct PrefsWindow_Data *data = (struct PrefsWindow_Data *)INST_DATA(cl,obj);
	PREFS p;
	ULONG flags, datas;
	float value;
	COLOR_ITEM *item;
	int i;

	GetAttr(MUIA_String_Contents,data->pf_view,(ULONG*)&p.data);
	p.id = ID_VIEW;
	p.length = strlen((char*)p.data)+1;
	p.AddPrefs();

	GetAttr(MUIA_String_Contents,data->pf_project,(ULONG*)&p.data);
	p.id = ID_PRJP;
	p.length = strlen((char*)p.data)+1;
	p.AddPrefs();

	GetAttr(MUIA_String_Contents,data->pf_object,(ULONG*)&p.data);
	p.id = ID_OBJP;
	p.length = strlen((char*)p.data)+1;
	p.AddPrefs();

	GetAttr(MUIA_String_Contents,data->pf_brush,(ULONG*)&p.data);
	p.id = ID_BRSP;
	p.length = strlen((char*)p.data)+1;
	p.AddPrefs();

	GetAttr(MUIA_String_Contents,data->pf_texture,(ULONG*)&p.data);
	p.id = ID_TXTP;
	p.length = strlen((char*)p.data)+1;
	p.AddPrefs();

	GetAttr(MUIA_String_Contents,data->pf_material,(ULONG*)&p.data);
	p.id = ID_MATP;
	p.length = strlen((char*)p.data)+1;
	p.AddPrefs();

	GetAttr(MUIA_String_Contents,data->pf_light,(ULONG*)&p.data);
	p.id = ID_LIGP;
	p.length = strlen((char*)p.data)+1;
	p.AddPrefs();

	datas = 0;
	GetAttr(MUIA_Selected,data->cm_showcoord,&flags);
	if(flags)
	{
		datas |= PREFFLAG_SHOWCOORD;
		data->disp->always_coord = TRUE;
		MUI_RequestIDCMP(data->drawarea,IDCMP_MOUSEMOVE);
	}
	else
	{
		data->disp->always_coord = FALSE;
		MUI_RejectIDCMP(data->drawarea,IDCMP_MOUSEMOVE);
	}
	GetAttr(MUIA_Selected, data->cm_extimages, &flags);
	if(flags)
		datas |= PREFFLAG_EXTIMAGES;
	GetAttr(MUIA_Selected, data->cm_usesurfacecolors, &flags);
	if(flags)
	{
		data->disp->usesurfacecolors = TRUE;
		datas |= PREFFLAG_USESURFACECOLORS;
	}
	else
	{
		data->disp->usesurfacecolors = FALSE;
	}

	GetAttr(MUIA_Selected, data->cm_directrender, &flags);
	if(flags)
	{
		data->disp->directrender = TRUE;
		datas |= PREFFLAG_OPENGL_DIRECTRENDER;
	}
	else
	{
		data->disp->directrender = FALSE;
	}

	p.data = &datas;
	p.id = ID_FLGS;
	p.length = sizeof(ULONG);
	p.AddPrefs();

	GetAttr(MUIA_StringSlider_Value,data->ls_undomem,(ULONG*)&value);
	datas = (ULONG)(value*1000.);
	p.data = &datas;
	p.id = ID_UNDO;
	p.length = sizeof(ULONG);
	p.AddPrefs();

	GetAttr(MUIA_StringSlider_Value, data->ls_jpegquality, (ULONG*)&value);
	datas = (ULONG)(value);
	p.data = &datas;
	p.id = ID_JPEG;
	p.length = sizeof(ULONG);
	p.AddPrefs();

	GetAttr(MUIA_StringSlider_Value, data->ls_renderprio, (ULONG*)&value);
	datas = (ULONG)(value);
	p.data = &datas;
	p.id = ID_RPRI;
	p.length = sizeof(ULONG);
	p.AddPrefs();

	GetAttr(MUIA_StringSlider_Value, data->ls_renderstack, (ULONG*)&value);
	datas = (ULONG)(value);
	p.data = &datas;
	p.id = ID_STCK;
	p.length = sizeof(ULONG);
	p.AddPrefs();

	p.data = NULL;

	DOMETHOD(app, MUIM_Application_PushMethod, data->originator, 3, MUIM_ChildWindow_Finish, obj, FALSE);

	for(i=0; i<COLOR_MAX; i++)
	{
		// get entry from list
		DOMETHOD(data->lv_colors, MUIM_List_GetEntry, i, (ULONG*)&item);
		if(!item)
			break;

		SetColorValue(item->index, &item->color);
	}
	gfx.FreePens();
	gfx.AllocPens();

	return 0;
}

/*************
 * DESCRIPTION:   user selected new entry in color list, update the colors
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG ColorListChanged(struct IClass *cl,Object *obj,Msg msg)
{
	struct PrefsWindow_Data *data = (struct PrefsWindow_Data *)INST_DATA(cl,obj);
	ULONG t;
	ULONG red,green,blue;
	COLOR_ITEM *item;

	DOMETHOD(data->lv_colors, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &item);
	if(item)
	{
#ifdef __STORM__
		t = (ULONG)(item->color.r*255.f);
#else
		t = (ULONG)(item->color.r*255.f+.5f);
#endif
		red = t<<24;
#ifdef __STORM__
		t = (ULONG)(item->color.g*255.f);
#else
		t = (ULONG)(item->color.g*255.f+.5f);
#endif
		green = t<<24;
#ifdef __STORM__
		t = (ULONG)(item->color.b*255.f);
#else
		t = (ULONG)(item->color.b*255.f+.5f);
#endif
		blue = t<<24;
		SetAttrs(data->coloradjust,
			MUIA_Coloradjust_Red, red,
			MUIA_Coloradjust_Green, green,
			MUIA_Coloradjust_Blue, blue,
			TAG_DONE);
	}

	return 0;
}

/*************
 * DESCRIPTION:   user selected new entry in color list, update the colors
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG ColorChanged(struct IClass *cl,Object *obj,Msg msg)
{
	struct PrefsWindow_Data *data = (struct PrefsWindow_Data *)INST_DATA(cl,obj);
	COLOR_ITEM *item;
	LONG col;

	DOMETHOD(data->lv_colors, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &item);
	if(item)
	{
		GetAttr(MUIA_Coloradjust_Red, data->coloradjust, (ULONG*)&col);
		item->color.r = float(col>>24)/255.f;
		GetAttr(MUIA_Coloradjust_Green, data->coloradjust, (ULONG*)&col);
		item->color.g = float(col>>24)/255.f;
		GetAttr(MUIA_Coloradjust_Blue, data->coloradjust, (ULONG*)&col);
		item->color.b = float(col>>24)/255.f;
	}
	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(PrefsWindow_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
		case MUIM_PrefWindow_Ok:
			return(Ok(cl,obj,msg));
		case MUIM_PrefWindow_ColorListChanged:
			return(ColorListChanged(cl, obj, msg));
		case MUIM_PrefWindow_ColorChanged:
			return(ColorChanged(cl, obj, msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
