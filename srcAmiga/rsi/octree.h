/***************
 * MODUL:         octree
 * NAME:          octree.h
 * DESCRIPTION:   octree definitions
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * BUGS:          none
 * TO DO:         all
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    07.03.95 ah    IntersectionWithOctree() now uses two
 *       seperate lists for limited and unlimited objects
 *    11.07.95 ah    added DisableObject() 
 *    13.12.96 ah    added minobjects to InitOctree()
 *    22.02.98 ah    moved the direction member of the OCTREE structure to the end of to
 *                   make the othere members long word aligned which speeds up access
 ***************/

#ifndef OCTREE_H
#define OCTREE_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

#define MAXOCTREEDEPTH 10  // Maximum Treedepth

typedef struct tCONTENT // content
{
	struct   tCONTENT *next;
	OBJECT   *object;
} CONTENT;

typedef struct tOCTREE     // octree
{
	struct   tOCTREE *parent;
	VOXEL    voxel;
	CONTENT  *content;
	struct   tOCTREE *child[8];
	WORD     direction;
} OCTREE;

// voxel determination methods
enum
{
	OCTREE_START, OCTREE_LINE, OCTREE_MULTI
};

// state of intersections
enum
{
	OCTREE_NONE, OCTREE_TEST, OCTREE_TESTMB
};

OBJECT* IntersectionWithOctree(RSICONTEXT*, RAY *, int *, int);
void InitOctree(RSICONTEXT*, const WORD, const ULONG);
void TermOctree(OCTREE**);

#endif // OCTREE_H
