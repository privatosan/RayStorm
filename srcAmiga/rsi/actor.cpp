/***************
 * MODUL:         actor
 * NAME:          actor.cpp
 * DESCRIPTION:   This file includes the definition of the actor-class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    23.08.95 ah    initial release
 *    28.08.95 ah    added Animate(), AnimateAll()
 *    29.08.95 ah    added FindAction(), FindActionBefore(), FindTransAction()
 *    31.08.95 ah    added CalcTime()
 *    21.10.95 ah    added precalculated matrices
 *    09.01.96 ah    fixed bug in CalcVoxel(); now ALL eight points of the
 *       given voxel are used for the new voxel
 ***************/

#include <stdlib.h>
#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef ACTOR_H
#include "actor.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
ACTOR::ACTOR()
{
	SetVector(&axis_pos, 0.f, 0.f, 0.f);
	axis_align = axis_pos;
	SetVector(&axis_size, 1.f, 1.f, 1.f);
	pos = NULL;
	align = NULL;
	size = NULL;
	time.begin = time.end = -1.f;
	pre_mat = NULL;
	matrix = NULL;
	invmatrix = NULL;
	rotmatrix = NULL;
}

/*************
 * DESCRIPTION:   destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
ACTOR::~ACTOR()
{
	if(pre_mat)
		delete [ ] pre_mat;
	else
	{
		// random jitter
		if(matrix)
			delete matrix;
		if(invmatrix)
			delete invmatrix;
		if(rotmatrix)
			delete rotmatrix;
	}

	if(pos)
		pos->FreeAll();

	if(align)
		align->FreeAll();

	if(size)
		size->FreeAll();
}

/*************
 * DESCRIPTION:   Inserts a actor in the actor list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void ACTOR::Insert(RSICONTEXT *rc)
{
	rc->Insert(this);
}

/*************
 * DESCRIPTION:   Takes a point and translates it with the actor matrix and
 *  calculates new voxel boundaries
 * INPUT:         p        point
 *                voxel    voxel
 * OUTPUT:        none
 *************/
void ACTOR::MaxVoxel(VECTOR *p, VOXEL *voxel)
{
	matrix->MultVectMat(p);
	// test if animated points are outside of voxel
	if(p->x < voxel->min.x)
		voxel->min.x = p->x;
	if(p->y < voxel->min.y)
		voxel->min.y = p->y;
	if(p->z < voxel->min.z)
		voxel->min.z = p->z;
	if(p->x > voxel->max.x)
		voxel->max.x = p->x;
	if(p->y > voxel->max.y)
		voxel->max.y = p->y;
	if(p->z > voxel->max.z)
		voxel->max.z = p->z;
}

/*************
 * DESCRIPTION:   Calculates the voxel of a animated object
 * INPUT:         time        time intervall of frame
 *                voxel       initialized voxel for begin of intervall
 *                world       pointer to world structure
 * OUTPUT:        none
 *************/
void ACTOR::CalcVoxel(VOXEL *voxel, const WORLD *world)
{
	VECTOR p1,p2,p3,p4,p5,p6,p7,p8;
	TIME time;
	int i;

	time.begin = world->time;
	// animate the eight corner points of the voxel and change voxel if
	// neccessary
	p1 = voxel->min;
	SetVector(&p2, voxel->min.x, voxel->min.y, voxel->max.z);
	SetVector(&p3, voxel->min.x, voxel->max.y, voxel->min.z);
	SetVector(&p4, voxel->min.x, voxel->max.y, voxel->max.z);
	SetVector(&p5, voxel->max.x, voxel->min.y, voxel->min.z);
	SetVector(&p6, voxel->max.x, voxel->min.y, voxel->max.z);
	SetVector(&p7, voxel->max.x, voxel->max.y, voxel->min.z);
	p8 = voxel->max;



	for(i=1; i < (int)world->motionblur_samples-1; i++)
	{
		time.end = CalcTime(i, world);
		// update matrix
		if((time.begin != this->time.begin) || (time.end != this->time.end))
			this->Animate(&time);
		time.begin = time.end;

		MaxVoxel(&p1,voxel);
		MaxVoxel(&p2,voxel);
		MaxVoxel(&p3,voxel);
		MaxVoxel(&p4,voxel);
		MaxVoxel(&p5,voxel);
		MaxVoxel(&p6,voxel);
		MaxVoxel(&p7,voxel);
		MaxVoxel(&p8,voxel);
	}

	// sample last position
	time.end = world->time + world->timelength;
	// update matrix
	if((time.begin != this->time.begin) || (time.end != this->time.end))
		this->Animate(&time);

	MaxVoxel(&p1,voxel);
	MaxVoxel(&p2,voxel);
	MaxVoxel(&p3,voxel);
	MaxVoxel(&p4,voxel);
	MaxVoxel(&p5,voxel);
	MaxVoxel(&p6,voxel);
	MaxVoxel(&p7,voxel);
	MaxVoxel(&p8,voxel);
}

/*************
 * DESCRIPTION:   Precalculates the matrices for all actors
 * INPUT:         cur_time    time intervall of frame
 * OUTPUT:        FALSE -> out of memory, else TRUE
 *************/
BOOL PreCalcMatrices(RSICONTEXT *rc, const TIME *cur_time)
{
	int i;
	ACTOR *cur;
	TIME time;
	PRE_MATRIX *pre_mat;

	cur = rc->actor_root;
	time.begin = cur_time->end;
	while(cur)
	{
		// random jitter -> allocate space for one matrix
		// else build a list of precalculate matrices
		if((rc->world.flags & WORLD_RANDOM_JITTER) || (rc->world.timelength == 0.))
		{
			if(!cur->matrix)
			{
				cur->matrix = new MATRIX;
				if(!cur->matrix)
					return FALSE;
			}
			if(!cur->invmatrix)
			{
				cur->invmatrix = new MATRIX;
				if(!cur->invmatrix)
					return FALSE;
			}
			if(!cur->rotmatrix)
			{
				cur->rotmatrix = new MATRIX;
				if(!cur->rotmatrix)
					return FALSE;
			}
		}
		else
		{
			// Free old precalculated matrices
			if(cur->pre_mat)
			{
				cur->pre_mat->FreeAll();
				cur->pre_mat = NULL;
			}

			i = 0;
			do
			{
				// jitter in time
				time.end = CalcTime(i, &rc->world);
				// find size,alignment and position at begin of intervall
				if(cur->time.begin != time.begin)
				{
					// set begin axis positions
					cur->FindSize(&cur->axis_size, time.begin);
					cur->FindAlignment(&cur->axis_align, time.begin);
					cur->FindPosition(&cur->axis_pos, time.begin);
				}
				// generate matrix
				pre_mat = new PRE_MATRIX;
				if(!pre_mat)
					return FALSE;
				cur->CalcMatrix(&pre_mat->matrix,&pre_mat->invmatrix,&pre_mat->rotmatrix,time.end);
				pre_mat->time = time;
				cur->AddPreMatrix(pre_mat);

				// next time intervall
				i++;
				time.begin = time.end;
			}
			while(i < (int)rc->world.motionblur_samples);
		}

		cur = (ACTOR*)cur->GetNext();
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   Calculates the matrix for a time intervall
 * INPUT:         time        time intervall of frame
 * OUTPUT:        none
 *************/
void ACTOR::Animate(const TIME *time)
{
	PRE_MATRIX *pre;

	// find size,alignment and position at begin of intervall
	if(this->time.begin != time->begin)
	{
		// set begin axis positions
		FindSize(&axis_size, time->begin);
		FindAlignment(&axis_align, time->begin);
		FindPosition(&axis_pos, time->begin);
	}
	// two ways to find the matrix
	// - random jitter -> calculate it each time
	// - else use precalculated matrices
	if(pre_mat)
	{
		// no random jitter -> use precalculated matrices
		pre_mat->FindMatrix(&matrix, &invmatrix, &rotmatrix, time);
		if(!matrix)
		{
			pre = new PRE_MATRIX;
			if(!pre)
			{
				// Failed to get memory -> use first pre matrix in list
				// it's better than making a crash, we need this matrix
				pre = pre_mat;
				CalcMatrix(&pre->matrix, &pre->invmatrix, &pre->rotmatrix, time->end);
				pre->time = *time;
			}
			else
			{
				CalcMatrix(&pre->matrix, &pre->invmatrix, &pre->rotmatrix, time->end);
				pre->time = *time;
				AddPreMatrix(pre);
			}
			matrix = &pre->matrix;
			invmatrix = &pre->invmatrix;
			rotmatrix = &pre->rotmatrix;
		}
		else
		{
			if(this->time.end != time->end)
			{
				// set end axis positions
				FindSize(&act_size, time->end);
				FindAlignment(&act_align, time->end);
				FindPosition(&act_pos, time->end);
			}
		}
	}
	else
	{
		// generate matrix
		CalcMatrix(matrix,invmatrix,rotmatrix,time->end);
	}
	this->time = *time;
}

/*************
 * DESCRIPTION:   Generates a animation matrix; axis parameters must be set
 * INPUT:         matrix      pointer to matrix
 *                invmatrix   pointer to inverted matrix
 *                rotmatrix   pointer to rotationmatrix
 *                end         end of time intervall
 * OUTPUT:        none
 *************/
void ACTOR::CalcMatrix(MATRIX *matrix, MATRIX *invmatrix, MATRIX *rotmatrix, const float end)
{
	VECTOR s,a,p;
	MATRIX m1,m2;

	// we need to translate the object to rotate and scale relative to actor axis
	SetVector(&p, -axis_pos.x, -axis_pos.y, -axis_pos.z);
	m1.SetTransMatrix(&p);

	// build a scale-rotate-translate matrix
	// determine size, alignment and position values at end of intervall

	FindSize(&act_size, end);
	// we need relative values for size
	s.x = act_size.x/axis_size.x;
	s.y = act_size.y/axis_size.y;
	s.z = act_size.z/axis_size.z;

	FindAlignment(&act_align, end);
	// and relative values for alignment
	VecSub(&act_align, &axis_align, &a);

	// find position
	FindPosition(&act_pos, end);

	// generate matrix
	m2.SetSRTMatrix(&s,&a,&act_pos);

	matrix->MultMat(&m1,&m2);
	matrix->InvMat(invmatrix);
	rotmatrix->SetRotMatrix(&a);
}

/*************
 * DESCRIPTION:   determines size at time
 * INPUT:         size        size
 *                time        actual time
 * OUTPUT:        none
 *************/
void ACTOR::FindSize(VECTOR *size, const float time)
{
	DIMENSION *dim,*dim1;
	float delta;

	// search size at end of intervall
	dim = (DIMENSION*)this->size->FindAction(time);
	// found a action -> take this size
	*size = dim->size;
	// outside of intervall -> directly take this size
	if(time < dim->time.end)
	{
		// inside intervall -> interpolate to right time
		// search size before
		dim1 = (DIMENSION*)this->size->FindActionBefore(time);
		// calculate delta
		VecSub(size,&dim1->size,size);
		if(dim->flags & ACTION_LINEAR)
		{
			delta = (time - dim->time.begin)/(dim->time.end - dim->time.begin);
			VecScale(delta,size,size);
			// add absolute size
			VecAdd(size,&dim1->size,size);
		}
	}
}

/*************
 * DESCRIPTION:   determines alignment at time
 * INPUT:         align       alignment
 *                time        actual time
 * OUTPUT:        none
 *************/
void ACTOR::FindAlignment(VECTOR *align, const float time)
{
	ALIGNMENT *alignm,*alignm1;
	float delta;

	// search alignment at end of intervall
	alignm = (ALIGNMENT*)this->align->FindAction(time);
	// found a action -> take this alignment
	*align = alignm->align;
	// outside of intervall -> directly take this alignment
	if(time < alignm->time.end)
	{
		// inside intervall -> interpolate to right time
		// search alignment before
		alignm1 = (ALIGNMENT*)this->align->FindActionBefore(time);
		// calculate delta
		VecSub(align,&alignm1->align,align);
		if(alignm->flags & ACTION_LINEAR)
		{
			delta = (time - alignm->time.begin)/(alignm->time.end - alignm->time.begin);
			VecScale(delta,align,align);
			// add absolute alignment
			VecAdd(align,&alignm1->align,align);
		}
	}
}

/*************
 * DESCRIPTION:   determines position at time
 * INPUT:         pos         position
 *                time        time
 * OUTPUT:        none
 *************/
void ACTOR::FindPosition(VECTOR *pos, const float time)
{
	POS *posi,*posi1;
	float delta;

	// search position at end of intervall
	posi = (POS*)this->pos->FindAction(time);
	// found a action -> take this position
	*pos = posi->pos;
	// outside of intervall -> directly take this position
	if(time < posi->time.end)
	{
		// inside intervall -> interpolate to right time
		// search position before
		posi1 = (POS*)this->pos->FindActionBefore(time);
		// calculate delta
		VecSub(pos,&posi1->pos,pos);
		if(posi->flags & ACTION_LINEAR)
		{
			delta = (time - posi->time.begin)/(posi->time.end - posi->time.begin);
			VecScale(delta,pos,pos);
			// add absolute position
			VecAdd(pos,&posi1->pos,pos);
		}
	}
}

/*************
 * DESCRIPTION:   Inserts a position in the position list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void ACTOR::AddPos(POS *pos)
{
	pos->LIST::Insert((LIST**)&this->pos);
}

/*************
 * DESCRIPTION:   Inserts a alignment in the alignment list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void ACTOR::AddAlign(ALIGNMENT *align)
{
	align->LIST::Insert((LIST**)&this->align);
}

/*************
 * DESCRIPTION:   Inserts a size in the size list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void ACTOR::AddSize(DIMENSION *size)
{
	size->LIST::Insert((LIST**)&this->size);
}

/*************
 * DESCRIPTION:   Inserts a precalculated size in the size list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void ACTOR::AddPreMatrix(PRE_MATRIX *matrix)
{
	matrix->LIST::Insert((LIST**)&this->pre_mat);
}

/*************
 * DESCRIPTION:   Frees all positions
 * INPUT:         none
 * OUTPUT:        none
 *************/
void POS::FreeAll()
{
	POS *next,*prev;

	prev = this;
	do
	{
		next = (POS*)prev->GetNext();
		delete prev;
		prev = next;
	}
	while(next);
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
ACTION::ACTION()
{
	time.begin = 0.f;
	time.end = 0.f;
	flags = ACTION_LINEAR;
}

/*************
 * DESCRIPTION:   find action at time code 'time'
 * INPUT:         time        actual time code
 * OUTPUT:        pointer to action, NULL if non found
 *************/
ACTION *ACTION::FindAction(const float time)
{
	ACTION *act,*nearer=NULL;

	act = this;
	while(act)
	{
		if((time >= act->time.begin) && (time <= act->time.end))
		{
			// found right intervall
			return act;
		}
		if(time >= act->time.end)
		{
			// found a action before; look if we've found a nearer before
			if(nearer)
			{
				if(nearer->time.end < act->time.end)
					nearer = act;
			}
			else
				nearer = act;
		}
		act = (ACTION*)act->LIST::GetNext();
	}
	return nearer;
}

/*************
 * DESCRIPTION:   find action before time code 'time'
 * INPUT:         time        time code
 * OUTPUT:        pointer to action, NULL if non found
 *************/
ACTION *ACTION::FindActionBefore(const float time)
{
	ACTION *act,*nearer=NULL;

	act = this;
	while(act)
	{
		if(time >= act->time.end)
		{
			// found a action before; look if we've found a nearer than before
			if(nearer)
			{
				if(nearer->time.end < act->time.end)
					nearer = act;
			}
			else
				nearer = act;
		}
		act = (ACTION*)act->LIST::GetNext();
	}
	return nearer;
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
POS::POS()
{
	SetVector(&pos, 0.f, 0.f, 0.f);
}

/*************
 * DESCRIPTION:   Frees all alignments
 * INPUT:         none
 * OUTPUT:        none
 *************/
void ALIGNMENT::FreeAll()
{
	ALIGNMENT *next,*prev;

	prev = this;
	do
	{
		next = (ALIGNMENT*)prev->GetNext();
		delete prev;
		prev = next;
	}
	while(next);
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
ALIGNMENT::ALIGNMENT()
{
	SetVector(&align, 0.f, 0.f, 0.f);
}

/*************
 * DESCRIPTION:   Frees all sizes
 * INPUT:         none
 * OUTPUT:        none
 *************/
void DIMENSION::FreeAll()
{
	DIMENSION *next,*prev;

	prev = this;
	do
	{
		next = (DIMENSION*)prev->GetNext();
		delete prev;
		prev = next;
	}
	while(next);
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
DIMENSION::DIMENSION()
{
	SetVector(&size, 0.f, 0.f, 0.f);
}

/*************
 * DESCRIPTION:   Calculate time for motion blur
 * INPUT:         sample      sample number
 *                world       pointer to world structure
 * OUTPUT:        time
 *************/
float CalcTime(int sample, const WORLD *world)
{
	if(world->motionblur_samples > 1)
	{
		if(world->flags & WORLD_RANDOM_JITTER)
			return world->time + ((float(sample)+float(rand())/RAND_MAX)/world->motionblur_samples) * world->timelength;
		else
			return world->time + (float(sample)/(world->motionblur_samples-1)) * world->timelength;
	}
	else
		return world->time;
}

/*************
 * DESCRIPTION:   Frees all precalculated matrices
 * INPUT:         none
 * OUTPUT:        none
 *************/
void PRE_MATRIX::FreeAll()
{
	PRE_MATRIX *next,*prev;

	prev = this;
	do
	{
		next = (PRE_MATRIX*)prev->GetNext();
		delete prev;
		prev = next;
	}
	while(next);
}

/*************
 * DESCRIPTION:   Find matrix for given time intervall
 * INPUT:         matrix      found matrix
 *                invmatrix   inverted matrix
 *                rotmatrix   rotation matrix
 *                time        actual time intervall
 * OUTPUT:        -
 *************/
void PRE_MATRIX::FindMatrix(MATRIX **matrix, MATRIX **invmatrix, MATRIX **rotmatrix, const TIME *time)
{
	PRE_MATRIX *cur;

	*matrix = NULL;
	*rotmatrix = NULL;

	cur = this;
	while(cur)
	{
		if((cur->time.begin == time->begin) && (cur->time.end == time->end))
		{
			*matrix = &cur->matrix;
			*invmatrix = &cur->invmatrix;
			*rotmatrix = &cur->rotmatrix;
			return;
		}
		cur = (PRE_MATRIX*)cur->GetNext();
	}
	return;
}
