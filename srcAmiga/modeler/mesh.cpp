/***************
 * PROGRAM:       Modeler
 * NAME:          mesh.cpp
 * DESCRIPTION:   Functions for mesh object class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    14.03.96 ah    initial release
 *    16.10.96 ah    improved speed of drawing function by precalculating the points
 *    18.12.96 ah    added GetSize()
 *    25.02.97 ah    made mesh editable
 *    04.03.97 ah    new mesh normal calculation routine
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

#ifndef MESH_H
#include "mesh.h"
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

#ifndef GFX_H
#include "gfx.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef MOUSE_H
#include "mouse.h"
#endif

#ifndef OPENGL_UTIL_H
#include "ogl_util.h"
#endif

#ifdef __AMIGA__
static struct NewMenu popupmenu[] =
{
	{ NM_TITLE, NULL              , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "Move"            , 0 ,0,0,(APTR)POP_OBJECT_MOVE},
	{ NM_ITEM , "Rotate"          , 0 ,0,0,(APTR)POP_OBJECT_ROTATE},
	{ NM_ITEM , "Scale"           , 0 ,0,0,(APTR)POP_OBJECT_SCALE},
	{ NM_ITEM , "Settings..."     , 0 ,0,0,(APTR)POP_SETTINGS},
	{ NM_ITEM , "Attributes..."   , 0 ,0,0,(APTR)POP_ATTRIBUTE},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};

static struct NewMenu popupmenu_external[] =
{
	{ NM_TITLE, NULL              , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "Move"            , 0 ,0,0,(APTR)POP_OBJECT_MOVE},
	{ NM_ITEM , "Rotate"          , 0 ,0,0,(APTR)POP_OBJECT_ROTATE},
	{ NM_ITEM , "Scale"           , 0 ,0,0,(APTR)POP_OBJECT_SCALE},
	{ NM_ITEM , "Settings..."     , 0 ,0,0,(APTR)POP_SETTINGS},
	{ NM_ITEM , "Attributes..."   , 0 ,0,0,(APTR)POP_ATTRIBUTE},
	{ NM_ITEM , "To scene"        , 0 ,0,0,(APTR)POP_OBJECT2SCENE},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};
#else
	#ifndef COLDEF_H
	#include "coldef.h"
	#endif
#endif

#define PUDDLESIZE 256     // size of puddle for variable array

#define POINT_UNSELECTED   0
#define POINT_SELECTED     1
#define POINT_DIRTY        2

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
MESH::MESH()
{
	SetName("Mesh");
	file = NULL;
// msurfs = NULL;
	maxpoints = 0;
	actpoints = 0;
	points = NULL;
	selpoints = NULL;
	maxedges = 0;
	actedges = 0;
	edges = NULL;
#ifdef NORMAL
	maxnormals = 0;
	curnormals = 0;
	normals = NULL;
#endif // NORMAL
	maxtrias = 0;
	acttrias = 0;
	trias = NULL;
	selflag = SELFLAG_NONE;
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
MESH::~MESH()
{
// MESHSURF *next, *prev;

	if(file)
		delete file;

	if(points)
		delete points;
	if(selpoints)
		delete selpoints;
	if(edges)
		delete edges;
#ifdef NORMAL
	if(normals)
		delete normals;
#endif // NORMAL
	if(trias)
		delete trias;

/* if(msurfs)
	{
		prev = msurfs;
		do
		{
			next = (MESHSURF*)prev->GetNext();
			if(prev->surf)
				delete prev->surf;
			delete prev;
			prev = next;
		}
		while(next);
	}*/
}

/*************
 * DESCRIPTION:   gets the popuplist for the selected object
 * INPUT:         -
 * OUTPUT:        popup list
 *************/
#ifdef __AMIGA__
struct NewMenu *MESH::GetPopUpList()
{
	if(external == EXTERNAL_ROOT)
		return popupmenu_external;
	else
		return popupmenu;
}
#endif

/*************
 * DESCRIPTION:   Insert surface in mesh surface list
 * INPUT:         msruf    surface to insert
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL MESH::Insert(SURFACE *surf)
{
	ASSERT(FALSE);
/*   MESHSURF *msurf;

	msurf = new MESHSURF;
	if(!msurf)
		return FALSE;
	msurf->surf = surf;*/

// msurf->SLIST::Insert((SLIST**)&this->msurfs);
	return TRUE;
}

/*************
 * DESCRIPTION:   Set filename for external objects
 * INPUT:         name     filename
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL MESH::SetFileName(char *name)
{
	if(file)
		delete [ ] file;

	file = new char[strlen(name)+1];
	if(!file)
		return FALSE;

	strcpy(file, name);
	return TRUE;
}

/*************
 * DESCRIPTION:   Join a mesh to another
 * INPUT:         mesh     mesh to add
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL MESH::Join(MESH *mesh)
{
	int pointbase, edgebase, trianglebase, i;
	VECTOR *point_from, *point_to;
	EDGE *edge_from, *edge_to;
	TRIANGLE *triangle_from, *triangle_to;
	MATRIX matrix, curmatrix,joinmatrix;
#ifdef NORMAL
	int normalbase;
	VECTOR *normal_from, *normal_to;
#endif // NORMAL

	// mesh to add has no points, I have nothing to do
	if(!mesh->actpoints)
		return TRUE;

	// add points to mesh
	pointbase = AddPoint(mesh->actpoints);
	if(pointbase == -1)
		return FALSE;
	// we have to transform the points from the local coordinate system of the
	// mesh to join to the coordinate system of the mesh to join to
	GetObjectMatrix(&matrix);
	matrix.InvMat(&curmatrix);
	mesh->GetObjectMatrix(&joinmatrix);
	matrix.MultMat(&joinmatrix, &curmatrix);
	point_from = mesh->points;
	point_to = &points[pointbase];
	for(i = 0; i < mesh->actpoints; i++)
	{
		*point_to = *point_from;
		matrix.MultVectMat(point_to);
		point_to++;
		point_from++;
	}
	CalcBBox();

	// mesh to add has no edges
	if(!mesh->actedges)
		return TRUE;

	// add edges to mesh, we have to add the points base value to the added edges
	edgebase = AddEdge(mesh->actedges);
	if(edgebase == -1)
		return FALSE;
	edge_from = mesh->edges;
	edge_to = &edges[edgebase];
	for(i = 0; i < mesh->actedges; i++)
	{
		edge_to->p[0] = edge_from->p[0] + pointbase;
		edge_to->p[1] = edge_from->p[1] + pointbase;
		edge_to++;
		edge_from++;
	}

#ifdef NORMAL
	// mesh to add has no normals
	if(!mesh->curnormals)
		return TRUE;

	// add normals to mesh
	normalbase = AddNormal(mesh->curnormals);
	if(normalbase == -1)
		return FALSE;
	normal_from = mesh->normals;
	normal_to = &normals[normalbase];
	for(i = 0; i < mesh->curnormals; i++)
	{
		*normal_to = *normal_from;
		matrix.MultDirMat(normal_to);
		normal_to++;
		normal_from++;
	}
#endif // NORMAL

	// mesh to add has no triangles
	if(!mesh->acttrias)
		return TRUE;

	// add triangles to mesh, we have to add the edge base value to the added triangle edge values and
	// the normal base value to the added normals
	trianglebase = AddTria(mesh->acttrias);
	if(trianglebase == -1)
		return FALSE;
	triangle_from = mesh->trias;
	triangle_to = &trias[trianglebase];
	for(i = 0; i < mesh->acttrias; i++)
	{
		triangle_to->e[0] = triangle_from->e[0] + edgebase;
		triangle_to->e[1] = triangle_from->e[1] + edgebase;
		triangle_to->e[2] = triangle_from->e[2] + edgebase;
		triangle_to++;
		triangle_from++;
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   Add points to mesh point array. If array is to small make
 *    it bigger.
 * INPUT:         amount      amount of points to allocate
 * OUTPUT:        -1 if failed else array position of first new point
 *************/
int MESH::AddPoint(int amount)
{
	VECTOR *tmppoints;
	UBYTE *tmpselpoints;
	int n;

	if(!points)
	{
		// no points yet -> allocate first puddle
		points = new VECTOR[amount];
		if(!points)
			return -1;
		maxpoints = amount;
		actpoints = amount;
		return 0;
	}
	else
	{
		if(actpoints+amount >= maxpoints)
		{
			n = (amount/PUDDLESIZE+1)*PUDDLESIZE;
			// have to allocte more memory
			tmppoints = new VECTOR[maxpoints+n];
			if(!tmppoints)
				return -1;
			memcpy(tmppoints,points,sizeof(VECTOR)*maxpoints);
			delete points;
			points = tmppoints;

			if(selpoints)
			{
				tmpselpoints = new UBYTE[maxpoints+n];
				if(!tmpselpoints)
					return -1;
				memcpy(tmpselpoints,selpoints,sizeof(UBYTE)*maxpoints);
				delete selpoints;
				selpoints = tmpselpoints;
				memset(&selpoints[maxpoints], 0, n);
			}

			maxpoints += n;
		}
		actpoints += amount;
		return actpoints-amount;
	}
}

/*************
 * DESCRIPTION:   Remove a point from the point array
 * INPUT:         point    point number
 * OUTPUT:        -
 *************/
void MESH::DeletePoint(UWORD point)
{
	actpoints--;
	if(point < actpoints)
		memcpy(&points[point], &points[point+1], (actpoints-point)*sizeof(VECTOR));
}

/*************
 * DESCRIPTION:   Add edge to mesh edge array. If array is to small make
 *    it bigger.
 * INPUT:         amount      amount of edges to allocate
 * OUTPUT:        -1 if failed else array position of new edge
 *************/
int MESH::AddEdge(int amount)
{
	EDGE *tmpedges;
	int n;
	int i;

	if(!edges)
	{
		// no edges yet -> allocate first puddle
		edges = new EDGE[amount];
		if(!edges)
			return -1;
		maxedges = amount;
		for(i=0; i<amount; i++)
			edges[i].flags = 0;
		actedges = amount;
		return 0;
	}
	else
	{
		if(actedges+amount >= maxedges)
		{
			n = (amount/PUDDLESIZE+1)*PUDDLESIZE;
			// have to allocte more memory
			tmpedges = new EDGE[maxedges+n];
			if(!tmpedges)
				return -1;
			memcpy(tmpedges,edges,sizeof(EDGE)*maxedges);
			delete edges;
			edges = tmpedges;
			for(i=maxedges; i<maxedges+amount; i++)
				edges[i].flags = 0;
			maxedges += n;
		}
		actedges += amount;
		return actedges-amount;
	}
}

/*************
 * DESCRIPTION:   Remove an edge from the edge array
 * INPUT:         edge     edge number
 * OUTPUT:        -
 *************/
void MESH::DeleteEdge(UWORD edge)
{
	actedges--;
	if(edge < actedges)
		memcpy(&edges[edge], &edges[edge+1], (actedges-edge)*sizeof(EDGE));
}

#ifdef NORMAL
/*************
 * DESCRIPTION:   Add normal to mesh normal array. If array is to small make
 *    it bigger.
 * INPUT:         amount      amount of normal to allocate
 * OUTPUT:        -1 if failed else array position of new normal
 *************/
int MESH::AddNormal(int amount)
{
	VECTOR *tmpnormals;
	int n;

	if(!normals)
	{
		// no edges yet -> allocate first puddle
		normals = new VECTOR[amount];
		if(!normals)
			return -1;
		maxnormals = amount;
		curnormals = amount;
		return 0;
	}
	else
	{
		if(curnormals+amount >= maxnormals)
		{
			n = (amount/PUDDLESIZE+1)*PUDDLESIZE;
			// have to allocte more memory
			tmpnormals = new VECTOR[maxnormals+n];
			if(!tmpnormals)
				return -1;
			memcpy(tmpnormals, normals, sizeof(VECTOR)*maxnormals);
			delete normals;
			normals = tmpnormals;
			maxnormals += n;
		}
		curnormals += amount;
		return curnormals-amount;
	}
}

/*************
 * DESCRIPTION:   Remove an normal from the normal array
 * INPUT:         edge     edge number
 * OUTPUT:        -
 *************/
void MESH::DeleteNormal(UWORD normal)
{
	curnormals--;
	if(normal < curnormals)
		memcpy(&normals[normal], &normals[normal+1], (curnormals-normal)*sizeof(VECTOR));
}
#endif // NORMAL

/*************
 * DESCRIPTION:   Add triangles to mesh triangle array. If array is to small
 *    make it bigger.
 * INPUT:         amount      amount of triangles to allocate
 * OUTPUT:        -1 if failed else array position of first new triangle
 *************/
int MESH::AddTria(int amount)
{
	TRIANGLE *tmptrias;
	int n;
// int i;

	if(!trias)
	{
		// no trias yet -> allocate first puddle
		trias = new TRIANGLE[amount];
		if(!trias)
			return -1;
		maxtrias = amount;
/*    for(i=0; i<amount; i++)
			trias[i].surf = NULL;*/
		acttrias = amount;
		return 0;
	}
	else
	{
		if(acttrias+amount >= maxtrias)
		{
			n = (amount/PUDDLESIZE+1)*PUDDLESIZE;
			// have to allocte more memory
			tmptrias = new TRIANGLE[maxtrias+n];
			if(!tmptrias)
				return -1;
			memcpy(tmptrias,trias,sizeof(TRIANGLE)*maxtrias);
			delete trias;
			trias = tmptrias;
/*       for(i=maxtrias; i<maxtrias+amount; i++)
				trias[i].surf = NULL;*/
			maxtrias += n;
		}
		acttrias += amount;
		return acttrias-amount;
	}
}

/*************
 * DESCRIPTION:   Remove a triangle from the triangle array
 * INPUT:         tria     triangle number
 * OUTPUT:        -
 *************/
void MESH::DeleteTria(UWORD tria)
{
	acttrias--;
	if(tria < acttrias)
		memcpy(&trias[tria], &trias[tria+1], (acttrias-tria)*sizeof(TRIANGLE));
}

/*************
 * DESCRIPTION:   Draw the click points of a mesh object
 * INPUT:         display
 *                stack
 * OUTPUT:        -
 *************/
void MESH::DrawTrackPoints(DISPLAY *display, MATRIX_STACK *stack)
{
	VECTOR *p, u;
	UBYTE *sp;
	int i;

	p = points;
	if(selpoints)
	{
		sp = selpoints;
		for(i=0; i<actpoints; i++)
		{
			if(*sp)
				gfx.SetPen(COLOR_SELECTED);

			u = *p;
			stack->MultVectStack(&u);
			display->view->MCtoDC_xy(&u);
			display->DrawTrack((int)u.x, (int)u.y);

			if(*sp)
				gfx.SetPen(COLOR_UNSELECTED);

			p++;
			sp++;
		}
	}
	else
	{
		for(i=0; i<actpoints; i++)
		{
			u = *p;
			stack->MultVectStack(&u);
			display->view->MCtoDC_xy(&u);
			display->DrawTrack((int)u.x, (int)u.y);
			p++;
		}
	}
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL MESH::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_MESH);
}

/*************
 * DESCRIPTION:   Draw the mesh object while it's edited
 * INPUT:         display
 *                stack
 * OUTPUT:        -
 *************/
void MESH::DrawEdited(DISPLAY *display, MATRIX_STACK *stack)
{
	MATRIX m, m1, m2;
	EDGE *e;
	int i;
	SHORT x1,y1, x2,y2;
	UBYTE *sp, sel0, sel1;
	VECTOR *p;

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

	if(flags & OBJECT_HANDLEDIRTY)
	{
		// draw dirty points and lines which connect them
		e = edges;
		for(i=0; i<actedges; i++)
		{
			sel0 = selpoints[e->p[0]];
			sel1 = selpoints[e->p[1]];
			if((sel0 & POINT_DIRTY) || (sel1 & POINT_DIRTY))
			{
				if(!(sel0 & POINT_SELECTED) && !(sel1 & POINT_SELECTED))
					gfx.SetPen(COLOR_UNSELECTED);
				else
					gfx.SetPen(COLOR_BKGROUND);
				display->view->MCtoDC2_xy(&m1, &points[e->p[0]], &x1, &y1);
				display->view->MCtoDC2_xy(&m1, &points[e->p[1]], &x2, &y2);
				gfx.ClipLine(x1,y1, x2,y2);
				if(sel0 & POINT_DIRTY)
					display->DrawTrack(x1, y1);
				if(sel1 & POINT_DIRTY)
					display->DrawTrack(x2, y2);
			}
			e++;
		}
		flags |= OBJECT_HANDLEDIRTY1;
		flags &= ~OBJECT_HANDLEDIRTY;
		return;
	}

	if(flags & OBJECT_HANDLEDIRTY1)
	{
		// draw selected and dirty points and lines
		gfx.SetPen(COLOR_SELECTED);
		e = edges;
		for(i=0; i<actedges; i++)
		{
			sel0 = selpoints[e->p[0]];
			sel1 = selpoints[e->p[1]];
			if((sel0 == (POINT_SELECTED | POINT_DIRTY)) || (sel1 == (POINT_SELECTED | POINT_DIRTY)))
			{
				display->view->MCtoDC2_xy(&m1, &points[e->p[0]], &x1, &y1);
				display->view->MCtoDC2_xy(&m1, &points[e->p[1]], &x2, &y2);
				gfx.ClipLine(x1,y1, x2,y2);
				if(sel0 & POINT_SELECTED)
					display->DrawTrack(x1, y1);
				if(sel1 & POINT_SELECTED)
					display->DrawTrack(x2, y2);
			}
			e++;
		}
		sp = selpoints;
		for(i=0; i<actpoints; i++)
		{
			*sp &= ~POINT_DIRTY;
			sp++;
		}
		flags &= ~OBJECT_HANDLEDIRTY1;
		return;
	}

	if(display->editmode & EDIT_ACTIVE)
	{
		// editing and changing active: draw selected points and lines
		gfx.SetPen(COLOR_SELECTED);
		e = edges;
		for(i=0; i<actedges; i++)
		{
			sel0 = selpoints[e->p[0]];
			sel1 = selpoints[e->p[1]];
			if(sel0 || sel1)
			{
				if(sel0)
					display->view->MCtoDC2_xy(&m, &points[e->p[0]], &x1, &y1);
				else
					display->view->MCtoDC2_xy(&m1, &points[e->p[0]], &x1, &y1);

				if(sel1)
					display->view->MCtoDC2_xy(&m, &points[e->p[1]], &x2, &y2);
				else
					display->view->MCtoDC2_xy(&m1, &points[e->p[1]], &x2, &y2);

				gfx.ClipLine(x1,y1, x2,y2);

				if(sel0)
					display->DrawTrack(x1, y1);
				if(sel1)
					display->DrawTrack(x2, y2);
			}
			e++;
		}
	}
	else
	{
		// editing and not active: redraw, draw all lines and points
		gfx.SetPen(COLOR_UNSELECTED);
		e = edges;
		for(i=0; i<actedges; i++)
		{
			display->view->MCtoDC2_xy(&m, &points[e->p[0]], &x1, &y1);
			display->view->MCtoDC2_xy(&m, &points[e->p[1]], &x2, &y2);

			if(e->flags & EDGE_SELECTED)
			{
				gfx.SetPen(COLOR_SELECTED);
				gfx.ClipLine(x1,y1, x2,y2);
				gfx.SetPen(COLOR_UNSELECTED);
			}
			else
			{
				if((selpoints[e->p[0]] || selpoints[e->p[1]]) && (display->editmode & EDIT_EDITPOINTS))
				{
					gfx.SetPen(COLOR_SELECTED);
					gfx.ClipLine(x1,y1, x2,y2);
					gfx.SetPen(COLOR_UNSELECTED);
				}
				else
					gfx.ClipLine(x1,y1, x2,y2);
			}

			e++;
		}
		p = points;
		sp = selpoints;
		for(i=0; i<actpoints; i++)
		{
			display->view->MCtoDC2_xy(&m, p, &x1, &y1);

			if(*sp)
			{
				gfx.SetPen(COLOR_SELECTED);
				display->DrawTrack(x1, y1);
				gfx.SetPen(COLOR_UNSELECTED);
			}
			else
			{
				display->DrawTrack(x1, y1);
			}

			p++;
			sp++;
		}
	}
}

/*************
 * DESCRIPTION:   Draw a mesh
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void MESH::Draw(DISPLAY *display, MATRIX_STACK *stack)
{
	SHORT *px, *py, *hpx, *hpy, x1,y1, x2,y2;
	VECTOR u,v, *p;
	int i, i1, i2;
	EDGE *e;
	MATRIX m;
	BOOL done;

	if(selpoints)
	{
		// any selected points -> editing
		DrawEdited(display, stack);
		return;
	}

	if(selected && !(display->editmode & EDIT_ACTIVE) && (display->editmode & EDIT_MESH) && (external == EXTERNAL_NONE))
	{
		gfx.SetPen(COLOR_UNSELECTED);
		DrawTrackPoints(display, stack);
	}

	if(flags & OBJECT_HANDLEDIRTY)
	{
		flags &= ~OBJECT_HANDLEDIRTY;
		gfx.SetPen(COLOR_BKGROUND);
		DrawTrackPoints(display, stack);
		if(selected)
			gfx.SetPen(COLOR_SELECTED);
		else
			gfx.SetPen(COLOR_UNSELECTED);
	}

	switch(display->display)
	{
#ifdef __OPENGL__
		case DISPLAY_SOLID:
#ifdef NORMAL
			OpenGLDrawMesh(trias, edges, points, normals, acttrias);
#else // NORMAL
			OpenGLDrawMesh(trias, edges, points, NULL, acttrias);
#endif // NORMAL
			break;
#endif // __OPENGL__
		case DISPLAY_BBOX:
			// draw a bounding box
			display->DrawBox(stack,&bboxmin,&bboxmax);
			break;
		case DISPLAY_WIRE:
			if(edges)
			{
				if((display->view->viewmode == VIEW_PERSP) || (display->view->viewmode == VIEW_CAMERA))
				{
					done = TRUE;
					if(display->view->BBoxInView(&bboxmax, &bboxmin))
					{
						m.MultMat(stack->GetMatrix(), display->view->GetViewMatrix());

						px = new SHORT[actpoints];
						py = new SHORT[actpoints];
						if (px && py)
						{
							hpx = px;
							hpy = py;
							p = points;
							for (i = 0; i < actpoints; i++)
							{
								display->view->MCtoDC2_xy(&m, p, hpx, hpy);
								p++;
								hpx++;
								hpy++;
							}

							if(display->CompletelyInScreen(stack, &bboxmax, &bboxmin))
							{
								e = edges;
								for (i = 0; i < actedges; i++)
								{
									i1 = e->p[0];
									i2 = e->p[1];
									gfx.Line(px[i1], py[i1], px[i2], py[i2]);
									e++;
								}
							}
							else
							{
								e = edges;
								for (i = 0; i < actedges; i++)
								{
									i1 = e->p[0];
									i2 = e->p[1];
									gfx.ClipLine(px[i1], py[i1], px[i2], py[i2]);
									e++;
								}
							}
						}
						else
							done = FALSE;

						if (px)
							delete [ ] px;
						if (py)
							delete [ ] py;
					}
					if(!done)
					{
						p = new VECTOR[actpoints];
						if(p)
						{
							for(i=0; i<actpoints; i++)
							{
								p[i] = points[i];
								stack->MultVectStack(&p[i]);
							}

							for(i=0; i<actedges; i++)
							{
								u = p[e->p[0]];
								v = p[e->p[1]];
								if(display->view->Clip3DLine(&u,&v))
								{
									display->view->MCtoDC_xy(&u);
									display->view->MCtoDC_xy(&v);
									gfx.ClipLine((int)u.x, (int)u.y, (int)v.x, (int)v.y);
								}
								e++;
							}

							delete [ ] p;
						}
						else
						{
							for(i=0; i<actedges; i++)
							{
								u = points[e->p[0]];
								stack->MultVectStack(&u);

								v = points[e->p[1]];
								stack->MultVectStack(&v);

								if(display->view->Clip3DLine(&u,&v))
								{
									display->view->MCtoDC_xy(&u);
									display->view->MCtoDC_xy(&v);
									gfx.ClipLine((int)u.x, (int)u.y, (int)v.x, (int)v.y);
								}
								e++;
							}
						}
					}
				}
				else
				{
					// we can use an optimized version of MCtoDC because in ortho view the matrix is alway homogen
					e = edges;

					px = new SHORT[actpoints];
					py = new SHORT[actpoints];

					m.MultMat(stack->GetMatrix(), display->view->GetViewMatrix());

					if(px && py)
					{
						hpx = px;
						hpy = py;
						p = points;
						for(i=0; i<actpoints; i++)
						{
							display->view->MCtoDC2_xy_ortho(&m, p, hpx, hpy);
							p++;
							hpx++;
							hpy++;
						}

						if(display->CompletelyInScreen(stack, &bboxmax, &bboxmin))
						{
							for(i=0; i<actedges; i++)
							{
								i1 = e->p[0];
								i2 = e->p[1];
								gfx.Line(px[i1], py[i1], px[i2], py[i2]);
								e++;
							}
						}
						else
						{
							for(i=0; i<actedges; i++)
							{
								i1 = e->p[0];
								i2 = e->p[1];
								gfx.ClipLine(px[i1], py[i1], px[i2], py[i2]);
								e++;
							}
						}
					}
					else
					{
						if(display->CompletelyInScreen(stack, &bboxmax, &bboxmin))
						{
							for(i=0; i<actedges; i++)
							{
								display->view->MCtoDC2_xy_ortho(&m, &points[e->p[0]], &x1, &y1);
								display->view->MCtoDC2_xy_ortho(&m, &points[e->p[1]], &x2, &y2);

								gfx.Line(x1,y1, x2,y2);
								e++;
							}
						}
						else
						{
							for(i=0; i<actedges; i++)
							{
								display->view->MCtoDC2_xy_ortho(&m, &points[e->p[0]], &x1, &y1);
								display->view->MCtoDC2_xy_ortho(&m, &points[e->p[1]], &x2, &y2);

								gfx.ClipLine(x1,y1, x2,y2);
								e++;
							}
						}
					}
					if (px)
						delete [ ] px;
					if (py)
						delete [ ] py;
				}
			}
			break;
	}
}

/*************
 * DESCRIPTION:   calculate bounding box for object
 * INPUT:         none
 * OUTPUT:        none
 *************/
void MESH::CalcBBox()
{
	int i;
	VECTOR *p;

	if(points)
	{
		SetVector(&bboxmax, -INFINITY, -INFINITY, -INFINITY);
		SetVector(&bboxmin,  INFINITY,  INFINITY,  INFINITY);

		p = points;
		for(i=0; i<actpoints; i++)
		{
			if(p->x > bboxmax.x)
				bboxmax.x = p->x;
			if(p->y > bboxmax.y)
				bboxmax.y = p->y;
			if(p->z > bboxmax.z)
				bboxmax.z = p->z;

			if(p->x < bboxmin.x)
				bboxmin.x = p->x;
			if(p->y < bboxmin.y)
				bboxmin.y = p->y;
			if(p->z < bboxmin.z)
				bboxmin.z = p->z;
			p++;
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
void MESH::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	int i;
	VECTOR *p;
	UBYTE *sp;
	MATRIX m, m1, transform;
	BOOL edit=FALSE;
	VECTOR local_pos, local_ox,local_oy,local_oz;

	if(points)
	{
		p = points;
		if(disp)
		{
			if((disp->editmode & EDIT_MESH) && selpoints)
			{
				edit = TRUE;
			}
		}
		if(edit)
		{
			GetObjectMatrix(&m);
			m.GenerateAxis(&local_ox,&local_oy,&local_oz, &local_pos);
			transform.SetOMatrix(&local_ox, &local_oy, &local_oz);
			m.SetSOTMatrix(size, ox,oy,oz, pos);
			m1.MultMat(&transform, &m);
			transform.InvMat(&m);
			transform.MultMat(&m1, &m);

			sp = selpoints;
			for(i=0; i<actpoints; i++)
			{
				if(*sp)
					transform.MultVectMat(p);
				p++;
				sp++;
			}
		}
		else
		{
			for(i=0; i<actpoints; i++)
			{
				p->x *= size->x;
				p->y *= size->y;
				p->z *= size->z;
				p++;
			}
		}
		CalcBBox();
	}
}

/*************
 * DESCRIPTION:   write mesh to scene file also handles external objects
 * INPUT:         iff      iff handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL MESH::WriteObject(struct IFFHandle *iff)
{
	UWORD *triabuffer;
	int i,j;

	if(external == EXTERNAL_ROOT)
	{
		if(PushChunk(iff, ID_EXTN, ID_FORM, IFFSIZE_UNKNOWN))
			return FALSE;
	}
	else
	{
		if(PushChunk(iff, ID_MESH, ID_FORM, IFFSIZE_UNKNOWN))
			return FALSE;
	}

	if(!WriteCommon(iff))
		return FALSE;

	if(!WriteSurface(iff))
		return FALSE;

	if(external == EXTERNAL_ROOT)
	{
		if(!WriteChunk(iff, ID_FILE, file, strlen(file)+1))
			return FALSE;
	}
	else
	{
		if(actpoints)
		{
			if(!WriteLongChunk(iff, ID_PNTS, points, actpoints*3))
				return FALSE;
			if(!WriteWordChunk(iff, ID_EDGE, edges, actedges*3))
				return FALSE;

			triabuffer = new UWORD[acttrias*3];
			if(!triabuffer)
				return FALSE;
			for(i=0; i<acttrias; i++)
			{
				j = i*3;
				triabuffer[j] = trias[i].e[0];
				triabuffer[j+1] = trias[i].e[1];
				triabuffer[j+2] = trias[i].e[2];
			}
			if(!WriteWordChunk(iff, ID_FACE, triabuffer, acttrias*3))
				return FALSE;
			delete triabuffer;
		}
	}

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   transfer mesh data to RayStorm Interface
 * INPUT:         stack    matrix stack
 *                object   pointer to created rsi object (only for CSG)
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult MESH::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	void *surface;
	BOOL retry;
	char buffer[256];
	VECTOR pos,orient_x,orient_y,orient_z, size;
	UWORD *triabuffer;
	int i,j;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	if(external == EXTERNAL_ROOT)
	{
		size = this->size;

		if(flags & OBJECT_APPLYSURF)
		{
			err = surf->ToRSI(rc, &surface,stack,(OBJECT*)this);
			if(err)
				return err;
		}

		retry = FALSE;
		do
		{
			if(flags & OBJECT_APPLYSURF)
			{
				err = PPC_STUB(rsiLoadObject)(CTXT, file,
					rsiTObjSurface,   surface,
					rsiTObjPos,       &pos,
					rsiTObjOrientX,   &orient_x,
					rsiTObjOrientY,   &orient_y,
					rsiTObjOrientZ,   &orient_z,
					rsiTObjScale,     &size,
					rsiTDone);
			}
			else
			{
				err = PPC_STUB(rsiLoadObject)(CTXT, file,
					rsiTObjPos,       &pos,
					rsiTObjOrientX,   &orient_x,
					rsiTObjOrientY,   &orient_y,
					rsiTObjOrientZ,   &orient_z,
					rsiTObjScale,     &size,
					rsiTDone);
			}
			if(err)
			{
				// can't load object -> ask user for other filename
				PPC_STUB(rsiGetErrorMsg)(CTXT, buffer, err);
				retry = utility.AskRequest("Can't load object '%s':\n%s\n\nDo you want to change the filename?", file, buffer);
				if(!retry)
					return err;
				else
				{
					strcpy(buffer, file);
					retry = utility.FileRequ(buffer, FILEREQU_OBJECT, FILEREQU_INITIALFILE);
					if(retry)
						SetFileName(buffer);
				}
			}
		}
		while(retry && err);
		return err;
	}
	if((external == EXTERNAL_NONE) && acttrias)
	{
		SetVector(&size, 1.f, 1.f, 1.f);

		err = surf->ToRSI(rc, &surface,stack,(OBJECT*)this);
		if(err)
			return err;

		triabuffer = new UWORD[acttrias*3];
		if(!triabuffer)
			return FALSE;
		for(i=0; i<acttrias; i++)
		{
			j = i*3;
			triabuffer[j] = trias[i].e[0];
			triabuffer[j+1] = trias[i].e[1];
			triabuffer[j+2] = trias[i].e[2];
		}
		err = PPC_STUB(rsiCreateMesh)(CTXT,
			surface,
			(rsiVECTOR*)points,
			(rsiEDGE*)edges, actedges,
			(rsiMESH*)triabuffer, acttrias,
			rsiTMeshPos, &pos,
			rsiTMeshOrientX, &orient_x,
			rsiTMeshOrientY, &orient_y,
			rsiTMeshOrientZ, &orient_z,
			rsiTMeshSize, &size,
			rsiTMeshNoPhong, flags & OBJECT_NOPHONG ? TRUE : FALSE,
			rsiTDone);
		delete triabuffer;
		if(err)
			return err;
	}

	return 0;
}

/*************
 * DESCRIPTION:   parse an external object from a RSCN file
 * INPUT:         iff      iff handle
 *                where    where to insert after
 *                dir      insert direction
 * OUTPUT:        created object
 *************/
OBJECT *ParseExternal(struct IFFHandle *iff, OBJECT *where, int dir)
{
	MESH *root;
	struct ContextNode *cn;
	long error = 0;
	ULONG flags;
	char *err;
	BOOL retry;
	char buffer[256];

	root = new MESH;
	if(!root)
		return NULL;

	if(!root->ReadAxis(iff))
	{
		delete root;
		return NULL;
	}
	root->surf = GetSurfaceByName(SURF_DEFAULTNAME);

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete root;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_EXTN))
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
						if(!ReadSurface(iff, &root->surf, (OBJECT*)root))
						{
							delete root;
							return NULL;
						}
						break;
				}
				break;
			case ID_NAME:
				if(!root->ReadName(iff))
				{
					delete root;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!root->ReadTrack(iff))
				{
					delete root;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(rscn_version < 200)
				{
					if(!ReadULONG(iff,&flags,1))
					{
						delete root;
						return NULL;
					}
					if(flags & 1)
						root->flags |= OBJECT_APPLYSURF;
					else
						root->flags &= ~OBJECT_APPLYSURF;
					if(flags & 2)
						root->flags |= OBJECT_NOPHONG;
					else
						root->flags &= ~OBJECT_NOPHONG;
				}
				else
				{
					if(!root->ReadFlags(iff))
					{
						delete root;
						return NULL;
					}
				}
				break;
			case ID_FILE:
				if(!ReadString(iff,buffer,256))
				{
					delete root;
					return NULL;
				}
				retry = FALSE;
				do
				{
					err = LoadObject(buffer,root);
					if(err)
					{
						// can't load object -> ask user for other filename
						retry = utility.AskRequest("Can't load object '%s':\n%s\n\nDo you want to change the filename?", buffer, err);
						if(!retry)
							return NULL;
						else
							retry = utility.FileRequ(buffer, FILEREQU_OBJECT, FILEREQU_INITIALFILEDIR);
					}
				}
				while(retry && err);
				((OBJECT*)root)->Insert(where,dir);
				break;
		}
	}

	return (OBJECT*)root;
}

/*************
 * DESCRIPTION:   parse a mesh from a RSCN file
 * INPUT:         iff      iff handle
 *                where    where to insert after
 *                dir      insert direction
 * OUTPUT:        created object
 *************/
OBJECT *ParseMesh(struct IFFHandle *iff, OBJECT *where, int dir)
{
	MESH *mesh;
	struct ContextNode *cn;
	long error = 0;
	ULONG flags;
	int i,j;
	UWORD *triabuffer;

	mesh = new MESH;
	if(!mesh)
		return NULL;

	if(!mesh->ReadAxis(iff))
	{
		delete mesh;
		return NULL;
	}
	mesh->surf = GetSurfaceByName(SURF_DEFAULTNAME);

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete mesh;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_MESH))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_NAME:
				if(!mesh->ReadName(iff))
				{
					delete mesh;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!mesh->ReadTrack(iff))
				{
					delete mesh;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(rscn_version < 200)
				{
					if(!ReadULONG(iff,&flags,1))
					{
						delete mesh;
						return NULL;
					}
					if(flags & 2)
						mesh->flags |= OBJECT_NOPHONG;
					else
						mesh->flags &= ~OBJECT_NOPHONG;
				}
				else
				{
					if(!mesh->ReadFlags(iff))
					{
						delete mesh;
						return NULL;
					}
				}
				break;
			case ID_PNTS:
				if(mesh->AddPoint(cn->cn_Size/12) == -1)
				{
					delete mesh;
					return NULL;
				}
				if(!ReadFloat(iff, (float*)(mesh->points), cn->cn_Size>>2))
				{
					delete mesh;
					return NULL;
				}
				break;
			case ID_EDGE:
				if(mesh->AddEdge(cn->cn_Size/6) == -1)
				{
					delete mesh;
					return NULL;
				}
				if(!ReadWord(iff, (WORD*)(mesh->edges), cn->cn_Size>>1))
				{
					delete mesh;
					return NULL;
				}
				break;
			case ID_FACE:
				triabuffer = new UWORD[cn->cn_Size>>1];
				if(!triabuffer)
				{
					delete mesh;
					return NULL;
				}
				if(!ReadWord(iff, (WORD*)triabuffer, cn->cn_Size>>1))
				{
					delete triabuffer;
					delete mesh;
					return NULL;
				}
				if(mesh->AddTria(cn->cn_Size/6) == -1)
				{
					delete triabuffer;
					delete mesh;
					return NULL;
				}
				for(i=0; i<cn->cn_Size/6; i++)
				{
					j = i*3;
					mesh->trias[i].e[0] = triabuffer[j];
					mesh->trias[i].e[1] = triabuffer[j+1];
					mesh->trias[i].e[2] = triabuffer[j+2];
				}
				delete triabuffer;
				break;
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						if(!ReadSurface(iff, &mesh->surf, (OBJECT*)mesh))
						{
							delete mesh;
							return NULL;
						}
						break;
				}
				break;
		}
	}

	mesh->CalcBBox();
	((OBJECT*)mesh)->Insert(where,dir);

	return (OBJECT*)mesh;
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *MESH::DupObj()
{
	MESH *dup;

	dup = new MESH;
	if(!dup)
		return NULL;

	*dup = *this;
	// set to NULL to avoid freeing if something below fails
	dup->points = NULL;
	dup->edges = NULL;
	dup->trias = NULL;

	if(file)
	{
		if(!dup->SetFileName(file))
		{
			delete dup;
			return NULL;
		}
	}

	dup->points = new VECTOR[actpoints];
	if(!dup->points)
	{
		delete dup;
		return NULL;
	}
	dup->maxpoints = actpoints;
	memcpy(dup->points, points, actpoints*sizeof(VECTOR));

	dup->edges = new EDGE[actedges];
	if(!dup->edges)
	{
		delete dup;
		return NULL;
	}
	dup->maxedges = actedges;
	memcpy(dup->edges, edges, actedges*sizeof(EDGE));

	dup->trias = new TRIANGLE[acttrias];
	if(!dup->trias)
	{
		delete dup;
		return NULL;
	}
	dup->maxtrias = acttrias;
	memcpy(dup->trias, trias, acttrias*sizeof(TRIANGLE));

	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   create a box mesh object
 * INPUT:         size     half edge length
 * OUTPUT:        TRUE if successfully created else FALSE
 *************/
static EDGE boxedges[] =
{
	{ 0,1 },
	{ 1,5 },
	{ 5,4 },
	{ 4,0 },
	{ 0,5 },
	{ 0,3 },
	{ 3,2 },
	{ 2,1 },
	{ 0,2 },
	{ 3,7 },
	{ 7,6 },
	{ 6,2 },
	{ 2,7 },
	{ 1,6 },
	{ 6,5 },
	{ 5,7 },
	{ 7,4 },
	{ 4,3 },
};

static TRIANGLE boxfaces[] =
{
#ifdef NORMAL
	{  0,  1,  4, 0, 0, 0 },
	{  4,  2,  3, 0, 0, 0 },
	{  5,  3, 17, 5, 5, 5 },
	{ 17, 16,  9, 5, 5, 5 },
	{  6,  9, 12, 1, 1, 1 },
	{ 12, 10, 11, 1, 1, 1 },
	{  7, 11, 13, 4, 4, 4 },
	{ 13, 14,  1, 4, 4, 4 },
	{  2, 15, 16, 2, 2, 2 },
	{ 15, 14, 10, 2, 2, 2 },
	{  0,  8,  7, 3, 3, 3 },
	{  5,  6,  8, 3, 3, 3 },
#else // NORMAL
	{  0,  1,  4 },
	{  4,  2,  3 },
	{  5,  3, 17 },
	{ 17, 16,  9 },
	{  6,  9, 12 },
	{ 12, 10, 11 },
	{  7, 11, 13 },
	{ 13, 14,  1 },
	{  2, 15, 16 },
	{ 15, 14, 10 },
	{  0,  8,  7 },
	{  5,  6,  8 },
#endif // NORMAL
};

BOOL MESH::CreateCube(VECTOR *size)
{
	int i;

	// test parameters
	if(size->x < EPSILON)
		size->x = EPSILON;
	if(size->y < EPSILON)
		size->y = EPSILON;
	if(size->z < EPSILON)
		size->z = EPSILON;

	if(AddPoint(8) == -1)
		return FALSE;

	if(AddEdge(18) == -1)
		return FALSE;

#ifdef NORMAL
	if(AddNormal(6) == -1)
		return FALSE;
#endif // NORMAL

	if(AddTria(12) == -1)
		return FALSE;

	// set points
	SetVector(&points[0],  size->x, -size->y, -size->z);
	SetVector(&points[1],  size->x, -size->y,  size->z);
	SetVector(&points[2], -size->x, -size->y,  size->z);
	SetVector(&points[3], -size->x, -size->y, -size->z);
	SetVector(&points[4],  size->x,  size->y, -size->z);
	SetVector(&points[5],  size->x,  size->y,  size->z);
	SetVector(&points[6], -size->x,  size->y,  size->z);
	SetVector(&points[7], -size->x,  size->y, -size->z);

	// set edges
	for(i=0; i<18; i++)
	{
		SetEdge(&edges[i], boxedges[i].p[0], boxedges[i].p[1]);
		edges[i].flags = EDGE_SHARP;
	}

#ifdef NORMAL
	// set normals
	SetVector(&normals[0], 1.f, 0.f, 0.f);
	SetVector(&normals[1], -1.f, 0.f, 0.f);
	SetVector(&normals[2], 0.f, 1.f, 0.f);
	SetVector(&normals[3], 0.f, -1.f, 0.f);
	SetVector(&normals[4], 0.f, 0.f, 1.f);
	SetVector(&normals[5], 0.f, 0.f, -1.f);
#endif // NORMAL

	for(i=0; i<18; i++)
	{
		SetTriangleEdges(&trias[i], boxfaces[i].e[0], boxfaces[i].e[1], boxfaces[i].e[2]);
#ifdef NORMAL
		SetTriangleNormals(&trias[i], boxfaces[i].n[0], boxfaces[i].n[1], boxfaces[i].n[2]);
#endif // NORMAL
	}

	CalcBBox();
	SetName("Box");

	return TRUE;
}

/*************
 * DESCRIPTION:   create a sphere mesh object
 * INPUT:         radius   radius
 *                divs     radial divisions (3...)
 *                slices   vertical slices (2...)
 * OUTPUT:        TRUE if successfully created else FALSE
 *************/
BOOL MESH::CreateSphere(float radius, int divs, int slices)
{
	VECTOR *point;
	EDGE *edge;
	TRIANGLE *tria;
	int i,j;

	// test parameters
	if(radius < EPSILON)
		radius = EPSILON;
	if(divs < 3)
		divs = 3;
	if(slices < 2)
		slices = 2;

	// allocate points, edges and triangles
	if(AddPoint((slices - 1)*divs + 2) == -1)
		return FALSE;

	if(AddEdge((slices - 2)*divs*3 + divs*3) == -1)
		return FALSE;

	if(AddTria((slices - 1)*divs*2) == -1)
		return FALSE;

	// set points
	point = points;

	// base point
	SetVector(point, 0.f, -radius, 0.f);
	point++;

	for(i=0; i<slices-1; i++)
	{
		for(j=0; j<divs; j++)
		{
			SetVector(point,
				 radius*sin(PI*(float)(i+1) / (float)slices) * sin(2.f*PI*(float)j / (float)divs),
				-radius*cos(PI*(float)(i+1) / (float)slices),
				 radius*sin(PI*(float)(i+1) / (float)slices) * cos(2.f*PI*(float)j / (float)divs));
			point++;
		}
	}

	// top point
	SetVector(point, 0.f, radius, 0.f);

	// set edges
	edge = edges;

	// base edges
	for(j=0; j<divs; j++)
	{
		SetEdge(edge, 0, j+1);
		edge++;
	}

	for(i=0; i<slices-2; i++)
	{
		for(j=0; j<divs-1; j++)
		{
			SetEdge(edge, j+i*divs+1, j+i*divs+2);
			edge++;
		}
		SetEdge(edge, (divs-1)+i*divs+1, i*divs+1);
		edge++;

		for(j=0; j<divs; j++)
		{
			SetEdge(edge, j+i*divs+1, j+(i+1)*divs+1);
			edge++;
		}

		for(j=0; j<divs-1; j++)
		{
			SetEdge(edge, j+i*divs+1, j+(i+1)*divs+2);
			edge++;
		}
		SetEdge(edge, (divs-1)+i*divs+1, (divs-1)+i*divs+2);
		edge++;
	}

	// top edges
	for(j=0; j<divs-1; j++)
	{
		SetEdge(edge, maxpoints-divs+j-1, maxpoints-divs+j);
		edge++;
	}
	SetEdge(edge, maxpoints-2, maxpoints-divs-1);
	edge++;
	for(j=0; j<divs; j++)
	{
		SetEdge(edge, maxpoints-divs+j-1, maxpoints-1);
		edge++;
	}

	// set triangles
	tria = trias;

	// base triangles
	for(j=0; j<divs-1; j++)
	{
		SetTriangleEdges(tria, j, j+divs, j+1);
		tria++;
	}
	SetTriangleEdges(tria, divs-1, 2*divs-1, 0);
	tria++;

	for(i=0; i<slices-2; i++)
	{
		for(j=0; j<divs-1; j++)
		{
			SetTriangleEdges(tria, divs*2+i*divs*3+j, divs*4+i*divs*3+j, divs*3+i*divs*3+j);
			tria++;
			SetTriangleEdges(tria, divs+i*divs*3+j, divs*3+i*divs*3+j, divs*2+i*divs*3+j+1);
			tria++;
		}
		SetTriangleEdges(tria, divs*2+i*divs*3+divs-1, divs*4+i*divs*3+divs-1, divs*3+i*divs*3+divs-1);
		tria++;
		SetTriangleEdges(tria, divs+i*divs*3+divs-1, divs*3+i*divs*3+divs-1, divs*2+i*divs*3);
		tria++;
	}

	// top triangles
	for(j=0; j<divs-1; j++)
	{
		SetTriangleEdges(tria, maxedges-divs+j, maxedges-divs+j+1, maxedges-2*divs+j);
		tria++;
	}
	SetTriangleEdges(tria, maxedges-1, maxedges-divs, maxedges-divs-1);
	tria++;

	CalcBBox();
	SetName("Sphere");

	return TRUE;
}

/*************
 * DESCRIPTION:   create a plane mesh object
 * INPUT:         size     half edge length
 *                xdivs    division in x direction
 *                zdivs    division in z direction
 * OUTPUT:        TRUE if successfully created else FALSE
 *************/
BOOL MESH::CreatePlane(VECTOR *size, int xdivs, int zdivs)
{
	VECTOR *point;
	EDGE *edge;
	TRIANGLE *tria;
	int i,j;

	// test parameters
	if(size->x < EPSILON)
		size->x = EPSILON;
	if(size->y < EPSILON)
		size->y = EPSILON;
	if(size->z < EPSILON)
		size->z = EPSILON;
	if(xdivs < 1)
		xdivs = 1;
	if(zdivs < 1)
		zdivs = 1;

	// allocate points, edges and triangles
	if(AddPoint((xdivs+1)*(zdivs+1)) == -1)
		return FALSE;

	if(AddEdge(3*xdivs*zdivs+xdivs+zdivs) == -1)
		return FALSE;

	if(AddTria(xdivs*zdivs*2) == -1)
		return FALSE;

	// create points
	point = points;
	for(i=0; i<=zdivs; i++)
	{
		for(j=0; j<=xdivs; j++)
		{
			SetVector(point,
				2*size->x*(float)j/(float)xdivs - size->x,
				0.,
				2*size->z*(float)i/(float)zdivs - size->z);
			point++;
		}
	}

	// create edges
	edge = edges;
	for(i=0; i<zdivs; i++)
	{
		for(j=0; j<xdivs; j++)
		{
			SetEdge(edge, i*(xdivs+1)+j, i*(xdivs+1)+j+1);
			edge++;
		}
		for(j=0; j<=xdivs; j++)
		{
			SetEdge(edge, i*(xdivs+1)+j, (i+1)*(xdivs+1)+j);
			edge++;
		}
		for(j=0; j<xdivs; j++)
		{
			SetEdge(edge, i*(xdivs+1)+j+1, (i+1)*(xdivs+1)+j);
			edge++;
		}
	}
	for(j=0; j<xdivs; j++)
	{
		SetEdge(edge, zdivs*(xdivs+1)+j, zdivs*(xdivs+1)+j+1);
		edge++;
	}

	// create triangles
	tria = trias;
	for(i=0; i<zdivs; i++)
	{
		for(j=0; j<xdivs; j++)
		{
			SetTriangleEdges(tria,
				i*(2*xdivs+xdivs+1)+j,
				i*(2*xdivs+xdivs+1)+xdivs+xdivs+1+j,
				i*(2*xdivs+xdivs+1)+xdivs+j);
			tria++;
			SetTriangleEdges(tria,
				i*(2*xdivs+xdivs+1)+xdivs+j+1,
				(i+1)*(2*xdivs+xdivs+1)+j,
				i*(2*xdivs+xdivs+1)+xdivs+xdivs+1+j);
			tria++;
		}
	}
	CalcBBox();
	SetName("Plane");

	return TRUE;
}

/*************
 * DESCRIPTION:   create a tube mesh object
 * INPUT:         radius         radius of cylinder
 *                height         height of cylinder
 *                divs           radial divisions (3...)
 *                slices         vertical slices (1...)
 *                closebottom    close bottom of cylinder
 *                closetop       close top of cylinder
 * OUTPUT:        TRUE if successfully created else FALSE
 *************/
BOOL MESH::CreateTube(float radius, float height, int divs, int slices, BOOL closebottom, BOOL closetop)
{
	VECTOR *point;
	EDGE *edge;
	TRIANGLE *tria;
	int i,j;
	int offset;

	// test parameters
	if(radius < EPSILON)
		radius = EPSILON;
	if(height < EPSILON)
		height = EPSILON;
	if(divs < 3)
		divs = 3;
	if(slices < 1)
		slices = 1;

	// allocate points, edges and triangles
	if(AddPoint(divs * (slices+1) + (closebottom ? 1 : 0) + (closetop ? 1 : 0)) == -1)
		return FALSE;

	if(AddEdge((divs*3*slices) + divs + (closebottom ? divs : 0) + (closetop ? divs : 0)) == -1)
		return FALSE;

	if(AddTria((divs*slices*2) + (closebottom ? divs : 0) + (closetop ? divs : 0)) == -1)
		return FALSE;

	// set points
	point = points;
	if(closebottom)
	{
		SetVector(point, 0., 0., 0.);
		point++;
	}
	for(i=0; i<=slices; i++)
	{
		for(j=0; j<divs; j++)
		{
			SetVector(point,
				radius * sin(2*PI*(float)j / (float)divs),
				height*(float)i / (float)slices,
				radius * cos(2*PI*(float)j / (float)divs));
			point++;
		}
	}
	if(closetop)
		SetVector(point, 0., height, 0.);

	// set edges
	edge = edges;
	if(closebottom)
	{
		for(j=0; j<divs; j++)
		{
			SetEdge(edge, 0, j+1);
			edge->flags = EDGE_SHARP;
			edge++;
		}
		offset = 1;
	}
	else
		offset = 0;

	for(j=0; j<divs; j++)
		edge[j].flags = EDGE_SHARP;

	for(i=0; i<slices; i++)
	{
		for(j=0; j<divs-1; j++)
		{
			SetEdge(edge, offset+j+i*divs, offset+j+i*divs+1);
			edge++;
		}
		SetEdge(edge, offset+(divs-1)+i*divs, offset+i*divs);
		edge++;

		for(j=0; j<divs; j++)
		{
			SetEdge(edge, offset+j+i*divs, offset+j+(i+1)*divs);
			edge++;
		}

		for(j=0; j<divs-1; j++)
		{
			SetEdge(edge, offset+j+i*divs, offset+j+(i+1)*divs+1);
			edge++;
		}
		SetEdge(edge, offset+(divs-1)+i*divs, offset+(divs-1)+i*divs+1);
		edge++;
	}
	for(j=0; j<divs-1; j++)
	{
		SetEdge(edge, offset+j+slices*divs, offset+j+slices*divs+1);
		edge->flags = EDGE_SHARP;
		edge++;
	}
	SetEdge(edge, offset+(divs-1)+slices*divs, offset+slices*divs);
	edge->flags = EDGE_SHARP;
	edge++;
	if(closetop)
	{
		for(j=0; j<divs; j++)
		{
			SetEdge(edge, maxpoints-1, maxpoints-divs+j-1);
			edge->flags = EDGE_SHARP;
			edge++;
		}
	}

	// set trias
	tria = trias;
	if(closebottom)
	{
		for(j=0; j<divs-1; j++)
		{
			SetTriangleEdges(tria, j, divs+j, j+1);
			tria++;
		}
		SetTriangleEdges(tria, divs-1, 2*divs-1, 0);
		tria++;
		offset = divs;
	}
	else
		offset = 0;

	for(i=0; i<slices; i++)
	{
		for(j=0; j<divs-1; j++)
		{
			SetTriangleEdges(tria, offset+divs+i*divs*3+j, offset+divs*3+i*divs*3+j, offset+divs*2+i*divs*3+j);
			tria++;
			SetTriangleEdges(tria, offset+i*divs*3+j, offset+divs*2+i*divs*3+j, offset+divs+i*divs*3+j+1);
			tria++;
		}
		SetTriangleEdges(tria, offset+divs*2+i*divs*3-1, offset+divs*4+i*divs*3-1, offset+divs*3+i*divs*3-1);
		tria++;
		SetTriangleEdges(tria, offset+divs+i*divs*3-1, offset+divs*3+i*divs*3-1, offset+divs+i*divs*3);
		tria++;
	}

	if(closetop)
	{
		for(j=0; j<divs-1; j++)
		{
			SetTriangleEdges(tria, maxedges-divs+j, maxedges-divs+j+1, maxedges-2*divs+j);
			tria++;
		}
		SetTriangleEdges(tria, maxedges-1, maxedges-divs, maxedges-divs-1);
		tria++;
	}

	CalcBBox();
	SetName("Cylinder");

	return TRUE;
}

/*************
 * DESCRIPTION:   create a cone mesh object
 * INPUT:         radius         radius of cone
 *                height         height of cone
 *                divs           radial divisions (3...)
 *                slices         vertical slices (1...)
 *                closebottom    close bottom of cone
 * OUTPUT:        TRUE if successfully created else FALSE
 *************/
BOOL MESH::CreateCone(float radius, float height, int divs, int slices, BOOL closebottom)
{
	VECTOR *point;
	EDGE *edge;
	TRIANGLE *tria;
	int i,j;
	int offset;

	// test parameters
	if(radius < EPSILON)
		radius = EPSILON;
	if(height < EPSILON)
		height = EPSILON;
	if(divs < 3)
		divs = 3;
	if(slices < 1)
		slices = 1;

	// allocate points, edges and triangles
	if(AddPoint(divs*slices + (closebottom ? 1 : 0) + 1) == -1)
		return FALSE;

	if(AddEdge((divs*3*(slices-1)) + 2*divs + (closebottom ? divs : 0)) == -1)
		return FALSE;

	if(AddTria((divs*(slices-1)*2) + divs + (closebottom ? divs : 0)) == -1)
		return FALSE;

	// set points
	point = points;
	if(closebottom)
	{
		SetVector(point, 0., 0., 0.);
		point++;
	}
	for(i=0; i<slices; i++)
	{
		for(j=0; j<divs; j++)
		{
			SetVector(point,
				radius * (float)(slices-i) / (float)slices * sin(2*PI*(float)j / (float)divs),
				height*(float)i / (float)slices,
				radius * (float)(slices-i) / (float)slices * cos(2*PI*(float)j / (float)divs));
			point++;
		}
	}
	SetVector(point, 0., height, 0.);

	// set edges
	edge = edges;
	if(closebottom)
	{
		for(j=0; j<divs; j++)
		{
			SetEdge(edge, 0, j+1);
			edge->flags = EDGE_SHARP;
			edge++;
		}
		offset = 1;
	}
	else
		offset = 0;

	if(slices > 1)
	{
		for(j=0; j<divs; j++)
			edge[j].flags = EDGE_SHARP;
	}

	for(i=0; i<slices-1; i++)
	{
		for(j=0; j<divs-1; j++)
		{
			SetEdge(edge, offset+j+i*divs, offset+j+i*divs+1);
			edge++;
		}
		SetEdge(edge, offset+(divs-1)+i*divs, offset+i*divs);
		edge++;

		for(j=0; j<divs; j++)
		{
			SetEdge(edge, offset+j+i*divs, offset+j+(i+1)*divs);
			edge++;
		}

		for(j=0; j<divs-1; j++)
		{
			SetEdge(edge, offset+j+i*divs, offset+j+(i+1)*divs+1);
			edge++;
		}
		SetEdge(edge, offset+(divs-1)+i*divs, offset+(divs-1)+i*divs+1);
		edge++;
	}
	for(j=0; j<divs-1; j++)
	{
		SetEdge(edge, offset+j+(slices-1)*divs, offset+j+(slices-1)*divs+1);
		edge->flags = EDGE_SHARP;
		edge++;
	}
	SetEdge(edge, offset+(divs-1)+(slices-1)*divs, offset+(slices-1)*divs);
	edge->flags = EDGE_SHARP;
	edge++;

	for(j=0; j<divs; j++)
	{
		SetEdge(edge, maxpoints-1, maxpoints-divs+j-1);
		edge++;
	}

	// set trias
	tria = trias;
	if(closebottom)
	{
		for(j=0; j<divs-1; j++)
		{
			SetTriangleEdges(tria, j, divs+j, j+1);
			tria++;
		}
		SetTriangleEdges(tria, divs-1, 2*divs-1, 0);
		tria++;
		offset = divs;
	}
	else
		offset = 0;

	for(i=0; i<slices-1; i++)
	{
		for(j=0; j<divs-1; j++)
		{
			SetTriangleEdges(tria, offset+divs+i*divs*3+j, offset+divs*3+i*divs*3+j, offset+divs*2+i*divs*3+j);
			tria++;
			SetTriangleEdges(tria, offset+i*divs*3+j, offset+divs*2+i*divs*3+j, offset+divs+i*divs*3+j+1);
			tria++;
		}
		SetTriangleEdges(tria, offset+divs*2+i*divs*3-1, offset+divs*4+i*divs*3-1, offset+divs*3+i*divs*3-1);
		tria++;
		SetTriangleEdges(tria, offset+divs+i*divs*3-1, offset+divs*3+i*divs*3-1, offset+divs+i*divs*3);
		tria++;
	}

	for(j=0; j<divs-1; j++)
	{
		SetTriangleEdges(tria, maxedges-divs+j, maxedges-divs+j+1, maxedges-2*divs+j);
		tria++;
	}
	SetTriangleEdges(tria, maxedges-1, maxedges-divs, maxedges-divs-1);
	tria++;

	CalcBBox();
	SetName("Cone");

	return TRUE;
}

/*************
 * DESCRIPTION:   create a torus mesh object
 * INPUT:         middle         torus radius
 *                radius         radius of torus ring
 *                divs           radial divisions (3...)
 *                slices         slices (3...)
 * OUTPUT:        TRUE if successfully created else FALSE
 *************/
BOOL MESH::CreateTorus(float middle, float radius, int divs, int slices)
{
	VECTOR *point;
	EDGE *edge;
	TRIANGLE *tria;
	int i,j,divs3;
	float x,z;

	// test parameters
	if(middle < EPSILON)
		middle = EPSILON;
	if(radius < EPSILON)
		radius = EPSILON;
	if(divs < 3)
		divs = 3;
	if(slices < 3)
		slices = 3;

	// allocate points, edges and triangles
	if(AddPoint(divs*slices) == -1)
		return FALSE;

	if(AddEdge((divs*slices*3)) == -1)
		return FALSE;

	if(AddTria((divs*slices*2)) == -1)
		return FALSE;

	// set points
	point = points;
	for(i=0; i<slices; i++)
	{
		x = cos(2*PI*(float)i / (float)slices);
		z = sin(2*PI*(float)i / (float)slices);
		for(j=0; j<divs; j++)
		{
			SetVector(point,
				middle*x + x * radius * cos(2*PI*(float)j / (float)divs),
				radius * sin(2*PI*(float)j / (float)divs),
				middle*z + z * radius * cos(2*PI*(float)j / (float)divs));
			point++;
		}
	}

	// set edges
	edge = edges;
	for(i=0; i<slices-1; i++)
	{
		for(j=0; j<divs-1; j++)
		{
			SetEdge(edge, j+i*divs, j+i*divs+1);
			edge++;
		}
		SetEdge(edge, (divs-1)+i*divs, i*divs);
		edge++;

		for(j=0; j<divs; j++)
		{
			SetEdge(edge, j+i*divs, j+(i+1)*divs);
			edge++;
		}

		for(j=0; j<divs-1; j++)
		{
			SetEdge(edge, j+i*divs, j+(i+1)*divs+1);
			edge++;
		}
		SetEdge(edge, (divs-1)+i*divs, (divs-1)+i*divs+1);
		edge++;
	}
	for(j=0; j<divs-1; j++)
	{
		SetEdge(edge, j+(slices-1)*divs, j+(slices-1)*divs+1);
		edge++;
	}
	SetEdge(edge, slices*divs-1, (slices-1)*divs);
	edge++;

	for(j=0; j<divs; j++)
	{
		SetEdge(edge, j+(slices-1)*divs, j);
		edge++;
	}

	for(j=0; j<divs-1; j++)
	{
		SetEdge(edge, j+(slices-1)*divs, j+1);
		edge++;
	}
	SetEdge(edge, slices*divs-1, 0);

	// set triangles
	tria = trias;
	divs3 = divs*3;
	for(i = 0; i < slices - 1; i++)
	{
		for(j = 0; j < divs - 1; j++)
		{
			SetTriangleEdges(tria, divs + i*divs3 + j, divs*3 + i*divs3 + j, divs*2 + i*divs3 + j);
			tria++;
			SetTriangleEdges(tria, i*divs3 + j, divs*2 + i*divs3 + j, divs + i*divs3 + j + 1);
			tria++;
		}
		SetTriangleEdges(tria, divs*2 + i*divs3 - 1, divs*4 + i*divs3 - 1, divs3 + i*divs3 - 1);
		tria++;
		SetTriangleEdges(tria, divs + i*divs3 - 1, divs3 + i*divs3 - 1, divs + i*divs3);
		tria++;
	}
	for(j = 0; j < divs - 1; j++)
	{
		SetTriangleEdges(tria, divs + (slices - 1)*divs3 + j, j, divs*2 + (slices - 1)*divs3 + j);
		tria++;
		SetTriangleEdges(tria, (slices - 1)*divs3 + j, divs*2 + (slices - 1)*divs3 + j, divs+(slices - 1)*divs3 + j + 1);
		tria++;
	}
	SetTriangleEdges(tria, divs*2 + (slices - 1)*divs3 - 1, divs - 1, divs3 + (slices - 1)*divs3 - 1);
	tria++;
	SetTriangleEdges(tria, divs + (slices - 1)*divs3 - 1, divs3 + (slices - 1)*divs3 - 1, divs + (slices - 1)*divs3);

	CalcBBox();
	SetName("Torus");

	return TRUE;
}

/*************
 * DESCRIPTION:   remove the root object of an external object and mark all child
 *    objects as EXTERNAL_NONE
 * INPUT:         -
 * OUTPUT:        -
 *************/
void MESH::ToScene()
{
	this->ExternToScene();
	this->Remove();
}

/*************
 * DESCRIPTION:   Get the size of a mesh
 * INPUT:         -
 * OUTPUT:        size
 *************/
int MESH::GetSize()
{
	int size;

	size = sizeof(*this) + strlen(GetName()) + 1;

	if(file)
		size += strlen(file) + 1;

	size += maxpoints*sizeof(VECTOR);

	size += maxedges*sizeof(EDGE);

	size += maxtrias*sizeof(TRIANGLE);

	return size;
}

/*************
 * DESCRIPTION:   Test if a point of a mesh is selected
 * INPUT:         disp     display
 *                x,y      position to test
 *                multi    multi select on/off
 *                stack    matrix stack
 * OUTPUT:        TRUE if new point selected else FALSE
 *************/
BOOL MESH::TestSelectedPoint(DISPLAY *display, int x, int y, BOOL multi, MATRIX_STACK *stack, int *index)
{
	int i;
	VECTOR *p, v;
	BOOL sel = FALSE;
	UBYTE *sp, *sp1;

	if(actpoints)
	{
		if(!selpoints)
		{
			selpoints = new UBYTE[maxpoints];
			if(!selpoints)
				return FALSE;
			memset(selpoints, 0, actpoints);
		}

		p = points;
		sp = selpoints;
		for(i=0; i<actpoints; i++)
		{
			v = *p;
			stack->MultVectStack(&v);
			display->view->MCtoDC_xy(&v);
			if (x < v.x + TRACKWIDTH && x > v.x - TRACKWIDTH && y > v.y - TRACKWIDTH && y < v.y + TRACKWIDTH)
			{
				if(*sp != POINT_SELECTED)
				{
					*sp = POINT_SELECTED | POINT_DIRTY;
					sel = TRUE;
				}
				*index = i;
				break;
			}
			sp++;
			p++;
		}

		if(!multi && sel)
		{
			// deselect all points
			sp1 = selpoints;
			for(i=0; i<actpoints; i++)
			{
				if(*sp1 == POINT_SELECTED)
					*sp1 = POINT_DIRTY;
				sp1++;
			}
		}

		if(sel)
			flags |= OBJECT_HANDLEDIRTY;
	}

	return sel;
}

BOOL MESH::TestSelectedPoints(DISPLAY *display, int x, int y, BOOL multi, MATRIX_STACK *stack)
{
	int index;

	return TestSelectedPoint(display, x,y, multi, stack, &index);
}

/*************
 * DESCRIPTION:   Test if a edge of a mesh is selected
 * INPUT:         disp     display
 *                x,y      position to test
 *                multi    multi select on/off
 *                stack    matrix stack
 * OUTPUT:        TRUE if new edge (or part of an edge) selected else FALSE
 *************/
BOOL MESH::TestSelectedEdges(DISPLAY *display, int x, int y, BOOL multi, MATRIX_STACK *stack)
{
	int index=-1, i, j;
	EDGE *e, *e1;

	TestSelectedPoint(display, x,y, TRUE, stack, &index);
	if(index != -1)
	{
		if(selflag == SELFLAG_NONE)
		{
			selpoint[0] = index;
			selflag = SELFLAG_FIRST;
			return TRUE;
		}
		else
		{
			e = edges;
			for(i=0; i<actedges; i++)
			{
				if(((e->p[0] == selpoint[0]) && (e->p[1] == index)) ||
					((e->p[0] == index) && (e->p[1] == selpoint[0])))
				{
					selpoint[0] = 0;
					if(!multi)
					{
						// deselect all other edges
						e1 = edges;
						for(j=0; j<actedges; j++)
						{
							if(e1->flags & EDGE_SELECTED)
							{
								e1->flags &= ~EDGE_SELECTED;
								e1->flags |= EDGE_DIRTY;

								selpoints[e1->p[0]] = POINT_DIRTY;
								selpoints[e1->p[1]] = POINT_DIRTY;
							}
							e1++;
						}
					}
					e->flags |= EDGE_SELECTED;
					selpoints[e->p[0]] = POINT_SELECTED | POINT_DIRTY;
					selpoints[e->p[1]] = POINT_SELECTED | POINT_DIRTY;
					selflag = SELFLAG_NONE;
					flags |= OBJECT_HANDLEDIRTY;
					return TRUE;
				}
				e++;
			}
			selpoints[index] = POINT_UNSELECTED;
			selpoints[selpoint[0]] = POINT_UNSELECTED;
			selpoint[0] = 0;
			selflag = SELFLAG_NONE;
		}
	}

	return FALSE;
}

/*************
 * DESCRIPTION:   Test if a triangle of a mesh is selected
 * INPUT:         disp     display
 *                x,y      position to test
 *                multi    multi select on/off
 *                stack    matrix stack
 * OUTPUT:        TRUE if new triangle (or part of a triangle) selected else FALSE
 *************/
BOOL MESH::TestSelectedTrias(DISPLAY *display, int x, int y, BOOL multi, MATRIX_STACK *stack)
{
	int index=-1, i, j;
	TRIANGLE *t, *t1;
	UWORD p1,p2,p3;

	TestSelectedPoint(display, x,y, TRUE, stack, &index);
	if(index != -1)
	{
		if(selflag == SELFLAG_NONE)
		{
			selpoint[0] = index;
			selflag = SELFLAG_FIRST;
			return TRUE;
		}
		else
		{
			if(selflag == SELFLAG_FIRST)
			{
				selpoint[1] = index;
				selflag = SELFLAG_SECOND;
				return TRUE;
			}
			else
			{
				t = trias;
				for(i=0; i<acttrias; i++)
				{
					p1 = edges[t->e[0]].p[0];
					p2 = edges[t->e[0]].p[1];
					if((p1 != edges[t->e[1]].p[0]) && (p2 != edges[t->e[1]].p[0]))
						p3 = edges[t->e[1]].p[0];
					else
						p3 = edges[t->e[1]].p[1];
					if(((p1 == index) && (p2 == selpoint[0]) && (p3 == selpoint[1])) ||
						((p1 == index) && (p3 == selpoint[0]) && (p2 == selpoint[1])) ||
						((p2 == index) && (p1 == selpoint[0]) && (p3 == selpoint[1])) ||
						((p2 == index) && (p3 == selpoint[0]) && (p1 == selpoint[1])) ||
						((p3 == index) && (p1 == selpoint[0]) && (p2 == selpoint[1])) ||
						((p3 == index) && (p2 == selpoint[0]) && (p1 == selpoint[1])))
					{
						selpoint[0] = 0;
						selpoint[1] = 0;
						if(!multi)
						{
							// deselect all other triangles
							t1 = trias;
							for(j=0; j<acttrias; j++)
							{
								if(t1->flags & TRIA_SELECTED)
								{
									t1->flags &= ~TRIA_SELECTED;
									t1->flags |= TRIA_DIRTY;

									edges[t1->e[0]].flags &= ~EDGE_SELECTED;
									edges[t1->e[0]].flags |= EDGE_DIRTY;
									edges[t1->e[1]].flags &= ~EDGE_SELECTED;
									edges[t1->e[1]].flags |= EDGE_DIRTY;
									edges[t1->e[2]].flags &= ~EDGE_SELECTED;
									edges[t1->e[2]].flags |= EDGE_DIRTY;

									selpoints[edges[t1->e[0]].p[0]] = POINT_DIRTY;
									selpoints[edges[t1->e[0]].p[1]] = POINT_DIRTY;
									selpoints[edges[t1->e[1]].p[0]] = POINT_DIRTY;
									selpoints[edges[t1->e[1]].p[1]] = POINT_DIRTY;
									selpoints[edges[t1->e[2]].p[0]] = POINT_DIRTY;
									selpoints[edges[t1->e[2]].p[1]] = POINT_DIRTY;
								}
								t1++;
							}
						}
						t->flags |= TRIA_SELECTED;
						edges[t->e[0]].flags |= EDGE_SELECTED;
						edges[t->e[1]].flags |= EDGE_SELECTED;
						edges[t->e[2]].flags |= EDGE_SELECTED;
						selpoints[edges[t->e[0]].p[0]] = POINT_SELECTED;
						selpoints[edges[t->e[0]].p[1]] = POINT_SELECTED;
						selpoints[edges[t->e[1]].p[0]] = POINT_SELECTED;
						selpoints[edges[t->e[1]].p[1]] = POINT_SELECTED;
						selpoints[edges[t->e[2]].p[0]] = POINT_SELECTED;
						selpoints[edges[t->e[2]].p[1]] = POINT_SELECTED;
						selflag = SELFLAG_NONE;
						flags |= OBJECT_HANDLEDIRTY;
						return TRUE;
					}
					t++;
				}
				selpoints[index] = POINT_UNSELECTED;
				selpoints[selpoint[0]] = POINT_UNSELECTED;
				selpoint[0] = 0;
				selpoints[selpoint[1]] = POINT_UNSELECTED;
				selpoint[1] = 0;
				selflag = SELFLAG_NONE;
			}
		}
	}

	return FALSE;
}

/*************
 * DESCRIPTION:   Free the selected points buffer
 * INPUT:         -
 * OUTPUT:        -
 *************/
void MESH::FreeSelectedPoints()
{
	EDGE *e;
	int i;

	e = edges;
	for(i=0; i<actedges; i++)
	{
		e->flags &= ~EDGE_SELECTED;
		e++;
	}

	if(selpoints)
	{
		delete selpoints;
		selpoints = NULL;
	}
	flags |= OBJECT_HANDLEDIRTY;
}

/*************
 * DESCRIPTION:   Add a point to the mesh object
 * INPUT:         display  display
 *                x,y      position
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL MESH::AddPoint(DISPLAY *display, int x, int y)
{
	int index;
	VECTOR pos;
	MATRIX m, m1;

	MouseTo3D(display, x,y, &pos);

	index = AddPoint(1);
	if(index == -1)
		return FALSE;

	GetObjectMatrix(&m);
	m.InvMat(&m1);
	m1.MultVectMat(&pos);

	points[index] = pos;

	if(!selpoints)
	{
		selpoints = new UBYTE[maxpoints];
		if(!selpoints)
			return FALSE;
		memset(selpoints, 0, actpoints);
	}
	selpoints[index] = POINT_DIRTY;
	flags |= OBJECT_HANDLEDIRTY | OBJECT_DIRTY;

	return TRUE;
}

/*************
 * DESCRIPTION:   Add a edge to the mesh object
 * INPUT:         display  display
 *                x,y      position
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL MESH::AddEdge(DISPLAY *display, int x, int y)
{
	int index=-1, edge;
	MATRIX m;
	MATRIX_STACK stack;
	EDGE *e;
	int i;

	GetObjectMatrix(&m);

	stack.Push(&m);

	TestSelectedPoint(display, x,y, TRUE, &stack, &index);
	if(index != -1)
	{
		if(selflag == SELFLAG_NONE)
		{
			selpoint[0] = index;
			selflag = SELFLAG_FIRST;
			flags |= OBJECT_HANDLEDIRTY | OBJECT_DIRTY;
			return TRUE;
		}
		else
		{
			// test if there is already an edge between this points
			// and deselect all edges
			e = edges;
			for(i=0; i<actedges; i++)
			{
				if(((e->p[0] == selpoint[0]) && (e->p[1] == index)) ||
					((e->p[0] == index) && (e->p[1] == selpoint[0])))
				{
					selpoint[0] = 0;
					e->flags |= EDGE_SELECTED;

					if(!selpoints)
					{
						selpoints = new UBYTE[maxpoints];
						if(!selpoints)
							return FALSE;
						memset(selpoints, 0, actpoints);
					}

					selpoints[e->p[0]] = POINT_SELECTED;
					selpoints[e->p[1]] = POINT_SELECTED;
					selflag = SELFLAG_NONE;
					flags |= OBJECT_HANDLEDIRTY | OBJECT_DIRTY;
					return TRUE;
				}
				if(e->flags & EDGE_SELECTED)
				{
					e->flags &= ~EDGE_SELECTED;
					e->flags |= EDGE_DIRTY;

					selpoints[e->p[0]] = POINT_DIRTY;
					selpoints[e->p[1]] = POINT_DIRTY;
				}
				e++;
			}
			edge = AddEdge(1);
			if(edge == -1)
				return FALSE;

			edges[edge].p[0] = selpoint[0];
			edges[edge].p[1] = index;
			edges[edge].flags |= EDGE_SELECTED;

			if(!selpoints)
			{
				selpoints = new UBYTE[maxpoints];
				if(!selpoints)
					return FALSE;
				memset(selpoints, 0, actpoints);
			}
			selpoints[selpoint[0]] = POINT_SELECTED;
			selpoints[index] = POINT_SELECTED;
			selflag = SELFLAG_NONE;
			flags |= OBJECT_HANDLEDIRTY | OBJECT_DIRTY;
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   Add a face to the mesh object
 * INPUT:         display  display
 *                x,y      position
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL MESH::AddFace(DISPLAY *display, int x, int y)
{
	int index=-1, tria, edge;
	MATRIX m;
	MATRIX_STACK stack;
	TRIANGLE *t;
	EDGE *e;
	int i, found;
	UWORD p1,p2,p3;

	GetObjectMatrix(&m);
	stack.Push(&m);

	TestSelectedPoint(display, x,y, TRUE, &stack, &index);
	if(index != -1)
	{
		if(selflag == SELFLAG_NONE)
		{
			selpoint[0] = index;
			selflag = SELFLAG_FIRST;
			flags |= OBJECT_HANDLEDIRTY | OBJECT_DIRTY;
			return TRUE;
		}
		else
		{
			if(selflag == SELFLAG_FIRST)
			{
				selpoint[1] = index;
				selflag = SELFLAG_SECOND;
				flags |= OBJECT_HANDLEDIRTY | OBJECT_DIRTY;
				return TRUE;
			}
			else
			{           // test if there is already an face between this points
				// and deselect all faces
				t = trias;
				for(i=0; i<acttrias; i++)
				{
					p1 = edges[t->e[0]].p[0];
					p2 = edges[t->e[0]].p[1];
					if((p1 != edges[t->e[1]].p[0]) && (p2 != edges[t->e[1]].p[0]))
						p3 = edges[t->e[1]].p[0];
					else
						p3 = edges[t->e[1]].p[1];
					if(((p1 == index) && (p2 == selpoint[0]) && (p3 == selpoint[1])) ||
						((p1 == index) && (p3 == selpoint[0]) && (p2 == selpoint[1])) ||
						((p2 == index) && (p1 == selpoint[0]) && (p3 == selpoint[1])) ||
						((p2 == index) && (p3 == selpoint[0]) && (p1 == selpoint[1])) ||
						((p3 == index) && (p1 == selpoint[0]) && (p2 == selpoint[1])) ||
						((p3 == index) && (p2 == selpoint[0]) && (p1 == selpoint[1])))
					{

						selpoint[0] = 0;
						selpoint[1] = 0;
						t->flags |= TRIA_SELECTED;
						edges[t->e[0]].flags |= EDGE_SELECTED;
						edges[t->e[1]].flags |= EDGE_SELECTED;
						edges[t->e[2]].flags |= EDGE_SELECTED;

						if(!selpoints)
						{
							selpoints = new UBYTE[maxpoints];
							if(!selpoints)
								return FALSE;
							memset(selpoints, 0, actpoints);
						}

						selpoints[edges[t->e[0]].p[0]] = POINT_SELECTED;
						selpoints[edges[t->e[0]].p[1]] = POINT_SELECTED;
						selpoints[edges[t->e[1]].p[0]] = POINT_SELECTED;
						selpoints[edges[t->e[1]].p[1]] = POINT_SELECTED;
						selpoints[edges[t->e[2]].p[0]] = POINT_SELECTED;
						selpoints[edges[t->e[2]].p[1]] = POINT_SELECTED;
						selflag = SELFLAG_NONE;
						flags |= OBJECT_HANDLEDIRTY | OBJECT_DIRTY;
						return TRUE;
					}

					if(t->flags & TRIA_SELECTED)
					{
						t->flags &= ~TRIA_SELECTED;
						t->flags |= TRIA_DIRTY;

						edges[t->e[0]].flags &= ~EDGE_SELECTED;
						edges[t->e[0]].flags |= EDGE_DIRTY;
						edges[t->e[1]].flags &= ~EDGE_SELECTED;
						edges[t->e[1]].flags |= EDGE_DIRTY;
						edges[t->e[2]].flags &= ~EDGE_SELECTED;
						edges[t->e[2]].flags |= EDGE_DIRTY;

						selpoints[edges[t->e[0]].p[0]] = POINT_DIRTY;
						selpoints[edges[t->e[0]].p[1]] = POINT_DIRTY;
						selpoints[edges[t->e[1]].p[0]] = POINT_DIRTY;
						selpoints[edges[t->e[1]].p[1]] = POINT_DIRTY;
						selpoints[edges[t->e[2]].p[0]] = POINT_DIRTY;
						selpoints[edges[t->e[2]].p[1]] = POINT_DIRTY;
					}
					t++;
				}

				tria = AddTria(1);
				if(tria == -1)
					return FALSE;

				e = edges;
				p1 = selpoint[0];
				p2 = selpoint[1];
				p3 = index;
				found = 0;
				for(i=0; (i < actedges) && (found != 7); i++)
				{
					if(((e->p[0] == p1) && (e->p[1] == p2)) ||
						((e->p[0] == p2) && (e->p[1] == p1)))
					{
						trias[tria].e[0] = i;
						found |= 1;
					}
					if(((e->p[0] == p2) && (e->p[1] == p3)) ||
						((e->p[0] == p3) && (e->p[1] == p2)))
					{
						trias[tria].e[1] = i;
						found |= 2;
					}
					if(((e->p[0] == p3) && (e->p[1] == p1)) ||
						((e->p[0] == p1) && (e->p[1] == p3)))
					{
						trias[tria].e[2] = i;
						found |= 4;
					}

					e++;
				}
				if(!(found & 1))
				{
					edge = AddEdge(1);
					if(!edge)
					{
						DeleteTria(tria);
						return FALSE;
					}
					edges[edge].p[0] = p1;
					edges[edge].p[1] = p2;
					trias[tria].e[0] = edge;
				}
				if(!(found & 2))
				{
					edge = AddEdge(1);
					if(!edge)
					{
						DeleteTria(tria);
						return FALSE;
					}
					edges[edge].p[0] = p2;
					edges[edge].p[1] = p3;
					trias[tria].e[1] = edge;
				}
				if(!(found & 4))
				{
					edge = AddEdge(1);
					if(!edge)
					{
						DeleteTria(tria);
						return FALSE;
					}
					edges[edge].p[0] = p3;
					edges[edge].p[1] = p1;
					trias[tria].e[2] = edge;
				}
				trias[tria].flags |= TRIA_SELECTED;

				edges[trias[tria].e[0]].flags |= EDGE_SELECTED;
				edges[trias[tria].e[1]].flags |= EDGE_SELECTED;
				edges[trias[tria].e[2]].flags |= EDGE_SELECTED;

				if(!selpoints)
				{
					selpoints = new UBYTE[maxpoints];
					if(!selpoints)
						return FALSE;
					memset(selpoints, 0, actpoints);
				}
				selpoints[selpoint[0]] = POINT_SELECTED;
				selpoints[selpoint[1]] = POINT_SELECTED;
				selpoints[index] = POINT_SELECTED;
				selflag = SELFLAG_NONE;
				flags |= OBJECT_HANDLEDIRTY | OBJECT_DIRTY;
			}
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   delete the edges which share a point
 * INPUT:         point
 * OUTPUT:        -
 *************/
void MESH::DeleteEdges(UWORD point)
{
	int i;
	EDGE *e;

	ASSERT(point < actpoints);

	for(i=0; i<actedges; i++)
	{
		e = &edges[i];

		if((e->p[0] == point) || (e->p[1] == point))
		{
			DeleteEdge(i);
			DeleteFaces(i);
			i--;
		}
		else
		{
			if(e->p[0] > point)
				e->p[0]--;
			if(e->p[1] > point)
				e->p[1]--;
		}
	}
}

/*************
 * DESCRIPTION:   delete the faces which share an edge
 * INPUT:         edge
 * OUTPUT:        -
 *************/
void MESH::DeleteFaces(UWORD edge)
{
	TRIANGLE *t;
	int i;

	ASSERT(edge < actedges);

	for(i=0; i<acttrias; i++)
	{
		t = &trias[i];

		if((t->e[0] == edge) || (t->e[1] == edge) || (t->e[2] == edge))
		{
			DeleteTria(i);
			i--;
		}
		else
		{
			if(t->e[0] > edge)
				t->e[0]--;
			if(t->e[1] > edge)
				t->e[1]--;
			if(t->e[2] > edge)
				t->e[2]--;
		}
	}
}

/*************
 * DESCRIPTION:   Delete the selected points
 * INPUT:         undo
 * OUTPUT:        always TRUE
 *************/
BOOL MESH::DeleteSelectedPoints(UNDO_DELETE *undo)
{
	int i;
	UBYTE *sp;

	ASSERT(undo);

	if(selpoints)
	{
		sp = selpoints;
		for(i=0; i<actpoints; i++)
		{
			if(*sp == POINT_SELECTED)
			{
				// point found: delete it
				DeletePoint(i);
				DeleteEdges(i);
				i--;
			}
			sp++;
		}
		delete selpoints;
		selpoints = NULL;
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   Delete the selected edges
 * INPUT:         undo
 * OUTPUT:        always TRUE
 *************/
BOOL MESH::DeleteSelectedEdges(UNDO_DELETE *undo)
{
	EDGE *e;
	int i;

	ASSERT(undo);

	e = edges;
	for(i=0; i<actedges; i++)
	{
		if(e->flags & EDGE_SELECTED)
		{
			DeleteEdge(i);
			DeleteFaces(i);
			i--;
		}
		else
			e++;
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   Delete the selected faces
 * INPUT:         undo
 * OUTPUT:        always TRUE
 *************/
BOOL MESH::DeleteSelectedTrias(UNDO_DELETE *undo)
{
	TRIANGLE *t;
	int i;

	ASSERT(undo);

	t = trias;
	for(i=0; i<acttrias; i++)
	{
		if(t->flags & TRIA_SELECTED)
		{
			edges[t->e[0]].flags &= ~EDGE_SELECTED;
			edges[t->e[1]].flags &= ~EDGE_SELECTED;
			edges[t->e[2]].flags &= ~EDGE_SELECTED;
			DeleteTria(i);
			i--;
		}
		else
			t++;
	}

	return TRUE;
}
