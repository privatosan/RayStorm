/***************
 * MODUL:         context
 * NAME:          context.h
 * DESCRIPTION:   Definition of the render context
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    14.12.97 ah    initial release
 ***************/

#ifndef CONTEXT_H
#define CONTEXT_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef CSG_H
#include "csg.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef WORLD_H
#include "world.h"
#endif

#ifndef OBJHAND_H
#include "objhand.h"
#endif

#ifndef ACTOR_H
#include "actor.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

class RSICONTEXT
{
	public:
		/* user data */
		void *data;

		/* rsi.cpp */
		SMALL_COLOR *scrarray;
		CAMERA camera;
		WORLD world;
		BOOL bOctreedone;
		char externalerror[256];
		char buf[256];

		/* object.cpp */
		OBJECT *obj_root;
		OBJECT *obj_unlimited_root;
		OBJECT *shadowcache[MAXRAYDEPTH+1];
		SURFACE surfaces[MAXRAYDEPTH+1];

		/* light.cpp */
		BOOL surface_enter;
		LIGHT *light_root;
		SURFACE surface;

		/* actor.cpp */
		ACTOR *actor_root;

		/* brush.cpp */
		IMAGE *image_root;

		/* objhand.cpp */
		SURF_NAME *surfname_root;
		HIERARCHIE *cur_hier;
		CSG *cur_csg;
		OBJECT **cur_node;

		/* octree.cpp */
		OCTREE *octreeRoot;
		ULONG maxOctreeDepth;
		ULONG identification;
		float xLength[MAXOCTREEDEPTH+1];
		float yLength[MAXOCTREEDEPTH+1];
		float zLength[MAXOCTREEDEPTH+1];
		OCTREE *startoctree;
		ULONG startdepth;
		OCTREE *rootoctree;
		ULONG rootdepth;

		/* surface.cpp */
		SURFACE *surf_root;

		/* texture.cpp */
		IM_TEXT *text_root;

		// callback functions
		void (*Cooperate)(RSICONTEXT*);
		BOOL (*CheckCancel)(RSICONTEXT*);
		void (*Log)(RSICONTEXT*, char *);
		void (*UpdateStatus)(RSICONTEXT*, float, float, int, int, SMALL_COLOR*);

	private:
		char *objhand_error;
		char *image_error;

	public:
		RSICONTEXT();
		~RSICONTEXT();

		void Cleanup();

		void SetCooperateCB(void (*)(RSICONTEXT*));
		void SetCheckCancelCB(BOOL (*)(RSICONTEXT*));
		void SetLogCB(void (*)(RSICONTEXT*, char *));
		void SetUpdateStatusCB(void (*)(RSICONTEXT*, float, float, int, int, SMALL_COLOR*));

		BOOL SetObjHandError(char*);
		char *GetObjHandError() { return objhand_error; };
		BOOL SetImageError(char*);
		char *GetImageError() { return image_error; };

		void FreeObjects();
		void RemoveObject(OBJECT*);

		void Insert(ACTOR *actor) { actor->SLIST::Insert((SLIST**)&actor_root); }
		void Insert(IMAGE *image) { image->SLIST::Insert((SLIST**)&image_root); }
		void Insert(LIGHT *light) { light->SLIST::Insert((SLIST**)&light_root); }
		void Insert(OBJECT *obj)
		{
			if(obj->flags & OBJECT_UNLIMITED)
				obj->SLIST::Insert((SLIST**)&obj_unlimited_root);
			else
				obj->SLIST::Insert((SLIST**)&obj_root);
		}
		void Insert(SURFACE *surface) { surface->SLIST::Insert((SLIST**)&surf_root); }
		void Insert(IM_TEXT *texture) { texture->SLIST::Insert((SLIST**)&text_root); }
};

#endif /* CONTEXT_H */
