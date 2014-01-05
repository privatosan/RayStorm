/***************
 * PROGRAM:       Modeler
 * NAME:          statewindow_class.cpp
 * DESCRIPTION:   state window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    16.07.96 ah    initial release
 *    25.02.97 ah    added display of estimated time
 ***************/

#include <string.h>
#include <stdio.h>

#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
extern struct GfxBase *GfxBase;
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <pragma/utility_lib.h>
#include <pragma/dos_lib.h>
#include <pragma/exec_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef STATEWINDOW_CLASS_H
#include "statewindow_class.h"
#endif

#ifndef DISPWINDOW_CLASS_H
#include "dispwindow_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef KEFILE_LIB_H
#include "pragma/keyfile_lib.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef MSGHANDLER_CLASS_H
#include "msghandler_class.h"
#endif

/*************
 * DESCRIPTION:   do initialations for state window
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct StateWindow_Data *data, databuf;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_ID, MAKE_ID('S','T','A','T'),
		MUIA_Window_CloseGadget, FALSE,
		MUIA_Window_NoMenus, TRUE,
		WindowContents, VGroup,
			Child, VGroup,
				Child, databuf.lv_log = ListviewObject,
					MUIA_Listview_Input, FALSE,
					MUIA_Listview_List, ListObject,
						ReadListFrame,
						MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
						MUIA_List_DestructHook, MUIV_List_DestructHook_String,
					End,
				End,
				Child, databuf.g_perc = GaugeObject,
					GaugeFrame,
					MUIA_Weight, 20,
					MUIA_Gauge_Horiz, TRUE,
					MUIA_Gauge_Max, 65535,
				End,
				Child, ScaleObject, MUIA_Scale_Horiz, TRUE, End,
				Child, HGroup,
					Child, databuf.t_tspend = TextObject,
						MUIA_Text_PreParse, MUIX_L,
						MUIA_Text_Contents, "Time spend: 00:00:00",
					End,
					Child, databuf.t_testimated = TextObject,
						MUIA_Text_PreParse, MUIX_C,
						MUIA_Text_Contents, "Time estimated: 00:00:00",
					End,
					Child, databuf.t_tleft = TextObject,
						MUIA_Text_PreParse, MUIX_R,
						MUIA_Text_Contents, "Time left: 00:00:00",
					End,
				End,
			End,
			Child, HGroup,
				Child, databuf.b_ok = SimpleButton("_Ok"),
				Child, databuf.b_show = SimpleButton("_Show"),
				Child, databuf.b_cancel = SimpleButton("_Cancel"),
			End,
		End,
		TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct StateWindow_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->originator = (Object *)GetTagData(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);
		data->display = (DISPLAY *)GetTagData(MUIA_StateWindow_Display,0,msg->ops_AttrList);
		// add signal for render task
		data->sigbit = AllocSignal(-1);
		if(data->sigbit == -1)
			data->sigbit = NULL;

		SetAttrs(data->b_ok, MUIA_Disabled, TRUE, TAG_DONE);
		DoMethod(data->b_ok,MUIM_Notify,MUIA_Pressed,FALSE,
			obj, 1, MUIM_StateWindow_Ok);

		SetAttrs(data->b_show, MUIA_Disabled, TRUE, TAG_DONE);
		DoMethod(data->b_show,MUIM_Notify,MUIA_Pressed,FALSE,
			obj, 1, MUIM_StateWindow_Show);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   write to log
 * INPUT:         text     text to print to log
 * OUTPUT:        none
 *************/
void WriteLog(void *rc, char *text)
{
	struct StateWindow_Data *data;
	ULONG entry;

	PPC_STUB(rsiGetUserData)(rc, (void**)&data);

	GetAttr(MUIA_List_Entries, data->lv_log, &entry);
	if(entry>50)
	{
		DoMethod(data->lv_log, MUIM_List_Remove, MUIV_List_Remove_First);
		entry--;
	}
	DoMethod(data->lv_log, MUIM_List_InsertSingle, text, MUIV_List_Insert_Bottom);
	DoMethod(data->lv_log, MUIM_List_Jump, entry);
}

/*************
 * DESCRIPTION:   update rendering status
 * INPUT:         percent     percent finished
 *                elapsed     elapsed seconds since start of rendering
 *                y           y coordinate of line
 *                lines       amount of lines
 *                line        last renderd line
 * OUTPUT:        none
 *************/
void UpdateStatus(void *rc, float percent, float elapsed, int y, int lines, rsiSMALL_COLOR *line)
{
	struct StateWindow_Data *data;
	char buffer[256];
	float end;

	PPC_STUB(rsiGetUserData)(rc, (void**)&data);

	SetAttrs(data->g_perc,MUIA_Gauge_Current,(ULONG)(percent*65535.),TAG_DONE);

	sprintf(buffer, "Time spend: %02d:%02d:%02d",
		(int)floor(elapsed/3600.f),
		(int)floor(elapsed/60.f) % 60,
		(int)floor(elapsed) % 60);
	SetAttrs(data->t_tspend,MUIA_Text_Contents,(ULONG)buffer,TAG_DONE);

	if(percent > 0.)
		end = elapsed / percent;
	else
		end = 0.;

	sprintf(buffer, "Time estimated: %02d:%02d:%02d",
		(int)floor(end/3600.f),
		(int)floor(end/60.f) % 60,
		(int)floor(end) % 60);
	SetAttrs(data->t_testimated,MUIA_Text_Contents,(ULONG)buffer,TAG_DONE);

	end = (1. - percent) * end;

	sprintf(buffer, "Time left: %02d:%02d:%02d",
		(int)floor(end/3600.f),
		(int)floor(end/60.f) % 60,
		(int)floor(end) % 60);
	SetAttrs(data->t_tleft,MUIA_Text_Contents,(ULONG)buffer,TAG_DONE);

	if(line && data->dispwin)
		DoMethod(app, MUIM_Application_PushMethod, data->dispwin, 4, MUIM_DispWindow_Render, y, lines, line);
}

/*************
 * DESCRIPTION:   check if cancel button is pressed
 * INPUT:         none
 * OUTPUT:        TRUE if pressed else FALSE
 *************/
BOOL CheckCancel(void *rc)
{
	struct StateWindow_Data *data;
	ULONG pressed;

	PPC_STUB(rsiGetUserData)(rc, (void**)&data);

	GetAttr(MUIA_Selected, data->b_cancel, &pressed);
	return pressed;
}

/*************
 * DESCRIPTION:   update window and input methods
 * INPUT:         none
 * OUTPUT:        none
 *************/
void Cooperate(void *rc)
{
	// Update window
	DoMethod(app,MUIM_Application_InputBuffered);
}

/*************
 * DESCRIPTION:   start the rendering process
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Render(struct IClass *cl,Object *obj,Msg msg)
{
	struct StateWindow_Data *data = (struct StateWindow_Data*)INST_DATA(cl,obj);
	static char title[256];
	char name[32];
	ULONG sernum;
	char *err;
	PREFS p;
	ULONG priority, stack;
	rsiCONTEXT *rc;
	BOOL subtaskrender = FALSE;

	data->dispwin = NULL;

	if(KeyfileBase)
	{
		keyGetInfo(name, &sernum);
		sprintf(title,"RayStorm registered by %s; Serial No.: %d", name, sernum);
	}
	else
		strcpy(title,"RayStorm Demo");

	SetAttrs(obj,MUIA_Window_Title,title, TAG_DONE);

	if(global.show && GfxBase->LibNode.lib_Version>=39)
	{
		// open display window
		data->dispwin = (Object*)NewObject(CL_DispWindow->mcc_Class,NULL,
			MUIA_ChildWindow_Originator,obj,TAG_DONE);
		if(data->dispwin)
		{
			DoMethod(app, OM_ADDMEMBER, data->dispwin);
			SetAttrs(data->dispwin, MUIA_Window_Open, TRUE, TAG_DONE);
			DoMethod(data->dispwin, MUIM_Render_ResetColors);
		}
		else
			MUI_Request(app,NULL,0,NULL,"*Ok","Can't open display window");
	}

	SetSleep(TRUE);
	err = sciRenderInit(WriteLog, UpdateStatus, CheckCancel, Cooperate, data, &rc);
	if(err)
	{
		if(err[0])
			MUI_Request(app,NULL,0,NULL,"*Ok",err);
		else
			MUI_Request(app,NULL,0,NULL,"*Ok","Unknown error");

		SetAttrs(data->b_ok, MUIA_Disabled, FALSE, TAG_DONE);
		SetAttrs(data->b_cancel, MUIA_Disabled, TRUE, TAG_DONE);
		SetSleep(FALSE);
		return 0;
	}

	if(data->sigbit)
	{
		p.id = ID_RPRI;
		if(p.GetPrefs())
			priority = *(ULONG*)p.data;
		else
			priority = -1;

		p.id = ID_STCK;
		if(p.GetPrefs())
			stack = *(ULONG*)p.data;
		else
			stack = 32;
		p.data = NULL;

		data->st = CreateSubTask(RenderSubTask, data->sigbit, "Scenario Render Task", priority, stack*1024);
		if(data->st)
		{
			data->renderdata.err = NULL;
			data->renderdata.rc = rc;
			data->renderdata.caller = obj;
			data->renderdata.msghandler = msghandler;
			// add this object to message handler, with that the
			// messages the subtask sends are handled.
			DoMethod(msghandler, MUIM_MsgHandler_AddObject, obj);
			SendSubTaskMsg(data->st, STC_START, &data->renderdata);
			SetSleep(FALSE);
			subtaskrender = TRUE;
		}
	}
	if(!subtaskrender)
	{
		err = sciRender(rc);
		DoMethod(obj, MUIM_StateWindow_RenderCleanup, err);
	}
	return 0;
}

/*************
 * DESCRIPTION:   start the rendering process
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG RenderCleanup(struct IClass *cl,Object *obj, MUIP_StateWindow_RenderCleanup *msg)
{
	struct StateWindow_Data *data = (struct StateWindow_Data*)INST_DATA(cl,obj);

	if(msg->err)
	{
		// close display window
		if(data->dispwin)
		{
			SetAttrs(data->dispwin, MUIA_Window_Open, FALSE, TAG_DONE);
			DoMethod(app, OM_REMMEMBER, data->dispwin);
			MUI_DisposeObject(data->dispwin);
			data->dispwin = NULL;
		}

		if(msg->err[0])
			MUI_Request(app,NULL,0,NULL,"*Ok",msg->err);
		else
			MUI_Request(app,NULL,0,NULL,"*Ok","Unknown error");

		SetAttrs(data->b_ok, MUIA_Disabled, FALSE, TAG_DONE);
		SetAttrs(data->b_cancel, MUIA_Disabled, TRUE, TAG_DONE);
		return 0;
	}

	SetAttrs(data->b_ok, MUIA_Disabled, FALSE, TAG_DONE);
	SetAttrs(data->b_show, MUIA_Disabled, FALSE, TAG_DONE);
	SetAttrs(data->b_cancel, MUIA_Disabled, TRUE, TAG_DONE);

	SetAttrs(obj,
		MUIA_Window_ActiveObject, data->b_ok,
		MUIA_Window_Activate, TRUE,
		TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:   this function is called when the user presses the OK-button
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Ok(struct IClass *cl,Object *obj,Msg msg)
{
	struct StateWindow_Data *data = (struct StateWindow_Data*)INST_DATA(cl,obj);

	// close display window
	if(data->dispwin)
	{
		SetAttrs(data->dispwin, MUIA_Window_Open, FALSE, TAG_DONE);
		DoMethod(app, OM_REMMEMBER, data->dispwin);
		MUI_DisposeObject(data->dispwin);
	}
	DoMethod(app, MUIM_Application_PushMethod, data->originator, 3, MUIM_ChildWindow_Finish, obj, TRUE);
	return 0;
}

/*************
 * DESCRIPTION:   Show renderd image
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Show(struct IClass *cl,Object *obj,Msg msg)
{
	SetAttrs(app,MUIA_Application_Sleep,TRUE, TAG_DONE);

	ViewPicture(obj, global.renderdpic);

	SetAttrs(app,MUIA_Application_Sleep,FALSE, TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:   cleanup
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Cleanup(struct IClass *cl,Object *obj,Msg msg)
{
	struct StateWindow_Data *data = (struct StateWindow_Data *)INST_DATA(cl,obj);

	if(data->st)
		KillSubTask(data->st);
	if(data->sigbit)
		FreeSignal(data->sigbit);

	return DoSuperMethodA(cl,obj,msg);
}

/*************
 * DESCRIPTION:   subtask is finished with rendering, remove it
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG SubTaskFinished(struct IClass *cl,Object *obj,Msg msg)
{
	struct StateWindow_Data *data = (struct StateWindow_Data*)INST_DATA(cl,obj);

	if(data->st)
	{
		KillSubTask(data->st);
		data->st = NULL;
	}
	if(data->sigbit)
		FreeSignal(data->sigbit);
	DoMethod(msghandler, MUIM_MsgHandler_RemoveObject, obj);

	DoMethod(obj, MUIM_StateWindow_RenderCleanup, data->renderdata.err);

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG StateWindow_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
		case MUIM_Cleanup:
			return(Cleanup(cl, obj, msg));
		case MUIM_StateWindow_Render:
			return(Render(cl,obj,msg));
		case MUIM_StateWindow_Ok:
			return(Ok(cl,obj,msg));
		case MUIM_StateWindow_Show:
			return(Show(cl,obj,msg));
		case MUIM_StateWindow_RenderCleanup:
			return(RenderCleanup(cl,obj, (struct MUIP_StateWindow_RenderCleanup*)msg));
		case MUIM_SubTask_Finished:
			return(SubTaskFinished(cl,obj,msg));
	}
	return(DoSuperMethodA(cl,obj,msg));
}
