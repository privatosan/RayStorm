/***************
 * PROGRAM:       Modeler
 * NAME:          materialitem_class.cpp
 * DESCRIPTION:   this class handles a material item of the material manager
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    04.04.97 ah    initial release
 *    25.10.97 ah    double click now opens properties dialog
 ***************/

#include <graphics/gfxbase.h>
#include <graphics/scale.h>
extern struct GfxBase *GfxBase;
#include <dos/dos.h>
#include <dos/dostags.h>
#include <exec/memory.h>

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

#ifndef MATERIALITEM_CLASS_H
#include "materialitem_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef RENDER_CLASS_H
#include "render_class.h"
#endif

#ifndef MSGHANDLER_H
#include "msghandler_class.h"
#endif

#ifndef SUBTASK_H
#include "subtask.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#define THUMBNAIL_WIDTH 16
#define THUMBNAIL_HEIGHT 16

/*************
 * DESCRIPTION:   new method
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct MaterialItem_Data *data,databuf;
	Object *preview;

	databuf.width = (ULONG)WARPOS_PPC_FUNC(GetTagData)(MUIA_Render_XRes, 64, msg->ops_AttrList);
	databuf.height = (ULONG)WARPOS_PPC_FUNC(GetTagData)(MUIA_Render_YRes, 64, msg->ops_AttrList);
	databuf.surf = (SURFACE*)WARPOS_PPC_FUNC(GetTagData)(MUIA_MaterialItem_Surface, NULL, msg->ops_AttrList);
	if(!databuf.surf)
		return 0;
	databuf.st = (struct SubTask*)WARPOS_PPC_FUNC(GetTagData)(MUIA_MaterialItem_SubTask, 0, msg->ops_AttrList);
	if(!databuf.st)
		return 0;

	// preview only for KS 3.0 and higher
	if(GfxBase->LibNode.lib_Version>=39)
	{
		preview = VGroup,
			Child, databuf.render = (Object*)NewObject(CL_Render->mcc_Class,NULL,
				MUIA_Background, MUII_BACKGROUND,
				MUIA_Render_XRes, databuf.width,
				MUIA_Render_YRes, databuf.height,
			End,
			Child, databuf.text = StringObject,
				StringFrame,
				MUIA_Font, MUIV_Font_Tiny,
				MUIA_String_Contents, databuf.surf->name,
			End,
		End;
	}
	else
	{
		preview = VGroup,
			Child, databuf.text = StringObject,
				StringFrame,
				MUIA_Font, MUIV_Font_Tiny,
				MUIA_String_Contents, databuf.surf->name,
			End,
		End;
	}

	obj = (Object *)DoSuperNew(cl,obj,
		MUIA_Draggable, TRUE,
		MUIA_InputMode, MUIV_InputMode_RelVerify,
		Child, preview,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct MaterialItem_Data*)INST_DATA(cl,obj);
		*data = databuf;

		data->thumbnail = NULL;
		data->thumbnailbitmap = NULL;
		data->seconds = 0;
		data->micros = 0;

		DOMETHOD(obj, MUIM_Notify, MUIA_Pressed, TRUE, obj, 1, MUIM_MaterialItem_Pressed);

		DOMETHOD(data->text, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
			obj, 1, MUIM_MaterialItem_Name);

		return((ULONG)obj);
	}
	return 0;
}

/*************
 * DESCRIPTION:   OM_SET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG Set(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct MaterialItem_Data *data = (struct MaterialItem_Data *)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

	tags=msg->ops_AttrList;
	do
	{
		tag=NextTagItem(&tags);
		if(tag)
		{
			switch (tag->ti_Tag)
			{
				case MUIA_MaterialItem_Surface:
					data->surf = (SURFACE*)tag->ti_Data;
					break;
			}
		}
	}
	while(tag);

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   OM_GET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG Get(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct MaterialItem_Data *data = (struct MaterialItem_Data *)INST_DATA(cl,obj);

	switch (msg->opg_AttrID)
	{
		case MUIA_MaterialItem_Surface:
			*(SURFACE**)(msg->opg_Storage) = data->surf;
			return TRUE;
	}

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   recalculate preview
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG Recalc(struct IClass *cl,Object *obj, Msg msg)
{
	MaterialItem_Data *data = (MaterialItem_Data *)INST_DATA(cl,obj);

	SetAttrs(data->text, MUIA_Text_Contents, data->surf->name, TAG_DONE);

	// preview only for KS 3.0 and higher
	if((GfxBase->LibNode.lib_Version>=39) && data->st)
	{
		data->previewdata.object = NULL;
		data->previewdata.surf = data->surf;
		data->previewdata.renderarea = data->render;
		data->previewdata.caller = obj;
		data->previewdata.width = data->width;
		data->previewdata.height = data->height;
		data->previewdata.msghandler = msghandler;
		data->previewdata.preview = 0;
		// add render area and this object to message handler, with that the
		// messages the subtask sends are handled.
		DOMETHOD(msghandler, MUIM_MsgHandler_AddObject, data->render);
		DOMETHOD(msghandler, MUIM_MsgHandler_AddObject, obj);
		SendSubTaskMsg(data->st,STC_START, &data->previewdata);
	}

	return 0;
}

/*************
 * DESCRIPTION:   Set name of surface
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG Name(struct IClass *cl,Object *obj, Msg msg)
{
	MaterialItem_Data *data = (MaterialItem_Data *)INST_DATA(cl,obj);
	char *name;

	GetAttr(MUIA_String_Contents, data->text, (ULONG*)&name);
	if(name)
		data->surf->SetName(name);

	return 0;
}

/*************
 * DESCRIPTION:   dispose object
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG Dispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct MaterialItem_Data *data = (struct MaterialItem_Data*)INST_DATA(cl,obj);

	// Remove renderarea and this object from the list of vaild objects. Messages
	// in the application message buffer are ignored for this objects.
	DOMETHOD(msghandler, MUIM_MsgHandler_RemoveObject, data->render);
	DOMETHOD(msghandler, MUIM_MsgHandler_RemoveObject, obj);

	if(data->thumbnail)
		MUI_DisposeObject(data->thumbnail);
	if(data->thumbnailbitmap)
		FreeBitMap(data->thumbnailbitmap);

	return 0;
}

/*************
 * DESCRIPTION:   resume subtask rendering if object is shown
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG ResumeTask(struct IClass *cl,Object *obj,Msg msg)
{
	MaterialItem_Data *data = (MaterialItem_Data *)INST_DATA(cl,obj);

	if(data->st)
		SendSubTaskMsg(data->st, STC_RESUME, NULL);
	return TRUE;
}

/*************
 * DESCRIPTION:   stop subtask rendering if object is hidden
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG StopTask(struct IClass *cl,Object *obj,Msg msg)
{
	MaterialItem_Data *data = (MaterialItem_Data *)INST_DATA(cl,obj);

	if(data->st)
		SendSubTaskMsg(data->st, STC_STOP, NULL);
	return TRUE;
}

/*************
 * DESCRIPTION:   subtask is finished with rendering, remove it
 * INPUT:         system
 * OUTPUT:        none
 *************/
static ULONG SubTaskFinished(struct IClass *cl,Object *obj,Msg msg)
{
	struct MaterialItem_Data *data = (struct MaterialItem_Data*)INST_DATA(cl,obj);
	Object *parent;

	if(data->st)
		SendSubTaskMsg(data->st, STC_CLEANUP, NULL);
	// Remove renderarea and this object from the list of vaild objects. Messages
	// in the application message buffer are ignored for this objects.
	DOMETHOD(msghandler, MUIM_MsgHandler_RemoveObject, data->render);
	DOMETHOD(msghandler, MUIM_MsgHandler_RemoveObject, obj);
	if(data->thumbnail)
	{
		MUI_DisposeObject(data->thumbnail);
		data->thumbnail = NULL;
	}
	if(data->thumbnailbitmap)
	{
		FreeBitMap(data->thumbnailbitmap);
		data->thumbnailbitmap = NULL;
	}
	sciUpdateThumbNail(data->surf);

	GetAttr(MUIA_Parent, obj, (ULONG*)&parent);
	DOMETHOD(parent, MUIM_Material_SubTaskFinished);

	return 0;
}

/*************
 * DESCRIPTION:   get a thumbnail image of the surface
 * INPUT:         system
 * OUTPUT:        bitmap object
 *************/
static ULONG GetThumbNail(struct IClass *cl,Object *obj,Msg msg)
{
	struct MaterialItem_Data *data = (struct MaterialItem_Data*)INST_DATA(cl,obj);
	struct BitMap *source;
	struct BitScaleArgs args;

	if(!data->thumbnail)
	{
		if(GfxBase->LibNode.lib_Version>=39)
		{
			GetAttr(MUIA_Render_BitMap, data->render, (ULONG*)&source);
			if(!source)
				return NULL;
			data->thumbnailbitmap = AllocBitMap(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, source->Depth, BMF_MINPLANES, source);
			if(!data->thumbnailbitmap)
				return NULL;

			args.bsa_SrcX = 0;
			args.bsa_SrcY = 0;
			args.bsa_SrcWidth = PREVIEW_WIDTH;
			args.bsa_SrcHeight = PREVIEW_HEIGHT;
			args.bsa_DestX = 0;
			args.bsa_DestY = 0;
			args.bsa_DestWidth = THUMBNAIL_WIDTH;
			args.bsa_DestHeight = THUMBNAIL_HEIGHT;
			args.bsa_XSrcFactor = PREVIEW_WIDTH/THUMBNAIL_WIDTH;
			args.bsa_XDestFactor = 1;
			args.bsa_YSrcFactor = PREVIEW_HEIGHT/THUMBNAIL_HEIGHT;
			args.bsa_YDestFactor = 1;
			args.bsa_SrcBitMap = source;
			args.bsa_DestBitMap = data->thumbnailbitmap;
			args.bsa_Flags = 0;
			BitMapScale(&args);
			WaitBlit();

			data->thumbnail =
				BitmapObject,
					MUIA_Bitmap_Bitmap, data->thumbnailbitmap,
					MUIA_Bitmap_Width, THUMBNAIL_WIDTH,
					MUIA_Bitmap_Height, THUMBNAIL_HEIGHT,
					MUIA_FixWidth, THUMBNAIL_WIDTH,
					MUIA_FixHeight, THUMBNAIL_HEIGHT,
				End;
		}
	}
	return (ULONG)(data->thumbnail);
}

/*************
 * DESCRIPTION:   called when user presses left mouse button. When douleclicking
 *    open material requester.
 * INPUT:         system
 * OUTPUT:        bitmap object
 *************/
static ULONG Pressed(struct IClass *cl,Object *obj,Msg msg)
{
	struct MaterialItem_Data *data = (struct MaterialItem_Data*)INST_DATA(cl,obj);
	ULONG seconds, micros;

	CurrentTime(&seconds, &micros);
	if(DoubleClick(data->seconds, data->micros, seconds, micros))
		sciOpenMaterialRequ(data->surf);
	data->seconds = seconds;
	data->micros = micros;

	return 0;
}

/*************
 * DESCRIPTION:   Dispatcher for material item class
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(MaterialItem_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl, obj, (struct opSet*)msg));
		case OM_SET:
			return(Set(cl,obj,(struct opSet*)msg));
		case OM_GET:
			return(Get(cl,obj,(struct opGet*)msg));
		case OM_DISPOSE:
			return(Dispose(cl,obj,msg));
		case MUIM_Hide:
			StopTask(cl, obj, msg);
			break;
		case MUIM_Show:
			ResumeTask(cl, obj, msg);
			break;
		case MUIM_MaterialItem_Recalc:
			return(Recalc(cl, obj, msg));
		case MUIM_MaterialItem_Name:
			return(Name(cl, obj, msg));
		case MUIM_SubTask_Finished:
			return(SubTaskFinished(cl, obj, msg));
		case MUIM_MaterialItem_GetThumbNail:
			return(GetThumbNail(cl, obj, msg));
		case MUIM_MaterialItem_StopTask:
			return StopTask(cl, obj, msg);
		case MUIM_MaterialItem_Pressed:
			return(Pressed(cl, obj, msg));
	}

	return(DOSUPERMETHODA(cl,obj,msg));
}
