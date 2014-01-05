/***************
 * PROGRAM:       Modeler
 * NAME:          surface.cpp
 * DESCRIPTION:   functions for surface class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.01.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 *    03.05.97 ah    changed the way the surfaces are handled; they
 *       are now stored in a list and objects share one surface;
 *       the surface name is now unique
 ***************/

#include <string.h>
#include <stdio.h>

#ifdef __STORM__
#include <pragma/dos_lib.h>
#endif

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef IFF_UTIL_H
#include "iff_util.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef ERRORS_H
#include "errors.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

static SURFACE *root=NULL;

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
SURFACE::SURFACE()
{
	name = NULL;
	texture = NULL;
	brush = NULL;
	flags = 0;
	SetColor(&ambient,   1.f, 1.f, 1.f);
	SetColor(&diffuse,   1.f, 1.f, 1.f);
	SetColor(&specular,  1.f, 1.f, 1.f);
	SetColor(&reflect,   0.f, 0.f, 0.f);
	SetColor(&transpar,  0.f, 0.f, 0.f);
	SetColor(&difftrans, 0.f, 0.f, 0.f);
	SetColor(&spectrans, 1.f, 1.f, 1.f);
	refphong = 12.f;
	transphong = 12.f;
	foglength = 0.f;
	refrindex = 1.f;
	translucency = 0.f;
#ifdef __AMIGA__
	win = NULL;
#endif
}

/*************
 * DESCRIPTION:   destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
SURFACE::~SURFACE()
{
	TEXTURE *tnext;
	BRUSH *bnext;

#ifdef __AMIGA__
	// close dialog
	SetAttrs(win, MUIA_Window_Open, FALSE, TAG_DONE);
	DoMethod(app, OM_REMMEMBER, win);
	MUI_DisposeObject(win);
#endif

	while(texture)
	{
		tnext = (TEXTURE*)texture->GetNext();
		delete texture;
		texture = tnext;
	}

	while(brush)
	{
		bnext = (BRUSH*)brush->GetNext();
		delete brush;
		brush = bnext;
	}

	if(name)
		delete name;
}

/*************
 * DESCRIPTION:   compare function for InsertSorted()
 * INPUT:         a,b      items to compare
 * OUTPUT:        > 0 -> a > b
 *                = 0 -> a = b
 *                < 0 -> a < b
 *************/
static int CompareFunction(void *a, void *b)
{
	return(strcmp(((SURFACE*)a)->name, ((SURFACE*)b)->name));
}

/*************
 * DESCRIPTION:   Add a new surface to the list. The surface is inserted sorted.
 * INPUT:         -
 * OUTPUT:        -
 *************/
void SURFACE::Add()
{
	this->SLIST::InsertSorted((SLIST**)&root, CompareFunction);
}

/*************
 * DESCRIPTION:   remove a surface from the list (NOTE: the surface is deleted)
 * INPUT:         -
 * OUTPUT:        -
 *************/
void SURFACE::Remove()
{
	this->SLIST::Remove((SLIST**)&root);
}

/*************
 * DESCRIPTION:   free all surfaces of surface list
 * INPUT:         -
 * OUTPUT:        -
 *************/
void FreeSurfaces()
{
	if(root)
	{
		root->SLIST::FreeList();
		root = NULL;
	}
}

/*************
 * DESCRIPTION:   get a surface by name
 * INPUT:         name
 * OUTPUT:        surface
 *************/
SURFACE *GetSurfaceByName(char *name)
{
	SURFACE *cur;

	cur = root;
	while(cur)
	{
		if(cur->name)
		{
			if(!strcmp(cur->name, name))
				return cur;
		}

		cur = (SURFACE*)cur->GetNext();
	}

	return NULL;
}

/*************
 * DESCRIPTION:   set surface name, if a surface with this name already exist
 *    create a different name
 * INPUT:         name     new name
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SURFACE::SetName(char *newname)
{
	SURFACE *surf;
	char *buffer;
	int index = 0;

	if(name)
	{
		delete name;
		name = NULL;
	}

	if(newname)
	{
		buffer = new char[strlen(newname)+10];
		if(!buffer)
			return FALSE;

		strcpy(buffer, newname);

		// search for an surface with this name
		surf = GetSurfaceByName(newname);
		if(surf && (surf != this))
		{
			// found it
			do
			{
				// create a new name with an index
				index++;
				sprintf(buffer,"%s.%d",newname,index);
			}
			while(GetSurfaceByName(buffer));
		}

		name = new char[strlen(buffer)+1];
		if(!name)
		{
			delete buffer;
			return FALSE;
		}

		strcpy(name, buffer);
		delete buffer;
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   write surface to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SURFACE::Write(struct IFFHandle *iff)
{
	TEXTURE *texture;
	BRUSH *brush;

	if(PushChunk(iff, ID_SURF, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(name)
	{
		if(!WriteChunk(iff, ID_NAME, name, strlen(name)+1))
			return FALSE;
	}

	if(!WriteLongChunk(iff, ID_FLGS, &flags, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_AMBT, &ambient, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_DIFU, &diffuse, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_SPEC, &specular, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_REFL, &reflect, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_TRNS, &transpar, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_DIFT, &difftrans, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_SPCT, &spectrans, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_RPHG, &refphong, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_TPHG, &transphong, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_FLEN, &foglength, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_IXOR, &refrindex, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_TNSL, &translucency, 1))
		return FALSE;

	texture = this->texture;
	while(texture)
	{
		texture->Write(iff);
		texture = (TEXTURE*)texture->GetNext();
	}

	brush = this->brush;
	while(brush)
	{
		brush->Write(iff);
		brush = (BRUSH*)brush->GetNext();
	}

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   read surface from scene file
 * INPUT:         iff      iff handler
 *                obj      object for this surface (needed for pre V2.0 files)
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SURFACE::Read(struct IFFHandle *iff, OBJECT *obj)
{
	struct ContextNode *cn;
	long error = 0;
	char buffer[256];
	BRUSH_OBJECT *brushobj;
	BRUSH *brush;
	TEXTURE_OBJECT *textureobj;
	TEXTURE *texture;

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
			return FALSE;

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_SURF))
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
					case ID_BRSH:
						if((rscn_version < 200) && obj)
						{
							brushobj = ParseBrushObject(iff, obj);
							if(!brushobj)
								return FALSE;
						}
						brush = ParseBrush(iff,this);
						if(!brush)
							return FALSE;

						if((rscn_version < 200) && obj)
							brushobj->brush = brush;
						break;
					case ID_ITXT:
						if((rscn_version < 200) && obj)
						{
							textureobj = ParseTextureObject(iff, obj);
							if(!textureobj)
								return FALSE;
						}
						texture = ParseImagineTexture(iff,this);
						if(!texture)
							return FALSE;

						if((rscn_version < 200) && obj)
							textureobj->texture = texture;
						break;
					case ID_RTXT:
						texture = ParseRayStormTexture(iff,this);
						if(!texture)
							return FALSE;
						break;
					case ID_HTXT:
						texture = ParseHyperTexture(iff,this);
						if(!texture)
							return FALSE;
						break;
				}
				break;
			case ID_NAME:
				if(!ReadString(iff,buffer,256))
					return FALSE;
				SetName(buffer);
				break;
			case ID_FLGS:
				if(!ReadULONG(iff,&flags,1))
					return FALSE;
				break;
			case ID_AMBT:
				if(!ReadFloat(iff,(float*)&ambient,3))
					return FALSE;
				break;
			case ID_DIFU:
				if(!ReadFloat(iff,(float*)&diffuse,3))
					return FALSE;
				break;
			case ID_SPEC:
				if(!ReadFloat(iff,(float*)&specular,3))
					return FALSE;
				break;
			case ID_REFL:
				if(!ReadFloat(iff,(float*)&reflect,3))
					return FALSE;
				break;
			case ID_TRNS:
				if(!ReadFloat(iff,(float*)&transpar,3))
					return FALSE;
				break;
			case ID_DIFT:
				if(!ReadFloat(iff,(float*)&difftrans,3))
					return FALSE;
				break;
			case ID_SPCT:
				if(!ReadFloat(iff,(float*)&spectrans,3))
					return FALSE;
				break;
			case ID_RPHG:
				if(!ReadFloat(iff,&refphong,1))
					return FALSE;
				break;
			case ID_TPHG:
				if(!ReadFloat(iff,&transphong,1))
					return FALSE;
				break;
			case ID_FLEN:
				if(!ReadFloat(iff,&foglength,1))
					return FALSE;
				break;
			case ID_IXOR:
				if(!ReadFloat(iff,&refrindex,1))
					return FALSE;
				break;
			case ID_TNSL:
				if(!ReadFloat(iff,&translucency,1))
					return FALSE;
				break;
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   read surface from scene file
 * INPUT:         iff      iff handler
 *                object   object surface
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL ReadSurface(struct IFFHandle *iff, SURFACE **surf, OBJECT *object)
{
	struct ContextNode *cn;
	long error;
	char buffer[256];
	SURFACE *tmp;
	BRUSH_OBJECT *brushobj;
	BRUSH *brush;
	TEXTURE_OBJECT *textureobj;
	TEXTURE *texture;

	if(rscn_version < 200)
	{
		*surf = sciCreateSurface(SURF_DEFAULTNAME);
		if(!*surf)
			return FALSE;

		if(!(*surf)->Read(iff, object))
			return FALSE;

		if(rscn_version == 100)
		{
			// in version 100 the brushes and textures were relative to the object
			(*surf)->ConvertV100(object);
		}
		if(rscn_version < 200)
		{
			// in pre 200 version surfaces were handled in a different way
			tmp = (*surf)->ConvertPreV200(object);
			if(!tmp)
				sciAddMaterial(*surf);
			else
				*surf = tmp;
		}
	}
	else
	{
		error = 0;

		while(!error)
		{
			error = ParseIFF(iff, IFFPARSE_RAWSTEP);
			if(error < 0 && error != IFFERR_EOC)
				return FALSE;

			// Get a pointer to the context node describing the current context
			cn = CurrentChunk(iff);
			if(!cn)
				continue;

			if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_SURF))
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
						case ID_BRSH:
							if(brush)
							{
								brushobj = ParseBrushObject(iff, object);
								if(!brushobj)
									return FALSE;
								brushobj->brush = brush;
								brush = (BRUSH*)brush->GetNext();
							}
							break;
						case ID_ITXT:
							if(texture)
							{
								textureobj = ParseTextureObject(iff, object);
								if(!textureobj)
									return FALSE;
								textureobj->texture = texture;
								texture = (TEXTURE*)texture->GetNext();
							}
							break;
					}
					break;
				case ID_NAME:
					if(!ReadString(iff,buffer,256))
						return FALSE;

					*surf = GetSurfaceByName(buffer);
					if(!(*surf))
					{
						*surf = GetSurfaceByName(SURF_DEFAULTNAME);
						if(!*surf)
							return FALSE;
					}
					brush = (*surf)->brush;
					texture = (*surf)->texture;
					break;
			}
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   add a brush to the surface brush list
 * INPUT:         brush    brush to add
 * OUTPUT:        none
 *************/
void SURFACE::AddBrush(BRUSH *brush)
{
	brush->SLIST::Append((SLIST**)&this->brush);
}

/*************
 * DESCRIPTION:   add a texture to the surface texture list
 * INPUT:         brush    brush to add
 * OUTPUT:        none
 *************/
void SURFACE::AddTexture(TEXTURE *texture)
{
	texture->SLIST::Append((SLIST**)&this->texture);
}

/*************
 * DESCRIPTION:   transfer surface data to RayStorm Interface
 * INPUT:         surf     created surface
 *                stack    matrix stack
 *                obj      object for this surface
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult SURFACE::ToRSI(rsiCONTEXT *rc, void **surf, MATRIX_STACK *stack, OBJECT *obj)
{
	rsiResult err;
	int flags;
	TEXTURE_OBJECT *texture;
	BRUSH_OBJECT *brush;

	if(this->flags & SURF_BRIGHT)
		flags = rsiFSurfBright;
	else
		flags = 0;

	err = PPC_STUB(rsiCreateSurface)(CTXT, surf, rsiTSurfFlags, flags, rsiTDone);
	if(err)
		return err;

	brush = obj->brush;
	while(brush)
	{
		err = brush->ToRSI(rc, *surf,obj,stack);
		if(err)
			return err;
		brush = (BRUSH_OBJECT*)brush->GetNext();
	}

	texture = obj->texture;
	while(texture)
	{
		err = texture->ToRSI(rc, *surf,obj,stack);
		if(err)
			return err;
		texture = (TEXTURE_OBJECT*)texture->GetNext();
	}

	return PPC_STUB(rsiSetSurface)(CTXT, *surf,
		rsiTSurfAmbient,     &ambient,
		rsiTSurfDiffuse,     &diffuse,
		rsiTSurfFoglen,      foglength,
		rsiTSurfSpecular,    &specular,
		rsiTSurfDifftrans,   &difftrans,
		rsiTSurfSpectrans,   &spectrans,
		rsiTSurfRefexp,      refphong,
		rsiTSurfTransexp,    transphong,
		rsiTSurfRefrindex,   refrindex,
		rsiTSurfReflect,     &reflect,
		rsiTSurfTranspar,    &transpar,
		rsiTSurfTransluc,    translucency,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate a surface with brushes and textures
 * INPUT:         -
 * OUTPUT:        new surface or NULL if failed
 *************/
SURFACE *SURFACE::Duplicate()
{
	SURFACE *dup;
	BRUSH *curbrush, *dupbrush;
	TEXTURE *curtexture, *duptexture;

	dup = new SURFACE;
	if(!dup)
		return NULL;

	*dup = *this;
	((LIST*)dup)->Init();

	dup->name = NULL;
	dup->SetName(name);

	dup->brush = NULL;
	dup->texture = NULL;

	// copy brushes
	curbrush = brush;
	while(curbrush)
	{
		dupbrush = (BRUSH*)curbrush->DupObj();
		if(!dupbrush)
			return NULL;

		// set all connections to NULL
		((LIST*)dupbrush)->Init();

		// and add to surface brush list
		dup->AddBrush(dupbrush);

		curbrush = (BRUSH*)curbrush->GetNext();
	}

	// copy textures
	curtexture = texture;
	while(curtexture)
	{
		duptexture = (TEXTURE*)curtexture->DupObj();
		if(!duptexture)
			return NULL;

		// set all connections to NULL
		((LIST*)duptexture)->Init();

		// and add to surface texture list
		dup->AddTexture(duptexture);

		curtexture = (TEXTURE*)curtexture->GetNext();
	}

	dup->win = NULL;

	return dup;
}

/*************
 * DESCRIPTION:   save a surface definition
 * INPUT:         filename    name of surface file
 * OUTPUT:        error string or NULL if all ok
 *************/
char *SURFACE::Save(char *filename)
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

	PUSH_CHUNK(ID_RMAT);

	// write surface
	Write(iff);

	IFFCleanup(iff);
	return NULL;
}

/*************
 * DESCRIPTION:   load a surface definition
 * INPUT:         filename    name of surface file
 * OUTPUT:        error string or NULL if all ok
 *************/
char *SURFACE::Load(char *filename)
{
	long error = 0;
	struct IFFHandle *iff;
	struct ContextNode *cn;

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
	if((cn->cn_ID != ID_FORM) || (cn->cn_Type != ID_RMAT))
	{
		IFFCleanup(iff);
		return errors[ERR_NORMATFILE];
	}

	while(!error || error == IFFERR_EOC)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error != IFFERR_EOC)
		{
			// Get a pointer to the context node describing the current context
			cn = CurrentChunk(iff);
			if (cn)
			{
				switch (cn->cn_ID)
				{
					case ID_FORM:
						switch(cn->cn_Type)
						{
							case ID_SURF:
								if(!Read(iff, NULL))
								{
									IFFCleanup(iff);
									return errors[ERR_RMATCORRUPT];
								}
								break;
						}
						break;
				}
			}
		}
	}
	if (error != IFFERR_EOF)
	{
		return errors[ERR_IFFPARSE];
	}

	IFFCleanup(iff);

	return NULL;
}

/*************
 * DESCRIPTION:   convert rscn version 100 brushes and textures
 * INPUT:         root     surface root object
 * OUTPUT:        -
 *************/
void SURFACE::ConvertV100(OBJECT *root)
{
	BRUSH_OBJECT *brush;
	TEXTURE_OBJECT *texture;
	VECTOR ox,oy,oz;
	MATRIX m;

	InvOrient(&root->orient_x, &root->orient_y, &root->orient_z, &ox, &oy, &oz);
	m.SetOMatrix(&ox,&oy,&oz);

	brush = root->brush;
	while(brush)
	{
		VecSub(&brush->pos, &root->pos, &brush->pos);
		m.MultVectMat(&brush->orient_x);
		m.MultVectMat(&brush->orient_y);
		m.MultVectMat(&brush->orient_z);
		brush = (BRUSH_OBJECT*)brush->GetNext();
	}

	texture = root->texture;
	while(texture)
	{
		VecSub(&texture->pos, &root->pos, &texture->pos);
		m.MultVectMat(&texture->orient_x);
		m.MultVectMat(&texture->orient_y);
		m.MultVectMat(&texture->orient_z);
		texture = (TEXTURE_OBJECT*)texture->GetNext();
	}
}

/*************
 * DESCRIPTION:   in pre 200 version surface were handled in a different way
 * INPUT:         -
 * OUTPUT:        NULL if no identical surface was found else identical surface
 *************/
SURFACE *SURFACE::ConvertPreV200(OBJECT *obj)
{
	SURFACE *surf;

	surf = FindIdentical();
	if(surf)
	{
		this->SLIST::Remove((SLIST**)&root);
		return surf;
	}

	return NULL;
}

/*************
 * DESCRIPTION:   Get size needed of a surface
 * INPUT:         -
 * OUTPUT:        size
 *************/
int SURFACE::GetSize()
{
	int size;
	BRUSH *brush;
	TEXTURE *texture;

	size = sizeof(*this);

	if(name)
		size += strlen(name) + 1;

	brush = this->brush;
	while(brush)
	{
		size += brush->GetSize();
		brush = (BRUSH*)brush->GetNext();
	}

	texture = this->texture;
	while(texture)
	{
		size += texture->GetSize();
		texture = (TEXTURE*)texture->GetNext();
	}

	return size;
}

/*************
 * DESCRIPTION:   Find a identical surface
 * INPUT:         -
 * OUTPUT:        identical surface or NULL if none found
 *************/
SURFACE *SURFACE::FindIdentical()
{
	SURFACE *surf, *cur;

	if(texture || brush)
		return NULL;

	cur = root;
	while(cur)
	{
		surf = cur;
		cur = (SURFACE*)cur->GetNext();

		if(surf->texture || surf->brush || (surf == this))
			continue;

		if(flags != surf->flags)
			continue;
		if(refphong != surf->refphong)
			continue;
		if(transphong != surf->transphong)
			continue;
		if(foglength != surf->foglength)
			continue;
		if(refrindex != surf->refrindex)
			continue;
		if(translucency != surf->translucency)
			continue;
		if((ambient.r != surf->ambient.r) || (ambient.g != surf->ambient.g) || (ambient.b != surf->ambient.b))
			continue;
		if((diffuse.r != surf->diffuse.r) || (diffuse.g != surf->diffuse.g) || (diffuse.b != surf->diffuse.b))
			continue;
		if((specular.r != surf->specular.r) || (specular.g != surf->specular.g) || (specular.b != surf->specular.b))
			continue;
		if((reflect.r != surf->reflect.r) || (reflect.g != surf->reflect.g) || (reflect.b != surf->reflect.b))
			continue;
		if((transpar.r != surf->transpar.r) || (transpar.g != surf->transpar.g) || (transpar.b != surf->transpar.b))
			continue;
		if((difftrans.r != surf->difftrans.r) || (difftrans.g != surf->difftrans.g) || (difftrans.b != surf->difftrans.b))
			continue;
		if((spectrans.r != surf->spectrans.r) || (spectrans.g != surf->spectrans.g) || (spectrans.b != surf->spectrans.b))
			continue;

		return surf;
	}
	return NULL;
}

/*************
 * DESCRIPTION:   Parse surfaces
 * INPUT:         -
 * OUTPUT:        error string
 *************/
char *ParseSurfaces(struct IFFHandle *iff)
{
	SURFACE *surf;
	struct ContextNode *cn;
	long error = 0;

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
			return errors[ERR_RSCNFILE];

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_SRFS))
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
					case ID_SURF:
						surf = sciCreateSurface(SURF_DEFAULTNAME);
						if(!surf)
							return errors[ERR_MEM];

						if(!surf->Read(iff, NULL))
						{
							delete surf;
							return errors[ERR_RSCNFILE];
						}
						sciAddMaterial(surf);
						break;
				}
				break;
		}
	}

	return NULL;
}

/*************
 * DESCRIPTION:   Write all surfaces
 * INPUT:         iff      iff handle
 * OUTPUT:        FALSE if failed
 *************/
BOOL WriteSurfaces(struct IFFHandle *iff)
{
	SURFACE *cur;

	cur = root;
	while(cur)
	{
		if(!(cur->flags & SURF_EXTERNAL) && !(cur->flags & SURF_DEFAULT))
		{
			if(!cur->Write(iff))
				return FALSE;
		}
		cur = (SURFACE*)cur->GetNext();
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   Reset the written flags of all surfaces
 * INPUT:         -
 * OUTPUT:        -
 *************/
void SurfaceResetWritten()
{
	SURFACE *cur;

	cur = root;
	while(cur)
	{
		cur->flags &= ~SURF_WRITTEN;
		cur = (SURFACE*)cur->GetNext();
	}
}
