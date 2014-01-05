/***************
 * PROGRAM:       Modeler
 * NAME:          render_class.cpp
 * DESCRIPTION:   render class is used to display 24bit images in a mui area
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.11.96 ah    initial release
 ***************/

#include <string.h>

#include <graphics/gfx.h>
#include <intuition/intuition.h>

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

#ifndef RENDER_CLASS_H
#include "render_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#define COLORS 4096

/*************
 * DESCRIPTION:   new method
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Render_Data *data;

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg)))
		return(0);

	data = (struct Render_Data*)INST_DATA(cl,obj);

	data->colors = NULL;
	data->rp.BitMap = NULL;
	data->xres = (int)GetTagData(MUIA_Render_XRes,0,msg->ops_AttrList);
	data->yres = (int)GetTagData(MUIA_Render_YRes,0,msg->ops_AttrList);
	data->cmap = NULL;

	return((ULONG)obj);
}

/*************
 * DESCRIPTION:   OM_GET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG Get(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct Render_Data *data = (struct Render_Data *)INST_DATA(cl,obj);

	switch (msg->opg_AttrID)
	{
		case MUIA_Render_BitMap:
			*(struct BitMap**)(msg->opg_Storage) = data->rp.BitMap;
			return TRUE;
	}

	return(DoSuperMethodA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   OM_SET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG Set(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Render_Data *data = (struct Render_Data *)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

	tags=msg->ops_AttrList;
	do
	{
		tag=NextTagItem(&tags);
		if(tag)
		{
			switch (tag->ti_Tag)
			{
				case MUIA_Render_Data:
					data->data = (rsiSMALL_COLOR*)tag->ti_Data;
					break;
				case MUIA_Render_Lines:
					data->lines = (int)tag->ti_Data;
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
	msg->MinMaxInfo->MinWidth  += data->xres;
	msg->MinMaxInfo->DefWidth  += data->xres;
	msg->MinMaxInfo->MaxWidth  += data->xres;

	msg->MinMaxInfo->MinHeight += data->yres;
	msg->MinMaxInfo->DefHeight += data->yres;
	msg->MinMaxInfo->MaxHeight += data->yres;

	return(0);
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG DrawUpdate(struct IClass *cl,Object *obj, struct MUIP_Render_DrawUpdate *msg)
{
	struct Render_Data *data = (struct Render_Data*)INST_DATA(cl,obj);
	int x, y;
	ULONG pen;
	struct Screen *screen;
	rsiSMALL_COLOR *d;
	LONG depth;
	BOOL done = FALSE;

	data->line = msg->line;

	if(CyberGfxBase)
	{
		if(GetCyberMapAttr(_rp(obj)->BitMap, CYBRMATTR_ISCYBERGFX))
		{
			depth = GetCyberMapAttr(_rp(obj)->BitMap, CYBRMATTR_DEPTH);
			if(depth > 8)
			{
				// high or TRUE color screen
				if(!data->rp.BitMap)
				{
					InitRastPort(&data->rp);

					data->rp.BitMap = AllocBitMap(
						_mwidth(obj),
						_mheight(obj),
						depth,
						BMF_DISPLAYABLE|BMF_MINPLANES,
						_screen(obj)->RastPort.BitMap);
					if(!data->rp.BitMap)
						return 0;
					SetRast(&data->rp,1);
				}
				WritePixelArray(
					data->data,
					0, 0,
					data->xres*sizeof(rsiSMALL_COLOR),
					&data->rp,
					0, data->line,
					data->xres, data->lines,
					RECTFMT_RGBA);
				done = TRUE;
			}
		}
	}
	if(!done)
	{
		if(!data->colors)
		{
			data->colors = new ULONG[COLORS];
			if(!data->colors)
				return 0;
			memset(data->colors, 0xFF, COLORS*sizeof(ULONG));
		}

		if(!data->rp.BitMap)
		{
			InitRastPort(&data->rp);

			data->rp.BitMap = AllocBitMap(
				_mwidth(obj),
				_mheight(obj),
				_rp(obj)->BitMap->Depth,
				BMF_CLEAR|BMF_DISPLAYABLE|BMF_MINPLANES,
				_screen(obj)->RastPort.BitMap);
			if(!data->rp.BitMap)
				return 0;
			SetRast(&data->rp,1);
		}

		if(!data->cmap)
		{
			GetAttr(MUIA_Window_Screen,_win(obj),(ULONG*)&screen);
			data->cmap = screen->ViewPort.ColorMap;
		}

		d = data->data;
		for(y=data->line; y<data->line+data->lines; y++)
		{
			for(x=0; x<data->xres; x++)
			{
				pen = ((((ULONG)d->r) << 4) & 0xF00) +
						((((ULONG)d->g)     ) & 0x0F0) +
						((((ULONG)d->b) >> 4));
				if(data->colors[pen] == 0xFFFFFFFF)
				{
					data->colors[pen] = ObtainBestPen(data->cmap,
						(ULONG)(d->r) << 24,
						(ULONG)(d->g) << 24,
						(ULONG)(d->b) << 24, TAG_DONE);

					if(data->colors[pen] == 0xFFFFFFFF)
					{
						pen = FindColor(data->cmap,
							(ULONG)(d->r) << 24,
							(ULONG)(d->g) << 24,
							(ULONG)(d->b) << 24, -1);
						SetAPen(&data->rp, pen);
					}
					else
					{
						SetAPen(&data->rp, data->colors[pen]);
					}
				}
				else
					SetAPen(&data->rp, data->colors[pen]);
				WritePixel(&data->rp, x, y);
				d++;
			}
		}
	}

	MUI_Redraw(obj, MADF_DRAWUPDATE);

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
	struct Render_Data *data = (struct Render_Data*)INST_DATA(cl,obj);
	struct RastPort *rp;

	DoSuperMethodA(cl,obj,(Msg)msg);

	if(data->rp.BitMap && _rp(obj))
	{
		rp = _rp(obj);

		if(msg->flags & MADF_DRAWUPDATE)
			BltBitMapRastPort(data->rp.BitMap,0,data->line,_rp(obj),_mleft(obj),_mtop(obj)+data->line,_mwidth(obj),data->lines,0xc0);
		else
		{
			if(msg->flags & MADF_DRAWOBJECT)
				BltBitMapRastPort(data->rp.BitMap,0,0,_rp(obj),_mleft(obj),_mtop(obj),_mwidth(obj),_mheight(obj),0xc0);
		}
	}

	return(0);
}

/*************
 * DESCRIPTION:   this function is called when the object should be destroyed
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG Cleanup(struct IClass *cl,Object *obj,Msg msg)
{
	struct Render_Data *data = (struct Render_Data *)INST_DATA(cl,obj);
	int i;

	if(data->colors)
	{
		for(i=0; i<COLORS; i++)
		{
			if(data->colors[i] != 0xFFFFFFFF)
				ReleasePen(data->cmap, data->colors[i]);
		}
		delete [ ] data->colors;
	}
	if(data->rp.BitMap)
		FreeBitMap(data->rp.BitMap);

	return TRUE;
}

/*************
 * DESCRIPTION:   this function must be called before each new render process
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG ResetColors(struct IClass *cl,Object *obj,Msg msg)
{
	struct Render_Data *data = (struct Render_Data*)INST_DATA(cl,obj);
	int i;

	if(data->colors)
	{
		if(data->cmap)
		{
			for(i=0; i<COLORS; i++)
			{
				if(data->colors[i] != 0xFFFFFFFF)
					ReleasePen(data->cmap, data->colors[i]);
			}
		}
		memset(data->colors, 0xFF, COLORS*4);
	}
	return 0;
}

/*************
 * DESCRIPTION:   Dispatcher for render class.
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG Render_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet*)msg));
		case OM_GET:
			return(Get(cl,obj,(struct opGet*)msg));
		case OM_SET:
			return(Set(cl,obj,(struct opSet*)msg));
		case MUIM_Cleanup:
			return(Cleanup(cl,obj,msg));
		case MUIM_AskMinMax:
			return(AskMinMax(cl,obj,(struct MUIP_AskMinMax*)msg));
		case MUIM_Draw:
			return(Draw(cl,obj,(struct MUIP_Draw*)msg));
		case MUIM_Render_ResetColors:
			return(ResetColors(cl,obj,msg));
		case MUIM_Render_DrawUpdate:
			return(DrawUpdate(cl,obj,(struct MUIP_Render_DrawUpdate*)msg));
	}

	return(DoSuperMethodA(cl,obj,msg));
}
