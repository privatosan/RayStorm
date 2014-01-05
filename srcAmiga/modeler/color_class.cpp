/***************
 * PROGRAM:       Modeler
 * NAME:          color_class.cpp
 * DESCRIPTION:   class for color window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    09.02.96 ah    initial release
 ***************/

#include <stdio.h>
#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef COLOR_CLASS_H
#include "color_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

/*************
 * DESCRIPTION:   do initialations for color window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Color_Data *data, databuf;
	ULONG rgb[3];
// char buffer[30];

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title, "Color",
		MUIA_Window_ID, MAKE_ID('C','O','L','R'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
//       Child, HGroup,
				Child, databuf.cadj = ColoradjustObject,
					MUIA_CycleChain,TRUE,
				End,
/*          Child, VGroup,
					Child, KeyNumericString(databuf.ns_red, NULL, NULL, 'r'), End,
					Child, KeyNumericString(databuf.ns_green, NULL, NULL, 'g'), End,
					Child, KeyNumericString(databuf.ns_blue, NULL, NULL, 'b'), End,
				End,
			End,*/
			Child, HGroup,
				Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE,End,
				Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE,End,
			End,
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct Color_Data*)INST_DATA(cl,obj);
		*data = databuf;
		data->originator = (Object*)GetTagData(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);
		data->colorfield = (Object*)GetTagData(MUIA_Colorfield,0,msg->ops_AttrList);

		SetAttrs(obj, MUIA_Window_ActiveObject, data->b_ok, TAG_DONE);

		GetAttr(MUIA_Colorfield_Red, data->colorfield, &rgb[0]);
		GetAttr(MUIA_Colorfield_Green, data->colorfield, &rgb[1]);
		GetAttr(MUIA_Colorfield_Blue, data->colorfield, &rgb[2]);
		SetAttrs(data->cadj,MUIA_Coloradjust_RGB,rgb, TAG_DONE);

/*    sprintf(buffer, "%d", rgb[0]);
		SetAttrs(data->ns_red, MUIA_String_Contents, buffer, TAG_DONE);
		sprintf(buffer, "%d", rgb[1]);
		SetAttrs(data->ns_green, MUIA_String_Contents, buffer, TAG_DONE);
		sprintf(buffer, "%d", rgb[2]);
		SetAttrs(data->ns_blue, MUIA_String_Contents, buffer, TAG_DONE);*/

		DoMethod(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_Color_Finish, obj, FALSE);
		DoMethod(data->b_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_Color_Finish, obj, FALSE);
		DoMethod(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_Color_Set);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Set(struct IClass *cl,Object *obj,Msg msg)
{
	struct Color_Data *data = (struct Color_Data*)INST_DATA(cl,obj);
	ULONG rgb[3];

	GetAttr(MUIA_Coloradjust_Red, data->cadj, &rgb[0]);
	GetAttr(MUIA_Coloradjust_Green, data->cadj, &rgb[1]);
	GetAttr(MUIA_Coloradjust_Blue, data->cadj, &rgb[2]);
	SetAttrs(data->colorfield, MUIA_Colorfield_RGB, rgb, TAG_DONE);

	DoMethod(app, MUIM_Application_PushMethod, data->originator, 3, MUIM_Color_Finish, obj, TRUE);

	return 0;
}

/*************
 * DESCRIPTION:   OM_GET method
 * INPUT:         cl       class
 *                obj      object
 *                msg      message
 * OUTPUT:
 *************/
static ULONG Get(struct IClass *cl, Object *obj, struct opGet *msg)
{
	struct Color_Data *data = (struct Color_Data *)INST_DATA(cl,obj);

	switch (msg->opg_AttrID)
	{
		case MUIA_Colorfield:
			*(Object**)(msg->opg_Storage) = data->colorfield;
			return TRUE;
	}

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG Color_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet*)msg));
		case MUIM_Color_Set:
			return(Set(cl,obj,msg));
		case OM_GET:
			return(Get(cl,obj,(struct opGet*)msg));
	}
	return(DoSuperMethodA(cl,obj,msg));
}
