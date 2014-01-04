/***************
 * NAME:          image.cpp
 * DESCRIPTION:   Functions for loading images
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    09.12.95 ah    initial release
 ***************/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __AMIGA__
#include <graphics/gfx.h>
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
#ifdef __PPC__
#include "m68k_stubs.h"
#endif // __PPC__
#endif

#ifndef IMAGE_H
#include "image.h"
#endif

#ifndef PICLOAD_H
#include "picload.h"
#endif

#ifndef FILEUTIL_H
#include "fileutil.h"
#endif

#ifndef __AMIGA__
extern char WorkingDirectory[256];
typedef char *(*READWRITEPROC)(PICTURE *);
#else
#include "rmod:pichand_lib.h"
struct Library *DataTypesBase;
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
 * OUTPUT:        FALSE if failed
 *************/
BOOL IMAGE::Load(char *filename, char *path)
{
	char name[256];
	FILE *typefile;
	FILE *imagefile;
	char typebuf[18];
	char imagebuf[16];
	PICTURE pic;
#ifdef __AMIGA__
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
	if(!ExpandPath(path, filename, name))
	{
		/* cannot open picture */
		return FALSE;
	}

	/* open image and read first 16 Bytes */
	imagefile = fopen(name,"rb");
	if(!imagefile)
		return FALSE;
	if(fread(imagebuf,1,16,imagefile) != 16)
	{
		fclose(imagefile);
		return FALSE;
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
		return FALSE;
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
			return FALSE;
		}
#ifdef __SWAP__
		mask = SwapW(mask);
#endif
		if(fread(typebuf,1,16,typefile) != 16)
		{
			fclose(typefile);
			return FALSE;
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
			return FALSE;
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

		// Load picture handler
		PicHandBase = OpenLibrary(handlername, 2L);
		if(!PicHandBase)
			return FALSE;

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
			return FALSE;
		if (!(picRead = (READWRITEPROC)GetProcAddress(hPicLib, "picRead_")))
			return FALSE;
#endif
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
		if(picRead(&pic))
		{
#ifdef __AMIGA__
			// failed to load it with own modules -> try it with datatypes
			if(!LoadAsDatatype(name))
			{
				// can't load it even with datatypes
				CloseLibrary(PicHandBase);
				return FALSE;
			}
#else
			FreeLibrary(hPicLib);
			return FALSE;
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
#else
		FreeLibrary(hPicLib);
#endif // __AMIGA__
	}
	else
	{
#ifdef __AMIGA__
		// there is no module for this picture type -> try it with datatypes
		if(!LoadAsDatatype(name))
		{
			// can't load it even with datatypes
			return FALSE;
		}
#else
		return FALSE;
#endif
	}
	return TRUE;
}

#ifdef __AMIGA__
/*************
 * DESCRIPTION:   Loads a image with the datatypes.
 * INPUT:         filename    name of picture to load
 * OUTPUT:        FALSE if failed
 *************/
BOOL IMAGE::LoadAsDatatype(char *filename)
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
		return FALSE;

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
			return FALSE;
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
			return FALSE;
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
		return FALSE;
	}

	if(!bmhd || !bitmap)
	{
		DisposeDTObject(obj);
		CloseLibrary(DataTypesBase);
		return FALSE;
	}

	width = bmhd->bmh_Width;
	height = bmhd->bmh_Height;

	if(!GetDisplayInfoData(NULL, (UBYTE*)&dispinfo, sizeof(struct DisplayInfo), DTAG_DISP, modeid))
	{
		DisposeDTObject(obj);
		CloseLibrary(DataTypesBase);
		return FALSE;
	}

	colormap = (SMALL_COLOR*)malloc(sizeof(SMALL_COLOR)*width*height);
	if(!colormap)
	{
		DisposeDTObject(obj);
		CloseLibrary(DataTypesBase);
		return FALSE;
	}

	c = colormap;

	if((bmhd->bmh_Depth > 8) && pic24bit)
	{
		// true color picture
		buffer = (UBYTE*)AllocVec(3*width, MEMF_PUBLIC);
		if(!buffer)
		{
			DisposeDTObject(obj);
			CloseLibrary(DataTypesBase);
			return FALSE;
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
		FreeVec(buffer);
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
			return FALSE;
		}

		InitRastPort(&temprp);
		temprp.BitMap = tempbitmap;

		buffer = (UBYTE*)AllocVec((width+15)&(~15), MEMF_PUBLIC);
		if(!buffer)
		{
			FreeBitMap(tempbitmap);
			DisposeDTObject(obj);
			CloseLibrary(DataTypesBase);
			return FALSE;
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
		FreeVec(buffer);
		FreeBitMap(tempbitmap);
	}

	DisposeDTObject(obj);
	CloseLibrary(DataTypesBase);
	return TRUE;
}
#endif
