/***************
 * PROGRAM:       Modeler
 * NAME:          globalwindow_class.cpp
 * DESCRIPTION:   global window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    25.04.96 ah    initial release
 ***************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libraries/asl.h>

#include <pragma/dos_lib.h>
#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef GLOBALWINDOW_CLASS_H
#include "globalwindow_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

static char *sampling[] =
{
	"None",
	"2x2",
	"3x3",
	"4x4",
	"5x5",
	"6x6",
	"7x7",
	"8x8",
	NULL
};

static char *pages[] =
{
	"Environment",
	"Misc",
	NULL
};

/*************
 * DESCRIPTION:   do initialations for global window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct GlobalWindow_Data *data,databuf;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title, "Global",
		MUIA_Window_ID, MAKE_ID('G','L','O','B'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, RegisterGroup(pages),
				MUIA_Register_Frame, TRUE,

				Child, VGroup,
					Child, ColGroup(4), GroupFrameT("Colors"),
						Child, Label2("Background color"), Child, databuf.cf_backcol = Colorfield,
						Child, Label2("Ambient color"), Child, databuf.cf_ambient = Colorfield,
					End,
					Child, HGroup, GroupFrameT("Backdrop picture"),
						Child, HGroup,
							Child, KeyLabel2("Enable",'e'),
							Child, databuf.cm_backenable = CheckMarkNew(global.GetEnableBack(),'e'),
							Child, databuf.pf_backpic = PopaslObject,
								MUIA_CycleChain,TRUE,
								MUIA_Popstring_String, KeyString(NULL,256,'b'), End,
								MUIA_Popstring_Button, PopButton(MUII_PopFile),
								ASLFR_TitleText, "Please select a Picture...",
							End,
							Child, databuf.b_viewbd = ImageButton(viewimage),
								MUIA_InputMode, MUIV_InputMode_RelVerify,
								TAG_DONE,
							End,
						End,
					End,
					Child, HGroup, GroupFrameT("Reflection map"),
						Child, HGroup,
							Child, KeyLabel2("Enable",'n'),
							Child, databuf.cm_reflenable = CheckMarkNew(global.GetEnableRefl(),'n'),
							Child, databuf.pf_reflmap = PopaslObject,
								MUIA_CycleChain,TRUE,
								MUIA_Popstring_String, KeyString(NULL,256,'r'), End,
								MUIA_Popstring_Button, PopButton(MUII_PopFile),
								ASLFR_TitleText, "Please select a Picture...",
								End,
							Child, databuf.b_viewrm = ImageButton(viewimage),
								MUIA_InputMode, MUIV_InputMode_RelVerify,
								TAG_DONE,
							End,
						End,
					End,
					Child, VGroup, GroupFrameT("Fog"),
						Child, ColGroup(2),
							Child, Label2("Fog color"), Child, databuf.cf_fog = Colorfield,
						End,
						Child, HGroup,
							Child, databuf.fs_flen = (Object*)NewObject(CL_Float->mcc_Class, NULL,
								MUIA_Float_Label, "Fog length",
								MUIA_Float_ControlChar, 'l',
								MUIA_Float_Min, 0,
								MUIA_Float_LowerBound, TRUE,
								MUIA_Float_LowerInside, TRUE,
							End,
							Child, databuf.fs_fheight = (Object*)NewObject(CL_Float->mcc_Class, NULL,
								MUIA_Float_Label, "Fog height",
								MUIA_Float_ControlChar, 'h',
							End,
						End,
					End,
					Child, VSpace(0),
				End,

				Child, VGroup,
					Child, HGroup,
						Child, VGroup, GroupFrameT("Antialiasing"),
							Child, ColGroup(2),
								Child, KeyLabel2("Level",'v'),
								Child, databuf.cy_antialias = CycleObject,
									MUIA_CycleChain,TRUE,
									MUIA_ControlChar, 'v',
									MUIA_Cycle_Entries, sampling,
									MUIA_ShortHelp, "Level of antialiasing",
								End,
								Child, Label2("Contrast"), Child, databuf.cf_anticont = Colorfield,
							End,
							Child, databuf.fs_gauss = (Object*)NewObject(CL_Float->mcc_Class, NULL,
								MUIA_Float_Label, "Filter width",
								MUIA_Float_ControlChar, 'w',
								MUIA_Float_Min, 0,
								MUIA_Float_LowerBound, TRUE,
								MUIA_ShortHelp, "Width of the gaussian filter",
							End,
						End,

						Child, VGroup, GroupFrameT("Softshadow"),
							Child, ColGroup(2),
/*                      Child, KeyLabel2("Motionblur",'m'),
								Child, databuf.cy_distrib = CycleObject,
									MUIA_CycleChain,TRUE,
									MUIA_ControlChar, 'd',
									MUIA_Cycle_Entries, sampling,
									MUIA_ShortHelp, "Motionblur oversampling",
								End,*/
								Child, KeyLabel2("Softshadow",'s'),
								Child, databuf.cy_softshad = CycleObject,
									MUIA_CycleChain,TRUE,
									MUIA_ControlChar, 's',
									MUIA_Cycle_Entries, sampling,
									MUIA_ShortHelp, "Softshadow oversampling",
								End,
							End,
							Child, ColGroup(3),
								Child, KeyLabel2("Random jitter",'j'), Child, databuf.cm_randjit = CheckMarkNew(global.randjit,'j'), Child, HSpace(0),
							End,
							Child, VSpace(0),
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
		data = (struct GlobalWindow_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->originator = (Object *)WARPOS_PPC_FUNC(GetTagData)(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);

		SetAttrs(obj, MUIA_Window_ActiveObject, data->b_ok, TAG_DONE);

//    SetAttrs(data->cy_distrib, MUIA_Disabled, TRUE, TAG_DONE);

		// set strings
		if(global.backpic)
			SetAttrs(data->pf_backpic, MUIA_String_Contents, global.backpic, TAG_DONE);
		DOMETHOD(data->pf_backpic, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			data->cm_backenable, 3, MUIM_Set, MUIA_Selected, TRUE);
		if(global.reflmap)
			SetAttrs(data->pf_reflmap, MUIA_String_Contents, global.reflmap, TAG_DONE);
		DOMETHOD(data->pf_reflmap, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			data->cm_reflenable, 3, MUIM_Set, MUIA_Selected, TRUE);

		// set color field values
		SetColorField(data->cf_backcol, &global.backcol);
		SetColorField(data->cf_ambient, &global.ambient);
		SetColorField(data->cf_fog, &global.fog);
		SetColorField(data->cf_anticont, &global.anticont);

		// set floating point fields
		SetAttrs(data->fs_flen, MUIA_Float_Value, &global.flen, TAG_DONE);
		SetAttrs(data->fs_fheight, MUIA_Float_Value, &global.fheight, TAG_DONE);
		SetAttrs(data->fs_gauss, MUIA_Float_Value, &global.gauss, TAG_DONE);

		// set sliders
		SetAttrs(data->cy_antialias, MUIA_Cycle_Active, global.antialias-1, TAG_DONE);
//    SetAttrs(data->cy_distrib, MUIA_Cycle_Active, global.distrib-1, TAG_DONE);
		SetAttrs(data->cy_softshad, MUIA_Cycle_Active, global.softshad-1, TAG_DONE);

		DOMETHOD(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_GlobalWindow_Ok);

		// close methods
		DOMETHOD(data->b_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_ChildWindow_Finish, obj, FALSE);
		DOMETHOD(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_ChildWindow_Finish, obj, FALSE);

		// open color window if someone click on colorfields
		DOMETHOD(data->cf_backcol,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_GlobalWindow_OpenColor,data->cf_backcol);
		DOMETHOD(data->cf_ambient,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_GlobalWindow_OpenColor,data->cf_ambient);
		DOMETHOD(data->cf_fog,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_GlobalWindow_OpenColor,data->cf_fog);
		DOMETHOD(data->cf_anticont,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_GlobalWindow_OpenColor,data->cf_anticont);

		// view pictures
		DOMETHOD(data->b_viewbd,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_GlobalWindow_View,data->pf_backpic);
		DOMETHOD(data->b_viewrm,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,2,MUIM_GlobalWindow_View,data->pf_reflmap);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   this funtion is called everytime the user presses th ok button
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Ok(struct IClass *cl,Object *obj,Msg msg)
{
	struct GlobalWindow_Data *data = (struct GlobalWindow_Data*)INST_DATA(cl,obj);
	char *s;
	ULONG flag;

	// set strings
	GetAttr(MUIA_String_Contents,data->pf_backpic,(ULONG*)&s);
	if(strlen(s))
		global.SetBackPic(s);
	else
		global.SetBackPic(NULL);

	GetAttr(MUIA_String_Contents,data->pf_reflmap,(ULONG*)&s);
	if(strlen(s))
		global.SetReflMap(s);
	else
		global.SetReflMap(NULL);

	// set surface colors
	GetColorField(data->cf_backcol,&global.backcol);
	GetColorField(data->cf_ambient,&global.ambient);
	GetColorField(data->cf_fog,&global.fog);
	GetColorField(data->cf_anticont,&global.anticont);

	// set surface values
	GetAttr(MUIA_Float_Value, data->fs_flen, (ULONG*)&global.flen);
	GetAttr(MUIA_Float_Value, data->fs_fheight, (ULONG*)&global.fheight);
	GetAttr(MUIA_Float_Value, data->fs_gauss, (ULONG*)&global.gauss);

	// set values
	GetAttr(MUIA_Cycle_Active,data->cy_antialias,(ULONG*)&global.antialias);
	global.antialias++;
// GetAttr(MUIA_Cycle_Active,data->cy_distrib,(ULONG*)&global.distrib);
// global.distrib++;
	GetAttr(MUIA_Cycle_Active,data->cy_softshad,(ULONG*)&global.softshad);
	global.softshad++;

	// set checkmarks
	GetAttr(MUIA_Selected,data->cm_backenable,(ULONG*)&flag);
	global.SetEnableBack(flag ? TRUE : FALSE);
	GetAttr(MUIA_Selected,data->cm_reflenable,(ULONG*)&flag);
	global.SetEnableRefl(flag ? TRUE : FALSE);
	GetAttr(MUIA_Selected,data->cm_randjit,(ULONG*)&flag);
	global.randjit = flag ? TRUE : FALSE;

	DOMETHOD(app, MUIM_Application_PushMethod, data->originator, 3, MUIM_ChildWindow_Finish, obj, TRUE);

	return 0;
}

/*************
 * DESCRIPTION:   open color pop up window
 * INPUT:         system
 * OUTPUT:        0
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
		SetAttrs(newobj,MUIA_Window_Open,TRUE, TAG_DONE);
	}
	return 0;
}

/*************
 * DESCRIPTION:   close color pop up window
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG CloseColor(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish* msg)
{
	Object *win;

	win = msg->win;
	SetAttrs(win,MUIA_Window_Open,FALSE, TAG_DONE);

	DOMETHOD(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);

	SetSleep(FALSE);

	return 0;
}

/*************
 * DESCRIPTION:   View a image
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
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(GlobalWindow_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
		case MUIM_GlobalWindow_Ok:
			return(Ok(cl,obj,msg));
		case MUIM_GlobalWindow_OpenColor:
			return(OpenColor(cl,obj,(struct MUIP_Object*)msg));
		case MUIM_Color_Finish:
			return(CloseColor(cl,obj,(struct MUIP_ChildWindow_Finish*)msg));
		case MUIM_GlobalWindow_View:
			return(View(cl,obj,(struct MUIP_Object*)msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
