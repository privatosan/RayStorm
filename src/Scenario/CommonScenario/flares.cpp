/***************
 * MODULE:        flares.cpp
 * PROJECT:       Scenario
 * DESCRIPTION:   lens flare functions
 * AUTHORS:       Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.10.96 mh    initial release
 *    13.11.96 mh    Duplicate
 *    18.11.96 ah    added Read() and ParseFlare()
 *    09.02.97 mh    added SelectedFlare
 ***************/

#ifndef FLARES_H
#include "flares.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef GFX_H
#include "gfx.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

/*************
 * DESCRIPTION:   constructor
 * INPUT:         -
 * OUTPUT:        -
 *************/
FLARE::FLARE()
{
	type = FLARE_DISC;
	func = 0;
	pos = 0.5f;
	radius = 0.1f;
	edges = 6;
	tilt = 0.f;
	color.r = color.g = color.b = 0.2f;
	selected = FALSE;
}

/*************
 * DESCRIPTION:   returns the index of the flare which is the closest to a given point
 * INPUT:         flareroot      list of flares
 *                mx,my          position of mouse click
 *                width,height   width and height of draw area
 *                lx, ly         position of light source
 * OUTPUT:        index of selected flare (starting from 1 !)
 *************/
int SelectedFlare(FLARE *root, int mx, int my, int width, int height, int lx, int ly)
{
	int dx, dy, d, dmin, i, index;
	FLARE *flare;

	flare = root;

	dmin = width;
	i = 1;
	index = 1;

	while (flare)
	{
		dx = mx - int(lx + float((width>>1) - lx)*flare->pos);
		dy = my - int(ly + float((height>>1) - ly)*flare->pos);

		d = (int)sqrt(dx*dx + dy*dy);
		if (d < dmin)
		{
			dmin = d;
			index = i;
		}
		flare = (FLARE *)flare->GetNext();
		i++;
	}
	return index;
}

/*************
 * DESCRIPTION:   flare draws itself and all flares which are linked after it
 * INPUT:         display        display class
 *                lx,ly          position of lightsource in screen coord.
 *                width,height   width and height of draw area
 *                selected       set color of selected flares differnt to unselected
 * OUTPUT:        none
 *************/
void FLARE::Draw(DISPLAY *display, RECTANGLE *r, int lx, int ly, BOOL selected)
{
	FLARE *obj;
	int scrwh, scrhh;
	int i, x, y, ox, oy, divs, fx, fy;
	float rad;

	obj = this;
	scrwh = (r->right - r->left) >> 1;
	scrhh = (r->bottom - r->top) >> 1;

	// go through all flares
	while (obj)
	{
		if(selected)
		{
			if(obj->selected)
				gfx.SetPen(COLOR_SELECTED);
			else
				gfx.SetPen(COLOR_UNSELECTED);
		}

		// calculate screen position of flare
		fx = scrwh - int((lx - (scrwh + r->left))*(obj->pos - 1)) + r->left;
		fy = scrhh - int((ly - (scrhh + r->top))*(obj->pos - 1)) + r->top;

		rad = obj->radius * scrwh;

		if (obj->type == FLARE_DISC)
			divs = 360 / 20;
		else
			divs = 360 / obj->edges;

		for (i = 0; i <= 360; i += divs)
		{
			x = (int)(cos(i*PI_180 + obj->tilt)*rad) + fx;
			y = (int)(sin(i*PI_180 + obj->tilt)*rad) + fy;
			if (i > 0)
				gfx.ClipLine(ox,oy, x,y);
			ox = x;
			oy = y;
		}
		display->AccumulateRect2D(fx - (int)rad, fy - (int)rad, fx + (int)rad, fy + (int)rad);

		obj = (FLARE *)obj->GetNext();
	}
}

/*************
 * DESCRIPTION:   duplicate a flare
 * INPUT:         -
 * OUTPUT:        new flare or NULL if failed
 *************/
FLARE *FLARE::Duplicate()
{
	FLARE *dup;

	dup = new FLARE;
	if (!dup)
		return NULL;

	*dup = *this;

	return dup;
}

/*************
 * DESCRIPTION:   transfer flare data to RayStorm
 * INPUT:         pointer to light source
 * OUTPUT:        error result of RayStorm
 *************/
rsiResult FLARE::ToRSI(rsiCONTEXT *rc, void *light)
{
	FLARE *obj;
	rsiResult err;
	void *flare;

	obj = this;

	while (obj)
	{
		err = PPC_STUB(rsiCreateFlare)(CTXT, light, &flare);
		if (err)
			return err;

		err = PPC_STUB(rsiSetFlare)(CTXT, flare,
			rsiTFlareType, obj->type,
			rsiTFlareFunc, obj->func,
			rsiTFlarePos, obj->pos,
			rsiTFlareColor, &obj->color,
			rsiTFlareRadius, obj->radius,
			rsiTFlareEdges, obj->edges,
			rsiTFlareTilt, obj->tilt,
			rsiTDone);
		if (err)
			return err;

		obj = (FLARE*)obj->GetNext();
	}

	return rsiERR_NONE;
}

/*************
 * DESCRIPTION:   write flare to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL FLARE::Write(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_FLAR, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteLongChunk(iff, ID_COLR, &color, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_TYPE, &type, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_FUNC, &func, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_POSI, &pos, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_RADI, &radius, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_EDGE, &edges, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_TILT, &tilt, 1))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   read star from scene file
 * INPUT:         iff      iff handler
 *                light    light to add star to
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL ParseFlare(struct IFFHandle *iff, LIGHT *light)
{
	struct ContextNode *cn;
	FLARE *flare;
	long error = 0;

	flare = new FLARE;
	if(!flare)
		return FALSE;

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
			return FALSE;

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_FLAR))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}

		switch (cn->cn_ID)
		{
			case ID_COLR:
				if(!ReadFloat(iff,(float*)&flare->color,3))
					return FALSE;
				break;
			case ID_TYPE:
				if(!ReadInt(iff,&flare->type,1))
					return FALSE;
				break;
			case ID_FUNC:
				if(!ReadInt(iff,&flare->func,1))
					return FALSE;
				break;
			case ID_POSI:
				if(!ReadFloat(iff,&flare->pos,1))
					return FALSE;
				break;
			case ID_RADI:
				if(!ReadFloat(iff,&flare->radius,1))
					return FALSE;
				break;
			case ID_EDGE:
				if(!ReadInt(iff,&flare->edges,1))
					return FALSE;
				break;
			case ID_TILT:
				if(!ReadFloat(iff,&flare->tilt,1))
					return FALSE;
				break;
		}
	}

	flare->Append((SLIST**)&light->flares);

	return TRUE;
}

/*************
 * DESCRIPTION:   Get size needed of a flare
 * INPUT:         -
 * OUTPUT:        size
 *************/
int FLARE::GetSize()
{
	return sizeof(*this);
}
