/***************
 * MODUL:         PNG-handler
 * NAME:          png-handler.cpp
 * DESCRIPTION:   In this module are all the functions to read and write
 *                PNG-files (Portable Network Graphics)
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.10.95 ah    initial release
 *    25.10.97 ah    removed picInit(), picCleanup()
 ***************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __AMIGA__
	#include <exec/memory.h>
	#include <utility/tagitem.h>
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
	extern "ASM" void _exit() { }
	#define SAVEDS __saveds
#else
	#define SAVEDS
#endif

#include "png.h"

#ifndef PICLOAD_H
#include "picload.h"
#endif

#define ERR_MEM   0
#define ERR_OPEN  1
#define ERR_READ  2
#define ERR_WRITE 3

static char *errors[] =
{
	"Not enough memory",
	"Can't open image file",
	"Error reading file",
	"Error writing file"
};

/*************
 * DESCRIPTION:   Read PNG-file
 * INPUT:         image       pointer to image structure
 * OUTPUT:        NULL if ok, else error string
 *************/
char* SAVEDS picRead(PICTURE *image)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_byte **row_pointers;
	int y;
	FILE *fp;

	fp = fopen(image->name,"rb");
	if(!fp)
		return errors[ERR_OPEN];

	/* Create and initialize the png_struct with the desired error handler
	 * functions.  If you want to use the default stderr and longjump method,
	 * you can supply NULL for the last three parameters.  We also supply the
	 * the compiler header file version, so that we know if the application
	 * was compiled with a compatible version of the library.  REQUIRED
	 */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose(fp);
		return errors[ERR_MEM];
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return errors[ERR_MEM];
	}

#ifndef __PPC__
	// setjmp don't works on PPC yet

	/* set error handling */
	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		/* If we get here, we had a problem reading the file */
		return errors[ERR_READ];
	}
#endif // __PPC__

	/* set up the input control */
	png_init_io(png_ptr, fp);

	/* read the file information */
	png_read_info(png_ptr, info_ptr);

	/* expand paletted colors into true rgb */
	if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);
	/* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
	if ((info_ptr->color_type == PNG_COLOR_TYPE_GRAY) && (info_ptr->bit_depth < 8))
		png_set_expand(png_ptr);

	/* pack rgb pixels to 4 bytes */
	png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);
	/* tell libpng to strip 16 bit/color files down to 8 bits/color */
	png_set_strip_16(png_ptr);

	image->colormap = (SMALL_COLOR*)image->malloc(sizeof(SMALL_COLOR)*info_ptr->width*info_ptr->height);
	if(!image->colormap)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return errors[ERR_MEM];
	}
	image->width = info_ptr->width;
	image->height = info_ptr->height;

	/* read the image */
	// allocate rows pointers
	row_pointers = (png_byte**)malloc(sizeof(png_byte*)*image->height);
	if(!row_pointers)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return errors[ERR_MEM];
	}
	// initialize pointers
	for(y=0; y<image->height; y++)
		row_pointers[y] = (png_byte*)&image->colormap[image->width*y];

	png_read_image(png_ptr, row_pointers);
	// free row pointers
	free(row_pointers);

	/* read the rest of the file, getting any additional chunks in info_ptr */
	png_read_end(png_ptr, info_ptr);

	/* clean up after the read, and free any memory allocated */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	/* free the structures */
	fclose(fp);

	/* that's it */
	return NULL;
}

/*************
 * DESCRIPTION:   write 24Bit PNG-file
 * INPUT:         image       pointer to image structure
 * OUTPUT:        NULL if ok, else error string
 *************/
char *picWrite(PICTURE *image)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_byte **row_pointers;
	png_text text_ptr;
	FILE *fp;
	int y;

	fp = fopen(image->name,"wb");
	if(!fp)
		return errors[ERR_OPEN];

	/* Create and initialize the png_struct with the desired error handler
	 * functions.  If you want to use the default stderr and longjump method,
	 * you can supply NULL for the last three parameters.  We also check that
	 * the library version is compatible with the one used at compile time,
	 * in case we are using dynamically linked libraries.  REQUIRED.
	 */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose(fp);
		return errors[ERR_MEM];
	}

	/* Allocate/initialize the image information data.  REQUIRED */
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
	{
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		fclose(fp);
		return errors[ERR_MEM];
	}

#ifndef __PPC__
	// setjmp don't works on PPC yet

	/* set error handling */
	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		fclose(fp);
		/* If we get here, we had a problem reading the file */
		return errors[ERR_WRITE];
	}
#endif // __PPC__

	/* set up the output control */
	png_init_io(png_ptr, fp);

	/* set the file information here */
	png_set_IHDR(png_ptr, info_ptr, image->width, image->height, 8, PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	/* Optionally write comments into the image */
	text_ptr.key = "Creator";
	text_ptr.text = "RayStorm";
	text_ptr.compression = PNG_TEXT_COMPRESSION_NONE;
	png_set_text(png_ptr, info_ptr, &text_ptr, 1);

	/* write the file information */
	png_write_info(png_ptr, info_ptr);

	/* get rid of filler bytes, pack rgb into 3 bytes.  The
		filler number is not used. */
	png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);

	/* write the image */
	// allocate rows pointers
	row_pointers = (png_byte**)malloc(sizeof(png_byte*)*image->height);
	if(!row_pointers)
	{
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		fclose(fp);
		return errors[ERR_MEM];
	}
	for(y=0; y<image->height; y++)
		row_pointers[y] = (png_byte*)&image->colormap[image->width*y];

	// initialize pointers
	png_write_image(png_ptr, row_pointers);

	/* write the rest of the file */
	png_write_end(png_ptr, info_ptr);

	/* clean up after the write, and free any memory allocated */
	png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);

	// free row pointers
	free(row_pointers);
	fclose(fp);

	/* that's it */
	return NULL;
}
