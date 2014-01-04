/***************
 * MODULE:        rsi.cpp
 * PROJECT:       RayStorm
 * DESCRIPTION:   interface for RayStorm (tm) renderer
 * AUTHORS:       Andreas Heumann & Mike Hesser
 * BUGS:          none
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    15.04.96 mh    transfer of methods from raystorm.cpp
 *    08.10.96 ah    added 'Orient'-parameters for brushes and textures
 *    22.10.96 mh    added rsiAddFlare
 *    24.10.96 mh    added comments
 *    31.10.96 mh    added rsiAddStar
 *    27.11.96 ah    added rsiTStarInnerHaloRadius
 *    01.12.96 ah    lens effects are only renderd if there are any lights with lens effects
 *    10.01.97 ah    added interface for box object
 *    12.01.97 ah    added interface for CSG object
 *    04.03.97 ah    added interface for shadowmaps
 *    19.03.97 ah    added interface for cylinder and sor object
 *    23.03.97 ah    added interface for cone
 *    21.06.97 ah    changed to new V2 brush and texture handling
 *    17.05.98 ah    motion blur is now calculated on per line basis not on pixel basis
 *                   if not random jittering (doubles the speed!)
 *    18.08.98 ah    added directional light
 *    10.05.99 ah    added brush mapping coordinate support
 *****************/

#include <time.h>
#include <string.h>

#ifdef __AMIGA__
	#include <pragma/exec_lib.h>
#else
	#include <windows.h>
	#include <direct.h>
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef VOXEL_H
#include "voxel.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef OCTREE_H
#include "octree.h"
#endif

#ifndef SPHERE_H
#include "sphere.h"
#endif

#ifndef PLANE_H
#include "plane.h"
#endif

#ifndef TRIANGLE_H
#include "triangle.h"
#endif

#ifndef BOX_H
#include "box.h"
#endif

#ifndef CYLINDER_H
#include "cylinder.h"
#endif

#ifndef SOR_H
#include "sor.h"
#endif

#ifndef CONE_H
#include "cone.h"
#endif

#ifndef CSG_H
#include "csg.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef FILEUTIL_H
#include "fileutil.h"
#endif

#ifndef OBJHAND_H
#include "objhand.h"
#endif

#define LIBRARY
#ifndef RSI_H
#include "rsi.h"
#endif

#ifndef PARAM_H
#include "param.h"
#endif

#ifndef ERROR_H
#include "error.h"
#endif

#ifndef FLARES_H
#include "flares.h"
#endif

#ifndef STAR_H
#include "star.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

char *errors[] =
{
	"",
	"Not enough memory",
	"Limitations of demo version reached",
	"Wrong screen resolution",
	"Error in triangle definition",
	"The view and up directions are identical",
	"Not enough memory for screen buffer",
	"The backdrop picture has the wrong size",
	"Can't open Imagine texture file",
	"Can't open brush file",
	"Error initializing Imagine texture",
	"Can't open picture",
	"Error reading picture",
	"Can't open picture type file",
	"Error reading picture type file",
	"Unknown picture format",
	"An error occcured while invoking picture handler",
	"Can't open object",
	"Error reading object",
	"Can't open object type file",
	"Error reading object type file",
	"Unknown object format",
	"An error occcured while invoking object handler",
	"Actor not defined",
	"Surface not defined",
	"Depth of octree too big (max. 6)",
	"Antialiasing value too big (max. 8)",
	"Invalid time intervall",
	"No picture renderd",
	"Distribution value too big (max. 8)",
	"Wrong error number",
	"Unknown parameter",
	"Unknown statistic type",
	"Need at least 4 points for surface of revolution",
	"Incorrect point in surface of revolution",
	"A CSG object needs at least 2 objects to operate with",
	"Invalid object type for CSG",
	"Need at last a V2 object handler to load file",
	"Need at last a V2 picture handler to load file",
	"Invalid keyfile"
};

/*#ifdef __PPC__
#define CTXT struct Library *RayStormBase, RSICONTEXT *rc
#else*/
#define CTXT RSICONTEXT *rc
//#endif // __PPC__

#ifdef __AMIGA__
	#include "lib.h"
	#ifdef __PPC__
	#define rsiResult int __saveds
	#else
	typedef int rsiResult;
	#endif
#else
	#define rsiResult __declspec(dllexport) int
	extern CRITICAL_SECTION gCriticalSection;
	extern float gRoot;
	extern char WorkingDirectory[256];
	HMODULE hKeyFile;
	typedef BOOL (*KEYDECODE1)();
	typedef BOOL (*KEYDECODE2)(char *);
	typedef BOOL *(*KEYWRITEPROC)(char *, PICTURE *, char *);
	KEYDECODE1 keyDecodeKeyfile1 = NULL;
	KEYDECODE2 keyDecodeKeyfile2 = NULL;
	KEYWRITEPROC keyWritePicture = NULL;
#endif

#define rsiDEFAULT   -1

extern "C"
{

/*************
* DESCRIPTION: copies time information into a buffer
* INPUT:       buffer   information to be stored
*              text     additional text info (i.e. Elapsed Time...)
*              time     time data
* OUTPUT:      -
*************/
void Time(char *buffer, char *text, clock_t time)
{
	clock_t h, m, s;

	s = time/CLOCKS_PER_SEC;
	h = s/3600;
	m = (s/60) % 60;
	s = s % 60;
	sprintf(buffer, "%s %02d:%02d:%02d", text, h, m, s);
}

/*************
* DESCRIPTION: converts and normalizes a motion blur color buffer line to a
*              screenbuffer line
* INPUT:       rc          current render context
*              array       screenbuffer line
*              mbarray     motion blur color buffer line
* OUTPUT:      error number  ERR_NONE when everything is ok
*************/
static void ConvertMBLine(RSICONTEXT *rc, SMALL_COLOR *array, COLOR *mbarray)
{
	COLOR color;
	float value, inv;
	int x;

	value = 255.f / rc->world.motionblur_samples;
	array += rc->camera.left;
	mbarray += rc->camera.left;
	for(x = rc->camera.left; x <= rc->camera.right; x++)
	{
		color = *mbarray;
		mbarray++;

		if(array->info & INFO_SAMPLED)
		{
			color.r *= value*.5f;
			color.g *= value*.5f;
			color.b *= value*.5f;
		}
		else
		{
			color.r *= value;
			color.g *= value;
			color.b *= value;
		}
		// Normalize the color
		if(color.r > 255.f)
		{
			inv = 255.f/color.r;
			color.r = 255.f;
			color.g *= inv;
			color.b *= inv;
		}
		if(color.g > 255.f)
		{
			inv = 255.f/color.g;
			color.r *= inv;
			color.g = 255.f;
			color.b *= inv;
		}
		if(color.b > 255.f)
		{
			inv = 255.f/color.b;
			color.r *= inv;
			color.g *= inv;
			color.b = 255.f;
		}

		array->r = (UBYTE)(color.r);
		array->g = (UBYTE)(color.g);
		array->b = (UBYTE)(color.b);
		array++;
	}
}

/*************
* DESCRIPTION: keeps the main raytracing loop, makes resolution test,
*              loads backdrop/reflection maps, allocates screen buffer,
*              initializes the scene, calls the callback routines to
*              keep the user of rsi informed about the rendering process
* INPUT:       rc          current render context
*              act_time    on which time the scene is renderd
* OUTPUT:      error number  ERR_NONE when everything is ok
*************/
static int Raytrace(RSICONTEXT *rc, const TIME *cur_time)
{
	int x, y, oldy, lastline, hy, ha;
	unsigned int r;
	char *err;
	IMAGE *image;
	COLOR *mb_buffer = NULL,
		*mb_buffer1 = NULL,
		*mb_buffer2 = NULL,
		color, *tmpc;
	float value, inv;
	float percent_per_row, oldtime=0, time;
	float percent, xfact, yfact;
	clock_t  starttime;
	float *filter;
	LONG arraypos, oldarraypos;
	BOOL error, singlesample=TRUE;
	BOOL done=FALSE;

	if(rc->scrarray)
	{
		delete [ ] rc->scrarray;
		rc->scrarray = NULL;
	}

	rc->Log(rc, "Initializing");
	starttime = clock();

	rc->world.time = cur_time->begin;
	rc->world.timelength = cur_time->end - cur_time->begin;
	// precalculate animation matrices
	PreCalcMatrices(rc, cur_time);
	// update lights
	UpdateLights(rc, cur_time->begin);
	UpdateLensEffects(rc, rc->camera.xres);
	// update objects
	UpdateObjects(rc, cur_time->begin);
	// generate object voxels
	GenVoxels(rc);

	// build octree
	InitOctree(rc, rc->world.depth, rc->world.minobjects);
	rc->bOctreedone = TRUE;

#ifdef __AMIGA__
	if(KeyfileBase)
		rc->world.validkeyfile = keyDecodeKeyfile1();
#else
	if (keyDecodeKeyfile1)
		rc->world.validkeyfile = keyDecodeKeyfile1();
#endif

	// get memory for screen buffer load backdrop picture
	if(rc->world.backdrop)
	{
		sprintf(rc->buf, "Loading backdrop picture: \"%s\"", rc->world.backdrop);
		rc->Log(rc, rc->buf);
		image = new IMAGE;
		if(!image)
			return ERR_SCREENMEM;
		error = image->Load(rc, rc->world.backdrop, rc->world.brushpath);
		if(error)
		{
			delete image;
			if(error == ERR_EXTERNAL)
			{
				err = rc->GetImageError();
				if(err)
					strcpy(rc->externalerror, err);
				else
					rc->externalerror[0] = 0;
			}
			return error;
		}

		// get mem for 24 bit screen buffer
		rc->scrarray = new SMALL_COLOR[rc->camera.xres*rc->camera.yres];
		if(!rc->scrarray)
			return ERR_SCREENMEM;

		if(image->width != rc->camera.xres || image->height != rc->camera.yres)
		{
			// scale image
			rc->Log(rc, "Scaling image");
			xfact = (float)image->width/(float)rc->camera.xres;
			yfact = (float)image->height/(float)rc->camera.yres;
			for(y=0; y<rc->camera.yres; y++)
			{
				hy = (int)floor(((float)(y)*yfact))*image->width;
				ha = y*rc->camera.xres;
				for(x=0; x<rc->camera.xres; x++)
				{
					rc->scrarray[ha++] = image->colormap[hy + (int)floor((float)(x)*xfact)];
				}
			}
		}
		else
		{
			// copy image
			memcpy(rc->scrarray, image->colormap, sizeof(SMALL_COLOR)*rc->camera.xres*rc->camera.yres);
		}
		delete image;
	}
	else
	{
		// get mem for 24 bit screen buffer
		rc->scrarray = new SMALL_COLOR[rc->camera.xres*rc->camera.yres];
		if(!rc->scrarray)
			return ERR_SCREENMEM;
		memset(rc->scrarray, 0, rc->camera.xres*rc->camera.yres*sizeof(SMALL_COLOR));
	}

	// initialize shadow map
	if(!(rc->world.flags & WORLD_QUICK))
	{
		if(TestIfShadowMaps(rc))
		{
			rc->Log(rc, "Building shadow map");
			InitShadowMap(rc);
		}
	}

	// initialize camera
	rc->camera.DoFData.filterwidth = rc->world.filterwidth;
	rc->camera.InitCamera();
	sprintf(rc->buf, "Rendering %dx%d", rc->camera.xres, rc->camera.yres);
	rc->Log(rc, rc->buf);

	percent_per_row = 1.f/(float)(rc->camera.bottom - rc->camera.top);
	y = rc->camera.yres - rc->camera.top - 1;
	oldy = y;
	arraypos = (rc->camera.yres - y - 1)*rc->camera.xres;
	oldarraypos = arraypos;
	lastline = rc->camera.yres - rc->camera.bottom - 1;

	// antialias on ?
	if(rc->world.samples != 1)
	{
		filter = new float[rc->world.samples*rc->world.samples];
		if(!filter)
			return ERR_MEM;
		CreateFilter(filter, &rc->world);

		if(!(rc->world.flags & WORLD_RANDOM_JITTER) && (rc->world.motionblur_samples > 1) && (rc->world.timelength > 0.f))
		{
			// if we are doing motionblur an not random jittering render the lines multiple time
			// and accumulate the single values, if random jittering we have to change the time
			// per pixel, here we can do it per line
			// if we don't have enough memory fall back to per pixel motion blur
			mb_buffer = new COLOR[rc->camera.xres];
			mb_buffer1 = new COLOR[rc->camera.xres];
			mb_buffer2 = new COLOR[rc->camera.xres];
			if(mb_buffer && mb_buffer1 && mb_buffer2)
			{
				singlesample = FALSE;
				tmpc = mb_buffer1;
				for(x = rc->camera.left; x <= rc->camera.right; x++)
				{
					tmpc->r = 0.f;
					tmpc->g = 0.f;
					tmpc->b = 0.f;
					tmpc++;
				}
			}
			else
			{
				if(mb_buffer)
				{
					delete mb_buffer;
					mb_buffer = NULL;
				}
				if(mb_buffer1)
				{
					delete mb_buffer1;
					mb_buffer1 = NULL;
				}
				if(mb_buffer2)
				{
					delete mb_buffer2;
					mb_buffer2 = NULL;
				}
			}
		}

		// first line is always completely supersampled
		SuperSampleLine(rc, y, &rc->scrarray[arraypos], filter, mb_buffer);
		y--;
		arraypos += rc->camera.xres;

		if(y >= lastline && !rc->CheckCancel(rc))
		{
			// also left and right pixel is supersampled
			if(singlesample)
			{
				SampleLine(rc, y, &rc->scrarray[arraypos], NULL, rc->world.time);
				SuperSamplePixel(rc, rc->camera.left, y, &rc->scrarray[arraypos+rc->camera.left], filter, NULL, rc->world.time);
				SuperSamplePixel(rc, rc->camera.right, y, &rc->scrarray[arraypos+rc->camera.right], filter, NULL, rc->world.time);
			}
			else
			{
				for(r = 0; r < rc->world.motionblur_samples; r++)
				{
					time = CalcTime(r, &rc->world);
					SampleLine(rc, y, &rc->scrarray[arraypos], mb_buffer1, time);
					SuperSamplePixel(rc, rc->camera.left, y, &rc->scrarray[arraypos+rc->camera.left], filter, &mb_buffer1[rc->camera.left], time);
					SuperSamplePixel(rc, rc->camera.right, y, &rc->scrarray[arraypos+rc->camera.right], filter, &mb_buffer1[rc->camera.right], time);
				}
				rc->scrarray[arraypos+rc->camera.left].info |= INFO_SAMPLED;
				rc->scrarray[arraypos+rc->camera.right].info |= INFO_SAMPLED;
				ConvertMBLine(rc, &rc->scrarray[arraypos - rc->camera.xres], mb_buffer);
				ConvertMBLine(rc, &rc->scrarray[arraypos], mb_buffer1);
			}
			arraypos += rc->camera.xres;
		}

		percent = (rc->camera.yres - rc->camera.top - 1 - y) * percent_per_row;
		time = float(clock()-starttime)/CLOCKS_PER_SEC;
		if(time > oldtime + UPDATE_INTERVAL)
		{
			rc->UpdateStatus(rc, percent, time, rc->camera.yres - oldy - 1, oldy-y, &rc->scrarray[oldarraypos]);
			oldy = y;
			oldarraypos = arraypos;
			oldtime = time;
		}

		while (y > lastline+1 && !rc->CheckCancel(rc))
		{
			if(singlesample)
			{
				// sample current line
				SampleLine(rc, y - 1, &rc->scrarray[arraypos], NULL, rc->world.time);
				SuperSamplePixel(rc, rc->camera.left, y - 1, &rc->scrarray[arraypos+rc->camera.left], filter, NULL, rc->world.time);
				SuperSamplePixel(rc, rc->camera.right, y - 1, &rc->scrarray[arraypos+rc->camera.right], filter, NULL, rc->world.time);
				// supersample previous line
				AdaptiveRefineLine(rc, y, &rc->scrarray[arraypos - rc->camera.xres*2],
									  &rc->scrarray[arraypos - rc->camera.xres],
									  &rc->scrarray[arraypos], filter, NULL, NULL, NULL);
			}
			else
			{
				tmpc = mb_buffer2;
				for(x = rc->camera.left; x <= rc->camera.right; x++)
				{
					tmpc->r = 0.f;
					tmpc->g = 0.f;
					tmpc->b = 0.f;
					tmpc++;
				}
				// sample current line
				for(r = 0; r < rc->world.motionblur_samples; r++)
				{
					time = CalcTime(r, &rc->world);
					SampleLine(rc, y - 1, &rc->scrarray[arraypos], mb_buffer2, time);
					SuperSamplePixel(rc, rc->camera.left, y - 1, &rc->scrarray[arraypos+rc->camera.left], filter, &mb_buffer2[rc->camera.left], time);
					SuperSamplePixel(rc, rc->camera.right, y - 1, &rc->scrarray[arraypos+rc->camera.right], filter, &mb_buffer2[rc->camera.right], time);
				}
				rc->scrarray[arraypos+rc->camera.left].info |= INFO_SAMPLED;
				rc->scrarray[arraypos+rc->camera.right].info |= INFO_SAMPLED;
				ConvertMBLine(rc, &rc->scrarray[arraypos], mb_buffer2);
				// supersample previous line
				AdaptiveRefineLine(rc, y, &rc->scrarray[arraypos - rc->camera.xres*2],
									  &rc->scrarray[arraypos - rc->camera.xres],
									  &rc->scrarray[arraypos], filter, mb_buffer, mb_buffer1, mb_buffer2);

				tmpc = mb_buffer;
				mb_buffer = mb_buffer1;
				mb_buffer1 = mb_buffer2;
				mb_buffer2 = tmpc;
			}
			y--;
			arraypos += rc->camera.xres;

			percent = (rc->camera.yres - rc->camera.top - 1 - y) * percent_per_row;
			time = float(clock()-starttime)/CLOCKS_PER_SEC;
			if(time > oldtime + UPDATE_INTERVAL)
			{
				rc->UpdateStatus(rc, percent, time, rc->camera.yres - oldy - 1, oldy-y, &rc->scrarray[oldarraypos]);
				oldy = y;
				oldarraypos = arraypos;
				oldtime = time;
				rc->Cooperate(rc);
			}
		}
		// Supersample last scanline
		if(y > lastline && !rc->CheckCancel(rc))
		{
			if(singlesample)
			{
				SuperSampleLine(rc, y - 1, &rc->scrarray[arraypos], filter, NULL);
				AdaptiveRefineLine(rc, y, &rc->scrarray[arraypos - rc->camera.xres*2],
									  &rc->scrarray[arraypos - rc->camera.xres],
									  &rc->scrarray[arraypos], filter, NULL, NULL, NULL);
			}
			else
			{
				SuperSampleLine(rc, y - 1, &rc->scrarray[arraypos], filter, mb_buffer2);
				ConvertMBLine(rc, &rc->scrarray[arraypos], mb_buffer2);
				AdaptiveRefineLine(rc, y, &rc->scrarray[arraypos - rc->camera.xres*2],
									  &rc->scrarray[arraypos - rc->camera.xres],
									  &rc->scrarray[arraypos], filter, mb_buffer, mb_buffer1, mb_buffer2);
			}
			y--;
		}

		// uncomment the following lines to mark all supersampled pixels red for debugging
		/*{
			for(x=rc->camera.xres*rc->camera.yres-1; x>=0; x--)
			{
				if(rc->scrarray[x].info & INFO_SAMPLED)
				{
					rc->scrarray[x].r = 255;
					rc->scrarray[x].g = 0;
					rc->scrarray[x].b = 0;
				}
			}
		}*/

		if(mb_buffer)
			delete mb_buffer;
		if(mb_buffer1)
			delete mb_buffer1;
		if(mb_buffer2)
			delete mb_buffer2;
		delete [ ] filter;
	}
	else // no antialias, do no super sampling
	{
		if(!(rc->world.flags & WORLD_RANDOM_JITTER) && (rc->world.motionblur_samples > 1) && (rc->world.timelength > 0.f))
		{
			// if we are doing motionblur an not random jittering render the lines multiple time
			// and accumulate the single values, if random jittering we have to change the time
			// per pixel, here we can do it per line
			// if we don't have enough memory fall back to per pixel motion blur
			mb_buffer = new COLOR[rc->camera.xres];
			if(mb_buffer)
				singlesample = FALSE;
		}
		while (y >= lastline && !done)
		{
			if(singlesample)
				SampleLine(rc, y, &rc->scrarray[arraypos], mb_buffer, rc->world.time);
			else
			{
				tmpc = mb_buffer;
				for(x = rc->camera.left; x <= rc->camera.right; x++)
				{
					tmpc->r = 0.f;
					tmpc->g = 0.f;
					tmpc->b = 0.f;
					tmpc++;
				}
				for(r = 0; r < rc->world.motionblur_samples; r++)
				{
					SampleLine(rc, y, &rc->scrarray[arraypos], mb_buffer, CalcTime(r, &rc->world));
				}
				value = 255.f / rc->world.motionblur_samples;
				for(x = rc->camera.left; x <= rc->camera.right; x++)
				{
					color = mb_buffer[x];

					color.r *= value;
					color.g *= value;
					color.b *= value;
					// Normalize the color
					if(color.r > 255.f)
					{
						inv = 255.f/color.r;
						color.r = 255.f;
						color.g *= inv;
						color.b *= inv;
					}
					if(color.g > 255.f)
					{
						inv = 255.f/color.g;
						color.r *= inv;
						color.g = 255.f;
						color.b *= inv;
					}
					if(color.b > 255.f)
					{
						inv = 255.f/color.b;
						color.r *= inv;
						color.g *= inv;
						color.b = 255.f;
					}

					rc->scrarray[arraypos+x].r = (UBYTE)(color.r);
					rc->scrarray[arraypos+x].g = (UBYTE)(color.g);
					rc->scrarray[arraypos+x].b = (UBYTE)(color.b);
				}
			}
			y--;
			arraypos += rc->camera.xres;
			percent = (rc->camera.yres-rc->camera.top - y - 1) * percent_per_row;
			time = float(clock()-starttime)/CLOCKS_PER_SEC;
			if(time > oldtime + UPDATE_INTERVAL)
			{
				done = rc->CheckCancel(rc);
				rc->UpdateStatus(rc, percent, time, rc->camera.yres - oldy - 1, oldy-y, &rc->scrarray[oldarraypos]);
				oldy = y;
				oldarraypos = arraypos;
				oldtime = time;
				rc->Cooperate(rc);
			}
		}
		if(mb_buffer)
			delete mb_buffer;
	}

	time = float(clock()-starttime)/CLOCKS_PER_SEC;
	if (oldy - y >= 1)
		rc->UpdateStatus(rc, 1.f, time, rc->camera.yres - oldy - 1, oldy - y, &rc->scrarray[oldarraypos]);
	else
		rc->UpdateStatus(rc, 1.f, time, rc->camera.yres - y - 2, 0, NULL);

	if (!rc->CheckCancel(rc) && (rc->camera.flags & FASTDOF) && rc->camera.aperture > 0.f)
	{
		rc->Log(rc, "Applying depth of field");
		ApplyDepthOfField(rc);
	}


	if (!rc->CheckCancel(rc) && TestIfLensEffects(rc))
	{
		rc->Log(rc, "Applying lens effects");
		ApplyLensEffects(rc);
	}

	rc->Log(rc, "Freeing octree");
	TermOctree(&rc->octreeRoot);
	rc->bOctreedone = FALSE;

	if(!rc->CheckCancel(rc))
	{
		rc->Log(rc, "Rendering finished");
		starttime = clock() - starttime;
		Time(rc->buf, "Total time:", starttime);
		rc->Log(rc, rc->buf);
	}

	return ERR_NONE;
}

/*
										RSI - Interface methods
*/

/*
	Callback routines

	These functions are necessary for communicating with the
	environment. Callbacks which are not set by the user will point
	to default callbacks
*/

/*************
* DESCRIPTION: connect to a function which is called after every
*              renderd line to give other tasks a chance to work
*              (if we are in a cooperative multitasking environment)
* INPUT:       pointer to callback function
* OUTPUT:      ERR_NONE
*************/
rsiResult rsiSetCooperateCB(CTXT, void (*cb)(RSICONTEXT*))
{
	rc->SetCooperateCB(cb);
	return ERR_NONE;
}

/*************
* DESCRIPTION: connect to a function which is called after every
*              renderd line to give the user the chance to cancel the
*              render process. The CheckCancel function must return
*              TRUE when the render process should be canceled otherwise
*              FALSE
* INPUT:       pointer to callback function
* OUTPUT:      ERR_NONE
*************/
rsiResult rsiSetCheckCancelCB(CTXT, BOOL (*cb)(RSICONTEXT*))
{
	rc->SetCheckCancelCB(cb);
	return ERR_NONE;
}

/*************
* DESCRIPTION: connect to a function which is called when
*              RayStorm comments what it does
*              The callback function gets a pointer to the message string
* INPUT:       pointer to callback function
* OUTPUT:      ERR_NONE
*************/
rsiResult rsiSetLogCB(CTXT, void (*cb)(RSICONTEXT*, char *))
{
	rc->SetLogCB(cb);
	return ERR_NONE;
}

/*************
* DESCRIPTION: connect to a function which is called when
*              RayStorm renderd a block to give the user information about
*              the rendering status.
*              1. how much is renderd (percentage)
*              2. time spent so far (in seconds)
*              3. current y-coord.
*              4. amount of lines to update
*              5. pointer to the first line of the block in the screen buffer
* INPUT:       pointer to callback function
* OUTPUT:      ERR_NONE
*************/
rsiResult rsiSetUpdateStatusCB(CTXT, void (*cb)(RSICONTEXT*, float, float, int, int, SMALL_COLOR*))
{
	rc->SetUpdateStatusCB(cb);
	return ERR_NONE;
}

/*************
* DESCRIPTION: make basic initializations (call this when the library/dll is loaded)
* INPUT:       -
* OUTPUT:      error number
*************/
rsiResult rsiInit()
{
#ifndef __AMIGA__
	char szBuf[12];

	szBuf[2] = 'y'; szBuf[3] = 'f';
	szBuf[0] = 'k'; szBuf[1] = 'e';
	szBuf[8] = 'd'; szBuf[9] = 'l';
	// first call to rsiInit, save current Directory where rsi.dll is
	if(!WorkingDirectory[0])
		getcwd(WorkingDirectory, 256);

	szBuf[6] = 'e'; szBuf[7] = '.';
	keyDecodeKeyfile1 = NULL;
	szBuf[4] = 'i'; szBuf[5] = 'l';
	keyDecodeKeyfile2 = NULL;
	keyWritePicture = NULL;

	szBuf[10] = 'l'; szBuf[11] = '\0';
	hKeyFile = LoadLibrary(szBuf);
	if (hKeyFile)
	{
		keyDecodeKeyfile1 = (KEYDECODE1)GetProcAddress(hKeyFile, (LPCSTR)3);
		keyDecodeKeyfile2 = (KEYDECODE2)GetProcAddress(hKeyFile, (LPCSTR)4);
		keyWritePicture = (KEYWRITEPROC)GetProcAddress(hKeyFile, (LPCSTR)1);
	}
#endif

	return ERR_NONE;
}

/*************
* DESCRIPTION: free allocated structures in rsiInit (call this when the library/dll is removed)
* INPUT:       -
* OUTPUT:      error number
*************/
rsiResult rsiExit()
{
#ifndef __AMIGA__
	if (hKeyFile)
	{
		FreeLibrary(hKeyFile);
		hKeyFile = NULL;
	}
#endif

	return ERR_NONE;
}

/*************
* DESCRIPTION: Allocate a render context
* INPUT:       rc    render context
* OUTPUT:      -
*************/
/*#ifdef __PPC__
rsiResult rsiCreateContext(struct Library *RayStormBase, RSICONTEXT **rc)
#else*/
rsiResult rsiCreateContext(RSICONTEXT **rc)
//#endif
{
	*rc = new RSICONTEXT;
	if(!*rc)
		return ERR_MEM;

	return ERR_NONE;
}

/*************
* DESCRIPTION: Free a allocated render context
* INPUT:       rc    render context
* OUTPUT:      -
*************/
rsiResult rsiFreeContext(CTXT)
{
	delete rc;

	return ERR_NONE;
}

/*************
* DESCRIPTION: Set the user data of the context
* INPUT:       rc    render context
* OUTPUT:      -
*************/
rsiResult rsiSetUserData(CTXT, void *data)
{
	rc->data = data;
	return ERR_NONE;
}

/*************
* DESCRIPTION: Set the user data of the context
* INPUT:       rc    render context
* OUTPUT:      -
*************/
rsiResult rsiGetUserData(CTXT, void **data)
{
	*data = rc->data;
	return ERR_NONE;
}

/*************
* DESCRIPTION: returns error message string to a give error number
* INPUT:       buf   buffer which gets the error string
*              no    error number
* OUTPUT:      -
*************/
rsiResult rsiGetErrorMsg(CTXT, char *buf, int no)
{
	if(no < ERR_NONE || no > ERR_EXTERNAL)
		no = ERR_WRONGERROR;

	if(no == ERR_EXTERNAL)
		strcpy(buf, rc->externalerror);
	else
		strcpy(buf, errors[no]);

	return ERR_NONE;
}

/*************
* DESCRIPTION: starts rendering process
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTRenderFlags      float    flags: QUICK
*              rsiTRenderTimeBegin  float    start time
*              rsiTRenderTimeEnd    float    end time  (for animation/motion blur)
*              ----------------------------------------------------
* INPUT:
* OUTPUT:      -
*************/
rsiResult rsiRender(CTXT, UBYTE **scr, ...)
{
	TIME time = {0,0};
	int error;
	va_list list;
	int flags;

	static PARAM para[] =
	{
		{rsiTRenderFlags, rsiTypeInt, NULL},
		{rsiTRenderTimeBegin, rsiTypeFloat, NULL},
		{rsiTRenderTimeEnd, rsiTypeFloat, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&flags;
	para[1].data = (void*)&time.begin;
	para[2].data = (void*)&time.end;

	va_start(list, scr);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(flags & WORLD_QUICK)
		rc->world.flags |= WORLD_QUICK;
	else
		rc->world.flags &= ~WORLD_QUICK;

	if(time.begin > time.end)
		return ERR_TIME;

#ifdef __STATISTICS__
	InitStatistics();
#endif

	error = Raytrace(rc, &time);
	*scr = (UBYTE*)rc->scrarray;

	return error;
}

/*************
* DESCRIPTION: sets the depth of the octree
*              values from 0 to 8 are allowed
* INPUT:       depth    octree depth
* OUTPUT:      -
*************/
rsiResult rsiSetOctreeDepth(CTXT, int depth)
{
	if(depth > MAXOCTREEDEPTH || depth < 0)
		return ERR_OCTREEDEPTH;
	rc->world.depth = depth;
	return ERR_NONE;
}

/*************
* DESCRIPTION: sets the field rendering area
* INPUT:       left,top,right,bottom   data of the rectangle
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetRenderField(CTXT, int left, int top, int right, int bottom)
{
	if(left == rsiDEFAULT)
		left = 0;
	if(right == rsiDEFAULT)
		right = rc->camera.xres - 1;
	if(bottom == rsiDEFAULT)
		bottom = rc->camera.yres - 1;
	if(top == rsiDEFAULT)
		top = 0;

	if(left < 0)
		left = 0;
	if(left > rc->camera.xres - 1)
		left = rc->camera.xres -  1;
	rc->camera.left = left;

	if(top < 0)
		top = 0;
	if(top > rc->camera.yres - 1)
		top = rc->camera.yres - 1;
	rc->camera.top = top;

	if(right < rc->camera.left)
		right = rc->camera.left;
	if(right > rc->camera.xres - 1)
		right = rc->camera.xres - 1;
	rc->camera.right = right;

	if(bottom < rc->camera.top)
		bottom = rc->camera.top;
	if(bottom > rc->camera.yres - 1)
		bottom = rc->camera.yres - 1;
	rc->camera.bottom = bottom;

	return ERR_NONE;
}

/*************
* DESCRIPTION: set antialiasing factor
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTAntiSamples      int      antialiasing samples
*              rsiTAntiFilter       float    filter value
*              rsiTAntiContrast     COLOR    contrast color
*              ----------------------------------------------------
* INPUT:       samples  number of antialiasing samples
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetAntialias(CTXT, ...)
{
	va_list list;
	COLOR *contrast=NULL;
	int samples;
	static PARAM para[] =
	{
		{rsiTAntiSamples, rsiTypeInt, NULL},
		{rsiTAntiFilter, rsiTypeFloat, NULL},
		{rsiTAntiContrast, rsiTypePointer, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&samples;
	para[1].data = (void*)&rc->world.filterwidth;
	para[2].data = (void*)&contrast;

	va_start(list, rc);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(samples > 8 || samples < 1)
		return ERR_ANTIALIAS;
	rc->world.samples = ULONG(samples);

	if(contrast)
		rc->world.contrast = *contrast;

	return ERR_NONE;
}

/*************
* DESCRIPTION: set distribution value for motion blur and soft shadows
* INPUT:       ms    distrib value for motion blur
*              ss    distrib value for soft shadows
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetDistribution(CTXT, int ms, int ss)
{
	if((ms < 1) || (ms > 8) ||
		(ss < 1) || (ss > 8))
		return ERR_DISTRIB;

	rc->world.motionblur_samples = ms*ms;
	rc->world.softshadow_samples = ss*ss;

	return ERR_NONE;
}

/*************
* DESCRIPTION: enables/disables random jittering
* INPUT:       randjit     TRUE random jitter, FALSE no random jitter
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetRandomJitter(CTXT, BOOL randjit)
{
	if(randjit)
		rc->world.flags |= WORLD_RANDOM_JITTER;
	else
		rc->world.flags &= ~WORLD_RANDOM_JITTER;

	return ERR_NONE;
}

/*************
* DESCRIPTION: set world attributes
*              tag names               type     meaning
*              ----------------------------------------------------
*              rsiTWorldBackground        COLOR    background color
*              rsiTWorldAmbient           COLOR    ambient color of world
*              rsiTWorldFogLength         float    fog length
*              rsiTWorldFogHeight         float    fog height
*              rsiTWorldFogColor          COLOR    color of fog
*              rsiTWorldBackdropPic       char*    name of backdrop picture
*              rsiTWorldReflectionMap     char*    name of reflection map
*              rsiTWorldMinObjects        ULONG    minimum objects for octree
*              rsiTWorldHyperSteps        ULONG    step value for hypertextures
*              rsiTWorldHyperMinDeltaDens float    minimum delta density for hypertextures
*              rsiTWorldHyperMaxDeltaDens float    maximum delta density for hypertextures
*              ----------------------------------------------------
* INPUT:       rc    render context
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetWorld(CTXT, ...)
{
	char *err;
	int error;
	COLOR *backgroundcolor=NULL, *ambientcolor=NULL, *fogcolor=NULL;
	char *backpic=NULL, *reflpic=NULL;
	va_list list;
	static PARAM para[] =
	{
		{rsiTWorldBackground, rsiTypePointer, NULL},
		{rsiTWorldAmbient, rsiTypePointer, NULL},
		{rsiTWorldFogLength, rsiTypeFloat, NULL},
		{rsiTWorldFogHeight, rsiTypeFloat, NULL},
		{rsiTWorldFogColor, rsiTypePointer, NULL},
		{rsiTWorldBackdropPic, rsiTypePointer, NULL},
		{rsiTWorldReflectionMap, rsiTypePointer, NULL},
		{rsiTWorldMinObjects, rsiTypeInt, NULL},
		{rsiTWorldHyperSteps, rsiTypeInt, NULL},
		{rsiTWorldHyperMinDeltaDens, rsiTypeFloat, NULL},
		{rsiTWorldHyperMaxDeltaDens, rsiTypeFloat, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&backgroundcolor;
	para[1].data = (void*)&ambientcolor;
	para[2].data = (void*)&rc->world.foglength;
	para[3].data = (void*)&rc->world.fogheight;
	para[4].data = (void*)&fogcolor;
	para[5].data = (void*)&backpic;
	para[6].data = (void*)&reflpic;
	para[7].data = (void*)&rc->world.minobjects;
	para[8].data = (void*)&rc->world.hyper_steps;
	para[9].data = (void*)&rc->world.hyper_min_density_step;
	para[10].data = (void*)&rc->world.hyper_max_density_step;

	va_start(list, rc);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(backgroundcolor)
		rc->world.back = *backgroundcolor;
	if(ambientcolor)
		rc->world.ambient = *ambientcolor;
	if(fogcolor)
		rc->world.fog = *fogcolor;
	if(backpic)
	{
		if(!rc->world.SetBackdropPic(backpic))
			return ERR_MEM;
	}
	if(reflpic)
	{
		if(rc->world.reflmap)
			delete rc->world.reflmap;

		rc->world.reflmap = new IMAGE;
		if(!rc->world.reflmap)
			return ERR_MEM;

		sprintf(rc->buf, "Loading reflection map \"%s\"", reflpic);
		rc->Log(rc, rc->buf);

		error = rc->world.reflmap->Load(rc, reflpic, rc->world.brushpath);
		if(error)
		{
			delete rc->world.reflmap;
			rc->world.reflmap = NULL;
			if(error == ERR_EXTERNAL)
			{
				err = rc->GetImageError();
				if(err)
					strcpy(rc->externalerror, err);
				else
					rc->externalerror[0] = 0;
			}
			return error;
		}
	}

	return ERR_NONE;
}

/*************
* DESCRIPTION: set camera attributes
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTCameraPos        VECTOR   position
*              rsiTCameraLook       VECTOR   look point of camera
*              rsiTCameraViewUp     VECTOR   view up vector
*              rsiTCameraPosActor   ACTOR    pointer to camera actor
*              rsiTCameraLookActor  ACTOR    pointer to camera look_at actor
*              rsiTCameraHFov       float    horizontal field of view
*              rsiTCameraVFov       float    vertical field of view
*              rsiTCameraFocalDist  float    distance from viewer to focal plane
*              rsiTCameraAperture   float    aperture value
*              rsiTCameraFlags      int      camera flags (fast DOF...)
*              ----------------------------------------------------
* INPUT:       pos   position of camera
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetCamera(CTXT, ...)
{
	VECTOR *pos=NULL,*look=NULL,*viewup=NULL;
	VECTOR dir, scrni;
	va_list list;

	static PARAM para[] =
	{
		{rsiTCameraPos, rsiTypePointer, NULL},
		{rsiTCameraLook, rsiTypePointer, NULL},
		{rsiTCameraViewUp, rsiTypePointer, NULL},
		{rsiTCameraPosActor, rsiTypePointer, NULL},
		{rsiTCameraLookActor, rsiTypePointer, NULL},
		{rsiTCameraHFov, rsiTypeFloat, NULL},
		{rsiTCameraVFov, rsiTypeFloat, NULL},
		{rsiTCameraFocalDist, rsiTypeFloat, NULL},
		{rsiTCameraAperture, rsiTypeFloat, NULL},
		{rsiTCameraFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos;
	para[1].data = (void*)&look;
	para[2].data = (void*)&viewup;
	para[3].data = (void*)&rc->camera.posactor;
	para[4].data = (void*)&rc->camera.viewactor;
	para[5].data = (void*)&rc->camera.hfov;
	para[6].data = (void*)&rc->camera.vfov;
	para[7].data = (void*)&rc->camera.focaldist;
	para[8].data = (void*)&rc->camera.aperture;
	para[9].data = (void*)&rc->camera.flags;

	va_start(list, rc);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		rc->camera.pos = *pos;
	if(look)
		rc->camera.lookp = *look;
	if(viewup)
		rc->camera.vup = *viewup;

	// check if vup and dir are identical
	VecSub(&rc->camera.lookp, &rc->camera.pos, &dir);
	VecNormalize(&dir);
	if(VecNormCross(&dir, &rc->camera.vup, &scrni) == 0)
		return ERR_VIEW;

	if(rc->camera.aperture > 0)
		rc->world.flags |= WORLD_DOF;
	else
		rc->world.flags &= ~WORLD_DOF;

	return ERR_NONE;
}

/*************
* DESCRIPTION: set render resolution
* INPUT:       xres, yres  resolution of picture
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetScreen(CTXT, int xres, int yres)
{
	if(xres>0)
		rc->camera.xres = xres;
	if(yres>0)
		rc->camera.yres = yres;

	return ERR_NONE;
}

/*************
* DESCRIPTION: sets the path where brushes can be found
* INPUT:       path  directory string
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetBrushPath(CTXT, char *path)
{
	if(rc->world.brushpath)
		delete rc->world.brushpath;
	rc->world.brushpath = new char[strlen(path) + 1];
	strcpy(rc->world.brushpath, path);

	return ERR_NONE;
}

/*************
* DESCRIPTION: sets the path where textures can be found
* INPUT:       path  directory string
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetTexturePath(CTXT, char *path)
{
	if(rc->world.texturepath)
		delete rc->world.texturepath;
	rc->world.texturepath = new char[strlen(path) + 1];
	strcpy(rc->world.texturepath, path);

	return ERR_NONE;
}

/*************
* DESCRIPTION: sets the path where objects can be found
* INPUT:       path  directory string
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetObjectPath(CTXT, char *path)
{
	if(rc->world.objectpath)
		delete rc->world.objectpath;
	rc->world.objectpath = new char[strlen(path) + 1];
	strcpy(rc->world.objectpath, path);

	return ERR_NONE;
}

/*************
* DESCRIPTION: saves the picture
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTPicType          char*    name of picture type (ilbm, tga, png, jpeg, ...)
*              rsiTPicJPEGQuality   int      jpeg quality (0...100)
*              ----------------------------------------------------
* INPUT:       filename    filename of picture
* OUTPUT:      rsiResult
*************/
rsiResult rsiSavePicture(CTXT, char *filename, ...)
{
	BOOL error;
	IMAGE image;
	char *type=NULL;
	va_list list;
	char *err;
	int quality = -1;
	JPEG_PARAM jpegparam;
	void *param = NULL;

	static PARAM para[] =
	{
		{rsiTPicType,        rsiTypePointer, NULL},
		{rsiTPicJPEGQuality, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&type;
	para[1].data = (void*)&quality;

	va_start(list, filename);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(!rc->scrarray)
		return ERR_NOPIC;

	sprintf(rc->buf, "Saving picture as: \"%s\"", filename);
	rc->Log(rc, rc->buf);

	image.name = filename;
	image.width = rc->camera.xres;
	image.height = rc->camera.yres;
	image.colormap = rc->scrarray;

	if(quality != -1)
	{
		jpegparam.quality = quality;
		param = &jpegparam;
	}

	// save image in given type
	if(type)
		error = image.Save(rc, type, param);
	else
#ifndef __AMIGA__
		error = image.Save(rc, "tga", param);
#else
		error = image.Save(rc, "ilbm", param);
#endif

	// set pointer to NULL because of destructor for image
	image.name = NULL;
	image.colormap = NULL;

	if(error)
	{
		err = rc->GetImageError();
		if(err)
		{
			strcpy(rc->externalerror, err);
			return ERR_EXTERNAL;
		}
		else
			return error;
	}

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a point light
* INPUT:       lightp   pointer pointer which retrieves the pointer to
*                       created pointlight
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreatePointLight(CTXT, POINT_LIGHT **lightp)
{
	POINT_LIGHT *light;

	light = new POINT_LIGHT;
	if(!light)
		return ERR_MEM;

	*lightp = light;

	rc->Insert(light);

	return ERR_NONE;
}

/*************
* DESCRIPTION: set attributes of point light
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTLightPos         VECTOR   position of light
*              rsiTLightColor       VECTOR   color of light
*              rsiTLightFlags       int      casts shadows/has flares/has star/has shadowmap ?
*              rsiTLightRadius      float    radius of light source
*              rsiTLightActor       ACTOR    light actor
*              rsiTLightShadowMapSize int    size of shadow map
*              ----------------------------------------------------
* INPUT:       light    pointer of lightsource to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetPointLight(CTXT, POINT_LIGHT *light, ...)
{
	VECTOR *pos=NULL;
	COLOR *color=NULL;
	va_list list;
	ULONG flags = -1;

	static PARAM para[] =
	{
		{rsiTLightPos, rsiTypePointer, NULL},
		{rsiTLightColor, rsiTypePointer, NULL},
		{rsiTLightFlags, rsiTypeInt, NULL},
		{rsiTLightRadius, rsiTypeFloat, NULL},
		{rsiTLightActor, rsiTypePointer, NULL},
		{rsiTLightFallOff, rsiTypeFloat, NULL},
		{rsiTLightShadowMapSize, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos;
	para[1].data = (void*)&color;
	para[2].data = (void*)&flags;
	para[3].data = (void*)&light->r;
	para[4].data = (void*)&light->actor;
	para[5].data = (void*)&light->falloff;
	para[6].data = (void*)&light->shadowmap_size;

	va_start(list, light);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		light->pos = *pos;
	if(color)
		light->color = *color;

	if(flags != -1)
	{
		if(flags & rsiFLightShadow)
			light->flags |= LIGHT_SHADOW;
		if(flags & rsiFLightFlares)
			light->flags |= LIGHT_FLARES;
		if(flags & rsiFLightStar)
			light->flags |= LIGHT_STAR;
		if(flags & rsiFLightShadowMap)
			light->flags |= LIGHT_SHADOWMAP;
	}

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a spot light
* INPUT:       lightp   pointer pointer which retrieves the pointer to
*                       created spotlight
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateSpotLight(CTXT, SPOT_LIGHT **lightp)
{
	SPOT_LIGHT *light;

	light = new SPOT_LIGHT;
	if(!light)
		return ERR_MEM;

	*lightp = light;

	rc->Insert(light);

	return ERR_NONE;
}

/*************
* DESCRIPTION: set attributes of spotlight
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTLightPos         VECTOR   position of light
*              rsiTLightColor       VECTOR   color of light
*              rsiTLightFlags       int      casts shadows/has flares/has star/has shadowmap ?
*              rsiTLightRadius      float    radius of light source
*              rsiTLightActor       ACTOR    light actor
*              rsiTLightLookPActor  ACTOR    look at actor
*              rsiTLightFallOff     float    fall off value
*              rsiTLightLookPoint   VECTOR   look point
*              rsiTLightAngle       float    opening angle
*              rsiTLightShadowMapSize int    size of shadow map
*              rsiTLightFallOffRadius float  brightness is full at falloff_radius*angle and zero at angle
*              ----------------------------------------------------
* INPUT:       light    pointer of lightsource to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetSpotLight(CTXT, SPOT_LIGHT *light, ...)
{
	VECTOR *pos=NULL;
	COLOR *color=NULL;
	VECTOR *look=NULL;
	float angle = -1.;
	ULONG flags = -1;
	va_list list;

	static PARAM para[] =
	{
		{rsiTLightPos, rsiTypePointer, NULL},
		{rsiTLightColor, rsiTypePointer, NULL},
		{rsiTLightFlags, rsiTypeInt, NULL},
		{rsiTLightRadius, rsiTypeFloat, NULL},
		{rsiTLightActor, rsiTypePointer, NULL},
		{rsiTLightLookPActor, rsiTypePointer, NULL},
		{rsiTLightFallOff, rsiTypeFloat, NULL},
		{rsiTLightLookPoint, rsiTypePointer, NULL},
		{rsiTLightAngle, rsiTypeFloat, NULL},
		{rsiTLightShadowMapSize, rsiTypeInt, NULL},
		{rsiTLightFallOffRadius, rsiTypeFloat, NULL},
		{rsiTDone, 0, 0}
	};


	para[0].data = (void*)&pos;
	para[1].data = (void*)&color;
	para[2].data = (void*)&flags;
	para[3].data = (void*)&light->r;
	para[4].data = (void*)&light->actor;
	para[5].data = (void*)&light->lookp_actor;
	para[6].data = (void*)&light->falloff;
	para[7].data = (void*)&look;
	para[8].data = (void*)&angle;
	para[9].data = (void*)&light->shadowmap_size;
	para[10].data = (void*)&light->falloff_radius;

	va_start(list, light);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		light->pos = *pos;
	if(color)
		light->color = *color;
	if(look)
		light->lookp = *look;
	if(angle >= 0.)
		light->angle = (float)cos(angle*PI_180);

	if(flags != -1)
	{
		if(flags & rsiFLightShadow)
			light->flags |= LIGHT_SHADOW;
		if(flags & rsiFLightFlares)
			light->flags |= LIGHT_FLARES;
		if(flags & rsiFLightStar)
			light->flags |= LIGHT_STAR;
		if(flags & rsiFLightShadowMap)
			light->flags |= LIGHT_SHADOWMAP;
	}

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a directional light
* INPUT:       lightp   pointer pointer which retrieves the pointer to
*                       created directional light
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateDirectionalLight(CTXT, DIRECTIONAL_LIGHT **lightp)
{
	DIRECTIONAL_LIGHT *light;

	light = new DIRECTIONAL_LIGHT;
	if(!light)
		return ERR_MEM;

	*lightp = light;

	rc->Insert(light);

	return ERR_NONE;
}

/*************
* DESCRIPTION: set attributes of directional light
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTLightPos         VECTOR   position of light
*              rsiTLightDir         VECTOR   direction the light is heading to
*              rsiTLightColor       VECTOR   color of light
*              rsiTLightFlags       int      casts shadows/has flares/has star/has shadowmap ?
*              rsiTLightRadius      float    radius of light source
*              rsiTLightActor       ACTOR    light actor
*              rsiTLightShadowMapSize int    size of shadow map
*              ----------------------------------------------------
* INPUT:       light    pointer of lightsource to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetDirectionalLight(CTXT, DIRECTIONAL_LIGHT *light, ...)
{
	VECTOR *pos=NULL, *dir=NULL;
	COLOR *color=NULL;
	va_list list;
	ULONG flags = -1;

	static PARAM para[] =
	{
		{rsiTLightPos, rsiTypePointer, NULL},
		{rsiTLightDir, rsiTypePointer, NULL},
		{rsiTLightColor, rsiTypePointer, NULL},
		{rsiTLightFlags, rsiTypeInt, NULL},
		{rsiTLightRadius, rsiTypeFloat, NULL},
		{rsiTLightActor, rsiTypePointer, NULL},
		{rsiTLightFallOff, rsiTypeFloat, NULL},
		{rsiTLightShadowMapSize, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos;
	para[1].data = (void*)&dir;
	para[2].data = (void*)&color;
	para[3].data = (void*)&flags;
	para[4].data = (void*)&light->r;
	para[5].data = (void*)&light->actor;
	para[6].data = (void*)&light->falloff;
	para[7].data = (void*)&light->shadowmap_size;

	va_start(list, light);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		light->pos = *pos;
	if(dir)
		light->dir = *dir;
	if(color)
		light->color = *color;

	if(flags != -1)
	{
		if(flags & rsiFLightShadow)
			light->flags |= LIGHT_SHADOW;
		if(flags & rsiFLightFlares)
			light->flags |= LIGHT_FLARES;
		if(flags & rsiFLightStar)
			light->flags |= LIGHT_STAR;
		if(flags & rsiFLightShadowMap)
			light->flags |= LIGHT_SHADOWMAP;
	}

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a flare and adds it to a light source
* INPUT:       light    light source to add the flare to
*              flare    pointer pointer which retrieves the created flare
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateFlare(CTXT, LIGHT *light, FLARE **flare)
{
	*flare = new FLARE;
	if(!*flare)
		return ERR_MEM;

	(*flare)->Insert((SLIST**)&light->flares);

	return ERR_NONE;
}

/*************
* DESCRIPTION: sets flare attributes
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTFlareType        int      type of flare (FLARE_ROUND, FLARE_POLY)
*              rsiTFlareFunc        int      intensity function (FLARE_PLAIN, FLARE_EXP...)
*              rsiTFlarePos         float    position of flare on the flare axis
*              rsiTFlareColor       COLOR    color of flare
*              rsiTFlareRadius      float    radius of flare
*              rsiTFlareTilt        float    tilt angle of polygonal flare (in degrees !)
*              rsiTFlareEdges       int      number of edges of polygonal flare
*              ----------------------------------------------------
* INPUT:       flare    lens flare to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetFlare(CTXT, FLARE *flare, ...)
{
	COLOR *color = NULL;
	va_list list;

	if(!flare)
		return ERR_PARAM;

	static PARAM para[] =
	{
		{rsiTFlareType, rsiTypeInt, NULL},
		{rsiTFlareFunc, rsiTypeInt, NULL},
		{rsiTFlarePos, rsiTypeFloat, NULL},
		{rsiTFlareColor, rsiTypePointer, NULL},
		{rsiTFlareRadius, rsiTypeFloat, NULL},
		{rsiTFlareEdges, rsiTypeInt, NULL},
		{rsiTFlareTilt, rsiTypeFloat, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&flare->type;
	para[1].data = (void*)&flare->func;
	para[2].data = (void*)&flare->pos;
	para[3].data = (void*)&color;
	para[4].data = (void*)&flare->radius;
	para[5].data = (void*)&flare->edges;
	para[6].data = (void*)&flare->tilt;

	va_start(list, flare);
	if(GetParams(&list, para))
	{
		delete flare;
		return ERR_PARAM;
	}

	if(color)
		flare->color = *color;

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a star and adds it to a light source
* INPUT:       light    light source to add the star to
*              star     pointer pointer which retrieves the created star
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateStar(CTXT, LIGHT *light, STAR **star)
{
	*star = new STAR;
	if(!*star)
		return ERR_MEM;

	if(light->star)
		delete light->star;

	light->star = *star;

	return ERR_NONE;
}

/*************
* DESCRIPTION: set the attributes of a star
*              tag names               type     meaning
*              ----------------------------------------------------
*              rsiTStarFlags           int      flags (star enabled, halo enabled)
*              rsiTStarColor           COLOR    color of star
*              rsiTStarRingColor       COLOR    color of ring
*              rsiTStarRadius          float    radius of star
*              rsiTStarHaloRadius      float    radius of halo (in relation to star radius!)
*              rsiTStarInnerHaloRadius float    radius of inner halo (in relation to halo radius!)
*              rsiTStarSpikes          int      exponent number of spikes (i.e. 3 -> 2^3 = 8 spikes)
*              rsiTStarIntensities     float*   pointer to intensities
*              rsiTStarSpikeWidth      float    width of spikes
*              rsiTStarTilt            float    tilt angle
*              rsiTStarIntensities     float    brightness
*              ----------------------------------------------------
* INPUT:       light    light source to add the star to
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetStar(CTXT, STAR *star, ...)
{
	float *intensities = NULL;
	COLOR *starcolor = NULL;
	COLOR *ringcolor = NULL;
	va_list list;

	static PARAM para[] =
	{
		{rsiTStarFlags, rsiTypeInt, NULL},
		{rsiTStarColor, rsiTypePointer, NULL},
		{rsiTStarRingColor, rsiTypePointer, NULL},
		{rsiTStarRadius, rsiTypeFloat, NULL},
		{rsiTStarHaloRadius, rsiTypeFloat, NULL},
		{rsiTStarInnerHaloRadius, rsiTypeFloat, NULL},
		{rsiTStarSpikes, rsiTypeInt, NULL},
		{rsiTStarIntensities, rsiTypePointer, NULL},
		{rsiTStarBrightness, rsiTypeFloat, NULL},
		{rsiTStarNoise, rsiTypeFloat, NULL},
		{rsiTStarTilt, rsiTypeFloat, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&star->flags;
	para[1].data = (void*)&starcolor;
	para[2].data = (void*)&ringcolor;
	para[3].data = (void*)&star->starradius;
	para[4].data = (void*)&star->haloradius;
	para[5].data = (void*)&star->innerhaloradius;
	para[6].data = (void*)&star->spikes;
	para[7].data = (void*)&intensities;
	para[8].data = (void*)&star->brightness;
	para[9].data = (void*)&star->noise;
	para[10].data = (void*)&star->tilt;

	va_start(list, star);
	if(GetParams(&list, para))
	{
		delete star;
		return ERR_PARAM;
	}

	if(starcolor)
		star->starcolor = *starcolor;
	if(ringcolor)
		star->ringcolor = *ringcolor;
	if(intensities)
	{
		if(star->intensities)
			delete star->intensities;

		star->intensities = new float[star->spikes];
		if(!star->intensities)
		{
			delete star;
			return ERR_MEM;
		}
		memcpy(star->intensities, intensities, sizeof(float) * star->spikes);
	}

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a sphere
* INPUT:       spherep  pointer pointer which retrieves the pointer to
*                       created sphere
*              srf      surface of sphere
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateSphere(CTXT, SPHERE **spherep, SURFACE *srf)
{
	SPHERE *sphere;

	sphere = new SPHERE;
	if(!sphere)
		return ERR_MEM;

	sphere->surf = srf;

	*spherep = sphere;

	rc->Insert(sphere);

	return ERR_NONE;
}

/*************
* DESCRIPTION: set sphere properties
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTSpherePos        VECTOR   position of sphere
*              rsiTSphereRadius     float    radius of sphere
*              rsiTSphereFuzzy      float    size of fuzzy region (0..1)
*              rsiTSphereActor      ACTOR    actor
*              rsiTSphereFlags      int      flags (invert)
*              ----------------------------------------------------
* INPUT:       sphere   pointer of sphere to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetSphere(CTXT, SPHERE *sphere, ...)
{
	VECTOR *pos=NULL;
	va_list list;
	int flags=0;

	static PARAM para[] =
	{
		{rsiTSpherePos, rsiTypePointer, NULL},
		{rsiTSphereRadius, rsiTypeFloat, NULL},
		{rsiTSphereFuzzy, rsiTypeFloat, NULL},
		{rsiTSphereActor, rsiTypePointer, NULL},
		{rsiTSphereFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos;
	para[1].data = (void*)&sphere->r;
	para[2].data = (void*)&sphere->ir;
	para[3].data = (void*)&sphere->actor;
	para[4].data = (void*)&flags;

	va_start(list, sphere);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		sphere->pos = *pos;

	if(flags & rsiFSphereInverted)
		sphere->flags |= OBJECT_INVERTED;

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a box
* INPUT:       boxp  pointer pointer which retrieves the pointer to
*                    created box
*              srf   surface of box
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateBox(CTXT, BOX **boxp, SURFACE *srf)
{
	BOX *box;

	box = new BOX;
	if(!box)
		return ERR_MEM;

	box->surf = srf;

	*boxp = box;

	rc->Insert(box);

	return ERR_NONE;
}

/*************
* DESCRIPTION: set box properties
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTBoxPos           VECTOR   position of box
*              rsiTBoxLowBounds     VECTOR   left, front, bottom edge
*              rsiTBoxHighBounds    VECTOR   right, back, top edge
*              rsiTBoxAlign         VECTOR   alignment of the box
*              rsiTBoxOrientX       VECTOR   x orientation
*              rsiTBoxOrientY       VECTOR   y orientation
*              rsiTBoxOrientZ       VECTOR   z orientation
*              rsiTBoxActor         ACTOR    actor
*              rsiTBoxFlags         int      flags (inverted)
*              ----------------------------------------------------
* INPUT:       box   pointer of box to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetBox(CTXT, BOX *box, ...)
{
	VECTOR *pos=NULL, *lowbounds=NULL, *highbounds=NULL, *align=NULL, *orient_x=NULL, *orient_y=NULL, *orient_z=NULL;
	va_list list;
	int flags=0;

	static PARAM para[] =
	{
		{rsiTBoxPos, rsiTypePointer, NULL},
		{rsiTBoxLowBounds, rsiTypePointer, NULL},
		{rsiTBoxHighBounds, rsiTypePointer, NULL},
		{rsiTBoxAlign, rsiTypePointer, NULL},
		{rsiTBoxOrientX, rsiTypePointer, NULL},
		{rsiTBoxOrientY, rsiTypePointer, NULL},
		{rsiTBoxOrientZ, rsiTypePointer, NULL},
		{rsiTBoxActor, rsiTypePointer, NULL},
		{rsiTBoxFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos;
	para[1].data = (void*)&lowbounds;
	para[2].data = (void*)&highbounds;
	para[3].data = (void*)&align;
	para[4].data = (void*)&orient_x;
	para[5].data = (void*)&orient_y;
	para[6].data = (void*)&orient_z;
	para[7].data = (void*)&box->actor;
	para[8].data = (void*)&flags;

	va_start(list, box);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		box->pos = *pos;
	if(lowbounds)
		box->lowbounds = *lowbounds;
	if(highbounds)
		box->highbounds = *highbounds;
	if(align)
		CalcOrient(align, &box->orient_x, &box->orient_y, &box->orient_z);
	if(orient_x)
		box->orient_x = *orient_x;
	if(orient_y)
		box->orient_y = *orient_y;
	if(orient_z)
		box->orient_z = *orient_z;

	InvOrient(&box->orient_x, &box->orient_y, &box->orient_z,
		&box->orient_x, &box->orient_y, &box->orient_z);

	if(flags & rsiFBoxInverted)
		box->flags |= OBJECT_INVERTED;

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a cylinder
* INPUT:       cylinderp  pointer pointer which retrieves the pointer to
*                    created cylinder
*              srf   surface of cylinder
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateCylinder(CTXT, CYLINDER **cylinderp, SURFACE *srf)
{
	CYLINDER *cylinder;

	cylinder = new CYLINDER;
	if(!cylinder)
		return ERR_MEM;

	cylinder->surf = srf;

	*cylinderp = cylinder;

	rc->Insert(cylinder);

	return ERR_NONE;
}

/*************
* DESCRIPTION: set cylinder properties
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTCylinderPos      VECTOR   position of cylinder
*              rsiTCylinderSize     float    height of cylinder
*              rsiTCylinderAlign    VECTOR   alignment of the cylinder
*              rsiTCylinderOrientX  VECTOR   x orientation
*              rsiTCylinderOrientY  VECTOR   y orientation
*              rsiTCylinderOrientZ  VECTOR   z orientation
*              rsiTCylinderActor    ACTOR    actor
*              rsiTCylinderFlags    int      flags (inverted, close top, close bottom)
*              ----------------------------------------------------
* INPUT:       cylinder    pointer of cylinder to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetCylinder(CTXT, CYLINDER *cylinder, ...)
{
	VECTOR *pos=NULL, *size=NULL, *align=NULL, *orient_x=NULL, *orient_y=NULL, *orient_z=NULL;
	va_list list;
	int flags=0;

	static PARAM para[] =
	{
		{rsiTCylinderPos, rsiTypePointer, NULL},
		{rsiTCylinderSize, rsiTypePointer, NULL},
		{rsiTCylinderAlign, rsiTypePointer, NULL},
		{rsiTCylinderOrientX, rsiTypePointer, NULL},
		{rsiTCylinderOrientY, rsiTypePointer, NULL},
		{rsiTCylinderOrientZ, rsiTypePointer, NULL},
		{rsiTCylinderActor, rsiTypePointer, NULL},
		{rsiTCylinderFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos;
	para[1].data = (void*)&size;
	para[2].data = (void*)&align;
	para[3].data = (void*)&orient_x;
	para[4].data = (void*)&orient_y;
	para[5].data = (void*)&orient_z;
	para[6].data = (void*)&cylinder->actor;
	para[7].data = (void*)&flags;

	va_start(list, cylinder);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		cylinder->pos = *pos;
	if(size)
		cylinder->size = *size;
	if(align)
		CalcOrient(align, &cylinder->ox, &cylinder->oy, &cylinder->oz);
	if(orient_x)
		cylinder->ox = *orient_x;
	if(orient_y)
		cylinder->oy = *orient_y;
	if(orient_z)
		cylinder->oz = *orient_z;

	if(flags & rsiFCylinderInverted)
		cylinder->flags |= OBJECT_INVERTED;

	if(flags & rsiFCylinderOpenTop)
		cylinder->flags &= ~OBJECT_CLOSETOP;

	if(flags & rsiFCylinderOpenBottom)
		cylinder->flags &= ~OBJECT_CLOSEBOTTOM;

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a surface of revolution
* INPUT:       sorp     pointer pointer which retrieves the pointer to
*                       created sor
*              srf      surface of sor
*              number   amount of points (minimum 4)
*              p        points which define the surface (z is ignored)
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateSOR(CTXT, SOR **sorp, SURFACE *srf, int number, VECT2D *p)
{
	SOR *sor;
	int i;

	if(number < 4)
		return ERR_SORNUMBER;

	for(i=0; i<number-1; i++)
	{
		if(fabs(p[i].y - p[i+1].y) < EPSILON)
			return ERR_SORPOINT;
	}

	sor = new SOR;
	if(!sor)
		return ERR_MEM;

	sor->surf = srf;

	sor->number = number-3;
	if(!sor->Compute(p))
	{
		delete sor;
		return ERR_MEM;
	}

	*sorp = sor;

	rc->Insert(sor);

	return ERR_NONE;
}

/*************
* DESCRIPTION: set surface of revolution properties
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTSORPos           VECTOR   position
*              rsiTSORSize          VECTOR   size
*              rsiTSORAlign         VECTOR   alignment of the sor
*              rsiTSOROrientX       VECTOR   x orientation
*              rsiTSOROrientY       VECTOR   y orientation
*              rsiTSOROrientZ       VECTOR   z orientation
*              rsiTSORActor         ACTOR    actor
*              rsiTSORFlags         int      flags (inverted, close top, close bottom)
*              ----------------------------------------------------
* INPUT:       sor      pointer of sor to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetSOR(CTXT, SOR *sor, ...)
{
	VECTOR *pos=NULL, *size=NULL, *align=NULL, *orient_x=NULL, *orient_y=NULL, *orient_z=NULL;
	va_list list;
	int flags=0;

	static PARAM para[] =
	{
		{rsiTSORPos, rsiTypePointer, NULL},
		{rsiTSORSize, rsiTypePointer, NULL},
		{rsiTSORAlign, rsiTypePointer, NULL},
		{rsiTSOROrientX, rsiTypePointer, NULL},
		{rsiTSOROrientY, rsiTypePointer, NULL},
		{rsiTSOROrientZ, rsiTypePointer, NULL},
		{rsiTSORActor, rsiTypePointer, NULL},
		{rsiTSORFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos;
	para[1].data = (void*)&size;
	para[2].data = (void*)&align;
	para[3].data = (void*)&orient_x;
	para[4].data = (void*)&orient_y;
	para[5].data = (void*)&orient_z;
	para[6].data = (void*)&sor->actor;
	para[7].data = (void*)&flags;

	va_start(list, sor);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		sor->pos = *pos;
	if(size)
		sor->size = *size;
	if(align)
		CalcOrient(align, &sor->orient_x, &sor->orient_y, &sor->orient_z);
	if(orient_x)
		sor->orient_x = *orient_x;
	if(orient_y)
		sor->orient_y = *orient_y;
	if(orient_z)
		sor->orient_z = *orient_z;

	if(flags & rsiFSORInverted)
		sor->flags |= OBJECT_INVERTED;

	if(flags & rsiFSOROpenTop)
		sor->flags &= ~OBJECT_CLOSETOP;

	if(flags & rsiFSOROpenBottom)
		sor->flags &= ~OBJECT_CLOSEBOTTOM;

	if(flags & rsiFSORSturm)
		sor->flags |= OBJECT_SORSTURM;

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a cone
* INPUT:       conep    pointer pointer which retrieves the pointer to
*                       created cone
*              srf      surface of sor
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateCone(CTXT, CONE **conep, SURFACE *srf)
{
	CONE *cone;

	cone = new CONE;
	if(!cone)
		return ERR_MEM;

	cone->surf = srf;

	*conep = cone;

	rc->Insert(cone);

	return ERR_NONE;
}

/*************
* DESCRIPTION: set cone properties
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTConePos          VECTOR   position
*              rsiTConeSize         VECTOR   size
*              rsiTConeAlign        VECTOR   alignment
*              rsiTConeOrientX      VECTOR   x orientation
*              rsiTConeOrientY      VECTOR   y orientation
*              rsiTConeOrientZ      VECTOR   z orientation
*              rsiTConeActor        ACTOR    actor
*              rsiTConeFlags        int      flags (inverted, close bottom)
*              ----------------------------------------------------
* INPUT:       cone     pointer of cone to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetCone(CTXT, CONE *cone, ...)
{
	VECTOR *pos=NULL, *size=NULL, *align=NULL, *orient_x=NULL, *orient_y=NULL, *orient_z=NULL;
	va_list list;
	int flags=0;

	static PARAM para[] =
	{
		{rsiTConePos, rsiTypePointer, NULL},
		{rsiTConeSize, rsiTypePointer, NULL},
		{rsiTConeAlign, rsiTypePointer, NULL},
		{rsiTConeOrientX, rsiTypePointer, NULL},
		{rsiTConeOrientY, rsiTypePointer, NULL},
		{rsiTConeOrientZ, rsiTypePointer, NULL},
		{rsiTConeActor, rsiTypePointer, NULL},
		{rsiTConeFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos;
	para[1].data = (void*)&size;
	para[2].data = (void*)&align;
	para[3].data = (void*)&orient_x;
	para[4].data = (void*)&orient_y;
	para[5].data = (void*)&orient_z;
	para[6].data = (void*)&cone->actor;
	para[7].data = (void*)&flags;

	va_start(list, cone);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		cone->pos = *pos;
	if(size)
		cone->size = *size;
	if(align)
		CalcOrient(align, &cone->ox, &cone->oy, &cone->oz);
	if(orient_x)
		cone->ox = *orient_x;
	if(orient_y)
		cone->oy = *orient_y;
	if(orient_z)
		cone->oz = *orient_z;

	if(flags & rsiFConeInverted)
		cone->flags |= OBJECT_INVERTED;

	if(flags & rsiFConeOpenBottom)
		cone->flags &= ~OBJECT_CLOSEBOTTOM;

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a triangle
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTTriangleActor       ACTOR    actor of triangle
*              rsiTTriangleNorm1       VECTOR   normal vector of first point
*              rsiTTriangleNorm2       VECTOR   normal vector of second point
*              rsiTTriangleNorm3       VECTOR   normal vector of third point
*              rsiTTriangleBrushCoord1 VECT2D   brush mapping coordinate of first point
*              rsiTTriangleBrushCoord2 VECT2D   brush mapping coordinate of second point
*              rsiTTriangleBrushCoord3 VECT2D   brush mapping coordinate of third point
*              ----------------------------------------------------
* INPUT:       srf      surface of triangle
*              v1,v2,v3 coords of the edges
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateTriangle(CTXT, SURFACE *srf, VECTOR *v1, VECTOR *v2, VECTOR *v3, ...)
{
	TRIANGLE *triangle;
	VECTOR *n1=NULL,*n2=NULL,*n3=NULL;
	VECT2D *bc1=NULL,*bc2=NULL,*bc3=NULL;
	va_list list;

	static PARAM para[] =
	{
		{rsiTTriangleActor, rsiTypePointer, NULL},
		{rsiTTriangleNorm1, rsiTypePointer, NULL},
		{rsiTTriangleNorm2, rsiTypePointer, NULL},
		{rsiTTriangleNorm3, rsiTypePointer, NULL},
		{rsiTTriangleBrushCoord1, rsiTypePointer, NULL},
		{rsiTTriangleBrushCoord2, rsiTypePointer, NULL},
		{rsiTTriangleBrushCoord3, rsiTypePointer, NULL},
		{rsiTDone, 0, 0}
	};

	triangle = new TRIANGLE;
	if(!triangle)
		return ERR_MEM;

	para[0].data = (void*)&triangle->actor;
	para[1].data = (void*)&n1;
	para[2].data = (void*)&n2;
	para[3].data = (void*)&n3;
	para[4].data = (void*)&bc1;
	para[5].data = (void*)&bc2;
	para[6].data = (void*)&bc3;

	va_start(list, v3);
	if(GetParams( &list, para ))
		return ERR_PARAM;

	if(!srf)
	{
		delete triangle;
		return ERR_SURFACE;
	}
	triangle->surf = srf;

	triangle->p[0] = *v1;
	triangle->p[1] = *v2;
	triangle->p[2] = *v3;

	if(n1 || n2 || n3)
	{
		triangle->vnorm = new VECTOR[3];
		if(!triangle->vnorm)
			return ERR_MEM;

		if(n1)
			triangle->vnorm[0] = *n1;
		if(n2)
			triangle->vnorm[1] = *n2;
		if(n3)
			triangle->vnorm[2] = *n3;
	}

	if(bc1 || bc2 || bc3)
	{
		triangle->brushcoord = new VECT2D[3];
		if(!triangle->brushcoord)
			return ERR_MEM;

		if(bc1)
			triangle->brushcoord[0] = *bc1;
		if(bc2)
			triangle->brushcoord[1] = *bc2;
		if(bc3)
			triangle->brushcoord[2] = *bc3;
	}

	rc->Insert(triangle);

	return ERR_NONE;
}

/*************
 * DESCRIPTION:   add a mesh object to scene and calculate normals
 *                tag names            type     meaning
 *                ----------------------------------------------------
 *                rsiTMeshActor        ACTOR    actor of mesh
 *                rsiTMeshPos          VECTOR
 *                rsiTMeshOrientX      VECTOR
 *                rsiTMeshOrientY      VECTOR
 *                rsiTMeshOrientZ      VECTOR
 *                rsiTMeshSize         VECTOR
 *                rsiTMeshNoPhong      int      disable phong shading
 *                rsiTMeshBrushCoords  VECT2D   brush mapping coordinates
 *                ----------------------------------------------------
 * INPUT:         srf         surface of mesh
 *                points      mesh points
 *                edges       mesh edges
 *                edgecount   amount of edges
 *                mesh        mesh triangles
 *                triacount   amount of triangles
 * OUTPUT:        rsiResult
 *************/
rsiResult rsiCreateMesh(CTXT, SURFACE *srf, VECTOR *points, EDGE *edges, int edgecount,
	MESH *mesh, int triacount, ...)
{
	ACTOR *actor = NULL;
	va_list list;
	MATRIX m,rot;
	int nophong=FALSE;
	VECTOR *pos=NULL,*ox=NULL,*oy=NULL,*oz=NULL,*size=NULL;
	VECTOR tpos,tox,toy,toz,tsize;
	VECT2D *brushcoord = NULL;

	static PARAM para[] =
	{
		{rsiTMeshActor, rsiTypePointer, NULL},
		{rsiTMeshPos, rsiTypePointer, NULL},
		{rsiTMeshOrientX, rsiTypePointer, NULL},
		{rsiTMeshOrientY, rsiTypePointer, NULL},
		{rsiTMeshOrientZ, rsiTypePointer, NULL},
		{rsiTMeshSize, rsiTypePointer, NULL},
		{rsiTMeshNoPhong, rsiTypeInt, NULL},
		{rsiTMeshBrushCoords, rsiTypePointer, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&actor;
	para[1].data = (void*)&pos;
	para[2].data = (void*)&ox;
	para[3].data = (void*)&oy;
	para[4].data = (void*)&oz;
	para[5].data = (void*)&size;
	para[6].data = (void*)&nophong;
	para[7].data = (void*)&brushcoord;

	va_start(list, triacount);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(!pos)
	{
		SetVector(&tpos, 0.f,0.f,0.f);
		pos = &tpos;
	}
	if(!ox)
	{
		SetVector(&tox, 1.f,0.f,0.f);
		ox = &tox;
	}
	if(!oy)
	{
		SetVector(&toy, 0.f,1.f,0.f);
		oy = &toy;
	}
	if(!oz)
	{
		SetVector(&tox, 0.f,0.f,1.f);
		oz = &toz;
	}
	if(!size)
	{
		SetVector(&tsize, 1.f,1.f,1.f);
		size = &tsize;
	}

	m.SetSOTMatrix(size, ox, oy, oz, pos);
	rot.SetOMatrix(ox, oy, oz);

	return !CreateMesh(rc, srf, NULL, actor, points, edges, edgecount, mesh, triacount, &m, &rot, nophong, brushcoord) ? ERR_MEM : ERR_NONE;
}

/*************
* DESCRIPTION: creates a plane
* INPUT:       planep   pointer pointer which retrieves the pointer to
*                       created plane
*              srf      surface of plane
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreatePlane(CTXT, PLANE **planep, SURFACE *srf)
{
	PLANE *plane;

	plane = new PLANE;
	if(!plane)
		return ERR_MEM;

	plane->surf = srf;

	rc->Insert(plane);

	*planep = plane;

	return ERR_NONE;
}

/*************
* DESCRIPTION: set attributes of plane
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTPlanePos         VECTOR   position of plane
*              rsiTPlaneNorm        VECTOR   normal vector of plane
*              rsiTPlaneActor       ACTOR    plane actor
*              ----------------------------------------------------
* INPUT:       plane    pointer of plane to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetPlane(CTXT, PLANE *plane, ...)
{
	VECTOR *pos=NULL,*norm=NULL;
	va_list list;

	static PARAM para[] =
	{
		{rsiTPlanePos, rsiTypePointer, NULL},
		{rsiTPlaneNorm, rsiTypePointer, NULL},
		{rsiTPlaneActor, rsiTypePointer, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos;
	para[1].data = (void*)&norm;
	para[2].data = (void*)&plane->actor;

	va_start(list, plane);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		plane->pos = *pos;
	if(norm)
		plane->norm = *norm;

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates a CSG object
* INPUT:       csgp  pointer pointer which retrieves the pointer to
*                    created CSG object
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateCSG(CTXT, CSG **csgp)
{
	CSG *csg;

	csg = new CSG;
	if(!csg)
		return ERR_MEM;

	*csgp = csg;

	rc->Insert(csg);

	return ERR_NONE;
}

/*************
* DESCRIPTION: set CSG properties
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTCSGOperator      INT      csg operator
*              rsiTCSGObject1       OBJECT   first object
*              rsiTCSGObject2       OBJECT   second object
*              rsiTCSGFlags         INT      flags (inverted)
*              ----------------------------------------------------
* INPUT:       csg   pointer of CSG object to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetCSG(CTXT, CSG *csg, ...)
{
	OBJECT *obj1=NULL, *obj2=NULL;
	va_list list;
	int flags=0;
	int operation = -1;

	static PARAM para[] =
	{
		{rsiTCSGOperator, rsiTypeInt, NULL},
		{rsiTCSGObject1, rsiTypePointer, NULL},
		{rsiTCSGObject2, rsiTypePointer, NULL},
		{rsiTCSGFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&operation;
	para[1].data = (void*)&obj1;
	para[2].data = (void*)&obj2;
	para[3].data = (void*)&flags;

	va_start(list, csg);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(obj1)
	{
		rc->RemoveObject(obj1);
		csg->obj1 = obj1;
		if(obj1->Identify() == IDENT_CSG)
			((CSG*)obj1)->prev = csg;
	}

	if(obj2)
	{
		rc->RemoveObject(obj2);
		csg->obj2 = obj2;
		if(obj2->Identify() == IDENT_CSG)
			((CSG*)obj2)->prev = csg;
	}

	if(flags & rsiFCSGInverted)
		csg->flags |= OBJECT_INVERTED;

	if(operation != -1)
	{
		csg->flags &= ~OBJECT_CSG_MASK;
		switch(operation)
		{
			case rsiFCSGUnion:
				csg->flags |= OBJECT_CSG_UNION;
				break;
			case rsiFCSGIntersection:
				csg->flags |= OBJECT_CSG_INTERSECT;
				break;
			case rsiFCSGDifference:
				csg->flags |= OBJECT_CSG_DIFFERENCE;
				break;
		}
	}

	return ERR_NONE;
}

/*************
* DESCRIPTION: load object in TDDD/3DS/RAW-format
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTObjPos           VECTOR   object position
*              rsiTObjAlign         VECTOR   alignment
*              rsiTObjScale         VECTOR   scale
*              rsiTObjActor         ACTOR    actor of object
*              rsiTObjSurface       SURFACE  surface of object (applied to all triangles)
*              rsiTObjOrientX       VECTOR   x orientation
*              rsiTObjOrientY       VECTOR   y orientation
*              rsiTObjOrientZ       VECTOR   z orientation
*              ----------------------------------------------------
* INPUT:       name     filename of object
*                       created plane
*              srf      surface of plane
* OUTPUT:      rsiResult
*************/
rsiResult rsiLoadObject(CTXT, char *name, ...)
{
	ACTOR *actor=NULL;
	BOOL  error;
	char  buf1[256], buf2[256];
	SURFACE *surface=NULL;
	VECTOR tpos,tscale, ox,oy,oz;
	VECTOR *pos=NULL,*align=NULL,*scale=NULL,*orient_x=NULL,*orient_y=NULL,*orient_z=NULL;
	va_list list;
	char *err;

	static PARAM para[] =
	{
		{rsiTObjPos, rsiTypePointer, NULL},
		{rsiTObjAlign, rsiTypePointer, NULL},
		{rsiTObjScale, rsiTypePointer, NULL},
		{rsiTObjActor, rsiTypePointer, NULL},
		{rsiTObjSurface, rsiTypePointer, NULL},
		{rsiTObjOrientX, rsiTypePointer, NULL},
		{rsiTObjOrientY, rsiTypePointer, NULL},
		{rsiTObjOrientZ, rsiTypePointer, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos;
	para[1].data = (void*)&align;
	para[2].data = (void*)&scale;
	para[3].data = (void*)&actor;
	para[4].data = (void*)&surface;
	para[5].data = (void*)&orient_x;
	para[6].data = (void*)&orient_y;
	para[7].data = (void*)&orient_z;

	va_start(list, name);
	if(GetParams( &list, para ))
		return ERR_PARAM;

	if(!name)
		return ERR_OPENOBJ;

	ExpandPath(rc->world.objectpath, name, buf1);
	if(!buf1[0])
	{
		// cannot find file
		return ERR_OPENOBJ;
	}

	sprintf(buf2, "Loading object: \"%s\"", name);
	rc->Log(rc, buf2);

	if(pos)
		tpos = *pos;
	else
		SetVector(&tpos, 0., 0., 0.);

	if(scale)
		tscale = *scale;
	else
		SetVector(&tscale, 1., 1., 1.);

	if(align)
	{
		orient_x = &ox;
		orient_y = &oy;
		orient_z = &oz;
		CalcOrient(align, orient_x, orient_y, orient_z);
	}

	error = ReadObject(rc, buf1, &tpos, orient_x, orient_y, orient_z, &tscale, actor, surface);

	if(error)
	{
		if(error == ERR_EXTERNAL)
		{
			err = rc->GetObjHandError();
			if(err)
				strcpy(rc->externalerror, rc->GetObjHandError());
			else
				rc->externalerror[0] = 0;
		}
		return error;
	}

	return ERR_NONE;
}

/*************
* DESCRIPTION: creates new surface
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTSurfFlags        int      surface flags
*              ----------------------------------------------------
* INPUT:       srf      pointer pointer which retrieves the pointer to
*                       created surface
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateSurface(CTXT, SURFACE **srf, ...)
{
	va_list list;
	SURFACE *surface;
	ULONG flags;

	static PARAM para[] =
	{
		{rsiTSurfFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	surface = new SURFACE;
	if(!surface)
		return ERR_MEM;

	para[0].data = (void*)&flags;

	va_start(list, srf);
	if(GetParams(&list, para))
		return ERR_PARAM;

	surface->flags = flags;
	rc->Insert(surface);

	*srf = surface;

	return ERR_NONE;
}

/*************
* DESCRIPTION: set surface properties
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTSurfAmbient      COLOR    ambient color
*              rsiTSurfDiffuse      COLOR    diffuse color
*              rsiTSurfFoglen       float    fog length
*              rsiTSurfSpecular     COLOR    specular color
*              rsiTSurfDifftrans    COLOR    diffuse transmission
*              rsiTSurfSpectrans    COLOR    specular transmission
*              rsiTSurfRefexp       float    reflection exponent
*              rsiTSurfTransexp     float    transmission exponent
*              rsiTSurfRefrindex    float    index of refraction
*              rsiTSurfReflect      COLOR    reflection color
*              rsiTSurfTranspar     COLOR    transparence color
*              rsiTSurfTransluc     FLOAT    translucency
*              ----------------------------------------------------
* INPUT:       surf     pointer to surface
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetSurface(CTXT, SURFACE *surf, ...)
{
	COLOR *ambient=NULL,*diffuse=NULL,*specular=NULL,*difftrans=NULL,
			*spectrans=NULL,*reflect=NULL,*transpar=NULL;
	va_list list;

	static PARAM para[] =
	{
		{rsiTSurfAmbient, rsiTypePointer, NULL},
		{rsiTSurfDiffuse, rsiTypePointer, NULL},
		{rsiTSurfFoglen, rsiTypeFloat, NULL},
		{rsiTSurfSpecular, rsiTypePointer, NULL},
		{rsiTSurfDifftrans, rsiTypePointer, NULL},
		{rsiTSurfSpectrans, rsiTypePointer, NULL},
		{rsiTSurfRefexp, rsiTypeFloat, NULL},
		{rsiTSurfTransexp, rsiTypeFloat, NULL},
		{rsiTSurfRefrindex, rsiTypeFloat, NULL},
		{rsiTSurfReflect, rsiTypePointer, NULL},
		{rsiTSurfTranspar, rsiTypePointer, NULL},
		{rsiTSurfTransluc, rsiTypeFloat, NULL},
		{rsiTDone, 0, 0}
	};

	if(!surf)
		return ERR_SURFACE;

	para[ 0].data = (void*)&ambient;
	para[ 1].data = (void*)&diffuse;
	para[ 2].data = (void*)&surf->foglength;
	para[ 3].data = (void*)&specular;
	para[ 4].data = (void*)&difftrans;
	para[ 5].data = (void*)&spectrans;
	para[ 6].data = (void*)&surf->refphong;
	para[ 7].data = (void*)&surf->transphong;
	para[ 8].data = (void*)&surf->refrindex;
	para[ 9].data = (void*)&reflect;
	para[10].data = (void*)&transpar;
	para[11].data = (void*)&surf->translucency;

	va_start(list, surf);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(ambient)
		surf->ambient = *ambient;
	if(diffuse)
		surf->diffuse = *diffuse;
	if(specular)
		surf->specular = *specular;
	if(difftrans)
		surf->difftrans = *difftrans;
	if(spectrans)
		surf->spectrans = *spectrans;
	if(reflect)
		surf->reflect = *reflect;
	if(transpar)
		surf->transpar = *transpar;

	return ERR_NONE;
}

/*************
* DESCRIPTION: add an Imagine texture
* INPUT:       texturep    pointer pointer which receives the pointer to
*                          the created texture
*              name        name of texture
*              surf        surface to apply the texture to
* OUTPUT:      rsiResult
*************/
rsiResult rsiAddImagineTexture(CTXT, IMAGINE_TEXTURE **texturep, char *name, SURFACE *surf)
{
	int err;

	if(!surf)
		return ERR_SURFACE;

	*texturep = new IMAGINE_TEXTURE;
	if(!*texturep)
		return ERR_MEM;

	sprintf(rc->buf, "Loading texture: \"%s\"", name);
	rc->Log(rc, rc->buf);

	err = (*texturep)->Load(rc, name, rc->world.texturepath);
	if(err)
	{
		delete *texturep;
		return err;
	}

	surf->AddTexture(*texturep);

	return ERR_NONE;
}

/*************
* DESCRIPTION: add a hyper texture
* INPUT:       texturep    pointer pointer which receives the pointer to
*                          the created texture
*              name        name of texture
*              surf        surface to apply the texture to
* OUTPUT:      rsiResult
*************/
rsiResult rsiAddHyperTexture(CTXT, HYPER_TEXTURE **texturep, char *name, SURFACE *surf)
{
	int err;

	if(!surf)
		return ERR_SURFACE;

	*texturep = new HYPER_TEXTURE;
	if(!*texturep)
		return ERR_MEM;

	sprintf(rc->buf, "Loading texture: \"%s\"", name);
	rc->Log(rc, rc->buf);

	err = (*texturep)->Load(rc, name, rc->world.texturepath);
	if(err)
	{
		delete *texturep;
		return err;
	}

	surf->AddHyperTexture(*texturep);

	return ERR_NONE;
}


/*************
* DESCRIPTION: add a RayStorm texture
* INPUT:       texturep    pointer pointer which receives the pointer to
*                          the created texture
*              name        name of texture
*              surf        surface to apply the texture to
* OUTPUT:      rsiResult
*************/
rsiResult rsiAddRayStormTexture(CTXT, RAYSTORM_TEXTURE **texturep, char *name, SURFACE *surf)
{
	int err;

	if(!surf)
		return ERR_SURFACE;

	*texturep = new RAYSTORM_TEXTURE;
	if(!*texturep)
		return ERR_MEM;

	sprintf(rc->buf, "Loading texture: \"%s\"", name);
	rc->Log(rc, rc->buf);

	err = (*texturep)->Load(rc, name, rc->world.texturepath);
	if(err)
	{
		delete *texturep;
		return err;
	}

	surf->AddTexture(*texturep);

	return ERR_NONE;
}

/*************
* DESCRIPTION: set texture properties
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTTexturePara1     float    first imagine texture parameter
*                    ...              ..
*              rsiTTexturePara16    float    last imagine texture parameter
*              rsiTTexturePos       VECTOR   position of texture
*              rsiTTextureAlign     VECTOR   alignment of texture
*              rsiTTextureSize      VECTOR   size of texture
*              rsiTTextureActor     ACTOR    actor of texture
*              rsiTTextureOrientX   VECTOR   texture orientation
*              rsiTTextureOrientY   VECTOR   texture orientation
*              rsiTTextureOrientZ   VECTOR   texture orientation
*              rsiTRTextureParams   void*    raystorm texture parameters
*              rsiTHTextureParams   void*    hyper texture parameters
*              ----------------------------------------------------
* INPUT:       texture     pointer to texture
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetTexture(CTXT, TEXTURE *texture, ...)
{
	VECTOR *pos=NULL,*align=NULL,*size=NULL,
		*orient_x=NULL,*orient_y=NULL,*orient_z=NULL;
	int i;
	va_list list;
	void *rparams = NULL, *hparams = NULL;

	static PARAM para[] =
	{
		{rsiTTexturePara1, rsiTypeFloat, NULL},
		{rsiTTexturePara2, rsiTypeFloat, NULL},
		{rsiTTexturePara3, rsiTypeFloat, NULL},
		{rsiTTexturePara4, rsiTypeFloat, NULL},
		{rsiTTexturePara5, rsiTypeFloat, NULL},
		{rsiTTexturePara6, rsiTypeFloat, NULL},
		{rsiTTexturePara7, rsiTypeFloat, NULL},
		{rsiTTexturePara8, rsiTypeFloat, NULL},
		{rsiTTexturePara9, rsiTypeFloat, NULL},
		{rsiTTexturePara10, rsiTypeFloat, NULL},
		{rsiTTexturePara11, rsiTypeFloat, NULL},
		{rsiTTexturePara12, rsiTypeFloat, NULL},
		{rsiTTexturePara13, rsiTypeFloat, NULL},
		{rsiTTexturePara14, rsiTypeFloat, NULL},
		{rsiTTexturePara15, rsiTypeFloat, NULL},
		{rsiTTexturePara16, rsiTypeFloat, NULL},
		{rsiTTexturePos, rsiTypePointer, NULL},
		{rsiTTextureAlign, rsiTypePointer, NULL},
		{rsiTTextureSize, rsiTypePointer, NULL},
		{rsiTTextureActor, rsiTypePointer, NULL},
		{rsiTTextureOrientX, rsiTypePointer, NULL},
		{rsiTTextureOrientY, rsiTypePointer, NULL},
		{rsiTTextureOrientZ, rsiTypePointer, NULL},
		{rsiTRTextureParams, rsiTypePointer, NULL},
		{rsiTHTextureParams, rsiTypePointer, NULL},
		{rsiTDone, 0, 0}
	};

	for(i=0; i<16; i++)
		para[i].data = (void*)&((IMAGINE_TEXTURE*)texture)->param[i];
	para[16].data = (void*)&pos;
	para[17].data = (void*)&align;
	para[18].data = (void*)&size;
	para[19].data = (void*)&texture->actor;
	para[20].data = (void*)&orient_x;
	para[21].data = (void*)&orient_y;
	para[22].data = (void*)&orient_z;
	para[23].data = (void*)&rparams;
	para[24].data = (void*)&hparams;

	va_start(list, texture);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		texture->pos = *pos;
	if(align)
		CalcOrient(align, &texture->orient_x, &texture->orient_y, &texture->orient_z);
	if(size)
		texture->size = *size;
	if(orient_x)
		texture->orient_x = *orient_x;
	if(orient_y)
		texture->orient_y = *orient_y;
	if(orient_z)
		texture->orient_z = *orient_z;
	if(rparams)
	{
		if(((RAYSTORM_TEXTURE*)texture)->data)
			memcpy(((RAYSTORM_TEXTURE*)texture)->data, rparams, ((RAYSTORM_TEXTURE*)texture)->tinfo->datasize);
	}
	if(hparams)
	{
		if(((HYPER_TEXTURE*)texture)->data)
			memcpy(((HYPER_TEXTURE*)texture)->data, hparams, ((HYPER_TEXTURE*)texture)->tinfo->datasize);
	}

	return ERR_NONE;
}

/************
* DESCRIPTION: add a brush
* INPUT:       brushp      pointer pointer which receives the pointer to
*                          the created brush
*              name        name of brush
*              surf        surface to apply the brush to
* OUTPUT:      rsiResult
*************/
rsiResult rsiAddBrush(CTXT, BRUSH **brushp, char *name, SURFACE *surf)
{
	char *err;
	int error;

	if(!surf)
		return ERR_SURFACE;

	*brushp = new BRUSH;
	if(!*brushp)
		return ERR_MEM;

	// look if image already loaded
	(*brushp)->image = FindImage(rc, name);
	if(!(*brushp)->image)
	{
		// didn't found the image => load it
		(*brushp)->image = new IMAGE;
		if(!(*brushp)->image)
		{
			delete *brushp;
			return ERR_MEM;
		}
		rc->Insert((*brushp)->image);
		sprintf(rc->buf, "Loading brush: \"%s\"", name);
		rc->Log(rc, rc->buf);

		error = (*brushp)->image->Load(rc, name, rc->world.brushpath);
		if(error)
		{
			delete *brushp;
			if(error == ERR_EXTERNAL)
			{
				err = rc->GetImageError();
				if(err)
					strcpy(rc->externalerror, err);
				else
					rc->externalerror[0] = 0;
				return ERR_EXTERNAL;
			}
			else
				return error;
		}
	}

	surf->AddBrush(*brushp);

	return ERR_NONE;
}

/************
* DESCRIPTION: set brush properties
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTBrushFlags       int      brush flags (WRAP method, SOFT...)
*              rsiTBrushPos         VECTOR   position of brush
*              rsiTBrushAlign       VECTOR   alignment of brush
*              rsiTBrushSize        VECTOR   size of brush
*              rsiTBrushActor       ACTOR    actor of brush
*              rsiTBRushOrientX     VECTOR   x orientation
*              rsiTBRushOrientY     VECTOR   y orientation
*              rsiTBRushOrientZ     VECTOR   z orientation
*              ----------------------------------------------------
* INPUT:       brush    pointer to brush
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetBrush(CTXT, BRUSH *brush, ...)
{
	VECTOR *pos=NULL,*align=NULL,*size=NULL,
		*orient_x=NULL,*orient_y=NULL,*orient_z=NULL;
	va_list list;

	static PARAM para[] =
	{
		{rsiTBrushFlags, rsiTypeInt, NULL},
		{rsiTBrushPos, rsiTypePointer, NULL},
		{rsiTBrushAlign, rsiTypePointer, NULL},
		{rsiTBrushSize, rsiTypePointer, NULL},
		{rsiTBrushActor, rsiTypePointer, NULL},
		{rsiTBrushOrientX, rsiTypePointer, NULL},
		{rsiTBrushOrientY, rsiTypePointer, NULL},
		{rsiTBrushOrientZ, rsiTypePointer, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&brush->flags;
	para[1].data = (void*)&pos;
	para[2].data = (void*)&align;
	para[3].data = (void*)&size;
	para[4].data = (void*)&brush->actor;
	para[5].data = (void*)&orient_x;
	para[6].data = (void*)&orient_y;
	para[7].data = (void*)&orient_z;

	va_start(list, brush);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos)
		brush->pos = *pos;
	if(size)
		brush->size = *size;
	if(align)
	{
		brush->align = *align;
		CalcOrient(&brush->align, &brush->orient_x, &brush->orient_y, &brush->orient_z);
	}
	if(orient_x)
		brush->orient_x = *orient_x;
	if(orient_y)
		brush->orient_y = *orient_y;
	if(orient_z)
		brush->orient_z = *orient_z;

	return ERR_NONE;
}

/************
* DESCRIPTION: creates a new actor
* INPUT:       act      pointer pointer which receives the pointer to
*                       the created actor
* OUTPUT:      rsiResult
*************/
rsiResult rsiCreateActor(CTXT, ACTOR **act)
{
	ACTOR *actor;
	POS *pos;
	DIMENSION *size;
	ALIGNMENT *align;

	actor = new ACTOR;
	if(!actor)
		return ERR_MEM;

	pos = new POS;
	if(!pos)
	{
		delete actor;
		return ERR_MEM;
	}

	size = new DIMENSION;
	if(!size)
	{
		delete pos;
		delete actor;
		return ERR_MEM;
	}

	align = new ALIGNMENT;
	if(!align)
	{
		delete align;
		delete pos;
		delete actor;
		return ERR_MEM;
	}

	// add default actions
	pos->time.begin = 0.;
	pos->time.end = 0.;
	pos->flags |= ACTION_LINEAR;
	actor->AddPos(pos);

	size->time = pos->time;
	size->flags |= ACTION_LINEAR;
	size->size = actor->axis_size;
	actor->AddSize(size);

	align->time = pos->time;
	align->flags |= ACTION_LINEAR;
	align->align = actor->axis_align;
	actor->AddAlign(align);

	// insert in actor list
	rc->Insert(actor);

	*act = actor;

	return ERR_NONE;
}

/************
* DESCRIPTION: set actor properties
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTActorPos         VECTOR   position of actor
*              rsiTActorAlign       VECTOR   alignment of actor
*              rsiTActorSize        VECTOR   size of actor
*              ----------------------------------------------------
* INPUT:       actor    pointer of actor to be set
* OUTPUT:      rsiResult
*************/
rsiResult rsiSetActor(CTXT, ACTOR *actor, ...)
{
	VECTOR *pos_=NULL,*align_=NULL,*scale_=NULL;
	va_list list;

	static PARAM para[] =
	{
		{rsiTActorPos, rsiTypePointer, NULL},
		{rsiTActorAlign, rsiTypePointer, NULL},
		{rsiTActorSize, rsiTypePointer, NULL},
		{rsiTDone, 0, 0}
	};

	para[0].data = (void*)&pos_;
	para[1].data = (void*)&align_;
	para[2].data = (void*)&scale_;

	va_start(list, actor);
	if(GetParams(&list, para))
		return ERR_PARAM;

	if(pos_)
	{
		actor->axis_pos = *pos_;
		actor->pos->pos = *pos_;
	}
	if(align_)
	{
		actor->axis_align = *align_;
		actor->align->align = *align_;
	}
	if(scale_)
	{
		actor->axis_size = *scale_;
		actor->size->size = *scale_;
	}

	return ERR_NONE;
}

/************
* DESCRIPTION: adds a new position to an actor
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTPosFlags         int      flags of position (linear. spline)
*              ----------------------------------------------------
* INPUT:       actor    pointer of actor which gets a new position
*              tbegin   start time when interpolation to new position starts
*              tend     time when actor reaches this position
*              pos      Position vector
* OUTPUT:      rsiResult
*************/
rsiResult rsiPosition(CTXT, ACTOR *actor, float tbegin, float tend, VECTOR *pos_, ...)
{
	POS *pos;
	va_list list;
	ULONG flags;

	static PARAM para[] =
	{
		{rsiTPosFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	if(!actor)
		return ERR_ACTOR;

	pos = new POS;
	if(!pos)
		return ERR_MEM;

	flags = pos->flags;

	para[0].data = (void*)&flags;

	va_start(list, pos_);
	if(GetParams(&list, para))
	{
		delete pos;
		return ERR_PARAM;
	}

	pos->flags = (UWORD)flags;

	if(tbegin >= 0.)
		pos->time.begin = tbegin;
	if(tend >= 0.)
		pos->time.end = tend;
	if(pos->time.begin > pos->time.end)
	{
		// I'm no time machine, can't go backwards in time
		delete pos;
		return ERR_TIME;
	}
	pos->pos = *pos_;

	actor->AddPos(pos);

	return ERR_NONE;
}

/************
* DESCRIPTION: adds a new alignment to an actor
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTAlignFlags       int      flags (linear. spline)
*              ----------------------------------------------------
* INPUT:       actor    pointer of actor which gets a new alignment
*              tbegin   start time when interpolation to new alignment starts
*              tend     time when actor reaches this alignment
*              pos      alignment vector
* OUTPUT:      rsiResult
*************/
rsiResult rsiAlignment(CTXT, ACTOR *actor, float tbegin, float tend, VECTOR *align_, ...)
{
	ALIGNMENT *align;
	va_list list;
	ULONG flags;

	static PARAM para[] =
	{
		{rsiTAlignFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	if(!actor)
		return ERR_ACTOR;

	align = new ALIGNMENT;
	if(!align)
		return ERR_MEM;

	flags = align->flags;

	para[0].data = (void*)&flags;

	va_start(list, align_);
	if(GetParams(&list, para))
	{
		delete align;
		return ERR_PARAM;
	}

	align->flags = (UWORD)flags;

	if(tbegin >= 0.)
		align->time.begin = tbegin;
	if(tend >= 0.)
		align->time.end = tend;
	if(align->time.begin > align->time.end)
	{
		delete align;
		return ERR_TIME;
	}

	align->align = *align_;

	actor->AddAlign(align);

	return ERR_NONE;
}

/************
* DESCRIPTION: adds a new size to an actor
*              tag names            type     meaning
*              ----------------------------------------------------
*              rsiTSizeFlags        int      flags (linear. spline)
*              ----------------------------------------------------
* INPUT:       actor    pointer of actor which gets a new size
*              tbegin   start time when interpolation to new size starts
*              tend     time when actor reaches this size
*              pos      size vector
* OUTPUT:      rsiResult
*************/
rsiResult rsiSize(CTXT, ACTOR *actor, float tbegin, float tend, VECTOR *size_, ...)
{
	DIMENSION *size;
	va_list list;
	ULONG flags;

	static PARAM para[] =
	{
		{rsiTSizeFlags, rsiTypeInt, NULL},
		{rsiTDone, 0, 0}
	};

	if(!actor)
		return ERR_ACTOR;

	size = new DIMENSION;
	if(!size)
		return ERR_MEM;

	flags = size->flags;

	para[0].data = (void*)&flags;

	va_start(list, size_);
	if(GetParams(&list, para))
	{
		delete size;
		return ERR_PARAM;
	}

	size->flags = (UWORD)flags;

	if(tbegin >= 0.)
		size->time.begin = tbegin;
	if(tend >= 0.)
		size->time.end = tend;
	if(size->time.begin > size->time.end)
	{
		delete size;
		return ERR_TIME;
	}

	size->size = *size_;

	actor->AddSize(size);

	return ERR_NONE;
}

/************
 * DESCRIPTION:   free all objects at the end and declare
 *    a world and a camera again
 * INPUT:         -
 * OUTPUT:        rsiResult
 *************/
rsiResult rsiCleanup(CTXT)
{
	rc->Log(rc, "Cleanup");
	rc->Cleanup();
	return ERR_NONE;
}

#ifdef __STATISTICS__
/************
 * DESCRIPTION:   get statistics on rendering process
 * INPUT:         -
 * OUTPUT:        rsiResult
 *************/
rsiResult rsiGetStats(CTXT, STAT **stats, int *number)
{
	GetStatistics(stats, number);
	return ERR_NONE;
}

/************
 * DESCRIPTION:   get description string for statistic value
 * INPUT:         -
 * OUTPUT:        rsiResult
 *************/
rsiResult rsiGetStatDescription(CTXT, STAT *stat, char *string, int length)
{
	char *s;

	s = GetDescription(stat);
	if(!s)
		return ERR_STATNAME;

	strncpy(string, s, length);

	return ERR_NONE;
}
#endif

} // of extern "C"
