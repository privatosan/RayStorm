/***************
 * MODULE:        Jpeg.c
 * PROJECT:       RayStorm
 * DESCRIPTION:   In this module are all the functions to read and write
 *                Jpeg-files
 * AUTHORS:       Mike Hesser, Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    18.09.97 mh    initial release, reading of JPEGs works
 *    21.09.97 mh    added saving of JPEG, error handling
 *    26.10.97 ah    removed PicInit() and picCleanup(), made it threadsave
 ***************/

#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#ifndef PICLOAD_H
#include "picload.h"
#endif
#include "jpeglib.h"

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
	#define SAVEDS __saveds
#else
	#define SAVEDS
#endif

#define ERR_MEM   0
#define ERR_OPEN  1
#define ERR_READ  2
#define ERR_WRITE 3

/* error handler, to avoid those pesky exit(0)'s */
struct my_error_mgr
{
	struct jpeg_error_mgr pub; /* "public" fields */
	jmp_buf setjmp_buffer;  /* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;
METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;


#ifdef __PPC__
	// longjmp don't works on PPC yet
	return;
#else
	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
#endif // __PPC__
}

#ifndef __PPC__
/* buffer for JPEG error messages */
static char error_buffer[JMSG_LENGTH_MAX];
#endif // !__PPC__

static char *errors[] =
{
	"Not enough memory",
	"Can't open image file",
	"Error reading file",
	"Error writing file"
};

/*************
 * DESCRIPTION:   Read JPEG-file
 * INPUT:         image       pointer to image structure
 * OUTPUT:        NULL if ok, else error string
 *************/
char* SAVEDS picRead(PICTURE *image)
{
	/* This struct contains the JPEG decompression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).*/
	struct my_error_mgr jerr;
	struct jpeg_decompress_struct cinfo;
	FILE *infile = NULL;
	unsigned char *j, *k, *pBuffer;
	int i, l;

	if ((infile = fopen(image->name, "rb")) == NULL)
		return errors[ERR_OPEN];

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

#ifndef __PPC__
	// setjmp don't works on PPC yet

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		* We need to clean up the JPEG object, close the input file, and return.
		*/
		(cinfo.err->format_message) ((j_common_ptr)&cinfo, error_buffer);

		jpeg_destroy_decompress(&cinfo);

		if (infile)
			fclose(infile);

		return error_buffer;
	}
#endif // __PPC__

	/* Step 1: allocate and initialize JPEG decompression object */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jpeg_create_decompress(&cinfo);      /* Now we can initialize the JPEG decompression object. */

	/* Step 2: specify data source (eg, a file) */
	jpeg_stdio_src(&cinfo, infile);

	if (!jpeg_read_header(&cinfo, TRUE))
	{
		jpeg_destroy_decompress(&cinfo);

		if (infile)
			fclose(infile);
		return errors[ERR_READ];
	}

	jpeg_start_decompress(&cinfo);

	image->colormap = (SMALL_COLOR*)image->malloc(sizeof(SMALL_COLOR)*cinfo.image_width*cinfo.image_height);
	if(!image->colormap)
	{
		jpeg_destroy_decompress(&cinfo);

		if (infile)
			fclose(infile);
		return errors[ERR_MEM];
	}
	memset(image->colormap, 0, sizeof(SMALL_COLOR)*cinfo.image_width*cinfo.image_height);

	image->width = cinfo.image_width;
	image->height = cinfo.image_height;

	/* allocate buffer to store scanlines */
	pBuffer = malloc(3*image->width);
	if (!pBuffer)
	{
		jpeg_destroy_decompress(&cinfo);

		if (infile)
			fclose(infile);
		return errors[ERR_MEM];
	}

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */
	if (cinfo.out_color_space == JCS_GRAYSCALE)
	{
		for (i = 0; i < (int)cinfo.image_height; i++)
		{
			jpeg_read_scanlines(&cinfo, (unsigned char **)(&pBuffer), 1);

			j = (unsigned char *)pBuffer;
			k = (unsigned char *)(&image->colormap[image->width*i]);
			for (l = 0; l < (int)cinfo.image_width; l++, j++, k += 4)
			{
				k[0] = k[1] = k[2] = *j;
				k[3] = 0;
			}
			if (image->Progress)
				image->Progress(image->caller, (float)i/(float)(image->height - 1));
		}
	}
	else
	{
		for (i = 0; i < (int)cinfo.image_height; i++)
		{
			jpeg_read_scanlines(&cinfo, (unsigned char **)(&pBuffer), 1);

			j = (unsigned char *)pBuffer;
			k = (unsigned char *)(&image->colormap[image->width*i]);
			for (l = 0; l < (int)cinfo.image_width; l++, j += 3, k += 4)
			{
				k[0] = j[0];
				k[1] = j[1];
				k[2] = j[2];
				k[3] = 0;
			}
			if (image->Progress)
				image->Progress(image->caller, (float)i/(float)(image->height - 1));
		}
	}
	free(pBuffer);

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	return NULL;
}

/*************
 * DESCRIPTION:   write 24Bit JPEG-file
 * INPUT:         image       pointer to image structure
 * OUTPUT:        NULL if ok, else error string
 *************/
char *picWrite(PICTURE *image)
{
	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;
	UBYTE *outrow, *pBuffer, *p;
	FILE *outfile = NULL;
	int i, j;
#ifndef __PPC__
	/* buffer for JPEG error messages */
	static char error_buffer[JMSG_LENGTH_MAX];
#endif // !__PPC__

	if ((outfile = fopen(image->name, "wb")) == NULL)
		return errors[ERR_OPEN];

	/* Step 1: allocate and initialize JPEG compression object */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

#ifndef __PPC__
	// setjmp don't works on PPC yet

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		* We need to clean up the JPEG object, close the input file, and return.
		*/
		jpeg_destroy_compress(&cinfo);

		/* Create the message */
		(cinfo.err->format_message) ((j_common_ptr)&cinfo, error_buffer);

		if (outfile)
			fclose(outfile);

		return error_buffer;
	}
#endif // !__PPC__

	jpeg_create_compress(&cinfo);        // Now we can initialize the JPEG compression object.

	/* Step 2: specify data source (eg, a file) */
	jpeg_stdio_dest(&cinfo, outfile);
	/* First we supply a description of the input image.
	* Four fields of the cinfo struct must be filled in:
	*/
	cinfo.image_width = image->width;   /* image width and height, in pixels */
	cinfo.image_height = image->height;

	cinfo.input_components = 3;      /* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;  /* colorspace of input image */

	/* Now use the library's routine to set default compression parameters.
	* (You must set at least cinfo.in_color_space before calling this,
	* since the defaults depend on the source color space.) */
	jpeg_set_defaults(&cinfo);
	/* Now you can set any non-default parameters you wish to.
	* Here we just illustrate the use of quality (quantization table) scaling:*/
	if(image->param)
		jpeg_set_quality(&cinfo, ((JPEG_PARAM*)(image->param))->quality, TRUE /* limit to baseline-JPEG values */);
	else
		jpeg_set_quality(&cinfo, 75, TRUE /* limit to baseline-JPEG values */);

	/* Step 4: Start compressor */
	/* TRUE ensures that we will write a complete interchange-JPEG file.
	* Pass TRUE unless you are very sure of what you're doing. */
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines remain to be written) */
	/*           jpeg_write_scanlines(...); */

	/* Here we use the library's state variable cinfo.next_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	* To keep things simple, we pass one scanline per call; you can pass
	* more if you wish, though. */

	pBuffer = malloc(3*image->width);
	outrow = (UBYTE*)image->colormap;
	for (i = 0; i < (int)cinfo.image_height; i++)
	{
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		* Here the array is only one element long, but you could pass
		* more than one scanline at a time if that's more convenient. */

		p = pBuffer;
		for (j = 0; j < (int)(sizeof(SMALL_COLOR)*cinfo.image_width); j++)
		{
			p[0] = outrow[j++];
			p[1] = outrow[j++];
			p[2] = outrow[j++];
			p += 3;
		}
		(void) jpeg_write_scanlines(&cinfo, &pBuffer, 1);
		outrow += image->width*sizeof(SMALL_COLOR);

#ifndef __AMIGA__
		if (image->Progress)
			image->Progress(image->caller, (float)i/(float)(cinfo.image_height - 1));
#endif
	}
	free(pBuffer);

	/* Step 6: Finish compression */

	jpeg_finish_compress(&cinfo);

	/* After finish_compress, we can close the output file. */
	fclose(outfile);

	/* Step 7: release JPEG compression object */
	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_compress(&cinfo);

	return NULL;
}
