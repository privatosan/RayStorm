/***************
 * MODUL:         brush
 * NAME:          brush.cpp
 * DESCRIPTION:   This file includes the memberfunctions of the brush-class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.03.95 ah    initial release
 *    07.08.95 ah    added IMAGE::IMAGE()
 *    15.08.95 ah    added REPEAT and MIRROR for brush
 *    08.09.95 ah    added Update() BRUSH()
 *    09.09.95 ah    added time for ApplyBrush(), brush is only updated
 *       if ApplyBrush() is called
 *    10.09.95 ah    added specular mapping
 *    01.11.95 ah    added soft color interpolation
 *    03.11.95 ah    changed flat brush mapping
 *    09.11.95 ah    added 'IMAGE::Load()'
 *    26.11.95 ah    added multiple paths for 'IMAGE::Load()'
 *    15.04.96 ah    added Datatype support
 *    08.02.97 ah    added 24bit Datatype support
 *    21.06.97 ah    changed to new V2 brush handling
 *    03.11.97 ah    Save() no passes a parameter down to the saver
 *    10.05.99 ah    added support of brush mapping coordinates
 ***************/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __AMIGA__
	#include <graphics/gfx.h>
	#include <intuition/gadgetclass.h>

	#include <exec/memory.h>
	#include <datatypes/datatypes.h>
	#include <datatypes/datatypesclass.h>
	#include <datatypes/pictureclass.h>
	#include <datatypes/pictureclassext.h>
	#include <cybergraphics/cybergraphics.h>
	#include <clib/alib_protos.h>
	#include <pragma/dos_lib.h>
	#include <pragma/exec_lib.h>
	#include <pragma/graphics_lib.h>
	#include <pragma/datatypes_lib.h>
	#include "lib.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef FILEUTIL_H
#include "fileutil.h"
#endif

#ifndef ERROR_H
#include "error.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

#ifndef __AMIGA__
extern char WorkingDirectory[256];
typedef BOOL *(*KEYWRITEPROC)(char *, PICTURE *, char *);
extern KEYWRITEPROC keyWritePicture;
typedef char *(*READWRITEPROC)(PICTURE *);
#else
#include "rmod:pichand_lib.h"
struct Library *DataTypesBase;
// StormC assumes that a library base is always global, so lock that
// library base
static BOOL LockLibBase = FALSE;
struct Library *PicHandBase;
#endif

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
IMAGE::IMAGE()
{
	colormap = NULL;
	name = NULL;
}

/*************
 * DESCRIPTION:   destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
IMAGE::~IMAGE()
{
	if(colormap)
	{
		free(colormap);
	}
	if(name)
		delete [ ] name;
}

/*************
 * DESCRIPTION:   Inserts a image in the image list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void IMAGE::Insert(RSICONTEXT *rc)
{
	rc->Insert(this);
}

#ifdef __PPC__
// need this stub because PPC always uses indirect addressing, __saveds
// sets up the neccessary stuff
void* __saveds PPC_malloc(size_t size)
{
	return malloc(size);
}
#endif // __PPC__

/*************
 * DESCRIPTION:   Loads a image. Invokes the different handlers for the
 *    picture types. Checks the first 16 Byte of the image file.
 * INPUT:         filename    name of picture to load
 *                path        picture is searched at this paths
 * OUTPUT:        ERR_NONE if ok, else error code
 *************/
int IMAGE::Load(RSICONTEXT *rc, char *filename, char *path)
{
	char buffer[256];
	FILE *typefile;
	FILE *imagefile;
	char typebuf[18];
	char imagebuf[16];
	char *errstr;
	PICTURE pic;
#ifdef __AMIGA__
	int err;
	char handlername[32];
	BPTR lock;
#else
	HINSTANCE hPicLib;
	READWRITEPROC picRead;
	char buf[256];
#endif
	int i;
	BOOL identok=FALSE;
	UWORD mask;

	/* expand path with BRUSHPATH */
	if(!ExpandPath(path,filename,buffer))
	{
		/* cannot open picture */
		return ERR_OPENPIC;
	}
	name = new char[strlen(buffer)+1];
	strcpy(name,buffer);

	/* open image and read first 16 Bytes */
	imagefile = fopen(name,"rb");
	if(!imagefile)
		return ERR_OPENPIC;
	if(fread(imagebuf,1,16,imagefile) != 16)
	{
		fclose(imagefile);
		return ERR_READPIC;
	}
	fclose(imagefile);
#ifdef __AMIGA__
	// Amiga-version: get directory RayStorm was started from and
	// search file from there
	// get a lock to the program directory
	lock = GetProgramDir();
	// make program directory to current directory
	lock = CurrentDir(lock);
	typefile = fopen("modules/picture/types","rb");
	// change current dir to old dir
	lock = CurrentDir(lock);
#else
	strcpy(buf, WorkingDirectory);
	AddPart(buf, "modules", 256);
	AddPart(buf, "picture", 256);
	AddPart(buf, "types.dat", 256);
	typefile = fopen(buf, "rb");
#endif
	if(!typefile)
	{
		return ERR_OPENPICTYPEFILE;
	}

	while(!feof(typefile) && !identok)
	{
		// Read identification string
		// Format:
		// UWORD mask;    mask for bytes to test '1'->test; lowest bit -> first byte
		// UBYTE id[16];  bytes to compare
		// char name[8];  name of module
		if(fread(&mask,2,1,typefile) != 1)
		{
			fclose(typefile);
			return ERR_READPICTYPEFILE;
		}
		#ifdef __SWAP__
		mask = SwapW(mask);
		#endif
		if(fread(typebuf,1,16,typefile) != 16)
		{
			fclose(typefile);
			return ERR_READPICTYPEFILE;
		}

		// Compare first 16 bytes of image with identstring
		identok = TRUE;
		for(i=0; i<16; i++)
		{
			if(mask & 0x8000)
			{
				if(typebuf[i] != imagebuf[i])
				{
					identok = FALSE;
					break;
				}
			}
			mask = mask << 1;
		}
		// Read imagename (max. 8 chars)
		if(fread(typebuf,1,8,typefile) != 8)
		{
			fclose(typefile);
			return ERR_READPICTYPEFILE;
		}
	}

	fclose(typefile);

	if(identok)
	{
		typebuf[8] = 0;
#ifdef __AMIGA__
		// open picture handler library
		// make program directory current directory
		lock = CurrentDir(lock);
		strcpy(handlername,"modules/picture");
		AddPart(handlername,typebuf,32);
#ifdef __PPC__
		strcat(handlername, "ppc");
#endif

		while(LockLibBase);
		LockLibBase = TRUE;

		// Load picture handler
		PicHandBase = OpenLibrary(handlername, 2L);
		if(!PicHandBase)
		{
			LockLibBase = FALSE;
			return ERR_WRONGPICHANDLER;
		}

		// change current dir to old dir
		CurrentDir(lock);
#else
		// load corresponding DLL
		strcpy(buf, WorkingDirectory);
		AddPart(buf, "modules", 256);
		AddPart(buf, "picture", 256);
		AddPart(buf, typebuf, 256);
		strcat(buf, ".dll");

		if (!(hPicLib = LoadLibrary(buf)))
			return ERR_PICHANDLER;
		if (!(picRead = (READWRITEPROC)GetProcAddress(hPicLib, "picRead_")))
			return ERR_PICHANDLER;
#endif

		rc->SetImageError(NULL);

		pic.name = name;
		pic.caller = NULL;
		pic.param = NULL;
#ifdef __PPC__
		pic.malloc = PPC_malloc;
#else
		pic.malloc = malloc;
#endif // __PPC__
		pic.Progress = NULL;

		// invoke handler
		errstr = picRead(&pic);
		if(errstr)
		{

#ifdef __AMIGA__
			// failed to load it with own modules -> try it with datatypes
			err = LoadAsDatatype(name);
			if(err != ERR_NONE)
			{
				// can't load it even with datatypes
				// copy error string
				rc->SetImageError(errors[err]);
				CloseLibrary(PicHandBase);
				LockLibBase = FALSE;
				return ERR_EXTERNAL;
			}
#else
			rc->SetImageError(errstr);
			FreeLibrary(hPicLib);
			return ERR_EXTERNAL;
#endif // __AMIGA__
		}
		else
		{
			width = pic.width;
			height = pic.height;
			colormap = pic.colormap;
		}

		// free handler library
#ifdef __AMIGA__
		CloseLibrary(PicHandBase);
		LockLibBase = FALSE;
#else
		FreeLibrary(hPicLib);
#endif // __AMIGA__
	}
	else
	{
#ifdef __AMIGA__
		rc->SetImageError(NULL);

		while(LockLibBase);
		LockLibBase = TRUE;

		// there is no module for this picture type -> try it with datatypes
		err = LoadAsDatatype(name);
		if(err != ERR_NONE)
		{
			// can't load it even with datatypes
			// copy error string
			rc->SetImageError(errors[err]);
			LockLibBase = FALSE;
			return ERR_EXTERNAL;
		}
		LockLibBase = FALSE;
#else
		return ERR_UNKNOWNPIC;
#endif
	}
	return ERR_NONE;
}

#ifdef __AMIGA__
/*************
 * DESCRIPTION:   Loads a image with the datatypes.
 * INPUT:         filename    name of picture to load
 * OUTPUT:        error code
 *************/
int IMAGE::LoadAsDatatype(char *filename)
{
	struct Library *PictureDTBase;
	BOOL pic24bit;             // TRUE for new 24bit picture.datatype (V43)
	Object *obj;
	struct BitMapHeader *bmhd;
	struct BitMap *bitmap, *tempbitmap;
	int x,y;
	SMALL_COLOR *c;
	UBYTE *colreg, *buffer;
	int color;
	ULONG modeid;
	struct DisplayInfo dispinfo;
	UBYTE r,g,b;
	struct RastPort rp, temprp;
	struct gpBlitPixelArray arg;

	DataTypesBase = OpenLibrary((unsigned char*)"datatypes.library", 39);
	if(!DataTypesBase)
		return ERR_NODATATYPESLIB;

	// try to open the 24bit picture datatype
	PictureDTBase = OpenLibrary("datatypes/picture.datatype", 43L);
	if(PictureDTBase)
	{
		// we only need the picture.datatype to test the version, therefore
		// close it now
		CloseLibrary(PictureDTBase);

		pic24bit = TRUE;
	}
	else
		pic24bit = FALSE;

	if(!pic24bit)
	{
		// no 24bit version available
		// get the picture object
		obj = NewDTObject(filename,
			DTA_SourceType,   DTST_FILE,
			DTA_GroupID,      GID_PICTURE,
			PDTA_Remap,       FALSE,
			TAG_DONE);
		if(!obj)
		{
			CloseLibrary(DataTypesBase);
			return ERR_OPENPIC;
		}
	}
	else
	{
		// get the picture object
		obj = NewDTObject(filename,
			DTA_SourceType,   DTST_FILE,
			DTA_GroupID,      GID_PICTURE,
			PDTA_Remap,       FALSE,
			PDTA_DestMode,    MODE_V43,
			TAG_DONE);
		if(!obj)
		{
			CloseLibrary(DataTypesBase);
			return ERR_OPENPIC;
		}
	}

	// get the bitmap header
	if(GetDTAttrs(obj,
		PDTA_ColorRegisters, &colreg,
		PDTA_BitMap, &bitmap,
		PDTA_ModeID, &modeid,
		PDTA_BitMapHeader, &bmhd,
		TAG_DONE) != 4)
	{
		DisposeDTObject(obj);
		CloseLibrary(DataTypesBase);
		return ERR_READPIC;
	}

	if(!bmhd || !bitmap)
	{
		DisposeDTObject(obj);
		CloseLibrary(DataTypesBase);
		return ERR_READPIC;
	}

	width = bmhd->bmh_Width;
	height = bmhd->bmh_Height;

	if(!GetDisplayInfoData(NULL, (UBYTE*)&dispinfo, sizeof(struct DisplayInfo), DTAG_DISP, modeid))
	{
		DisposeDTObject(obj);
		CloseLibrary(DataTypesBase);
		return ERR_READPIC;
	}

	colormap = (SMALL_COLOR*)malloc(sizeof(SMALL_COLOR)*width*height);
	if(!colormap)
	{
		DisposeDTObject(obj);
		CloseLibrary(DataTypesBase);
		return ERR_MEM;
	}

	c = colormap;

	if((bmhd->bmh_Depth > 8) && pic24bit)
	{
		// true color picture
		buffer = (UBYTE*)malloc(3*width);
		if(!buffer)
		{
			DisposeDTObject(obj);
			CloseLibrary(DataTypesBase);
			return ERR_MEM;
		}

		for(y=0; y<height; y++)
		{
			arg.MethodID = DTM_READPIXELARRAY;
			arg.PixelArray = buffer;
			arg.PixelArrayMode = RECTFMT_RGB;
			arg.PixelArrayMod = 3*width;
			arg.LeftEdge = 0;
			arg.TopEdge = y;
			arg.Width = width;
			arg.Height = 1;
#ifdef __PPC__
			ppc_DoMethodA(obj, (Msg)&arg);
#else
			DoMethodA(obj, (Msg)&arg);
#endif

			for(x=0; x<width; x++)
			{
				c->r = buffer[x*3];
				c->g = buffer[x*3+1];
				c->b = buffer[x*3+2];
				c++;
			}
		}
		free(buffer);
	}
	else
	{
		// normal picture

		// planar to chunky conversion
		InitRastPort(&rp);
		rp.BitMap = bitmap;

		tempbitmap = AllocBitMap(width, 1, bmhd->bmh_Depth, BMF_CLEAR, NULL);
		if(!tempbitmap)
		{
			DisposeDTObject(obj);
			CloseLibrary(DataTypesBase);
			return ERR_MEM;
		}

		InitRastPort(&temprp);
		temprp.BitMap = tempbitmap;

		buffer = (UBYTE*)malloc((width+15)&(~15));
		if(!buffer)
		{
			FreeBitMap(tempbitmap);
			DisposeDTObject(obj);
			CloseLibrary(DataTypesBase);
			return ERR_MEM;
		}

		if(dispinfo.PropertyFlags & DIPF_IS_HAM)
		{
			// Simulate HAM mode
			if(bmhd->bmh_Depth == 8)
			{
				// Super HAM
				for(y=0; y<height; y++)
				{
					r = g = b = 0;
					ReadPixelLine8(&rp, 0, y, width, buffer, &temprp);
					for(x=0; x<width; x++)
					{
						color = buffer[x];
						switch(color & 0xc0)
						{
							case 0x00:  // Take it from color registers
								color *= 3;
								r = colreg[color];
								g = colreg[color+1];
								b = colreg[color+2];
								break;
							case 0x40:  // modify blue
								b = color << 2;
								break;
							case 0x80:  // modify red
								r = color << 2;
								break;
							case 0xc0:  // modify green
								g = color << 2;
								break;
						}
						c->r = r;
						c->g = g;
						c->b = b;
						c++;
					}
				}
			}
			else
			{
				// Normal HAM
				for(y=0; y<height; y++)
				{
					r = g = b = 0;
					ReadPixelLine8(&rp, 0, y, width, buffer, &temprp);
					for(x=0; x<width; x++)
					{
						color = buffer[x];
						switch(color & 0x30)
						{
							case 0x00:  // Take it from color registers
								color *= 3;
								r = colreg[color];
								g = colreg[color+1];
								b = colreg[color+2];
								break;
							case 0x40:  // modify blue
								b = color << 4;
								break;
							case 0x80:  // modify red
								r = color << 4;
								break;
							case 0xc0:  // modify green
								g = color << 4;
								break;
						}
						c->r = r;
						c->g = g;
						c->b = b;
						c++;
					}
				}
			}
		}
		else
		{
			for(y=0; y<height; y++)
			{
				ReadPixelLine8(&rp, 0, y, width, buffer, &temprp);
				for(x=0; x<width; x++)
				{
					color = buffer[x];
					color *= 3;
					c->r = colreg[color];
					c->g = colreg[color+1];
					c->b = colreg[color+2];
					c++;
				}
			}
		}
		free(buffer);
		FreeBitMap(tempbitmap);
	}

	DisposeDTObject(obj);
	CloseLibrary(DataTypesBase);
	return ERR_NONE;
}
#endif // __AMIGA__

/*************
 * DESCRIPTION:   Saves a image. Invokes the different handlers.
 * INPUT:         handler     handler name
 *                param       parameter to pass to saver
 * OUTPUT:        error code
 *************/
int IMAGE::Save(RSICONTEXT *rc, char *handler, void *param)
{
	char buf[256];
	char error[256];
	PICTURE image;
#ifdef __AMIGA__
	BPTR lock;
	char buf1[32];
	int i;
#endif

	rc->SetImageError(NULL);

	image.name = name;
	image.colormap = colormap;
	image.width = width;
	image.height = height;
	image.caller = NULL;
	image.param = param;
	image.malloc = malloc;
	image.Progress = NULL;

#ifndef __AMIGA__
	if (keyWritePicture)
	{
		strcpy(buf, WorkingDirectory);
		AddPart(buf, "modules", 256);
		AddPart(buf, "picture", 256);
		AddPart(buf, handler, 256);
		strcat(buf, ".dll");

		if (!keyWritePicture(buf, &image, error))
		{
			rc->SetImageError(error);
			return ERR_PICHANDLER;
		}
	}
	return ERR_NONE;
#else
	if(KeyfileBase)
	{
		// get directory RayStorm was started from and
		// search file from there
		// get a lock to the program directory
		lock = GetProgramDir();
		if(!NameFromLock(lock, buf, 256))
			return ERR_PICHANDLER;
		AddPart(buf, "modules/picture", 256);

		// we need lower case names because library names are case sensitve
		i=0;
		strcpy(buf1, handler);
		while(buf1[i])
		{
			if((buf1[i] >= 'A') && (buf1[i] <= 'Z'))
				buf1[i] += 'a'-'A';
			i++;
		}
		AddPart(buf, buf1, 256);
#ifdef __PPC__
		strcat(buf, "ppc");
#endif

		if(!keyWritePicture(buf, &image, error))
		{
			rc->SetImageError(error);
			return ERR_PICHANDLER;
		}
	}

	return ERR_NONE;
#endif // !__AMIGA__
}

/*************
 * DESCRIPTION:   bi-linear interp of four pixels.  Note this blends
 *                the top with the bottom, and the left with the right.
 * INPUT:         x,y      integer position
 *                fx,fy    floating point position
 *                color    interpolated color
 * OUTPUT:        none
 *************/
void IMAGE::ColorInterpolate(int x, int y, float fx, float fy, COLOR *color)
{
	int xplus, yplus, offset;
	float f1,f2,f3,f4;
	SMALL_COLOR *x0y0, *x1y0, *x0y1, *x1y1;

	// bi-linear interp of four pixels.  Note this blends
	// the top with the bottom, and the left with the right.
	if (x == width - 1)
		xplus = 1 - width;
	else
		xplus = 1;
	if (y == height - 1)
		yplus = (1 - height) * width;
	else
		yplus = width;

	offset = x + y * width;

	f1 = (1.f-fx)*(1.f-fy);
	x0y0 = &colormap[offset];
	f2 = fx*(1.f-fy);
	x1y0 = &colormap[offset+xplus];
	f3 = (1.f-fx)*fy;
	x0y1 = &colormap[offset+yplus];
	f4 = fx*fy;
	x1y1 = &colormap[offset+xplus+yplus];

	color->r = (float(x0y0->r)*f1 +
		float(x1y0->r)*f2 +
		float(x0y1->r)*f3 +
		float(x1y1->r)*f4)*3.921568627e-3f;
	color->g = (float(x0y0->g)*f1 +
		float(x1y0->g)*f2 +
		float(x0y1->g)*f3 +
		float(x1y1->g)*f4)*3.921568627e-3f;
	color->b = (float(x0y0->b)*f1 +
		float(x1y0->b)*f2 +
		float(x0y1->b)*f3 +
		float(x1y1->b)*f4)*3.921568627e-3f;
}

/*************
 * DESCRIPTION:   bi-linear interp of four pixels.  Note this blends
 *                the top with the bottom, and the left with the right.
 * INPUT:         ix,iy    integer position
 *                fx,fy    floating point position
 * OUTPUT:        interpolated value (0-1)
 *************/
float IMAGE::ImageIndex(int ix, int iy, float fx, float fy)
{
	int xplus, yplus, offset;
	float x0y0, x1y0, x0y1, x1y1;

	// bi-linear interp of four pixels.  Note this blends
	// the top with the bottom, and the left with the right.
	if (ix == width - 1)
		xplus = 1 - width;
	else
		xplus = 1;
	if (iy == height - 1)
		yplus = (1 - height) * width;
	else
		yplus = width;

	offset = ix + iy * width;
	x0y0 = float(colormap[offset].r);
	x1y0 = float(colormap[offset+xplus].r);
	x0y1 = float(colormap[offset+yplus].r);
	x1y1 = float(colormap[offset+xplus+yplus].r);
	return (float)(x0y0*(1.0-fx)*(1.0-fy) +
			  x1y0*(fx)*(1.0-fy) +
			  x0y1*(1.0-fx)*(fy) +
			  x1y1*(fx)*(fy))/255.f;
}

/*************
 * DESCRIPTION:   find image with name 'name'
 * INPUT:         imagename      name of image
 * OUTPUT:        pointer to image, NULL if non found
 *************/
IMAGE* FindImage(RSICONTEXT *rc, char *imagename)
{
	IMAGE *cur;

	cur = rc->image_root;
	while(cur && cur->name)
	{
		if(!strcmp(imagename, cur->name))
			return cur;
		cur = (IMAGE*)cur->GetNext();
	}
	return NULL;
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
BRUSH::BRUSH()
{
	image = NULL;
	flags = MAP_COLOR;
	actor = NULL;
	time = 0;
}

/*************
 * DESCRIPTION:   Apply brush to surface
 * INPUT:         obj      current object
 *                norm     surface normal
 *                surf     surface structure
 *                pos_     position of hitpoint
 *                time     current time
 * OUTPUT:        none
 *************/
void BRUSH::ApplyBrush(OBJECT *obj, VECTOR *norm, SURFACE *surf, VECTOR *pos_, const float time)
{
	float phi, theta, u, v, outval;
	VECTOR realnorm, tmp;
	SMALL_COLOR *map_color;
	int x, y;

	if((flags & WRAP_MASK) == WRAP_COORD)
	{
		obj->BrushCoords(pos_, &u, &v);

		if(flags & REPEAT)
		{
			// repeat brush
			if(flags & MIRROR)
			{
				x = int(floor(u));
				if(x&1)
					u = 1.f - (u - x);
				else
					u -= x;
				y = int(floor(v));
				if(y & 1)
					v = 1.f - (v - y);
				else
					v -= y;
			}
			else
			{
				// don't mirror
				u -= (float)floor(u);
				v -= (float)floor(v);
			}
		}
		else
		{
			// dont't repeat brush
			// test if brush out of range
			if((u > 1.f) || (u < 0.f) ||
				(v > 1.f) || (v < 0.f))
				return;
		}
	}
	else
	{
		// update brush
		if(actor && this->time != time)
			this->Update(time);

		VecSub(pos_, &pos, &tmp);

		// convert vector to brush coordinate system
		realnorm.x = dotp(&orient_x, &tmp);
		realnorm.y = dotp(&orient_y, &tmp);
		realnorm.z = dotp(&orient_z, &tmp);

		switch(flags & WRAP_MASK)
		{
			case WRAP_XY:
				// Spheremapping
				// Test if brush out of range
				if((fabs(realnorm.x) < size.x) &&
					(fabs(realnorm.y) < size.y) &&
					(fabs(realnorm.z) < size.z))
					return;

				// Point is coincident with origin
				if(VecNormalize(&realnorm) == 0.f)
					return;

				if (realnorm.y > 1.f) // roundoff
				{
					phi = PI;
				}
				else
				{
					if (realnorm.y < -1.f) // roundoff
					{
						phi = 0.f;
					}
					else
					{
						phi = (float)acos(realnorm.y);
					}
				}

				v = phi * INV_PI;

				if (fabs(v) < EPSILON || equal(v, 1.f))
				{
					u = 0.f;
				}
				else
				{
					theta = realnorm.x / (float)sin(phi);
					if (theta > 1.f)
					{
						theta = 0.f;
					}
					else
					{
						if (theta < -1.f)
						{
							theta = 0.5f;
						}
						else
						{
							theta = (float)acos(theta) * INV_TWOPI;
						}
					}

					if (realnorm.z > 0.f)
						u = 1.f - theta;
					else
						u = theta;
				}
				break;
			case WRAP_X:
			case WRAP_Y:
				// Cylindermapping round y or x
				if((flags & WRAP_MASK) == WRAP_X)
				{
					// Cylindermapping round x
					phi = realnorm.x;
					realnorm.x = realnorm.y;
					realnorm.y = phi;
				}

				if(flags & REPEAT)
				{
					// Test if brush out of range
					if((fabs(realnorm.x) > size.x) ||
						(fabs(realnorm.z) > size.z))
						return;

					// repeat brush
					if(flags & MIRROR)
					{
						v = realnorm.y / size.y;
						y = int(floor(v));
						if(y&1)
							v = 1.f - (v - y);
						else
							v -= y;
					}
					else
					{
						// don't mirror
						v = realnorm.y / size.y;
						v -= (float)floor(v);
						v = 1.f - v;
					}
				}
				else
				{
					// Test if brush out of range
					if((fabs(realnorm.x) > size.x) ||
						(realnorm.y > size.y) ||
						(realnorm.y < 0.f) ||
						(fabs(realnorm.z) > size.z))
						return;

					v = 1.f - realnorm.y / size.y;
				}
				realnorm.x /= (float)sqrt(realnorm.x*realnorm.x + realnorm.z*realnorm.z);

				if (fabs(realnorm.x) > 1.f)
				{
					u = 0.5f;
				}
				else
				{
					u = (float)acos(realnorm.x) * INV_TWOPI;
				}
				if (realnorm.z > 0.f)
					u = 1.f - u;

				break;
			default:
				// flat mapping
				if(flags & REPEAT)
				{
					// repeat brush
					if(flags & MIRROR)
					{
						u = realnorm.x / size.x;
						x = int(floor(u));
						if(x&1)
							u = 1.f - (u - x);
						else
							u -= x;
						v = realnorm.y / size.y;
						y = int(floor(v));
						if(y & 1)
							v = 1.f - (v - y);
						else
							v -= y;
					}
					else
					{
						// don't mirror
						u = realnorm.x / size.x;
						u -= (float)floor(u);
						u = 1.f - u;
						v = realnorm.y / size.y;
						v -= (float)floor(v);
						v = 1.f - v;
					}
				}
				else
				{
					// dont't repeat brush
					// test if brush out of range
					if((fabs(realnorm.x) > size.x) ||
						(fabs(realnorm.y) > size.y) ||
						(fabs(realnorm.z) > size.z))
						return;

					u = 1.f - ((realnorm.x+size.x) / (2.f*size.x));
					v = 1.f - ((realnorm.y+size.y) / (2.f*size.y));
				}
				break;
		}

		if((u < 0.f) || (v < 0.f))
			return;
	}

	u = 1.f - u;
	u *= (float)(image->width);
	v *= (float)(image->height);
	x = (int)(floor(u));
	y = (int)(floor(v));
	u -= float(x);
	v -= float(y);
	switch(flags & MAP_MASK)
	{
		case MAP_COLOR:
			if(flags & SOFT)
			{
				// soft color interpolation
				image->ColorInterpolate(x, y, u, v, &surf->diffuse);
			}
			else
			{
				map_color = &image->colormap[y*image->width + x];
				surf->diffuse.r = float(map_color->r)*3.921568627e-3f;
				surf->diffuse.g = float(map_color->g)*3.921568627e-3f;
				surf->diffuse.b = float(map_color->b)*3.921568627e-3f;
			}
			break;
		case MAP_REFLECTIFITY:
			if(flags & SOFT)
			{
				// soft color interpolation
				image->ColorInterpolate(x,y,u,v,&surf->reflect);
			}
			else
			{
				map_color = &image->colormap[y*image->width + x];
				surf->reflect.r = float(map_color->r)*3.921568627e-3f;
				surf->reflect.g = float(map_color->g)*3.921568627e-3f;
				surf->reflect.b = float(map_color->b)*3.921568627e-3f;
			}
			surf->diffuse.r *= 1.f - surf->reflect.r;
			surf->diffuse.g *= 1.f - surf->reflect.g;
			surf->diffuse.b *= 1.f - surf->reflect.b;
			break;
		case MAP_FILTER:
			if(flags & SOFT)
			{
				// soft color interpolation
				image->ColorInterpolate(x,y,u,v,&surf->transpar);
				image->ColorInterpolate(x,y,u,v,&surf->difftrans);
			}
			else
			{
				map_color = &image->colormap[y*image->width + x];
				surf->transpar.r = float(map_color->r)*3.921568627e-3f;
				surf->transpar.g = float(map_color->g)*3.921568627e-3f;
				surf->transpar.b = float(map_color->b)*3.921568627e-3f;
				surf->difftrans = surf->transpar;
			}
			surf->diffuse.r *= 1.f - surf->transpar.r;
			surf->diffuse.g *= 1.f - surf->transpar.g;
			surf->diffuse.b *= 1.f - surf->transpar.b;
			break;
		case MAP_ALTITUDE:
			outval = image->ImageIndex(x, y, u, v);
			MakeBump(norm, &orient_x, &orient_y,
				image->ImageIndex((x == image->width - 1) ? 0 : x+1, y, u, v) - outval,
				image->ImageIndex(x, (y == image->height - 1) ? 0 : y+1, u, v) - outval);
			break;
		case MAP_SPECULAR:
			if(flags & SOFT)
			{
				// soft color interpolation
				image->ColorInterpolate(x,y,u,v,&surf->specular);
			}
			else
			{
				map_color = &image->colormap[y*image->width + x];
				surf->specular.r = float(map_color->r)*3.921568627e-3f;
				surf->specular.g = float(map_color->g)*3.921568627e-3f;
				surf->specular.b = float(map_color->b)*3.921568627e-3f;
			}
			break;
	}
}

/*************
 * DESCRIPTION:   Update brush parameters
 * INPUT:         time     current time
 * OUTPUT:        none
 *************/
void BRUSH::Update(const float time)
{
	TIME t;
	VECTOR a;

	if((actor->time.begin != this->time) || (actor->time.end != time))
	{
		t.begin = this->time;
		t.end = time;
		actor->Animate(&t);
	}
	actor->matrix->MultVectMat(&pos);

	a = actor->act_align;
	VecAdd(&a,&align,&a);
	CalcOrient(&a, &orient_x, &orient_y, &orient_z);
	this->time = time;
}
