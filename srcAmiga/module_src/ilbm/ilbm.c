/***************
 * MODUL:         ILBM-handler
 * NAME:          ilbm.c
 * DESCRIPTION:   In this module are all the functions to read and write
 *    IFF-ILBM-files
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * BUGS:          none
 * TO DO:
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.11.95 ah    initial release
 *    14.04.96 ah    made chunky2planar() and planar2chunky() a little faster
 *    14.04.97 ah    now 10 lines at once are written
 *    26.10.97 ah    Removed picInit() and picCleanup(). Added progress and made it threadsave
 ***************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef __AMIGA__
	#include "iffparse.h"
	#define SAVEDS
#else
	#include <utility/tagitem.h>
	#include <libraries/iffparse.h>
	#include <pragma/iffparse_lib.h>
	#include <pragma/dos_lib.h>

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
#endif

#ifndef ILBM_H
#include "ilbm.h"
#endif

enum errnums
{
	ERR_MEM,ERR_OPEN,ERR_ILBMFILE,ERR_ILBMDEPTH,ERR_ILBMWRITE
};

static char *errors[] =
{
	"Not enough memory",
	"Can't open image file",
	"Error reading ILBM file",
	"Only ILBM files with 24 planes are supported",
	"Error writing ILBM file"
};

typedef struct
{
	struct IFFHandle *iff;
	BitMapHeader *bmhd;
	UBYTE *row;
	UBYTE *buffer;
} HANDLER_DATA;

static LONG propchks[] =
{
	ID_ILBM, ID_BMHD,
	ID_ILBM, ID_BODY,
};

static UBYTE mask[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

static void Planar2Chunky(SMALL_COLOR *, UBYTE *, int, int);
static ULONG unpackrow(UBYTE *, UBYTE *, ULONG );
static void Chunky2Planar(SMALL_COLOR *, UBYTE *, int , int);
static ULONG packrow(UBYTE *, UBYTE *, ULONG );

/*************
 * DESCRIPTION:   free mem and iff-handle
 * INPUT:         data     handler data
 * OUTPUT:        none
 *************/
static void HandlerCleanup(HANDLER_DATA *data)
{
	if(data->bmhd)
		free(data->bmhd);
	if(data->row)
		free(data->row);
	if(data->buffer)
		free(data->buffer);
	if(data->iff)
	{
		CloseIFF(data->iff);
		if(data->iff->iff_Stream)
			Close(data->iff->iff_Stream);
		FreeIFF(data->iff);
	}
}

/*************
 * DESCRIPTION:   Read IFF-file
 * INPUT:         image       pointer to image structure
 * OUTPUT:        NULL if ok, else error string
 *************/
char* SAVEDS picRead(PICTURE *image)
{
	BitMapHeader *bmhd;
	struct StoredProperty *sp;
	UBYTE *body, *tmpbody, compress;
	LONG error;
	ULONG y, p, from;
	int bytes;
	SMALL_COLOR *colormap;
	HANDLER_DATA data;

	data.row = NULL;
	data.buffer = NULL;
	data.iff = NULL;
	data.bmhd = NULL;

	data.iff = AllocIFF();
	if (!data.iff)
	{
		HandlerCleanup(&data);
		return errors[ERR_MEM];
	}

#ifdef __AMIGA__
	data.iff->iff_Stream = Open(image->name, MODE_OLDFILE);
#else
	data.iff->iff_Stream = Open_(image->name, MODE_OLDFILE);
#endif
	if (!(data.iff->iff_Stream))
	{
		HandlerCleanup(&data);
		return errors[ERR_OPEN];
	}

	InitIFFasDOS(data.iff);
	error = OpenIFF(data.iff, IFFF_READ);
	if (error)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMFILE];
	}

	error = PropChunks(data.iff, propchks, 2);
	if (error)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMFILE];
	}
	error = StopOnExit(data.iff, ID_ILBM, ID_FORM);
	if (error)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMFILE];
	}
	error = ParseIFF(data.iff, IFFPARSE_SCAN);
	if (error != IFFERR_EOF && error != IFFERR_EOC)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMFILE];
	}

	sp = FindProp(data.iff, ID_ILBM, ID_BMHD);
	if (!sp)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMFILE];
	}
	bmhd = (BitMapHeader*)sp->sp_Data;
	// Only images with 24planes are supported
	if(bmhd->nPlanes != 24)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMDEPTH];
	}
	compress = bmhd->compression;

#ifdef __SWAP__
	bmhd->w = SwapW(bmhd->w);
	bmhd->h = SwapW(bmhd->h);
#endif
	colormap = (SMALL_COLOR*)image->malloc(sizeof(SMALL_COLOR)*bmhd->w*bmhd->h);
	if(!colormap)
	{
		HandlerCleanup(&data);
		return errors[ERR_MEM];
	}
	memset(colormap,0,sizeof(SMALL_COLOR)*bmhd->w*bmhd->h);
	image->colormap = colormap;
	image->width = bmhd->w;
	image->height = bmhd->h;

	sp = FindProp(data.iff, ID_ILBM, ID_BODY);
	if (!sp)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMFILE];
	}
	bytes = RowBytes(image->width);
	if (compress == cmpByteRun1)
	{
		// get memory for one row
		tmpbody = (UBYTE*)malloc(sizeof(UBYTE)*bytes*24);
		if (!tmpbody)
		{
			HandlerCleanup(&data);
			return errors[ERR_MEM];
		}
		from = 0;
	}
	else
	{
		body = (UBYTE*)sp->sp_Data;
	}
	for(y=image->height; y>0; y--)
	{
		if (compress == cmpByteRun1)
		{
			// decompress row
			for(p=0; p<24; p++)
			{
				from += unpackrow(&((UBYTE*)sp->sp_Data)[from], &tmpbody[p*bytes], bytes);
			}
			Planar2Chunky(colormap,tmpbody,image->width,bytes);
		}
		else
		{
			Planar2Chunky(colormap,body,image->width,bytes);
			body += 24*bytes;
		}
		colormap += image->width;
		if(image->Progress)
		{
			// call it only every 10th line
			if((y % 10) == 0)
				image->Progress(image->caller, 1.f - (float)y/(float)image->height);
		}
	}
	if (compress == cmpByteRun1)
	{
		// free buffer
		free(tmpbody);
	}
	HandlerCleanup(&data);

	return NULL;
}

/*************
 * DESCRIPTION:   Convert 24 planes to 24Bit chunky
 * INPUT:         colormap    pointer to chunky array (r,g,b - Bytes)
 *                row         pointer to planar buffer for one row
 *                width       width of one row
 *                bytes       bytes per row (one plane)
 * OUTPUT:        none
 *************/
static void Planar2Chunky(SMALL_COLOR *colormap, UBYTE *row, int width, int bytes)
{
	register int x;
	register UBYTE m;
	register UBYTE *r;

	for(x=0; x<width; x++)
	{
		r = row + (x>>3);
		m = mask[7-(x&0x7)];

		// red byte
		if(*r & m)
			colormap->r |= 0x01;
		r += bytes;
		if(*r & m)
			colormap->r |= 0x02;
		r += bytes;
		if(*r & m)
			colormap->r |= 0x04;
		r += bytes;
		if(*r & m)
			colormap->r |= 0x08;
		r += bytes;
		if(*r & m)
			colormap->r |= 0x10;
		r += bytes;
		if(*r & m)
			colormap->r |= 0x20;
		r += bytes;
		if(*r & m)
			colormap->r |= 0x40;
		r += bytes;
		if(*r & m)
			colormap->r |= 0x80;
		r += bytes;

		// green byte
		if(*r & m)
			colormap->g |= 0x01;
		r += bytes;
		if(*r & m)
			colormap->g |= 0x02;
		r += bytes;
		if(*r & m)
			colormap->g |= 0x04;
		r += bytes;
		if(*r & m)
			colormap->g |= 0x08;
		r += bytes;
		if(*r & m)
			colormap->g |= 0x10;
		r += bytes;
		if(*r & m)
			colormap->g |= 0x20;
		r += bytes;
		if(*r & m)
			colormap->g |= 0x40;
		r += bytes;
		if(*r & m)
			colormap->g |= 0x80;
		r += bytes;

		// blue byte
		if(*r & m)
			colormap->b |= 0x01;
		r += bytes;
		if(*r & m)
			colormap->b |= 0x02;
		r += bytes;
		if(*r & m)
			colormap->b |= 0x04;
		r += bytes;
		if(*r & m)
			colormap->b |= 0x08;
		r += bytes;
		if(*r & m)
			colormap->b |= 0x10;
		r += bytes;
		if(*r & m)
			colormap->b |= 0x20;
		r += bytes;
		if(*r & m)
			colormap->b |= 0x40;
		r += bytes;
		if(*r & m)
			colormap->b |= 0x80;

		colormap++;
	}
}

/*************
 * DESCRIPTION:   Unpack data packed with "cmpByteRun1" run compression.
 *                control bytes:
 *                [0..127]   : followed by n+1 bytes of data.
 *                [-1..-127] : followed by byte to be repeated (-n)+1 times.
 *                -128       : NOOP.
 * INPUT:         source      pointer to sourcebuffer
 *                dest        pointer to destinationbuffer
 *                bytes       amount of bytes of one row
 * OUTPUT:        amount of bytes read from source
 *************/
static ULONG unpackrow(UBYTE *source, UBYTE *dest, ULONG bytes)
{
	register ULONG from=0, to=0;

	while(to<bytes)
	{
		if(source[from] < 0x80)
		{
			// copy next n+1 bytes
			memcpy(&dest[to], &source[from+1], source[from]+1);
			to += source[from]+1;
			from += source[from++]+1;
		}
		else
		{
			if(source[from] != 0x80)
			{
				// repeat next (-n)+1 bytes
				memset(&dest[to], source[from+1], 255-source[from]+2);
				to += 255-source[from]+2;
				from += 2;
			}
			else
			{
				// else do nothing
				from++;
			}
		}
	}
	return from;
}

/*************
 * DESCRIPTION:   write 24Bit ILBM-file
 * INPUT:         image       pointer to image structure
 * OUTPUT:        NULL if ok else error string
 *************/
char *picWrite(PICTURE *image)
{
	long error;
	long length;
	int y,y1, i;
	int bytes;
	SMALL_COLOR *scrarray;
	HANDLER_DATA data;

	data.row = NULL;
	data.buffer = NULL;
	data.iff = NULL;
	data.bmhd = NULL;

	// Allocate IFF_File structure.
	data.iff = AllocIFF();
	if(!data.iff)
	{
		HandlerCleanup(&data);
		return errors[ERR_MEM];
	}

	// Set up IFF_File for AmigaDOS I/O.
#ifdef __AMIGA__
	data.iff->iff_Stream = Open(image->name, MODE_NEWFILE);
#else
	data.iff->iff_Stream = Open_(image->name, MODE_NEWFILE);
#endif

	if(!data.iff->iff_Stream)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMWRITE];
	}
	InitIFFasDOS(data.iff);

	// Start the IFF transaction.
	error = OpenIFF(data.iff, IFFF_WRITE);
	if(error)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMWRITE];
	}

	error = PushChunk(data.iff, ID_ILBM, ID_FORM, IFFSIZE_UNKNOWN);
	if(error)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMWRITE];
	}

	error = PushChunk(data.iff, ID_ILBM, ID_BMHD, IFFSIZE_UNKNOWN);
	if(error)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMWRITE];
	}

	data.bmhd = (BitMapHeader*)malloc(sizeof(BitMapHeader));
	if(!data.bmhd)
	{
		HandlerCleanup(&data);
		return errors[ERR_MEM];
	}
#ifdef __SWAP__
	data.bmhd->w = (signed short)SwapW(image->width);
	data.bmhd->h = (signed short)SwapW(image->height);
#else
	data.bmhd->w = (signed short)image->width;
	data.bmhd->h = (signed short)image->height;
#endif
	data.bmhd->x = 0;
	data.bmhd->y = 0;
	data.bmhd->nPlanes = 24;
	data.bmhd->masking = mskNone;
	data.bmhd->compression = cmpByteRun1;
	data.bmhd->flags = 0;
	data.bmhd->transparentColor = 0;
	data.bmhd->xAspect = 1;
	data.bmhd->yAspect = 1;
#ifdef __SWAP__
	data.bmhd->pageWidth = (signed short)SwapW(image->width);
	data.bmhd->pageHeight = (signed short)SwapW(image->height);
#else
	data.bmhd->pageWidth = (signed short)image->width;
	data.bmhd->pageHeight = (signed short)image->height;
#endif
	error = WriteChunkBytes(data.iff, data.bmhd, sizeof(BitMapHeader));
	if(error<0)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMWRITE];
	}

	error = PopChunk(data.iff);
	if(error)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMWRITE];
	}

	error = PushChunk(data.iff, ID_ILBM, ID_BODY, IFFSIZE_UNKNOWN);
	if(error)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMWRITE];
	}

	// get memory for one row
	bytes = RowBytes(image->width);
	data.row = (UBYTE*)malloc(sizeof(UBYTE)*bytes*24);
	if(!data.row)
	{
		HandlerCleanup(&data);
		return errors[ERR_MEM];
	}
	data.buffer = (UBYTE*)malloc(sizeof(UBYTE)*bytes*48*10);
	if(!data.buffer)
	{
		HandlerCleanup(&data);
		return errors[ERR_MEM];
	}

	scrarray = image->colormap;
	for(y=0; y<image->height; y+=10)
	{
		length = 0;
		for(y1=0; (y1<10) && (y+y1<image->height); y1++)
		{
			// clear row
			memset(data.row, 0, bytes*24);
			Chunky2Planar(scrarray, data.row, image->width, bytes);
			scrarray += image->width;
			for(i=0; i<24; i++)
				length += packrow(&data.row[i*bytes], &data.buffer[length], bytes);
		}
		error = WriteChunkBytes(data.iff, (APTR)data.buffer, length);
		if(error<0)
		{
			HandlerCleanup(&data);
			return errors[ERR_ILBMWRITE];
		}
		if(image->Progress)
			image->Progress(image->caller, (float)y/(float)image->height);
	}

	error = PopChunk(data.iff);
	if(error)
	{
		HandlerCleanup(&data);
		return errors[ERR_ILBMWRITE];
	}

	HandlerCleanup(&data);
	return NULL;
}

/*************
 * DESCRIPTION:   Convert 24Bit chunky to 24 planes
 * INPUT:         scrarray    pointer to chunky array (r,g,b - Bytes)
 *                row         pointer to planar buffer for one row
 *                width       width of one row
 *                bytes       bytes per row (one plane)
 * OUTPUT:        none
 *************/
static void Chunky2Planar(SMALL_COLOR *scrarray, UBYTE *row, int width, int bytes)
{
	register int x;
	register UBYTE m;
	register UBYTE *r;

	for(x=0; x<width; x++)
	{
		r = row + (x>>3);
		m = mask[7 - (x & 0x7)];

		// red byte
		if(scrarray->r & 0x01)
			*r |= m;
		r += bytes;
		if(scrarray->r & 0x02)
			*r |= m;
		r += bytes;
		if(scrarray->r & 0x04)
			*r |= m;
		r += bytes;
		if(scrarray->r & 0x08)
			*r |= m;
		r += bytes;
		if(scrarray->r & 0x10)
			*r |= m;
		r += bytes;
		if(scrarray->r & 0x20)
			*r |= m;
		r += bytes;
		if(scrarray->r & 0x40)
			*r |= m;
		r += bytes;
		if(scrarray->r & 0x80)
			*r |= m;
		r += bytes;

		// green byte
		if(scrarray->g & 0x01)
			*r |= m;
		r += bytes;
		if(scrarray->g & 0x02)
			*r |= m;
		r += bytes;
		if(scrarray->g & 0x04)
			*r |= m;
		r += bytes;
		if(scrarray->g & 0x08)
			*r |= m;
		r += bytes;
		if(scrarray->g & 0x10)
			*r |= m;
		r += bytes;
		if(scrarray->g & 0x20)
			*r |= m;
		r += bytes;
		if(scrarray->g & 0x40)
			*r |= m;
		r += bytes;
		if(scrarray->g & 0x80)
			*r |= m;
		r += bytes;

		// blue byte
		if(scrarray->b & 0x01)
			*r |= m;
		r += bytes;
		if(scrarray->b & 0x02)
			*r |= m;
		r += bytes;
		if(scrarray->b & 0x04)
			*r |= m;
		r += bytes;
		if(scrarray->b & 0x08)
			*r |= m;
		r += bytes;
		if(scrarray->b & 0x10)
			*r |= m;
		r += bytes;
		if(scrarray->b & 0x20)
			*r |= m;
		r += bytes;
		if(scrarray->b & 0x40)
			*r |= m;
		r += bytes;
		if(scrarray->b & 0x80)
			*r |= m;

		scrarray++;
	}
}

/*************
 * DESCRIPTION:   Convert data to "cmpByteRun1" run compression.
 *                control bytes:
 *                [0..127]   : followed by n+1 bytes of data.
 *                [-1..-127] : followed by byte to be repeated (-n)+1 times.
 *                -128       : NOOP.
 * INPUT:         source      pointer to sourcebuffer
 *                dest        pointer to destinationbuffer
 *                bytes       amount of bytes of one row
 * OUTPUT:        amount of packed bytes
 *************/
static ULONG packrow(UBYTE *source, UBYTE *dest, ULONG bytes)
{
	register ULONG from, to, tmp;
	register UBYTE act;        // actual byte
	register UBYTE n;

	from = 0;
	to = 0;
	while(from<bytes)
	{
		act = source[from++];
		if((from<bytes) && (act==source[from]))
		{
			// repeat byte
			n = 0xFF;
			from++;
			while((act==source[from]) && (from<bytes) && (n>0x80))
			{
				from++;
				n--;
			}
			dest[to++] = n;
			dest[to++] = act;
		}
		else
		{
			// copy bytes
			n = 0;
			tmp = to++;
			while((source[from]!=source[from+1]) && ((from+1)<bytes) && (n<0x80))
			{
				dest[to++] = act;
				act = source[from++];
				n++;
			}
			if((from+1)==bytes)
			{
				dest[to++] = act;
				act = source[from++];
				n++;
			}
			dest[tmp] = n;
			dest[to++] = act;
		}
	}
	return to;
}
