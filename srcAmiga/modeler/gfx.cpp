/***************
 * PROGRAM:       Modeler
 * NAME:          gfx.cpp
 * DESCRIPTION:   graphic commands, AMIGA implementation
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.11.97 ah    initial release
 ***************/

#include <string.h>
#include <stdio.h>

#include <graphics/GfxBase.h>
#include <graphics/gfxmacros.h>
#include <pragma/graphics_lib.h>
#include <pragma/layers_lib.h>
#include <cybergraphics/cybergraphics.h>
#include <proto/cybergraphics.h>
extern struct GfxBase *GfxBase;

#ifndef MUIUTILITY_H
#include "MUIUtility.h"
#endif

#ifndef GFX_H
#include "gfx.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifdef MURX
#define ABS(a)    (((a)<0) ? -(a) : (a))
#define SGN(a)    (((a)<0) ? -1 : 0)

void Line(GFX *gfx, int x1, int y1, int x2, int y2)
{
	int d, x, y, ax, ay, sx, sy, dx, dy;

	dx = x2-x1;
	ax = ABS(dx)<<1;
	sx = SGN(dx);

	dy = y2-y1;
	ay = ABS(dy)<<1;
	sy = SGN(dy);

	x = x1;
	y = y1;
	if (ax>ay)
	{
		/* x dominant */
		d = ay-(ax>>1);
		for (;;)
		{
			(*dotproc)(x, y);
			if (x == x2)
				return;
			if (d >= 0)
			{
				y += sy;
				d -= ax;
			}
			x += sx;
			d += ay;
		}
	}
	else
	{
		/* y dominant */
		d = ax-(ay>>1);
		for (;;)
		{
			(*dotproc)(x, y);
			if (y == y2)
				return;
			if (d >= 0)
			{
				x += sx;
				d -= ay;
			}
			y += sy;
			d += ax;
		}
	}
}
#endif // MURX

/*************
 * DESCRIPTION:   Initialize graphics abstraction layer
 * INPUT:         window to render into
 * OUTPUT:        none
 *************/
void GFX::Init(Object *win, DISPLAY *disp)
{
	LONG depth = -1;

	penlist = NULL;

	display = disp;

	directrport = _rp(win);
	left = _mleft(win);
	top = _mtop(win);

	InitRastPort(&memrport1);
	InitRastPort(&memrport2);

	if(GfxBase->LibNode.lib_Version<39)
	{
		// allocate first memory bitmap
		memrport1.BitMap = NewAllocBitMap(_mwidth(win),_mheight(win), _screen(win)->RastPort.BitMap->Depth);
		if(!memrport1.BitMap)
			return;
		// allocate second memory bitmap
		memrport2.BitMap = NewAllocBitMap(_mwidth(win),_mheight(win), _screen(win)->RastPort.BitMap->Depth);
		if(!memrport2.BitMap)
			return;
	}
	else
	{
		if(CyberGfxBase)
		{
			if(GetCyberMapAttr(_rp(win)->BitMap, CYBRMATTR_ISCYBERGFX))
				depth = GetCyberMapAttr(_rp(win)->BitMap, CYBRMATTR_DEPTH);
		}
		if(depth == -1)
			depth = _screen(win)->RastPort.BitMap->Depth;

		// allocate first memory bitmap
		memrport1.BitMap = AllocBitMap(_mwidth(win), _mheight(win),
			depth, BMF_MINPLANES,
			_screen(win)->RastPort.BitMap);
		if(!memrport1.BitMap)
			return;
		// allocate second memory bitmap
		memrport2.BitMap = AllocBitMap(_mwidth(win), _mheight(win),
			depth, BMF_MINPLANES,
			_screen(win)->RastPort.BitMap);
		if(!memrport2.BitMap)
			return;
	}

	cmap = _screen(win)->ViewPort.ColorMap;
	AllocPens();
}

/*************
 * DESCRIPTION:   Allocated pens
 * INPUT:         none
 * OUTPUT:        none
 *************/
void GFX::AllocPens()
{
	int i;
	COLOR color;

	for(i=0; i<COLOR_MAX; i++)
	{
		GetColorValue(i, &color);
		pens[i].pen = ObtainBestPen(cmap,
			(ULONG)(color.r*255.f) << 24,
			(ULONG)(color.g*255.f) << 24,
			(ULONG)(color.b*255.f) << 24,
			TAG_DONE);

		if(pens[i].pen == 0xFFFFFFFF)
		{
			pens[i].pen = FindColor(cmap,
				(ULONG)(color.r*255.f) << 24,
				(ULONG)(color.g*255.f) << 24,
				(ULONG)(color.b*255.f) << 24, -1);

			pens[i].allocated = FALSE;
		}
		else
			pens[i].allocated = TRUE;
	}
}

/*************
 * DESCRIPTION:   Free allocated pens
 * INPUT:         none
 * OUTPUT:        none
 *************/
void GFX::FreePens()
{
	PENS *next;
	int i;

	for(i=0; i<COLOR_MAX; i++)
	{
		if(pens[i].allocated)
			ReleasePen(cmap, pens[i].pen);
	}

	while(penlist)
	{
		next = (PENS*)penlist->GetNext();
		if(penlist->allocated)
			ReleasePen(cmap, penlist->pen);
		delete penlist;
		penlist = next;
	}
};

/*************
 * DESCRIPTION:   Cleanup graphics abstraction layer
 * INPUT:         none
 * OUTPUT:        none
 *************/
void GFX::Cleanup()
{
	FreePens();

	if(GfxBase->LibNode.lib_Version<39)
	{
		if(memrport1.BitMap)
		{
			NewFreeBitMap(memrport1.BitMap);
			memrport1.BitMap = NULL;
		}
		if(memrport2.BitMap)
		{
			NewFreeBitMap(memrport2.BitMap);
			memrport2.BitMap = NULL;
		}
	}
	else
	{
		if(memrport1.BitMap)
		{
			FreeBitMap(memrport1.BitMap);
			memrport1.BitMap = NULL;
		}
		if(memrport2.BitMap)
		{
			FreeBitMap(memrport2.BitMap);
			memrport2.BitMap = NULL;
		}
	}
}

/*************
 * DESCRIPTION:   Set the rastport to render into
 * INPUT:         newport
 * OUTPUT:        none
 *************/
void GFX::SetRastPort(struct RastPort *newrport)
{
	rport = newrport;
}

/*************
 * DESCRIPTION:   Draw a line
 * INPUT:         x1,y1 x2,y2
 * OUTPUT:        none
 *************/
void GFX::Line(int x1, int y1, int x2, int y2)
{
	Move(rport, (SHORT)x1, (SHORT)y1);
	Draw(rport, (SHORT)x2, (SHORT)y2);
}

/*************
 * DESCRIPTION:   Draw a clipped line
 * INPUT:         x,y x1,y1
 * OUTPUT:        none
 *************/
void GFX::ClipLine(int x, int y, int x1, int y1)
{
	if(x < clipleft)
	{
		if(x1 < clipleft)
			return;
		y -= ((y1-y) * (x - clipleft)) / (x1-x);
		x = clipleft;
	}
	else
	{
		if(x1 < clipleft)
		{
			y1 = y - ((y1-y) * (x - clipleft)) / (x1-x);
			x1 = clipleft;
		}
	}

	if(x > clipright)
	{
		if(x1 > clipright)
			return;
		y -= ((y1-y) * (x - clipright)) / (x1-x);
		x = clipright;
	}
	else
	{
		if(x1 > clipright)
		{
			y1 = y - ((y1-y) * (x - clipright)) / (x1-x);
			x1 = clipright;
		}
	}

	if(y < cliptop)
	{
		if(y1 < cliptop)
			return;
		x -= ((x1-x) * (y - cliptop)) / (y1-y);
		y = cliptop;
	}
	else
	{
		if(y1 < cliptop)
		{
			x1 = x - (x1-x) * (y - cliptop) / (y1-y);
			y1 = cliptop;
		}
	}

	if(y > clipbottom)
	{
		if(y1 > clipbottom)
			return;
		x -= ((x1-x) * (y - clipbottom)) / (y1-y);
		y = clipbottom;
	}
	else
	{
		if(y1 > clipbottom)
		{
			x1 = x - (x1-x) * (y - clipbottom) / (y1-y);
			y1 = clipbottom;
		}
	}

	if(output == GFX_SCREEN)
	{
		x += left;
		y += top;
		x1 += left;
		y1 += top;
	}
	Move(rport, (SHORT)x, (SHORT)y);
	Draw(rport, (SHORT)x1, (SHORT)y1);

}

/*************
 * DESCRIPTION:   Set the draw pen
 * INPUT:         pen
 * OUTPUT:        none
 *************/
void GFX::SetPen(int pen)
{
	SetAPen(rport, pens[pen].pen);
}

/*************
 * DESCRIPTION:   Set the color and draw pen
 * INPUT:         r,g,b    color
 * OUTPUT:        none
 *************/
void GFX::SetColor(float r, float g, float b)
{
	PENS *cur;

	// search in the list of allocated pens for a pen with the same color
	cur = penlist;
	while(cur)
	{
		if((cur->color.r == r) && (cur->color.g == g) && (cur->color.b == b))
		{
			// found a pen
			SetAPen(rport, cur->pen);
			return;
		}
		cur = (PENS*)cur->GetNext();
	}

	// no pen with this color allocted, get a new one
	cur = new PENS;
	if(!cur)
		return;
	cur->Insert((SLIST**)&penlist);

	cur->pen = ObtainBestPen(cmap,
		(ULONG)(r*255.f) << 24,
		(ULONG)(g*255.f) << 24,
		(ULONG)(b*255.f) << 24,
		TAG_DONE);
	if(cur->pen == 0xFFFFFFFF)
	{
		cur->pen = FindColor(cmap,
			(ULONG)(r*255.f) << 24,
			(ULONG)(g*255.f) << 24,
			(ULONG)(b*255.f) << 24, -1);

		cur->allocated = FALSE;
	}
	else
		cur->allocated = TRUE;
	cur->color.r = r;
	cur->color.g = g;
	cur->color.b = b;

	SetAPen(rport, cur->pen);
}

/*************
 * DESCRIPTION:   Set the write mode
 * INPUT:         newwritemode
 * OUTPUT:        oldwrtiemode
 *************/
gfxMode GFX::SetWriteMode(gfxMode newwritemode)
{
	gfxMode oldwritemode;

	oldwritemode = writemode;
	writemode = newwritemode;

	switch(writemode)
	{
		case GFX_NORMAL:
		case GFX_TRANSPARENT:
			SetDrMd(rport, JAM1);
			SetDrPt(rport,0xffff);
			break;
		case GFX_COMPLEMENT:
			SetDrMd(rport, COMPLEMENT);
			SetDrPt(rport,0xffff);
			break;
		case GFX_DOTTED:
			SetDrMd(rport, JAM1);
			SetDrPt(rport,0x8888);
			break;
		case GFX_COMPLEMENT_DOTTED:
			SetDrMd(rport, COMPLEMENT);
			SetDrPt(rport,0x8888);
			break;
	}
	return oldwritemode;
}

/*************
 * DESCRIPTION:   Set the output
 * INPUT:         newoutput
 * OUTPUT:        oldoutput
 *************/
gfxOutput GFX::SetOutput(gfxOutput newoutput)
{
	gfxOutput oldoutput;

	oldoutput = output;
	output = newoutput;

	rport = GetRastPortFromMode(output);

	return oldoutput;
}

/*************
 * DESCRIPTION:   Set the clipping rectangle
 * INPUT:         left,top, right,bottom
 * OUTPUT:        none
 *************/
void GFX::SetClipping(int left, int top, int right, int bottom)
{
/*   struct Rectangle rect;
	struct Region *region;

	if(output != GFX_SCREEN)
	{
		switch(output)
		{
			case GFX_MEMORY1:
				region = memregion1;
				break;
			case GFX_MEMORY2:
				region = memregion2;
				break;
		}

		// remove all old clipping rectangles
		ClearRegion(region);

		// install new clipping rectangle
		rect.MinX = left;
		rect.MinY = top;
		rect.MaxX = left + width;
		rect.MaxY = top + height;
		OrRectRegion(region, &rect);
	}*/

	clipleft = left;
	cliptop = top;
	clipright = right;
	clipbottom = bottom;
}

/*************
 * DESCRIPTION:   Draw a rectangle
 * INPUT:         left,top, right,bottom
 * OUTPUT:        none
 *************/
void GFX::Rectangle(int left, int top, int right, int bottom)
{
	if(left < clipleft)
		left = clipleft;
	if(right > clipright)
		right = clipright;
	if(right < left)
		return;

	if(top < cliptop)
		top = cliptop;
	if(bottom > clipbottom)
		bottom = clipbottom;
	if(top > bottom)
		return;

	if(output == GFX_SCREEN)
	{
		left += this->left;
		top += this->top;
		right += this->left;
		bottom += this->top;
	}

	Move(rport, (SHORT)left, (SHORT)top);
	Draw(rport, (SHORT)right, (SHORT)top);
	Draw(rport, (SHORT)right, (SHORT)bottom);
	Draw(rport, (SHORT)left, (SHORT)bottom);
	Draw(rport, (SHORT)left, (SHORT)top);
}

/*************
 * DESCRIPTION:   Draw a filled rectangle
 * INPUT:         left,top, right,bottom
 * OUTPUT:        none
 *************/
void GFX::FillRect(int left, int top, int right, int bottom)
{
	if(left < clipleft)
		left = clipleft;
	if(right > clipright)
		right = clipright;
	if(right < left)
		return;

	if(top < cliptop)
		top = cliptop;
	if(bottom > clipbottom)
		bottom = clipbottom;
	if(top > bottom)
		return;

	if(output == GFX_SCREEN)
	{
		left += this->left;
		top += this->top;
		right += this->left;
		bottom += this->top;
	}

	RectFill(rport, left, top, right, bottom);
}

/*************
 * DESCRIPTION:
 * INPUT:         none
 * OUTPUT:        none
 *************/
void GFX::BitBlt(gfxOutput from, int src_left, int src_top, int width, int height, gfxOutput to, int dst_left, int dst_top)
{
	if(to == GFX_SCREEN)
	{
		dst_left += left;
		dst_top += top;
	}
	if(from == GFX_SCREEN)
	{
		src_left += left;
		src_top += top;
	}
	ClipBlit(GetRastPortFromMode(from), src_left, src_top, GetRastPortFromMode(to), dst_left, dst_top, width, height, 0xc0);
//   BltBitMap(GetRastPortFromMode(from)->BitMap, src_left, src_top, GetRastPortFromMode(to)->BitMap, dst_left, dst_top, width, height, 0xc0, 0xff, NULL);
}

/*************
 * DESCRIPTION:
 * INPUT:         none
 * OUTPUT:        none
 *************/
void GFX::TextXY(int x, int y, char *s, int len)
{
	if(x < clipleft)
		return;
	if(y > clipbottom)
		return;
	if((y - GetTextExtentY()) < cliptop)
		return;
	if(x + TextLength(rport, s, len != -1 ? len : strlen(s)) > clipright)
		return;

	if(output == GFX_SCREEN)
	{
		x += left;
		y += top;
	}

	Move(rport, x, y);
	Text(rport, s, len != -1 ? len : strlen(s));
}

/*************
 * DESCRIPTION:
 * INPUT:         none
 * OUTPUT:        none
 *************/
int GFX::GetTextExtentX(char *s)
{
	return TextLength(rport, s, strlen(s));
}

/*************
 * DESCRIPTION:
 * INPUT:         none
 * OUTPUT:        none
 *************/
int GFX::GetTextExtentY()
{
	return rport->Font->tf_YSize;
}

/*************
 * DESCRIPTION:
 * INPUT:         none
 * OUTPUT:        none
 *************/
struct RastPort *GFX::GetRastPortFromMode(gfxOutput output)
{
	switch(output)
	{
		case GFX_SCREEN:
			return directrport;
			break;
		case GFX_MEMORY1:
			return &memrport1;
			break;
		case GFX_MEMORY2:
			return &memrport2;
			break;
	}
	ASSERT(FALSE);
	return NULL;
}
