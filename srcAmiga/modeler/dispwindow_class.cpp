/***************
 * PROGRAM:       Modeler
 * NAME:          dispwindow_class.cpp
 * DESCRIPTION:   state window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.09.96 ah    initial release
 ***************/

#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef DISPWINDOW_CLASS_H
#include "dispwindow_class.h"
#endif

#ifndef RENDER_CLASS_H
#include "render_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

/*************
 * DESCRIPTION:   do initialations for display window
 * INPUT:         cl
 *                obj
 *                Msg
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct DispWindow_Data databuf,*data;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Window_Title, "Display",
		MUIA_Window_ID, MAKE_ID('D','I','S','P'),
		MUIA_Window_NoMenus, TRUE,
		MUIA_Window_CloseGadget, FALSE,
//    MUIA_Window_UseBottomBorderScroller, TRUE,
//    MUIA_Window_UseRightBorderScroller, TRUE,
		WindowContents, VGroup,
			Child, ScrollgroupObject,
//          MUIA_Scrollgroup_UseWinBorder, TRUE,
				MUIA_Scrollgroup_Contents, VirtgroupObject,
					VirtualFrame,
					Child, databuf.renderarea = (Object *)NewObject(CL_Render->mcc_Class,NULL,
						MUIA_Background, MUII_BACKGROUND,
						MUIA_Render_XRes, global.xres,
						MUIA_Render_YRes, global.yres,
						TAG_DONE),
				End,
			End,
		End,
		TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct DispWindow_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->originator = (Object *)WARPOS_PPC_FUNC(GetTagData)(MUIA_ChildWindow_Originator,0,msg->ops_AttrList);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   Dispatcher for diplay window class.
 * INPUT:         cl       class
 *                obj      object
 *                msg      message
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG DispWindow_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	struct DispWindow_Data *data;

	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet *)msg));
		case MUIM_DispWindow_Render:
			data = (struct DispWindow_Data *)INST_DATA(cl,obj);
			SetAttrs(data->renderarea,
				MUIA_Render_Data, ((struct MUIP_DispWindow_Line *)msg)->line,
				MUIA_Render_Lines, ((struct MUIP_DispWindow_Line *)msg)->lines,
				TAG_DONE);
			DOMETHOD(data->renderarea, MUIM_Render_DrawUpdate, ((struct MUIP_DispWindow_Line *)msg)->y);
			return 0;
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
