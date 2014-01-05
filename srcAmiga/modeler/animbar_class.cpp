/***************
 * PROGRAM:       Modeler
 * NAME:          animbar_class.cpp
 * DESCRIPTION:   functions for animation bar subclass
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.11.98 ah    initial release
 *
 * Tags                          type     default
 * --------------------------------------------------------------------------
 ***************/

#include <math.h>
#include <stdlib.h>

#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef ANIMBAR_CLASS_H
#include "animbar_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

/*************
 * DESCRIPTION:   do initialations for animbar subclass
 * INPUT:         cl       class
 *                obj      object
 *                msg      message
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct AnimBar_Data *data,databuf;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Group_Horiz, TRUE,
		Child, databuf.b_first = ImageButton(tapefirstimage),
			MUIA_ShortHelp, "First",
		End,
		Child, databuf.b_prev = ImageButton(tapeprevimage),
			MUIA_ShortHelp, "Prev",
		End,
		Child, databuf.b_stop = ImageButton(tapestopimage),
			MUIA_ShortHelp, "Stop",
		End,
		Child, databuf.b_play = ImageButton(tapeplayimage),
			MUIA_ShortHelp, "Play",
		End,
		Child, databuf.b_next = ImageButton(tapenextimage),
			MUIA_ShortHelp, "Next",
		End,
		Child, databuf.b_last = ImageButton(tapelastimage),
			MUIA_ShortHelp, "Last",
		End,
		Child, databuf.sl_frame = Slider(0,100,0),
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct AnimBar_Data*)INST_DATA(cl,obj);
		*data = databuf;

		return((ULONG)obj);
	}
	return(0);
}

#if 0
/*************
 * DESCRIPTION:   OM_SET method
 * INPUT:         cl       class
 *                obj      object
 *                msg      message
 * OUTPUT:
 *************/
static ULONG Set(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Float_Data *data = (struct Float_Data *)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;
	char buffer[32];

	tags=msg->ops_AttrList;
	do
	{
		tag = WARPOS_PPC_FUNC(NextTagItem)(&tags);
		if(tag)
		{
			switch (tag->ti_Tag)
			{
				case MUIA_Float_Value:
					data->value = *(float*)tag->ti_Data;
					Float2String(data->value, buffer);
					SetAttrs(data->fs_gadget, MUIA_String_Contents, buffer, TAG_DONE);
					break;
			}
		}
	}
	while(tag);

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   OM_GET method
 * INPUT:         cl       class
 *                obj      object
 *                msg      message
 * OUTPUT:
 *************/
static ULONG Get(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct Float_Data *data = (struct Float_Data *)INST_DATA(cl,obj);
	char *s;

	switch (msg->opg_AttrID)
	{
		case MUIA_Float_Value:
			GetAttr(MUIA_String_Contents,data->fs_gadget,(ULONG*)&s);
			*(float*)(msg->opg_Storage) = atof(s);
			return TRUE;
	}

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}
#endif

/*************
 * DESCRIPTION:
 * INPUT:
 * OUTPUT:
 *************/
SAVEDS ASM ULONG AnimBar_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet*)msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
