/***************
 * PROGRAM:       Modeler
 * NAME:          plugin_class.cpp
 * DESCRIPTION:   This class defines the window which holds the plugin dialog
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.09.98 ah    initial release
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

#ifndef PLUGIN_CLASS_H
#include "plugin_class.h"
#endif

#ifndef DIALOG_CLASS_H
#include "dialog_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

/*************
 * FUNCTION:      New
 * DESCRIPTION:   do initialations for Plugin window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Plugin_Data databuf, *data;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title, "Plugin",
		MUIA_Window_ID, MAKE_ID('P','L','U','G'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			databuf.lv_dialog = (Object *)NewObject(CL_Dialog->mcc_Class,NULL),
			Child, HGroup,
				Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE, End,
				Child, databuf.b_cancel = KeyTextButton("Cancel",'c'), TAG_DONE, End,
			End,
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct Plugin_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->originator = (Object*)WARPOS_PPC_FUNC(GetTagData)(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);
		SetAttrs(obj, MUIA_Window_ActiveObject, data->b_ok, TAG_DONE);

		DOMETHOD(data->b_cancel,MUIM_Notify,MUIA_Pressed,FALSE,
			MUIV_Notify_Application, 6, MUIM_Application_PushMethod, data->originator,
			3, MUIM_ChildWindow_Finish, obj, FALSE);
		DOMETHOD(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			MUIV_Notify_Application, 6, MUIM_Application_PushMethod, data->originator,
			3, MUIM_ChildWindow_Finish, obj, FALSE);

		// close requester with ok buttom
		DOMETHOD(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj,1,MUIM_Plugin_Ok);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * FUNCTION:      Ok
 * DESCRIPTION:   this function is called when the user clicks the ok
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Ok(struct IClass *cl,Object *obj,Msg msg)
{
	struct Plugin_Data *data = (struct Plugin_Data*)INST_DATA(cl,obj);

		// close the window
		DOMETHOD(app, MUIM_Application_PushMethod, data->originator, 3, MUIM_ChildWindow_Finish, obj, TRUE);
	}

	return 0;
}

/*************
 * FUNCTION:      Plugin_Dispatcher
 * DESCRIPTION:   Dispatcher of plugin class
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(Plugin_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet*)msg));
		case MUIM_Plugin_Ok:
			return(Ok(cl,obj,msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
