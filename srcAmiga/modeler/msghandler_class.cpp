/***************
 * PROGRAM:       Modeler
 * NAME:          msghandler_class.cpp
 * DESCRIPTION:   This handles messages from subtasks. It gets the message and
 *    looks if the object to receive the message is still valid. It maintains
 *    a list of valid objects.
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    30.08.97 ah    initial release
 *
 * Tags                          arguments
 * --------------------------------------------------------------------------
 * MUIM_MsgHandler_AddObject     Object*     add a new valid object
 * MUIM_MsgHandler_RemoveObject  Object*     remove an object
 * MUIM_MsgHandler_PushMethod    msg         push a method, the argument is the original PushMethod message
 ***************/

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef MSGHANDLER_CLASS_H
#include "msghandler_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

void VALID_OBJECT::Insert(VALID_OBJECT **root)
{
	next = *root;
	*root = this;
}

void VALID_OBJECT::Remove(VALID_OBJECT **root)
{
	VALID_OBJECT *cur,*prev;

	prev = NULL;
	cur = *root;
	while(cur && (cur != this))
	{
		prev = cur;
		cur = cur->GetNext();
	}
	if(cur)
	{
		if(prev)
			prev->next = cur->next;
		else
			*root = cur->next;
		delete cur;
	}
}

void VALID_OBJECT::FreeList()
{
	VALID_OBJECT *cur, *next;

	cur = this;
	while(cur)
	{
		next = cur->GetNext();
		delete cur;
		cur = next;
	}
}

/*************
 * DESCRIPTION:   do initialations for float subclass
 * INPUT:         system
 * OUTPUT:        created object or 0 if failed
 *************/
static ULONG New(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct MsgHandler_Data *data;

	obj = (Object *)DoSuperNew(cl,obj,TAG_MORE,msg->ops_AttrList);
	if(obj)
	{
		data = (struct MsgHandler_Data*)INST_DATA(cl,obj);
		data->objects = NULL;

		return((ULONG)obj);
	}
	return 0;
}

/*************
 * DESCRIPTION:   Add a new object to the list of valid objects
 * INPUT:         system
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
static ULONG AddObject(struct IClass *cl, Object *obj, struct MUIP_Object *msg)
{
	struct MsgHandler_Data *data = (struct MsgHandler_Data *)INST_DATA(cl,obj);
	VALID_OBJECT *object;

	object = new VALID_OBJECT;
	if(!object)
		return FALSE;

	object->object = msg->obj;
	object->Insert(&data->objects);

	return TRUE;
}

/*************
 * DESCRIPTION:   Remove a object from the list of valid objects
 * INPUT:         system
 * OUTPUT:        FALSE if object not found, else TRUE
 *************/
static ULONG RemoveObject(struct IClass *cl, Object *obj, struct MUIP_Object *msg)
{
	struct MsgHandler_Data *data = (struct MsgHandler_Data *)INST_DATA(cl,obj);
	VALID_OBJECT *cur;

	cur = data->objects;
	while(cur)
	{
		if(cur->object == msg->obj)
		{
			cur->Remove(&data->objects);
			return TRUE;
		}
		cur = (VALID_OBJECT*)cur->GetNext();
	}

	return FALSE;
}

/*************
 * DESCRIPTION:   Try to send a message to a object. Ignore the message if the destination
 *    object is not in our list of valid object.
 * INPUT:         system
 * OUTPUT:        return value of called method
 *************/
static ULONG PushMessage(struct IClass *cl, Object *obj, struct MUIP_Application_PushMethod *msg)
{
	struct MsgHandler_Data *data = (struct MsgHandler_Data *)INST_DATA(cl,obj);
	VALID_OBJECT *cur;

	cur = data->objects;
	while(cur)
	{
		if(cur->object == msg->dest)
			return DoMethodA(msg->dest, (Msg)&((&msg->count)[1]));
		cur = (VALID_OBJECT*)cur->GetNext();
	}

	// object is no longer valid: ignore message
	return 0;
}

/*************
 * DESCRIPTION:   Dispose
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Dispose(struct IClass *cl, Object *obj, Msg msg)
{
	struct MsgHandler_Data *data = (struct MsgHandler_Data *)INST_DATA(cl,obj);

	if(data->objects)
		data->objects->FreeList();

	return 0;
}

/*************
 * DESCRIPTION:
 * INPUT:
 * OUTPUT:
 *************/
SAVEDS ASM ULONG MsgHandler_Dispatcher(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	switch(msg->MethodID)
	{
		case OM_NEW:
			return(New(cl, obj, (struct opSet*)msg));
		case OM_DISPOSE:
			return(Dispose(cl, obj, msg));
		case MUIM_MsgHandler_AddObject:
			return(AddObject(cl, obj, (struct MUIP_Object*)msg));
		case MUIM_MsgHandler_RemoveObject:
			return(RemoveObject(cl, obj, (struct MUIP_Object*)msg));
		case MUIM_MsgHandler_PushMessage:
			return(PushMessage(cl, obj, (struct MUIP_Application_PushMethod*)msg));
	}
	return(DoSuperMethodA(cl,obj,msg));
}
