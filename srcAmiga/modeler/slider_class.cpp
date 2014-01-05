/***************
 * PROGRAM:       Modeler
 * NAME:          slider_class.cpp
 * DESCRIPTION:   class for slider subclass
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    20.11.96 ah    initial release
 *
 * Tags                                type     default
 * --------------------------------------------------------------------------
 * MUIA_StringSlider_Min         IS-   ULONG    1     lower border
 * MUIA_StringSlider_Max         IS-   ULONG    100   upper border
 * MUIA_StringSlider_LowerBound  I--   BOOL     FALSE limit to lower border
 * MUIA_StringSlider_UpperBound  I--   BOOL     FALSE limit to upper border
 * MUIA_StringSlider_Value       ISG   float    1     value
 * MUIA_StringSlider_FLoat       I--   BOOL     TRUE  string accepts floats
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

#ifndef SLIDER_CLASS_H
#include "slider_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

/*************
 * DESCRIPTION:   do initialations for string slider subclass
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct StringSlider_Data *data,databuf;
	char buffer[32];
	float value;

	obj = (Object *)WARPOS_PPC_FUNC(DoSuperNew)(cl,obj,
		MUIA_Group_Columns, 2,
		Child, databuf.sl_slider = SliderObject,
			MUIA_CycleChain, TRUE,
		End,
		Child, databuf.fs_string = StringObject,
			StringFrame,
			MUIA_String_MaxLen, 30,
			MUIA_String_Accept, "0123456789eE.+-",
			MUIA_String_AdvanceOnCR, TRUE,
			MUIA_CycleChain, TRUE,
			MUIA_HorizWeight, 33,
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct StringSlider_Data*)INST_DATA(cl,obj);
		*data = databuf;
		value = (float)WARPOS_PPC_FUNC(GetTagData)(MUIA_StringSlider_Value,1,msg->ops_AttrList);
		data->lower = WARPOS_PPC_FUNC(GetTagData)(MUIA_StringSlider_Min,1,msg->ops_AttrList);
		data->upper = WARPOS_PPC_FUNC(GetTagData)(MUIA_StringSlider_Max,100,msg->ops_AttrList);
		data->lowerbound = (BOOL)WARPOS_PPC_FUNC(GetTagData)(MUIA_StringSlider_LowerBound,FALSE,msg->ops_AttrList);
		data->upperbound = (BOOL)WARPOS_PPC_FUNC(GetTagData)(MUIA_StringSlider_UpperBound,FALSE,msg->ops_AttrList);
		if((BOOL)WARPOS_PPC_FUNC(GetTagData)(MUIA_StringSlider_Float, TRUE, msg->ops_AttrList))
			SetAttrs(obj, MUIA_String_Accept, "0123456789eE.+-", TAG_DONE);
		else
			SetAttrs(obj, MUIA_String_Accept, "0123456789+-", TAG_DONE);

		Float2String(value,buffer);
		SetAttrs(data->fs_string, MUIA_String_Contents, buffer, TAG_DONE);
		SetAttrs(data->sl_slider, MUIA_Slider_Level, (int)ceil(value), TAG_DONE);
		SetAttrs(data->sl_slider, MUIA_Slider_Min, data->lower, TAG_DONE);
		SetAttrs(data->sl_slider, MUIA_Slider_Max, data->upper, TAG_DONE);

		DOMETHOD(data->sl_slider, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
			data->fs_string, 4, MUIM_SetAsString, MUIA_String_Contents, "%ld", MUIV_TriggerValue);

		DOMETHOD(data->fs_string, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			obj, 1, MUIM_StringSlider_StringAck);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   OM_SET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG Set(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct StringSlider_Data *data = (struct StringSlider_Data *)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;
	float value;
	char buffer[32];

	tags=msg->ops_AttrList;
	do
	{
		tag = WARPOS_PPC_FUNC(NextTagItem)(&tags);
		if(tag)
		{
			switch (tag->ti_Tag)
			{
				case MUIA_StringSlider_Value:
					value = *(float*)tag->ti_Data;
					Float2String(value, buffer);
					SetAttrs(data->fs_string, MUIA_String_Contents, buffer, TAG_DONE);
					SetAttrs(data->sl_slider, MUIA_NoNotify, TRUE, MUIA_Slider_Level, (int)ceil(value), TAG_DONE);
					break;
				case MUIA_StringSlider_Min:
					data->lower = tag->ti_Data;
					SetAttrs(data->sl_slider, MUIA_Slider_Min, data->lower, TAG_DONE);
					break;
				case MUIA_StringSlider_Max:
					data->upper = tag->ti_Data;
					SetAttrs(data->sl_slider, MUIA_Slider_Max, data->lower, TAG_DONE);
					break;
			}
		}
	}
	while(tag);

	return(WARPOS_PPC_FUNC(DoSuperMethodA)(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   OM_GET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG Get(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct StringSlider_Data *data = (struct StringSlider_Data *)INST_DATA(cl,obj);
	char *s;

	switch (msg->opg_AttrID)
	{
		case MUIA_StringSlider_Value:
			GetAttr(MUIA_String_Contents,data->fs_string,(ULONG*)&s);
			*(float*)(msg->opg_Storage) = atof(s);
			return TRUE;
	}

	return(WARPOS_PPC_FUNC(DoSuperMethodA)(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   this method is called whenever the user presses return in
 *    the float string object
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG StringAck(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct StringSlider_Data *data = (struct StringSlider_Data *)INST_DATA(cl,obj);
	char *s;
	float value;
	char buffer[32];

	GetAttr(MUIA_String_Contents,data->fs_string,(ULONG*)&s);

	value = atof(s);
	if(data->lowerbound && (value < data->lower))
	{
		value = data->lower;
		Float2String(value, buffer);
		SetAttrs(data->fs_string, MUIA_String_Contents, buffer, TAG_DONE);
	}
	if(data->upperbound && (value > data->upper))
	{
		value = data->upper;
		Float2String(value, buffer);
		SetAttrs(data->fs_string, MUIA_String_Contents, buffer, TAG_DONE);
	}

	SetAttrs(data->sl_slider, MUIA_NoNotify, TRUE, MUIA_Slider_Level, (int)ceil(value), TAG_DONE);

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:
 *************/
SAVEDS ASM ULONG StringSlider_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet*)msg));
		case OM_SET:
			return(Set(cl,obj,(struct opSet*)msg));
		case OM_GET:
			return(Get(cl,obj,(struct opGet*)msg));
		case MUIM_StringSlider_StringAck:
			return(StringAck(cl,obj,(struct opGet*)msg));
	}
	return(WARPOS_PPC_FUNC(DoSuperMethodA)(cl,obj,msg));
}
