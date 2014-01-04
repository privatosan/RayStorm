/***************
 * PROGRAM:       Modeler
 * NAME:          display.cpp
 * DESCRIPTION:   display functions
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.11.95 ah    initial release
 *    09.09.96 mh    only blit bounding rectangle of objects
 *    05.02.97 ah    added DrawSelBox()
 *    14.10.97 mh    added GFX, shortened several routines
 ***************/

#include <string.h>
#include <math.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef __AMIGA__
#include <gl/gl.h>
#endif

#include <gl/glu.h>

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef VIEW_H
#include "view.h"
#endif

#ifndef ERRORS_H
#include "errors.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef MAGIC_H
#include "magic.h"
#endif

#ifndef OPENGL_UTIL_H
#include "ogl_util.h"
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
DISPLAY::DISPLAY()
{
	views[0] = new VIEW;
	views[1] = new VIEW;
	views[2] = new VIEW;
	views[3] = new VIEW;

	ToDefaults();
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
DISPLAY::~DISPLAY()
{
	int i;

	for (i = 0; i < 4; i++)
	{
		if (views[i])
			delete views[i];
	}
}

/*************
 * DESCRIPTION:   Set left top edge and size of viewport
 * INPUT:         -
 * OUTPUT:        -
 *************/
void DISPLAY::SetViewport(int w, int h)
{
	vpwidth = w;
	vpheight = h;
	SetMultiView(multiview);
}

/*************
 * DESCRIPTION:   Set value to default values
 * INPUT:         none
 * OUTPUT:        none
 *************/
void DISPLAY::ToDefaults()
{
	CAMERA *camera;
	PREFS p;

	camera = new CAMERA;
	if (!camera)
		return;

	camera->Insert(NULL, INSERT_HORIZ);

	this->camera = camera;

	bboxmin = camera->bboxmin;
	bboxmax = camera->bboxmax;

	views[0]->viewmode = VIEW_TOP;
	views[1]->viewmode = VIEW_PERSP;
	views[2]->viewmode = VIEW_FRONT;
	views[3]->viewmode = VIEW_RIGHT;
	view = views[1];

	display = DISPLAY_WIRE;
	editmode = EDIT_ROTATE;
	grid = TRUE;
	gridsnap = FALSE;
	gridsize = 1.f;
	always_coord = FALSE;
	coord_disp = FALSE;
	name_disp = FALSE;
	multiview = FALSE;
	p.id = ID_FLGS;
	if(p.GetPrefs())
	{
		usesurfacecolors = ((*((ULONG*)p.data)) & PREFFLAG_USESURFACECOLORS) ? TRUE : FALSE;
#ifdef __AMIGA__
		directrender = ((*((ULONG*)p.data)) & PREFFLAG_OPENGL_DIRECTRENDER) ? TRUE : FALSE;
#endif // __AMIGA__
	}
	else
	{
		usesurfacecolors = TRUE;
#ifdef __AMIGA__
		directrender = TRUE;
#endif // __AMIGA__
	}
	p.data = NULL;
	SetMultiView(FALSE);
	active = ACTIVE_X | ACTIVE_Y | ACTIVE_Z;

	filter_flags = FILTER_ALL;
	filter_flags ^= FILTER_BRUSH;
	filter_flags ^= FILTER_TEXTURE;

	localpos = localalign = localsize = FALSE;
}

/*************
 * DESCRIPTION:   Extend rectangle to fit x and y
 * INPUT:         r     rectangle
 *                x,y   point
 * OUTPUT:        -
 *************/
static void Extend(RECTANGLE *r, int x, int y)
{
	if (x < r->left)
		r->left = x;
	if (x > r->right)
		r->right = x;

	if (y < r->top)
		r->top = y;
	if (y > r->bottom)
		r->bottom = y;
}

/*************
 * DESCRIPTION:   convert rectangle 3d to 2d coordinates
 * INPUT:         r     rectangle
 *                x,y   point
 * OUTPUT:        -
 *************/
void DISPLAY::RectangleTo2D(RECTANGLE *r)
{
	VECTOR u;

	u = r->max;
	view->MCtoDC_xy(&u);
	Extend(r, (int)u.x, (int)u.y);

	SetVector(&u, r->max.x, r->max.y, r->min.z);
	view->MCtoDC_xy(&u);
	Extend(r, (int)u.x, (int)u.y);

	SetVector(&u, r->max.x, r->min.y, r->max.z);
	view->MCtoDC_xy(&u);
	Extend(r, (int)u.x, (int)u.y);

	SetVector(&u, r->max.x, r->min.y, r->min.z);
	view->MCtoDC_xy(&u);
	Extend(r, (int)u.x, (int)u.y);

	SetVector(&u, r->min.x, r->max.y, r->max.z);
	view->MCtoDC_xy(&u);
	Extend(r, (int)u.x, (int)u.y);

	SetVector(&u, r->min.x, r->max.y, r->min.z);
	view->MCtoDC_xy(&u);
	Extend(r, (int)u.x, (int)u.y);

	SetVector(&u, r->min.x, r->min.y, r->max.z);
	view->MCtoDC_xy(&u);
	Extend(r, (int)u.x, (int)u.y);

	u = r->min;
	view->MCtoDC_xy(&u);
	Extend(r, (int)u.x, int(u.y));
}

/*************
 * DESCRIPTION:   extends r1, so that r2 is within the area of r1
 * INPUT:         r1    first rectangle
 *                r2    second rectangle
 *                clipleft, cliptop, clipright, clipbottom     clipping rectangle
 * OUTPUT:        none
 *************/
void DISPLAY::UnionRectangle(RECTANGLE *r1, RECTANGLE *r2, SHORT clipleft, SHORT cliptop, SHORT clipright, SHORT clipbottom)
{
	RectangleTo2D(r2);

	// extend it with an extra area around it because of text display
	r2->left -= SAFETY_ZONE;
	r2->right += SAFETY_ZONE;
	r2->top -= SAFETY_ZONE;
	r2->bottom += SAFETY_ZONE;

	// clip to view-area
	if (r2->left < clipleft)
		r2->left = clipleft;
	else
	{
		if (r2->left > clipright)
			r2->left = clipright;
	}

	if (r2->top < cliptop)
		r2->top = cliptop;
	else
	{
		if (r2->top > clipbottom)
			r2->top = clipbottom;
	}

	if (r2->right > clipright)
		r2->right = clipright;
	else
	{
		if (r2->right < clipleft)
			r2->right = clipleft;
	}

	if (r2->bottom > clipbottom)
		r2->bottom = clipbottom;
	else
	{
		if (r2->bottom < cliptop)
			r2->bottom = cliptop;
	}

	if (r1->set)
	{
		if (r1->left > r2->left)
			r1->left = r2->left;
		if (r1->right < r2->right)
			r1->right = r2->right;
		if (r1->top > r2->top)
			r1->top = r2->top;
		if (r1->bottom < r2->bottom)
			r1->bottom = r2->bottom;
	}
	else
	{
		*r1 = *r2;
		r1->set = TRUE;
	}
}

/*************
 * DESCRIPTION:   grows the current accumulation rectangle (accrect), so that
 *    v is inside
 * INPUT:         v        point
 *                stack    Matrix Stack
 * OUTPUT:        none
 *************/
void DISPLAY::AccumulateRect(VECTOR *v, MATRIX_STACK *stack)
{
	stack->MultVectStack(v);

	if (!accrect.set)
	{
		// first time to add a point
		accrect.left   =  100000;
		accrect.top    =  100000;
		accrect.right  = -100000;
		accrect.bottom = -100000;
		accrect.min = accrect.max = *v;
		accrect.set = TRUE;
	}
	else
	{
		if (v->x < accrect.min.x)
			accrect.min.x = v->x;
		if (v->x > accrect.max.x)
			accrect.max.x = v->x;

		if (v->y < accrect.min.y)
			accrect.min.y = v->y;
		if (v->y > accrect.max.y)
			accrect.max.y = v->y;

		if (v->z < accrect.min.z)
			accrect.min.z = v->z;
		if (v->z > accrect.max.z)
			accrect.max.z = v->z;
	}
}

/*************
 * DESCRIPTION:   grows the current accumulation rectangle (accrect), so that
 *    both edges are inside
 * INPUT:         left,top,right,bottom   rectangle
 * OUTPUT:        none
 *************/
void DISPLAY::AccumulateRect2D(int left, int top, int right, int bottom)
{
	int t;

	if (left>right)
	{
		t = left;
		left = right;
		right = t;
	}
	if (top>bottom)
	{
		t = top;
		top = bottom;
		bottom = t;
	}

	if (!accrect.set)
	{
		// first time to add a point
		accrect.left   = left;
		accrect.top    = top;
		accrect.right  = right;
		accrect.bottom = bottom;
		SetVector(&accrect.min,  INFINITY,   INFINITY,   INFINITY);
		SetVector(&accrect.max, -INFINITY,  -INFINITY,  -INFINITY);
		accrect.set = TRUE;
	}
	else
	{
		if (left < accrect.left)
			accrect.left = left;
		if (left > accrect.right)
			accrect.right = left;

		if (top < accrect.top)
			accrect.top = top;
		if (top > accrect.bottom)
			accrect.bottom = top;

		if (right < accrect.left)
			accrect.left = right;
		if (right > accrect.right)
			accrect.right = right;

		if (bottom < accrect.top)
			accrect.top = bottom;
		if (bottom > accrect.bottom)
			accrect.bottom = bottom;
	}
}

/*************
 * DESCRIPTION:   grows the current accumulation rectangle (accrect), so that the
 *                bounding box given by min, max vector fits into it (called by OBJECT/DrawObjects)
 * INPUT:         min,max  bounding box
 *                stack    Matrix Stack
 * OUTPUT:        none
 *************/
void DISPLAY::AccumulateBoundingRect(OBJECT *obj, MATRIX_STACK *stack)
{
	VECTOR v;

	SetVector(&v, 1.1f * obj->size.x, 0.f, 0.f);
	AccumulateRect(&v, stack);
	SetVector(&v, 0.f, 1.1f * obj->size.y, 0.f);
	AccumulateRect(&v, stack);
	SetVector(&v, 0.f, 0.f, 1.1f * obj->size.z);
	AccumulateRect(&v, stack);

	VecScale(1.1f, &obj->bboxmax, &v);
	AccumulateRect(&v, stack);
	SetVector(&v, obj->bboxmax.x*1.1f, obj->bboxmax.y*1.1f, obj->bboxmin.z*1.1f);
	AccumulateRect(&v, stack);
	SetVector(&v, obj->bboxmax.x*1.1f, obj->bboxmin.y*1.1f, obj->bboxmax.z*1.1f);
	AccumulateRect(&v, stack);
	SetVector(&v, obj->bboxmax.x*1.1f, obj->bboxmin.y*1.1f, obj->bboxmin.z*1.1f);
	AccumulateRect(&v, stack);
	SetVector(&v, obj->bboxmin.x*1.1f, obj->bboxmax.y*1.1f, obj->bboxmax.z*1.1f);
	AccumulateRect(&v, stack);
	SetVector(&v, obj->bboxmin.x*1.1f, obj->bboxmax.y*1.1f, obj->bboxmin.z*1.1f);
	AccumulateRect(&v, stack);
	SetVector(&v, obj->bboxmin.x*1.1f, obj->bboxmin.y*1.1f, obj->bboxmax.z*1.1f);
	AccumulateRect(&v, stack);
	VecScale(1.1f, &obj->bboxmin, &v);
	AccumulateRect(&v, stack);
}

/*************
 * DESCRIPTION:   Transform a line, clip and draw it (perspective view)
 * INPUT:         v1,v2    line
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void DISPLAY::TransformDraw(VECTOR *u, VECTOR *v, MATRIX_STACK *stack)
{
	VECTOR v1,v2;

	v1 = *u;
	stack->MultVectStack(&v1);

	v2 = *v;
	stack->MultVectStack(&v2);

	if ((view->viewmode == VIEW_PERSP) || (view->viewmode == VIEW_CAMERA))
		if (!view->Clip3DLine(&v1,&v2))
			return;

	view->MCtoDC_xy(&v1);
	view->MCtoDC_xy(&v2);
	gfx.ClipLine((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y);
}

/*************
 * DESCRIPTION:   determine which window contains the given point
 * INPUT:         x,y   point to test
 * OUTPUT:        none
 *************/
VIEW *DISPLAY::SetViewByPoint(int x, int y)
{
	VIEW *vw;
	int i;

	if (!multiview)
	{
		if ((x < view->left) || (x > view->right) ||
			 (y < view->top) || (y > view->bottom))
		{
			return NULL;
		}
		return view;
	}

	for (i = 0; i < 4; i++)
	{
		vw = views[i];

		if ((x >= vw->left - 1) && (x <= vw->right + 1) &&
			 (y >= vw->top - 1) && (y <= vw->bottom + 1))
		{
			gfx.SetOutput(GFX_SCREEN);
			gfx.SetPen(COLOR_UNSELECTED);
			gfx.SetClipping(view->left, view->top, view->left+view->width-1, view->top+view->height-1);
			gfx.Rectangle(view->left, view->top, view->left+view->width-1, view->top+view->height-1);
			view = views[i];
			gfx.SetPen(COLOR_SELECTED);
			gfx.SetClipping(view->left, view->top, view->left+view->width-1, view->top+view->height-1);
			gfx.Rectangle(view->left, view->top, view->left+view->width-1, view->top+view->height-1);

			// enable or disable Set Render Window menu item
			switch(view->viewmode)
			{
				case VIEW_FRONT:
					utility.EnableMenuItem(MENU_SETRENDERWINDOW, FALSE);
					utility.CheckToolbarItem(TOOLBAR_VIEWFRONT);
					break;
				case VIEW_RIGHT:
					utility.EnableMenuItem(MENU_SETRENDERWINDOW, FALSE);
					utility.CheckToolbarItem(TOOLBAR_VIEWRIGHT);
					break;
				case VIEW_TOP:
					utility.EnableMenuItem(MENU_SETRENDERWINDOW, FALSE);
					utility.CheckToolbarItem(TOOLBAR_VIEWTOP);
					break;
				case VIEW_PERSP:
					utility.EnableMenuItem(MENU_SETRENDERWINDOW, FALSE);
					utility.CheckToolbarItem(TOOLBAR_VIEWPERSP);
					break;
				case VIEW_CAMERA:
					utility.EnableMenuItem(MENU_SETRENDERWINDOW, TRUE);
					utility.CheckToolbarItem(TOOLBAR_VIEWCAMERA);
					break;
			}
			return view;
		}
	}
//   ASSERT(FALSE);
	return NULL;
}

/*************
 * DESCRIPTION:   Draw a line from u to v
 * INPUT:         u,v      line
 * OUTPUT:        none
 *************/
void DISPLAY::DrawLine(VECTOR *u, VECTOR *v)
{
	VECTOR u1 = *u, v1 = *v;

	view->MCtoDC_xy(&u1);
	view->MCtoDC_xy(&v1);

	gfx.ClipLine((int)u1.x, (int)u1.y, (int)v1.x, (int)v1.y);
}

/*************
 * DESCRIPTION:   Draw coordinate cross
 * INPUT:         none
 * OUTPUT:        none
 *************/
void DISPLAY::DrawCross()
{
	int topx, topy, rightx, righty;
	char t, r;
	char *name;

	// write coordinates
	switch (view->viewmode)
	{
		case VIEW_FRONT:
			t = 'Y';
			r = 'X';
			name = "FRONT";
			break;
		case VIEW_TOP:
			t = 'Z';
			r = 'X';
			name = "TOP";
			break;
		case VIEW_RIGHT:
			t = 'Y';
			r = 'Z';
			name = "RIGHT";
			break;
		case VIEW_CAMERA:
			name = "CAMERA";
			break;
		case VIEW_PERSP:
			name = "PERSPECTIVE";
			break;
		default:
			return;
	}

	gfx.SetPen(COLOR_TEXT);
	gfx.TextXY(view->left + 1, view->top + gfx.GetTextExtentY() + 1, name);

	if((view->viewmode == VIEW_CAMERA) || (view->viewmode == VIEW_PERSP))
		return;

	topx = view->left + (view->width >> 1) - (gfx.GetTextExtentX("X") >> 1);
	topy = view->top + gfx.GetTextExtentY() + 1;
	rightx = view->right - gfx.GetTextExtentX("X") - 1;
	righty = view->top + (view->height >> 1) + (gfx.GetTextExtentY() >> 1);

	gfx.TextXY(topx, topy, &t, 1);
	gfx.TextXY(rightx, righty, &r, 1);
}

/*************
 * DESCRIPTION:   Draw selection box
 * INPUT:         -
 * OUTPUT:        -
 *************/
void DISPLAY::DrawSelBox()
{
	gfxMode oldMode;
	gfxOutput oldOutput;

	oldOutput = gfx.SetOutput(GFX_SCREEN);
	gfx.SetPen(COLOR_UNSELECTED);
	oldMode = gfx.SetWriteMode(GFX_COMPLEMENT_DOTTED);

	gfx.Rectangle(selbox.left, selbox.top, selbox.right, selbox.bottom);

	gfx.SetWriteMode(oldMode);
	gfx.SetOutput(oldOutput);
}

/*************
 * DESCRIPTION:   Draw grid
 * INPUT:         -
 * OUTPUT:        -
 *************/
void DISPLAY::DrawGrid()
{
	gfxMode oldMode;
	VECTOR v1, v2, origin;
	float dx, dy, t1, t2, viewh, viewv;
	int i;
	if (!grid)
		return;

#ifdef __OPENGL__
	if(display == DISPLAY_SOLID)
	{
		OpenGLDrawGrid(this);
	}
	else
#endif // __OPENGL__
	{
		gfx.SetPen(COLOR_GRID);
		oldMode = gfx.SetWriteMode(GFX_DOTTED);

		switch (view->viewmode)
		{
			case VIEW_CAMERA:
			case VIEW_PERSP:
				dx = -SUBDIV*gridsize;
				t1 = SUBDIV*gridsize;
				for (i = 0; i <= 2*SUBDIV; i++)
				{
					SetVector(&v1, t1, 0.f, dx);
					SetVector(&v2, -t1, 0.f, dx);

					if (view->Clip3DLine(&v1, &v2))
					{
						view->MCtoDC_xy(&v1);
						view->MCtoDC_xy(&v2);
						gfx.ClipLine((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y);
					}
					SetVector(&v1, dx, 0.f, t1);
					SetVector(&v2, dx, 0.f, -t1);

					if (view->Clip3DLine(&v1, &v2))
					{
						view->MCtoDC_xy(&v1);
						view->MCtoDC_xy(&v2);
						gfx.ClipLine((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y);
					}
					dx += gridsize;
				}
				gfx.SetWriteMode(oldMode);
				return;
			
			case VIEW_FRONT:
				viewh = view->viewpoint.x;
				viewv = view->viewpoint.y;
				break;

			case VIEW_RIGHT:
				viewh = view->viewpoint.z;
				viewv = view->viewpoint.y;
				break;
			
			case VIEW_TOP:
				viewh = view->viewpoint.x;
				viewv = view->viewpoint.z;
				break;
		}

		t1 = view->width/view->zoom*0.5f + 2*gridsize;
		t2 = view->height/view->zoom*0.5f+ 2*gridsize;
		dx = viewh - fmod(viewh, gridsize) - (t1 - fmod(t1, gridsize));
		dy = viewv - fmod(viewv, gridsize) - (t2 - fmod(t2, gridsize));

		switch (view->viewmode)
		{
			case VIEW_FRONT:
				SetVector(&v1, dx, dy, 0.f);
				SetVector(&v2, dx + gridsize, dy + gridsize, 0.f);
				break;
			case VIEW_RIGHT:
				SetVector(&v1, 0.f, dy, dx);
				SetVector(&v2, 0.f, dy + gridsize, dx + gridsize);
				break;
			case VIEW_TOP:
				SetVector(&v1, dx, 0.f, dy);
				SetVector(&v2, dx + gridsize, 0.f, dy + gridsize);
				break;
		}

		view->MCtoDC_xy(&v1);
		view->MCtoDC_xy(&v2);

		dx = v2.x - v1.x;
		dy = v2.y - v1.y;

		SetVector(&origin, 0.f, 0.f, 0.f);
		view->MCtoDC_xy(&origin);

		if (dx > MIN_GRIDPIXELS)
		{
			v1.x -= dx;
			v1.y -= dy;
			do
			{
				// do not draw zero lines yet
				if (fabs(v1.x - origin.x) > 1)
					gfx.ClipLine((int)v1.x, view->top, (int)v1.x, view->bottom);
				if (fabs(v1.y - origin.y) > 1)
					gfx.ClipLine(view->left, (int)v1.y, view->right, (int)v1.y);
				v1.x += dx;
				v1.y += dy;
			}
			while (v1.x < view->right || v1.y > view->top);
		}

		gfx.SetWriteMode(oldMode);

		// draw horizontal and vertical zero line
		gfx.ClipLine((int)origin.x, view->top, (int)origin.x, view->bottom);
		gfx.ClipLine(view->left, (int)origin.y, view->right, (int)origin.y);
	}
}

/*************
 * DESCRIPTION:   draw the track (click) point
 * INPUT:         x, y     coordinates
 * OUTPUT:        none
 *************/
void DISPLAY::DrawTrack(int x, int y)
{
	gfx.FillRect(x - TRACKWIDTH, y - TRACKWIDTH, x + TRACKWIDTH, y + TRACKWIDTH);
}

/*************
 * DESCRIPTION:   returns the render area (in screen coordinates)
 * INPUT:         pointer to rectangle where the information will be stored
 * OUTPUT:        none
 *************/
void DISPLAY::GetRenderRect(RECTANGLE *r)
{
	float delta;

	delta = view->width*(camera->vfov/camera->hfov);

	if (delta < view->height)
	{
		delta *= 0.5f;
		r->left = view->left;
		r->top = (int)((float)view->height*0.5f + (float)view->top - delta);
		r->right = view->right;
		r->bottom = (int)((float)view->height*0.5f + (float)view->top + delta);

	}
	else
	{
		delta = view->height * (camera->hfov / camera->vfov) * 0.5f;
		r->left = (int)((float)view->width * 0.5f + (float)view->left - delta);
		r->top = view->top;
		r->right = (int)((float)view->width * 0.5f + (float)view->left + delta);
		r->bottom = view->bottom;
	}
}

/*************
 * DESCRIPTION:   Draws help lines to perspective window to show the size
 *    of the rendered picture. Also draw lines which show the rendering box.
 * INPUT:         remove      TRUE: remove lines
 * OUTPUT:        none
 *************/
void DISPLAY::DrawRenderLines(BOOL remove)
{
	RECTANGLE r;
	int w, h;
	gfxMode oldMode;

	GetRenderRect(&r);

#ifdef __OPENGL__
	if(display == DISPLAY_SOLID)
	{
		OpenGLDrawRenderLines(this, &r, remove);
	}
	else
#endif // __OPENGL__
	{
		if (remove)
			gfx.SetPen(COLOR_BKGROUND);
		else
			gfx.SetPen(COLOR_UNSELECTED);
		oldMode = gfx.SetWriteMode(GFX_DOTTED);

		if (r.top == view->top)
		{
			gfx.ClipLine(r.left, r.top, r.left, r.bottom);
			gfx.ClipLine(r.right, r.top, r.right, r.bottom);
		}
		else
		{
			gfx.ClipLine(r.left, r.top, r.right, r.top);
			gfx.ClipLine(r.left, r.bottom, r.right, r.bottom);
		}

		if (global.enablearea)
		{
			w = r.right - r.left;
			h = r.bottom - r.top;
			gfx.Rectangle((int)(w*global.xmin + r.left), (int)(h*global.ymin + r.top),
							  (int)(w*global.xmax + r.left), (int)(h*global.ymax + r.top));
		}
		gfx.SetWriteMode(oldMode);
	}
}

/*************
 * DESCRIPTION:   Draw the bounding box which indicates that an object is selected
 * INPUT:         stack    matrix stack
 *                min      min size
 *                max      max size
 * OUTPUT:        none
 *************/
void DISPLAY::DrawSelBox(MATRIX_STACK *stack, VECTOR *min_, VECTOR *max_)
{
	VECTOR u,v, min,max, min1,max1;

	SetVector(&min,
		min_->x * 1.1f,
		min_->y * 1.1f,
		min_->z * 1.1f);

	SetVector(&max,
		max_->x * 1.1f,
		max_->y * 1.1f,
		max_->z * 1.1f);

	SetVector(&min1,
		max.x*.2f + min.x*.8f,
		max.y*.2f + min.y*.8f,
		max.z*.2f + min.z*.8f);
	SetVector(&max1,
		max.x*.8f + min.x*.2f,
		max.y*.8f + min.y*.2f,
		max.z*.8f + min.z*.2f);

#ifdef __OPENGL__
	if(display == DISPLAY_SOLID)
	{
		OpenGLDrawSelBox(&min, &min1, &max, &max1);
	}
	else
#endif // __OPENGL__
	{
		gfx.SetPen(COLOR_SELBOX);

		u = max;
		SetVector(&v, max1.x, max.y, max.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, max.x, max1.y, max.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, max.x, max.y, max1.z);
		TransformDraw(&u,&v,stack);

		SetVector(&u, max.x, max.y, min.z);
		SetVector(&v, max1.x, max.y, min.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, max.x, max1.y, min.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, max.x, max.y, min1.z);
		TransformDraw(&u,&v,stack);

		SetVector(&u, max.x, min.y, max.z);
		SetVector(&v, max1.x, min.y, max.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, max.x, min1.y, max.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, max.x, min.y, max1.z);
		TransformDraw(&u,&v,stack);

		SetVector(&u, max.x, min.y, min.z);
		SetVector(&v, max1.x, min.y, min.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, max.x, min1.y, min.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, max.x, min.y, min1.z);
		TransformDraw(&u,&v,stack);

		SetVector(&u, min.x, max.y, max.z);
		SetVector(&v, min1.x, max.y, max.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, min.x, max1.y, max.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, min.x, max.y, max1.z);
		TransformDraw(&u,&v,stack);

		SetVector(&u, min.x, max.y, min.z);
		SetVector(&v, min1.x, max.y, min.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, min.x, max1.y, min.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, min.x, max.y, min1.z);
		TransformDraw(&u,&v,stack);

		SetVector(&u, min.x, min.y, max.z);
		SetVector(&v, min1.x, min.y, max.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, min.x, min1.y, max.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, min.x, min.y, max1.z);
		TransformDraw(&u,&v,stack);

		u = min;
		SetVector(&v, min1.x, min.y, min.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, min.x, min1.y, min.z);
		TransformDraw(&u,&v,stack);
		SetVector(&v, min.x, min.y, min1.z);
		TransformDraw(&u,&v,stack);
	}
}

/*************
 * DESCRIPTION:   Draw bounding box
 * INPUT:         stack    matrix stack
 *                min      min size
 *                max      max size
 * OUTPUT:        none
 *************/
void DISPLAY::DrawBox(MATRIX_STACK *stack, VECTOR *min, VECTOR *max)
{
	VECTOR u,v;

#ifdef __OPENGL__
	if(display == DISPLAY_SOLID)
	{
		OpenGLDrawBox(min, max);
	}
	else
	{
#endif // __OPENGL__
		u = *min;
		SetVector(&v, max->x, min->y, min->z);
		TransformDraw(&u,&v,stack);

		v.x = min->x;
		v.y = max->y;
		TransformDraw(&u,&v,stack);

		v.y = min->y;
		v.z = max->z;
		TransformDraw(&u,&v,stack);

		SetVector(&u, max->x, max->y, min->z);
		SetVector(&v, min->x, max->y, min->z);
		TransformDraw(&u,&v,stack);

		v.x = max->x;
		v.y = min->y;
		TransformDraw(&u,&v,stack);

		v.y = max->y;
		v.z = max->z;
		TransformDraw(&u,&v,stack);

		SetVector(&u, min->x, max->y, max->z);
		SetVector(&v, min->x, min->y, max->z);
		TransformDraw(&u,&v,stack);

		v.x = max->x;
		v.y = max->y;
		TransformDraw(&u,&v,stack);

		v.x = min->x;
		v.z = min->z;
		TransformDraw(&u,&v,stack);

		SetVector(&u, max->x, min->y, max->z);
		SetVector(&v, max->x, min->y, min->z);
		TransformDraw(&u,&v,stack);

		v.x = min->x;
		v.z = max->z;
		TransformDraw(&u,&v,stack);

		v.x = max->x;
		v.y = max->y;
		TransformDraw(&u,&v,stack);
#ifdef __OPENGL__
	}
#endif // __OPENGL__
}

/*************
 * DESCRIPTION:   Draw a sphere
 * INPUT:         stack    matrix stack
 *                radius
 * OUTPUT:        none
 *************/
void DISPLAY::DrawSphere(MATRIX_STACK *stack, float radius)
{
	MATRIX m;
	SHORT x1,y1, x2,y2;
	VECTOR u,v,w;
	int i;

#ifdef __OPENGL__
	if(display == DISPLAY_SOLID)
	{
		OpenGLDrawSphere(radius);
	}
	else
	{
#endif // __OPENGL__
		// draw a wireframe sphere
		if((view->viewmode == VIEW_PERSP) || (view->viewmode == VIEW_CAMERA))
		{
			SetVector(&u, 0.f, 0.f, radius);
			v.x = 0.f;
			for(i=SPHERE_DIVS; i<=360; i+=SPHERE_DIVS)
			{
				v.y = sin(i*PI_180)*radius;
				v.z = cos(i*PI_180)*radius;
				TransformDraw(&u,&v,stack);
				u = v;
			}

			SetVector(&u, 0.f, 0.f, radius);
			v.y = 0.f;
			for(i=SPHERE_DIVS; i<=360; i+=SPHERE_DIVS)
			{
				v.x = sin(i*PI_180)*radius;
				v.z = cos(i*PI_180)*radius;
				TransformDraw(&u,&v,stack);
				u = v;
			}

			SetVector(&u, 0.f, radius, 0.f);
			v.z = 0.f;
			for(i=SPHERE_DIVS; i<=360; i+=SPHERE_DIVS)
			{
				v.x = sin(i*PI_180)*radius;
				v.y = cos(i*PI_180)*radius;
				TransformDraw(&u,&v,stack);
				u = v;
			}
		}
		else
		{
			m.MultMat(stack->GetMatrix(), view->GetViewMatrix());

			SetVector(&w, 0.f, 0.f, radius);
			view->MCtoDC2_xy_ortho(&m, &w, &x1, &y1);
			for(i=SPHERE_DIVS; i<=360; i+=SPHERE_DIVS)
			{
				SetVector(&u, 0.f, sin(i*PI_180)*radius, cos(i*PI_180)*radius);
				view->MCtoDC2_xy_ortho(&m, &u, &x2, &y2);
				gfx.ClipLine(x1,y1, x2,y2);
				x1 = x2;
				y1 = y2;
			}
			SetVector(&w, 0.f, 0.f, radius);
			view->MCtoDC2_xy_ortho(&m, &w, &x1, &y1);
			for(i=SPHERE_DIVS; i<=360; i+=SPHERE_DIVS)
			{
				SetVector(&u, sin(i*PI_180)*radius, 0.f, cos(i*PI_180)*radius);
				view->MCtoDC2_xy_ortho(&m, &u, &x2, &y2);
				gfx.ClipLine(x1,y1, x2,y2);
				x1 = x2;
				y1 = y2;
			}
			SetVector(&w, 0.f, radius, 0.f);
			view->MCtoDC2_xy_ortho(&m, &w, &x1, &y1);
			for(i=SPHERE_DIVS; i<=360; i+=SPHERE_DIVS)
			{
				SetVector(&u, sin(i*PI_180)*radius, cos(i*PI_180)*radius, 0.f);
				view->MCtoDC2_xy_ortho(&m, &u, &x2, &y2);
				gfx.ClipLine(x1,y1, x2,y2);
				x1 = x2;
				y1 = y2;
			}
		}
#ifdef __OPENGL__
	}
#endif // __OPENGL__
}

/*************
 * DESCRIPTION:   Draw a cylinder
 * INPUT:         stack    matrix stack
 *                size     size
 * OUTPUT:        none
 *************/
void DISPLAY::DrawCylinder(MATRIX_STACK *stack, VECTOR *size, ULONG flags)
{
	VECTOR bottom,bottom1,centerbottom, top,top1,centertop;
	int i;
	MATRIX m;
	SHORT xbottom,ybottom, xbottom1,ybottom1, xcenterbottom,ycenterbottom;
	SHORT xtop,ytop, xtop1,ytop1, xcentertop,ycentertop;

#ifdef __OPENGL__
	if(display == DISPLAY_SOLID)
	{
		OpenGLDrawCylinder(size, flags);
	}
	else
	{
#endif // __OPENGL__
		// draw a wireframe cylinder
		if((view->viewmode == VIEW_PERSP) || (view->viewmode == VIEW_CAMERA))
		{
			SetVector(&bottom, 0.f, 0.f, size->z);
			SetVector(&centerbottom, 0.f, 0.f, 0.f);
			SetVector(&top, 0.f, size->y, size->z);
			SetVector(&centertop, 0.f, size->y, 0.f);
			for(i=CYLINDER_DIVS; i<=360; i+=CYLINDER_DIVS)
			{
				TransformDraw(&bottom,&top,stack);

				SetVector(&bottom1, sin(i*PI_180)*size->x, 0.f, cos(i*PI_180)*size->z);
				TransformDraw(&bottom,&bottom1,stack);
				if(!(flags & OBJECT_OPENBOTTOM))
					TransformDraw(&bottom, &centerbottom, stack);

				top1.x = bottom1.x;
				top1.y = size->y;
				top1.z = bottom1.z;
				TransformDraw(&top,&top1,stack);
				if(!(flags & OBJECT_OPENTOP))
					TransformDraw(&top, &centertop, stack);

				bottom = bottom1;
				top = top1;
			}
		}
		else
		{
			m.MultMat(stack->GetMatrix(), view->GetViewMatrix());


			SetVector(&bottom, 0.f, 0.f, size->z);
			view->MCtoDC2_xy_ortho(&m, &bottom, &xbottom, &ybottom);
			SetVector(&centerbottom, 0.f, 0.f, 0.f);
			view->MCtoDC2_xy_ortho(&m, &centerbottom, &xcenterbottom, &ycenterbottom);

			SetVector(&top, 0.f, size->y, size->z);
			view->MCtoDC2_xy_ortho(&m, &top, &xtop, &ytop);
			SetVector(&centertop, 0.f, size->y, 0.f);
			view->MCtoDC2_xy_ortho(&m, &centertop, &xcentertop, &ycentertop);

			for(i=CYLINDER_DIVS; i<=360; i+=CYLINDER_DIVS)
			{
				gfx.ClipLine(xbottom,ybottom, xtop,ytop);

				SetVector(&bottom1, sin(i*PI_180)*size->x, 0.f, cos(i*PI_180)*size->z);
				view->MCtoDC2_xy_ortho(&m, &bottom1, &xbottom1, &ybottom1);
				gfx.ClipLine(xbottom,ybottom, xbottom1,ybottom1);
				if(!(flags & OBJECT_OPENBOTTOM))
					gfx.ClipLine(xbottom,ybottom, xcenterbottom,ycenterbottom);
				xbottom = xbottom1;
				ybottom = ybottom1;

				top1.x = bottom1.x;
				top1.y = size->y;
				top1.z = bottom1.z;
				view->MCtoDC2_xy_ortho(&m, &top1, &xtop1, &ytop1);
				gfx.ClipLine(xtop,ytop, xtop1,ytop1);
				if(!(flags & OBJECT_OPENTOP))
					gfx.ClipLine(xtop,ytop, xcentertop,ycentertop);
				xtop = xtop1;
				ytop = ytop1;
			}
		}
#ifdef __OPENGL__
	}
#endif // __OPENGL__
}

/*************
 * DESCRIPTION:   Test is a bounding box is completely out of the screen
 * INPUT:         stack    matrix stack
 *                min      min size
 *                max      max size
 * OUTPUT:        TRUE if outside else FALSE
 *************/
BOOL DISPLAY::OutOfScreen(MATRIX_STACK *stack, VECTOR *min, VECTOR *max)
{
	SHORT x0,y0, x1,y1, x2,y2, x3,y3, x4,y4, x5,y5, x6,y6, x7,y7;
	VECTOR u;
	MATRIX m;

	if (view->viewmode == VIEW_PERSP || view->viewmode == VIEW_CAMERA)
	{
		// in perspective view additionally test if behind view plane
		u = *max;
		stack->MultVectStack(&u);
		if (view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x0 = (SHORT)u.x;
		y0 = (SHORT)u.y;
		if (x0 > view->left && x0 < view->right && y0 > view->top && y0 < view->bottom)
			return FALSE;

		SetVector(&u, min->x, max->y, max->z);
		stack->MultVectStack(&u);
		if (view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x1 = (SHORT)u.x;
		y1 = (SHORT)u.y;
		if (x1 > view->left && x1<view->right && y1 > view->top && y1 < view->bottom)
			return FALSE;

		SetVector(&u, min->x, max->y, min->z);
		stack->MultVectStack(&u);
		if (view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x2 = (SHORT)u.x;
		y2 = (SHORT)u.y;
		if (x2 > view->left && x2 < view->right && y2 > view->top && y2 < view->bottom)
			return FALSE;

		SetVector(&u, max->x, max->y, min->z);
		stack->MultVectStack(&u);
		if (view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x3 = (SHORT)u.x;
		y3 = (SHORT)u.y;
		if (x3 > view->left && x3 < view->right && y3 > view->top && y3 < view->bottom)
			return FALSE;

		SetVector(&u, max->x, min->y, max->z);
		stack->MultVectStack(&u);
		if (view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x4 = (SHORT)u.x;
		y4 = (SHORT)u.y;
		if (x4 > view->left && x4 < view->right && y4 > view->top && y4 < view->bottom)
			return FALSE;

		SetVector(&u, min->x, min->y, max->z);
		stack->MultVectStack(&u);
		if (view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x5 = (SHORT)u.x;
		y5 = (SHORT)u.y;
		if (x5 > view->left && x5 < view->right && y5 > view->top && y5 < view->bottom)
			return FALSE;

		u = *min;
		stack->MultVectStack(&u);
		if (view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x6 = (SHORT)u.x;
		y6 = (SHORT)u.y;
		if (x6 > view->left && x6 < view->right && y6 > view->top && y6 < view->bottom)
			return FALSE;

		SetVector(&u, max->x, min->y, min->z);
		stack->MultVectStack(&u);
		if (view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x7 = (SHORT)u.x;
		y7 = (SHORT)u.y;
		if (x7 > view->left && x7 < view->right && y7 > view->top && y7 < view->bottom)
			return FALSE;
	}
	else
	{
		m.MultMat(stack->GetMatrix(), view->GetViewMatrix());

		view->MCtoDC2_xy_ortho(&m, max, &x0, &y0);
		if (x0 > view->left && x0 < view->right && y0 > view->top && y0 < view->bottom)
			return FALSE;

		SetVector(&u, min->x, max->y, max->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x1, &y1);
		if (x1 > view->left && x1 < view->right && y1 > view->top && y1 < view->bottom)
			return FALSE;

		SetVector(&u, min->x, max->y, min->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x2, &y2);
		if (x2 > view->left && x2 < view->right && y2 > view->top && y2 < view->bottom)
			return FALSE;

		SetVector(&u, max->x, max->y, min->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x3, &y3);
		if (x3 > view->left && x3 < view->right && y3 > view->top && y3 < view->bottom)
			return FALSE;

		SetVector(&u, max->x, min->y, max->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x4, &y4);
		if (x4 > view->left && x4 < view->right && y4 > view->top && y4 < view->bottom)
			return FALSE;

		SetVector(&u, min->x, min->y, max->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x5, &y5);
		if (x5 > view->left && x5 < view->right && y5 > view->top && y5 < view->bottom)
			return FALSE;

		view->MCtoDC2_xy_ortho(&m, min, &x6, &y6);
		if (x6 > view->left && x6 < view->right && y6 > view->top && y6 < view->bottom)
			return FALSE;

		SetVector(&u, max->x, min->y, min->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x7, &y7);
		if (x7 > view->left && x7 < view->right && y7 > view->top && y7 < view->bottom)
			return FALSE;
	}

	// completely outside of left border
	if (x0 < view->left && x1 < view->left && x2 < view->left && x3 < view->left &&
		 x4 < view->left && x5 < view->left && x6 < view->left && x7 < view->left)
		return TRUE;
	// completely outside of right border
	if (x0>view->right && x1>view->right && x2>view->right && x3>view->right &&
		 x4>view->right && x5>view->right && x6>view->right && x7>view->right)
		return TRUE;
	// completely outside of top border
	if (y0 < view->top && y1 < view->top && y2 < view->top && y3 < view->top &&
		 y4 < view->top && y5 < view->top && y6 < view->top && y7 < view->top)
		return TRUE;
	// completely outside of bottom border
	if (y0 > view->bottom && y1 > view->bottom && y2 > view->bottom && y3 > view->bottom &&
		 y4 > view->bottom && y5 > view->bottom && y6 > view->bottom && y7 > view->bottom)
		return TRUE;

	return FALSE;
}

/*************
 * DESCRIPTION:   Test is a bounding box is completely in view bounds
 * INPUT:         stack    matrix stack
 *                min      min size
 *                max      max size
 * OUTPUT:        TRUE if inside else FALSE
 *************/
BOOL DISPLAY::CompletelyInScreen(MATRIX_STACK *stack, VECTOR *min, VECTOR *max)
{
	SHORT x,y;
	VECTOR u;
	MATRIX m;

	if (view->viewmode == VIEW_PERSP || view->viewmode == VIEW_CAMERA)
	{
		// in perspective view additionally test if behind view plane
		u = *max;
		stack->MultVectStack(&u);
		if (!view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x = (SHORT)u.x;
		y = (SHORT)u.y;
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, min->x, max->y, max->z);
		stack->MultVectStack(&u);
		if (!view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x = (SHORT)u.x;
		y = (SHORT)u.y;
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, min->x, max->y, min->z);
		stack->MultVectStack(&u);
		if (!view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x = (SHORT)u.x;
		y = (SHORT)u.y;
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, max->x, max->y, min->z);
		stack->MultVectStack(&u);
		if (!view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x = (SHORT)u.x;
		y = (SHORT)u.y;
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, max->x, min->y, max->z);
		stack->MultVectStack(&u);
		if (!view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x = (SHORT)u.x;
		y = (SHORT)u.y;
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, min->x, min->y, max->z);
		stack->MultVectStack(&u);
		if (!view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x = (SHORT)u.x;
		y = (SHORT)u.y;
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		u = *min;
		stack->MultVectStack(&u);
		if (!view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x = (SHORT)u.x;
		y = (SHORT)u.y;
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, max->x, min->y, min->z);
		stack->MultVectStack(&u);
		if (!view->InView(&u))
			return FALSE;
		view->MCtoDC_xy(&u);
		x = (SHORT)u.x;
		y = (SHORT)u.y;
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;
	}
	else
	{
		m.MultMat(stack->GetMatrix(), view->GetViewMatrix());

		view->MCtoDC2_xy_ortho(&m, max, &x, &y);
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, min->x, max->y, max->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x, &y);
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, min->x, max->y, min->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x, &y);
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, max->x, max->y, min->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x, &y);
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, max->x, min->y, max->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x, &y);
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, min->x, min->y, max->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x, &y);
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		view->MCtoDC2_xy_ortho(&m, min, &x, &y);
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;

		SetVector(&u, max->x, min->y, min->z);
		view->MCtoDC2_xy_ortho(&m, &u, &x, &y);
		if (x < view->left || x > view->right || y < view->top || y > view->bottom)
			return FALSE;
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   Draw the name for an object
 * INPUT:         stack       matrix stack
 *                text        name of object
 * OUTPUT:        none
 *************/
void DISPLAY::DrawName(MATRIX_STACK *stack, char *name)
{
	SHORT len,height;
	gfxMode oldMode;
	VECTOR u;

	SetVector(&u, 0.f, 0.f, 0.f);
	stack->MultVectStack(&u);
	view->MCtoDC_xy(&u);
	u.x += TRACKWIDTH;

	gfx.SetPen(COLOR_TEXT);
	oldMode = gfx.SetWriteMode(GFX_TRANSPARENT);

	len = gfx.GetTextExtentX(name);
	height = gfx.GetTextExtentY() >> 1;

	if ((SHORT)u.y - height > view->top &&
		 (SHORT)u.y + height < view->bottom &&
		 (SHORT)u.x + len < view->right &&
		 (SHORT)u.x > view->left)
	{
		AccumulateRect2D((int)u.x, (int)u.y - height, (int)u.x + len, (int)u.y + height);
		gfx.TextXY((int)u.x, (int)u.y + height, name);
	}

	gfx.SetWriteMode(oldMode);
}

/*************
 * DESCRIPTION:   Draw the axis for an object
 * INPUT:         stack       matrix stack
 *                flags       AXIS_???
 *                size        axis size
 * OUTPUT:        -
 *************/
void DISPLAY::DrawAxis(MATRIX_STACK *stack, ULONG flags, VECTOR *size)
{
	VECTOR center, u, v, vh;
	int i, len, height;
	char xyz[3];

	xyz[0] = 'X';
	xyz[1] = 'Y';
	xyz[2] = 'Z';

	SetVector(&u, 0.f, 0.f, 0.f);
	stack->MultVectStack(&u);

	height = gfx.GetTextExtentY() >> 1;
	len = gfx.GetTextExtentX("X") >> 1;

	if(flags & AXIS_SELECTED)
	{
		if(flags & AXIS_REMOVE)
			gfx.SetPen(COLOR_BKGROUND);
		else
			gfx.SetPen(COLOR_AXIS);

		// draw the three axes
		for (i = 0; i < 3; i++)
		{
			switch (i)
			{
				case 0 : SetVector(&vh, size->x, 0.f, 0.f); break;
				case 1 : SetVector(&vh, 0.f, size->y, 0.f); break;
				case 2 : SetVector(&vh, 0.f, 0.f, size->z); break;
			}

			center = u;
			SetVector(&v, 0.f, 0.f, 0.f);
			v = vh;
			stack->MultVectStack(&v);
			if (view->Clip3DLine(&center, &v))
			{
				view->MCtoDC_xy(&center);
				view->MCtoDC_xy(&v);
				gfx.ClipLine((int)center.x, (int)center.y, (int)v.x, (int)v.y);
			}

			SetVector(&v, 0.f, 0.f, 0.f);
			v = vh;
			VecScale(1.1f, &v, &v);
			stack->MultVectStack(&v);
			if (view->InView(&v))
			{
				view->MCtoDC_xy(&v);
				gfx.TextXY((int)v.x - len, (int)v.y + height, &xyz[i], 1);
			}
		}
	}
	if(flags & AXIS_REMOVE)
		gfx.SetPen(COLOR_BKGROUND);
	else
		gfx.SetPen(COLOR_HOTSPOT);
	if (view->InView(&u))
	{
		view->MCtoDC_xy(&u);
		DrawTrack((SHORT)u.x, (SHORT)u.y); // draw dot
	}
}

/*************
 * DESCRIPTION:   Switch multiview on or off
 * INPUT:         mode  TRUE->multiview on else off
 * OUTPUT:        -
 *************/
void DISPLAY::SetMultiView(BOOL mode)
{
	int wh, hh;
#if defined(__OPENGL__)
	int i;
#endif // __OPENGL__

	if (mode)
	{
		wh = vpwidth/2;
		hh = vpheight/2;

#ifdef __OPENGL__
		if(display == DISPLAY_SOLID)
		{
#ifdef __AMIGA__
			views[0]->opengl.CreateContext(1,1, wh-3, hh-3, directrender);
			views[1]->opengl.CreateContext(wh+1,1, wh-3, hh-3, directrender);
			views[2]->opengl.CreateContext(1,hh+1, wh-3, hh-3, directrender);
			views[3]->opengl.CreateContext(wh+1,hh+1, wh-3, hh-3, directrender);
#else // __AMIGA__
			views[0]->opengl.CreateContext(1,1, wh-3, hh-3);
			views[1]->opengl.CreateContext(wh+1,1, wh-3, hh-3);
			views[2]->opengl.CreateContext(1,hh+1, wh-3, hh-3);
			views[3]->opengl.CreateContext(wh+1,hh+1, wh-3, hh-3);
#endif // __AMIGA__
		}
		else
		{
			for(i=0; i<4; i++)
			{
				if(views[i]->opengl.IsInitialized())
					views[i]->opengl.DestroyContext();
			}
		}
#endif

		views[0]->SetViewRect(0, 0, wh - 1, hh - 1, camera);
		views[1]->SetViewRect(wh, 0, vpwidth - 1, hh - 1, camera);
		views[2]->SetViewRect(0, hh, wh - 1, vpheight - 1, camera);
		views[3]->SetViewRect(wh, hh, vpwidth - 1, vpheight - 1, camera);

/*      wh = vpwidth/4;
		hh = vpheight/3;

		views[0]->SetViewRect(0, 0, wh - 1, hh - 2, wleft,wtop, camera);
		views[1]->SetViewRect(0, hh + 1, wh - 1, 2*hh - 2, wleft,wtop, camera);
		views[2]->SetViewRect(0, 2*hh + 1, wh - 1, vpheight - 1, wleft,wtop, camera);
		views[3]->SetViewRect(wh + 2, 0, vpwidth - 1, vpheight - 1, wleft,wtop, camera);*/
	}
	else
	{
#ifdef __OPENGL__
		if(mode != multiview)
		{
			for(i=0; i<4; i++)
			{
				// free old contexts
				if(views[i] != view)
					views[i]->opengl.DestroyContext();
			}
		}
		if(display == DISPLAY_SOLID)
		{
#ifdef __AMIGA__
			view->opengl.CreateContext(1,1, vpwidth-3, vpheight-3, directrender);
#else // __AMIGA__
			view->opengl.CreateContext(1,1, vpwidth-3, vpheight-3);
#endif // __AMIGA__
		}
		else
		{
			if(view->opengl.IsInitialized())
				view->opengl.DestroyContext();
		}
#endif
		view->SetViewRect(0, 0, vpwidth - 1, vpheight - 1, camera);
	}

	multiview = mode;
}

#ifdef __OPENGL__
/*************
 * DESCRIPTION:   Get the view range of the current view
 * INPUT:         near_dist near view distance
 *                far_dist  far view distance
 * OUTPUT:        see INPUT near and far
 *************/
void DISPLAY::GetViewRange(float *near_dist, float *far_dist)
{
	float dx,dy,dz;

	dx = bboxmax.x - bboxmin.x;
	dy = bboxmax.y - bboxmin.y;
	dz = bboxmax.z - bboxmin.z;
	*far_dist = (float)sqrt(dx*dx + dy*dy + dz*dz);
	*near_dist = *far_dist * 1e-4f;
	if(*near_dist < .1f)
		*near_dist = .1f;
#if 0
	VECTOR v;
	MATRIX m,m1,m2;

	*near_dist = INFINITY;
	*far_dist = -INFINITY;

	m1.SetTMatrix(&view->pos);
	m2.SetOMatrix(&view->axis_x, &view->axis_y, &view->axis_z);
	m.MultMat(&m1,&m2);

	v = bboxmin;
	m.MultVectMat(&v);
	v.z = fabs(v.z);
	if(v.z > *far_dist)
		*far_dist = v.z;
	if(v.z < *near_dist)
		*near_dist = v.z;

	SetVector(&v, bboxmin.x, bboxmin.y, bboxmax.z);
	m.MultVectMat(&v);
	v.z = fabs(v.z);
	if(v.z > *far_dist)
		*far_dist = v.z;
	if(v.z < *near_dist)
		*near_dist = v.z;

	SetVector(&v, bboxmin.x, bboxmax.y, bboxmin.z);
	m.MultVectMat(&v);
	v.z = fabs(v.z);
	if(v.z > *far_dist)
		*far_dist = v.z;
	if(v.z < *near_dist)
		*near_dist = v.z;

	SetVector(&v, bboxmin.x, bboxmax.y, bboxmax.z);
	m.MultVectMat(&v);
	v.z = fabs(v.z);
	if(v.z > *far_dist)
		*far_dist = v.z;
	if(v.z < *near_dist)
		*near_dist = v.z;

	SetVector(&v, bboxmax.x, bboxmin.y, bboxmin.z);
	m.MultVectMat(&v);
	v.z = fabs(v.z);
	if(v.z > *far_dist)
		*far_dist = v.z;
	if(v.z < *near_dist)
		*near_dist = v.z;

	SetVector(&v, bboxmax.x, bboxmin.y, bboxmax.z);
	m.MultVectMat(&v);
	v.z = fabs(v.z);
	if(v.z > *far_dist)
		*far_dist = v.z;
	if(v.z < *near_dist)
		*near_dist = v.z;

	SetVector(&v, bboxmax.x, bboxmax.y, bboxmin.z);
	m.MultVectMat(&v);
	v.z = fabs(v.z);
	if(v.z > *far_dist)
		*far_dist = v.z;
	if(v.z < *near_dist)
		*near_dist = v.z;

	v = bboxmax;
	m.MultVectMat(&v);
	v.z = fabs(v.z);
	if(v.z > *far_dist)
		*far_dist = v.z;
	if(v.z < *near_dist)
		*near_dist = v.z;

	if(*near_dist < .5f)
		*near_dist = .5f;
	if(*near_dist > *far_dist)
		*near_dist = *far_dist + 1.f;
#endif
}

/*************
 * DESCRIPTION:   Setup the viewing matrix and all neccessary settings for OpenGL
 * INPUT:         -
 * OUTPUT:        -
 *************/
void DISPLAY::SetupOpenGL()
{
	COLOR clearcolor;
	VECTOR up, look, orient_x, orient_y, orient_z, pos;
	float near_dist,far_dist;
	MATRIX m,m1;

	view->opengl.MakeCurrent();
	GetColorValue(COLOR_BKGROUND, &clearcolor);
	switch(view->viewmode)
	{
		case VIEW_CAMERA:
			GetViewRange(&near_dist, &far_dist);

			camera->GetObjectMatrix(&m);
			m.GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);
			m.SetOMatrix(&orient_x, &orient_y, &orient_z);
			if(camera->track)
			{
				camera->track->GetObjectMatrix(&m1);
				m1.GenerateAxis(&orient_x, &orient_y, &orient_z, &look);
			}
			else
			{
				SetVector(&look, 0.f, 0.f, 1.f);
				m.MultVectMat(&look);
				VecAdd(&look, &pos, &look);
			}
			SetVector(&up, 0.f, 1.f, 0.f);
			m.MultVectMat(&up);
			OpenGLSetupCamera(this, &clearcolor, &pos, &look, &up, near_dist, far_dist);
			break;
		case VIEW_PERSP:
			GetViewRange(&near_dist, &far_dist);

			pos.x = sin(view->valign.y)*cos(view->valign.x);
			pos.y = cos(view->valign.y);
			pos.z = sin(view->valign.y)*sin(view->valign.x);

			orient_y.x = sin(view->valign.y - PI_2)*cos(view->valign.x);
			orient_y.y = cos(view->valign.y - PI_2);
			orient_y.z = sin(view->valign.y - PI_2)*sin(view->valign.x);
			orient_z.x = -pos.x;
			orient_z.y = -pos.y;
			orient_z.z = -pos.z;
			VecNormalize(&orient_z);
			VecCross(&orient_y,&orient_z,&orient_x);
			InvOrient(&orient_x, &orient_y, &orient_z, &orient_x, &orient_y, &orient_z);
			m.SetOMatrix(&orient_x, &orient_y, &orient_z);

			SetVector(&look, 0.f, 0.f, view->dist);
			m.MultVectMat(&look);
			VecAdd(&look, &view->vpos, &look);

			SetVector(&up, 0.f, 1.f, 0.f);
			m.MultVectMat(&up);
			OpenGLSetupPerspective(this, &clearcolor, &pos, &look, &up, near_dist, far_dist);
			break;
		case VIEW_RIGHT:
			OpenGLSetupRight(this, &clearcolor);
			break;
		case VIEW_TOP:
			OpenGLSetupTop(this, &clearcolor);
			break;
		case VIEW_FRONT:
			OpenGLSetupFront(this, &clearcolor);
			break;
	}
	SetupOpenGLLights(this);
}
#endif // __OPENGL__

/*************
 * DESCRIPTION:   Draw a single view
 * INPUT:         view     view to draw
 *                oldview  old view
 *                mode     redraw mode
 *                changed  display changed since editing began
 * OUTPUT:        -
 *************/
void DISPLAY::RedrawView(VIEW *view, VIEW *oldview, int mode, BOOL changed)
{
	RECTANGLE *orect;

	ASSERT(view);
	ASSERT(oldview);

	wleft = view->left;
	wtop = view->top;
	wwidth = view->width;
	wheight = view->height;

	if(view->viewmode == VIEW_CAMERA)
		view->SetCamera(camera);

#ifndef __AMIGA__
	// investigate why Mike needs this !!!
	view->SetViewing(camera);
#endif // __AMIGA__

	if(mode == REDRAW_ALL)
	{
#ifdef __OPENGL__
		if(display == DISPLAY_SOLID)
		{
			view->SetViewing(camera);
			SetupOpenGL();

			PPC_STUB(glDisable)(GL_LIGHTING);
			//DrawCross();
			DrawGrid();
			if(view->viewmode == VIEW_CAMERA)
				DrawRenderLines(FALSE);
			PPC_STUB(glEnable)(GL_LIGHTING);
			// draw objects
			DrawObjects(this, TRUE);
			DrawObjects(this, FALSE);
			// draw white rectangle around selected screen
			gfx.SetOutput(GFX_SCREEN);
			if(view == oldview)
				gfx.SetPen(COLOR_SELECTED);
			else
				gfx.SetPen(COLOR_UNSELECTED);
			gfx.SetClipping(wleft, wtop, wleft+wwidth-1, wtop+wheight-1);
			gfx.Rectangle(wleft, wtop, wleft+wwidth-1, wtop+wheight-1);

			view->opengl.SwapBuffers();
		}
		else
		{
#endif // __OPENGL__
			gfx.SetOutput(GFX_MEMORY2);
			gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
			gfx.SetOutput(GFX_MEMORY1);
			gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);

			gfx.SetPen(COLOR_BKGROUND);
			gfx.FillRect(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
			view->SetViewing(camera);

			DrawCross();
			DrawGrid();
			if(view->viewmode == VIEW_CAMERA)
				DrawRenderLines(FALSE);
			// draw unselected objects
			DrawObjects(this, FALSE);
			// copy screen with grid and unselected objects to back buffer
			gfx.SetClipping(wleft, wtop, wleft+wwidth-1, wtop+wheight-1);
			gfx.BitBlt(GFX_MEMORY1, wleft+1, wtop+1, wwidth-2, wheight-2, GFX_MEMORY2, wleft+1, wtop+1);
			gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
			// add selected objects
			DrawObs(this, REDRAW_SELECTED);
			// draw white rectangle around selected screen
			if(view == oldview)
				gfx.SetPen(COLOR_SELECTED);
			else
				gfx.SetPen(COLOR_UNSELECTED);
			gfx.SetClipping(wleft, wtop, wleft+wwidth-1, wtop+wheight-1);
			gfx.Rectangle(wleft, wtop, wleft+wwidth-1, wtop+wheight-1);
			gfx.BitBlt(GFX_MEMORY1, wleft, wtop, wwidth, wheight, GFX_SCREEN, wleft, wtop);
#ifdef __OPENGL__
		}
#endif // __OPENGL__
	}
	else
	{
		if(editmode & EDIT_ACTIVE)
		{
			if((view->viewmode == VIEW_CAMERA) &&
				 camera->selected)
			{
				camera->SetObject(this,
					&pos,
					&orient_x,
					&orient_y,
					&orient_z,
					&size);
			}

			if((editmode & EDIT_OBJECT) && !((view->viewmode == VIEW_CAMERA) && camera->selected))
			{
#ifdef __OPENGL__
				if(display == DISPLAY_SOLID)
				{
					SetupOpenGL();
					PPC_STUB(glDisable)(GL_LIGHTING);
					//DrawCross();
					DrawGrid();
					if(view->viewmode == VIEW_CAMERA)
						DrawRenderLines(FALSE);
					PPC_STUB(glEnable)(GL_LIGHTING);
					DrawObjects(this, TRUE);
					DrawObjects(this, FALSE);
					view->opengl.SwapBuffers();
				}
				else
				{
#endif // __OPENGL__
					// editing an object
					gfx.SetOutput(GFX_MEMORY2);
					gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
					gfx.SetOutput(GFX_SCREEN);
					gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
					gfx.SetOutput(GFX_MEMORY1);
					gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
					orect = &view->oldaccrect;
					if(orect->set)
					{
						// copy previous region from background to back buffer
						gfx.BitBlt(GFX_MEMORY2, orect->left, orect->top, orect->right - orect->left + 1, orect->bottom - orect->top + 1, GFX_MEMORY1, orect->left, orect->top);
					}
					else
					{
						// first time: copy whole background to back buffer
						gfx.BitBlt(GFX_MEMORY2, wleft+1, wtop+1, wwidth-2, wheight-2, GFX_MEMORY1, wleft+1, wtop+1);
						orect->set = TRUE;
						orect->left = wleft+1;
						orect->top = wtop+1;
						orect->right = wleft+wwidth-2;
						orect->bottom = wtop+wheight-2;
					}
					// mark accumulation rectangle uninitialized
					accrect.set = FALSE;
					// draw objects to backbuffer
					view->SetViewing(camera);
					DrawObjects(this, TRUE);

					// extend old rectangle
					UnionRectangle(orect, &accrect, wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
					// blit changes to screen
					gfx.BitBlt(GFX_MEMORY1, orect->left, orect->top, orect->right - orect->left + 1, orect->bottom - orect->top + 1, GFX_SCREEN, orect->left, orect->top);
					*orect = accrect;
#ifdef __OPENGL__
				}
#endif // __OPENGL__
			}
			else
			{
#ifdef __OPENGL__
				if(display == DISPLAY_SOLID)
				{
					SetupOpenGL();
					PPC_STUB(glDisable)(GL_LIGHTING);
					//DrawCross();
					DrawGrid();
					if(view->viewmode == VIEW_CAMERA)
						DrawRenderLines(FALSE);
					PPC_STUB(glEnable)(GL_LIGHTING);
					DrawObjects(this, TRUE);
					DrawObjects(this, FALSE);
					view->opengl.SwapBuffers();
				}
				else
				{
#endif // __OPENGL__
					// editing the world
					gfx.SetOutput(GFX_MEMORY2);
					gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
					gfx.SetPen(COLOR_BKGROUND);
					// clear the buffer
					gfx.FillRect(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
					view->SetViewing(camera);
					DrawCross();
					DrawGrid();
					if(view->viewmode == VIEW_CAMERA)
						DrawRenderLines(FALSE);
					DrawObjects(this, TRUE);
					DrawObjects(this, FALSE);
					gfx.BitBlt(GFX_MEMORY2, wleft+1, wtop+1, wwidth-2, wheight-2, GFX_SCREEN, wleft+1, wtop+1);
#ifdef __OPENGL__
				}
#endif // __OPENGL__
			}
		}
		else
		{
#ifdef __OPENGL__
			if(display == DISPLAY_SOLID)
			{
				SetupOpenGL();
				PPC_STUB(glDisable)(GL_LIGHTING);
				//DrawCross();
				DrawGrid();
				if(view->viewmode == VIEW_CAMERA)
					DrawRenderLines(FALSE);
				PPC_STUB(glEnable)(GL_LIGHTING);
				DrawObjects(this, TRUE);
				DrawObjects(this, FALSE);
				view->opengl.SwapBuffers();
			}
			else
			{
#endif // __OPENGL__
				switch (mode)
				{
					case REDRAW_REMOVE_SELECTED:
						gfx.SetOutput(GFX_SCREEN);
						gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
						gfx.BitBlt(GFX_MEMORY2, wleft+1, wtop+1, wwidth-2, wheight-2, GFX_SCREEN, wleft+1, wtop+1);
						break;
					case REDRAW_SELECTED:
						if(editmode & EDIT_MESH)
						{
							gfx.SetOutput(GFX_MEMORY2);
							gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
							DrawObs(this, REDRAW_SELECTED);
							gfx.BitBlt(GFX_MEMORY2, wleft+1, wtop+1, wwidth-2, wheight-2, GFX_SCREEN, wleft+1, wtop+1);
						}
						else
						{
							gfx.SetOutput(GFX_MEMORY1);
							gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
							gfx.BitBlt(GFX_MEMORY2, wleft+1, wtop+1, wwidth-2, wheight-2, GFX_MEMORY1, wleft+1, wtop+1);
							DrawObs(this, REDRAW_SELECTED);
							gfx.BitBlt(GFX_MEMORY1, wleft+1, wtop+1, wwidth-2, wheight-2, GFX_SCREEN, wleft+1, wtop+1);
						}
						break;
					case REDRAW_DIRTY:
						gfx.SetOutput(GFX_MEMORY2);
						gfx.SetClipping(wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
						orect = &accrect;
						orect->set = FALSE;
						DrawObs(this, REDRAW_DIRTY); // remove selected objects
						gfx.BitBlt(GFX_MEMORY2, wleft+1, wtop+1, wwidth-2, wheight-2, GFX_MEMORY1, wleft+1, wtop+1);
						gfx.SetOutput(GFX_MEMORY1);
						DrawObs(this, REDRAW_SELECTED); // add selected objects
						if(orect->set)
						{
							UnionRectangle(orect, orect, wleft+1, wtop+1, wleft+wwidth-2, wtop+wheight-2);
							gfx.BitBlt(GFX_MEMORY1, orect->left, orect->top, orect->right - orect->left + 1, orect->bottom - orect->top + 1, GFX_SCREEN, orect->left, orect->top);
						}
						break;
					default:
						ASSERT(FALSE);
						break;
				}
#ifdef __OPENGL__
			}
#endif // __OPENGL__
		}
	}
}

/*************
 * DESCRIPTION:   redraws the display
 * INPUT:         mode     draw mode
 *                changed  display changed since editing began
 * OUTPUT:        -
 *************/
void DISPLAY::Redraw(int mode, BOOL changed)
{
	VIEW *oldview;
	int i;

	if(display == DISPLAY_SOLID)
	{
		CalcSceneBBox(this, &bboxmin, &bboxmax);
		if(grid)
		{
			if(bboxmin.x > -SUBDIV*gridsize)
				bboxmin.x = -SUBDIV*gridsize;
			if(bboxmax.x < SUBDIV*gridsize)
				bboxmax.x = SUBDIV*gridsize;
			if(bboxmin.z > -SUBDIV*gridsize)
				bboxmin.z = -SUBDIV*gridsize;
			if(bboxmax.z < SUBDIV*gridsize)
				bboxmax.z = SUBDIV*gridsize;
		}
	}

	if(mode == REDRAW_CURRENT_VIEW)
	{
		RedrawView(view, view, REDRAW_ALL, changed);
	}
	else
	{
		if(multiview)
		{
			oldview = view;
			for (i = 0; i < 4; i++)
			{
				view = views[i];
				// if editing the world skip over the unselected views
				if(((editmode & (EDIT_ACTIVE | EDIT_OBJECT)) != EDIT_ACTIVE) || (view == oldview))
				{
					RedrawView(view, oldview, mode, changed);
				}
			}
			view = oldview;
		}
		else
		{
			RedrawView(view, view, mode, changed);
		}
	}

	if(mode == REDRAW_DIRTY)
		RemoveDirtyFlag();
}

/*************
 * DESCRIPTION:   sets the display mode (BBOX, WIRE, SOLID)
 * INPUT:         mode     display mode
 * OUTPUT:        -
 *************/
void DISPLAY::SetDisplayMode(ULONG mode)
{
	if(display == mode)
		return;

	display = mode;

	if(mode == DISPLAY_SOLID)
		SetMultiView(multiview);
}
