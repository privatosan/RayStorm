/***************
 * PROGRAM:       Modeler
 * NAME:          browser_class.cpp
 * DESCRIPTION:   class for browser window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    08.07.96 ah    initial release
 ***************/

#include <stdio.h>
#include <string.h>

#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef BROWSER_CLASS_H
#include "browser_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

/*************
 * FUNCTION:      New
 * DESCRIPTION:   do initialations for browser window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Browser_Data databuf, *data;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title, "Object browser",
		MUIA_Window_ID, MAKE_ID('B','R','O','W'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, databuf.lv_list = ListviewObject,
				MUIA_CycleChain,TRUE,
				MUIA_Listview_List, databuf.lv_listtree = (Object *)NewObject(CL_BrowserTree->mcc_Class,NULL,End,
			End,
			Child, HGroup,
				Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE, End,
				Child, databuf.b_show = KeyTextButton("Show",'s'), TAG_DONE, End,
				Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE, End,
			End,
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct Browser_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->originator = (Object*)WARPOS_PPC_FUNC(GetTagData)(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);
		data->mode = (int)WARPOS_PPC_FUNC(GetTagData)(MUIA_Browser_Mode,0,msg->ops_AttrList);
		data->dontselect = (BOOL)WARPOS_PPC_FUNC(GetTagData)(MUIA_Browser_DontSelect,FALSE,msg->ops_AttrList);
		data->draw_area = (Object*)WARPOS_PPC_FUNC(GetTagData)(MUIA_Browser_DrawArea,0,msg->ops_AttrList);
		data->redraw = FALSE;

		SetAttrs(obj, MUIA_Window_ActiveObject, data->b_ok, TAG_DONE);

		SetAttrs(data->lv_listtree,
			MUIA_BrowserTree_DrawArea, data->draw_area,
			MUIA_BrowserTree_Mode, data->mode,
			MUIA_BrowserTree_DontSelect, data->dontselect,
			TAG_DONE);

		DOMETHOD(data->b_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
			MUIV_Notify_Application, 6, MUIM_Application_PushMethod, data->originator,
			3, MUIM_ChildWindow_Finish, obj, FALSE);
		DOMETHOD(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			MUIV_Notify_Application, 6, MUIM_Application_PushMethod, data->originator,
			3, MUIM_ChildWindow_Finish, obj, FALSE);

		// close requester with ok buttom
		DOMETHOD(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_Browser_Ok);

		// close and select with double click
		DOMETHOD(data->lv_listtree,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,
			obj,1,MUIM_Browser_Ok);

		// show object if someone is pressing the show button
		DOMETHOD(data->b_show,MUIM_Notify,MUIA_Pressed,FALSE,
			data->lv_listtree,1,MUIM_BrowserTree_Show);

		DOMETHOD(obj, MUIM_Notify, MUIA_Window_Open, TRUE,
			obj, 1, MUIM_Browser_Build);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * FUNCTION:      Build
 * DESCRIPTION:   build object tree
 * INPUT:         system
 * OUTPUT:        -
 *************/
static ULONG Build(struct IClass *cl,Object *obj,Msg msg)
{
	struct Browser_Data *data = (struct Browser_Data *)INST_DATA(cl,obj);

	DOMETHOD(data->lv_listtree, MUIM_BrowserTree_Build);
	return 0;
}

/*************
 * FUNCTION:      Get
 * DESCRIPTION:   OM_GET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG Get(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct Browser_Data *data = (struct Browser_Data *)INST_DATA(cl,obj);

	switch (msg->opg_AttrID)
	{
		case MUIA_Browser_ActiveObject:
			// get entry from list
			if(data->ok)
				GetAttr(MUIA_BrowserTree_ActiveObject, data->lv_listtree, (ULONG*)msg->opg_Storage);
			else
				*(OBJECT**)(msg->opg_Storage) = NULL;
			return TRUE;
		case MUIA_Browser_Mode:
			*(ULONG*)msg->opg_Storage = data->mode;
			return TRUE;
	}

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * FUNCTION:      Ok
 * DESCRIPTION:   this function is called when the user clicks the ok button or
 *    double clicks on the list view
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Ok(struct IClass *cl,Object *obj,Msg msg)
{
	struct Browser_Data *data = (struct Browser_Data*)INST_DATA(cl,obj);

	if(!data->redraw)
	{
		// close the window
		DOMETHOD(app, MUIM_Application_PushMethod, data->originator, 3, MUIM_Browser_Finish, obj, TRUE);
	}

	return 0;
}

/*************
 * FUNCTION:      Browser_Dispatcher
 * DESCRIPTION:   Dispatcher of browser class
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(Browser_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet*)msg));
		case OM_GET:
			return(Get(cl,obj,(struct opGet*)msg));
		case MUIM_Browser_Ok:
			return(Ok(cl,obj,msg));
		case MUIM_Browser_Build:
			return Build(cl,obj,msg);
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
