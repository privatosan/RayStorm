/***************
 * MODUL:         object
 * NAME:          object.cpp
 * DESCRIPTION:   This modul contains all the functions for the rootclass
 *                OBJECT
 *                It handles two lists: one for normal objects and one
 *                for unlimited objects.
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    07.03.95 ah    inserted second OBJECT-list for unlimited
 *       objects (makes all more than twice as fast)
 *    03.04.95 ah    changed to new surface structure
 *    10.05.95 ah    added adaptive supersampling
 *    16.08.95 ah    added BRIGHT-flag
 *    21.08.95 ah    added global fog
 *    27.08.95 ah    added UpdateObjects(), GenVoxels()
 *    11.10.95 ah    added triangle blocks
 *    02.12.96 ah    added shadow caching
 *    13.12.96 ah    added minobjects to BuildOctree()
 *    28.02.97 mh    added z-buffer update to Sampleline and SuperSamplePixel
 *    17.05.98 ah    motion blur is now calculated on per line basis not on pixel basis
 *                   if not random jittering (doubles the speed!)
 ***************/

#include <math.h>
#include <stdlib.h>

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef OCTREE_H
#include "octree.h"
#endif

#ifndef WORLD_H
#include "world.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef TRIANGLE_H
#include "triangle.h"
#endif

#ifndef HTEXTURE_H
#include "htexture.h"
#endif

#ifndef LOGO_H
#include "logo.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

#ifdef __AMIGA__
	#include <exec/memory.h>
	#include <pragma/exec_lib.h>
	#include "lib.h"
#else
	#include "fileutil.h"
	typedef BOOL (*KEYDECODE2)(char *);
	extern KEYDECODE2 keyDecodeKeyfile2;
	extern char WorkingDirectory[256];
#endif // __AMIGA__

/*
 * "Dither matrices" used to encode the 'number' of a ray that samples a
 * particular portion of a pixel.
 */
static int TwoSamples[4] = {0, 2,
				 3, 1};
static int ThreeSamples[9] =  {0, 2, 7,
				 6, 5, 1,
				 3, 8, 4};
static int FourSamples[16] =  { 0,  8,  2, 10,
				 12,  4, 14,  6,
				  3, 11,  1,  9,
				 15,  7, 13,  5};
static int FiveSamples[25] =  { 0,  8, 23, 17,  2,
				 19, 12,  4, 20, 15,
				  3, 21, 16,  9,  6,
				 14, 10, 24,  1, 13,
				 22,  7, 18, 11,  5};
static int SixSamples[36] =   { 0, 31,  2, 33, 34,  5,
				  6, 10, 26, 27,  7, 29,
				 23, 13, 15, 14, 22, 18,
				 12, 19, 21, 20, 16, 17,
				 24, 28,  9,  8, 25, 11,
				 35,  4, 32,  3,  1, 30};
static int SevenSamples[49] = { 0, 46, 15, 40,  9, 34,  3,
				  4, 22, 47, 16, 41, 10, 28,
				 29,  5, 23, 48, 17, 35, 11,
				 12, 30,  6, 24, 42, 18, 36,
				 37, 13, 31, 21, 25, 43, 19,
				 20, 38,  7, 32,  1, 26, 44,
				 45, 14, 39,  8, 33,  2, 27};
static int EightSamples[64] = { 0, 57, 58,  4,  3, 61, 62,  7,
				 48, 14, 13, 51, 52, 10,  9, 55,
				 40, 22, 21, 43, 44, 18, 17, 47,
				 31, 33, 34, 28, 27, 37, 38, 24,
				 39, 25, 26, 36, 35, 29, 30, 32,
				 16, 46, 45, 19, 20, 42, 41, 23,
				  8, 54, 53, 11, 12, 50, 49, 15,
				 63,  1,  2, 60, 59,  5,  6, 56};
static int *samples[7] = { TwoSamples, ThreeSamples, FourSamples, FiveSamples,
	SixSamples, SevenSamples, EightSamples};

static void InitShadowCache(RSICONTEXT*);

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
OBJECT::OBJECT()
{
	surf = NULL;
	actor = NULL;
	time = 0.f;
	flags = 0;
}

/*************
 * DESCRIPTION:   Calculate color of object
 * INPUT:         world    pointer to world
 *                ray      pointer to current ray
 *                color    pointer to color
 *                contrib
 *                back     color of background
 *                state    octree test state
 * OUTPUT:        none
 *************/
void OBJECT::GetColor(RSICONTEXT *rc, RAY *ray, COLOR *color, COLOR *contrib,
	 SMALL_COLOR *back, int *state)
{
	VECTOR norm, n, pos;    // surface normal, point of intersection
	SURFACE *surf;          // object surface paramters
	BRUSH *brush;
	TEXTURE *texture;
	float k;             // cosine of angle between normal and ray direction
	float geom_k;        // cosine of angle between geometric normal and ray direction
	VECTOR refl;         // reflected direction

	// Calculate normal.
	VecAddScaled(&ray->start, ray->lambda, &ray->dir, &pos);
	this->Normal(&ray->dir, &pos, &norm, &n);

	surf = &rc->surfaces[ray->depth-1];
#ifdef __STORM__
	// bug introduced with StormC 2.00.24
	rc->surfaces[ray->depth-1] = *this->surf;
#else
	*surf = *this->surf;
#endif
	brush = surf->brush;
	while(brush)
	{
		brush->ApplyBrush(this, &norm, surf, &pos, ray->time);
		brush = (BRUSH*)brush->GetNext();
	}
	texture = surf->texture;
	while(texture)
	{
		texture->Apply(&norm, surf, &pos, ray);
		texture = (TEXTURE*)texture->GetNext();
	}

	// set ambient color
	ColorMultiply(surf->ambient, rc->world.ambient, color);

	// Calculate direction of reflected ray.
	k = -dotp(&ray->dir, &norm);
	VecAddScaled(&ray->dir, 2.f*k, &norm, &refl);
	// because of inaccurate calculations with floats the reflect vector
	// may point inside the triangle (which means that the cosinus of
	// the angle between the geometric normal and the vector is less 0).
	geom_k = dotp(&refl, &n);
	if(geom_k < EPSILON)
	{
		k -= EPSILON - geom_k;
		VecAddScaled(&ray->dir, 2.f*k, &norm, &refl);
	}
	// Perform lighting calculations based on surface normal & other properties,
	// incident ray direction and position, and light source properties.
	// Spawn any necessary reflected and transmitted rays.
	DoLighting(rc, this, surf, &pos, &norm, ray, &refl, color, contrib, k, back, state);

	// Do global fog
	if (rc->world.foglength > EPSILON)
	{
		// test if ray intersects global fog
		if (Abs(ray->dir.y) > EPSILON)
		{
			k = Abs((rc->world.fogheight - ray->start.y) / ray->dir.y);
		}
		else
		{
			if(ray->start.y < rc->world.fogheight)
				k = ray->lambda;
			else
				k = 0.f;
		}
		if (k > EPSILON &&
			 ((k <= ray->lambda && ray->start.y > rc->world.fogheight) ||
			 ray->start.y < rc->world.fogheight))
		{
			if(ray->start.y > rc->world.fogheight)
				k = Abs(ray->lambda-k);
			else
				k = ray->lambda;
			// Add fog to the given color.
			k = LNHALF * k/rc->world.foglength;
			k = k < -10.f ? 0.f : exp(k);
			color->r = k*color->r + (1.-k)*rc->world.fog.r;
			color->g = k*color->g + (1.-k)*rc->world.fog.g;
			color->b = k*color->b + (1.-k)*rc->world.fog.b;
		}
	}
}

/*************
 * DESCRIPTION:   Inserts an object in the object list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void OBJECT::Insert(RSICONTEXT *rc)
{
	rc->Insert(this);
}

/*************
 * DESCRIPTION:   test intersection with octree
 * INPUT:         ray      pointer to ray
 *                state    octree state
 *                mode     mode to find start voxel
 * OUTPUT:        none
 *************/
OBJECT *IntersectRayOctree(RSICONTEXT *rc, RAY *ray,int *state, int mode)
{
	return IntersectionWithOctree(rc, ray, state, mode);
}

/*************
 * DESCRIPTION:   Samples one line
 * INPUT:         y        y screen line position
 *                ray      pointer to ray
 *                array    pointer to color array
 *                colors   color array for motion blur when not random jittering
 *                time     current time
 * OUTPUT:        none
 *************/
#define DYNAMIC_HYPER_TEXTURE

#define DENSITY_MAX_STEP 0.05f
#define DENSITY_MIN_STEP 0.01f

void SampleLine(RSICONTEXT *rc, int y, SMALL_COLOR *array, COLOR *colors, float time)
{
	OBJECT *obj, *newobj;
	COLOR contrib,color,point;
	SMALL_COLOR pixel;
	int x,r;
	RAY ray, newray;
	float value;
	int state, s;     // performed object tests in octree
	int filternum;
	float attenuation;
	float d, density, l;
	VECTOR hitpos;
	ULONG index;
	BOOL randomjitter;
#ifdef DYNAMIC_HYPER_TEXTURE
	float old_density, delta_density, cur_step;
	BOOL done;
#endif // DYNAMIC_HYPER_TEXTURE

	InitShadowCache(rc);

	// do random jitter if it is enabled or we fell back from normal motion blur (colors == NULL) and
	// if we have a time interval
	randomjitter = ((rc->world.flags & WORLD_RANDOM_JITTER) || !colors) && (rc->world.timelength > 0.f);

	ray.time = time;
	ray.sample = 0;
	for(x=rc->camera.left; x<=rc->camera.right; x++)
	{
		color.r = color.g = color.b = 0.f;
		r = 0;
		do
		{
			// calculate ray
			if(randomjitter)
			{
				// jitter in time
				ray.time = CalcTime(r, &rc->world);
			}
			rc->camera.CalcRay(x,y, &ray, &rc->world, ray.time);
			obj = IntersectionWithOctree(rc, &ray, &state, OCTREE_START);
			if(obj)
			{
				// ray hits!

				// depth of field ? write depth of ray (lambda) to info byte
				if(rc->camera.flags & FASTDOF)
				{
					filternum = abs(int(DOF_DEPTHS*(ray.lambda - rc->camera.focaldist)/rc->camera.focaldist));
					if (filternum < 0 || filternum > DOF_DEPTHS)
						filternum = DOF_DEPTHS;

					array[x].info |= filternum;
				}

				// calculate hypertextures
				if(obj->surf->hypertexture)
				{
					newray = ray;
					VecAddScaled(&ray.start, ray.lambda+10.f*EPSILON, &ray.dir, &newray.start);

					done = FALSE;
					attenuation = 0.f;
					while(!done)
					{
						newobj = IntersectionWithOctree(rc, &newray, &s, OCTREE_START);
						if(newobj)
						{
							state = s;

#ifdef DYNAMIC_HYPER_TEXTURE
							old_density = 0.f;
							cur_step = rc->world.step;
							for(d = 0.f; (d <= newray.lambda+EPSILON) && (attenuation < 0.99f); d += cur_step)
							{
								VecAddScaled(&newray.start, d, &ray.dir, &hitpos);
								point = obj->surf->diffuse;
								density = obj->surf->hypertexture->ApplyTextures(rc, &hitpos, obj, &point, ray.time);

								density = 1.f - pow(1.f - density, 2.5f * cur_step);

								if(density > 1.f)
									density = 1.f;
								if(density < 0.f)
									density = 0.f;

								delta_density = fabs(density - old_density);
								if(delta_density > DENSITY_MAX_STEP)
									cur_step *= .5f;
								if(delta_density < DENSITY_MIN_STEP)
									cur_step *= 2.f;

								l = density * (1.f - attenuation);
								ColorMultAdd(&color, l, &point);
								attenuation += l;
							}
#else // DYNAMIC_HYPER_TEXTURE
							for(d = 0.f; (d <= newray.lambda+EPSILON) && (attenuation < 0.99f); d += rc->world.step)
							{
								VecAddScaled(&newray.start, d, &ray.dir, &hitpos);
								point = obj->surf->diffuse;
								density = obj->surf->hypertexture->ApplyTextures(rc, &hitpos, obj, &point, ray.time);

								density = 1.f - pow(1.f - density, 2.5f * rc->world.step);

								if(density > 1.f)
									density = 1.f;
								if(density < 0.f)
									density = 0.f;

								l = density * (1.f - attenuation);
								ColorMultAdd(&color, l, &point);
								attenuation += l;
							}
#endif // DYNAMIC_HYPER_TEXTURE
							l = 1.f - attenuation;

							if(newobj == obj)
							{
								color.r += l * rc->world.back.r;
								color.g += l * rc->world.back.g;
								color.b += l * rc->world.back.b;
								// No other object behind, done
								// TODO: if the hyper texture object is not completely opaque we have to trace further
								done = TRUE;
							}
							else
							{
								if(newobj->surf->hypertexture)
								{
									// Another hyper texture object behind this, apply hypertexture to this following object
									VecAddScaled(&newray.start, newray.lambda+10.f*EPSILON, &ray.dir, &newray.start);
									obj = newobj;
								}
								else
								{
									ray = newray;
									contrib.r = contrib.g = contrib.b = 1.f;
									ray.depth = 1;
									newobj->GetColor(rc, &ray, &point, &contrib, &array[x], &state);
									color.r += l * point.r;
									color.g += l * point.g;
									color.b += l * point.b;
								}
							}
						}
						else
						{
							// no intersection after
							done = TRUE;
						}
					}
				}
				else
				{
					contrib.r = contrib.g = contrib.b = 1.f;
					ray.depth = 1;
					obj->GetColor(rc, &ray, &point, &contrib, &array[x], &state);
					color.r += point.r;
					color.g += point.g;
					color.b += point.b;
				}
			}
			else
			{
				// no hit: use background color
				array[x].info |= DOF_DEPTHS;
				if(!rc->world.backdrop)
				{
					pixel.r = UBYTE(rc->world.back.r * 255.f);
					pixel.g = UBYTE(rc->world.back.g * 255.f);
					pixel.b = UBYTE(rc->world.back.b * 255.f);
					// Do global fog
					if(rc->world.DoGlobalFog(&ray,&pixel,&point))
					{
						color.r += point.r;
						color.g += point.g;
						color.b += point.b;
					}
					else
					{
						color.r += rc->world.back.r;
						color.g += rc->world.back.g;
						color.b += rc->world.back.b;
					}
				}
				else
				{
					// Do global fog
					if(rc->world.DoGlobalFog(&ray,&array[x],&point))
					{
						color.r += point.r;
						color.g += point.g;
						color.b += point.b;
					}
					else
					{
						color.r += float(array[x].r)/255.f;
						color.g += float(array[x].g)/255.f;
						color.b += float(array[x].b)/255.f;
					}
				}
			}
			r++;
		}
		while(randomjitter &&
				r<rc->world.motionblur_samples &&
				(state == OCTREE_TESTMB));
		// loop while
		// - random jitter is enabled
		// - not all samples are taken
		// - motion blured object test is performed

		if((rc->world.flags & WORLD_RANDOM_JITTER) || !colors)
		{
			// normalize color if distributive sampled
			value = 255.f/r;
			color.r *= value;
			color.g *= value;
			color.b *= value;
			// Normalize the color
			if(color.r > 255.f)
			{
				value = 255.f/color.r;
				color.r = 255.f;
				color.g *= value;
				color.b *= value;
			}
			if(color.g > 255.f)
			{
				value = 255.f/color.g;
				color.r *= value;
				color.g = 255.f;
				color.b *= value;
			}
			if(color.b > 255.f)
			{
				value = 255.f/color.b;
				color.r *= value;
				color.g *= value;
				color.b = 255.f;
			}

			array[x].r = (UBYTE)(color.r);
			array[x].g = (UBYTE)(color.g);
			array[x].b = (UBYTE)(color.b);
		}
		else
		{
			colors[x].r += color.r;
			colors[x].g += color.g;
			colors[x].b += color.b;
		}
		array[x].info &= ~INFO_SAMPLED;

		// insert logo in unregistered version
		if(!rc->world.validkeyfile)
		{
			if((x > ((rc->camera.xres-LOGO_X)>>1)) && (x < ((rc->camera.xres+LOGO_X)>>1)) &&
				(y > ((rc->camera.yres-LOGO_Y)>>1)) && (y < ((rc->camera.yres+LOGO_Y)>>1)))
			{
				index = logo_data[x-((rc->camera.xres-LOGO_X)>>1) + (LOGO_Y-1-y+((rc->camera.yres-LOGO_Y)>>1))*LOGO_X];
				if(index)
				{
					index *= 3;
					if((rc->world.flags & WORLD_RANDOM_JITTER) || !colors)
					{
						array[x].r = logo_palette[index];
						array[x].g = logo_palette[index+1];
						array[x].b = logo_palette[index+2];
					}
					else
					{
						colors[x].r = (float)(logo_palette[index]) / 255.f;
						colors[x].g = (float)(logo_palette[index+1]) / 255.f;
						colors[x].b = (float)(logo_palette[index+2]) / 255.f;
					}
				}
			}
		}
	}
}

/*************
 * DESCRIPTION:   Creates filter for super sampling
 * INPUT:         filter   filter data
 *                world    pointer to world
 * OUTPUT:        none
 *************/
void CreateFilter(float *filter, WORLD *world)
{
	float upos,vpos,norm;
	int x,y;

	// Init filter
	world->sampledelta = world->filterwidth/world->samples;
	norm = 0.f;
	upos = -0.5f*world->filterwidth + 0.5f*world->sampledelta;
	for (x = 0; x < world->samples; x++)
	{
		vpos = -0.5f*world->filterwidth + 0.5f*world->sampledelta;
		for (y = 0; y < world->samples; y++)
		{
			filter[x+y*world->samples] = exp(-0.5f*(upos*upos+vpos*vpos));
			norm += filter[x+y*world->samples];
			vpos += world->sampledelta;
		}
		upos += world->sampledelta;
	}
	for (x = 0; x < world->samples; x++)
		for (y = 0; y < world->samples; y++)
			filter[x+y*world->samples] /= norm;
}

/*************
 * DESCRIPTION:   Supersamples one pixel
 * INPUT:         x,y      screen pixel position
 *                camera   pointer to camera
 *                pixel    pointer to color pixel
 *                world    pointer to world
 *                filter   pointer to filtermap
 *                mbcolor  color for motion blur when not random jittering
 *                time     current time
 * OUTPUT:        none
 *************/
void SuperSamplePixel(RSICONTEXT *rc, int x, int y, SMALL_COLOR *pixel, const float *filter, COLOR *mbcolor, float time)
{
	OBJECT *obj, *newobj;
	COLOR contrib, point, color, color1;
	SMALL_COLOR pixel1;
	int u,v,s=0,r;
	float upos,vpos;
	float value, l,attenuation,d,density;
	VECTOR hitpos;
	int filterpos = 0;
	RAY ray, newray;
	int *sample;
	int state, filternum;      // performed object tests in octree
	ULONG index;
	BOOL randomjitter;

	// do random jitter if it is enabled or we fell back from normal motion blur (colors == NULL) and
	// if we have a time interval
	randomjitter = ((rc->world.flags & WORLD_RANDOM_JITTER) || !mbcolor) && (rc->world.timelength > 0.f);

	STATISTIC(STAT_SUPERSAMPLE);

	sample = samples[rc->world.samples-2];
	ray.time = time;
	// supersampling
	color.r = color.g = color.b = 0.f;
	vpos = y - 0.5*rc->world.filterwidth;
	for (v=0; v<rc->world.samples; v++, vpos += rc->world.sampledelta)
	{
		upos = x - 0.5f*rc->world.filterwidth;
		for (u=0; u<rc->world.samples; u++, upos += rc->world.sampledelta)
		{
			color1.r = color1.g = color1.b = 0.f;
			ray.sample = sample[s++];
			r = 0;
			do
			{
				if(randomjitter)
				{
					// jitter in time
					ray.time = CalcTime(r, &rc->world);
				}
				rc->camera.CalcRay(upos,vpos, &ray, &rc->world, ray.time);
				obj = IntersectionWithOctree(rc, &ray, &state,OCTREE_LINE);
				if(obj)
				{
					// depth of field ? write depth of ray (lambda) to info byte
					if(rc->camera.flags & FASTDOF)
					{
						filternum = abs(int(DOF_DEPTHS*(ray.lambda - rc->camera.focaldist)/rc->camera.focaldist));
						if(filternum < 0 || filternum > DOF_DEPTHS)
							filternum = DOF_DEPTHS;

						pixel->info |= filternum;
					}
					// calculate hypertextures
					if(obj->surf->hypertexture)
					{
						attenuation = 0.f;
						newray = ray;
						VecAddScaled(&ray.start, ray.lambda+10.f*EPSILON, &ray.dir, &newray.start);

						newobj = IntersectionWithOctree(rc, &newray, &s, OCTREE_START);
						if(newobj)
						{
							state = s;

							for(d = 0.f; (d <= newray.lambda+EPSILON) && (attenuation < 0.99f); d += rc->world.step)
							{
								VecAddScaled(&newray.start, d, &ray.dir, &hitpos);
								point = obj->surf->diffuse;
								density = obj->surf->hypertexture->ApplyTextures(rc, &hitpos, obj, &point, ray.time);

								density = 1.f - pow(1.f - density, 2.5f * rc->world.step);

								if(density > 1.f)
									density = 1.f;
								if(density < 0.f)
									density = 0.f;

								l = density * (1.f - attenuation);
								ColorMultAdd(&color, l, &point);
								attenuation += l;
							}
							l = (1.f - attenuation) * filter[filterpos];
							if(newobj == obj)
							{
								color1.r += l * rc->world.back.r;
								color1.g += l * rc->world.back.g;
								color1.b += l * rc->world.back.b;
							}
							else
							{
								ray = newray;
								contrib.r = contrib.g = contrib.b = 1.f;
								ray.depth = 1;
								newobj->GetColor(rc, &ray, &point, &contrib, pixel, &state);
								color1.r += l * point.r;
								color1.g += l * point.g;
								color1.b += l * point.b;
							}
						}
					}
					else
					{
						contrib.r = contrib.g = contrib.b = 1.f;
						ray.depth = 1;
						obj->GetColor(rc, &ray, &point, &contrib, pixel, &state);
						value = filter[filterpos];
						color1.r += point.r*value;
						color1.g += point.g*value;
						color1.b += point.b*value;
					}
				}
				else
				{
					// no hit: use background color
					pixel->info |= DOF_DEPTHS;
					if(!rc->world.backdrop)
					{
						value = filter[filterpos];
						pixel1.r = UBYTE(rc->world.back.r * 255.f);
						pixel1.g = UBYTE(rc->world.back.g * 255.f);
						pixel1.b = UBYTE(rc->world.back.b * 255.f);
						// Do global fog
						if(rc->world.DoGlobalFog(&ray,&pixel1,&point))
						{
							color1.r += point.r*value;
							color1.g += point.g*value;
							color1.b += point.b*value;
						}
						else
						{
							color1.r += rc->world.back.r*value;
							color1.g += rc->world.back.g*value;
							color1.b += rc->world.back.b*value;
						}
					}
					else
					{
						value = filter[filterpos];
						// Do global fog
						if(rc->world.DoGlobalFog(&ray,pixel,&point))
						{
							color1.r += point.r*value;
							color1.g += point.g*value;
							color1.b += point.b*value;
						}
						else
						{
							value /= 255.f;
							color1.r += float(pixel->r)*value;
							color1.g += float(pixel->g)*value;
							color1.b += float(pixel->b)*value;
						}
					}
				}
				r++;
			}
			while(randomjitter &&
					r<rc->world.motionblur_samples &&
					(state == OCTREE_TESTMB));
			// loop while
			// - a time intervall is given
			// - not all samples are taken
			// - motion blured object test is performed

			// normalize color if distributive sampled
			value = 1.f/r;
			color.r += color1.r*value;
			color.g += color1.g*value;
			color.b += color1.b*value;
			filterpos++;
		}
	}
	// Normalize the color
	if((rc->world.flags & WORLD_RANDOM_JITTER) || !mbcolor)
	{
		if(color.r > 1.f)
		{
			value = 1.f/color.r;
			color.r = 1.f;
			color.g *= value;
			color.b *= value;
		}
		if(color.g > 1.f)
		{
			value = 1.f/color.g;
			color.r *= value;
			color.g = 1.f;
			color.b *= value;
		}
		if(color.b > 1.f)
		{
			value = 1.f/color.b;
			color.r *= value;
			color.g *= value;
			color.b = 1.f;
		}
		pixel->r = (UBYTE)(color.r*255.f);
		pixel->g = (UBYTE)(color.g*255.f);
		pixel->b = (UBYTE)(color.b*255.f);
		pixel->info |= INFO_SAMPLED;
	}
	else
	{
		mbcolor->r += color.r;
		mbcolor->g += color.g;
		mbcolor->b += color.b;
		pixel->info |= INFO_SAMPLED_MB;
	}

	// insert logo in unregistered version
	if(!rc->world.validkeyfile)
	{
		if((x > ((rc->camera.xres-LOGO_X)>>1)) && (x < ((rc->camera.xres+LOGO_X)>>1)) &&
			(y > ((rc->camera.yres-LOGO_Y)>>1)) && (y < ((rc->camera.yres+LOGO_Y)>>1)))
		{
			index = logo_data[x-((rc->camera.xres-LOGO_X)>>1) + (LOGO_Y-1-y+((rc->camera.yres-LOGO_Y)>>1))*LOGO_X];
			if(index)
			{
				index *= 3;
				if((rc->world.flags & WORLD_RANDOM_JITTER) || !mbcolor)
				{
					pixel->r = logo_palette[index];
					pixel->g = logo_palette[index+1];
					pixel->b = logo_palette[index+2];
				}
				else
				{
					mbcolor->r += (float)(logo_palette[index]) / 255.f;
					mbcolor->g += (float)(logo_palette[index+1]) / 255.f;
					mbcolor->b += (float)(logo_palette[index+2]) / 255.f;
				}
			}
		}
	}
}

/*************
 * DESCRIPTION:   Supersamples one complete line
 * INPUT:         y        screen pixel position
 *                array    pointer to color array
 *                filter   pointer to filtermap
 *                colors   color array for motion blur when not random jittering
 * OUTPUT:        none
 *************/
void SuperSampleLine(RSICONTEXT *rc, int y, SMALL_COLOR *array, const float *filter, COLOR *colors)
{
	int x;
	BOOL randomjitter;
	int r;
	COLOR *color;
	SMALL_COLOR *col;

	// do random jitter if it is enabled or we fell back from normal motion blur (colors == NULL) and
	// if we have a time interval
	randomjitter = (rc->world.flags & WORLD_RANDOM_JITTER) || !colors;

	InitShadowCache(rc);

	if(randomjitter)
	{
		for(x=rc->camera.left; x<=rc->camera.right; x++)
		{
			SuperSamplePixel(rc, x,y, &array[x], filter, NULL, rc->world.time);
		}
	}
	else
	{
		color = &colors[rc->camera.left];
		for(x = rc->camera.left; x <= rc->camera.right; x++)
		{
			color->r = 0.f;
			color->g = 0.f;
			color->b = 0.f;
			color++;
		}
		for(r=0; r<rc->world.motionblur_samples; r++)
		{
			for(x=rc->camera.left; x<=rc->camera.right; x++)
			{
				SuperSamplePixel(rc, x,y, &array[x], filter, &colors[x], CalcTime(r, &rc->world));
			}
		}
		color = &colors[rc->camera.left];
		col = &array[rc->camera.left];
		for(x=rc->camera.left; x<=rc->camera.right; x++)
		{
			color->r *= 2.f;
			color->g *= 2.f;
			color->b *= 2.f;
			color++;
			col->info |= INFO_SAMPLED;
			col++;
		}
	}
}

/*************
 * DESCRIPTION:   Calculate contrast of pixel
 * INPUT:         contrast maximum contrast
 *                x        screen position
 *                pix0     last line
 *                pix1     current line
 *                pix2     next line
 * OUTPUT:        none
 *************/
static BOOL ExcessiveContrast(COLOR *contrast, int x, SMALL_COLOR *pix0, SMALL_COLOR *pix1,
	SMALL_COLOR *pix2)
{
	int min, max, sum, diff;
	SMALL_COLOR *p0, *p10, *p11, *p12, *p2;

	p0 = &pix0[x];
	p10 = &pix1[x-1];
	p11 = p10+1;
	p12 = p10+2;
	p2 = &pix2[x];

	max = MAX(p0->r, p10->r);
	if (p11->r > max)
		max = p11->r;
	if (p12->r > max)
		max = p12->r;
	if (p2->r > max)
		max = p2->r;

	min = MIN(p0->r, p10->r);
	if (p11->r < min)
		min = p11->r;
	if (p12->r < min)
		min = p12->r;
	if (p2->r < min)
		min = p2->r;

	diff = max - min;
	sum = max + min;
	if (sum > EPSILON)
		if (float(diff)/float(sum) > contrast->r)
			return TRUE;

	max = MAX(p0->g, p10->g);
	if (p11->g > max)
		max = p11->g;
	if (p12->g > max)
		max = p12->g;
	if (p2->g > max)
		max = p2->g;

	min = MIN(p0->g, p10->g);
	if (p11->g < min)
		min = p11->g;
	if (p12->g < min)
		min = p12->g;
	if (p2->g < min)
		min = p2->g;

	diff = max - min;
	sum = max + min;
	if (sum > EPSILON)
		if (float(diff)/float(sum) > contrast->g)
			return TRUE;

	max = MAX(p0->b, p10->b);
	if (p11->b > max)
		max = p11->b;
	if (p12->b > max)
		max = p12->b;
	if (p2->b > max)
		max = p2->b;

	min = MIN(p0->b, p10->b);
	if (p11->b < min)
		min = p11->b;
	if (p12->b < min)
		min = p12->b;
	if (p2->b < min)
		min = p2->b;

	diff = max - min;
	sum = max + min;
	if (sum > EPSILON)
		if (float(diff)/float(sum) > contrast->b)
			return TRUE;

	return FALSE;
}

/*************
 * DESCRIPTION:   Supersamples one pixel
 * INPUT:         y        screen line position
 *                array0   last line
 *                array1   current line
 *                array2   next line
 *                world    pointer to world structure
 *                camera   pointer to camera structure
 *                filter   filter data
 *                colors0  color array for motion blur when not random jittering (last line)
 *                colors1  color array for motion blur when not random jittering (current line)
 *                colors2  color array for motion blur when not random jittering (next line)
 * OUTPUT:        none
 *************/
void AdaptiveRefineLine(RSICONTEXT *rc, int y, SMALL_COLOR *array0, SMALL_COLOR *array1,
	SMALL_COLOR *array2, const float *filter, COLOR *colors0, COLOR *colors1, COLOR *colors2)
{
	int x;
	BOOL randomjitter;
	int r;
	COLOR color;
	SMALL_COLOR *a0, *a1, *a2;
	float inv, value, time;

	// do random jitter if it is enabled or we fell back from normal motion blur (colors == NULL) and
	// if we have a time interval
	randomjitter = (rc->world.flags & WORLD_RANDOM_JITTER) || !colors0 || !colors1 || !colors2;

	// Walk down scan1, looking at 4-neighbors for excessive contrast.
	// If found, supersample *all* neighbors not already supersampled.
	// The process is repeated until either there are no
	// high-contrast regions or all such regions are already supersampled.
	if(randomjitter)
	{
		for (x=rc->camera.left+1; x<rc->camera.right; x++)
		{
			// Find min and max RGB for area we care about
			if (ExcessiveContrast(&rc->world.contrast, x, array0,array1,array2) || (rc->world.flags & WORLD_DOF))
			{
				if(!(array0[x].info & INFO_SAMPLED))
					SuperSamplePixel(rc, x,   y+1, &array0[x],   filter, NULL, rc->world.time);
				if(!(array1[x-1].info & INFO_SAMPLED))
					SuperSamplePixel(rc, x-1, y,   &array1[x-1], filter, NULL, rc->world.time);
				if(!(array1[x].info & INFO_SAMPLED))
					SuperSamplePixel(rc, x,   y,   &array1[x],   filter, NULL, rc->world.time);
				if(!(array1[x+1].info & INFO_SAMPLED))
					SuperSamplePixel(rc, x+1, y,   &array1[x+1], filter, NULL, rc->world.time);
				if(!(array2[x].info & INFO_SAMPLED))
					SuperSamplePixel(rc, x,   y-1, &array2[x],   filter, NULL, rc->world.time);
			}
		}
	}
	else
	{
		for(r=0; r<rc->world.motionblur_samples; r++)
		{
			for(x=rc->camera.left+1; x<rc->camera.right; x++)
			{
				// Find min and max RGB for area we care about
				if (ExcessiveContrast(&rc->world.contrast, x, array0,array1,array2) || (rc->world.flags & WORLD_DOF))
				{
					time = CalcTime(r, &rc->world);
					if(!(array0[x].info & INFO_SAMPLED))
						SuperSamplePixel(rc, x,   y+1, &array0[x],   filter, &colors0[x], time);
					if(!(array1[x].info & INFO_SAMPLED))
						SuperSamplePixel(rc, x-1, y,   &array1[x-1], filter, &colors1[x-1], time);
					if(!(array1[x].info & INFO_SAMPLED))
						SuperSamplePixel(rc, x,   y,   &array1[x],   filter, &colors1[x], time);
					if(!(array1[x].info & INFO_SAMPLED))
						SuperSamplePixel(rc, x+1, y,   &array1[x+1], filter, &colors1[x+1], time);
					if(!(array2[x].info & INFO_SAMPLED))
						SuperSamplePixel(rc, x,   y-1, &array2[x],   filter, &colors2[x], time);
				}
			}
		}
		a0 = &array0[rc->camera.left+1];
		a1 = &array1[rc->camera.left+1];
		a2 = &array2[rc->camera.left+1];
		value = 127.5f / rc->world.motionblur_samples;
		for(x=rc->camera.left+1; x<rc->camera.right; x++)
		{
			if(a0->info & INFO_SAMPLED_MB)
			{
				color = colors0[x];
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

				a0->r = (UBYTE)(color.r);
				a0->g = (UBYTE)(color.g);
				a0->b = (UBYTE)(color.b);
				a0->info &= ~INFO_SAMPLED_MB;
				a0->info |= INFO_SAMPLED;
			}
			if(a1->info & INFO_SAMPLED_MB)
			{
				color = colors1[x];
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

				a1->r = (UBYTE)(color.r);
				a1->g = (UBYTE)(color.g);
				a1->b = (UBYTE)(color.b);
				a1->info &= ~INFO_SAMPLED_MB;
				a1->info |= INFO_SAMPLED;
			}
			if(a2->info & INFO_SAMPLED_MB)
			{
				color = colors2[x];
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

				a2->r = (UBYTE)(color.r);
				a2->g = (UBYTE)(color.g);
				a2->b = (UBYTE)(color.b);
				a2->info &= ~INFO_SAMPLED_MB;
				a2->info |= INFO_SAMPLED;
			}
			a0++;
			a1++;
			a2++;
		}
	}
}

/*************
 * DESCRIPTION:   update all objects
 * INPUT:         time     current time
 * OUTPUT:        none
 *************/
void UpdateObjects(RSICONTEXT *rc, const float time)
{
	OBJECT *obj;

	obj = rc->obj_root;
	while(obj)
	{
		obj->Update(time);
		obj->time = time;
		obj = (OBJECT*)obj->GetNext();
	}
	obj = rc->obj_unlimited_root;
	while(obj)
	{
		obj->Update(time);
		obj->time = time;
		obj = (OBJECT*)obj->GetNext();
	}
}

/*************
 * DESCRIPTION:   generate object voxels
 * INPUT:         none
 * OUTPUT:        none
 *************/
void GenVoxels(RSICONTEXT *rc)
{
	OBJECT *obj;

#ifdef __AMIGA__
	if(KeyfileBase)
		rc->world.validkeyfile = !keyDecodeKeyfile2();
#else
	char buf[256];
	if(keyDecodeKeyfile2)
	{
		strcpy(buf, WorkingDirectory);
		AddPart(buf, "keyfile.dll", 256);
		rc->world.validkeyfile = !keyDecodeKeyfile2(buf);
	}
#endif

	obj = rc->obj_root;
	while(obj)
	{
		obj->GenVoxel();

		// only do motion blur if timelength bigger than 0
		if(obj->actor && rc->world.timelength>0 && rc->world.motionblur_samples>1)
			obj->actor->CalcVoxel(&obj->voxel, &rc->world);

		obj = (OBJECT*)obj->GetNext();
	}
}

/*************
 * DESCRIPTION:   initialize the shadow cache
 * INPUT:         -
 * OUTPUT:        -
 *************/
static void InitShadowCache(RSICONTEXT *rc)
{
	int i;

	for(i=0; i<MAXRAYDEPTH+1; i++)
		rc->shadowcache[i] = NULL;
}

/*************
 * DESCRIPTION:   Disable object so it is not tested by next intersection test
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::DisableObject(RSICONTEXT *rc)
{
	identification = rc->identification + 1;
}

