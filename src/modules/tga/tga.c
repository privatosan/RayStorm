/***********
 * MODULE:        tga
 * NAME:          tga.c
 * DESCRIPTION:   routines to read and write TARGA (TGA) files
 *                source borrowed from targa.c (POVRAY)
 * AUTHORS:       PovRay-Team, Mike Hesser, Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    09.01.96 ah    initial release
 *    19.10.96 mh    speed up
 *    14.03.97 mh    bugfix: reading of color map
 *    11.10.97 mh    no tga.h anymore
 *    25.10.97 ah    removed picInit(), picCleanup(). Made it thread-save
 ***********/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef PICLOADER_H
#include "picload.h"
#endif

typedef struct
{
	UBYTE Red, Green, Blue, Filter;
} IMAGE_COLOR;

#ifdef __AMIGA__
	#include <exec/memory.h>
	#include <pragma/dos_lib.h>
	#include <pragma/exec_lib.h>

	#ifdef __PPC__
	#include "piclib.h"
	#pragma libbase PicHandBase
	extern "ASM" void *_picWrite(void *arg1);
	extern "ASM" void *__saveds _PPCStub__ppc_picWrite(long *args)
	{
		return _picWrite(
			(void *) args[0]
		);
	}
	#endif
	#define SAVEDS __saveds
#else
	#define SAVEDS
#endif

enum errnums
{
	ERR_MEM, ERR_CREATE, ERR_WRITE, ERR_READ, ERR_MANGLED
};

static char *errors[] =
{
	"Not enough memory",
	"Can't create tga file",
	"Error writing tga file",
	"Error reading tga file",
	"Error mangled tga file"
};

typedef struct
{
	SMALL_COLOR *cmap;
	FILE *file;
} HANDLER_DATA;

static void readTGACleanup(HANDLER_DATA*);

/**********
 * DESCRIPTION:   converts a color (from 8,16,24,32 Bit)
 * INPUT:         cmap        color map
 *                tcolor      color is written there
 *                pixelsize   depth of pixel
 *                bytes       source from which to convert
 *                count       number of colors to transform
 * OUTPUT:        -
 **********/
static void ConvertTargaColor(SMALL_COLOR *cmap, SMALL_COLOR *tcolor, int pixelsize, UBYTE *bytes, int count)
{
	int i,j;
	j = 0;
	
	switch (pixelsize)
	{
		case 1:
			for (i = 0; i < count; i++)
			{
				tcolor[i].r = cmap[bytes[i]].r;
				tcolor[i].g = cmap[bytes[i]].g;
				tcolor[i].b = cmap[bytes[i]].b;
//            tcolor[i].Filter = 0;
			}
			break;
		case 2:
			for (i = 0; i < count; i++)
			{
				tcolor[i].r = ((bytes[j+1] & 0x7c) << 1);
				tcolor[i].g = (((bytes[j+1] & 0x03) << 3) | ((bytes[j] & 0xe0) >> 5)) << 3;
				tcolor[i].b = (bytes[j] & 0x1f) << 3;
//            tcolor[i].Filter = (bytes[j+1] & 0x80 ? 255 : 0);
				j += 2;
			}
			break;
		case 3:
			for (i = 0; i < count; i++)
			{
				tcolor[i].b = bytes[j++];
				tcolor[i].g = bytes[j++];
				tcolor[i].r = bytes[j++];
//            tcolor[i].Filter = 0;
			}
			break;
		case 4:
			for (i = 0; i < count; i++)
			{
				tcolor[i].b = bytes[j++];
				tcolor[i].g = bytes[j++];
				tcolor[i].r = bytes[j++];
//            tcolor[i].Filter = bytes[j++];
			}
			break;
	}
}

/*************
 * DESCRIPTION:   Reads a Targa image into an RGB image buffer.
 *                Handles 8, 16, 24, 32 bit formats.  Raw or color mapped.
 *                Simple raster and RLE compressed pixel encoding.
 *                Right side up or upside down orientations.
 *                taken form targa.c (POVRAY)
 * INPUT:         image       pointer to image structure
 * OUTPUT:        NULL if ok, else error string
 *************/
char* SAVEDS picRead(PICTURE *image)
{
	SMALL_COLOR pixel, *colormap;
	UBYTE cflag, bytes[4];
	UBYTE idbuf[256], tgaheader[18];
	int h, i, j;
	ULONG ftype, idlen, cmlen, cmsiz, psize, orien;
	USHORT width, height;
	UBYTE *tempbuf;
	HANDLER_DATA data;

	data.cmap = NULL;

	// Start by trying to open the file
	if ((data.file = fopen(image->name, "rb")) == NULL)
	{
		readTGACleanup(&data);
		return errors[ERR_CREATE];
	}
	if (fread(tgaheader, 18, 1, data.file) != 1)
	{
		readTGACleanup(&data);
		return errors[ERR_READ];
	}

	// Decipher the header information
	idlen = tgaheader[ 0];
	ftype = tgaheader[ 2];
	cmsiz = tgaheader[ 7] / 8;
	cmlen = tgaheader[ 5] + (tgaheader[ 6] << 8);
	width = tgaheader[12] + (tgaheader[13] << 8);
	height= tgaheader[14] + (tgaheader[15] << 8);
	psize = tgaheader[16] / 8;
	orien = tgaheader[17] & 0x20; // Right side up ?

	colormap = (SMALL_COLOR*)image->malloc(sizeof(SMALL_COLOR)*width*height);
	if(!colormap)
	{
		readTGACleanup(&data);
		return errors[ERR_MEM];
	}
	memset(colormap, 0, sizeof(SMALL_COLOR)*width*height);
	image->colormap = colormap;
	image->width = width;
	image->height = height;

	// Determine if this is a supported Targa type
	if (ftype == 9 || ftype == 10)
		cflag = 1;
	else
	{
		if (ftype == 1 || ftype == 2 || ftype == 3)
			cflag = 0;
		else
		{
			readTGACleanup(&data);
			return errors[ERR_READ];
		 }
	}

	// Skip over the picture ID information
	if (idlen > 0 && fread(idbuf, idlen, 1, data.file) != 1)
	{
		readTGACleanup(&data);
		return errors[ERR_READ];
	}

	if (cmlen == 0 && psize == 1)
	{
		readTGACleanup(&data);
		return errors[ERR_MANGLED];
	}
	
	// Read in the the color map (if any)
	if (cmlen > 0)
	{
		if (psize != 1)
		{
			readTGACleanup(&data);
			return errors[ERR_MANGLED];
		}
		data.cmap = malloc(sizeof(IMAGE_COLOR)*cmlen);
		if (!data.cmap)
		{
			readTGACleanup(&data);
			return errors[ERR_MEM];
		}

		for (i = 0; i < (int)cmlen; i++)
		{
			if (fread(bytes, cmsiz, 1, data.file) != 1)
			{
				readTGACleanup(&data);
				return errors[ERR_READ];
			}
			ConvertTargaColor(data.cmap, &data.cmap[i], cmsiz, bytes, 1);
		}
	}

	// Read the image into the buffer
	if (cflag)
	{
		i = 0; // row counter
		j = 0; // column counter

		while (i < height)
		{
			if (!orien)
				colormap = image->colormap + (height - i - 1)*width;
			else
				colormap = image->colormap + i*width;

			// Grab a header
			if ((h = fgetc(data.file)) == EOF)
			{
				readTGACleanup(&data);
				return errors[ERR_READ];
			}
			// compression bit set ?
			if (h & 0x80)
			{
				// Repeat buffer
				h &= 0x7F;

				if (fread(bytes, 1, psize, data.file) != psize)
				{
					readTGACleanup(&data);
					return errors[ERR_READ];
				}
				if (cmlen == 0)
					ConvertTargaColor(data.cmap, &pixel, psize, bytes, 1);
				for (; h >= 0; h--)
				{
					*colormap = pixel;
					colormap++;
					if (++j == width)
					{
						i++;
						j = 0;
					}
				}
			}
			else
			{
				// Copy buffer
				for (; h >= 0; h--)
				{
					if (fread(bytes, 1, psize, data.file) != psize)
					{
						readTGACleanup(&data);
						return errors[ERR_READ];
					}
					ConvertTargaColor(data.cmap, colormap, psize, bytes, 1);
					if (++j == width)
					{
						i++;
						j = 0;
					}
				}
			}
			if (image->Progress)
				image->Progress(image->caller, (float)i/(float)(height - 1));
		}
	}
	else
	{
		tempbuf = (UBYTE*)malloc(width*psize);
		if (!tempbuf)
		{
			readTGACleanup(&data);
			return errors[ERR_MEM];
		}
		// Simple raster image file, read in all of the pixels
		for (i = 0; i < height; i++)
		{
			if (!orien)
				colormap = image->colormap + (height - i - 1)*width;
			else
				colormap = image->colormap + i*width;

			if (fread(tempbuf, psize, width, data.file) != width)
			{
				readTGACleanup(&data);
				return errors[ERR_READ];
			}
			ConvertTargaColor(data.cmap, colormap, psize, tempbuf, width);
			if (image->Progress)
				image->Progress(image->caller, (float)i/(float)(height - 1));
		}
		free(tempbuf);
	}
	// Any data following the image is ignored.

	// Close the image file
	readTGACleanup(&data);
	return NULL;
}

/**********
 * DESCRIPTION:   free tga-handle
 * INPUT:         data     handler data
 * OUTPUT:        -
 **********/
static void readTGACleanup(HANDLER_DATA *data)
{
	if (data->file)
		fclose(data->file);
	if (data->cmap)
		free(data->cmap);
}

/*************
 * DESCRIPTION:   write 24Bit TGA-file
 * INPUT:         image       pointer to image structure
 * OUTPUT:        NULL if ok else error string
 *************/
char *picWrite(PICTURE *image)
{
	FILE  *file;
	int   i, x, y;
	UBYTE *linebuf;
	UBYTE *line;

	file = fopen(image->name, "wb");
	if (!file)
	{
		return errors[ERR_CREATE];
	}

	// 00, 00, 02, then 7 00's...
	for (i = 0; i < 10; i++)
	{
		if (i == 2)
			putc(i, file);
		else
			putc(0, file);
	}

	// y origin set to "First_Line"
	putc(0, file);
	putc(0, file);

	putc(image->width % 256, file);  // write width and height
	putc(image->width / 256, file);
	putc(image->height % 256, file);
	putc(image->height / 256, file);
	putc(24, file);  // 24 bits/pixel
	putc(32, file);  // Bitmask, pertinent bit: top-down raster

	linebuf = malloc(sizeof(UBYTE)*3*image->width);

	if (!linebuf)
	{
		fclose(file);
		return errors[ERR_MEM];
	}

	line = (UBYTE *)image->colormap;
	for (y = 0; y < image->height; y++)
	{
		i = 0;
		for (x = 0; x < image->width; x++)
		{
			linebuf[i++] = line[2];
			linebuf[i++] = line[1];
			linebuf[i++] = line[0];
			line+= 4;
		}
		fwrite(linebuf, 3, image->width, file);
		if (image->Progress)
			image->Progress(image->caller, (float)y/(float)(image->height - 1));
	}
	free(linebuf);
	fclose(file);

	return NULL;
}
