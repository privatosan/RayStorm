/***************
 * PROGRAM:       Modeler
 * NAME:          animtree_class.cpp
 * DESCRIPTION:   class for anim list tree
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    25.10.97 ah    initial release
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef ANIMTREE_CLASS_H
#include "animtree_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

extern "C" SAVEDS ASM ULONG AnimTree_ListDisplayFunc(REG(a0) struct Hook *hook, REG(a2) APTR object, REG(a1) APTR message);

/*
// hooks
typedef struct
{
	OBJECT *obj;
	Object *thumbnail, *list;
} OBJECT_ITEM;

static SAVEDS ASM ULONG ListOpenFunc(REG(a1) MUIS_Listtree_TreeNode *tn)
{
	((OBJECT_ITEM*)tn->tn_User)->obj->flags |= OBJECT_NODEOPEN;
	return 0;
}

static SAVEDS ASM ULONG ListCloseFunc(REG(a1) MUIS_Listtree_TreeNode *tn)
{
	((OBJECT_ITEM*)tn->tn_User)->obj->flags &= ~OBJECT_NODEOPEN;
	return 0;
}

static SAVEDS ASM ULONG ListConstFunc(REG(a1) OBJECT_ITEM *oi)
{
	OBJECT_ITEM *noi;

	noi = new OBJECT_ITEM;
	if(!noi)
		return NULL;

	*noi = *oi;
	return (ULONG)noi;
}

static SAVEDS ASM ULONG ListDestFunc(REG(a1) OBJECT_ITEM *oi)
{
	if(oi->thumbnail)
		DoMethod(oi->list, MUIM_List_DeleteImage, oi->thumbnail);
	delete oi;
	return 0;
}
*/

SAVEDS ASM ULONG WARPOS_PPC_FUNC(AnimTree_ListDisplayFunc)(REG(a0) struct Hook *hook, REG(a2) APTR object, REG(a1) APTR message)
{
	static char buffer[32] = "|------||--|";
	char **array = (char**)object;
	MUIS_Listtree_TreeNode *tn = (MUIS_Listtree_TreeNode*)message;
// OBJECT_ITEM *oi;

	array[0] = NULL;

/* oi = (OBJECT_ITEM*)(tn->tn_User);

	if(oi->thumbnail)
		sprintf(buffer, "\33O[%08lx]", oi->thumbnail);
	else
		buffer[0] = 0;*/

	array[1] = buffer;
	return 0;
}

/*
static const struct Hook ListOpenHook = { { NULL,NULL },(HOOKFUNC)ListOpenFunc,NULL,NULL };
static const struct Hook ListCloseHook = { { NULL,NULL },(HOOKFUNC)ListCloseFunc,NULL,NULL };
static const struct Hook ListConstHook = { { NULL,NULL },(HOOKFUNC)ListConstFunc,NULL,NULL };
static const struct Hook ListDestHook = { { NULL,NULL },(HOOKFUNC)ListDestFunc,NULL,NULL };
*/
static const struct Hook ListDisplayHook = { { NULL,NULL },(HOOKFUNC)AnimTree_ListDisplayFunc,NULL,NULL };

/*************
 * DESCRIPTION:   do initialations for browser tree
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct AnimTree_Data databuf, *data;
	struct MUIS_Listtree_TreeNode *prev;

	obj = (Object*)DoSuperNew(cl,obj,
		MUIA_Frame, MUIV_Frame_InputList,
//    MUIA_ContextMenu, TRUE,
		MUIA_List_MinLineHeight, 18,
		MUIA_Listtree_DoubleClick, MUIV_Listtree_DoubleClick_Off,
		MUIA_Listtree_SortHook, MUIV_Listtree_SortHook_Tail,
		MUIA_Listtree_DisplayHook, &ListDisplayHook,
		MUIA_Listtree_Format, "MIW=15 MAW=-1 BAR,MIW=-1 MAW=-1",
/*    MUIA_Listtree_ConstructHook, &ListConstHook,
		MUIA_Listtree_DestructHook, &ListDestHook,
		MUIA_Listtree_OpenHook, &ListOpenHook,
		MUIA_Listtree_CloseHook, &ListCloseHook,*/
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct AnimTree_Data*)INST_DATA(cl,obj);
		*data = databuf;

		prev = (struct MUIS_Listtree_TreeNode*)DOMETHOD(obj, MUIM_Listtree_Insert, "Objects", NULL, NULL, MUIV_Listtree_Insert_PrevNode_Sorted, TNF_LIST);
		prev = (struct MUIS_Listtree_TreeNode*)DOMETHOD(obj, MUIM_Listtree_Insert, "Camera", NULL, prev, MUIV_Listtree_Insert_PrevNode_Sorted, TNF_LIST);
		DOMETHOD(obj, MUIM_Listtree_Insert, "Position", NULL, prev, MUIV_Listtree_Insert_PrevNode_Sorted, TNF_OPEN);
		DOMETHOD(obj, MUIM_Listtree_Insert, "Alignment", NULL, prev, MUIV_Listtree_Insert_PrevNode_Sorted, TNF_OPEN);
		DOMETHOD(obj, MUIM_Listtree_Insert, "Size", NULL, prev, MUIV_Listtree_Insert_PrevNode_Sorted, TNF_OPEN);
		DOMETHOD(obj, MUIM_Listtree_Insert, "Material", NULL, prev, MUIV_Listtree_Insert_PrevNode_Sorted, TNF_OPEN);

		return((ULONG)obj);
	}
	return(0);
}

/*************
 * DESCRIPTION:   Dispatcher of animation tree class
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(AnimTree_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet*)msg));
	}
	return(DOSUPERMETHODA(cl,obj,msg));
}
