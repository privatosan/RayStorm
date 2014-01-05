/***************
 * PROGRAM:       Modeler
 * NAME:          numreq_class.cpp
 * DESCRIPTION:   class for numeric requester
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.04.96 ah    initial release
 ***************/

#include <stdio.h>
#include <stdlib.h>
#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef NUMREQ_CLASS_H
#include "numreq_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

/*************
 * DESCRIPTION:   do initialations for numeric requester
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct NumReq_Data *data,databuf;
	char buffer[30];
	float *v;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_TopEdge, MUIV_Window_TopEdge_Moused,
		MUIA_Window_LeftEdge, MUIV_Window_TopEdge_Moused,
		MUIA_Window_TopEdge, MUIV_Window_TopEdge_Moused,
		MUIA_Window_LeftEdge, MUIV_Window_TopEdge_Moused,
		MUIA_Window_ID, MAKE_ID('N','U','M','R'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, HGroup,
				Child, databuf.t_num = MUI_MakeObject(MUIO_Label,NULL,MUIO_Label_DoubleFrame),
				Child, databuf.fs_num = StringObject,
					StringFrame,
					MUIA_String_MaxLen  , 30,
					MUIA_String_Accept,  "0123456789eE-+.",
					MUIA_CycleChain,TRUE,
				End,
			End,
			Child, HGroup,
				Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE,End,
				Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE,End,
			End,
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct NumReq_Data*)INST_DATA(cl,obj);
		*data = databuf;
		data->originator = (Object*)WARPOS_PPC_FUNC(GetTagData)(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);
		v = (float*)WARPOS_PPC_FUNC(GetTagData)(MUIA_NumReq_Value,0,msg->ops_AttrList);
		data->value = *v;
		data->text = (char*)WARPOS_PPC_FUNC(GetTagData)(MUIA_NumReq_Text,0,msg->ops_AttrList);
		SetAttrs(obj, MUIA_Window_Title, data->text, TAG_DONE);
		SetAttrs(obj, MUIA_Window_ActiveObject, data->b_ok, TAG_DONE);

		Float2String(data->value,buffer);
		SetAttrs(data->fs_num,MUIA_String_Contents,buffer, TAG_DONE);
		SetAttrs(data->t_num,MUIA_Text_Contents,data->text, TAG_DONE);

		DOMETHOD(data->b_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_NumReq_Finish, obj, FALSE);
		DOMETHOD(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_NumReq_Finish, obj, FALSE);
		DOMETHOD(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_NumReq_Set);

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
	struct NumReq_Data *data = (struct NumReq_Data *)INST_DATA(cl,obj);
	float *v;

	switch (msg->opg_AttrID)
	{
		case MUIA_NumReq_Value:
			v = (float*)(msg->opg_Storage);
			*v = data->value;
			return TRUE;
	}

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   set the numeric value if ok has been pressed
 * INPUT:         system
 * OUTPUT:
 *************/
ULONG NumReq_Set(struct IClass *cl,Object *obj,Msg msg)
{
	struct NumReq_Data *data = (struct NumReq_Data*)INST_DATA(cl,obj);
	char *s;

	GetAttr(MUIA_String_Contents,data->fs_num,(ULONG*)&s);
	data->value = atof(s);

	DOMETHOD(app, MUIM_Application_PushMethod, data->originator, 3, MUIM_NumReq_Finish, obj, TRUE);

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(NumReq_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
		case OM_GET:
			return(Get(cl,obj,(struct opGet*)msg));
		case MUIM_NumReq_Set:
			return(NumReq_Set(cl,obj,msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}

