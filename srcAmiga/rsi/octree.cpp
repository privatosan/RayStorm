/***************
 * MODUL:         octree
 * NAME:          octree.cpp
 * DESCRIPTION:   This module includes all functions for creating, testing
 *    and destructing a octree
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    07.03.95 ah    IntersectionWithOctree() now uses two
 *       seperate lists for limited and unlimited objects
 *    12.03.95 ah    bugfix if IntersectionWithOctree()
 *    11.07.95 ah    added DisableObject()
 *    08.09.95 ah    replace VoxelIntersect() by direct voxel test
 *    22.10.95 ah    added tested_object to IntersectionWithOctree()
 *    12.01.96 ah    optimized determination of startvoxel
 ***************/

#include <math.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef ERROR_H
#include "error.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

#ifndef VOXEL_H
#include "voxel.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef OCTREE_H
#include "octree.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

#define  HASCHILDREN    0x8000

#define  BACK           0x0000
#define  FRONT          0x0001
#define  DOWN           0x0000
#define  UP             0x0002
#define  LEFT           0x0000
#define  RIGHT          0x0004
#define  BACKFRONT      0x0001
#define  DOWNUP         0x0002
#define  LEFTRIGHT      0x0004
#define  DIRECTIONMASK  0x0007
#if defined __AMIGA__ || __unix__
#define  NODIRECTION    -1
#else
#define  NODIRECTION    0xFFFF
#endif
#define  LEFTDOWNBACK   (LEFT|DOWN|BACK)
#define  LEFTDOWNFRONT  (LEFT|DOWN|FRONT)
#define  LEFTUPBACK     (LEFT|UP|BACK)
#define  LEFTUPFRONT    (LEFT|UP|FRONT)
#define  RIGHTDOWNBACK  (RIGHT|DOWN|BACK)
#define  RIGHTDOWNFRONT (RIGHT|DOWN|FRONT)
#define  RIGHTUPBACK    (RIGHT|UP|BACK)
#define  RIGHTUPFRONT   (RIGHT|UP|FRONT)
#define  ISBACK(x)      (((x) & BACKFRONT) == BACK)
#define  ISFRONT(x)     (((x) & BACKFRONT) == FRONT)
#define  ISDOWN(x)      (((x) & DOWNUP) == DOWN)
#define  ISUP(x)        (((x) & DOWNUP) == UP)
#define  ISLEFT(x)      (((x) & LEFTRIGHT) == LEFT)
#define  ISRIGHT(x)     (((x) & LEFTRIGHT) == RIGHT)

/*************
 * DESCRIPTION:   determine minimum and maximum voxel
 * INPUT:         header   pointer to header of objectlist
 *                voxel    pointer to voxel
 * OUTPUT:        none
 *************/
static void MinimizeMaximizeObject(OBJECT *header, VOXEL *voxel)
{
	OBJECT *help;

	help = header;
	while(help)
	{
		UnionVoxel(voxel, voxel, &help->voxel);
		help = (OBJECT*)help->GetNext();
	}
	if(fabs(voxel->max.x-voxel->min.x) < 1e-3f)
	{
		voxel->max.x += 1e-3f;
		voxel->min.x -= 1e-3f;
	}
	if(fabs(voxel->max.y-voxel->min.y) < 1e-3f)
	{
		voxel->max.y += 1e-3f;
		voxel->min.y -= 1e-3f;
	}
	if(fabs(voxel->max.z-voxel->min.z) < 1e-3f)
	{
		voxel->max.z += 1e-3f;
		voxel->min.z -= 1e-3f;
	}
}

/*************
 * DESCRIPTION:   get memory for CONTENT and insert in CONTENT-list
 * INPUT:         next     pointer to next CONTENT
 *                object   pointer to OBJECT
 * OUTPUT:        pointer to CONTENT-list; NULL if failed
 *************/
static CONTENT *AllocateContent(CONTENT *next, OBJECT *object)
{
	CONTENT *content;

	content = new CONTENT;
	if(content)
	{
		content->next = next;
		content->object = object;
	}
	return(content);
}

/*************
 * DESCRIPTION:   free a CONTENT-list
 * INPUT:         content  pointer to CONTENT-list
 * OUTPUT:        none
 *************/
static void FreeContentList(CONTENT *content)
{
	CONTENT *help, *next;

	help = content;
	while(help)
	{
		next = help->next;
		delete help;
		help = next;
	}
}

/*************
 * DESCRIPTION:   allocate memory for whole CONTENT-list
 * INPUT:         object   pointer to OBJECT-list
 * OUTPUT:        pointer to CONTENT-list, NULL if failed
 *************/
static CONTENT *AllocateContentList(OBJECT *object)
{
	OBJECT *help;
	CONTENT *content, *old;

	content = NULL;
	help = object;
	while(help)
	{
		help->identification = 0;
		old = content;
		content= (CONTENT*)AllocateContent(old, help);
		if(content)
		{
			help = (OBJECT*)help->GetNext();
		}
		else
		{
			FreeContentList(old);
			help = NULL;
		}
	}
	return(content);
}

/*************
 * DESCRIPTION:   get memory for OCTREE-object, initialize and insert in list
 * INPUT:         parent      pointer to parent OCTREE-object
 *                direction   direction of OCTREE-object
 *                voxel       pointer to VOXEL
 * OUTPUT:        pointer to OCTREE-list; NULL if failed
 *************/
static OCTREE *AllocateOctree(OCTREE *parent, const WORD direction, const VOXEL *voxel)
{
	OCTREE *octree;

	octree = new OCTREE;
	if(octree)
	{
		octree->parent = parent;
		octree->direction = direction;
		octree->voxel = *voxel;
		octree->content = NULL;
		octree->child[LEFTDOWNBACK]=
		octree->child[LEFTDOWNFRONT]=
		octree->child[LEFTUPBACK]=
		octree->child[LEFTUPFRONT]=
		octree->child[RIGHTDOWNBACK]=
		octree->child[RIGHTDOWNFRONT]=
		octree->child[RIGHTUPBACK]=
		octree->child[RIGHTUPFRONT]= NULL;
	}
	return(octree);
}

/*************
 * DESCRIPTION:   free OCTREE-object and all children (recursiv)
 * INPUT:         octree   pointer to OCTREE-object
 * OUTPUT:        none
 *************/
static void FreeOctree(OCTREE *octree)
{
	if(octree)
	{
		FreeOctree(octree->child[LEFTDOWNBACK]);
		FreeOctree(octree->child[LEFTDOWNFRONT]);
		FreeOctree(octree->child[LEFTUPBACK]);
		FreeOctree(octree->child[LEFTUPFRONT]);
		FreeOctree(octree->child[RIGHTDOWNBACK]);
		FreeOctree(octree->child[RIGHTDOWNFRONT]);
		FreeOctree(octree->child[RIGHTUPBACK]);
		FreeOctree(octree->child[RIGHTUPFRONT]);
		FreeContentList(octree->content);
		delete octree;
	}
}

/*************
 * DESCRIPTION:   divide OCTREE-object in eight children:
 *                - if depth < maxdepth
 *                - if at least one object is in actual OCTREE-object
 * INPUT:         octree            pointer to OCTREE-object
 *                depth             current depth of octree
 *                maxOctreeDepth    maximum depth of octree
 * OUTPUT:        none
 *************/
static void DivideOctree(OCTREE *octree, const WORD depth, WORD maxOctreeDepth)
{
	WORD direction;
	VECTOR mid;
	VOXEL voxel, newvoxel;
	OBJECT *object;
	CONTENT *content, *help, *old;

	content = octree->content;
	if(!content)
		return;
	if(!content->next)
		return;

	if(depth < maxOctreeDepth)
	{
		voxel = octree->voxel;
		mid.x = 0.5f * (voxel.min.x+voxel.max.x);
		mid.y = 0.5f * (voxel.min.y+voxel.max.y);
		mid.z = 0.5f * (voxel.min.z+voxel.max.z);
		for(direction = LEFTDOWNBACK; direction <= RIGHTUPFRONT; direction++)
		{
			newvoxel.min.x = ISLEFT(direction) ? voxel.min.x : mid.x;
			newvoxel.min.y = ISDOWN(direction) ? voxel.min.y : mid.y;
			newvoxel.min.z = ISBACK(direction) ? voxel.min.z : mid.z;
			newvoxel.max.x = ISLEFT(direction) ? mid.x : voxel.max.x;
			newvoxel.max.y = ISDOWN(direction) ? mid.y : voxel.max.y;
			newvoxel.max.z = ISBACK(direction) ? mid.z : voxel.max.z;
			octree->child[direction] =
				AllocateOctree(octree, direction, &newvoxel);
			if(octree->child[direction])
			{
				content = NULL;
				help = octree->content;
				while(help)
				{
					object = help->object;
					if(!((object->voxel.min.x > newvoxel.max.x) ||
						(object->voxel.max.x < newvoxel.min.x) ||
						(object->voxel.min.y > newvoxel.max.y) ||
						(object->voxel.max.y < newvoxel.min.y) ||
						(object->voxel.min.z > newvoxel.max.z) ||
						(object->voxel.max.z < newvoxel.min.z)))
					{
						old = content;
						content = AllocateContent(old, object);
						if(content)
						{
							help = help->next;
						}
						else
						{
							FreeContentList(old);
							help = NULL;
						}
					}
					else
					{
						help = help->next;
					}
				}
				octree->child[direction]->content = content;
				DivideOctree(octree->child[direction], depth+1, maxOctreeDepth);
			}
		}
		octree->direction |= HASCHILDREN;
		FreeContentList(octree->content);
		octree->content = NULL;
	}
}

/*************
 * DESCRIPTION:   mainfunction: test if ray intersects with object
 * INPUT:         ray            pointer to RAY-structure
 *                state          OCTREE_NONE  if no test was performed
 *    OCTREE_TEST   if a object intersection test was performed
 *    OCTREE_TESTMB if a intersection with a object with actor was performed
 *                mode           startvoxel determination mode
 * OUTPUT:        NULL no intersection, else pointer to object
 *************/
OBJECT *IntersectionWithOctree(RSICONTEXT *rc, RAY *ray, int *state, int mode)
{
	WORD depth, runDepth, oldDepth;
	WORD direction, mainDirection, currentDirection, currentDirectionMask;
	WORD directions[MAXOCTREEDEPTH+1];
	BOOL found;
	BOOL xRayNotZero, yRayNotZero, zRayNotZero;
	float xNextPlane, yNextPlane, zNextPlane, lambda;
	float xInvRay, yInvRay, zInvRay;
	float xDistance[MAXOCTREEDEPTH+1];
	float yDistance[MAXOCTREEDEPTH+1];
	float zDistance[MAXOCTREEDEPTH+1];
	VECTOR start, location;
	OBJECT *help, *intersect;
	CONTENT *content;
	OCTREE *octree;

	STATISTIC(STAT_OCTREETEST);

	*state = OCTREE_NONE;
	// next ray
	rc->identification++;
	intersect = NULL;
	ray->lambda = INFINITY;

	// first calculate unlimited objects
	help = rc->obj_unlimited_root;
	while(help != (OBJECT *)NULL)
	{
		if(help->identification != rc->identification)
		{
			if(help->actor)
				*state = OCTREE_TESTMB;
			else
				if(*state != OCTREE_TESTMB)
					*state = OCTREE_TEST;
			if(help->actor && help->time != ray->time)
			{
				help->Update(ray->time);
				help->time = ray->time;
			}
			if(help->RayIntersect(ray))
			{
				intersect = ray->obj;
			}
			help->identification = rc->identification;
		}
		help = (OBJECT*)help->GetNext();
	}

	// there are too less objects in the scene -> return
	if(!rc->octreeRoot)
	{
		help = rc->obj_root;
		while(help != (OBJECT *)NULL)
		{
			if(help->identification != rc->identification)
			{
				if(help->actor)
					*state = OCTREE_TESTMB;
				else
					if(*state != OCTREE_TESTMB)
						*state = OCTREE_TEST;
				if(help->actor && help->time != ray->time)
				{
					help->Update(ray->time);
					help->time = ray->time;
				}
				if(help->RayIntersect(ray))
				{
					intersect = ray->obj;
				}
				help->identification = rc->identification;
			}
			help = (OBJECT*)help->GetNext();
		}
		return intersect;
	}

	// initialize
	octree = rc->octreeRoot;
	depth = 0;
	mainDirection = ((ray->dir.x < 0.f) ? LEFT : RIGHT)
					  | ((ray->dir.y < 0.f) ? DOWN : UP)
					  | ((ray->dir.z < 0.f) ? BACK : FRONT);
	if(PointInVoxel(&ray->start, &octree->voxel))
	{ // start is in root`s voxel
		start = ray->start;
	}
	else
	{
		if(RayVoxelIntersection(&lambda, &ray->start, &ray->dir, &octree->voxel))
		{ // ray hits root`s voxel
			lambda += 7e-4f;
			start.x = ray->start.x + lambda*ray->dir.x;
			start.y = ray->start.y + lambda*ray->dir.y;
			start.z = ray->start.z + lambda*ray->dir.z;
		}
		else
		{ // ray miss root`s voxel
			return(intersect);
		}
	}
	// find the start voxel
	// look if point is in startvoxel, if so use this voxel else search
	// start voxel
	// then use two strategies:
	// - line mode: take end voxel as startvoxel for new test
	// - multi mode: don't change startvoxel
	if((mode == OCTREE_START) && rc->rootoctree)
	{
		if(PointInVoxel(&start, &rc->rootoctree->voxel))
		{
			octree = rc->rootoctree;
			depth = rc->rootdepth;
		}
		else
		{
			rc->rootoctree = NULL;
		}
	}
	else
	{
		if(rc->startoctree)
		{
			if(PointInVoxel(&start, &rc->startoctree->voxel))
			{
				octree = rc->startoctree;
				depth = rc->startdepth;
			}
		}
	}
	if(!depth)
	{
		// search start voxel
		while(octree->direction & HASCHILDREN)
		{
			currentDirection = NODIRECTION;
			direction = LEFTDOWNBACK;
			do
			{
				if(PointInVoxel(&start, &octree->child[direction]->voxel))
				{
					currentDirection = direction;
				}
				direction++;
			}
			while((currentDirection == NODIRECTION) && (direction <= RIGHTUPFRONT));

			// with float (single) there can be problems if the EPSILON
			// added to lamda is too low. Add this lines to catches the error
			if(currentDirection == NODIRECTION)
				return intersect;
			octree = octree->child[currentDirection];
			depth++;
		}
		if(!rc->rootoctree)
		{
			rc->rootoctree = octree;
			rc->rootdepth = depth;
		}
		rc->startoctree = octree;
		rc->startdepth = depth;
	}
	// precalculating
	xRayNotZero = (ray->dir.x > 0.f ? ray->dir.x : - ray->dir.x) > EPSILON;
	if(xRayNotZero)
	{
		xInvRay = 1.f/ray->dir.x;
	}
	yRayNotZero = (ray->dir.y > 0.f ? ray->dir.y : - ray->dir.y) > EPSILON;
	if(yRayNotZero)
	{
		yInvRay = 1.f/ray->dir.y;
	}
	zRayNotZero = (ray->dir.z > 0.f ? ray->dir.z : - ray->dir.z) > EPSILON;
	if(zRayNotZero)
	{
		zInvRay = 1.f/ray->dir.z;
	}
	for(runDepth = 0; runDepth <= rc->maxOctreeDepth; runDepth++)
	{
		xDistance[runDepth] = (xRayNotZero ? (float)fabs(rc->xLength[runDepth]*xInvRay) : INFINITY);
		yDistance[runDepth] = (yRayNotZero ? (float)fabs(rc->yLength[runDepth]*yInvRay) : INFINITY);
		zDistance[runDepth] = (zRayNotZero ? (float)fabs(rc->zLength[runDepth]*zInvRay) : INFINITY);
	}
	if(ray->dir.x < 0.f)
	{
		xNextPlane = (xRayNotZero ? (octree->voxel.min.x-start.x)*xInvRay : INFINITY);
	}
	else
	{
		xNextPlane = (xRayNotZero ? (octree->voxel.max.x-start.x)*xInvRay : INFINITY);
	}
	if(ray->dir.y < 0.f)
	{
		yNextPlane = (yRayNotZero ? (octree->voxel.min.y-start.y)*yInvRay : INFINITY);
	}
	else
	{
		yNextPlane = (yRayNotZero ? (octree->voxel.max.y-start.y)*yInvRay : INFINITY);
	}
	if(ray->dir.z < 0.0)
	{
		zNextPlane = (zRayNotZero ? (octree->voxel.min.z-start.z)*zInvRay : INFINITY);
	}
	else
	{
		zNextPlane = (zRayNotZero ? (octree->voxel.max.z-start.z)*zInvRay : INFINITY);
	}
	while(octree != (OCTREE *)NULL)
	{ // find intersections
		content = octree->content;
		while(content != (CONTENT *)NULL)
		{
			help = content->object;
			if(help->identification != rc->identification)
			{
				if(RayVoxelIntersection(&lambda, &ray->start, &ray->dir, &help->voxel))
				{
					if(help->actor)
						*state = OCTREE_TESTMB;
					else
						if(*state != OCTREE_TESTMB)
							*state = OCTREE_TEST;
					if(help->actor && help->time != ray->time)
					{
						help->Update(ray->time);
						help->time = ray->time;
					}
					if(help->RayIntersect(ray))
					{
						intersect = ray->obj;
					}
				}
				help->identification = rc->identification;
			}
			content = content->next;
		}
		if(intersect)
		{
			location.x = ray->start.x + ray->lambda*ray->dir.x;
			location.y = ray->start.y + ray->lambda*ray->dir.y;
			location.z = ray->start.z + ray->lambda*ray->dir.z;
		}
		if(intersect && PointInVoxel(&location, &octree->voxel))
		{
			// found the nearest intersection
			if(mode == OCTREE_LINE)
			{
				rc->startoctree = octree;
				rc->startdepth = depth;
			}
			return intersect;
		}
		else
		{
			// raygenerator
			if((xNextPlane < yNextPlane) && (xNextPlane < zNextPlane))
			{
				currentDirectionMask = LEFTRIGHT;
				yNextPlane -= xNextPlane;
				zNextPlane -= xNextPlane;
				xNextPlane = xDistance[depth];
			}
			else
			{
				if(yNextPlane < zNextPlane)
				{
					currentDirectionMask = DOWNUP;
					xNextPlane -= yNextPlane;
					zNextPlane -= yNextPlane;
					yNextPlane = yDistance[depth];
				}
				else
				{
					currentDirectionMask = BACKFRONT;
					xNextPlane -= zNextPlane;
					yNextPlane -= zNextPlane;
					zNextPlane = zDistance[depth];
				}
			}
			oldDepth = depth;    // go up
			do
			{
				if((currentDirectionMask == LEFTRIGHT) ||
				  ((octree->direction & LEFTRIGHT) !=
				  (mainDirection & LEFTRIGHT)))
				{
					xNextPlane += xDistance[depth];
				}
				if((currentDirectionMask == DOWNUP) ||
				  ((octree->direction & DOWNUP) !=
				  (mainDirection & DOWNUP)))
				{
					yNextPlane += yDistance[depth];
				}
				if((currentDirectionMask == BACKFRONT) ||
				  ((octree->direction & BACKFRONT) !=
				  (mainDirection & BACKFRONT)))
				{
					zNextPlane += zDistance[depth];
				}
				found = ((octree->direction != NODIRECTION) ?
					((octree->direction & currentDirectionMask) !=
					(mainDirection & currentDirectionMask)) : FALSE);
				directions[depth--] = octree->direction & DIRECTIONMASK;
				octree = octree->parent;
			}
			while((octree != (OCTREE *)NULL) && !found);
			if(found)
			{  // go down
				while((octree->direction & HASCHILDREN) && (depth < oldDepth))
				{
					depth++;
					octree=octree->child[directions[depth]^currentDirectionMask];
					if((currentDirectionMask == LEFTRIGHT) ||
						(octree->direction & LEFTRIGHT) != (mainDirection & LEFTRIGHT))
					{
						xNextPlane -= xDistance[depth];
					}
					if((currentDirectionMask == DOWNUP) ||
						(octree->direction & DOWNUP) != (mainDirection & DOWNUP))
					{
						yNextPlane -= yDistance[depth];
					}
					if((currentDirectionMask == BACKFRONT) ||
						(octree->direction & BACKFRONT) != (mainDirection & BACKFRONT))
					{
						zNextPlane -= zDistance[depth];
					}
				}
				if(depth == oldDepth)
				{  /* smaller neighbour ? */
					while(octree->direction & HASCHILDREN)
					{
						depth++;
						direction = 0;
						if((currentDirectionMask == LEFTRIGHT) ||
							(xNextPlane > xDistance[depth]))
						{
							direction |= ((mainDirection & LEFTRIGHT) ^ LEFTRIGHT);
						}
						else
						{
							direction |= (mainDirection & LEFTRIGHT);
						}
						if((currentDirectionMask == DOWNUP) ||
							(yNextPlane > yDistance[depth]))
						{
							direction |= ((mainDirection & DOWNUP) ^ DOWNUP);
						}
						else
						{
							direction |= (mainDirection & DOWNUP);
						}
						if((currentDirectionMask == BACKFRONT) ||
							(zNextPlane > zDistance[depth]))
						{
							direction |= ((mainDirection & BACKFRONT) ^ BACKFRONT);
						}
						else
						{
							direction |= (mainDirection & BACKFRONT);
						}
						octree = octree->child[direction];
						if((currentDirectionMask == LEFTRIGHT) ||
							((octree->direction & LEFTRIGHT) !=
							(mainDirection & LEFTRIGHT)))
						{
							xNextPlane -= xDistance[depth];
						}
						if((currentDirectionMask == DOWNUP) ||
							((octree->direction & DOWNUP) !=
							(mainDirection & DOWNUP)))
						{
							yNextPlane -= yDistance[depth];
						}
						if((currentDirectionMask == BACKFRONT) ||
							((octree->direction & BACKFRONT) !=
							(mainDirection & BACKFRONT)))
						{
							zNextPlane -= zDistance[depth];
						}
					}
				}
			}
		}
	}
	return(intersect);
}

/*************
 * DESCRIPTION:   initialze octree
 * INPUT:         maxDepth    maximum depth of ocrtree
 *                minoobjects minimum objects for octree prevention
 * OUTPUT:        none
 *************/
void InitOctree(RSICONTEXT *rc, const WORD maxDepth, const ULONG minobjects)
{
	WORD depth;
	VOXEL voxel;
	OBJECT *obj;
	ULONG count;

	// free previously created octree
	TermOctree(&rc->octreeRoot);

	// if there are too less objects in scene don't generate the octree
	obj = rc->obj_root;
	count = 0;
	while(obj && (count <= minobjects))
	{
		count++;
		obj = (OBJECT*)obj->GetNext();
	}

	if(count <= minobjects)
		return;

	rc->maxOctreeDepth = MIN(maxDepth, MAXOCTREEDEPTH);
	voxel.min.x = voxel.min.y = voxel.min.z = INFINITY;
	voxel.max.x = voxel.max.y = voxel.max.z = -INFINITY;
	MinimizeMaximizeObject(rc->obj_root, &voxel);
	rc->octreeRoot = AllocateOctree((OCTREE *)NULL, NODIRECTION, &voxel);
	if(rc->octreeRoot != (OCTREE *)NULL)
	{
		rc->octreeRoot->content = AllocateContentList(rc->obj_root);
		DivideOctree(rc->octreeRoot, 0, rc->maxOctreeDepth);
		/* precalculating */
		rc->xLength[0] = voxel.max.x-voxel.min.x;
		rc->yLength[0] = voxel.max.y-voxel.min.y;
		rc->zLength[0] = voxel.max.z-voxel.min.z;
		for(depth=1; depth <= rc->maxOctreeDepth; depth++)
		{
			rc->xLength[depth] = 0.5f*rc->xLength[depth-1];
			rc->yLength[depth] = 0.5f*rc->yLength[depth-1];
			rc->zLength[depth] = 0.5f*rc->zLength[depth-1];
		}
		rc->identification = 0;
	}
}

/*************
 * DESCRIPTION:   free the allocated octree
 * INPUT:         octreeRoot
 * OUTPUT:        none
 *************/
void TermOctree(OCTREE **octreeRoot)
{
	if(*octreeRoot)
	{
		FreeOctree(*octreeRoot);
		*octreeRoot = NULL;
	}
}

