/***-************
 * MODULE:        star.cpp
 * PROJECT:       Scenario
 * DESCRIPTION:   draws light star
 * AUTHORS:       Mike Hesser & Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.10.96 mh    initial release
 *    13.11.96 mh    Duplicate
 *    18.11.96 ah    added Read() and ParseStar()
 *    27.11.96 ah    added innerhaloradius
 *    14.12.96 ah    added spikewidth
 *    18.12.96 ah    added GetSize()
 *    30.01.97 mh    added tilt angle
 ***************/

#include <string.h>
#include <stdlib.h>

#ifndef STAR_H
#include "star.h"
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
STAR::STAR()
{
	starcolor.r = 1.0f;
	starcolor.g = 0.6f;
	starcolor.b = 0.6f;
	ringcolor.r = 0.156f;
	ringcolor.g = 0.0392f;
	ringcolor.b = 0.0196f;

	spikes = 4;
	noise = 5;
	flags = STAR_ENABLE | STAR_HALOENABLE;
	starradius = 0.3f;
	haloradius = 0.6f;
	innerhaloradius = 0.4f;
	intensities = NULL;
	brightness = 40.0f;
	range = 0.;
	tilt = 0.;
}

/*************
 * DESCRIPTION:   destructor
 * INPUT:         -
 * OUTPUT:        -
 *************/
STAR::~STAR()
{
	if (intensities)
		delete intensities;
}

/*************
 * DESCRIPTION:   generate spikes. Type and range are kept in random and range
 * INPUT:         spikes_  number of spikes
 * OUTPUT:        none
 *************/
void STAR::SetSpikes(int spikes_)
{
	int i, j, h, step;
	float diff;

	// delete given intensities and allocate memory for new ones
	if (intensities)
		delete intensities;

	spikes = spikes_;
	intensities = new float[spikes];
	if (!intensities)
		return;

	// calculate no. of sub-spike lengths
	// i.e. a star with 8 spikes has two lengths. One for the four main directions
	// and the other length for the spikes between them
	h = int(log(float(spikes))/log(2.));
	if (h > 2)
		diff = range/(h - 2);
	else
	{
		h++;
		diff = range;
	}

	// generate the spikes
	step = 1;
	for (j = 1; j < h; j++)
	{
		for (i = 0; i < spikes; i += step)
		{
			if (j == 1)
				intensities[i] = 1.f - range;
			else
				intensities[i] += diff;
		}
		step += step;
	}

	// apply random length
	if (flags & STAR_RANDOM)
	{
		for (i = 0; i < spikes; i++)
		{
			intensities[i] += (float(rand())/RAND_MAX) * range - range;
			if (intensities[i] < 0.f)
				intensities[i] = 0.f;
			if (intensities[i] > 1.f)
				intensities[i] = 1.f;
		}
	}
}

/*************
 * DESCRIPTION:   draw light burn star
 * INPUT:         display     display class
 * OUTPUT:        none
 *************/
void STAR::Draw(DISPLAY *display, int centerx, int centery, int scrw)
{
	STAR *obj;
	int i;
	int x, y, ox, oy;
	float rad, angle;

	obj = this;

	// draw the spikes with length "intensity"
	if (flags & STAR_ENABLE)
	{
		for (i = 0; i < spikes; i++)
		{
			angle = i*(360.f/spikes)*PI_180 + tilt;
			rad = intensities[i] * starradius * scrw * .5f;
			x = (int)(cos(angle)*rad);
			y = (int)(sin(angle)*rad);
			gfx.ClipLine(centerx, centery, centerx + x, centery + y);
		}
	}

	// draw halo
	if (flags & STAR_HALOENABLE)
	{
		rad = haloradius * starradius * scrw * .5f;
		for (i = 0; i <= 360; i += 4)
		{
			angle = float(i)*PI_180;
			x = (int)(cos(angle)*rad) + centerx;
			y = (int)(sin(angle)*rad) + centery;
			if (i > 0)
				gfx.ClipLine(ox,oy, x,y);
			ox = x;
			oy = y;
		}
		rad *= innerhaloradius;
		for (i = 0; i <= 360; i += 4)
		{
			angle = float(i)*PI_180;
			x = (int)(cos(angle)*rad) + centerx;
			y = (int)(sin(angle)*rad) + centery;
			if (i > 0)
				gfx.ClipLine(ox,oy, x,y);
			ox = x;
			oy = y;
		}
	}

	// set bounding rectangle
	rad = starradius * scrw * 0.5f;
	display->AccumulateRect2D(centerx - (int)rad, centery - (int)rad,
								  centerx + (int)rad, centery + (int)rad);
}

/*************
 * DESCRIPTION:   duplicate a star
 * INPUT:         -
 * OUTPUT:        new star or NULL if failed
 *************/
STAR *STAR::Duplicate()
{
	STAR *dup;

	dup = new STAR;
	if (!dup)
		return NULL;

	*dup = *this;

	dup->intensities = new float[spikes];
	if (!dup->intensities)
	{
		delete dup;
		return NULL;
	}

	memcpy(dup->intensities, intensities, sizeof(float) * spikes);

	return dup;
}

/*************
 * DESCRIPTION:   transfer star data to RayStorm
 * INPUT:         pointer to light source
 * OUTPUT:        error result of RayStorm
 *************/
rsiResult STAR::ToRSI(rsiCONTEXT *rc, void *light)
{
	rsiResult err;
	void *star;

	err = PPC_STUB(rsiCreateStar)(CTXT, light, &star);
	if(err)
		return err;

	return PPC_STUB(rsiSetStar)(CTXT, star,
		rsiTStarFlags, flags,
		rsiTStarColor, &starcolor,
		rsiTStarRingColor, &ringcolor,
		rsiTStarRadius, starradius,
		rsiTStarTilt, tilt,
		rsiTStarHaloRadius, haloradius,
		rsiTStarInnerHaloRadius, innerhaloradius,
		rsiTStarSpikes, spikes,
		rsiTStarNoise, noise,
		rsiTStarIntensities, intensities,
		rsiTStarBrightness, brightness,
		rsiTDone);
}

/*************
 * DESCRIPTION:   write star to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL STAR::Write(struct IFFHandle *iff)
{
	ULONG f;

	if(PushChunk(iff, ID_STAR, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	f = 0;
	if(flags & STAR_ENABLE)
		f |= RSCN_STAR_ENABLE;
	if(flags & STAR_HALOENABLE)
		f |= RSCN_STAR_HALOENABLE;
	if(flags & STAR_RANDOM)
		f |= RSCN_STAR_RANDOM;
	if(!WriteLongChunk(iff, ID_FLGS, &f, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_SRAD, &starradius, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_TILT, &tilt, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_SPIK, &spikes, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_HRAD, &haloradius, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_IHRD, &innerhaloradius, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_RANG, &range, 1))
		return FALSE;

	if(intensities)
	{
		if(!WriteLongChunk(iff, ID_INTE, intensities, spikes))
			return FALSE;
	}

	if(!WriteLongChunk(iff, ID_COLR, &starcolor, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_RCOL, &ringcolor, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_NOIS, &noise, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_BRIG, &brightness, 1))
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
BOOL ParseStar(struct IFFHandle *iff, LIGHT *light)
{
	struct ContextNode *cn;
	long error = 0;
	ULONG flags;

	light->star = new STAR;
	if(!light->star)
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

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_STAR))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}

		switch (cn->cn_ID)
		{
			case ID_FLGS:
				if(!ReadULONG(iff,&flags,1))
					return FALSE;
				if(flags & RSCN_STAR_ENABLE)
					light->star->flags |= STAR_ENABLE;
				else
					light->star->flags &= ~STAR_ENABLE;
				if(flags & RSCN_STAR_HALOENABLE)
					light->star->flags |= STAR_HALOENABLE;
				else
					light->star->flags &= ~STAR_HALOENABLE;
				if(flags & RSCN_STAR_RANDOM)
					light->star->flags |= STAR_RANDOM;
				else
					light->star->flags &= ~STAR_RANDOM;
				break;
			case ID_SRAD:
				if(!ReadFloat(iff,&light->star->starradius,1))
					return FALSE;
				break;
			case ID_TILT:
				if(!ReadFloat(iff,&light->star->tilt,1))
					return FALSE;
				break;
			case ID_SPIK:
				if(!ReadInt(iff,&light->star->spikes,1))
					return FALSE;
				break;
			case ID_HRAD:
				if(!ReadFloat(iff,&light->star->haloradius,1))
					return FALSE;
				break;
			case ID_IHRD:
				if(!ReadFloat(iff,&light->star->innerhaloradius,1))
					return FALSE;
				break;
			case ID_RANG:
				if(!ReadFloat(iff,&light->star->range,1))
					return FALSE;
				break;
			case ID_INTE:
				if(light->star->spikes)
				{
					light->star->intensities = new float[light->star->spikes];
					if(!light->star->intensities)
						return FALSE;
					if(!ReadFloat(iff, light->star->intensities, light->star->spikes))
						return FALSE;
				}
				break;
			case ID_COLR:
				if(!ReadFloat(iff,(float*)&light->star->starcolor,3))
					return FALSE;
				break;
			case ID_RCOL:
				if(!ReadFloat(iff,(float*)&light->star->ringcolor,3))
					return FALSE;
				break;
			case ID_NOIS:
				if(!ReadFloat(iff,&light->star->noise,1))
					return FALSE;
				break;
			case ID_BRIG:
				if(!ReadFloat(iff,&light->star->brightness,1))
					return FALSE;
				break;
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   Get size needed of a star
 * INPUT:         -
 * OUTPUT:        size
 *************/
int STAR::GetSize()
{
	return sizeof(*this) + spikes*sizeof(float);
}
