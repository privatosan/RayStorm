/***************
 * MODUL:         light
 * NAME:          light.cpp
 * DESCRIPTION:   This file includes all functions needed for lighting
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    13.03.95 ah    all different lighsources are now in this
 *       module; Consturctor initializes with defaults
 *    26.04.95 ah    added spot light
 *    15.08.95 ah    added soft shadows
 *    16.08.95 ah    added fog
 *    10.09.95 ah    added Update() and UpdateLights()
 *    27.09.95 ah    bugfix in DoLighting(): transparence
 *    28.09.95 ah    added global reflection map
 *    11.10.95 ah    changed spotlight 'dir' to 'lookp';
 *       added animated look point for spotlight
 *    07.02.96 ah    applied brushes and textures to surfaces which lie
 *       between the light and the illuminated surface
 *    11.02.96 ah    global reflection map is now applied in quick mode too
 *    22.10.96 mh    added ApplyFlares
 *    28.11.96 ah    added visibility test for lens flares
 *    02.12.96 ah    added shadow cache
 *    27.02.97 ah    added shadowmap
 *    24.05.97 ah    little bug in translucency calculation
 ***************/

#include <time.h>
#include <math.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

#ifndef WORLD_H
#include "world.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

#ifndef OCTREE_H
#include "octree.h"
#endif

#ifndef ACTOR_H
#include "actor.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
LIGHT::LIGHT()
{
	// Initialize with defaults
	pos.x = 0.f;
	pos.y = 0.f;
	pos.z = 0.f;
	color.r = 1.f;
	color.g = 1.f;
	color.b = 1.f;
	r = 0.f;
	falloff = 0.f;
	flags = LIGHT_STAR|LIGHT_FLARES;
	time = 0.f;
	actor = NULL;
	flares = NULL;
	star = NULL;
	shadowmap = NULL;
	shadowmap_size = 128;
}

/*************
 * INPUT:         none
 * OUTPUT:        none
 *************/
LIGHT::~LIGHT()
{
	if(shadowmap)
		delete [ ] shadowmap;
	if(flares)
		FreeFlares(flares);
	if(star)
		delete star;
}

/*************
 * DESCRIPTION:   return the color of the global reflection map at the ray
 *    direction
 * INPUT:         ray      current ray
 *                color    returned color
 *                world    world
 * OUTPUT:        none
 *************/
static void ApplyReflMap(RAY *ray,COLOR *color,WORLD *world)
{
	float phi, theta, u, v;
	int x, y;
	SMALL_COLOR pixel;
	COLOR pixelcolor;

	if (ray->dir.y > 1.f) // roundoff
		phi = PI;
	else
	{
		if (ray->dir.y < -1.f) // roundoff
			phi = 0.f;
		else
			phi = (float)acos(ray->dir.y);
	}

	v = phi * INV_PI;

	if (fabs(v) < EPSILON || equal(v, 1.f))
	{
		u = 0.f;
	}
	else
	{
		theta = ray->dir.x / (float)sin(phi);
		if (theta > 1.f)
			theta = 0.f;
		else
		{
			if (theta < -1.f)
				theta = 0.5f;
			else
				theta = (float)acos(theta) * INV_TWOPI;
		}

		if (ray->dir.z > 0.f)
			u = 1.f - theta;
		else
			u = theta;
	}

	u *= world->reflmap->width;
	v *= world->reflmap->height;
	x = (int)floor(u);
	y = (int)floor(v);
	u -= float(x);
	v -= float(y);
	world->reflmap->ColorInterpolate(x,y,u,v,&pixelcolor);
	pixel.r = (UBYTE)(pixelcolor.r*255.f);
	pixel.g = (UBYTE)(pixelcolor.g*255.f);
	pixel.b = (UBYTE)(pixelcolor.b*255.f);

	// Do global fog
	if(!world->DoGlobalFog(ray,&pixel,color))
		*color = pixelcolor;
}

#define RAND() (((float)rand())/RAND_MAX)

/*************
 * DESCRIPTION:
 * INPUT:         fx,fy    floating point position
 *                dist     distance from hitpoint to lightsource
 * OUTPUT:        0 -> full shadow ... 1 -> no shadow
 *************/
float LIGHT::InterpolateShadowMap(float fx, float fy, float dist)
{
#if 0
	int xplus, yplus, xminus, yminus, ix, iy;
	float xnyn,xnyz,xnyp, xzyn,xzyz,xzyp, xpyn,xpyz,xpyp;

	fx *= shadowmap_size;
	fy *= shadowmap_size>>1;

	ix = (int)floor(fx);
	iy = (int)floor(fy);

	fx -= (float)ix;
	fy -= (float)iy;

	// bi-linear interp of four pixels
	if (ix == shadowmap_size - 1)
		xplus = 1 - shadowmap_size;
	else
		xplus = 1;

	if (ix == 0)
		xminus = shadowmap_size - 1;
	else
		xminus = -1;

	if (iy == (shadowmap_size>>1) - 1)
		yplus = (1 - (shadowmap_size>>1)) * shadowmap_size;
	else
		yplus = shadowmap_size;

	if (iy == 0)
		yminus = ((shadowmap_size>>1) - 1) * shadowmap_size;
	else
		yminus = -shadowmap_size;

	iy *= shadowmap_size;

	xnyn = shadowmap[iy+yminus + ix+xminus];
	xnyz = shadowmap[iy + ix+xminus];
	xnyp = shadowmap[iy+yplus + ix+xminus];

	xzyn = shadowmap[iy+yminus + ix];
	xzyz = shadowmap[iy + ix];
	xzyp = shadowmap[iy+yplus + ix];

	xpyn = shadowmap[iy+yminus + ix+xplus];
	xpyz = shadowmap[iy + ix+xplus];
	xpyp = shadowmap[iy+yplus + ix+xplus];

	if((xnyn > dist) &&
		(xnyz > dist) &&
		(xnyp > dist) &&
		(xzyn > dist) &&
		(xzyz > dist) &&
		(xzyp > dist) &&
		(xpyn > dist) &&
		(xpyz > dist) &&
		(xpyp > dist))
	{
		return 1.f;
	}
	if(xnyn > dist)
		xnyn = (1.f-fx)*(1.f-fy);
	else
		xnyn = 0.f;
	if(xnyz > dist)
		xnyz = (1.f-fx)*fy;
	else
		xnyz = 0.f;
	if(xnyp > dist)
		xnyp = (1.f-fx)*fy;
	else
		xnyp = 0.f;

	if(xzyn > dist)
		xzyn = fx*(1.f-fy);
	else
		xzyn = 0.f;
	if(xzyz > dist)
		xzyz = fx*fy;
	else
		xzyz = 0.f;
	if(xzyp > dist)
		xzyp = fx*fy;
	else
		xzyp = 0.f;

	if(xpyn > dist)
		xpyn = (1.f-fx)*(1.f-fy);
	else
		xpyn = 0.f;
	if(xpyz > dist)
		xpyz = (1.f-fx)*fy;
	else
		xpyz = 0.f;
	if(xpyp > dist)
		xpyp = (1.f-fx)*fy;
	else
		xpyp = 0.f;

	return (xnyn+xnyz+xnyp + xzyn+xzyz+xzyp + xpyn+xpyz+xpyp)*0.44444444f;
#endif
#if 1
	int xminus, yminus, ix, iy;
	float x0y0, x1y0, x0y1, x1y1;

	fx *= shadowmap_size;
	fy *= shadowmap_size>>1;

	ix = (int)floor(fx);
	iy = (int)floor(fy);

	fx -= (float)ix;
	fy -= (float)iy;

	// bi-linear interp of four pixels
	if (ix == 0)
		xminus = shadowmap_size - 1;
	else
		xminus = -1;

	if (iy == 0)
		yminus = ((shadowmap_size>>1) - 1) * shadowmap_size;
	else
		yminus = -shadowmap_size;

	iy *= shadowmap_size;

	x0y0 = shadowmap[iy + ix];
	x1y0 = shadowmap[iy + ix+xminus];
	x0y1 = shadowmap[iy+yminus + ix];
	x1y1 = shadowmap[iy+yminus + ix+xminus];

	if((x0y0 > dist) &&
		(x1y0 > dist) &&
		(x0y1 > dist) &&
		(x1y1 > dist))
	{
		return 1.f;
	}
	if(x0y0 > dist)
		x0y0 = fx*fy;
	else
		x0y0 = 0.f;
	if(x1y0 > dist)
		x1y0 = (1.f-fx)*fy;
	else
		x1y0 = 0.f;
	if(x0y1 > dist)
		x0y1 = fx*(1.f-fy);
	else
		x0y1 = 0.f;
	if(x1y1 > dist)
		x1y1 = (1.f-fx)*(1.f-fy);
	else
		x1y1 = 0.f;

	return x1y1 + x0y1 + x1y0 + x0y0;
#endif
#if 0
	if(x0y0 > dist)
		x0y0 = dist;
	if(x0y1 > dist)
		x0y1 = dist;
	if(x1y0 > dist)
		x1y0 = dist;
	if(x1y1 > dist)
		x1y1 = dist;

/*   if(x0y0 < dist)
		x0y0 = 0.f;
	if(x0y1 < dist)
		x0y1 = 0.f;
	if(x1y0 < dist)
		x1y0 = 0.f;
	if(x1y1 < dist)
		x1y1 = 0.f;*/

	d = 1.f/dist;

	return ((x1y1*(1.f-fx) + x0y1*(fx)) * (1.f-fy) +
			  (x1y0*(1.f-fx) + x0y0*(fx)) * (fy)) * d;
#endif
}

/*************
 * DESCRIPTION:   Performs lighting for a point on the surface of an object
 * INPUT:         obj         object
 *                surf        surface
 *                hitpos      hit position
 *                norm        object normal
 *                ray         ray
 *                refl        reflected direction
 *                color       actual color
 *                contrib     contribution to actual color
 *                world       world information
 *                k           cosine of angle between normal and ray direction
 *                back        color of current background pixel
 *                state       octree test state
 * OUTPUT:        none
 *************/
void DoLighting(RSICONTEXT *rc, OBJECT *obj, SURFACE *surf, VECTOR *hitpos, VECTOR *norm,
					RAY *ray, VECTOR *refl, COLOR *color, COLOR *contrib, float k,
					SMALL_COLOR *back, int *state)
{
	LIGHT *light;
	COLOR lightcolor, newcontrib, newcolor;
	SMALL_COLOR pixel;
	VECTOR light_dir,circle_pnt, uaxis, vaxis, norm1, light_to_hit;
	float costheta, cosalpha, intens, phi, theta, dist, dist_light_hit, u,v;
	RAY newray;
	BOOL total_int_refl=FALSE;
	OBJECT *reflobj, *transobj, *shadow=NULL;
	unsigned int l;
	BRUSH *brush;
	TEXTURE *texture;
	int tempstate; // temporary octree state

	// First ray: entering surface
	if(ray->depth == 1)
		rc->surface_enter = TRUE;

	// initialize new ray (from object to lightsource)
	newray = *ray;
	newray.lambda = 0.f; //++

	light = rc->light_root;
	while(light)
	{
		if(light->time != ray->time)
		{
			light->Update(ray->time);
			light->time = ray->time;
		}
		lightcolor.r = lightcolor.g = lightcolor.b = 0.f;
		l = 0;
		light->GetLightDirection(hitpos, &light_dir);
		VecSub(hitpos, &light->pos, &light_to_hit);
		if((light->r > 0.f) && (rc->world.softshadow_samples > 1))
			VecCoordSys(&light_to_hit, &uaxis, &vaxis);
		dist_light_hit = VecNormalize(&light_to_hit);
		do
		{
			if((light->r > 0.f) && (rc->world.softshadow_samples > 1))
			{
				// Soft shadows
				// jitter the points on the light source, so we get different directions
				rc->world.UnitCirclePointSoftShadow(&circle_pnt, l);
				VecComb(light->r*circle_pnt.x, &uaxis, light->r*circle_pnt.y, &vaxis, &circle_pnt);
				newray.dir.x = hitpos->x - (light->pos.x + circle_pnt.x);
				newray.dir.y = hitpos->y - (light->pos.y + circle_pnt.y);
				newray.dir.z = hitpos->z - (light->pos.z + circle_pnt.z);
				dist_light_hit = VecNormalize(&newray.dir);
			}
			else
				newray.dir = light_to_hit;
			if(light->DoLight(&light_to_hit, &newcolor))
			{
				if((light->flags & LIGHT_SHADOW) && !(rc->world.flags & WORLD_QUICK) && !(surf->flags & BRIGHT))
				{
					if(light->shadowmap)
					{
						if (light_to_hit.y > 1.f) // roundoff
							phi = PI;
						else
						{
							if (light_to_hit.y < -1.f) // roundoff
								phi = 0.f;
							else
								phi = (float)acos(light_to_hit.y);
						}

						v = phi * INV_PI;

						if (fabs(v) < EPSILON || equal(v, 1.f))
						{
							u = 0.f;
						}
						else
						{
							theta = light_to_hit.x / (float)sin(phi);
							if (theta > 1.f)
								theta = 0.f;
							else
							{
								if (theta < -1.f)
									theta = 0.5f;
								else
									theta = (float)acos(theta) * INV_TWOPI;
							}

							if (light_to_hit.z > 0.f)
								u = 1.f - theta;
							else
								u = theta;
						}

						intens = light->InterpolateShadowMap(u,v, dist_light_hit*.99f);
						newcolor.r *= intens;
						newcolor.g *= intens;
						newcolor.b *= intens;
						shadow = NULL;
					}
					else
					{
						dist = 0.f;
						// do shadow ray
						newray.dir.x = - newray.dir.x;
						newray.dir.y = - newray.dir.y;
						newray.dir.z = - newray.dir.z;
						newray.start = *hitpos;
						do
						{
							shadow = NULL;
							if(rc->shadowcache[ray->depth-1] && (rc->shadowcache[ray->depth-1] != obj))
							{
								if(rc->shadowcache[ray->depth-1]->actor && rc->shadowcache[ray->depth-1]->time != newray.time)
								{
									rc->shadowcache[ray->depth-1]->Update(newray.time);
									rc->shadowcache[ray->depth-1]->time = newray.time;
								}
								newray.lambda = INFINITY;
								if(rc->shadowcache[ray->depth-1]->RayIntersect(&newray))
								{
									shadow = newray.obj;
									STATISTIC(STAT_SHADOWCACHEHIT);
								}
								else
									rc->shadowcache[ray->depth-1]->DisableObject(rc);
							}
							if(!shadow)
							{
								obj->DisableObject(rc);
								STATISTIC(STAT_SHADOWTEST);
								shadow = IntersectRayOctree(rc, &newray, &tempstate, OCTREE_MULTI);
								rc->shadowcache[ray->depth-1] = shadow;
								if(tempstate != OCTREE_NONE && *state != OCTREE_TESTMB)
									*state = tempstate;
							}

							if(shadow)
							{
								if(dist + newray.lambda > dist_light_hit)
								{
									// intersection after lightsource -> ignore
									shadow = NULL;
									rc->shadowcache[ray->depth-1] = NULL;
									break;
								}

								// apply textures and brushes to surface and transmit
								// light trough object if transparent
								rc->surface = *shadow->surf;
								VecAddScaled(&newray.start, newray.lambda+5e-4f, &newray.dir, &newray.start);

								// normal is not needed
								SetVector(&norm1, 1.f, 0.f, 0.f);
								brush = rc->surface.brush;
								while(brush)
								{
									brush->ApplyBrush(obj, &norm1, &rc->surface, &newray.start, ray->time);
									brush = (BRUSH*)brush->GetNext();
								}
								texture = rc->surface.texture;
								while(texture)
								{
									texture->Apply(&norm1, &rc->surface, &newray.start, &newray);
									texture = (TEXTURE*)texture->GetNext();
								}

								// Light is transmitted through object
								if (rc->surface.transpar.r > EPSILON ||
									 rc->surface.transpar.g > EPSILON ||
									 rc->surface.transpar.b > EPSILON)
								{
									newcolor.r *= rc->surface.transpar.r;
									newcolor.g *= rc->surface.transpar.g;
									newcolor.b *= rc->surface.transpar.b;
									shadow = NULL;
									dist += newray.lambda;
								}
								else
								{
									newcolor.r = newcolor.g = newcolor.b = 0.f;
									break;
								}
							}
							else
							{
								break;
							}
						}
						while(!shadow);
					}
				}
				if(!shadow)
				{
					if(light->falloff > EPSILON)
					{
						// calculate fall off
						intens = 1.f - dist_light_hit/light->falloff;
						if(intens > 0.f)
						{
							ColorMultAdd(&lightcolor, intens, &newcolor);
						}
					}
					else
					{
						lightcolor.r += newcolor.r;
						lightcolor.g += newcolor.g;
						lightcolor.b += newcolor.b;
					}
				}
			}
			l++;
		}
		while((light->r > 0.f) && (l < rc->world.softshadow_samples));

		intens = 1.f/l;
		lightcolor.r *= intens;
		lightcolor.g *= intens;
		lightcolor.b *= intens;

		if (lightcolor.r > EPSILON ||
			 lightcolor.g > EPSILON ||
			 lightcolor.b > EPSILON)
		{
			// Between lightsource and object is no object, or object
			// is transparent

			// Bright object -> each point same brightness
			if(surf->flags & BRIGHT)
				costheta = 1.f;
			else
				costheta = -dotp(&light_dir, norm);
			if (costheta <= 0.f)
			{
				// Light source is on opposite side of surface,
				// hence light must be transmitted through...
				if (surf->translucency > EPSILON)
				{
					ColorAddScaled(color, -costheta, &lightcolor, &surf->difftrans);
					if (surf->transphong > EPSILON)
					{
						cosalpha = dotp(refl, &light_dir);
						if(cosalpha > EPSILON)
						{
							// Specular highlight = cosine of the angle raised to the
							// appropriate power.
							intens = (float)pow(cosalpha, surf->transphong);
							ColorAddScaled(color, intens, &lightcolor, &surf->spectrans);
						}
					}
					ColorScale(surf->translucency, color, color);
				}
			}
			else
			{
				// Diffuse reflection.
				// Falls off as the cosine of the angle between
				// the normal and the ray to the light (costheta).
				ColorAddScaled(color, costheta, &lightcolor, &surf->diffuse);
				/*
				 * Specularly reflected highlights.
				 * Fall off as the cosine of the angle
				 * between the reflected ray and the ray to the light source.
				 */
				if(surf->refphong > EPSILON)
				{
					cosalpha = -dotp(refl, &light_dir);
					// Specular highlight = cosine of the angle raised to the
					// appropriate power.
					if(cosalpha > EPSILON)
					{
						intens = (float)pow(cosalpha, surf->refphong);
						ColorAddScaled(color, intens, &lightcolor, &surf->specular);
					}
				}
			}
		}
		light = (LIGHT *)light->GetNext();
	}
	if(rc->world.flags & WORLD_QUICK)
	{
		// in quick mode -> apply global reflection map
		newray.start = *hitpos;
		newray.dir = *refl;        // Direction == reflection
		if(!rc->world.reflmap)
		{
			pixel.r = UBYTE(rc->world.back.r * 255);
			pixel.g = UBYTE(rc->world.back.g * 255);
			pixel.b = UBYTE(rc->world.back.b * 255);
			// Do global fog
			if(!rc->world.DoGlobalFog(&newray,&pixel,&newcolor))
			{
				newcolor = rc->world.back;
			}
		}
		else
		{
			ApplyReflMap(&newray,&newcolor, &rc->world);
		}
		ColorMultiply(newcolor, surf->reflect, &newcolor);
		ColorAdd(*color, newcolor, color);
		return;
	}

	if(ray->depth >= MAXRAYDEPTH || (rc->world.flags & WORLD_QUICK))
	{
		// Don't spawn any transmitted/reflected rays.
		return;
	}

	newray.lambda = 0.f; //++

	// Specular transmission (refraction).
	if (surf->transpar.r > EPSILON ||
		 surf->transpar.g > EPSILON ||
		 surf->transpar.b > EPSILON)
	{
		ColorMultiply(surf->transpar, *contrib, &newcontrib);
		// Only cast new ray if contrib > CUTOFF
		if (newcontrib.r > 0.002f ||
			 newcontrib.g > 0.002f ||
			 newcontrib.b > 0.002f)
		{
			if (rc->surface_enter)
			{
				// Entering surface.
				total_int_refl = Refract(&newray.dir, 1.f/surf->refrindex, &ray->dir, norm, k);
				rc->surface_enter = FALSE;
			}
			else
			{
				// Leaving surface.
				total_int_refl = Refract(&newray.dir, surf->refrindex, &ray->dir, norm, k);
				rc->surface_enter = TRUE;
			}
			if (!total_int_refl)
			{
				// Origin == hit point
				// go away from surface
				VecAddScaled(hitpos, 5e-4f, &ray->dir, &newray.start);
				STATISTIC(STAT_TRANSRAY);
				transobj = IntersectRayOctree(rc, &newray, &tempstate, OCTREE_LINE);
				if(tempstate != OCTREE_NONE && *state != OCTREE_TESTMB)
					*state = tempstate;
				if(transobj)
				{
					// ray hits!
					newray.depth = ray->depth+1;
					transobj->GetColor(rc, &newray, &newcolor, &newcontrib, back, state);
					if (surf->foglength > EPSILON)
					{
						// Add fog to the given color.
						intens = LNHALF * newray.lambda/surf->foglength;
						intens = intens < -10.f ? 0.f : (float)exp(intens);
						newcolor.r = intens*newcolor.r + (1.f-intens)*surf->transpar.r;
						newcolor.g = intens*newcolor.g + (1.f-intens)*surf->transpar.g;
						newcolor.b = intens*newcolor.b + (1.f-intens)*surf->transpar.b;
					}
					else
						ColorMultiply(newcolor, surf->transpar, &newcolor);
				}
				else
				{ // ray misses, use background color
					if(!rc->world.backdrop)
					{
						pixel.r = UBYTE(rc->world.back.r * 255.f);
						pixel.g = UBYTE(rc->world.back.g * 255.f);
						pixel.b = UBYTE(rc->world.back.b * 255.f);
						// Do global fog
						if(!rc->world.DoGlobalFog(&newray,&pixel,&newcolor))
						{
							newcolor = rc->world.back;
						}
					}
					else
					{
						// Do global fog
						if(!rc->world.DoGlobalFog(&newray,back,&newcolor))
						{
							newcolor.r = float(back->r)/255.f;
							newcolor.g = float(back->g)/255.f;
							newcolor.b = float(back->b)/255.f;
						}
					}
					ColorMultiply(newcolor, surf->transpar, &newcolor);
				}
				color->r = color->r * (1.f-surf->transpar.r) + newcolor.r;
				color->g = color->g * (1.f-surf->transpar.g) + newcolor.g;
				color->b = color->b * (1.f-surf->transpar.b) + newcolor.b;
			}
		}
	}

	// reflection
	if (surf->reflect.r > EPSILON ||
		 surf->reflect.g > EPSILON ||
		 surf->reflect.b > EPSILON)
	{
		ColorMultiply(surf->reflect, *contrib, &newcontrib);
		// Only cast new ray if contrib > CUTOFF
		if (newcontrib.r > .002f ||
			 newcontrib.g > .002f ||
			 newcontrib.b > .002f)
		{
			// Don't hit reflective object again

			// Ok, here are some problems. Normally we should go away from
			// surface to avoid hitting this surface again. But due to
			// rounding errors it's possible that we hit this surface again.
			// So we disable the object and hope no one want's reflections
			// inside of a perfect sphere (Note: maybe later we have complexer
			// objects (spline or heightfields or something like that) then
			// we'll have this problem again :-( )
//         VecAddScaled(hitpos, 5e-4f, refl, &newray.start);

			newray.start = *hitpos;
			obj->DisableObject(rc);
			newray.dir = *refl;        // Direction == reflection
			STATISTIC(STAT_REFLRAY);
			reflobj = IntersectRayOctree(rc, &newray, &tempstate, OCTREE_LINE);
			if(tempstate != OCTREE_NONE && *state != OCTREE_TESTMB)
				*state = tempstate;
			if(reflobj)
			{
				// ray hits!
				newray.depth = ray->depth+1;
				reflobj->GetColor(rc, &newray, &newcolor, &newcontrib, back, state);
			}
			else
			{ // ray misses, use background color
				if(!rc->world.reflmap)
				{
					pixel.r = UBYTE(rc->world.back.r * 255.f);
					pixel.g = UBYTE(rc->world.back.g * 255.f);
					pixel.b = UBYTE(rc->world.back.b * 255.f);
					// Do global fog
					if(!rc->world.DoGlobalFog(&newray,&pixel,&newcolor))
					{
						newcolor = rc->world.back;
					}
				}
				else
				{
					ApplyReflMap(&newray, &newcolor, &rc->world);
				}
			}
			ColorMultiply(newcolor, surf->reflect, &newcolor);
			ColorAdd(*color, newcolor, color);
		}
	}
	ray->ray_length += newray.lambda; //++
}

/*************
 * DESCRIPTION:   Inserts a object in the light list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void LIGHT::Insert(RSICONTEXT *rc)
{
	rc->Insert(this);
}

/*************
 * DESCRIPTION:   get the direction from a light source to the hitpoint
 * INPUT:         hitpoint
 *                dir
 * OUTPUT:        -
 *************/
void LIGHT::GetLightDirection(VECTOR *hitpos, VECTOR *dir)
{
	VecSub(hitpos, &pos, dir);
	VecNormalizeNoRet(dir);
}

/*************
 * DESCRIPTION:   update all lights
 * INPUT:         time        current time
 * OUTPUT:        none
 *************/
void UpdateLights(RSICONTEXT *rc, const float time)
{
	LIGHT *light;

	light = rc->light_root;
	while(light)
	{
		light->Update(time);
		light->time = time;
		light = (LIGHT*)light->GetNext();
	}
}

/*************
 * DESCRIPTION:   generate shadowmaps for all light sources
 * INPUT:         -
 * OUTPUT:        -
 *************/
void InitShadowMap(RSICONTEXT *rc)
{
	LIGHT *light;
	int state;
	int i,j, lights=0, curlight;
	RAY ray;
	float u,v, su;
	clock_t starttime;
	float time, oldtime;

	light = rc->light_root;
	while(light)
	{
		if((light->flags & LIGHT_SHADOW) && (light->flags & LIGHT_SHADOWMAP))
			lights++;
		light = (LIGHT*)light->GetNext();
	}

	starttime = clock();
	oldtime = 0;
	curlight = 0;
	if(lights)
	{
		light = rc->light_root;
		while(light)
		{
			if((light->flags & LIGHT_SHADOW) && (light->flags & LIGHT_SHADOWMAP))
			{
				// build shadow map
				light->shadowmap = new float[light->shadowmap_size*(light->shadowmap_size>>1)];
				if(light->shadowmap)
				{
					ray.start = light->pos;
					for(i=0; i<light->shadowmap_size>>1; i++)
					{
						for(j=0; j<light->shadowmap_size; j++)
						{
							u = ((float)i)/(light->shadowmap_size>>1)*PI;
							su = (float)sin(u);
							u = (float)cos(u);
							v = ((float)j)/light->shadowmap_size*2.f*PI;

							SetVector(&ray.dir, su*(float)cos(v), u, su*(float)sin(v));
							IntersectRayOctree(rc, &ray, &state, OCTREE_MULTI);
							light->shadowmap[i*light->shadowmap_size + light->shadowmap_size-1-j] = ray.lambda;
						}
						time = float(clock()-starttime)/CLOCKS_PER_SEC;
						if(time > oldtime + UPDATE_INTERVAL)
						{
							rc->UpdateStatus(rc, (float)curlight/(float)lights + (float)i/(float)((light->shadowmap_size>>1)*lights), time, 0, 0, 0);
							oldtime = time;
						}
					}
				}
			}
			rc->Cooperate(rc);
			light = (LIGHT*)light->GetNext();
		}
	}
}

/*************
 * DESCRIPTION:   test if there is any light with a shadow map
 * INPUT:         -
 * OUTPUT:        TRUE if any shadow map else FALSE
 *************/
BOOL TestIfShadowMaps(RSICONTEXT *rc)
{
	LIGHT *light;

	light = rc->light_root;
	while(light)
	{
		if((light->flags & LIGHT_SHADOW) && (light->flags & LIGHT_SHADOWMAP))
			return TRUE;
		light = (LIGHT*)light->GetNext();
	}
	return FALSE;
}

/*************
 * DESCRIPTION:   Get lightcolor and intensity for a point light
 * INPUT:         dir         light-ray direction
 *                color       color of light
 * OUTPUT:        TRUE if non-zero illumination, else FALSE
 *************/
BOOL POINT_LIGHT::DoLight(VECTOR *dir, COLOR *color)
{
	*color = this->color;
	return TRUE;
}

/*************
 * DESCRIPTION:   Update light source
 * INPUT:         time        actual time
 * OUTPUT:        none
 *************/
void POINT_LIGHT::Update(const float time)
{
	TIME t;

	if(actor)
	{
		if(time != this->time)
		{
			if((actor->time.begin != this->time) || (actor->time.end != time))
			{
				t.begin = this->time;
				t.end = time;
				actor->Animate(&t);
			}
			// animated lightsource
			if(r != 0.f)
			{
				// light has radius -> animate radius
				r *= actor->act_size.x;
				r /= actor->axis_size.x;
			}
			actor->matrix->MultVectMat(&pos);
		}
	}
}

/*************
 * DESCRIPTION:   test if light is visible (for lens flares)
 * INPUT:         ray      casted ray, initialized with camera position as start point,
 *    direction from camera to light and time as starttime of the frame
 * OUTPUT:        TRUE for visible FALSE for invisible
 *************/
BOOL POINT_LIGHT::TestVisibility(RSICONTEXT *rc, RAY *ray)
{
	int state;

	// motion blur problem (is light visible, how long at which position ?)
	return !IntersectRayOctree(rc, ray, &state, OCTREE_START);
}

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
SPOT_LIGHT::SPOT_LIGHT()
{
	lookp.x = 0.f;
	lookp.y = 0.f;
	lookp.z = 1.f;
	angle = 45.f*PI_180;
	lookp_actor = NULL;
	falloff_radius = 1.f;
}

/*************
 * DESCRIPTION:   Get lightcolor and intensity for a spot light
 * INPUT:         dir         light-hitpoint direction
 *                color       color of light
 * OUTPUT:        TRUE if non-zero illumination, else FALSE
 *************/
BOOL SPOT_LIGHT::DoLight(VECTOR *dir, COLOR *color)
{
	float costheta;
	VECTOR ldir;

	// Calculate light direction
	VecSub(&lookp,&pos,&ldir);
	VecNormalizeNoRet(&ldir);
	// Compute intensity of spotlight along 'dir'.
	costheta = dotp(dir, &ldir);
	// Behind spotlight.
	if (costheta < angle)
		return FALSE;
	if(falloff_radius < 1.f)
	{
		if(angle == 1.f)
			return FALSE;
		costheta = (1.f - costheta) / (1.f - angle);
		if (costheta <= falloff_radius)
			*color = this->color;
		else
		{
			// cubic interpolation
			costheta = (costheta - 1.f) / (falloff_radius - 1.f);
			costheta = (3 - 2*costheta)*costheta*costheta;
			color->r = this->color.r * costheta;
			color->g = this->color.g * costheta;
			color->b = this->color.b * costheta;
		}
	}
	else
		*color = this->color;
	return TRUE;
}

/*************
 * DESCRIPTION:   Update light source
 * INPUT:         time        actual time
 * OUTPUT:        none
 *************/
void SPOT_LIGHT::Update(const float time)
{
	TIME t;

	if(actor)
	{
		if(time != this->time)
		{
			if((actor->time.begin != this->time) || (actor->time.end != time))
			{
				t.begin = this->time;
				t.end = time;
				actor->Animate(&t);
			}
			// animated lightsource
			if(r != 0.f)
			{
				// light has radius -> animate radius
				r *= actor->act_size.x;
				r /= actor->axis_size.x;
			}
			actor->matrix->MultVectMat(&pos);
		}
	}
	if(lookp_actor)
	{
		if(time != this->time)
		{
			// animate lookpoint
			if((lookp_actor->time.begin != this->time) || (lookp_actor->time.end != time))
			{
				t.begin = this->time;
				t.end = time;
				lookp_actor->Animate(&t);
			}
			lookp_actor->matrix->MultVectMat(&lookp);
		}
	}
}

/*************
 * DESCRIPTION:   test if light is visible (for lens flares)
 * INPUT:         ray      casted ray, initialized with camera position as start point,
 *    direction from camera to light and time as starttime of the frame
 * OUTPUT:        TRUE for visible FALSE for invisible
 *************/
BOOL SPOT_LIGHT::TestVisibility(RSICONTEXT *rc, RAY *ray)
{
	int state;
	float costheta;
	VECTOR ldir;

	VecSub(&lookp,&pos,&ldir);
	VecNormalizeNoRet(&ldir);
	costheta = -dotp(&ray->dir, &ldir);
	// outside of light spot ?
	if (costheta <= 0.f || costheta < angle)
	{
		ray->lambda = -INFINITY;
		return FALSE;
	}

	// motion blur problem (is light visible, how long at which position ?)
	return !IntersectRayOctree(rc, ray, &state, OCTREE_START);
}

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
DIRECTIONAL_LIGHT::DIRECTIONAL_LIGHT()
{
	// a directional light is pointing from a position to a direction
	SetVector(&dir, 0.f, 0.f, 1.f);
}

/*************
 * DESCRIPTION:   Get lightcolor and intensity for a directional light
 * INPUT:         dir         light-ray direction
 *                color       color of light
 * OUTPUT:        TRUE if non-zero illumination, else FALSE
 *************/
BOOL DIRECTIONAL_LIGHT::DoLight(VECTOR *dir, COLOR *color)
{
	float costheta;

	costheta = dotp(dir, &this->dir);
	// Behind light
	if (costheta <= 0.f)
		return FALSE;

	*color = this->color;
	return TRUE;
}

/*************
 * DESCRIPTION:   Update light source
 * INPUT:         time        actual time
 * OUTPUT:        none
 *************/
void DIRECTIONAL_LIGHT::Update(const float time)
{
	TIME t;

	if(actor)
	{
		if(time != this->time)
		{
			if((actor->time.begin != this->time) || (actor->time.end != time))
			{
				t.begin = this->time;
				t.end = time;
				actor->Animate(&t);
			}
			// animated lightsource
			if(r != 0.f)
			{
				// light has radius -> animate radius
				r *= actor->act_size.x;
				r /= actor->axis_size.x;
			}
			actor->matrix->MultVectMat(&pos);
			actor->rotmatrix->MultVectMat(&dir);
		}
	}
}

/*************
 * DESCRIPTION:   test if light is visible (for lens flares)
 * INPUT:         ray      casted ray, initialized with camera position as start point,
 *    direction from camera to light and time as starttime of the frame
 * OUTPUT:        TRUE for visible FALSE for invisible
 *************/
BOOL DIRECTIONAL_LIGHT::TestVisibility(RSICONTEXT *rc, RAY *ray)
{
	int state;

	// motion blur problem (is light visible, how long at which position ?)
	return !IntersectRayOctree(rc, ray, &state, OCTREE_START);
}

/*************
 * DESCRIPTION:   get the direction from a light source to the hitpoint
 * INPUT:         hitpoint
 *                dir
 * OUTPUT:        -
 *************/
void DIRECTIONAL_LIGHT::GetLightDirection(VECTOR *hitpos, VECTOR *dir)
{
	*dir = this->dir;
}

/*************
 * DESCRIPTION:   draws flares and visible lights to screen buffer
 * INPUT:         rc    render context
 * OUTPUT:        none
 *************/
void ApplyLensEffects(RSICONTEXT *rc)
{
	LIGHT *light;
	FLARE *flare;
	SMALL_COLOR *line, *oldline;
	int scrwh, scrhh;
	int i,oldi, x,y,w, fx, lightx, lighty;
	VECTOR dir;
	RAY ray;
	BOOL visible;
	float percent, percent_per_row, lambda;
	clock_t starttime;
	VECTOR norm, sx, sy;
	float d, d1;
	float time, oldtime=0;

	starttime = clock();

	scrwh = rc->camera.xres >> 1;
	scrhh = rc->camera.yres >> 1;

	line = rc->scrarray + rc->camera.top*rc->camera.xres;
	oldline = line;

	percent_per_row = (float)1./(rc->camera.bottom - rc->camera.top);

	// we need the normal of the view plane to calculate the screen position
	VecSub(&rc->camera.lookp, &rc->camera.pos, &norm);
	VecNormalizeNoRet(&norm);
	d1 = dotp(&norm, &rc->camera.lookp) - dotp(&norm, &rc->camera.pos);

	sx = rc->camera.scrnx;
	d = VecNormalize(&sx);
	VecScale(1.f/d, &sx, &sx);
	sy = rc->camera.scrny;
	d = VecNormalize(&sy);
	VecScale(1.f/d, &sy, &sy);

	oldi = rc->camera.top;
	for (i = rc->camera.top; (i < rc->camera.bottom) && !rc->CheckCancel(rc); i++)
	{
		// now we have to go through all lights and draw
		// the lens flares they eventually have
		light = rc->light_root;

		while (light)
		{
			if ((light->flares && (light->flags & LIGHT_FLARES)) ||
				(light->star && (light->flags & LIGHT_STAR)))
			{
				// find out if light source is visible
				// therefore you need to shoot a ray from camera to light source
				// if there is no object between, and the hit is within camera view -> draw lens flares

				VecSub(&light->pos, &rc->camera.pos, &dir);
				lambda = VecNormalize(&dir);
				ray.dir = dir;
				ray.start = rc->camera.pos;
				visible = light->TestVisibility(rc, &ray);

				// blocking object is behind light
				if (visible || ray.lambda > lambda)
				{
					d = dotp(&norm, &dir);
					if (fabs(d) > EPSILON)
					{
						d = d1 / d;

						if (d > EPSILON)
						{
							VecScale(d, &dir, &dir);

							VecSub(&dir, &rc->camera.firstray, &dir);
							d = VecNormalize(&dir);

							lightx = (int)floor(dotp(&sx, &dir)*d);
							lighty = rc->camera.yres - (int)floor(dotp(&sy, &dir)*d) - 1;

							if((lightx >= rc->camera.left) && (lightx < rc->camera.right) &&
								(lighty >= rc->camera.top) && (lighty < rc->camera.bottom))
							{
								if(light->flags & LIGHT_FLARES)
								{
									flare = light->flares;

									while (flare)
									{
										y = (scrhh - int((lighty - scrhh)*(flare->pos - 1.f))) - i;

										// if we are in the area of the flare, draw one row
										if (y >= -flare->sr && y <= flare->sr)
										{
											// calculate screen position of flare
											fx = scrwh - int((lightx - scrwh)*(flare->pos - 1.f));
											w = int(sqrt(float(flare->sr*flare->sr - y*y)));

											for (x = -w; x < w; x++)
											{
												if ((fx + x) > rc->camera.left && (fx + x) < rc->camera.right)
													flare->Draw(line, fx, x,y);
											}
										}
										flare = (FLARE *)flare->GetNext();
									}
								}
							}
							if (light->star && (light->flags & LIGHT_STAR))
							{
								y = lighty - i;
								for (x = (int)floor(-light->star->acc_sr); x < (int)floor(light->star->acc_sr); x++)
								{
									if ((lightx + x) > rc->camera.left && (lightx + x) < rc->camera.right)
										light->star->Draw(line, &light->color, lightx, x, y);
								}
							}
						}
					}
				}
			}
			light = (LIGHT*)light->GetNext();
		}
		time = float(clock()-starttime)/CLOCKS_PER_SEC;
		if(time > oldtime + UPDATE_INTERVAL)
		{
			percent = (i - rc->camera.top) * percent_per_row;
			rc->UpdateStatus(rc, percent, time, oldi, i-oldi, oldline);
			oldtime = time;
			oldline = line;
			oldi = i;
		}
		rc->Cooperate(rc);

		line += rc->camera.xres;
	}
	if(i-oldi > 1)
		rc->UpdateStatus(rc, 1.f, float(clock()-starttime)/CLOCKS_PER_SEC, oldi, i-oldi-1, oldline);
}

/*************
 * DESCRIPTION:   precalculate lens flare values
 * INPUT:         xres     screen resolution
 * OUTPUT:        none
 *************/
void UpdateLensEffects(RSICONTEXT *rc, int xres)
{
	LIGHT *light;
	FLARE *flare;

	light = rc->light_root;
	while(light)
	{
		flare = light->flares;
		while(flare)
		{
			flare->Update(xres);
			flare = (FLARE *)flare->GetNext();
		}
		if(light->star)
			light->star->Update(xres);
		light = (LIGHT*)light->GetNext();
	}
}

/*************
 * DESCRIPTION:   test if there is any light with lens effects
 * INPUT:         -
 * OUTPUT:        TRUE if any lens effects else FALSE
 *************/
BOOL TestIfLensEffects(RSICONTEXT *rc)
{
	LIGHT *light;

	light = rc->light_root;
	while(light)
	{
		if(light->flares || light->star)
			return TRUE;
		light = (LIGHT*)light->GetNext();
	}
	return FALSE;
}
