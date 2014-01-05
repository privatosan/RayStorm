/***************
 * NAME:          loader.cpp
 * DESCRIPTION:   Functions for loading images
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.08.98 ah    initial release
 ***************/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <graphics/gfx.h>
#include <exec/memory.h>
#include <cybergraphics/cybergraphics.h>
#include <clib/alib_protos.h>
#include <pragma/dos_lib.h>
#include <pragma/exec_lib.h>
#include <pragma/graphics_lib.h>

#ifndef IMAGE_H
#include "loader.h"
#endif

#ifndef PICLOAD_H
#include "picload.h"
#endif

#include "rmod:pichand_lib.h"
struct Library *PicHandBase;

#ifdef __PPC__
// need this stub because PPC always uses indirect addressing, __saveds
// sets up the neccessary stuff
void* __saveds PPC_malloc(size_t size)
{
	return malloc(size);
}
#endif

void Cleanup(IMAGE *image)
{
	free(image->colormap);
}

/*************
 * DESCRIPTION:   Loads a image. Invokes the different handlers for the
 *    picture types. Checks the first 16 Byte of the image file.
 * INPUT:         filename    name of picture to load (includes path)
 *                colormap
 * OUTPUT:        FALSE if failed
 *************/
BOOL Load(char *name, IMAGE *image)
{
	FILE *typefile;
	FILE *imagefile;
	char typebuf[18];
	char imagebuf[16];
	PICTURE pic;
	char handlername[32];
	BPTR lock;
	int i;
	BOOL identok=FALSE;
	UWORD mask;

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
	// Amiga-version: get directory RayStorm was started from and
	// search file from there
	// get a lock to the program directory
	lock = GetProgramDir();
	// make program directory to current directory
	lock = CurrentDir(lock);
	typefile = fopen("modules/picture/types","rb");
	// change current dir to old dir
	lock = CurrentDir(lock);
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
		pic.name = name;
		pic.caller = NULL;
		pic.param = NULL;
#ifdef __PPC__
		pic.malloc = PPC_malloc;
#else
		pic.malloc = malloc;
#endif
		pic.Progress = NULL;

		// invoke handler
		if(picRead(&pic))
		{
			CloseLibrary(PicHandBase);
			return FALSE;
		}
		else
		{
			image->width = pic.width;
			image->height = pic.height;
			image->colormap = pic.colormap;
		}

		CloseLibrary(PicHandBase);
	}
	else
	{
	  return FALSE;
	}
	return TRUE;
}

