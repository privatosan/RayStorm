/***************
 * PROGRAM:       Modeler
 * NAME:          sor.cpp
 * DESCRIPTION:   Functions for sor object class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    09.04.97 ah    initial release
 *    25.06.97 ah    added AddPoint()
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef ERROR_H
#include "errors.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef SOR_H
#include "sor.h"
#endif

#ifndef IFF_UTIL_H
#include "iff_util.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef MAGIC_H
#include "magic.h"
#endif

#ifndef GFX2D_H
#include "gfx.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef __AMIGA__
#ifndef COLDEF_H
#include "coldef.h"
#endif
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
SOR_NODE::SOR_NODE()
{
	flags = SOR_NODE_RECALC;
}

/*************
 * DESCRIPTION:   limit transformed sor node to EPSILON < x < INFINITY and
 *    prev.y < y < next.y
 * INPUT:         v           value to limit
 *                transform   transformation matrix
 *                size        object scale factor
 * OUTPUT:        none
 *************/
void SOR_NODE::Limit(VECTOR *v, MATRIX *transform, VECTOR *size)
{
	SOR_NODE *node;
	VECTOR u;

	node = (SOR_NODE*)GetPrev();
	if(node)
	{
		SetVector(&u, node->p.x, node->p.y, 0.f);
		if(node->flags & SOR_NODE_NODESELECTED)
			transform->MultVectMat(&u);
		if(v->y*size->y < u.y*size->y + 1e-2)
			v->y = u.y + 1e-2;
	}
	node = (SOR_NODE*)GetNext();
	if(node)
	{
		SetVector(&u, node->p.x, node->p.y, 0.f);
		if(node->flags & SOR_NODE_NODESELECTED)
			transform->MultVectMat(&u);
		if(v->y*size->y > u.y*size->y - 1e-2)
			v->y = u.y - 1e-2;
	}

	if(v->x*size->x < EPSILON)
		v->x = EPSILON;
	v->z = 0.f;
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
SOR::SOR()
{
	VECT2D p;

	SetName("SOR");
	nodes = NULL;

	SetVect2D(&p, 1.f, 2.f);
	AddNode(&p);
	SetVect2D(&p, 1.f, 1.f);
	AddNode(&p);
	SetVect2D(&p, 1.f, 0.f);
	AddNode(&p);
	SetVect2D(&p, 1.f, -1.f);
	AddNode(&p);
	SetVect2D(&p, 1.f, -2.f);
	AddNode(&p);
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
SOR::~SOR()
{
	if(nodes)
		nodes->FreeList();
}

/*************
 * DESCRIPTION:   free all nodes of a sor
 * INPUT:         -
 * OUTPUT:        -
 *************/
void SOR::FreeAllNodes()
{
	if(nodes)
	{
		nodes->FreeList();
		nodes = NULL;
	}
}

/*************
 * DESCRIPTION:   Add a node to node list (the point is sorted in
 *    according to its y position)
 * INPUT:         point    2D point to add
 * OUTPUT:        created node
 *************/
SOR_NODE *SOR::AddNode(VECT2D *point)
{
	SOR_NODE *node;

	node = new SOR_NODE;
	if(!node)
		return NULL;
	node->p = *point;

	AddNode(node);

	return node;
}

/*************
 * DESCRIPTION:   Add a node to node list (the point is sorted in
 *    according to its y position)
 * INPUT:         node     node
 * OUTPUT:        -
 *************/
void SOR::AddNode(SOR_NODE *node)
{
	SOR_NODE *cur, *prev;

	if(nodes)
	{
		// search insert position
		cur = nodes;
		prev = NULL;
		while(cur)
		{
			if(cur->p.y > node->p.y)
				break;

			prev = cur;
			cur = (SOR_NODE*)cur->GetNext();
		}
		if(cur)
		{
			if(!cur->GetNext())
			{
				node->flags |= SOR_NODE_SPLINELAST;
				if(prev)
					prev->flags &= ~SOR_NODE_SPLINELAST;
				else
					cur->flags &= ~SOR_NODE_SPLINELAST;
			}
			if(!prev)
			{
				node->flags |= SOR_NODE_FIRST;
				cur->flags &= ~SOR_NODE_FIRST;
				node->Insert((DLIST**)&nodes);
			}
			else
				node->InsertAfter((DLIST*)prev);
		}
		else
		{
			prev->flags &= ~SOR_NODE_LAST;
			prev->flags |= SOR_NODE_SPLINELAST;
			node->flags |= SOR_NODE_LAST;
			node->InsertAfter((DLIST*)prev);
		}
	}
	else
	{
		node->flags |= SOR_NODE_FIRST | SOR_NODE_LAST | SOR_NODE_SPLINELAST;
		node->Insert((DLIST**)&nodes);
	}
}

/*************
 * DESCRIPTION:   remove a node from the list and keep track of all
 *    the flags
 * INPUT:         node     node to remove
 * OUTPUT:        -
 *************/
void SOR::RemoveNode(SOR_NODE *node)
{
	SOR_NODE *next, *prev, *prevprev;

	next = (SOR_NODE*)node->GetNext();
	if(next)
	{
		next->flags |= SOR_NODE_RECALC;
		next->flags &= ~SOR_NODE_SPLINESELECTED;
	}
	prev = (SOR_NODE*)node->GetPrev();
	if(prev)
	{
		prev->flags |= SOR_NODE_RECALC;
		prev->flags &= ~SOR_NODE_SPLINESELECTED;
		prevprev = (SOR_NODE*)prev->GetPrev();
		if(prevprev)
		{
			prevprev->flags |= SOR_NODE_RECALC;
			prevprev->flags &= ~SOR_NODE_SPLINESELECTED;
		}
	}

	if(node->flags & SOR_NODE_FIRST)
	{
		if(next)
			next->flags |= SOR_NODE_FIRST;
	}

	if(node->flags & SOR_NODE_SPLINELAST)
	{
		if(prev)
			prev->flags |= SOR_NODE_SPLINELAST;
		else
		{
			if(next)
				next->flags |= SOR_NODE_SPLINELAST;
		}
	}

	if(node->flags & SOR_NODE_LAST)
	{
		if(prev)
		{
			prev->flags |= SOR_NODE_LAST;
			if(prevprev)
			{
				prev->flags &= ~SOR_NODE_SPLINELAST;
				prevprev->flags |= SOR_NODE_SPLINELAST;
			}
		}

	}

	node->flags = SOR_NODE_RECALC;
	node->DeChain((DLIST**)&nodes);
}

/*************
 * DESCRIPTION:   Draw the click points of a sor object
 * INPUT:         display
 *                stack
 * OUTPUT:        -
 *************/
void SOR::DrawTrackPoints(DISPLAY *display, MATRIX_STACK *stack)
{
	SOR_NODE *cur;
	VECTOR u;

	cur = nodes;
	while(cur)
	{
		SetVector(&u, cur->p.x*size.x, cur->p.y*size.y, 0.f);
		stack->MultVectStack(&u);
		display->view->MCtoDC_xy(&u);
		display->DrawTrack((int)u.x, (int)u.y);

		cur = (SOR_NODE*)cur->GetNext();
	}
}

/*************
 * DESCRIPTION:   draw a 2D cubic spline
 * INPUT:
 * OUTPUT:        -
 *************/
static void CubicSpline2D(DISPLAY *display, MATRIX *m, SOR_NODE *node, VECT2D *k1, VECT2D *k2, VECT2D *k3, VECT2D *k4, float sz, float sy, float sx)
{
	float w, step;
	float k[4];
	MATRIX mat, mat1;
	double x,y;
	VECTOR u,v;
	SHORT x1,y1, x2,y2;

	if(node->flags & SOR_NODE_RECALC)
	{
		// recalculate coefficents
		k[0] = k2->x * k2->x;
		k[1] = k3->x * k3->x;
		k[2] = (k3->x - k1->x) / (k3->y - k1->y) * 2.f * k2->x;
		k[3] = (k4->x - k2->x) / (k4->y - k2->y) * 2.f * k3->x;

		w = k2->y;

		mat.m[2] = w;
		mat.m[1] = mat.m[2] * w;
		mat.m[0] = mat.m[1] * w;
		mat.m[3] = 1.f;

		mat.m[9] = 2.f * w;
		mat.m[8] = mat.m[9] * 1.5f * w;
		mat.m[10] = 1.f;
		mat.m[11] = 0.f;

		w = k3->y;

		mat.m[6] = w;
		mat.m[5] = mat.m[6] * w;
		mat.m[4] = mat.m[5] * w;
		mat.m[7] = 1.f;

		mat.m[13] = 2.f * w;
		mat.m[12] = mat.m[13] * 1.5f * w;
		mat.m[14] = 1.f;
		mat.m[15] = 0.f;

		mat.InvMat(&mat1);

		// Calculate coefficients of cubic patch
		node->a = k[0] * mat1.m[0] + k[1] * mat1.m[1] + k[2] * mat1.m[2] + k[3] * mat1.m[3];
		node->b = k[0] * mat1.m[4] + k[1] * mat1.m[5] + k[2] * mat1.m[6] + k[3] * mat1.m[7];
		node->c = k[0] * mat1.m[8] + k[1] * mat1.m[9] + k[2] * mat1.m[10] + k[3] * mat1.m[11];
		node->d = k[0] * mat1.m[12] + k[1] * mat1.m[13] + k[2] * mat1.m[14] + k[3] * mat1.m[15];

		node->flags &= ~SOR_NODE_RECALC;
	}

	step = (k3->y - k2->y)/SOR_STEP;
	SetVector(&u, k2->x*sx, k2->y*sy, k2->x*sz);
	display->view->MCtoDC2_xy(m, &u, &x1, &y1);
	for(y = k2->y+step; y < k3->y-step; y+=step)
	{
		x = ((node->a*y + node->b)*y + node->c)*y + node->d;
		if(x < EPSILON)
			x = 0.f;
		else
			x = sqrt(x);
		SetVector(&v, x*sx, y*sy, x*sz);
		display->view->MCtoDC2_xy(m, &v, &x2, &y2);
		gfx.ClipLine(x1, y1, x2, y2);
		display->AccumulateRect2D(x1, y1, x2, y2);
		x1 = x2;
		y1 = y2;
	}
	SetVector(&v, k3->x*sx, k3->y*sy, k3->x*sz);
	display->view->MCtoDC2_xy(m, &v, &x2, &y2);
	gfx.ClipLine(x1, y1, x2, y2);
	display->AccumulateRect2D(x1, y1, x2, y2);
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL SOR::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_SOR);
}

/*************
 * DESCRIPTION:   Draw the sor object while it's edited
 * INPUT:         display
 *                stack
 * OUTPUT:        -
 *************/
void SOR::DrawEdited(DISPLAY *display, MATRIX_STACK *stack)
{
	MATRIX m, m1, m2, m3, transform;
	VECT2D tmp, tmp1, tmp2, tmp3;
	VECTOR u, lpos,lox,loy,loz;
	BOOL sel;
	SHORT x1,y1, x,y;
	SOR_NODE *cur, *next, *next1, *prev;

	if(display->editmode & EDIT_ACTIVE)
	{
		stack->Pop(&m2);
		stack->PopEnd(&m);
		m1.MultMat(stack->GetMatrix(), display->view->GetViewMatrix());
		stack->PushEnd(&m);
		stack->Push(&m2);
		m.MultMat(stack->GetMatrix(), display->view->GetViewMatrix());
	}
	else
	{
		m.MultMat(stack->GetMatrix(), display->view->GetViewMatrix());
		m1 = m;
	}

	if((display->editmode & EDIT_ACTIVE) || (flags & OBJECT_HANDLEDIRTY))
	{
		if(flags & OBJECT_HANDLEDIRTY)
		{
			flags &= ~OBJECT_HANDLEDIRTY;
			return;
		}
		else
		{
			// draw selected nodes and splines
			cur = nodes;
			GetObjectMatrix(&m2);
			m2.GenerateAxis(&lox,&loy,&loz, &lpos);
			transform.SetOMatrix(&lox, &loy, &loz);
			m2.SetSOTMatrix(&display->size, &display->orient_x, &display->orient_y, &display->orient_z, &display->pos);
			m3.MultMat(&transform, &m2);
			transform.SetOMatrix(&lox, &loy, &loz);
			transform.InvMat(&m2);
			transform.MultMat(&m3, &m2);
			while(cur)
			{
				next = (SOR_NODE*)cur->GetNext();
				if(cur->flags & SOR_NODE_NODESELECTED)
				{
					gfx.SetPen(COLOR_SELECTED);
					SetVector(&u, cur->p.x, cur->p.y, 0.f);
					transform.MultVectMat(&u);
					cur->Limit(&u, &transform, &size);
					u.x *= size.x;
					u.y *= size.y;
				}
				else
				{
					gfx.SetPen(COLOR_UNSELECTED);
					SetVector(&u, cur->p.x*size.x, cur->p.y*size.y, 0.f);
				}
				display->view->MCtoDC2_xy(&m1, &u, &x1, &y1);
				display->DrawTrack(x1, y1);
				display->AccumulateRect2D(x1+TRACKWIDTH, y1+TRACKWIDTH, x1-TRACKWIDTH, y1-TRACKWIDTH);

				if(cur->flags & SOR_NODE_SPLINESELECTED)
				{
					cur->flags |= SOR_NODE_RECALC;
					if((cur->flags & SOR_NODE_FIRST) || (cur->flags & SOR_NODE_SPLINELAST))
					{
						if(next->flags & SOR_NODE_NODESELECTED)
						{
							SetVector(&u, next->p.x, next->p.y, 0.f);
							transform.MultVectMat(&u);
							next->Limit(&u, &transform, &size);
							u.x *= size.x;
							u.y *= size.y;
						}
						else
							SetVector(&u, next->p.x*size.x, next->p.y*size.y, 0.f);
						display->view->MCtoDC2_xy(&m1, &u, &x, &y);
						gfx.ClipLine(x, y, x1, y1);
						display->AccumulateRect2D(x, y, x1, y1);
					}
					else
					{
						gfx.SetPen(COLOR_UNSELECTED);
						if(prev->flags & SOR_NODE_NODESELECTED)
						{
							gfx.SetPen(COLOR_SELECTED);
							SetVector(&u, prev->p.x, prev->p.y, 0.f);
							transform.MultVectMat(&u);
							prev->Limit(&u, &transform, &size);
							SetVect2D(&tmp, u.x, u.y);
						}
						else
							SetVect2D(&tmp, prev->p.x, prev->p.y);
						if(cur->flags & SOR_NODE_NODESELECTED)
						{
							gfx.SetPen(COLOR_SELECTED);
							SetVector(&u, cur->p.x, cur->p.y, 0.f);
							transform.MultVectMat(&u);
							cur->Limit(&u, &transform, &size);
							SetVect2D(&tmp1, u.x, u.y);
						}
						else
							SetVect2D(&tmp1, cur->p.x, cur->p.y);
						if(next->flags & SOR_NODE_NODESELECTED)
						{
							gfx.SetPen(COLOR_SELECTED);
							SetVector(&u, next->p.x, next->p.y, 0.f);
							transform.MultVectMat(&u);
							next->Limit(&u, &transform, &size);
							SetVect2D(&tmp2, u.x, u.y);
						}
						else
							SetVect2D(&tmp2, next->p.x, next->p.y);
						next1 = (SOR_NODE*)next->GetNext();
						if(next1->flags & SOR_NODE_NODESELECTED)
						{
							gfx.SetPen(COLOR_SELECTED);
							SetVector(&u, next1->p.x, next1->p.y, 0.f);
							transform.MultVectMat(&u);
							next1->Limit(&u, &transform, &size);
							SetVect2D(&tmp3, u.x, u.y);
						}
						else
							SetVect2D(&tmp3, next1->p.x, next1->p.y);
						CubicSpline2D(display, &m1,
							cur,
							&tmp,
							&tmp1,
							&tmp2,
							&tmp3,
							0.f, size.y, size.x);
					}
				}
				prev = cur;
				cur = next;
			}
		}
	}
	else
	{
		// draw the whole thing
		gfx.SetPen(COLOR_UNSELECTED);
		cur = nodes;
		sel = FALSE;
		while(cur)
		{
			next = (SOR_NODE*)cur->GetNext();
			if(cur->flags & SOR_NODE_NODESELECTED)
			{
				gfx.SetPen(COLOR_SELECTED);
				sel = TRUE;
			}

			SetVector(&u, cur->p.x*size.x, cur->p.y*size.y, 0.f);
			display->view->MCtoDC2_xy(&m, &u, &x1, &y1);
			display->DrawTrack(x1, y1);

			if((cur->flags & SOR_NODE_SPLINESELECTED) && !sel)
			{
				gfx.SetPen(COLOR_SELECTED);
				sel = TRUE;
			}

			if(next)
			{
				if((cur->flags & SOR_NODE_FIRST) || (cur->flags & SOR_NODE_SPLINELAST))
				{
					SetVector(&u, next->p.x*size.x, next->p.y*size.y, 0.f);
					display->view->MCtoDC2_xy(&m, &u, &x, &y);
					gfx.ClipLine(x, y, x1, y1);
				}
				else
				{
					CubicSpline2D(display, &m,
						cur,
						&prev->p,
						&cur->p,
						&next->p,
						&((SOR_NODE*)next->GetNext())->p,
						0.f, size.y, size.x);
				}
			}

			if(sel)
			{
				gfx.SetPen(COLOR_UNSELECTED);
				sel = FALSE;
			}

			prev = cur;
			cur = next;
		}
	}
	flags &= ~OBJECT_HANDLEDIRTY;
}

/*************
 * DESCRIPTION:   Draw a sor
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void SOR::DrawIt(DISPLAY *display, MATRIX_STACK *stack)
{
	VECTOR u,v;
	int j;
	float sinus,cosinus;
	MATRIX m;
	SOR_NODE *cur, *next, *prev;

	switch(display->display)
	{
		case DISPLAY_BBOX:
			// draw a bounding box
			display->DrawBox(stack,&bboxmin,&bboxmax);
			break;
		case DISPLAY_WIRE:
			if(display->view->BBoxInView(&bboxmax, &bboxmin))
			{
				m.MultMat(stack->GetMatrix(), display->view->GetViewMatrix());
				for(j=0; j<=360; j+=SOR_DIVS)
				{
					sinus = sin(j*PI_180)*size.z;
					cosinus = cos(j*PI_180)*size.x;

					prev = nodes;
					cur = (SOR_NODE*)nodes->GetNext();
					while(cur && !(cur->flags & SOR_NODE_SPLINELAST))
					{
						next = (SOR_NODE*)cur->GetNext();

						CubicSpline2D(display, &m,
							cur,
							&prev->p,
							&cur->p,
							&next->p,
							&((SOR_NODE*)next->GetNext())->p,
							sinus, size.y, cosinus);

						prev = cur;
						cur = next;
					}
				}

				prev = nodes;
				cur = (SOR_NODE*)nodes->GetNext();
				while(cur && !(cur->flags & SOR_NODE_LAST))
				{
					SetVector(&u, cur->p.x*size.x, cur->p.y*size.y, 0.f);
					for(j=SOR_DIVS; j<=360; j+=SOR_DIVS)
					{
						// draw caps
						if((!(flags & OBJECT_OPENBOTTOM) && (prev->flags & SOR_NODE_FIRST)) ||
							(!(flags & OBJECT_OPENTOP) && (cur->flags & SOR_NODE_SPLINELAST)))
						{
							SetVector(&v, 0.f, cur->p.y*size.y, 0.f);
							display->TransformDraw(&u, &v, stack);
						}

						SetVector(&v,
							cur->p.x*cos(j*PI_180)*size.x,
							cur->p.y*size.y,
							cur->p.x*sin(j*PI_180)*size.z);
						display->TransformDraw(&u,&v,stack);
						u = v;
					}
					prev = cur;
					cur = (SOR_NODE*)cur->GetNext();
				}
			}
			break;
	}
}

void SOR::Draw(DISPLAY *display, MATRIX_STACK *stack)
{
	VECTOR u,v;
	SOR_NODE *cur;

	if(display->editmode & EDIT_MESH)
	{
		if(!(flags & OBJECT_MESHEDITING))
			flags |= OBJECT_MESHEDITING;

		if(selected)
		{
			DrawEdited(display, stack);
			return;
		}
	}

	if(flags & OBJECT_MESHEDITING)
	{
		if(display->view->viewmode & VIEW_PERSP)
			flags &= ~OBJECT_MESHEDITING;

		gfx.SetPen(COLOR_BKGROUND);
		DrawTrackPoints(display, stack);

		cur = nodes;
		SetVector(&u, cur->p.x*size.x, cur->p.y*size.y, 0.f);
		cur = (SOR_NODE*)cur->GetNext();
		SetVector(&v, cur->p.x*size.x, cur->p.y*size.y, 0.f);
		display->TransformDraw(&u,&v,stack);

		while(!(cur->flags & SOR_NODE_SPLINELAST))
			cur = (SOR_NODE*)cur->GetNext();

		SetVector(&u, cur->p.x*size.x, cur->p.y*size.y, 0.f);
		cur = (SOR_NODE*)cur->GetNext();
		SetVector(&v, cur->p.x*size.x, cur->p.y*size.y, 0.f);
		display->TransformDraw(&u,&v,stack);

		if(selected)
			gfx.SetPen(COLOR_SELECTED);
		else
			gfx.SetPen(COLOR_UNSELECTED);
	}

	DrawIt(display, stack);
}

/*************
 * DESCRIPTION:   calculate bounding box for object
 * INPUT:         none
 * OUTPUT:        none
 *************/
void SOR::CalcBBox()
{
	SOR_NODE *cur;
	float x,y,z;

	if(nodes)
	{
		SetVector(&bboxmax, -INFINITY, -INFINITY, -INFINITY);
		SetVector(&bboxmin,  INFINITY,  INFINITY,  INFINITY);

		cur = (SOR_NODE*)nodes->GetNext();
		if(cur)
		{
			while(cur->GetNext())
			{
				x = cur->p.x*size.x;
				y = cur->p.y*size.y;
				z = cur->p.x*size.z;

				if(x > bboxmax.x)
					bboxmax.x = x;
				if(y > bboxmax.y)
					bboxmax.y = y;
				if(z > bboxmax.z)
					bboxmax.z = z;

				if(-x < bboxmin.x)
					bboxmin.x = -x;
				if(y < bboxmin.y)
					bboxmin.y = y;
				if(-z < bboxmin.z)
					bboxmin.z = -z;

				cur = (SOR_NODE*)cur->GetNext();
			}
		}
	}
	else
	{
		// no points -> make bbox as big as axis
		bboxmax = size;
		SetVector(&bboxmin, -size.x, -size.y, -size.z);
	}
}

/*************
 * DESCRIPTION:   sets the new object specs
 * INPUT:         disp     pointer to display structure
 *                pos      translate factor
 *                ox,oy,oz rotate factor
 *                size     scale factor
 * OUTPUT:        none
 *************/
void SOR::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	SOR_NODE *cur;
	VECTOR v;
	MATRIX m, m1, transform;
	VECTOR local_ox,local_oy,local_oz, local_pos;

	if(nodes)
	{
		if(disp)
		{
			if(disp->editmode & EDIT_MESH)
			{
				GetObjectMatrix(&m);
				m.GenerateAxis(&local_ox,&local_oy,&local_oz, &local_pos);
				transform.SetOMatrix(&local_ox, &local_oy, &local_oz);
				m.SetSOTMatrix(size, ox,oy,oz, pos);
				m1.MultMat(&transform, &m);
				transform.InvMat(&m);
				transform.MultMat(&m1, &m);

				cur = nodes;
				while(cur)
				{
					if(cur->flags & SOR_NODE_NODESELECTED)
					{
						SetVector(&v, cur->p.x, cur->p.y, 0.);
						transform.MultVectMat(&v);
						cur->Limit(&v, &transform, &this->size);
						SetVect2D(&cur->p, v.x, v.y);
					}
					cur = (SOR_NODE*)cur->GetNext();
				}
			}
		}

		CalcBBox();
	}
}

/*************
 * DESCRIPTION:   write sor to scene file
 * INPUT:         iff      iff handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SOR::WriteObject(struct IFFHandle *iff)
{
	VECT2D *points;
	int curnodes;
	SOR_NODE *cur;

	if(PushChunk(iff, ID_SOR, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteCommon(iff))
		return FALSE;

	if(!WriteSurface(iff))
		return FALSE;

	if(nodes)
	{
		// count nodes
		curnodes = 0;
		cur = nodes;
		while(cur)
		{
			curnodes++;
			cur = (SOR_NODE*)cur->GetNext();
		}
		points = new VECT2D[curnodes];
		if(!points)
			return FALSE;

		// copy points
		curnodes = 0;
		cur = nodes;
		while(cur)
		{
			points[curnodes] = cur->p;
			curnodes++;
			cur = (SOR_NODE*)cur->GetNext();
		}

		if(!WriteLongChunk(iff, ID_PNTS, points, curnodes*2))
		{
			delete points;
			return FALSE;
		}
		delete points;
	}

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   transfer sor data to RayStorm Interface
 * INPUT:         stack    matrix stack
 *                object   pointer to created rsi object (only for CSG)
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult SOR::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	void *surface;
	VECTOR orient_x, orient_y, orient_z, pos;
	int flags;
	VECT2D *points;
	int curnodes;
	SOR_NODE *cur;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	err = surf->ToRSI(rc, &surface,stack,(OBJECT*)this);
	if(err)
		return err;

	// count nodes
	curnodes = 0;
	cur = nodes;
	while(cur)
	{
		curnodes++;
		cur = (SOR_NODE*)cur->GetNext();
	}
	points = new VECT2D[curnodes];
	if(!points)
		return rsiERR_MEM;

	// copy points
	curnodes = 0;
	cur = nodes;
	while(cur)
	{
		points[curnodes] = cur->p;
		curnodes++;
		cur = (SOR_NODE*)cur->GetNext();
	}

	err = PPC_STUB(rsiCreateSOR)(CTXT, object, surface, curnodes, (rsiVECTOR2D*)points);
	if(err)
	{
		delete points;
		return err;
	}
	delete points;

	flags = 0;
	if(this->flags & OBJECT_INVERTED)
		flags |= rsiFCSGInverted;
	if(this->flags & OBJECT_OPENTOP)
		flags |= rsiFSOROpenTop;
	if(this->flags & OBJECT_OPENBOTTOM)
		flags |= rsiFSOROpenBottom;
	if(this->flags & OBJECT_SOR_ACCURATE)
		flags |= rsiFSORSturm;

	return PPC_STUB(rsiSetSOR)(CTXT, *object,
		rsiTSORPos,          &pos,
		rsiTSORSize,         &size,
		rsiTSOROrientX,      &orient_x,
		rsiTSOROrientY,      &orient_y,
		rsiTSOROrientZ,      &orient_z,
		rsiTSORFlags,        flags,
		rsiTDone);
}

/*************
 * DESCRIPTION:   parse a sor from a RSCN file
 * INPUT:         iff      iff handle
 *                where    where to insert after
 *                dir      insert direction
 * OUTPUT:        created object
 *************/
OBJECT *ParseSOR(struct IFFHandle *iff, OBJECT *where, int dir)
{
	SOR *sor;
	struct ContextNode *cn;
	long error = 0;
	VECT2D *points;
	int i;

	sor = new SOR;
	if(!sor)
		return NULL;

	if(!sor->ReadAxis(iff))
	{
		delete sor;
		return NULL;
	}

	sor->surf = sciCreateSurface(SURF_DEFAULTNAME);
	if(!sor->surf)
	{
		delete sor;
		return NULL;
	}
	sor->surf = GetSurfaceByName(SURF_DEFAULTNAME);

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete sor;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_SOR))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						if(!ReadSurface(iff, &sor->surf, (OBJECT*)sor))
						{
							delete sor;
							return NULL;
						}
						break;
				}
				break;
			case ID_NAME:
				if(!sor->ReadName(iff))
				{
					delete sor;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!sor->ReadTrack(iff))
				{
					delete sor;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(!sor->ReadFlags(iff))
				{
					delete sor;
					return NULL;
				}
				break;
			case ID_PNTS:
				sor->FreeAllNodes();
				points = new VECT2D[cn->cn_Size>>3];
				if(!points)
				{
					delete sor;
					return NULL;
				}
				if(!ReadFloat(iff, (float*)(points), cn->cn_Size>>2))
				{
					delete points;
					delete sor;
					return NULL;
				}
				for(i = (cn->cn_Size>>3)-1; i >= 0; i--)
				{
					if(!sor->AddNode(&points[i]))
					{
						delete points;
						delete sor;
						return NULL;
					}
				}
				delete points;
				break;
		}
	}

	sor->CalcBBox();
	((OBJECT*)sor)->Insert(where,dir);

	return (OBJECT*)sor;
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *SOR::DupObj()
{
	SOR *dup;
	SOR_NODE *cur, *dupnode;

	dup = new SOR;
	if(!dup)
		return NULL;

	*dup = *this;
	// set to NULL to avoid freeing if something below fails
	dup->nodes = NULL;

	// skip to end of list
	cur = nodes;
	while(cur->GetNext())
		cur = (SOR_NODE*)cur->GetNext();

	// copy nodes
	while(cur)
	{
		dupnode = new SOR_NODE;
		if(!dupnode)
		{
			delete dup;
			return NULL;
		}
		*dupnode = *cur;
		dupnode->Insert((DLIST**)&dup->nodes);
		cur = (SOR_NODE*)cur->GetPrev();
	}

	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   Get the size of a sor
 * INPUT:         -
 * OUTPUT:        size
 *************/
int SOR::GetSize()
{
	int size;
	SOR_NODE *cur;

	size = sizeof(*this) + strlen(GetName()) + 1;

	cur = nodes;
	while(cur)
	{
		size += sizeof(SOR_NODE);
		cur = (SOR_NODE*)cur->GetNext();
	}

	return size;
}

/*************
 * DESCRIPTION:   Test if a point of a sor is selected
 * INPUT:         disp     display
 *                x,y      position to test
 *                multi    multi select on/off
 *                stack    matrix stack
 * OUTPUT:        TRUE if new point selected else FALSE
 *************/
BOOL SOR::TestSelectedPoints(DISPLAY *display, int x, int y, BOOL multi, MATRIX_STACK *stack)
{
	VECTOR v;
	SOR_NODE *cur, *next, *prev=NULL, *prevprev=NULL, *selected=NULL;

	if(nodes)
	{
		cur = nodes;
		while(cur)
		{
			next = (SOR_NODE*)cur->GetNext();
			if(!(cur->flags & SOR_NODE_NODESELECTED))
			{
				SetVector(&v, cur->p.x*size.x, cur->p.y*size.y, 0.);
				stack->MultVectStack(&v);
				display->view->MCtoDC_xy(&v);
				if (x < v.x + TRACKWIDTH && x > v.x - TRACKWIDTH && y > v.y - TRACKWIDTH && y < v.y + TRACKWIDTH)
				{
					cur->flags |= SOR_NODE_NODESELECTED | SOR_NODE_NODEDIRTY;
					if(!(cur->flags & SOR_NODE_LAST))
						cur->flags |= SOR_NODE_SPLINESELECTED | SOR_NODE_SPLINEDIRTY;
					if(next)
					{
						if(next->GetNext())
							next->flags |= SOR_NODE_SPLINESELECTED | SOR_NODE_SPLINEDIRTY;
					}
					if(prev)
						prev->flags |= SOR_NODE_SPLINESELECTED | SOR_NODE_SPLINEDIRTY;
					if(prevprev)
						prevprev->flags |= SOR_NODE_SPLINESELECTED | SOR_NODE_SPLINEDIRTY;
					selected = cur;
				}
			}
			prevprev = prev;
			prev = cur;
			cur = next;
		}

		if(!multi && selected)
		{
			// deselect all nodes
			cur = nodes;
			while(cur)
			{
				if(cur != selected)
				{
					if(cur->flags & SOR_NODE_NODESELECTED)
					{
						cur->flags &= ~SOR_NODE_NODESELECTED;
						cur->flags |= SOR_NODE_NODEDIRTY;
					}
				}
				if((cur->flags & SOR_NODE_SPLINESELECTED) && !(cur->flags & SOR_NODE_SPLINEDIRTY))
				{
					cur->flags &= ~SOR_NODE_SPLINESELECTED;
					cur->flags |= SOR_NODE_SPLINEDIRTY;
				}
				cur = (SOR_NODE*)cur->GetNext();
			}
		}

		if(selected)
			flags |= OBJECT_HANDLEDIRTY;
	}

	return (selected ? TRUE : FALSE);
}

/*************
 * DESCRIPTION:   Delete the selected points of a sor object
 * INPUT:         undo     undo structure
 * OUTPUT:        always TRUE
 *************/
BOOL SOR::DeleteSelectedPoints(UNDO_DELETE *undo)
{
	SOR_NODE *cur;
	int num;

	// count nodes
	cur = nodes;
	num = 0;
	while(cur)
	{
		num++;
		cur = (SOR_NODE*)cur->GetNext();
	}

	cur = nodes;
	while(cur && (num > 4))
	{
		if(cur->flags & SOR_NODE_NODESELECTED)
		{
			RemoveNode(cur);
			undo->AddSORNode(this, cur);
			num--;
		}
		cur = (SOR_NODE*)cur->GetNext();
	}
	flags |= OBJECT_DIRTY;
	return TRUE;
}

/*************
 * DESCRIPTION:   Remove node at a specified position from the list without deleting it
 * INPUT:         pos   position of node
 * OUTPUT:        node found at this position
 *************/
SOR_NODE *SOR::RemoveNodeAtPos(VECT2D *pos)
{
	SOR_NODE *cur;

	cur = nodes;
	while(cur)
	{
		if((cur->p.x == pos->x) && (cur->p.y == pos->y))
		{
			RemoveNode(cur);
			return cur;
		}
		cur = (SOR_NODE*)cur->GetNext();
	}
	return NULL;
}

/*************
 * DESCRIPTION:   Add a point to the sor object
 * INPUT:         pos      position of cursor (3D)
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SOR::AddPoint(VECTOR *pos)
{
	VECT2D point;
	SOR_NODE *node;

	node = nodes;
	while(node)
	{
		if(node->flags & SOR_NODE_NODESELECTED)
			break;
		node = (SOR_NODE*)node->GetNext();
	}

	if(!node)
	{
		// add before first node
		point.x = nodes->p.x;
		point.y = nodes->p.y-1.;
	}
	else
	{
		point.x = node->p.x;
		point.y = node->p.y;
		node = (SOR_NODE*)node->GetNext();
		if(node)
		{
			point.x = (point.x + node->p.x) * .5;
			point.y = (point.y + node->p.y) * .5;
		}
		else
		{
			point.y += 1.;
		}
	}
	node = AddNode(&point);
	if(!node)
		return FALSE;

	node->flags |= SOR_NODE_NODESELECTED;
	if(!(node->flags & SOR_NODE_LAST))
		node->flags |= SOR_NODE_SPLINESELECTED;
	else
		((SOR_NODE*)node->GetPrev())->flags |= SOR_NODE_SPLINESELECTED;

	flags |= OBJECT_DIRTY;
	return TRUE;
}
