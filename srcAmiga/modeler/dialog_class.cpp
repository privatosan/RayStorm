/***************
 * PROGRAM:       Modeler
 * NAME:          dialog_class.cpp
 * DESCRIPTION:   functions for RayStorm dialog subclass
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    28.05.98 ah    initial release
 *
 * Tags                          type              default
 * --------------------------------------------------------------------------
 * MUIA_Dialog_Dialog  I--   DIALOG_ELEMENT    NULL     structure with dialog definition
 * MUIA_Dialog_Data    I--   void*             NULL     structure with dialog data
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

#ifndef DIALOG_CLASS_H
#include "dialog_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

Object *CreateLabel(DIALOG_ELEMENT **dialog)
{
	char *text=NULL;

	text = (char*)**dialog;
	(*dialog)++;

	return Label2(text);
}

Object *CreateColorBox(DIALOG_ELEMENT **dialog, Object *win, UBYTE **d)
{
	ULONG id=0;
//   ULONG c;
//   COLOR color;
	Object *obj;

//   color.r = color.g = color.b = 0.f;

	/*while(**dialog != DIALOG_END)
	{
		switch(**dialog)
		{
			case DIALOG_COLORDEFAULT:
				c = (ULONG)*(*dialog + 1);
				color.r = (float)((c>>16) & 8)/255.f;
				color.g = (float)((c>>8) & 8)/255.f;
				color.b = (float)(c & 8)/255.f;
				break;
		}
		(*dialog) += 2;
	}
	(*dialog)++;*/

	obj =
		ColorfieldObject,
			ButtonFrame,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			MUIA_CycleChain, TRUE,
			MUIA_ObjectID, (ULONG)*d,
		End;

	if(obj)
	{
		SetColorField(obj, (COLOR*)*d);
		DoMethod(obj, MUIM_Notify, MUIA_Pressed, FALSE,
			win, 2, MUIM_Dialog_OpenColor, obj);
	}

	*d += sizeof(COLOR);

	return obj;
}

Object *CreateFloat(DIALOG_ELEMENT **dialog, Object *win, UBYTE **d)
{
	ULONG id=0;
	Object *obj;
	char buffer[32];

	obj =
		StringObject,
			StringFrame,
			MUIA_String_MaxLen  , 30,
			MUIA_String_Accept,  "0123456789eE-+.",
			MUIA_String_AdvanceOnCR, TRUE,
			MUIA_CycleChain, TRUE,
			MUIA_ObjectID, (ULONG)*d,
		End;

	if(obj)
	{
		Float2String(*((float*)*d), buffer);
		SetAttrs(obj, MUIA_String_Contents, buffer, TAG_DONE);
		DoMethod(obj, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			win, 2, MUIM_Dialog_FloatUpdate, obj);
	}

	*d += sizeof(float);

	return obj;
}

Object *CreateNumber(DIALOG_ELEMENT **dialog, Object *win, UBYTE **d)
{
	ULONG id=0;
	Object *obj;
	char buffer[32];
	ULONG positive = FALSE;

	while(**dialog != DIALOG_END)
	{
		switch(**dialog)
		{
			case DIALOG_NUMBER_POSITIVE:
				positive = (ULONG)*(*dialog + 1);
				break;
		}
		(*dialog) += 2;
	}
	(*dialog)++;

	if(positive)
	{
		obj =
			StringObject,
				StringFrame,
				MUIA_String_MaxLen  , 30,
				MUIA_String_Accept,  "0123456789",
				MUIA_String_AdvanceOnCR, TRUE,
				MUIA_CycleChain, TRUE,
				MUIA_ObjectID, (ULONG)*d,
			End;
	}
	else
	{
		obj =
			StringObject,
				StringFrame,
				MUIA_String_MaxLen  , 30,
				MUIA_String_Accept,  "0123456789-+",
				MUIA_String_AdvanceOnCR, TRUE,
				MUIA_CycleChain, TRUE,
				MUIA_ObjectID, (ULONG)*d,
			End;
	}

	if(obj)
	{
		sprintf(buffer, "%d", *((int*)*d));
		SetAttrs(obj, MUIA_String_Contents, buffer, TAG_DONE);
		DoMethod(obj, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			win, 2, MUIM_Dialog_IntUpdate, obj);
	}

	*d += sizeof(int);

	return obj;
}

Object *CreateSlider(DIALOG_ELEMENT **dialog, Object *win, UBYTE **d)
{
	ULONG id=0;
	Object *obj;
	ULONG min, max;
	float value;

	min = *(ULONG*)(*dialog);
	(*dialog)++;
	max = *(ULONG*)(*dialog);
	(*dialog)++;

	obj =
		(Object*)NewObject(CL_StringSlider->mcc_Class, NULL,
			MUIA_StringSlider_Min, min,
			MUIA_StringSlider_Max, max,
			MUIA_StringSlider_LowerBound, TRUE,
			MUIA_StringSlider_UpperBound, TRUE,
			MUIA_CycleChain, TRUE,
			MUIA_ObjectID, (ULONG)*d,
		End;

	if(obj)
	{
		value = *((ULONG*)*d);
		SetAttrs(obj, MUIA_StringSlider_Value, &value, TAG_DONE);
		DoMethod(obj, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
			win, 2, MUIM_Dialog_SliderUpdate, obj);
		DoMethod(obj, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			win, 2, MUIM_Dialog_SliderUpdate, obj);
	}

	*d += sizeof(float);

	return obj;
}

Object *CreateCheckBox(DIALOG_ELEMENT **dialog, Object *win, UBYTE **d)
{
	ULONG id=0;
	Object *obj;

	obj =
		ImageObject,
			ImageButtonFrame,
			MUIA_InputMode, MUIV_InputMode_Toggle,
			MUIA_Image_Spec, MUII_CheckMark,
			MUIA_Image_FreeVert, TRUE,
			MUIA_Background, MUII_ButtonBack,
			MUIA_ShowSelState, FALSE,
			MUIA_CycleChain,TRUE,
			MUIA_ObjectID, (ULONG)*d,
		End;

	if(obj)
	{
		SetAttrs(obj, MUIA_Selected, *((ULONG*)*d), TAG_DONE);
		DoMethod(obj, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			win, 2, MUIM_Dialog_CheckBoxUpdate, obj);
	}

	*d += sizeof(ULONG);

	return obj;
}

Object *CreateImage(DIALOG_ELEMENT **dialog, Object *win, UBYTE **d)
{
	ULONG id=0;
	Object *obj;
	ULONG width,height;

	width = *(ULONG*)(*dialog);
	(*dialog)++;
	height = *(ULONG*)(*dialog);
	(*dialog)++;

	obj =
		(Object *)NewObject(CL_Render->mcc_Class,NULL,
			MUIA_Background, MUII_BACKGROUND,
			MUIA_Frame, MUIV_Frame_Text,
			MUIA_Render_XRes, width,
			MUIA_Render_YRes, height,
		End,

	SetAttrs(obj, MUIA_Render_Lines, height, TAG_DONE);

	*(Object**)(*d) = obj;
	*d += sizeof(Object*);

	return obj;
}

ULONG CreateDialog(DIALOG_ELEMENT **dialog, TagItem *tags, Object *win, UBYTE **d)
{
	Object *obj;
	ULONG tag_items = 0, sub_tag_items;
	ULONG columns;

	do
	{
		switch(**dialog)
		{
			case DIALOG_HGROUP:
				(*dialog)++;
				sub_tag_items = CreateDialog(dialog, &tags[1], win, d);
				tags[sub_tag_items + 1].ti_Tag = TAG_DONE;
				tags[0].ti_Tag = MUIA_Group_Child;
				tags[0].ti_Data = (ULONG)MUI_NewObject(MUIC_Group, MUIA_Group_Horiz, TRUE, TAG_MORE, &tags[1]);
				tags++;
				tag_items++;
				break;
			case DIALOG_VGROUP:
				(*dialog)++;
				sub_tag_items = CreateDialog(dialog, &tags[1], win, d);
				tags[sub_tag_items + 1].ti_Tag = TAG_DONE;
				tags[0].ti_Tag = MUIA_Group_Child;
				tags[0].ti_Data = (ULONG)MUI_NewObject(MUIC_Group, TAG_MORE, &tags[1]);
				tags++;
				tag_items++;
				break;
			case DIALOG_COLGROUP:
				(*dialog)++;
				columns =  (ULONG)*(*dialog);
				(*dialog)++;
				sub_tag_items = CreateDialog(dialog, &tags[1], win, d);
				tags[sub_tag_items + 1].ti_Tag = TAG_DONE;
				tags[0].ti_Tag = MUIA_Group_Child;
				tags[0].ti_Data = (ULONG)MUI_NewObject(MUIC_Group, MUIA_Group_Columns, columns, TAG_MORE, &tags[1]);
				tags++;
				tag_items++;
				break;
			case DIALOG_CHECKBOX:
				(*dialog)++;
				obj = CreateCheckBox(dialog, win, d);
				if(obj)
				{
					tags[0].ti_Tag = MUIA_Group_Child;
					tags[0].ti_Data = (ULONG)obj;
					tag_items++;
					tags++;
				}
				break;
			case DIALOG_FLOAT:
				(*dialog)++;
				obj = CreateFloat(dialog, win, d);
				if(obj)
				{
					tags[0].ti_Tag = MUIA_Group_Child;
					tags[0].ti_Data = (ULONG)obj;
					tag_items++;
					tags++;
				}
				break;
			case DIALOG_NUMBER:
				(*dialog)++;
				obj = CreateNumber(dialog, win, d);
				if(obj)
				{
					tags[0].ti_Tag = MUIA_Group_Child;
					tags[0].ti_Data = (ULONG)obj;
					tag_items++;
					tags++;
				}
				break;
			case DIALOG_SLIDER:
				(*dialog)++;
				obj = CreateSlider(dialog, win, d);
				if(obj)
				{
					tags[0].ti_Tag = MUIA_Group_Child;
					tags[0].ti_Data = (ULONG)obj;
					tag_items++;
					tags++;
				}
				break;
			case DIALOG_LABEL:
				(*dialog)++;
				obj = CreateLabel(dialog);
				if(obj)
				{
					tags[0].ti_Tag = MUIA_Group_Child;
					tags[0].ti_Data = (ULONG)obj;
					tag_items++;
					tags++;
				}
				break;
			case DIALOG_COLORBOX:
				(*dialog)++;
				obj = CreateColorBox(dialog, win, d);
				if(obj)
				{
					tags[0].ti_Tag = MUIA_Group_Child;
					tags[0].ti_Data = (ULONG)obj;
					tag_items++;
					tags++;
				}
				break;
			case DIALOG_IMAGE:
				(*dialog)++;
				obj = CreateImage(dialog, win, d);
				if(obj)
				{
					tags[0].ti_Tag = MUIA_Group_Child;
					tags[0].ti_Data = (ULONG)obj;
					tag_items++;
					tags++;
				}
				break;
		}
	}
	while((**dialog != DIALOG_END) && (**dialog != DIALOG_FINISH));

	if(**dialog != DIALOG_FINISH)
		(*dialog)++;

	return tag_items;
}

/*************
 * DESCRIPTION:   do initialations for RayStorm dialog subclass
 * INPUT:         cl       class
 *                obj      object
 *                msg      message
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Dialog_Data *data,databuf;
	DIALOG_ELEMENT *dialog;
	TagItem taglist[100];
	ULONG tags;
	Object *group;
	UBYTE *d;

	databuf.dialog = dialog = (DIALOG_ELEMENT*)GetTagData(MUIA_Dialog_Dialog, NULL, msg->ops_AttrList);
	databuf.data = (void*)GetTagData(MUIA_Dialog_Data, NULL, msg->ops_AttrList);
	if(!databuf.dialog || !databuf.data)
		return 0;

	obj = (Object *)DoSuperNew(cl,obj,
		TAG_MORE, (ULONG)msg->ops_AttrList);
	if(obj)
	{
		data = (struct Dialog_Data*)INST_DATA(cl,obj);
		*data = databuf;
		d = (UBYTE*)data->data;

		tags = CreateDialog(&dialog, taglist, obj, &d);
		taglist[tags].ti_Tag = TAG_DONE;

		group = HGroup, TAG_MORE, &taglist);
		if(group)
		{
			if(DoMethod(obj, MUIM_Group_InitChange))
			{
				DoMethod(obj, OM_ADDMEMBER, group);
				DoMethod(obj, MUIM_Group_ExitChange);
			}
		}

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   open color pop up window
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG OpenColor(struct IClass *cl, Object *obj, struct MUIP_Object *msg)
{
	Object *newobj;

	// create a color window
	newobj = (Object*)NewObject(CL_Color->mcc_Class,NULL,
		MUIA_ChildWindow_Originator, obj,
		MUIA_Colorfield, msg->obj,
		TAG_DONE);
	if(newobj)
	{
		SetSleep(TRUE);

		DoMethod(app, OM_ADDMEMBER, newobj);
		SetAttrs(newobj, MUIA_Window_Open, TRUE, TAG_DONE);
	}
	return 0;
}

/*************
 * DESCRIPTION:   close color pop up window
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG CloseColor(struct IClass *cl, Object *obj, struct MUIP_ChildWindow_Finish* msg)
{
	Object *win, *object, *parent;
	COLOR *data;

	win = msg->win;
	SetAttrs(win, MUIA_Window_Open, FALSE, TAG_DONE);
	GetAttr(MUIA_Colorfield, win, (ULONG*)&object);
	GetAttr(MUIA_ObjectID, object, (ULONG*)&data);
	if(data)
	{
		GetColorField(object, data);
		GetAttr(MUIA_WindowObject, obj, (ULONG*)&parent);
		DoMethod(parent, MUIM_Parent_Update);
	}

	DoMethod(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);

	SetSleep(FALSE);

	return 0;
}

/*************
 * DESCRIPTION:   updates a slider value
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG SliderUpdate(struct IClass *cl, Object *obj, struct MUIP_Object *msg)
{
	Object *parent;
	ULONG *data;
	float f;

	GetAttr(MUIA_ObjectID, msg->obj, (ULONG*)&data);
	if(data)
	{
		GetAttr(MUIA_StringSlider_Value, msg->obj, (ULONG*)&f);
		*data = (ULONG)f;
		GetAttr(MUIA_WindowObject, obj, (ULONG*)&parent);
		DoMethod(parent, MUIM_Parent_Update);
	}

	return 0;
}

/*************
 * DESCRIPTION:   updates a float value
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG FloatUpdate(struct IClass *cl, Object *obj, struct MUIP_Object *msg)
{
	Object *parent;
	float *data;
	char *s;

	GetAttr(MUIA_ObjectID, msg->obj, (ULONG*)&data);
	if(data)
	{
		GetAttr(MUIA_String_Contents, msg->obj, (ULONG*)&s);
		*data = atof(s);
		GetAttr(MUIA_WindowObject, obj, (ULONG*)&parent);
		DoMethod(parent, MUIM_Parent_Update);
	}

	return 0;
}

/*************
 * DESCRIPTION:   updates an int value
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG IntUpdate(struct IClass *cl, Object *obj, struct MUIP_Object *msg)
{
	Object *parent;
	int *data;
	char *s;

	GetAttr(MUIA_ObjectID, msg->obj, (ULONG*)&data);
	if(data)
	{
		GetAttr(MUIA_String_Contents, msg->obj, (ULONG*)&s);
		*data = atoi(s);
		GetAttr(MUIA_WindowObject, obj, (ULONG*)&parent);
		DoMethod(parent, MUIM_Parent_Update);
	}

	return 0;
}

/*************
 * DESCRIPTION:   updates a check box value
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG CheckBoxUpdate(struct IClass *cl, Object *obj, struct MUIP_Object *msg)
{
	Object *parent;
	ULONG *data;

	GetAttr(MUIA_ObjectID, msg->obj, (ULONG*)&data);
	if(data)
	{
		GetAttr(MUIA_Selected, msg->obj, data);
		GetAttr(MUIA_WindowObject, obj, (ULONG*)&parent);
		DoMethod(parent, MUIM_Parent_Update);
	}

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:
 * OUTPUT:
 *************/
SAVEDS ASM ULONG Dialog_Dispatcher(REG(a0) struct IClass *cl, REG(a2) Object *obj, REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl, obj, (struct opSet*)msg));
		case MUIM_Dialog_OpenColor:
			return(OpenColor(cl, obj, (struct MUIP_Object*)msg));
		case MUIM_Color_Finish:
			return(CloseColor(cl,obj,(struct MUIP_ChildWindow_Finish*)msg));
		case MUIM_Dialog_SliderUpdate:
			return(SliderUpdate(cl, obj, (struct MUIP_Object*)msg));
		case MUIM_Dialog_FloatUpdate:
			return(FloatUpdate(cl, obj, (struct MUIP_Object*)msg));
		case MUIM_Dialog_CheckBoxUpdate:
			return(CheckBoxUpdate(cl, obj, (struct MUIP_Object*)msg));
		case MUIM_Dialog_IntUpdate:
			return(IntUpdate(cl, obj, (struct MUIP_Object*)msg));
	}
	return(DoSuperMethodA(cl,obj,msg));
}
