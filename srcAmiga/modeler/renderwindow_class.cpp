/***************
 * PROGRAM:       Modeler
 * NAME:          renderwindow_class.cpp
 * DESCRIPTION:   render window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    28.05.96 ah    initial release
 *    13.12.96 ah    added mimium octree objects
 ***************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libraries/asl.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <pragma/utility_lib.h>
extern struct GfxBase *GfxBase;

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef RENDERWINDOW_CLASS_H
#include "renderwindow_class.h"
#endif

#ifndef STATEWINDOW_CLASS_H
#include "statewindow_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

// resolution presets
static char *default_resolutions[] =
{
	"NTSC   80x  50",
	"NTSC  160x 100",
	"NTSC  320x 200",
	"NTSC  640x 400",
	"PAL    80x  64",
	"PAL   160x 128",
	"PAL   320x 256",
	"PAL   640x 512",
	"VGA    80x  60",
	"VGA   160x 120",
	"VGA   320x 240",
	"VGA   640x 480",
	"SVGA  800x 600",
	"SVGA 1024x 768",
	"SVGA 1280x1024",
	NULL
};

static ULONG default_res[] =
{
	 80, 50,
	160,100,
	320,200,
	640,400,
	 80, 64,
	160,128,
	320,256,
	640,512,
	 80, 60,
	160,120,
	320,240,
	640,480,
	800,600,
	1024,768,
	1280,1024
};

// supported formats
static char *formats[] =
{
	"ILBM",
	"PNG",
	"TGA",
	"JPEG",
	NULL
};

static char *pages[] =
{
	"Resolution",
	"Misc",
	"Output",
	NULL
};

#define FLAGS_REDRAW    1
#define FLAGS_RESFILE   2

/*************
 * DESCRIPTION:   do initialations for render window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct RenderWindow_Data *data,databuf;
	char buffer[30];
	int i;
	float fnum;

	if(!sciReadResFile(&databuf.resolutions, &databuf.res))
	{
		databuf.resolutions = default_resolutions;
		databuf.res = default_res;
		databuf.flags = 0;
	}
	else
		databuf.flags = FLAGS_RESFILE;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title, "Render",
		MUIA_Window_ID, MAKE_ID('R','E','N','D'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, RegisterGroup(pages),
				MUIA_Register_Frame, TRUE,

				Child, VGroup,
					Child, HGroup,
						Child, VGroup,
							Child, ColGroup(2),
								Child, KeyNumericString(databuf.ns_xres,"X",NULL,'x'),
									MUIA_ShortHelp, "Horizontal resolution",
								End,
								Child, KeyNumericString(databuf.ns_yres,"Y",NULL,'y'),
									MUIA_ShortHelp, "Vertical resolution",
								End,
							End,
							Child, VSpace(0),
						End,
						Child, databuf.lv_res = ListviewObject,
						MUIA_Weight, 200,
							MUIA_CycleChain,TRUE,
							MUIA_Listview_List, ListObject,
								InputListFrame,
								MUIA_List_SourceArray, databuf.resolutions,
							End,
						End,
					End,
					Child, VGroup, GroupFrameT("Render field"),
						Child, HGroup,
							Child, KeyLabel2("Enable",'e'),
							Child, databuf.cm_enable = CheckMarkNew(global.enablearea,'e'),
							Child, HSpace(0),
						End,
						Child, ColGroup(4),
							Child, NumericString(databuf.ns_xmin,"Xmin",NULL), MUIA_CycleChain, TRUE, MUIA_Disabled, !global.enablearea, End,
							Child, NumericString(databuf.ns_xmax,"Xmax",NULL), MUIA_CycleChain, TRUE, MUIA_Disabled, !global.enablearea, End,
							Child, NumericString(databuf.ns_ymin,"Ymin",NULL), MUIA_CycleChain, TRUE, MUIA_Disabled, !global.enablearea, End,
							Child, NumericString(databuf.ns_ymax,"Ymax",NULL), MUIA_CycleChain, TRUE, MUIA_Disabled, !global.enablearea, End,
						End,
					End,
				End,

				Child, VGroup,
					Child, ColGroup(2),
						Child, KeyLabel2("Quick",'q'),
						Child, HGroup,
							Child, databuf.cm_quick = CheckMarkNew(global.quick,'q'),
							Child, HSpace(0),
						End,

						Child, Label("Octree depth"),
						Child, databuf.sl_depth = Slider(1,10,1),

						Child, Label2("Min. objects"),
						Child, databuf.ls_minobjects = (Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
							MUIA_StringSlider_Min, 0,
							MUIA_StringSlider_Max, 20,
							MUIA_StringSlider_LowerBound, TRUE,
							MUIA_ShortHelp, "Amount of objects for which no octree is build",
						End,
					End,
					Child, VSpace(0),
				End,

				Child, VGroup,
					Child, ColGroup(2),
						Child, KeyLabel2("Filename",'n'),
						Child, HGroup,
							Child, databuf.pf_name = PopaslObject,
								MUIA_CycleChain,TRUE,
								MUIA_Popstring_String, KeyString(NULL,256,'n'), End,
								MUIA_Popstring_Button, PopButton(MUII_PopFile),
								MUIA_ShortHelp, "Name of generated file",
								ASLFR_TitleText  , "Please select the output file...",
							End,
							Child, databuf.b_view = ImageButton(viewimage),
								MUIA_InputMode, MUIV_InputMode_RelVerify,
								MUIA_ShortHelp, "View ouptput picture",
								TAG_DONE,
							End,
						End,

						Child, KeyLabel2("Format",'f'),
						Child, databuf.cy_fmt = CycleObject,
							MUIA_CycleChain,TRUE,
							MUIA_ControlChar, 'f',
							MUIA_Cycle_Entries, formats,
							MUIA_ShortHelp, "Format of generated image",
						End,

						Child, KeyLabel2("Show",'s'),
						Child, HGroup,
							Child, databuf.cm_show = CheckMarkNew(global.show,'s'),
							Child, HSpace(0),
						End,
					End,
					Child, VSpace(0),
				End,
			End,
			Child, HGroup,
				Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE, End,
				Child, HSpace(0),
				Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE, End,
			End,
		End,
		TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct RenderWindow_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->originator = (Object *)WARPOS_PPC_FUNC(GetTagData)(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);
		data->camera = (CAMERA*)WARPOS_PPC_FUNC(GetTagData)(MUIA_RenderWindow_Camera,0,msg->ops_AttrList);

		SetAttrs(obj, MUIA_Window_ActiveObject, data->b_ok, TAG_DONE);

		// set strings
		SetAttrs(data->pf_name, MUIA_String_Contents, global.renderdpic, TAG_DONE);

		// set numeric fields
		sprintf(buffer, "%d", global.xres);
		SetAttrs(data->ns_xres,MUIA_String_Contents,buffer, TAG_DONE);
		sprintf(buffer, "%d", global.yres);
		SetAttrs(data->ns_yres,MUIA_String_Contents,buffer, TAG_DONE);

		sprintf(buffer, "%d", (int)(global.xmin*global.xres+.5));
		SetAttrs(data->ns_xmin,MUIA_String_Contents,buffer, TAG_DONE);
		sprintf(buffer, "%d", (int)(global.xmax*global.xres+.5)-1);
		SetAttrs(data->ns_xmax,MUIA_String_Contents,buffer, TAG_DONE);
		sprintf(buffer, "%d", (int)(global.ymin*global.yres+.5));
		SetAttrs(data->ns_ymin,MUIA_String_Contents,buffer, TAG_DONE);
		sprintf(buffer, "%d", (int)(global.ymax*global.yres+.5)-1);
		SetAttrs(data->ns_ymax,MUIA_String_Contents,buffer, TAG_DONE);

		i = 0;
		while(formats[i])
		{
			if(!strcmp(formats[i],global.picformat))
				break;
			i++;
		}
		if(formats[i])
			SetAttrs(data->cy_fmt,MUIA_Cycle_Active, i, TAG_DONE);

		// set slider
		SetAttrs(data->sl_depth,MUIA_Slider_Level,global.octreedepth, TAG_DONE);

		fnum = global.minobjects;
		SetAttrs(data->ls_minobjects, MUIA_NoNotify, TRUE, MUIA_StringSlider_Value, &fnum, TAG_DONE);

		DOMETHOD(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_RenderWindow_Ok);

		// close methods
		DOMETHOD(data->b_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
			MUIV_Notify_Application, 7, MUIM_Application_PushMethod, data->originator,
			3, MUIM_RenderWindow_Finish, obj, FALSE, FALSE);
		DOMETHOD(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			MUIV_Notify_Application, 7, MUIM_Application_PushMethod, data->originator,
			3, MUIM_RenderWindow_Finish, obj, FALSE, FALSE);

		// set resolutions if an entry in the listview is selected
		DOMETHOD(data->lv_res,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,
			obj,1,MUIM_RenderWindow_SetRes);

		// enable render area
		DOMETHOD(data->cm_enable,MUIM_Notify,MUIA_Selected,FALSE,
			data->ns_xmin,3,MUIM_Set,MUIA_Disabled,TRUE);
		DOMETHOD(data->cm_enable,MUIM_Notify,MUIA_Selected,TRUE,
			data->ns_xmin,3,MUIM_Set,MUIA_Disabled,FALSE);
		DOMETHOD(data->cm_enable,MUIM_Notify,MUIA_Selected,FALSE,
			data->ns_xmax,3,MUIM_Set,MUIA_Disabled,TRUE);
		DOMETHOD(data->cm_enable,MUIM_Notify,MUIA_Selected,TRUE,
			data->ns_xmax,3,MUIM_Set,MUIA_Disabled,FALSE);
		DOMETHOD(data->cm_enable,MUIM_Notify,MUIA_Selected,FALSE,
			data->ns_ymin,3,MUIM_Set,MUIA_Disabled,TRUE);
		DOMETHOD(data->cm_enable,MUIM_Notify,MUIA_Selected,TRUE,
			data->ns_ymin,3,MUIM_Set,MUIA_Disabled,FALSE);
		DOMETHOD(data->cm_enable,MUIM_Notify,MUIA_Selected,FALSE,
			data->ns_ymax,3,MUIM_Set,MUIA_Disabled,TRUE);
		DOMETHOD(data->cm_enable,MUIM_Notify,MUIA_Selected,TRUE,
			data->ns_ymax,3,MUIM_Set,MUIA_Disabled,FALSE);

		// view picture
		DOMETHOD(data->b_view,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_RenderWindow_View,data->pf_name);

		// disable show for versions less 39
		if(GfxBase->LibNode.lib_Version<39)
			SetAttrs(data->cm_show,MUIA_Disabled,TRUE, TAG_DONE);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   copy the current values
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG SetValues(Object *obj,struct IClass *cl)
{
	struct RenderWindow_Data *data = (struct RenderWindow_Data*)INST_DATA(cl,obj);
	char *s;
	ULONG num;
	float fnum;

	GetAttr(MUIA_String_Contents,data->pf_name,(ULONG*)&s);
	if(strlen(s))
		global.SetRenderdPic(s);
	else
		global.SetRenderdPic(NULL);

	GetAttr(MUIA_String_Contents,data->ns_xres,(ULONG*)&s);
	global.xres = atol(s);
	GetAttr(MUIA_String_Contents,data->ns_yres,(ULONG*)&s);
	global.yres = atol(s);

	// update camera field of view
	if(data->camera->flags & OBJECT_CAMERA_VFOV)
	{
		fnum = data->camera->hfov*global.yres/global.xres;
		if(data->camera->vfov != fnum)
		{
			data->flags |= FLAGS_REDRAW;
			data->camera->vfov = fnum;
		}
	}

	GetAttr(MUIA_Selected,data->cm_enable,&num);
	global.enablearea = num ? TRUE : FALSE;
	GetAttr(MUIA_String_Contents,data->ns_xmin,(ULONG*)&s);
	global.xmin = (atof(s)/global.xres);
	GetAttr(MUIA_String_Contents,data->ns_xmax,(ULONG*)&s);
	global.xmax = ((atof(s)+1)/global.xres);
	GetAttr(MUIA_String_Contents,data->ns_ymin,(ULONG*)&s);
	global.ymin = (atof(s)/global.yres);
	GetAttr(MUIA_String_Contents,data->ns_ymax,(ULONG*)&s);
	global.ymax = ((atof(s)+1)/global.yres);

	GetAttr(MUIA_Selected,data->cm_quick,&num);
	global.quick = num ? TRUE : FALSE;
	GetAttr(MUIA_Selected,data->cm_show,&num);
	global.show = num ? TRUE : FALSE;
	GetAttr(MUIA_StringSlider_Value, data->ls_minobjects, (ULONG*)&fnum);
	global.minobjects = (ULONG)fnum;

	GetAttr(MUIA_Cycle_Active,data->cy_fmt,&num);
	global.SetPicType(formats[num]);

	GetAttr(MUIA_Slider_Level,data->sl_depth,(ULONG*)&global.octreedepth);

	return 0;
}

/*************
 * DESCRIPTION:   this function is called when the ok button is pressed
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Ok(struct IClass *cl,Object *obj,Msg msg)
{
	struct RenderWindow_Data *data = (struct RenderWindow_Data*)INST_DATA(cl,obj);

	SetValues(obj,cl);

	DOMETHOD(app, MUIM_Application_PushMethod, data->originator, 4, MUIM_RenderWindow_Finish, obj, TRUE, (data->flags & FLAGS_REDRAW) ? TRUE : FALSE);

	return 0;
}

/*************
 * DESCRIPTION:   set resolution according to list view
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG SetRes(struct IClass *cl,Object *obj,Msg msg)
{
	char buffer[30];
	struct RenderWindow_Data *data = (struct RenderWindow_Data*)INST_DATA(cl,obj);
	ULONG active;

	GetAttr(MUIA_List_Active,data->lv_res,&active);

	sprintf(buffer, "%d", data->res[active*2]);
	SetAttrs(data->ns_xres, MUIA_String_Contents, buffer, TAG_DONE);

	sprintf(buffer, "%d", (int)((float)(data->res[active*2])*global.xmin));
	SetAttrs(data->ns_xmin, MUIA_String_Contents, buffer, TAG_DONE);

	sprintf(buffer, "%d", (int)((float)(data->res[active*2])*global.xmax)-1);
	SetAttrs(data->ns_xmax, MUIA_String_Contents, buffer, TAG_DONE);

	sprintf(buffer, "%d", data->res[active*2+1]);
	SetAttrs(data->ns_yres, MUIA_String_Contents, buffer, TAG_DONE);

	sprintf(buffer, "%d", (int)((float)(data->res[active*2+1])*global.ymin));
	SetAttrs(data->ns_ymin, MUIA_String_Contents, buffer, TAG_DONE);

	sprintf(buffer, "%d", (int)((float)(data->res[active*2+1])*global.ymax)-1);
	SetAttrs(data->ns_ymax, MUIA_String_Contents, buffer, TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:   View an image
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG View(struct IClass *cl,Object *obj,struct MUIP_Object* msg)
{
	char *file;

	SetAttrs(app,MUIA_Application_Sleep,TRUE, TAG_DONE);

	GetAttr(MUIA_String_Contents, msg->obj, (ULONG*)&file);

	ViewPicture(obj, file);

	SetAttrs(app,MUIA_Application_Sleep,FALSE, TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:   dispose object
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Dispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct RenderWindow_Data *data = (struct RenderWindow_Data*)INST_DATA(cl,obj);
	int i;

	if(data->flags & FLAGS_RESFILE)
	{
		ASSERT(!data->resolutions);
		i = 0;
		while(data->resolutions[i])
		{
			delete data->resolutions[i];
			i++;
		}
		delete data->resolutions;
		ASSERT(!data->res);
		delete data->res;
	}

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(RenderWindow_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
		case MUIM_RenderWindow_Ok:
			return(Ok(cl,obj,msg));
		case MUIM_RenderWindow_SetRes:
			return(SetRes(cl,obj,msg));
		case MUIM_RenderWindow_View:
			return(View(cl,obj,(struct MUIP_Object*)msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
