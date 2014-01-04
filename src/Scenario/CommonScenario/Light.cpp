/***************
 * PROGRAM:       Modeler
 * NAME:          light.cpp
 * DESCRIPTION:   Functions for light object class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.04.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 ***************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef VIEW_H
#include "view.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef FLARES_H
#include "flares.h"
#endif

#ifndef STAR_H
#include "star.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef ERRORS_H
#include "errors.h"
#endif

#ifndef OPENGL_UTIL_H
#include "ogl_util.h"
#endif

#ifndef __AMIGA__
#include "resource.h"
#else
#include "pragma/dos_lib.h"
#ifndef UTILITY_H
#include "utility.h"
#endif
static struct NewMenu popupmenu[] =
{
	{ NM_TITLE, NULL              , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "Move"            , 0 ,0,0,(APTR)POP_OBJECT_MOVE},
	{ NM_ITEM , "Rotate"          , 0 ,0,0,(APTR)POP_OBJECT_ROTATE},
	{ NM_ITEM , "Scale"           , 0 ,0,0,(APTR)POP_OBJECT_SCALE},
	{ NM_ITEM , "Settings..."     , 0 ,0,0,(APTR)POP_SETTINGS},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};

#endif

// default flares
#define DEFFLARES 12

typedef struct
{
	float pos;
	float radius;
	int type;
	int func;
	COLOR color;
} DEFFLARE;

static DEFFLARE default_flares[] =
{
	{-.40f, .030f, FLARE_DISC, FLARE_FUNC_RADIAL,{ .40f,  .40f,  .40f}},
	{ .26f, .018f, FLARE_DISC, FLARE_FUNC_RADIAL,{0.00f, 0.00f,  .20f}},
	{ .30f, .050f, FLARE_DISC, FLARE_FUNC_EXP1,  {0.00f, 0.00f,  .11f}},
	{ .36f, .027f, FLARE_DISC, FLARE_FUNC_EXP2,  {0.00f, 0.00f,  .15f}},
	{ .50f, .022f, FLARE_DISC, FLARE_FUNC_EXP3,  { .30f,  .20f, 0.00f}},
	{ .64f, .005f, FLARE_DISC, FLARE_FUNC_PLAIN, {1.00f, 1.00f, 1.00f}},
	{ .88f, .007f, FLARE_DISC, FLARE_FUNC_PLAIN, {1.00f, 1.00f, 1.00f}},
	{ .96f, .052f, FLARE_DISC, FLARE_FUNC_RADIAL,{ .20f,  .10f, 0.00f}},
	{1.08f, .025f, FLARE_DISC, FLARE_FUNC_RADIAL,{ .30f,  .20f, 0.00f}},
	{1.20f, .033f, FLARE_DISC, FLARE_FUNC_EXP1,  {0.00f,  .45f,  .80f}},
	{1.60f, .100f, FLARE_DISC, FLARE_FUNC_RADIAL,{ .20f,  .30f, 0.00f}},
	{2.00f, .200f, FLARE_DISC, FLARE_FUNC_RADIAL,{ .18f,  .18f,  .25f}},
};

/*************
 * DESCRIPTION:   Constructor of light (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
LIGHT::LIGHT()
{
	r = 0.f;
	color.r = color.g = color.b = 1.f;
	falloff = 0.f;

	flares = NULL;
	star = NULL;
	shadowmapsize = 128;
	flags |= OBJECT_LIGHT_SHADOW;
}

/*************
 * DESCRIPTION:   Destructor of light
 * INPUT:         none
 * OUTPUT:        none
 *************/
LIGHT::~LIGHT()
{
	FLARE *next;

	while (flares)
	{
		next = (FLARE *)flares->GetNext();
		delete flares;
		flares = next;
	}
	if (star)
		delete star;
}

/*************
 * DESCRIPTION:   Set the color with wich the object is drawn
 * INPUT:         -
 * OUTPUT:        -
 *************/
void LIGHT::SetDrawColor(DISPLAY *display)
{
#ifdef __OPENGL__
	if(display->display == DISPLAY_SOLID)
		SetGLColor(COLOR_LIGHT);
	else
#endif // __OPENGL__
		gfx.SetPen(COLOR_LIGHT);
}

/*************
 * DESCRIPTION:   write light source header to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL LIGHT::WriteLight(struct IFFHandle *iff)
{
	FLARE *flare;

	if(!WriteCommon(iff))
		return FALSE;

	if(star)
	{
		if(!star->Write(iff))
			return FALSE;
	}

	flare = flares;
	while(flare)
	{
		if(!flare->Write(iff))
			return FALSE;

		flare = (FLARE*)flare->GetNext();
	}

	if(!WriteLongChunk(iff, ID_COLR, &color, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_DIST, &falloff, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_RADI, &r, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_SMSZ, &shadowmapsize, 1))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   gets the popuplist for the selected object
 * INPUT:         -
 * OUTPUT:        popup list
 *************/
#ifdef __AMIGA__
struct NewMenu *LIGHT::GetPopUpList()
{
	return popupmenu;
}
#else
int LIGHT::GetResourceID()
{
	return IDR_POPUP_POINTLIGHT;
}
#endif

/*************
 * DESCRIPTION:   set the default flares to a flare list or to this light source
 * INPUT:         flares      flare root pointer or NULL to add to light
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL LIGHT::SetDefaultFlares(FLARE **flares)
{
	int count;
	FLARE *flare;

	if(!flares)
		flares = &this->flares;

	for(count=0; count<DEFFLARES; count++)
	{
		flare = new FLARE;
		if(!flare)
			return FALSE;
		flare->pos = default_flares[count].pos;
		flare->radius = default_flares[count].radius;
		flare->type = default_flares[count].type;
		flare->func = default_flares[count].func;
		flare->color = default_flares[count].color;

		flare->Append((SLIST**)flares);
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL LIGHT::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_LIGHT);
}

/*************
 * DESCRIPTION:   draws lensflares and light burn
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void LIGHT::DrawLensEffects(DISPLAY *disp, MATRIX_STACK *stack)
{
	VECTOR v;
	RECTANGLE r;

	// draw flares in camview/perspective mode only
	if (disp->view->viewmode != VIEW_CAMERA)
		return;

	// get 2D coord. of light source
	SetVector(&v, 0.f,0.f,0.f);
	stack->MultVectStack(&v);
	disp->view->MCtoDC_xy(&v);

	// get render area, light effects can only be drawn when light is
	// within render area
	disp->GetRenderRect(&r);
	if ((int)v.x < r.left || (int)v.x > r.right ||
		 (int)v.y < r.top  || (int)v.y > r.bottom)
		return;

	if (flares && (flags & OBJECT_LIGHT_FLARES))
		flares->Draw(disp, &r, (int)v.x, (int)v.y, FALSE);

	if (star && (flags & OBJECT_LIGHT_STAR))
		star->Draw(disp, (int)v.x, (int)v.y, r.right - r.left);
}

/*************
 * DESCRIPTION:   Get the size of a light source
 * INPUT:         -
 * OUTPUT:        size
 *************/
int LIGHT::GetSize()
{
	int size;
	FLARE *flare;

	size = sizeof(*this) + strlen(GetName()) + 1;

	flare = flares;
	while(flare)
	{
		size += flare->GetSize();
		flare = (FLARE*)flare->GetNext();
	}
	if(star)
		size += star->GetSize();

	return size;
}

/*************
 * DESCRIPTION:   save a light definition
 * INPUT:         filename    name of light file
 * OUTPUT:        error string or NULL if all ok
 *************/
char *LIGHT::Save(char *filename)
{
	struct IFFHandle *iff;

	// Allocate IFF_File structure.
	iff = AllocIFF();
	if(!iff)
		return errors[ERR_MEM];

	// Set up IFF_File for AmigaDOS I/O.
#ifdef __AMIGA__
	iff->iff_Stream = Open(filename, MODE_NEWFILE);
#else
	iff->iff_Stream = Open_(filename, MODE_NEWFILE);
#endif
	if(!iff->iff_Stream)
	{
		IFFCleanup(iff);
		return errors[ERR_OPEN];
	}
	InitIFFasDOS(iff);

	// Start the IFF transaction.
	if(OpenIFF(iff, IFFF_WRITE))
	{
		IFFCleanup(iff);
		return errors[ERR_IFFPARSE];
	}

	PUSH_CHUNK(ID_RLIG);

	PUSH_CHUNK(GetType() == OBJECT_POINTLIGHT ? ID_PLGT : ID_SLGT);

	// write light
	WriteLight(iff);

	POP_CHUNK();

	POP_CHUNK();

	IFFCleanup(iff);
	return NULL;
}

/*************
 * DESCRIPTION:   load a light definition
 * INPUT:         filename    name of light file
 * OUTPUT:        error string or NULL if all ok
 *************/
char *LIGHT::Load(char *filename)
{
	long error = 0;
	struct IFFHandle *iff;
	struct ContextNode *cn;
	LIGHT *light;
	FLARE *next;
	VECTOR old_pos, old_ox,old_oy,old_oz, old_size;
	char *old_name;
	OBJECT *old_track;
	ULONG type;

	iff = AllocIFF();
	if (!iff)
		return errors[ERR_MEM];

#ifdef __AMIGA__
	iff->iff_Stream = Open(filename, MODE_OLDFILE);
#else
	iff->iff_Stream = Open_(filename, MODE_OLDFILE);
#endif
	if (!(iff->iff_Stream))
	{
		IFFCleanup(iff);
		return errors[ERR_OPEN];
	}

	InitIFFasDOS(iff);
	error = OpenIFF(iff, IFFF_READ);
	if (error)
	{
		IFFCleanup(iff);
		return errors[ERR_IFFPARSE];
	}

	error = ParseIFF(iff, IFFPARSE_RAWSTEP);
	if (error)
	{
		IFFCleanup(iff);
		return errors[ERR_IFFPARSE];
	}
	cn = CurrentChunk(iff);
	if(!cn)
	{
		IFFCleanup(iff);
		return errors[ERR_IFFPARSE];
	}
	if((cn->cn_ID != ID_FORM) || (cn->cn_Type != ID_RLIG))
	{
		IFFCleanup(iff);
		return errors[ERR_NORLIGFILE];
	}
	error = ParseIFF(iff, IFFPARSE_RAWSTEP);
	if (error)
	{
		IFFCleanup(iff);
		return errors[ERR_IFFPARSE];
	}

	rscn_version = VERSION;
	switch(GetType())
	{
		case OBJECT_POINTLIGHT:
			type = ID_PLGT;
			break;
		case OBJECT_SPOTLIGHT:
			type = ID_SLGT;
			break;
		case OBJECT_DIRECTIONALLIGHT:
			type = ID_DLGT;
			break;
	}
	light = (LIGHT*)ParseLight(iff, NULL, 0, type);
	if(!light)
	{
		IFFCleanup(iff);
		return errors[ERR_RLIGCORRUPT];
	}
	IFFCleanup(iff);

	while(flares)
	{
		next = (FLARE*)flares->GetNext();
		delete flares;
		flares = next;
	}
	if(star)
		delete star;

	old_name = GetName();
	old_track = track;
	old_pos = pos;
	old_ox = orient_x;
	old_oy = orient_y;
	old_oz = orient_z;
	old_size = size;
	light->SetName(NULL);
	*this = *light;
	SetName(old_name);
	track = old_track;
	pos = old_pos;
	orient_x = old_ox;
	orient_y = old_oy;
	orient_z = old_oz;
	size = old_size;

	light->flares = NULL;
	light->star = NULL;
	delete light;

	return NULL;
}

/*************
 * DESCRIPTION:   duplicates the lens effects (star, flares) of a object
 * INPUT:         dup
 * OUTPUT:        FALSE if failed
 *************/
BOOL LIGHT::DuplicateLensEffects(LIGHT *dup)
{
	FLARE *flare, *dupflare;

	dup->star = NULL;
	dup->flares = NULL;

	if(star)
	{
		dup->star = star->Duplicate();
		if(!dup->star)
			return FALSE;
	}

	if(flares)
	{
		flare = flares;
		while(flare)
		{
			dupflare = flare->Duplicate();
			if(!dupflare)
				return FALSE;
			dupflare->Append((SLIST**)&dup->flares);
			flare = (FLARE*)flare->GetNext();
		}
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   parse a point light source from a RSCN file
 * INPUT:         iff      iff handler
 *                where    where to insert after
 *                dir      insert direction
 *                id       chunk id (ID_PLGT, ID_SLGT, ID_DLGT)
 * OUTPUT:        created object
 *************/
OBJECT *ParseLight(struct IFFHandle *iff, OBJECT *where, int dir, ULONG id)
{
	LIGHT *light;
	struct ContextNode *cn;
	long error = 0;
	ULONG flags;
	float t;

	switch(id)
	{
		case ID_PLGT:
			light = new POINT_LIGHT;
			break;
		case ID_SLGT:
			light = new SPOT_LIGHT;
			break;
		case ID_DLGT:
			light = new DIRECTIONAL_LIGHT;
			break;
	}
	if(!light)
		return NULL;

	if(!light->ReadAxis(iff))
	{
		delete light;
		return NULL;
	}

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete light;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == id))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_STAR:
						if(!ParseStar(iff,(LIGHT*)light))
							return FALSE;
						break;
					case ID_FLAR:
						if(!ParseFlare(iff,(LIGHT*)light))
							return FALSE;
						break;
				}
				break;
			case ID_NAME:
				if(!light->ReadName(iff))
				{
					delete light;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!light->ReadTrack(iff))
				{
					delete light;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(rscn_version < 200)
				{
					if(!ReadULONG(iff, &flags, 1))
					{
						delete light;
						return NULL;
					}
					if(flags & 1)
						light->flags |= OBJECT_LIGHT_SHADOW;
					else
						light->flags &= ~OBJECT_LIGHT_SHADOW;
					if(flags & 2)
						light->flags |= OBJECT_LIGHT_FLARES;
					else
						light->flags &= ~OBJECT_LIGHT_FLARES;
					if(flags & 4)
						light->flags |= OBJECT_LIGHT_STAR;
					else
						light->flags &= ~OBJECT_LIGHT_STAR;
					if(flags & 8)
						light->flags |= OBJECT_LIGHT_TRACKFALLOFF;
					else
						light->flags &= ~OBJECT_LIGHT_TRACKFALLOFF;
				}
				else
				{
					if(!light->ReadFlags(iff))
					{
						delete light;
						return NULL;
					}
				}
				break;
			case ID_COLR:
				if(!ReadFloat(iff,(float*)&light->color,3))
				{
					delete light;
					return NULL;
				}
				break;
			case ID_DIST:
				if(!ReadFloat(iff,&light->falloff,1))
				{
					delete light;
					return NULL;
				}
				break;
			case ID_RADI:
				if(!ReadFloat(iff,&light->r,1))
				{
					delete light;
					return NULL;
				}
				break;
			case ID_SMSZ:
				if(!ReadULONG(iff, &light->shadowmapsize, 1))
				{
					delete light;
					return NULL;
				}
				break;
			case ID_ANGL:
				ASSERT(id == ID_SLGT);
				if(id == ID_SLGT)
				{
					if(!ReadFloat(iff,&((SPOT_LIGHT*)light)->angle,1))
					{
						delete light;
						return NULL;
					}
				}
				break;
			case ID_FALL:
				ASSERT(id == ID_SLGT);
				if(id == ID_SLGT)
				{
					if(!ReadFloat(iff,&((SPOT_LIGHT*)light)->falloff_radius, 1))
					{
						delete light;
						return NULL;
					}
				}
				break;
		}
	}

	if(id == ID_SLGT)
	{
		t = tan(((SPOT_LIGHT*)light)->angle)*10.f*light->size.x;
		SetVector(&light->bboxmin, -t, -t, -1.f);
		SetVector(&light->bboxmax, t, t, 10.f*light->size.x);
	}

	if(where)
		light->Insert(where, dir);

	return (OBJECT*)light;
}

/*************
 * DESCRIPTION:   Constructor of point light (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
POINT_LIGHT::POINT_LIGHT()
{
	SetName("Point light");
	CalcBBox();
}

/*************
 * DESCRIPTION:   Draw a point light
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void POINT_LIGHT::Draw(DISPLAY *disp,MATRIX_STACK *stack)
{
	VECTOR u,v,s;

	switch(disp->display)
	{
		case DISPLAY_BBOX:
		case DISPLAY_WIRE:
			DrawLensEffects(disp, stack);

			VecScale(0.577f, &size, &s);

			u = s;
			v.x = -s.x;
			v.y = -s.y;
			v.z = -s.z;
			disp->TransformDraw(&u,&v,stack);

			u.x = -s.x;
			v.x = s.x;
			disp->TransformDraw(&u,&v,stack);

			u.x = s.x;
			u.z = -s.z;
			v.x = -s.x;
			v.z = s.z;
			disp->TransformDraw(&u,&v,stack);

			u.x = -s.x;
			v.x = s.x;
			disp->TransformDraw(&u,&v,stack);
			break;
#ifdef __OPENGL__
		case DISPLAY_SOLID:
			OpenGLDrawPointLight(&s);
			break;
#endif // __OPENGL__
	}
}

/*************
 * DESCRIPTION:   sets the new object specs
 * INPUT:         disp     pointer to display structure
 *                pos      translate factor
 *                ox,oy,oz rotate factor
 *                size     scale factor
 * OUTPUT:        none
 *************/
void POINT_LIGHT::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	CalcBBox();
}

/*************
 * DESCRIPTION:   write point light source to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL POINT_LIGHT::WriteObject(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_PLGT, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteLight(iff))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   transfer point light data to RayStorm Interface
 * INPUT:         stack       matrix stack
 *                object   pointer to created rsi object
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult POINT_LIGHT::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	VECTOR orient_x, orient_y, orient_z, pos;
	ULONG flags;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	err = PPC_STUB(rsiCreatePointLight)(CTXT, object);
	if(err)
		return err;

	// transfer lens flare and star data
	if(flares && (this->flags & OBJECT_LIGHT_FLARES))
	{
		err = flares->ToRSI(rc, *object);
		if(err)
			return err;
	}
	if(star && (this->flags & OBJECT_LIGHT_STAR))
	{
		err = star->ToRSI(rc, *object);
		if(err)
			return err;
	}

	flags = 0;
	if(this->flags & OBJECT_LIGHT_SHADOW)
		flags |= rsiFLightShadow;
	if(this->flags & OBJECT_LIGHT_FLARES)
		flags |= rsiFLightFlares;
	if(this->flags & OBJECT_LIGHT_STAR)
		flags |= rsiFLightStar;
	if(this->flags & OBJECT_LIGHT_SHADOWMAP)
		flags |= rsiFLightShadowMap;

	return PPC_STUB(rsiSetPointLight)(CTXT, *object,
		rsiTLightPos,           &pos,
		rsiTLightColor,         &color,
		rsiTLightRadius,        r,
		rsiTLightFlags,         flags,
		rsiTLightFallOff,       falloff,
		rsiTLightShadowMapSize, shadowmapsize,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *POINT_LIGHT::DupObj()
{
	POINT_LIGHT *dup;

	dup = new POINT_LIGHT;
	if(!dup)
		return NULL;
	*dup = *this;

	if(!DuplicateLensEffects(dup))
	{
		delete dup;
		return NULL;
	}

	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   calculate bounding box for object
 * INPUT:         none
 * OUTPUT:        none
 *************/
void POINT_LIGHT::CalcBBox()
{
	SetVector(&bboxmin, -size.x*0.577, -size.y*0.577, -size.z*0.577);
	SetVector(&bboxmax, size.x*0.577, size.y*0.577, size.z*0.577);
}

/*************
 * DESCRIPTION:   Constructor of spot light (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
SPOT_LIGHT::SPOT_LIGHT()
{
	track = NULL;

	SetName("Spot light");
	angle = 45.f*PI_180;
	falloff_radius = 1.f;

	CalcBBox();
}

/*************
 * DESCRIPTION:   Draw a spot light
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void SPOT_LIGHT::Draw(DISPLAY *disp,MATRIX_STACK *stack)
{
	VECTOR u,v,s;
	float t;
	VECTOR p, ox, oy, oz;
	VECTOR cp, cox, coy, coz;
	MATRIX m,m1;

	switch(disp->display)
	{
		case DISPLAY_BBOX:
		case DISPLAY_WIRE:
			if (flares)
			{
				// draw flares if camera is within illuminated area
				if ((disp->editmode & EDIT_ACTIVE) && (selected || disp->camera->selected))
				{
					m.SetOMatrix(&disp->orient_x, &disp->orient_y, &disp->orient_z);

					// get absolute light and camera axis
					GetObjectMatrix(&m1);
					m1.GenerateAxis(&ox, &oy, &oz, &p);
					if(selected)
					{
						m.MultMatOrient(&ox, &oy, &oz);
						VecAdd(&p, &disp->pos, &p);
					}
					InvOrient(&ox, &oy, &oz, &ox, &oy, &oz);

					disp->camera->GetObjectMatrix(&m1);
					m1.GenerateAxis(&cox, &coy, &coz, &cp);
					if(disp->camera->selected)
					{
						VecAdd(&cp, &disp->pos, &cp);
					}

					VecSub(&cp, &p, &s);
					VecNormalize(&s);

					if (acos(dotp(&s, &oz)) <= angle)
						DrawLensEffects(disp, stack);
				}
				else
				{
					// neither camera nor light source is selected

					// get absolute light and camera axis
					GetObjectMatrix(&m1);
					m1.GenerateAxis(&ox, &oy, &oz, &p);
					InvOrient(&ox, &oy, &oz, &ox, &oy, &oz);
					disp->camera->GetObjectMatrix(&m1);
					m1.GenerateAxis(&cox, &coy, &coz, &cp);

					VecSub(&cp, &p, &s);
					VecNormalize(&s);

					if (acos(dotp(&s, &oz)) <= angle)
						DrawLensEffects(disp, stack);
				}
			}

			VecScale(0.577f, &size, &s);

			u = s;
			SetVector(&v, -s.x, -s.y, -s.z);
			disp->TransformDraw(&u,&v,stack);

			u.x = -s.x;
			v.x = s.x;
			disp->TransformDraw(&u,&v,stack);

			u.x = s.x;
			u.z = -s.z;
			v.x = -s.x;
			v.z = s.z;
			disp->TransformDraw(&u,&v,stack);

			u.x = -s.x;
			v.x = s.x;
			disp->TransformDraw(&u,&v,stack);

			// draw lines which indicate the opening angle
			SetVector(&u, 0.f, 0.f, 0.f);
			v.x = t = tan(angle) * size.x * 10.f;
			v.y = 0.f;
			v.z = size.x * 10.f;
			disp->TransformDraw(&u,&v,stack);

			v.x = -t;
			disp->TransformDraw(&u,&v,stack);

			v.x = 0.f;
			v.y = t;
			disp->TransformDraw(&u,&v,stack);

			v.y = - v.y;
			disp->TransformDraw(&u,&v,stack);

			t *= 0.707f;
			v.x = t;
			v.y = t;
			disp->TransformDraw(&u,&v,stack);

			v.x = -v.x;
			disp->TransformDraw(&u,&v,stack);

			v.y = -v.y;
			disp->TransformDraw(&u,&v,stack);

			v.x = -v.x;
			disp->TransformDraw(&u,&v,stack);

			break;
#ifdef __OPENGL__
		case DISPLAY_SOLID:
			OpenGLDrawSpotLight(&s, &size, angle);
			break;
#endif // __OPENGL__
	}
}

/*************
 * DESCRIPTION:   sets the new object specs
 * INPUT:         disp     pointer to display structure
 *                pos      translate factor
 *                ox,oy,oz rotate factor
 *                size     scale factor
 * OUTPUT:        none
 *************/
void SPOT_LIGHT::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	CalcBBox();
}

/*************
 * DESCRIPTION:   write spot light source to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SPOT_LIGHT::WriteObject(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_SLGT, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteLight(iff))
		return FALSE;

	if(!WriteLongChunk(iff, ID_ANGL, &angle, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_FALL, &falloff_radius, 1))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   transfer spot light data to RayStorm Interface
 * INPUT:         stack    matrix stack
 *                object   pointer to created rsi object
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult SPOT_LIGHT::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	VECTOR look;
	VECTOR orient_x, orient_y, orient_z, pos;
	ULONG flags;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	err = PPC_STUB(rsiCreateSpotLight)(CTXT, object);
	if(err)
		return err;

	// transfer lens flare and star data
	if(flares && (this->flags & OBJECT_LIGHT_FLARES))
	{
		err = flares->ToRSI(rc, *object);
		if(err)
			return err;
	}
	if(star && (this->flags & OBJECT_LIGHT_STAR))
	{
		err = star->ToRSI(rc, *object);
		if(err)
			return err;
	}

	flags = 0;
	if(this->flags & OBJECT_LIGHT_SHADOW)
		flags |= rsiFLightShadow;
	if(this->flags & OBJECT_LIGHT_FLARES)
		flags |= rsiFLightFlares;
	if(this->flags & OBJECT_LIGHT_STAR)
		flags |= rsiFLightStar;
	if(this->flags & OBJECT_LIGHT_SHADOWMAP)
		flags |= rsiFLightShadowMap;

	InvOrient(&orient_x, &orient_y, &orient_z, &orient_x, &orient_y, &orient_z);
	VecAdd(&pos, &orient_z, &look);

	return PPC_STUB(rsiSetSpotLight)(CTXT, *object,
		rsiTLightPos,           &pos,
		rsiTLightColor,         &color,
		rsiTLightRadius,        r,
		rsiTLightFlags,         flags,
		rsiTLightFallOff,       falloff,
		rsiTLightAngle,         angle*INV_PI_180,
		rsiTLightFallOffRadius, falloff_radius,
		rsiTLightLookPoint,     &look,
		rsiTLightShadowMapSize, shadowmapsize,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *SPOT_LIGHT::DupObj()
{
	SPOT_LIGHT *dup;

	dup = new SPOT_LIGHT;
	if(!dup)
		return NULL;
	*dup = *this;

	if(!DuplicateLensEffects(dup))
	{
		delete dup;
		return NULL;
	}

	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   calculate bounding box for object
 * INPUT:         none
 * OUTPUT:        none
 *************/
void SPOT_LIGHT::CalcBBox()
{
	float t;

	t = tan(angle) * 10.f * size.x;

	SetVector(&bboxmin, -t, -t, -1.f);
	SetVector(&bboxmax, t, t, 10.f * size.x);
}

/*************
 * DESCRIPTION:   Constructor of directional light (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
DIRECTIONAL_LIGHT::DIRECTIONAL_LIGHT()
{
	SetName("Directional light");
	CalcBBox();
}

/*************
 * DESCRIPTION:   Draw a directional light
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void DIRECTIONAL_LIGHT::Draw(DISPLAY *disp,MATRIX_STACK *stack)
{
	VECTOR u,v;

	switch(disp->display)
	{
		case DISPLAY_BBOX:
		case DISPLAY_WIRE:
			DrawLensEffects(disp, stack);

			SetVector(&u, -size.x, -size.y, 0.f);
			SetVector(&v, -size.x, -size.y, size.z);
			disp->TransformDraw(&u,&v,stack);
			SetVector(&u, -size.x, 0.f, 0.f);
			SetVector(&v, -size.x, 0.f, size.z);
			disp->TransformDraw(&u,&v,stack);
			SetVector(&u, -size.x, size.y, 0.f);
			SetVector(&v, -size.x, size.y, size.z);
			disp->TransformDraw(&u,&v,stack);
			SetVector(&u, 0.f, -size.y, 0.f);
			SetVector(&v, 0.f, -size.y, size.z);
			disp->TransformDraw(&u,&v,stack);
			SetVector(&u, 0.f, size.y, 0.f);
			SetVector(&v, 0.f, size.y, size.z);
			disp->TransformDraw(&u,&v,stack);
			SetVector(&u, size.x, -size.y, 0.f);
			SetVector(&v, size.x, -size.y, size.z);
			disp->TransformDraw(&u,&v,stack);
			SetVector(&u, size.x, 0.f, 0.f);
			SetVector(&v, size.x, 0.f, size.z);
			disp->TransformDraw(&u,&v,stack);
			SetVector(&u, size.x, size.y, 0.f);
			SetVector(&v, size.x, size.y, size.z);
			disp->TransformDraw(&u,&v,stack);
			break;
#ifdef __OPENGL__
		case DISPLAY_SOLID:
			OpenGLDrawDirectionalLight(&size);
			break;
#endif // __OPENGL__
	}
}

/*************
 * DESCRIPTION:   sets the new object specs
 * INPUT:         disp     pointer to display structure
 *                pos      translate factor
 *                ox,oy,oz rotate factor
 *                size     scale factor
 * OUTPUT:        none
 *************/
void DIRECTIONAL_LIGHT::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	CalcBBox();
}

/*************
 * DESCRIPTION:   write directional light source to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL DIRECTIONAL_LIGHT::WriteObject(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_DLGT, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteLight(iff))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   transfer directional light data to RayStorm Interface
 * INPUT:         stack       matrix stack
 *                object   pointer to created rsi object
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult DIRECTIONAL_LIGHT::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	VECTOR orient_x, orient_y, orient_z, pos;
	ULONG flags;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);
	InvOrient(&orient_x, &orient_y, &orient_z, &orient_x, &orient_y, &orient_z);

	err = PPC_STUB(rsiCreateDirectionalLight)(CTXT, object);
	if(err)
		return err;

	// transfer lens flare and star data
	if(flares && (this->flags & OBJECT_LIGHT_FLARES))
	{
		err = flares->ToRSI(rc, *object);
		if(err)
			return err;
	}
	if(star && (this->flags & OBJECT_LIGHT_STAR))
	{
		err = star->ToRSI(rc, *object);
		if(err)
			return err;
	}

	flags = 0;
	if(this->flags & OBJECT_LIGHT_SHADOW)
		flags |= rsiFLightShadow;
	if(this->flags & OBJECT_LIGHT_FLARES)
		flags |= rsiFLightFlares;
	if(this->flags & OBJECT_LIGHT_STAR)
		flags |= rsiFLightStar;
	if(this->flags & OBJECT_LIGHT_SHADOWMAP)
		flags |= rsiFLightShadowMap;

	return PPC_STUB(rsiSetDirectionalLight)(CTXT, *object,
		rsiTLightPos,           &pos,
		rsiTLightDir,           &orient_z,
		rsiTLightColor,         &color,
		rsiTLightRadius,        r,
		rsiTLightFlags,         flags,
		rsiTLightFallOff,       falloff,
		rsiTLightShadowMapSize, shadowmapsize,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *DIRECTIONAL_LIGHT::DupObj()
{
	DIRECTIONAL_LIGHT *dup;

	dup = new DIRECTIONAL_LIGHT;
	if(!dup)
		return NULL;
	*dup = *this;

	if(!DuplicateLensEffects(dup))
	{
		delete dup;
		return NULL;
	}

	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   calculate bounding box for object
 * INPUT:         none
 * OUTPUT:        none
 *************/
void DIRECTIONAL_LIGHT::CalcBBox()
{
	SetVector(&bboxmin, -size.x, -size.y, 0.f);
	SetVector(&bboxmax, size.x, size.y, size.z);
}

