/***************
 * MODUL:         htexture
 * NAME:          htexture.cpp
 * DESCRIPTION:   This file includes all functions for the hypertexture class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.03.97 ah    initial release
 ***************/

#include "stdlib.h"

#ifndef HTEXTURE_H
#include "htexture.h"
#endif

#ifndef SPHERE_H
#include "sphere.h"
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
HYPERTEXTURE::HYPERTEXTURE()
{
	SetVector(&pos, 0.f, 0.f, 0.f);
	SetVector(&orient_x, 1.f, 0.f, 0.f);
	SetVector(&orient_y, 0.f, 1.f, 0.f);
	SetVector(&orient_z, 0.f, 0.f, 1.f);
	SetVector(&size, 1.f, 1.f, 1.f);

	actor = NULL;
	time = 0;
}

/*************
 * DESCRIPTION:   Apply a hypertexture to surface
 * INPUT:         pos      current position on surface
 *                obj      current object
 *                color    color of texture
 *                time     current time
 * OUTPUT:        density
 *************/
float HYPERTEXTURE::ApplyTextures(RSICONTEXT *rc, const VECTOR *pos, OBJECT *obj, COLOR *color, float time)
{
	VECTOR relpos;
	HYPERTEXTURE *texture;
	float density, dens, objdensity;

	objdensity = obj->Density(pos);

	dens = 0.f;
	texture = this;
	while(texture)
	{
		// update texture if neccassary
		if(texture->actor && texture->time != time)
			texture->Update(time);

		VecSub(pos, &texture->pos, &relpos);
		relpos.x = dotp(&texture->orient_x, &relpos) / texture->size.x;
		relpos.y = dotp(&texture->orient_y, &relpos) / texture->size.y;
		relpos.z = dotp(&texture->orient_z, &relpos) / texture->size.z;

		density = texture->Density(rc, &relpos, objdensity);
		texture->Color(density, color);
		dens += density;
		texture = (HYPERTEXTURE*)texture->GetNext();
	}
	return (dens > 1.f ? 1.f : dens);
}

/*************
 * DESCRIPTION:   Update texture axis
 * INPUT:         time     actual time
 * OUTPUT:        none
 *************/
void HYPERTEXTURE::Update(const float time)
{
	TIME t;
	MATRIX r;

	if((actor->time.begin != this->time) || (actor->time.end != time))
	{
		t.begin = this->time;
		t.end = time;
		actor->Animate(&t);
	}
	actor->matrix->MultVectMat(&pos);

	actor->rotmatrix->InvMat(&r);
	r.MultVectMat(&orient_x);
	r.MultVectMat(&orient_y);
	r.MultVectMat(&orient_z);

	size.x *= actor->act_size.x / actor->axis_size.x;
	size.y *= actor->act_size.y / actor->axis_size.y;
	size.z *= actor->act_size.z / actor->axis_size.z;

	this->time = time;
}
/*************
 * DESCRIPTION:   Constructor of explosion texture
 * INPUT:         none
 * OUTPUT:        none
 *************/
EXPLOSION::EXPLOSION()
{
	p1 = 3.f; // frequency
	p2 = 2.f; // amplitude
	p3 = 3.f; // octaves
}

/*************
 * DESCRIPTION:   compute texture density function
 * INPUT:         pos      position
 *                density  object density
 * OUTPUT:        texture density
 *************/
float EXPLOSION::Density(RSICONTEXT *rc, const VECTOR *pos, float density)
{
	VECTOR p;
	float t;

	if(density > 0.f)
	{
		VecSub(pos, &this->pos, &p);
		VecScale(p1, &p, &p);
		t = p2 * Turbulence(&p, (int)p3, rc->hashTable);
		if(t < 0.f)
			t = 0.f;

		density *= t;
	}

	return density;
}

/*************
 * DESCRIPTION:   map densities to colors
 * INPUT:         density     density
 *                color       color
 * OUTPUT:        none
 *************/
void EXPLOSION::Color(float density, COLOR *color)
{
	color->r = 1.f;
	color->g = density > 1.f ? 1.f : density;
	color->b = 0.f;
}

#ifdef __MURX__
/*************
 * DESCRIPTION:
 * INPUT:         a     value
 *                b     factor
 * OUTPUT:        new value
 *************/
static float bias(float a, float b)
{
	return pow((double)a, log(b)/log(.5f));
// return pow(a, log(b)*-1.442695041f);
}

/*************
 * DESCRIPTION:
 * INPUT:         a     value
 *                b     factor
 * OUTPUT:        new value
 *************/
static float gain(float a, float b)
{
	float p = log(1.f - b) / log(.5f);
// float p = log(1.f - b) * -1.442695041f;

	if (a < .001f)
		return 0.f;
	else if (a > .999f)
		return 1.f;
	if (a < 0.5f)
		return pow((double)(2.f * a), (double)p) * 0.5f;
	else
		return 1.f - pow((double)(2.f * (1.f - a)), (double)p) * 0.5f;
}
#endif
