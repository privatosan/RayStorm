/***************
 * PROGRAM:       Modeler
 * NAME:          csg.cpp
 * DESCRIPTION:   Functions for csg object class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    04.04.97 ah    initial release
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef CSG_H
#include "csg.h"
#endif

#ifndef VIEW_H
#include "view.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

/*************
 * DESCRIPTION:   Constructor of csg (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
CSG::CSG()
{
	SetName("CSG");
	operation = CSG_UNION;
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL CSG::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_CSG);
}

/*************
 * DESCRIPTION:   Draw a csg
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void CSG::Draw(DISPLAY *disp,MATRIX_STACK *stack)
{
}

/*************
 * DESCRIPTION:   sets the new object specs
 * INPUT:         disp     pointer to display structure
 *                pos      translate factor
 *                ox,oy,oz rotate factor
 *                size     scale factor
 * OUTPUT:        none
 *************/
void CSG::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
}

/*************
 * DESCRIPTION:   write csg to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL CSG::WriteObject(struct IFFHandle *iff)
{
	UWORD oper;

	if(PushChunk(iff, ID_CSG, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteCommon(iff))
		return FALSE;

	switch(operation)
	{
		case CSG_UNION:
			oper = RSCN_CSG_UNION;
			break;
		case CSG_INTERSECTION:
			oper = RSCN_CSG_INTERSECTION;
			break;
		case CSG_DIFFERENCE:
			oper = RSCN_CSG_DIFFERENCE;
			break;
	}
	if(!WriteWordChunk(iff, ID_OPER, &oper, 1))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a csg object from a RSCN file
 * INPUT:         iff      iff handler
 *                where    where to insert after
 *                dir      insert direction
 * OUTPUT:        created object
 *************/
OBJECT *ParseCSG(struct IFFHandle *iff, OBJECT *where, int dir)
{
	CSG *csg;
	UWORD oper;
	struct ContextNode *cn;
	long error = 0;

	csg = new CSG;
	if(!csg)
		return NULL;

	if(!csg->ReadAxis(iff))
	{
		delete csg;
		return NULL;
	}

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete csg;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_CSG))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_NAME:
				if(!csg->ReadName(iff))
				{
					delete csg;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!csg->ReadTrack(iff))
				{
					delete csg;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(!csg->ReadFlags(iff))
				{
					delete csg;
					return NULL;
				}
				break;
			case ID_OPER:
				if(!ReadWord(iff,(WORD*)&oper,1))
				{
					delete csg;
					return NULL;
				}
				switch(oper)
				{
					case RSCN_CSG_UNION:
						csg->operation = CSG_UNION;
						break;
					case RSCN_CSG_INTERSECTION:
						csg->operation = CSG_INTERSECTION;
						break;
					case RSCN_CSG_DIFFERENCE:
						csg->operation = CSG_DIFFERENCE;
						break;
				}
				break;
		}
	}

	csg->Insert(where, dir);

	return (OBJECT*)csg;
}

/*************
 * DESCRIPTION:   transfer csg object data to RayStorm Interface
 * INPUT:         stack    matrix stack
 *                object   pointer to created rsi object
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult CSG::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	int op, flags;
	OBJECT *obj;
	void *csgobj, *csg, *root;
	MATRIX m;

	err = PPC_STUB(rsiCreateCSG)(CTXT, &csg);
	if(err)
		return err;

	root = csg;

	switch(operation)
	{
		case CSG_UNION:
			op = rsiFCSGUnion;
			break;
		case CSG_INTERSECTION:
			op = rsiFCSGIntersection;
			break;
		case CSG_DIFFERENCE:
			op = rsiFCSGDifference;
			break;
	}

	flags = 0;
	if(this->flags & OBJECT_INVERTED)
		flags = rsiFCSGInverted;

	err = PPC_STUB(rsiSetCSG)(CTXT, csg,
		rsiTCSGOperator,     op,
		rsiTCSGFlags,        flags,
		rsiTDone);
	if(err)
		return err;

	obj = (OBJECT*)this->GoDown();
	if(!obj)
		return rsiERR_CSG;

	if(!obj->CSGAble())
		return rsiERR_NOTCSGABLE;

	do
	{
		// create first object (maybe another CSG object)
		m.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z,&obj->pos);
		stack->Push(&m);

		err = obj->ToRSI(rc, stack, &csgobj);
		if(err)
			return err;

		stack->Pop();

		err = PPC_STUB(rsiSetCSG)(CTXT, csg,
			rsiTCSGObject1,      csgobj,
			rsiTDone);
		if(err)
			return err;

		obj = (OBJECT*)obj->GetNext();
		if(!obj)
			return rsiERR_CSG;

		if(!obj->CSGAble())
			return rsiERR_NOTCSGABLE;

		// if there is more than one object following -> create a new CSG object
		// because each CSG object can only handle two CSG able objects
		if(obj->GetNext())
		{
			err = PPC_STUB(rsiCreateCSG)(CTXT, &csgobj);
			if(err)
				return err;

			err = PPC_STUB(rsiSetCSG)(CTXT, csgobj,
				rsiTCSGOperator,     op,
				rsiTCSGFlags,        flags,
				rsiTDone);
			if(err)
				return err;

			err = PPC_STUB(rsiSetCSG)(CTXT, csg,
				rsiTCSGObject2,      csgobj,
				rsiTDone);
			if(err)
				return err;

			csg = csgobj;
		}
		else
		{
			// create second object
			m.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z,&obj->pos);
			stack->Push(&m);

			err = obj->ToRSI(rc, stack, &csgobj);
			if(err)
				return err;

			stack->Pop();

			err = PPC_STUB(rsiSetCSG)(CTXT, csg,
				rsiTCSGObject2,      csgobj,
				rsiTDone);
			if(err)
				return err;

			// no more objects following
			obj = NULL;
		}
	}
	while(obj);

	*object = root;
	return rsiERR_NONE;
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *CSG::DupObj()
{
	CSG *dup;

	dup = new CSG;
	if(!dup)
		return NULL;
	*dup = *this;
	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   Get the size of a csg
 * INPUT:         -
 * OUTPUT:        size
 *************/
int CSG::GetSize()
{
	return sizeof(*this) + strlen(GetName()) + 1;
}

/*************
 * DESCRIPTION:   calculate bounding box for object
 * INPUT:         none
 * OUTPUT:        none
 *************/
void CSG::CalcBBox()
{
	SetVector(&bboxmin, -size.x, -size.y, -size.z);
	bboxmax = size;
}
