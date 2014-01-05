/***************
 * PROGRAM:       Modeler
 * NAME:          float_class.cpp
 * DESCRIPTION:   functions for float subclass
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    20.07.97 ah    initial release
 *
 * Tags                          type     default
 * --------------------------------------------------------------------------
 * MUIA_Float_Min          I--   float    1     lower border
 * MUIA_Float_Max          I--   float    100   upper border
 * MUIA_Float_LowerBound   I--   BOOL     FALSE limit to lower border
 * MUIA_Float_UpperBound   I--   BOOL     FALSE limit to upper border
 * MUIA_Float_LowerInside  I--   BOOL     FALSE lower limit is in valid range
 * MUIA_Float_UppderInside I--   BOOL     FALSE upper limit is in valid range
 * MUIA_Float_Value        ISG   float    1     value
 * MUIA_Float_Label        I--   char*    NULL  label of string box
 * MUIA_Float_ControlChar  I--   char*    NULL  control char of gadget
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

#ifndef FLOAT_CLASS_H
#include "float_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

/*************
 * DESCRIPTION:   do initialations for float subclass
 * INPUT:         cl       class
 *                obj      object
 *                msg      message
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Float_Data *data,databuf;
	char buffer[32];
	float value;

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Group_Horiz, TRUE,
		Child, KeyFloatString(
			databuf.fs_gadget,
			GetTagData(MUIA_Float_Label,NULL,msg->ops_AttrList),
			NULL,
			GetTagData(MUIA_Float_ControlChar,NULL,msg->ops_AttrList)),
		End,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct Float_Data*)INST_DATA(cl,obj);
		*data = databuf;
		value = (float)WARPOS_PPC_FUNC(GetTagData)(MUIA_Float_Value,1,msg->ops_AttrList);
		data->lower = WARPOS_PPC_FUNC(GetTagData)(MUIA_Float_Min,1,msg->ops_AttrList);
		data->upper = WARPOS_PPC_FUNC(GetTagData)(MUIA_Float_Max,100,msg->ops_AttrList);
		data->lowerbound = (BOOL)WARPOS_PPC_FUNC(GetTagData)(MUIA_Float_LowerBound,FALSE,msg->ops_AttrList);
		data->upperbound = (BOOL)WARPOS_PPC_FUNC(GetTagData)(MUIA_Float_UpperBound,FALSE,msg->ops_AttrList);
		data->lowerinside = (BOOL)WARPOS_PPC_FUNC(GetTagData)(MUIA_Float_LowerInside,FALSE,msg->ops_AttrList);
		data->upperinside = (BOOL)WARPOS_PPC_FUNC(GetTagData)(MUIA_Float_UpperInside,FALSE,msg->ops_AttrList);

		Float2String(value,buffer);
		SetAttrs(data->fs_gadget, MUIA_String_Contents, buffer, TAG_DONE);

		DOMETHOD(data->fs_gadget, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			obj, 1, MUIM_Float_StringAck);

		return((ULONG)obj);
	}
	return(0);
}

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

/*************
 * DESCRIPTION:   this method is called whenever the user presses return in
 *    the float string object
 * INPUT:         cl       class
 *                obj      object
 *                msg      message
 * OUTPUT:
 *************/
static ULONG StringAck(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct Float_Data *data = (struct Float_Data *)INST_DATA(cl,obj);
	char *s;
	float value;
	char buffer[32];

	GetAttr(MUIA_String_Contents,data->fs_gadget,(ULONG*)&s);

	value = atof(s);
	if(data->lowerbound && (((value < data->lower) && data->lowerinside) || ((value <= data->lower) && !data->lowerinside)))
	{
		value = data->value;
		Float2String(value, buffer);
		SetAttrs(data->fs_gadget, MUIA_String_Contents, buffer, TAG_DONE);
		DisplayBeep(_screen(obj));
	}
	if(data->upperbound && (((value > data->upper) && data->upperinside) || ((value >= data->upper) && !data->upperinside)))
	{
		value = data->value;
		Float2String(value, buffer);
		SetAttrs(data->fs_gadget, MUIA_String_Contents, buffer, TAG_DONE);
		DisplayBeep(_screen(obj));
	}
	data->value = value;

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:
 * OUTPUT:
 *************/
SAVEDS ASM ULONG Float_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet*)msg));
		case OM_SET:
			return(Set(cl,obj,(struct opSet*)msg));
		case OM_GET:
			return(Get(cl,obj,(struct opGet*)msg));
		case MUIM_Float_StringAck:
			return(StringAck(cl,obj,(struct opGet*)msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
