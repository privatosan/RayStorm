/***************
 * PROGRAM:       Modeler
 * NAME:          undo.cpp
 * DESCRIPTION:   Functions for undo class. This class implements an multilevel
 *    undo/redo function. For each undo/redo-action there is a class definition.
 * AUTHORS:       Andreas Heumann & Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    16.12.96 ah    initial release
 *    07.01.97 ah    added UNDO_CREATE
 *    30.01.97 ah    added Redo()
 *    28.02.97 ah    tracked objects do call UpdateTracking()
 *    29.07.97 ah    chnaged the class UNDO_TRANSFORM: now it handles
 *       a list of objects when a couple of objects are selected
 ***************/

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef UNDO_H
#include "undo.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef SOR_H
#include "sor.h"
#endif

static UNDO *root = NULL;
static UNDO *redoroot = NULL;
static int undo_space = 0;

/*************
 * DESCRIPTION:   Add a new undo action to the undo list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void UNDO::Add()
{
	int size;
	UNDO *cur, *next, *prev;
	PREFS p;
	ULONG maxundospace;

	// free redos
	cur = redoroot;
	while(cur)
	{
		undo_space -= cur->GetSize();
		next = (UNDO*)cur->GetNext();
		delete cur;
		cur = next;
	}
	redoroot = NULL;
	utility.EnableMenuItem(MENU_REDO, FALSE);

	undo_space += this->GetSize();

	if(!root)
		utility.EnableMenuItem(MENU_UNDO, TRUE);

	this->SLIST::Insert((SLIST**)&root);

	p.id = ID_UNDO;
	if(p.GetPrefs())
		maxundospace = *(ULONG*)(p.data);
	else
		maxundospace = 100000;
	p.data = NULL;

	if(undo_space > maxundospace)
	{
		// free old undos
		size = 0;
		cur = root;
		prev = NULL;
		while(cur && (size < maxundospace))
		{
			size += cur->GetSize();
			if(size < maxundospace)
			{
				prev = cur;
				cur = (UNDO*)cur->GetNext();
			}
		}
		if(cur != root)
		{
			if(prev)
				prev->SetNext(NULL);
			else
				root = NULL;
			while(cur)
			{
				next = (UNDO*)cur->GetNext();
				undo_space -= this->GetSize();
				delete cur;
				cur = next;
			}
		}
	}
}

/*************
 * DESCRIPTION:   Take the first action from the undo list and undo it
 * INPUT:         -
 * OUTPUT:        see undo.h for the values
 *************/
int Undo()
{
	int result;
	UNDO *next;

	if(root)
	{
		undo_space -= root->GetSize();
		result = root->Undo();
		next = (UNDO*)root->GetNext();

		// move to redo list
		if(!redoroot)
			utility.EnableMenuItem(MENU_REDO, TRUE);
		root->SLIST::Insert((SLIST**)&redoroot);
		undo_space += root->GetSize();

		root = next;
		if(!root)
			utility.EnableMenuItem(MENU_UNDO, FALSE);
	}
	else
		result = UNDO_NOUNDO;

	return result;
}

/*************
 * DESCRIPTION:   Take the last undone action from the redo list and redo it
 * INPUT:         -
 * OUTPUT:        see undo.h for the values
 *************/
int Redo()
{
	int result;
	UNDO *next;

	if(redoroot)
	{
		undo_space -= redoroot->GetSize();
		result = redoroot->Redo();
		next = (UNDO*)redoroot->GetNext();

		// move to undo list
		if(!root)
			utility.EnableMenuItem(MENU_UNDO, TRUE);
		redoroot->SLIST::Insert((SLIST**)&root);
		redoroot = next;
		if(!redoroot)
			utility.EnableMenuItem(MENU_REDO, FALSE);
	}
	else
		result = UNDO_NOREDO;

	return result;
}

/*************
 * DESCRIPTION:   Free all undo actions
 * INPUT:         -
 * OUTPUT:        -
 *************/
void FreeUndo()
{
	if(root)
		root->FreeList();
	root = NULL;
	utility.EnableMenuItem(MENU_UNDO, FALSE);
	if(redoroot)
		redoroot->FreeList();
	redoroot = NULL;
	utility.EnableMenuItem(MENU_REDO, FALSE);
}

/*************
 * DESCRIPTION:   Constructor of transformation undo
 * INPUT:         -
 * OUTPUT:        none
 *************/
UNDO_TRANSFORM::UNDO_TRANSFORM()
{
	objects = NULL;
}

/*************
 * DESCRIPTION:   destructor of transform undo
 * INPUT:         -
 * OUTPUT:        -
 *************/
UNDO_TRANSFORM::~UNDO_TRANSFORM()
{
	if(objects)
		objects->FreeList();
}

/*************
 * DESCRIPTION:   add an object to the transformation list
 * INPUT:         obj      object to undo transformation from
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL UNDO_TRANSFORM::AddTransformed(OBJECT *obj)
{
	UNDO_TRANSFORM_OBJECT *cur;

	cur = new UNDO_TRANSFORM_OBJECT;
	if(!cur)
		return FALSE;

	cur->object = obj;
	cur->pos = obj->pos;
	cur->orient_x = obj->orient_x;
	cur->orient_y = obj->orient_y;
	cur->orient_z = obj->orient_z;
	cur->align = obj->align;
	cur->size = obj->size;

	cur->Append((SLIST**)&objects);
	return TRUE;
}

/*************
 * DESCRIPTION:   add all selected objects to the transformation list
 * INPUT:         -
 * OUTPUT:        -
 *************/
static void AddSelected(OBJECT *obj, void *undo)
{
	if(obj)
		((UNDO_TRANSFORM*)undo)->AddTransformed(obj);
}

void UNDO_TRANSFORM::AddSelectedObjects()
{
	GetSelectedObjects(AddSelected, this, GETOBJECT_BRUSHES | GETOBJECT_TEXTURES);
}

/*************
 * DESCRIPTION:   This routine does both undo and redo for transformations.
 * INPUT:         -
 * OUTPUT:        -
 *************/
void UNDO_TRANSFORM::Undo_Redo()
{
	UNDO_TRANSFORM_OBJECT *cur;

	cur = objects;
	while(cur)
	{
		cur->Undo_Redo();
		cur = (UNDO_TRANSFORM_OBJECT*)cur->GetNext();
	}
}

/*************
 * DESCRIPTION:   Undo a transformation
 * INPUT:         -
 * OUTPUT:        UNDO_OK_REDRAW
 *************/
int UNDO_TRANSFORM::Undo()
{
	Undo_Redo();

	return UNDO_OK_REDRAW;
}

/*************
 * DESCRIPTION:   Redo a transformation
 * INPUT:         -
 * OUTPUT:        UNDO_OK_REDRAW
 *************/
int UNDO_TRANSFORM::Redo()
{
	Undo_Redo();

	return UNDO_OK_REDRAW;
}

/*************
 * DESCRIPTION:   Get size needed for this undo operation
 * INPUT:         -
 * OUTPUT:        size of undo operation
 *************/
int UNDO_TRANSFORM::GetSize()
{
	UNDO_TRANSFORM_OBJECT *cur;
	int size;

	size = sizeof(UNDO_TRANSFORM);
	cur = objects;
	while(cur)
	{
		size += sizeof(UNDO_TRANSFORM_OBJECT);
		cur = (UNDO_TRANSFORM_OBJECT*)cur->GetNext();
	}
	return size;
}

/*************
 * DESCRIPTION:   Update the saved axis of all objects
 * INPUT:         -
 * OUTPUT:        -
 *************/
void UNDO_TRANSFORM::UpdateObjects()
{
	UNDO_TRANSFORM_OBJECT *cur;

	cur = objects;
	while(cur)
	{
		cur->pos = cur->object->pos;
		cur->orient_x = cur->object->orient_x;
		cur->orient_y = cur->object->orient_y;
		cur->orient_z = cur->object->orient_z;
		cur->align = cur->object->align;
		cur->size = cur->object->size;
		cur = (UNDO_TRANSFORM_OBJECT*)cur->GetNext();
	}
}

/*************
 * DESCRIPTION:   Set the objects
 * INPUT:         -
 * OUTPUT:        -
 *************/
typedef struct
{
	DISPLAY *display;
	MATRIX_STACK stack;
	UNDO_TRANSFORM_OBJECT *root;
} TRANS_DATA;

static void SetObject(OBJECT *obj, void *data)
{
	MATRIX m;
	UNDO_TRANSFORM_OBJECT *cur;

	if(obj)
	{
		cur = ((TRANS_DATA*)data)->root;
		m.SetOMatrix(&obj->orient_x, &obj->orient_y, &obj->orient_z);
		while(cur)
		{
			if(obj == cur->object)
			{
				obj->SetActiveObject(((TRANS_DATA*)data)->display, &((TRANS_DATA*)data)->stack, TRUE);

				obj->SetObject(((TRANS_DATA*)data)->display,
					&((TRANS_DATA*)data)->display->pos,
					&((TRANS_DATA*)data)->display->orient_x,
					&((TRANS_DATA*)data)->display->orient_y,
					&((TRANS_DATA*)data)->display->orient_z,
					&((TRANS_DATA*)data)->display->size);
				break;
			}
			cur = (UNDO_TRANSFORM_OBJECT*)cur->GetNext();
		}
		((TRANS_DATA*)data)->stack.Push(&m);
	}
	else
	{
		((TRANS_DATA*)data)->stack.Pop();
	}
}

void UNDO_TRANSFORM::SetObjects(DISPLAY *display)
{
	TRANS_DATA data;

	if(!(display->editmode & EDIT_MESH))
	{
		InvOrient(&display->orient_x, &display->orient_y, &display->orient_z,
			&display->orient_x, &display->orient_y, &display->orient_z);
	}

	data.display = display;
	data.root = objects;
	GetObjects(SetObject, &data, GETOBJECT_BRUSHES | GETOBJECT_TEXTURES);
}

/*************
 * DESCRIPTION:   Reset the open counter and window pointer of the objects
 *    in the transform list
 * INPUT:         -
 * OUTPUT:        -
 *************/
void UNDO_TRANSFORM::ResetOpenCount()
{
	UNDO_TRANSFORM_OBJECT *cur;

	cur = objects;
	while(cur)
	{
		cur->object->opencnt--;
		cur->object->win = NULL;
		cur = (UNDO_TRANSFORM_OBJECT*)cur->GetNext();
	}
}

/*************
 * DESCRIPTION:   This routine does both undo and redo for transformations.
 * INPUT:         -
 * OUTPUT:        -
 *************/
void UNDO_TRANSFORM_OBJECT::Undo_Redo()
{
	VECTOR dpos, dox, doy, doz, dsize;
	MATRIX m,m1;
	VECTOR rpos, rorient_x, rorient_y, rorient_z, rsize;

	// Save current values for redo/undo
	rpos = object->pos;
	rorient_x = object->orient_x;
	rorient_y = object->orient_y;
	rorient_z = object->orient_z;
	rsize = object->size;

	// we need delta values
	VecSub(&pos, &object->pos, &dpos);
	object->pos = pos;

	m.SetOMatrix(&orient_x, &orient_y, &orient_z);
	m.InvMat(&m1);
	dox = object->orient_x;
	doy = object->orient_y;
	doz = object->orient_z;
	m1.MultMatOrient(&dox, &doy, &doz);
	object->orient_x = orient_x;
	object->orient_y = orient_y;
	object->orient_z = orient_z;

	dsize.x = size.x / object->size.x;
	dsize.y = size.y / object->size.y;
	dsize.z = size.z / object->size.z;
	object->size = size;

	object->align = align;

	object->SetTreeSizes(&dsize);
	object->SetObject(NULL, &dpos, &dox, &dox, &dox, &dsize);

	if(object->istracked)
		UpdateTracking();

	// remember old values for redo/undo
	pos = rpos;
	orient_x = rorient_x;
	orient_y = rorient_y;
	orient_z = rorient_z;
	size = rsize;
}

/*************
 * DESCRIPTION:   Constructor of delete undo
 * INPUT:         -
 * OUTPUT:        -
 *************/
UNDO_DELETE::UNDO_DELETE()
{
	objects = NULL;
	redoobjects = NULL;
	sornodes = NULL;
	redosornodes = NULL;
}

/*************
 * DESCRIPTION:   destructor of delete undo
 * INPUT:         -
 * OUTPUT:        -
 *************/
UNDO_DELETE::~UNDO_DELETE()
{
	if(redoobjects)
		redoobjects->FreeList();

	if(objects)
		objects->FreeObjectsList();

	if(sornodes)
		sornodes->FreeList();

	if(redosornodes)
		redosornodes->FreeList();
}

/*************
 * DESCRIPTION:   Constructor of sor node delete undo
 * INPUT:         -
 * OUTPUT:        -
 *************/
UNDO_SORNODE::UNDO_SORNODE()
{
	node = NULL;
}

/*************
 * DESCRIPTION:   destructor of sor node delete undo
 * INPUT:         -
 * OUTPUT:        -
 *************/
UNDO_SORNODE::~UNDO_SORNODE()
{
	if(node)
		delete node;
}

/*************
 * DESCRIPTION:   add an deleted object to the object undo list
 * INPUT:         obj      object to add to list
 * OUTPUT:        -
 *************/
void UNDO_DELETE::AddDeleted(OBJECT *obj)
{
	obj->LIST::Append((LIST**)&objects);
}

/*************
 * DESCRIPTION:   add a cutted object to the object undo list. The object
 *    is duplicated.
 * INPUT:         obj      object to add to list
 * OUTPUT:        TRUE all ok else FALSE
 *************/
BOOL UNDO_DELETE::AddCutted(OBJECT *obj)
{
	OBJECT *dups = NULL;

	if(DuplicateObjects(&dups, obj))
	{
		dups->LIST::Append((LIST**)&objects);
		return TRUE;
	}
	else
		return FALSE;
}

/*************
 * DESCRIPTION:   add a deleted SOR node to the node undo list
 * INPUT:         node     node to add to list
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL UNDO_DELETE::AddSORNode(SOR *sor, SOR_NODE *node)
{
	UNDO_SORNODE *cur;

	cur = new UNDO_SORNODE;
	if(!cur)
		return FALSE;

	cur->sor = sor;
	cur->node = node;

	cur->Append((SLIST**)&sornodes);
	return TRUE;
}

/*************
 * DESCRIPTION:   Undo a deleted object
 * INPUT:         -
 * OUTPUT:        result
 *************/
int UNDO_DELETE::Undo()
{
	OBJECT *cur, *next;
	OBJECTLIST *obj;
	UNDO_SORNODE *node, *nextnode;
	REDO_SORNODE *node1;

	cur = objects;
	while(cur)
	{
		next = (OBJECT*)cur->GetNext();

		obj = new OBJECTLIST;
		if(!obj)
			return UNDO_FAILED;
		obj->object = cur;
		obj->SLIST::Insert((SLIST**)&redoobjects);

		cur->InitWithoutDown();
		cur->Append();
		cur = next;
	}
	objects = NULL;

	node = sornodes;
	while(node)
	{
		nextnode = (UNDO_SORNODE*)node->GetNext();

		// move to redo list
		node1 = new REDO_SORNODE;
		if(!node1)
			return UNDO_FAILED;
		node1->sor = node->sor;
		node1->p = node->node->p;
		node1->SLIST::Insert((SLIST**)&redosornodes);

		node->sor->AddNode(node->node);
		node->node = NULL;
		delete node;
		node = nextnode;
	}
	sornodes = NULL;

	return UNDO_OK_REDRAW;
}

/*************
 * DESCRIPTION:   Redo a deleted object
 * INPUT:         -
 * OUTPUT:        UNDO_OK_REDRAW
 *************/
int UNDO_DELETE::Redo()
{
	OBJECTLIST *cur, *next;
	REDO_SORNODE *node, *nextnode;
	UNDO_SORNODE *node1;

	cur = redoobjects;
	while(cur)
	{
		next = (OBJECTLIST*)cur->GetNext();

		cur->object->DeChainObject();
		cur->object->InitWithoutDown();

		cur->object->LIST::Append((LIST**)&objects);

		delete cur;
		cur = next;
	}
	redoobjects = NULL;

	node = redosornodes;
	while(node)
	{
		nextnode = (REDO_SORNODE*)node->GetNext();

		node1 = new UNDO_SORNODE;
		if(!node1)
			return UNDO_FAILED;

		node1->node = node->sor->RemoveNodeAtPos(&node->p);
		if(!node1->node)
		{
			delete node1;
			return UNDO_FAILED;
		}
		node1->sor = node->sor;
		node1->Append((SLIST**)&sornodes);

		delete node;
		node = nextnode;
	}
	redosornodes = NULL;

	return UNDO_OK_REDRAW;
}

/*************
 * DESCRIPTION:   Get size needed for this undo operation
 * INPUT:         -
 * OUTPUT:        size of undo operation
 *************/
int UNDO_DELETE::GetSize()
{
	int size = 0;
	OBJECTLIST *cur;
	UNDO_SORNODE *unode;
	REDO_SORNODE *rnode;

	if(objects)
		size += objects->GetSizeOfTree();

	cur = redoobjects;
	while(cur)
	{
		size += sizeof(cur);
		cur = (OBJECTLIST*)cur->GetNext();
	}

	unode = sornodes;
	while(unode)
	{
		size += sizeof(unode);
		size += sizeof(unode->node);
		unode = (UNDO_SORNODE*)unode->GetNext();
	}

	rnode = redosornodes;
	while(rnode)
	{
		size += sizeof(rnode);
		rnode = (REDO_SORNODE*)rnode->GetNext();
	}

	size += sizeof(UNDO_DELETE);
	return size;
}

/*************
 * DESCRIPTION:   Constructor of create undo
 * INPUT:         -
 * OUTPUT:        -
 *************/
UNDO_CREATE::UNDO_CREATE()
{
	objects = NULL;
	redoobjects = NULL;
}

/*************
 * DESCRIPTION:   destructor of create undo
 * INPUT:         -
 * OUTPUT:        -
 *************/
UNDO_CREATE::~UNDO_CREATE()
{
	OBJECTLIST *cur, *next;

	cur = objects;
	while(cur)
	{
		next = (OBJECTLIST*)cur->GetNext();
		delete cur;
		cur = next;
	}

	if(redoobjects)
		redoobjects->FreeObjectsList();
}

/*************
 * DESCRIPTION:   add an created object to the object undo list
 * INPUT:         obj      object to add to list
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL UNDO_CREATE::AddCreated(OBJECT *obj)
{
	OBJECTLIST *created;

	created = new OBJECTLIST;
	if(!created)
		return FALSE;

	created->object = obj;

	created->SLIST::Insert((SLIST**)&objects);

	return TRUE;
}

/*************
 * DESCRIPTION:   Undo a created object
 * INPUT:         -
 * OUTPUT:        UNDO_OK_REDRAW
 *************/
int UNDO_CREATE::Undo()
{
	OBJECTLIST *cur;

	cur = objects;
	while(cur)
	{
		cur->object->DeChainObject();
		cur->object->InitWithoutDown();
		cur->object->LIST::Append((LIST**)&redoobjects);
		cur = (OBJECTLIST*)cur->GetNext();
	}
	objects = NULL;
	return UNDO_OK_REDRAW;
}

/*************
 * DESCRIPTION:   Redo a created object
 * INPUT:         -
 * OUTPUT:        result
 *************/
int UNDO_CREATE::Redo()
{
	OBJECT *cur, *next;
	OBJECTLIST *obj;

	cur = redoobjects;
	while(cur)
	{
		next = (OBJECT*)cur->GetNext();

		obj = new OBJECTLIST;
		if(!obj)
			return UNDO_FAILED;
		obj->object = cur;
		obj->SLIST::Insert((SLIST**)&objects);

		cur->InitWithoutDown();
		cur->Append();
		cur = next;
	}
	redoobjects = NULL;

	return UNDO_OK_REDRAW;
}

/*************
 * DESCRIPTION:   Get size needed for this undo operation
 * INPUT:         -
 * OUTPUT:        size of undo operation
 *************/
int UNDO_CREATE::GetSize()
{
	int size;
	OBJECTLIST *cur;

	size = sizeof(UNDO_CREATE);

	if(redoobjects)
		size += redoobjects->GetSizeOfTree();

	cur = objects;
	while(cur)
	{
		size += sizeof(OBJECTLIST);
		cur = (OBJECTLIST*)cur->GetNext();
	}
	return size;
}
