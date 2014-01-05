/***************
 * PROGRAM:       Modeler
 * NAME:          requview_class.cpp
 * DESCRIPTION:   this class is needed for requesters which show a preview
 *    of something (lensflares, light star)
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    05.11.96 ah    initial release
 *    17.02.97 ah    flares can now be selected with the mouse
 ***************/

#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>

extern struct GfxBase *GfxBase;
#include <pragma/graphics_lib.h>
#include <pragma/intuition_lib.h>
#include <pragma/utility_lib.h>
#include <cybergraphics/cybergraphics.h>
#include <proto/cybergraphics.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef REQUVIEW_CLASS_H
#include "requview_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#define BITMAP_DEPTH 2

// light position in flare display window
#define LIGHT_POSX .8
#define LIGHT_POSY .2

#define REQUVIEW_FLARES 1
#define REQUVIEW_STAR   2
#define REQUVIEW_ENABLE 4

/*************
 * DESCRIPTION:   new method
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG OM_New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct RequView_Data *data;

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg)))
		return(0);

	data = (struct RequView_Data*)INST_DATA(cl,obj);

	data->rp.BitMap = NULL;
	data->flares = NULL;
	data->star = NULL;
	data->width = 0;
	data->height = 0;
	data->flags = 0;

	return((ULONG)obj);
}

/*************
 * INPUT:         system
 * DESCRIPTION:   OM_SET method
 * OUTPUT:
 *************/
static ULONG mSet(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct RequView_Data *data = (struct RequView_Data *)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

	tags=msg->ops_AttrList;
	do
	{
		tag=NextTagItem(&tags);
		if(tag)
		{
			switch (tag->ti_Tag)
			{
				case MUIA_RequView_Display:
					data->display = (DISPLAY*)tag->ti_Data;
					break;
				case MUIA_RequView_Flares:
					data->flares = (FLARE*)tag->ti_Data;
					data->flags = REQUVIEW_FLARES | REQUVIEW_ENABLE;
					break;
				case MUIA_RequView_Star:
					data->star = (STAR*)tag->ti_Data;
					data->flags = REQUVIEW_STAR | REQUVIEW_ENABLE;
					break;
			}
		}
	}
	while(tag);

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   AskMinMax method will be called before the window is
 *    opened and before layout takes place. We need to tell MUI the
 *    minimum, maximum and default size of our object.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG AskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	struct Render_Data *data = (struct Render_Data*)INST_DATA(cl,obj);

	/* let our superclass first fill in what it thinks about sizes.
		this will e.g. add the size of frame and inner spacing. */
	DoSuperMethodA(cl,obj,(Msg)msg);

	/* now add the values specific to our object. note that we
		indeed need to *add* these values, not just set them! */
	msg->MinMaxInfo->MinWidth  += 50;
	msg->MinMaxInfo->DefWidth  += 100;
	msg->MinMaxInfo->MaxWidth  += 2048;

	msg->MinMaxInfo->MinHeight += 50;
	msg->MinMaxInfo->DefHeight += 100;
	msg->MinMaxInfo->MaxHeight += 2048;

	return(0);
}

/*************
 * DESCRIPTION:   Draw method is called whenever MUI feels we should render
 *    our object. This usually happens after layout is finished
 *    or when we need to refresh in a simplerefresh window.
 *    Note: You may only render within the rectangle
 *       _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj).
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct RequView_Data *data = (struct RequView_Data*)INST_DATA(cl,obj);
	SHORT oldtop, oldbottom, oldwidth, oldheight, oldleft, oldright;
	gfxOutput oldgfxoutput;
	RECTANGLE r;
	ULONG depth;

	DoSuperMethodA(cl,obj,(Msg)msg);

	if((_mwidth(obj) != data->width) || (_mheight(obj) != data->height))
	{
		if(GfxBase->LibNode.lib_Version<39)
		{
			if(data->rp.BitMap)
				NewFreeBitMap(data->rp.BitMap);
		}
		else
		{
			if(data->rp.BitMap)
				FreeBitMap(data->rp.BitMap);
		}
		data->rp.BitMap = NULL;
	}

	if(!data->rp.BitMap)
	{
		InitRastPort(&data->rp);

		data->width = _mwidth(obj);
		data->height = _mheight(obj);

		if(GfxBase->LibNode.lib_Version<39)
		{
			NewFreeBitMap(data->rp.BitMap);
			data->rp.BitMap = NewAllocBitMap(data->width, data->height, _screen(obj)->RastPort.BitMap->Depth);
		}
		else
		{
			FreeBitMap(data->rp.BitMap);
			if(CyberGfxBase)
			{
				if(GetCyberMapAttr(_rp(obj)->BitMap, CYBRMATTR_ISCYBERGFX))
				{
					depth = GetCyberMapAttr(_rp(obj)->BitMap, CYBRMATTR_DEPTH);
				}
			}
			else
			{
				depth = _screen(obj)->RastPort.BitMap->Depth;
			}
			// allocate first memory bitmap
			data->rp.BitMap = AllocBitMap(data->width, data->height,
				depth, BMF_MINPLANES,
				_screen(obj)->RastPort.BitMap);
		}
	}

	if ((msg->flags & MADF_DRAWUPDATE) || (msg->flags & MADF_DRAWOBJECT))
	{
		SetRast(&data->rp, 0);
		if(data->flags | REQUVIEW_ENABLE)
		{
			oldtop = data->display->view->top;
			oldbottom = data->display->view->bottom;
			oldwidth = data->display->view->width;
			oldheight = data->display->view->height;
			oldleft = data->display->view->left;
			oldright = data->display->view->right;

			data->display->view->top = 0;
			data->display->view->bottom = data->height-1;
			data->display->view->width = data->width;
			data->display->view->height = data->height;
			data->display->view->left = 0;
			data->display->view->right = data->width-1;

			oldgfxoutput = gfx.SetOutput(GFX_MEMORY1);
			gfx.SetRastPort(&data->rp);
		}

		if((data->flags | REQUVIEW_FLARES) && data->flares)
		{
			// draw flares in draw area
			r.left = 0;
			r.right = data->width-1;
			r.top = 0;
			r.bottom = data->height-1;
			data->flares->Draw(data->display, &r, (int)(data->width * LIGHT_POSX), (int)(data->height * LIGHT_POSY), TRUE);
		}

		if((data->flags | REQUVIEW_STAR) && data->star)
		{
			// draw light-burn in draw area
			gfx.SetPen(COLOR_SELECTED);
			data->star->Draw(data->display, data->width >> 1, data->height >> 1, data->width);
		}

		if(data->flags | REQUVIEW_ENABLE)
		{
			gfx.SetOutput(oldgfxoutput);

			data->display->view->top = oldtop;
			data->display->view->bottom = oldbottom;
			data->display->view->width = oldwidth;
			data->display->view->height = oldheight;
			data->display->view->left = oldleft;
			data->display->view->right = oldright;
		}

		BltBitMapRastPort(data->rp.BitMap,0,0,_rp(obj),_mleft(obj),_mtop(obj),data->width,data->height,0xc0);
	}
	else
	{
		if (msg->flags & MADF_DRAWOBJECT)
			BltBitMapRastPort(data->rp.BitMap,0,0,_rp(obj),_mleft(obj),_mtop(obj),data->width,data->height,0xc0);
	}

	return(0);
}

/*************
 * DESCRIPTION:   this function is called when the object should be destroyed
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG OM_Dispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct RequView_Data *data = (struct RequView_Data *)INST_DATA(cl,obj);

	if(GfxBase->LibNode.lib_Version<39)
	{
		if(data->rp.BitMap)
			NewFreeBitMap(data->rp.BitMap);
	}
	else
	{
		if(data->rp.BitMap)
			FreeBitMap(data->rp.BitMap);
	}

	return( DoSuperMethodA(cl,obj,msg) );
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG Setup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
	if (!(DoSuperMethodA(cl,obj,(Msg)msg)))
		return(FALSE);

	MUI_RequestIDCMP(obj,IDCMP_MOUSEBUTTONS);

	return TRUE;
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG Cleanup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
	MUI_RejectIDCMP(obj,IDCMP_MOUSEBUTTONS);

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   handle intuition input messages
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG HandleInput(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
	struct RequView_Data *data = (struct RequView_Data*)INST_DATA(cl,obj);
	Object *win;
	int num;

	if(msg->imsg || data->flares)
	{
		if(msg->imsg->Class == IDCMP_MOUSEBUTTONS)
		{
			if(msg->imsg->Code == SELECTUP)
			{
				if(msg->imsg->MouseX > _mleft(obj) &&
					msg->imsg->MouseX < _mright(obj) &&
					msg->imsg->MouseY > _mtop(obj) &&
					msg->imsg->MouseY < _mbottom(obj))
				{
					num = SelectedFlare(
						data->flares,
						msg->imsg->MouseX - _mleft(obj),
						msg->imsg->MouseY - _mtop(obj),
						data->width,
						data->height,
						(int)(data->width * LIGHT_POSX),
						(int)(data->height * LIGHT_POSY));
					GetAttr(MUIA_WindowObject, obj, (ULONG*)&win);
					SetAttrs(win, MUIA_Settings_FlareNumber, num, TAG_DONE);
					DoMethod(win, MUIM_Settings_FlareNumber);
				}
			}
		}
	}

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   Dispatcher for requester view class.
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG RequView_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(OM_New(cl,obj,(struct opSet*)msg));
		case OM_SET:
			return(mSet(cl,obj,(struct opSet*)msg));
		case OM_DISPOSE:
			return(OM_Dispose(cl,obj,msg));
		case MUIM_AskMinMax:
			return(AskMinMax(cl,obj,(struct MUIP_AskMinMax*)msg));
		case MUIM_Draw:
			return(Draw(cl,obj,(struct MUIP_Draw*)msg));
		case MUIM_HandleInput:
			return(HandleInput(cl,obj,(struct MUIP_HandleInput*)msg));
		case MUIM_Setup:
			return(Setup(cl,obj,(struct MUIP_HandleInput*)msg));
		case MUIM_Cleanup:
			return(Cleanup(cl,obj,(struct MUIP_HandleInput*)msg));
	}

	return(DoSuperMethodA(cl,obj,msg));
}
