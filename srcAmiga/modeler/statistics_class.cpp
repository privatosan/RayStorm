/***************
 * PROGRAM:       Modeler
 * NAME:          statistics_class.cpp
 * DESCRIPTION:   class for statistics window
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		05.12.96	ah		initial release
 ***************/

#include <string.h>
#include <stdio.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef STATISTICS_CLASS_H
#include "statistics_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef DEBUG_LIB
	#ifndef RSI_LIB_H
	#include "rsi_lib.h"
	#endif
#else
	#ifndef RSI_H
	#include "rsi.h"
	#endif
#endif

static SAVEDS ASM APTR ConstFunc(REG(a2) APTR pool, REG(a1) rsiSTAT *stat)
{
	rsiSTAT *newstat;

	newstat = new rsiSTAT;
	if(!newstat)
		return NULL;

	*newstat = *stat;
	return (APTR)newstat;
}

static SAVEDS ASM void DestFunc(REG(a2) APTR pool, REG(a1) rsiSTAT *stat)
{
	delete stat;
}

static SAVEDS ASM LONG DispFunc(REG(a1) rsiSTAT *stat, REG(a2) char **array)
{
	static char buffer[64], numbuffer[32];

	if(rsiGetStatDescription(stat, buffer, 64) != rsiERR_NONE)
		strcpy(buffer, "No description");
	*array++ = buffer;

	sprintf(numbuffer, "%d", stat->value);
	*array = numbuffer;

	return 0;
}

static const struct Hook ConstHook = { { NULL,NULL },(HOOKFUNC)ConstFunc,NULL,NULL };
static const struct Hook DestHook = { { NULL,NULL },(HOOKFUNC)DestFunc,NULL,NULL };
static const struct Hook DispHook = { { NULL,NULL },(HOOKFUNC)DispFunc,NULL,NULL };

/*************
 * FUNCTION:		New
 * DESCRIPTION:	do initialations for statistics window
 * INPUT:			cl			class
 *						obj		object
 *						msg		message
 * OUTPUT:			created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Statistics_Data *data,databuf;
	int num;
	rsiSTAT *stats;
	int i;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title, "Statistics",
		MUIA_Window_ID, MAKE_ID('S','T','C','S'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, databuf.lv_stats = ListviewObject,
				MUIA_Listview_Input, FALSE,
				MUIA_CycleChain,TRUE,
				MUIA_Listview_List, ListObject,
					ReadListFrame,
					MUIA_List_Format, ",P=33r",
					MUIA_List_ConstructHook, &ConstHook,
					MUIA_List_DestructHook, &DestHook,
					MUIA_List_DisplayHook, &DispHook,
				End,
			End,
			Child, databuf.b_ok = KeyTextButton("Ok",'o'), TAG_DONE,End,
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct Statistics_Data*)INST_DATA(cl,obj);
		*data = databuf;
		data->originator = (Object*)GetTagData(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);

		rsiGetStats(&stats, &num);
		for(i=0; i<num; i++)
			DoMethod(data->lv_stats, MUIM_List_InsertSingle, (APTR)&stats[i], MUIV_List_Insert_Bottom);

		DoMethod(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_ChildWindow_Finish, obj, FALSE);
		DoMethod(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			MUIV_Notify_Application,6,MUIM_Application_PushMethod,data->originator,
			3, MUIM_ChildWindow_Finish, obj, TRUE);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * FUNCTION:		Statistics_Dispatcher
 * DESCRIPTION:
 * INPUT:
 * OUTPUT:
 *************/
SAVEDS ASM ULONG Statistics_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch (msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
	}
	return(DoSuperMethodA(cl,obj,msg));
}
