/***************
 * MODUL:         context
 * NAME:          context.cpp
 * DESCRIPTION:   Context class member functions
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    14.12.97 ah    initial release
 ***************/

#include <string.h>

#ifndef CONTEXT_H
#include "context.h"
#endif

#ifndef TRIANGLE_H
#include "triangle.h"
#endif

#ifndef NOISE_H
#include "noise.h"
#endif

// callback functions
void DefCooperate(RSICONTEXT*) {}
BOOL DefCheckCancel(RSICONTEXT*) { return FALSE; }
void DefLog(RSICONTEXT*, char *) {}
void DefUpdateStatus(RSICONTEXT*, float, float, int, int, SMALL_COLOR*) {}

/*************
 * DESCRIPTION:   constructor
 * INPUT:         -
 * OUTPUT:        -
 *************/
RSICONTEXT::RSICONTEXT()
{
	scrarray = NULL;
	obj_root = NULL;
	obj_unlimited_root = NULL;
	surface_enter = TRUE;
	surfname_root = NULL;
	light_root = NULL;
	actor_root = NULL;
	image_root = NULL;
	image_error = NULL;
	objhand_error = NULL;
	cur_hier = NULL;
	cur_csg = NULL;
	cur_node = NULL;
	octreeRoot = NULL;
	bOctreedone = FALSE;
	startoctree = NULL;
	rootoctree = NULL;
	surf_root = NULL;
	text_root = NULL;

	Cooperate = DefCooperate;
	CheckCancel = DefCheckCancel;
	Log = DefLog;
	UpdateStatus = DefUpdateStatus;

	// init hypertexture noise functions
	InitNoise(&hashTable);
}

/*************
 * DESCRIPTION:   destructor
 * INPUT:         -
 * OUTPUT:        -
 *************/
RSICONTEXT::~RSICONTEXT()
{
	int i;

	for(i=0; i<MAXRAYDEPTH+1; i++)
	{
		surfaces[i].brush = NULL;
		surfaces[i].texture = NULL;
		surfaces[i].hypertexture = NULL;
	}
	surface.brush = NULL;
	surface.texture = NULL;
	surface.hypertexture = NULL;

	Cleanup();

	// cleanup hypertexture noise functions
	CleanupNoise(hashTable);
}

/*************
 * DESCRIPTION:   Free all lists, buffers and all the stuff
 * INPUT:         -
 * OUTPUT:        -
 *************/
void RSICONTEXT::Cleanup()
{
	if(bOctreedone)
	{
		TermOctree(&octreeRoot);
		bOctreedone = FALSE;
	}
	if(obj_root)
	{
		FreeObjects();
		obj_root = NULL;
	}
	if(obj_unlimited_root)
	{
		obj_unlimited_root->FreeList();
		obj_unlimited_root = NULL;
	}
	if(light_root)
	{
		light_root->FreeList();
		light_root = NULL;
	}
	if(actor_root)
	{
		actor_root->FreeList();
		actor_root = NULL;
	}
	if(image_root)
	{
		IMAGE *next, *cur;

		cur = image_root;
		while(cur)
		{
			next = (IMAGE*)cur->GetNext();
			delete cur;
			cur = next;
		}
//      image_root->FreeList();
		image_root = NULL;
	}
	if(surfname_root)
	{
		surfname_root->FreeList();
		surfname_root = NULL;
	}
	if(surf_root)
	{
		surf_root->FreeList();
		surf_root = NULL;
	}
	if(text_root)
	{
		text_root->FreeList();
		text_root = NULL;
	}
	if(cur_hier)
	{
		cur_hier->FreeList();
		cur_hier = NULL;
	}
	if(scrarray)
	{
		delete [ ] scrarray;
		scrarray = NULL;
	}
	if(image_error)
	{
		delete image_error;
		image_error = NULL;
	}
	if(objhand_error)
	{
		delete objhand_error;
		objhand_error = NULL;
	}

	camera.ToDefaults();
	world.ToDefaults();
}

/*************
 * DESCRIPTION:   Set the cooperate call back
 * INPUT:         -
 * OUTPUT:        -
 *************/
void RSICONTEXT::SetCooperateCB(void (*cb)(RSICONTEXT*))
{
	if(!cb)
		Cooperate = DefCooperate;
	else
		Cooperate = cb;
}

/*************
 * DESCRIPTION:   Set the check cancel call back
 * INPUT:         -
 * OUTPUT:        -
 *************/
void RSICONTEXT::SetCheckCancelCB(BOOL (*cb)(RSICONTEXT*))
{
	if(!cb)
		CheckCancel = DefCheckCancel;
	else
		CheckCancel = cb;
}

/*************
 * DESCRIPTION:   Set the log call back
 * INPUT:         -
 * OUTPUT:        -
 *************/
void RSICONTEXT::SetLogCB(void (*cb)(RSICONTEXT*, char *))
{
	if(!cb)
		Log = DefLog;
	else
		Log = cb;
}

/*************
 * DESCRIPTION:   Set the update status call back
 * INPUT:         -
 * OUTPUT:        -
 *************/
void RSICONTEXT::SetUpdateStatusCB(void (*cb)(RSICONTEXT*, float, float, int, int, SMALL_COLOR*))
{
	if(!cb)
		UpdateStatus = DefUpdateStatus;
	else
		UpdateStatus = cb;
}

/*************
 * DESCRIPTION:   sets the object handler error string
 * INPUT:         error    string
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
BOOL RSICONTEXT::SetObjHandError(char *error)
{
	if(objhand_error)
		delete objhand_error;

	if(error)
	{
		objhand_error = new char[strlen(error)+1];
		if(!objhand_error)
			return FALSE;

		strcpy(objhand_error, error);
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   sets the image error string
 * INPUT:         error    string
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
BOOL RSICONTEXT::SetImageError(char *error)
{
	if(image_error)
		delete image_error;

	if(error)
	{
		image_error = new char[strlen(error)+1];
		if(!image_error)
			return FALSE;

		strcpy(image_error, error);
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   Frees all objects
 * INPUT:         none
 * OUTPUT:        none
 *************/
void RSICONTEXT::FreeObjects()
{
	OBJECT *next,*prev;
	TRIANGLE *t;

	if(obj_root)
	{
		prev = obj_root;
		do
		{
			next = (OBJECT*)prev->GetNext();
			// Triangles can be saved in blocks, handle this case here
			if(prev->Identify() == IDENT_TRIANGLE)
			{
				t = (TRIANGLE*)prev;
/*            if(t->flags & OBJECT_INBLOCK)
				{
					// triangle is in block -> delete only normals
					if(t->vnorm)
						delete [ ] t->vnorm;
					t->vnorm = NULL;
				}
				else*/
				{
					if(t->flags & OBJECT_FIRSTBLOCK)
					{
						// first triangle of block -> delete whole block
						delete [ ] t;
					}
					else
					{
						if(!(t->flags & OBJECT_INBLOCK))
						{
							// Normal triangle
							delete prev;
						}
					}
				}
			}
			else
				delete prev;
			prev = next;
		}
		while(next);
	}
	obj_root = NULL;
}

/*************
 * DESCRIPTION:   Removes an object fromt the object list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void RSICONTEXT::RemoveObject(OBJECT *obj)
{
	obj->LIST::Remove((LIST**)&obj_root);
}

