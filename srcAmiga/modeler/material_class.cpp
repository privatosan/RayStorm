/***************
 * PROGRAM:       Modeler
 * NAME:          material_class.cpp
 * DESCRIPTION:   this class handles all things needed for the material class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.04.97 ah    initial release
 ***************/

#include <string.h>

#include <pragma/utility_lib.h>
#include <clib/exec_protos.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef MATERIAL_CLASS_H
#include "material_class.h"
#endif

#ifndef MATERIALITEM_CLASS_H
#include "materialitem_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

static SAVEDS ASM ULONG LayoutFunc(REG(a0) struct Hook*,REG(a2) Object*,REG(a1) struct MUI_LayoutMsg*);
static const struct Hook LayoutHook = { { NULL,NULL },(HOOKFUNC)LayoutFunc,NULL,NULL };

static struct NewMenu menu_material[] =
{
	{ NM_TITLE, NULL                 , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "New"                , 0 ,0,0,(APTR)POP_MATERIAL_NEW},
	{ NM_ITEM , "Load..."            , 0 ,0,0,(APTR)POP_MATERIAL_LOAD},
	{ NM_ITEM , "Save..."            , 0 ,0,0,(APTR)POP_MATERIAL_SAVE},
	{ NM_ITEM , "Delete"             , 0 ,0,0,(APTR)POP_MATERIAL_DELETE},
	{ NM_ITEM , "Properties..."      , 0 ,0,0,(APTR)POP_MATERIAL_PROPERTIES},
	{ NM_ITEM , "Assign to selected" , 0 ,0,0,(APTR)POP_MATERIAL_ASSIGNTOSELECTED},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};

static struct NewMenu menu_global[] =
{
	{ NM_TITLE, "Material"        , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "New"             , 0 ,0,0,(APTR)POP_MATERIAL_NEW},
	{ NM_ITEM , "Load new..."     , 0 ,0,0,(APTR)POP_MATERIAL_LOADNEW},
	{ NM_ITEM , "Remove unused"   , 0 ,0,0,(APTR)POP_MATERIAL_REMOVEUNUSED},
	{ NM_ITEM , "Remove identical", 0 ,0,0,(APTR)POP_MATERIAL_REMOVEIDENTICAL},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};

/*************
 * DESCRIPTION:   Go through material items and find that one with the given surface
 * INPUT:         surf
 * OUTPUT:        found material item or NULL if none found
 *************/
Object *SearchSurface(Object *obj, SURFACE *surface)
{
	Object *childs, *child;
	SURFACE *surf;

	if(surface)
	{
		// search for surface
		GetAttr(MUIA_Group_ChildList, obj, (ULONG*)&childs);
		if(childs)
		{
			child = (Object*)NextObject(&childs);
			if(child)
			{
				child = (Object*)NextObject(&childs);
				while(child)
				{
					GetAttr(MUIA_MaterialItem_Surface, child, (ULONG*)&surf);
					if(surface == surf)
						return child;
					child = (Object*)NextObject(&childs);
				}
			}
		}
	}
	return NULL;
}

/*************
 * DESCRIPTION:   new method
 * INPUT:         system
 * OUTPUT:
 *************/
static SAVEDS ASM ULONG LayoutFunc(REG(a0) struct Hook *h,REG(a2) Object *obj,REG(a1) struct MUI_LayoutMsg *lm)
{
	Object *cstate, *child;
	WORD maxminwidth, maxminheight;
	LONG x,y, cw,ch, maxh, xspace, yspace;

	switch (lm->lm_Type)
	{
		case MUILM_MINMAX:
			cstate = (Object *)lm->lm_Children->mlh_Head;

			maxminwidth  = 0;
			maxminheight = 0;

			/* find out biggest widths & heights of our children */
			child = (Object*)NextObject(&cstate);
			while(child)
			{
				if(maxminwidth <MUI_MAXMAX && _minwidth(child) > maxminwidth)
					maxminwidth  = _minwidth(child);
				if(maxminheight<MUI_MAXMAX && _minheight(child) > maxminheight)
					maxminheight = _minheight(child);
				child = (Object*)NextObject(&cstate);
			}

			/* set the result fields in the message */
			lm->lm_MinMax.MinWidth  = maxminwidth;
			lm->lm_MinMax.MinHeight = maxminheight;
			lm->lm_MinMax.DefWidth  = 2*maxminwidth;
			lm->lm_MinMax.DefHeight = 2*maxminheight;
			lm->lm_MinMax.MaxWidth  = MUI_MAXMAX;
			lm->lm_MinMax.MaxHeight = MUI_MAXMAX;

			return 0;

		case MUILM_LAYOUT:
			/*
			** Layout function. Here, we have to call MUI_Layout() for each
			** our children. MUI wants us to place them in a rectangle
			** defined by (0,0,lm->lm_Layout.Width-1,lm->lm_Layout.Height-1)
			** You are free to put the children anywhere in this rectangle.
			**
			** If you are a virtual group, you may also extend
			** the given dimensions and place your children anywhere. Be sure
			** to return the dimensions you need in lm->lm_Layout.Width and
			** lm->lm_Layout.Height in this case.
			**
			** Return TRUE if everything went ok, FALSE on error.
			** Note: Errors during layout are not easy to handle for MUI.
			**       Better avoid them!
			*/

			cstate = (Object *)lm->lm_Children->mlh_Head;

			xspace = 4;
			yspace = 4;

			x = xspace;
			y = yspace;
			maxh = 0;
			child = (Object*)NextObject(&cstate);
			while(child)
			{
				cw = _minwidth(child);
				ch = _minheight(child);

				if(x+cw+xspace >= lm->lm_Layout.Width)
				{
					x = xspace;
					y += maxh+yspace;
					maxh = 0;
				}

				if(ch > maxh)
					maxh = ch;

				if(!MUI_Layout(child, x,y, cw,ch, 0))
					return(FALSE);

				x += cw+xspace;

				child = (Object*)NextObject(&cstate);
			}
			lm->lm_Layout.Height = y + maxh+yspace;

			return TRUE;
	}
	return(MUILM_UNKNOWN);
}

/*************
 * DESCRIPTION:   new method
 * INPUT:         system
 * OUTPUT:        created object or NULL
 *************/
static SAVEDS ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Material_Data *data,databuf;

	obj = (Object *)DoSuperNew(cl,obj,
		VirtualFrame,
		MUIA_ContextMenu, TRUE,
		MUIA_Group_LayoutHook, &LayoutHook,
		MUIA_Background, MUII_ListBack,
	TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct Material_Data*)INST_DATA(cl,obj);
		*data = databuf;
		data->contextmenu = NULL;
		data->sigbit = AllocSignal(-1);
		if(data->sigbit == -1)
			data->sigbit = NULL;
		data->st = CreateSubTask(PreviewSubTask, data->sigbit, "Scenario Preview Task", -2, 4096);
		if(!data->st)
			return 0;
		data->recalclist = NULL;
		data->initchange = FALSE;
		return((ULONG)obj);
	}

	return(0);
}

/*************
 * DESCRIPTION:   OM_SET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG Set(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Material_Data *data = (struct Material_Data *)INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

	tags=msg->ops_AttrList;
	do
	{
		tag=NextTagItem(&tags);
		if(tag)
		{
			switch (tag->ti_Tag)
			{
				case MUIA_Material_MainWindow:
					data->main = (Object*)tag->ti_Data;
					break;
			}
		}
	}
	while(tag);

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   add an new material to manager
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG AddMaterial(struct IClass *cl,Object *obj,struct MUIP_Material_Material *msg)
{
	Material_Data *data = (Material_Data *)INST_DATA(cl,obj);
	Object *childs, *child;
// Object *first, *next;
	Object *newobj, *after = NULL;
	SURFACE *surf;
	BOOL success;
	int res;

	GetAttr(MUIA_Group_ChildList, obj, (ULONG*)&childs);
	if(childs)
	{
		child = (Object*)NextObject(&childs);
		if(child)
		{
			child = (Object*)NextObject(&childs);
			while(child)
			{
				GetAttr(MUIA_MaterialItem_Surface, child, (ULONG*)&surf);
				res = stricmp(surf->name, msg->surf->name);
				if(res == 0)
					return TRUE;
				if(res < 0)
					after = child;
				if(surf == msg->surf)
					return TRUE;
				child = (Object*)NextObject(&childs);
			}
		}
	}

	newobj = (Object*)NewObject(CL_MaterialItem->mcc_Class,NULL,
		ImageButtonFrame,
		MUIA_MaterialItem_Surface, msg->surf,
		MUIA_MaterialItem_SubTask, data->st,
		MUIA_Background,  MUII_BACKGROUND,
		MUIA_Render_XRes, PREVIEW_WIDTH,
		MUIA_Render_YRes, PREVIEW_HEIGHT,
		MUIA_CycleChain, TRUE,
	TAG_DONE);

	if(newobj)
	{
		if(!data->initchange)
			success = DOMETHOD(obj, MUIM_Group_InitChange);
		else
			success = TRUE;
		if(success)
		{
//       if(!after)
				DOMETHOD(obj, OM_ADDMEMBER, newobj);

/*       GetAttr(MUIA_Group_ChildList, obj, (ULONG*)&childs);
			if(childs)
			{
				child = (Object*)NextObject(&childs);
				if(child)
				{
					first = (Object*)NextObject(&childs);
					child = first;
					while(child)
					{
						next = (Object*)NextObject(&childs);

						if(child != newobj)
						{
							DOMETHOD(obj, OM_REMMEMBER, child);

							DOMETHOD(obj, OM_ADDMEMBER, child);

							if(after == child)
								DOMETHOD(obj, OM_ADDMEMBER, newobj);
						}

						child = next;
						if(child == first)
							break;
					}
				}
			}*/
			if(!data->initchange)
				DOMETHOD(obj, MUIM_Group_ExitChange);
			// add to recalc list
			DOMETHOD(obj, MUIM_Material_RecalcItem, newobj);
			return TRUE;
		}
		else
		{
			MUI_DisposeObject(newobj);
		}
	}

	return FALSE;
}

/*************
 * DESCRIPTION:   update a material
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG UpdateMaterial(struct IClass *cl,Object *obj,struct MUIP_Material_Material *msg)
{
	Object *item;

	item = SearchSurface(obj, msg->surf);
	if(item)
		DOMETHOD(obj, MUIM_Material_RecalcItem, item);

	return 0;
}

/*************
 * DESCRIPTION:   free all materials
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG FreeMaterials(struct IClass *cl,Object *obj, Msg msg)
{
	struct Material_Data *data = (struct Material_Data*)INST_DATA(cl,obj);
	Object *childs, *child, *next;

	if(data->recalclist)
	{
		data->recalclist->FreeList();
		data->recalclist = NULL;
	}

	GetAttr(MUIA_Group_ChildList, obj, (ULONG*)&childs);
	if(childs)
	{
		if(DOMETHOD(obj, MUIM_Group_InitChange))
		{
			child = (Object*)NextObject(&childs);
			if(child)
			{
				child = (Object*)NextObject(&childs);
				while(child)
				{
					next = (Object*)NextObject(&childs);
					DOMETHOD(obj, OM_REMMEMBER, child);
					MUI_DisposeObject(child);
					child = next;
				}
			}
			DOMETHOD(obj, MUIM_Group_ExitChange);
		}
	}

	return NULL;
}

/*************
 * DESCRIPTION:   free a given material
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG RemoveMaterial(struct IClass *cl,Object *obj,struct MUIP_Material_Material *msg)
{
	Object *item;

	item = SearchSurface(obj, msg->surf);
	if(item)
	{
		if(DOMETHOD(obj, MUIM_Group_InitChange))
		{
			// remove us from the recalc list
			DOMETHOD(obj, MUIM_Material_RemoveRecalcItem, item);
			// and delete it
			DOMETHOD(obj, OM_REMMEMBER, item);
			MUI_DisposeObject(item);
			DOMETHOD(obj, MUIM_Group_ExitChange);
		}
	}
	return 0;
}

/*************
 * DESCRIPTION:   Go through all materials and delete all, which are not assign to an object.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG RemoveUnused(struct IClass *cl,Object *obj, Msg msg)
{
	Object *childs, *child, *next;
	SURFACE *surf;
	OBJECT *object;

	GetAttr(MUIA_Group_ChildList, obj, (ULONG*)&childs);
	if(childs)
	{
		child = (Object*)NextObject(&childs);
		if(child)
		{
			child = (Object*)NextObject(&childs);
			while(child)
			{
				next = (Object*)NextObject(&childs);
				GetAttr(MUIA_MaterialItem_Surface, child, (ULONG*)&surf);
				object = IsSurfaceUsed(surf);
				if(!object)
				{
					// nobody needs this surface, delete it
					if(sciRemoveMaterial(surf))
						sciDeleteSurface(surf);
				}

				child = next;
			}
		}
	}
	return NULL;
}

/*************
 * DESCRIPTION:   Go through all materials and delete all, which are identical.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG RemoveIdentical(struct IClass *cl,Object *obj, Msg msg)
{
	Object *childs, *child, *next;
	SURFACE *surf, *ident;

	GetAttr(MUIA_Group_ChildList, obj, (ULONG*)&childs);
	if(childs)
	{
		child = (Object*)NextObject(&childs);
		if(child)
		{
			child = (Object*)NextObject(&childs);
			while(child)
			{
				next = (Object*)NextObject(&childs);
				GetAttr(MUIA_MaterialItem_Surface, child, (ULONG*)&surf);

				do
				{
					ident = surf->FindIdentical();
					if(ident)
					{
						if(!(ident->flags & SURF_DEFAULT))
						{
							// we have found a identical surface
							// first assign the current surface to all objects with this identical surface
							sciChangeSurface(ident, surf);
							// then remove the surface
							if(sciRemoveMaterial(ident))
								sciDeleteSurface(ident);
						}
					}
				}
				while(ident);

				child = next;
			}
		}
	}
	return NULL;
}

/*************
 * DESCRIPTION:   Called when a popup menun is about to be created.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG ContextMenuBuild(struct IClass *cl,Object *obj,struct MUIP_ContextMenuBuild *msg)
{
	struct Material_Data *data = (struct Material_Data*)INST_DATA(cl,obj);
	Object *childs, *child;
	LONG left,right,top,bottom;
	SURFACE *surf;

	if(data->contextmenu)
		MUI_DisposeObject(data->contextmenu);

	GetAttr(MUIA_Group_ChildList, obj, (ULONG*)&childs);
	if(childs)
	{
		child = (Object*)NextObject(&childs);
		if(child)
		{
			child = (Object*)NextObject(&childs);
			while(child)
			{
				GetAttr(MUIA_LeftEdge, child, (ULONG*)&left);
				GetAttr(MUIA_RightEdge, child, (ULONG*)&right);
				GetAttr(MUIA_TopEdge, child, (ULONG*)&top);
				GetAttr(MUIA_BottomEdge, child, (ULONG*)&bottom);
				if((msg->mx >= left) && (msg->mx < right) &&
					(msg->my >= top) && (msg->my < bottom))
				{
					GetAttr(MUIA_MaterialItem_Surface, child, (ULONG*)&surf);
					menu_material->nm_Label = surf->GetName();
					if(!menu_material->nm_Label)
						menu_material->nm_Label = "Unused";
					data->contextmenu = MUI_MakeObject(MUIO_MenustripNM, menu_material, 0);
					data->contextobject = child;
					return (ULONG)data->contextmenu;
				}
				child = (Object*)NextObject(&childs);
			}

			data->contextmenu = MUI_MakeObject(MUIO_MenustripNM, menu_global, 0);
			return (ULONG)data->contextmenu;
		}
	}
	return NULL;
}

/*************
 * DESCRIPTION:   Called when an user selects a menu item from a popup menu.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG ContextMenuChoice(struct IClass *cl,Object *obj,struct MUIP_ContextMenuChoice *msg)
{
	struct Material_Data *data = (struct Material_Data*)INST_DATA(cl,obj);
	SURFACE *surf, *oldsurf;

	switch(muiUserData(msg->item))
	{
		case POP_NONE:
			break;
		case POP_MATERIAL_NEW:
			DOMETHOD(data->main, MUIM_MainWindow_MaterialNew);
			break;
		case POP_MATERIAL_DELETE:
			GetAttr(MUIA_MaterialItem_Surface, data->contextobject, (ULONG*)&surf);
			if(sciRemoveMaterial(surf))
				sciDeleteSurface(surf);
			break;
		case POP_MATERIAL_PROPERTIES:
			GetAttr(MUIA_MaterialItem_Surface, data->contextobject, (ULONG*)&surf);
			sciOpenMaterialRequ(surf);
			break;
		case POP_MATERIAL_LOAD:
			surf = sciCreateSurface(SURF_DEFAULTNAME);
			if(surf)
			{
				if(sciLoadMaterial(surf))
				{
					GetAttr(MUIA_MaterialItem_Surface, data->contextobject, (ULONG*)&oldsurf);
					delete oldsurf;

					SetAttrs(data->contextobject, MUIA_MaterialItem_Surface, (ULONG*)surf, TAG_DONE);
					DOMETHOD(obj, MUIM_Material_RecalcItem, data->contextobject);
				}
				else
					sciDeleteSurface(surf);
			}
			break;
		case POP_MATERIAL_SAVE:
			GetAttr(MUIA_MaterialItem_Surface, data->contextobject, (ULONG*)&surf);
			sciSaveMaterial(surf);
			break;
		case POP_MATERIAL_LOADNEW:
			DOMETHOD(data->main, MUIM_MainWindow_MaterialLoadNew);
			break;
		case POP_MATERIAL_ASSIGNTOSELECTED:
			GetAttr(MUIA_MaterialItem_Surface, data->contextobject, (ULONG*)&surf);
			sciAssignSurfaceToSelectedObjects(surf);
			break;
		case POP_MATERIAL_REMOVEUNUSED:
			SetSleep(TRUE);
			DOMETHOD(data->main, MUIM_MainWindow_MaterialRemoveUnused);
			SetSleep(FALSE);
			break;
		case POP_MATERIAL_REMOVEIDENTICAL:
			DOMETHOD(data->main, MUIM_MainWindow_MaterialRemoveIdentical);
			DOMETHOD(obj, MUIM_Material_RemoveIdentical);
			break;
	}

	return 0;
}

/*************
 * DESCRIPTION:   cleanup
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Cleanup(struct IClass *cl,Object *obj,Msg msg)
{
	struct Material_Data *data = (struct Material_Data *)INST_DATA(cl,obj);

	if(data->st)
		KillSubTask(data->st);
	if(data->contextmenu)
		MUI_DisposeObject(data->contextmenu);
	if(data->sigbit)
		FreeSignal(data->sigbit);
	if(data->recalclist)
		data->recalclist->FreeList();

	return DOSUPERMETHODA(cl,obj,msg);
}

/*************
 * DESCRIPTION:   get a thumb nail image for a surface
 * INPUT:         system
 * OUTPUT:        image object
 *************/
static ULONG GetThumbNail(struct IClass *cl,Object *obj,struct MUIP_Material_Material *msg)
{
	Object *item;

	item = SearchSurface(obj, msg->surf);
	if(item)
		return DOMETHOD(item, MUIM_MaterialItem_GetThumbNail);

	return NULL;
}

/*************
 * DESCRIPTION:   called when a subtask rendering is finished -> start rendering
 *    of waiting items (if there are any)
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG SubTaskFinished(struct IClass *cl,Object *obj, Msg msg)
{
	struct Material_Data *data = (struct Material_Data *)INST_DATA(cl,obj);

	if(data->recalclist)
		data->recalclist->Remove((SLIST**)&data->recalclist);

	if(data->recalclist && !data->initchange)
		DOMETHOD(data->recalclist->obj, MUIM_MaterialItem_Recalc);
	return 0;
}

/*************
 * DESCRIPTION:   Add a new item to the recalc list. If no other item is recalculating
 *    its display start the recalculation of the new item.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG RecalcItem(struct IClass *cl,Object *obj, struct MUIP_Object *msg)
{
	struct Material_Data *data = (struct Material_Data *)INST_DATA(cl,obj);
	RECALC_ITEM *item;
	BOOL working;

	item = new RECALC_ITEM;
	if(item)
	{
		if(data->recalclist)
			working = TRUE;
		else
			working = FALSE;

		item->obj = msg->obj;
		item->Append((SLIST**)&data->recalclist);

		if(!working && !data->initchange)
			DOMETHOD(msg->obj, MUIM_MaterialItem_Recalc);
	}
	return 0;
}

/*************
 * DESCRIPTION:   Remove a item from the recalc list.
 * INPUT:         system
 * OUTPUT:        FALSE if not in list, TRUE if removed
 *************/
static SAVEDS ULONG RemoveRecalcItem(struct IClass *cl,Object *obj, struct MUIP_Object *msg)
{
	struct Material_Data *data = (struct Material_Data *)INST_DATA(cl,obj);
	RECALC_ITEM *cur;

	cur = data->recalclist;
	while(cur)
	{
		if(cur->obj == msg->obj)
		{
			if(cur == data->recalclist)
			{
				// first item: start rendering of next item
				DOMETHOD(obj, MUIM_Material_SubTaskFinished);
			}
			else
				cur->Remove((SLIST**)&data->recalclist);
			return TRUE;
		}
		cur = (RECALC_ITEM*)cur->GetNext();
	}

	return FALSE;
}

/*************
 * DESCRIPTION:   Dispatcher for material class
 * INPUT:         system
 * OUTPUT:        0
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(Material_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	struct Material_Data *data = (struct Material_Data *)INST_DATA(cl,obj);

	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl,obj,(struct opSet*)msg));
		case OM_SET:
			return(Set(cl,obj,(struct opSet*)msg));
		case MUIM_Cleanup:
			return(Cleanup(cl, obj, msg));
		case MUIM_Material_AddMaterial:
			return(AddMaterial(cl,obj,(struct MUIP_Material_Material*)msg));
		case MUIM_Material_UpdateMaterial:
			return(UpdateMaterial(cl,obj,(struct MUIP_Material_Material*)msg));
		case MUIM_Material_RemoveMaterial:
			return(RemoveMaterial(cl,obj,(struct MUIP_Material_Material*)msg));
		case MUIM_Material_GetThumbNail:
			return(GetThumbNail(cl,obj,(struct MUIP_Material_Material*)msg));
		case MUIM_Material_FreeMaterials:
			return(FreeMaterials(cl, obj, msg));
		case MUIM_ContextMenuBuild:
			return(ContextMenuBuild(cl,obj,(struct MUIP_ContextMenuBuild*)msg));
		case MUIM_ContextMenuChoice:
			return(ContextMenuChoice(cl,obj,(struct MUIP_ContextMenuChoice*)msg));
		case MUIM_Material_SubTaskFinished:
			return(SubTaskFinished(cl, obj, msg));
		case MUIM_Material_RecalcItem:
			return(RecalcItem(cl, obj, (struct MUIP_Object*)msg));
		case MUIM_Material_RemoveRecalcItem:
			return(RemoveRecalcItem(cl, obj, (struct MUIP_Object*)msg));
		case MUIM_Material_InitChange:
			data->initchange = TRUE;
			DOMETHOD(obj, MUIM_Group_InitChange);
			return 0;
		case MUIM_Material_ExitChange:
			data->initchange = FALSE;
			DOMETHOD(obj, MUIM_Group_ExitChange);
			if(data->recalclist)
				DOMETHOD(data->recalclist->obj, MUIM_MaterialItem_Recalc);
			return 0;
		case MUIM_Material_StopTask:
			if(data->recalclist)
				DOMETHOD(data->recalclist->obj, MUIM_MaterialItem_StopTask);
			return 0;
		case MUIM_Material_RemoveUnused:
			return(RemoveUnused(cl, obj, msg));
		case MUIM_Material_RemoveIdentical:
			return(RemoveIdentical(cl, obj, msg));
	}

	return(DOSUPERMETHODA(cl,obj,msg));
}
