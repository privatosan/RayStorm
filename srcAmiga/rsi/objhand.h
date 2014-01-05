/***************
 * MODUL:         Object handler
 * NAME:          objhand.h
 * DESCRIPTION:   In this module are all the definitions to read
 *    object files
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.01.96 ah    initial release
 *    18.04.96 ah    added surf to ReadObject()
 ***************/

#ifndef OBJHAND_H
#define OBJHAND_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef ACTOR_H
#include "actor.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

class CSG;
class RSICONTEXT;

/* this class is used to make a connection between
	the name of an surface and the surface itself.
	It is destroyed after the object is loaded */
class SURF_NAME : public LIST
{
	public:
		char *name;
		SURFACE *surf;

		SURF_NAME() { name = NULL; };
		~SURF_NAME() { if(name) delete name; };
};

/* this class is used to handle the hierarchie of
	CSG objects */
class HIERARCHIE : public LIST
{
	public:
		HIERARCHIE *prev;
		CSG *node;

		HIERARCHIE() { node = NULL; };
};

int ReadObject(RSICONTEXT*, char* , VECTOR *, VECTOR *, VECTOR *, VECTOR *,
	VECTOR *,ACTOR *,SURFACE *);

#endif // OBJHAND_H
