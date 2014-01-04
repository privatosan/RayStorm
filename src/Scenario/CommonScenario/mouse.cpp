/***************
 * PROGRAM:       Modeler
 * NAME:          mouse.cpp
 * DESCRIPTION:   mouse handler
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    28.06.96 ah    initial release
 *    04.10.96 ah    imporved object scaling
 *    31.01.97 ah    added HandleMouseButtons()
 ***************/

#include <math.h>

#ifdef __AMIGA__
#ifndef DRAW_CLASS_H
#include "draw_class.h"
#endif

#ifndef MAINWINDOW_CLASS_H
#include "mainwindow_class.h"
#endif

#ifndef MUIUTILITY_H
#include "MUIutility.h"
#endif
#else
#include "camview.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef GFX_H
#include "gfx.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef MOUSE_H
#include "mouse.h"
#endif

#ifndef MAGIC_H
#include "magic.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef MESH_H
#include "mesh.h"
#endif

#ifndef UNDO_H
#include "undo.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifdef __AMIGA__

#pragma chip

UWORD nopointer[] =
{
	0,0,0,0,0,0,
};

#pragma fast

struct NewMenu world_popupmenu[] =
{
	{ NM_TITLE, "World"              , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "Move world"         , 0 ,0,0,(APTR)POP_WORLD_MOVE},
	{ NM_ITEM , "Rotate world"       , 0 ,0,0,(APTR)POP_WORLD_ROTATE},
	{ NM_ITEM , "Zoom world"         , 0 ,0,0,(APTR)POP_WORLD_SCALE},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "Primitives"         , 0 ,0,0,0},
	{ NM_SUB  , "Sphere"             , 0 ,0,0,(APTR)POP_SPHERE},
	{ NM_SUB  , "Box"                , 0 ,0,0,(APTR)POP_BOX},
	{ NM_SUB  , "Cylinder"           , 0 ,0,0,(APTR)POP_CYLINDER},
	{ NM_SUB  , "Cone"               , 0 ,0,0,(APTR)POP_CONE},
	{ NM_SUB  , "SOR"                , 0 ,0,0,(APTR)POP_SOR},
	{ NM_SUB  , "Plane"              , 0 ,0,0,(APTR)POP_PLANE},
	{ NM_SUB  , "Camera"             , 0 ,0,0,(APTR)POP_CAMERA},
	{ NM_SUB  , "CSG"                , 0 ,0,0,(APTR)POP_CSG},
	{ NM_ITEM , "Lights"             , 0 ,0,0,0},
	{ NM_SUB  , "Directional light " , 0 ,0,0,(APTR)POP_DIRECTIONALLIGHT},
	{ NM_SUB  , "Point light"        , 0 ,0,0,(APTR)POP_POINTLIGHT},
	{ NM_SUB  , "Spot light "        , 0 ,0,0,(APTR)POP_SPOTLIGHT},
	{ NM_ITEM , "Meshes"             , 0 ,0,0,0},
	{ NM_SUB  , "Axis"               , 0 ,0,0,(APTR)POP_AXIS},
	{ NM_SUB  , "Sphere"             , 0 ,0,0,(APTR)POP_MESHSPHERE},
	{ NM_SUB  , "Box"                , 0 ,0,0,(APTR)POP_MESHCUBE},
	{ NM_SUB  , "Cylinder"           , 0 ,0,0,(APTR)POP_MESHTUBE},
	{ NM_SUB  , "Cone"               , 0 ,0,0,(APTR)POP_MESHCONE},
	{ NM_SUB  , "Plane"              , 0 ,0,0,(APTR)POP_MESHPLANE},
	{ NM_SUB  , "Torus"              , 0 ,0,0,(APTR)POP_MESHTORUS},
	{ NM_ITEM , NM_BARLABEL          , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "Settings..."        , 0 ,0,0,(APTR)POP_WORLD_SETTINGS},
	{ NM_ITEM , "Render"             , 0 ,0,0,(APTR)POP_WORLD_RENDER},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};

#define SETCOORDDISPLAY(v) if(display->coord_disp || display->always_coord) SetCoordDisplay(v, mouse_data->win);

#else

#define SETCOORDDISPLAY(v) if(display->coord_disp || display->always_coord) mouse_data->pView->SetCoordDisplay(v);

#endif

/*************
 * DESCRIPTION:   convert screen mouse coordinates to 3D world coordinates
 * INPUT:         display  current display
 *                x,y      mouse position
 *                v        3D coordinates
 * OUTPUT:        TRUE if inside of a view else FALSE
 *************/
BOOL MouseTo3D(DISPLAY *display, int x, int y, VECTOR *v)
{
	VIEW *view = display->view;
	BOOL inside = FALSE;

	switch(view->viewmode)
	{
		case VIEW_FRONT:
			v->x = view->viewpoint.x + (x - view->left - (view->width>>1)) / view->zoom;
			v->y = view->viewpoint.y - (y - view->top - (view->height>>1)) / view->zoom;
			v->z = view->viewpoint.z;
			inside = TRUE;
			break;
		case VIEW_RIGHT:
			v->z = view->viewpoint.z + (x - view->left - (view->width>>1)) / view->zoom;
			v->y = view->viewpoint.y - (y - view->top - (view->height>>1)) / view->zoom;
			v->x = view->viewpoint.x;
			inside = TRUE;
			break;
		case VIEW_TOP:
			v->x = view->viewpoint.x + (x - view->left - (view->width>>1)) / view->zoom;
			v->z = view->viewpoint.z - (y - view->top - (view->height>>1)) / view->zoom;
			v->y = view->viewpoint.y;
			inside = TRUE;
			break;
	}

	return inside;
}

/*************
 * DESCRIPTION:   center view(s) to mouse position
 * INPUT:         display  current display
 *                x,y      mouse position
 * OUTPUT:        TRUE if view changed (redraw neccessary) else FALSE
 *************/
BOOL Center(DISPLAY *display, int x, int y)
{
	return MouseTo3D(display, x, y, &display->view->viewpoint);
}

/*************
 * DESCRIPTION:   find object
 * INPUT:         view     current view
 *                obj      object to find
 * OUTPUT:        TRUE if view changed (redraw neccessary) else FALSE
 *************/
BOOL Find(VIEW *view, OBJECT *obj)
{
	if(view->viewmode != VIEW_PERSP && view->viewmode != VIEW_CAMERA)
		view->viewpoint = obj->pos;
	else
	{
		view->vpos = obj->pos;
		if(view->viewmode != VIEW_CAMERA)
			view->SetVirtualCamera();
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   zooms selected objects
 * INPUT:         view     current view
 *                all      TRUE: focus all objects; FALSE: focus selected objects
 * OUTPUT:        TRUE if view changed (redraw neccessary) else FALSE
 *************/
BOOL Focus(VIEW *view, BOOL all)
{
	VECTOR bboxmin,bboxmax,look;
	float f;

	if(!GetBBox(&bboxmin, &bboxmax, all))
		return FALSE;

	// calculate center of the bounding box
	look.x = .5f*(bboxmax.x + bboxmin.x);
	look.y = .5f*(bboxmax.y + bboxmin.y);
	look.z = .5f*(bboxmax.z + bboxmin.z);

	// determine maximum size
	f = MAX(fabs(bboxmax.x - bboxmin.x), fabs(bboxmax.y - bboxmin.y));
	f = MAX(f, fabs(bboxmax.z - bboxmin.z));

	if(view->viewmode != VIEW_PERSP && view->viewmode != VIEW_CAMERA)
	{
		view->viewpoint = look;

		view->zoom = .9f*(MIN(view->width, view->height)/f);
	}
	else
	{
		view->dist = f;
		view->vpos = look;
		if(view->viewmode != VIEW_CAMERA)
			view->SetVirtualCamera();
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   zooms into the scene by factor two
 * INPUT:         view     current view
 * OUTPUT:        none
 *************/
void ZoomIn(VIEW *view)
{
	if(view->viewmode == VIEW_PERSP)
	{
		view->dist /= ZOOM_INOUT;

		if(view->dist < MIN_DIST)
			view->dist = MIN_DIST;
		view->SetVirtualCamera();
	}
	else
	{
		view->zoom *= ZOOM_INOUT;

		if(view->zoom < MIN_ZOOM)
			view->zoom = MIN_ZOOM;
	}
}

/*************
 * DESCRIPTION:   zooms out of the scene by factor two
 * INPUT:         view     current view
 * OUTPUT:        none
 *************/
void ZoomOut(VIEW *view)
{
	if(view->viewmode == VIEW_PERSP)
	{
		view->dist *= ZOOM_INOUT;

		if(view->dist < MIN_DIST)
			view->dist = MIN_DIST;
		view->SetVirtualCamera();
	}
	else
	{
		view->zoom /= ZOOM_INOUT;

		if(view->zoom < MIN_ZOOM)
			view->zoom = MIN_ZOOM;
	}
}

/*************
 * DESCRIPTION:   sets object position. Uses active axes to filter and snaps to grid
 * INPUT:         display        current display
 *                mouse_data     mouse data
 *                x,y,z          position delta
 * OUTPUT:        none
 *************/
static void SetObjectPos(DISPLAY *display, MOUSEDATA *mouse_data, float x, float y, float z)
{
	float t;

	if (display->gridsnap)
	{
		t = 1.f/display->gridsize;
		x = int(x*t)*display->gridsize + display->offset.x;
		y = int(y*t)*display->gridsize + display->offset.y;
		z = int(z*t)*display->gridsize + display->offset.z;
	}
	if(display->active & ACTIVE_X)
		display->pos.x = x;
	if(display->active & ACTIVE_Y)
		display->pos.y = y;
	if(display->active & ACTIVE_Z)
		display->pos.z = z;

	SETCOORDDISPLAY(&display->pos);
}

/*************
 * DESCRIPTION:   sets object alignment. Uses activ axes to filter
 * INPUT:         display        current display
 *                mouse_data     mouse data
 *                x,y,z          position delta
 * OUTPUT:        none
 *************/
static void SetObjectAlign(DISPLAY *display, MOUSEDATA *mouse_data, float x, float y, float z)
{
	VECTOR v;

	if(display->active & ACTIVE_X)
		v.x = x;
	else
		v.x = 0.;
	if(display->active & ACTIVE_Y)
		v.y = y;
	else
		v.y = 0.;
	if(display->active & ACTIVE_Z)
		v.z = z;
	else
		v.z = 0.;

	CalcOrient(&v, &display->orient_x, &display->orient_y, &display->orient_z);

	v.y = -v.y;

	display->align = v;

	SETCOORDDISPLAY(&v);
}

/*************
 * DESCRIPTION:   sets object scale. Uses activ axes to filter
 * INPUT:         display     current display
 *                mouse_data  mouse data
 *                x,y,z       scale delta
 * OUTPUT:        none
 *************/
static void SetObjectScale(DISPLAY *display, MOUSEDATA *mouse_data, float x, float y, float z)
{
	if(display->active & ACTIVE_X)
		display->size.x = x < EPSILON ? EPSILON : x;
	if(display->active & ACTIVE_Y)
		display->size.y = y < EPSILON ? EPSILON : y;
	if(display->active & ACTIVE_Z)
		display->size.z = z < EPSILON ? EPSILON : z;

	SETCOORDDISPLAY(&display->size);
}

/*************
 * DESCRIPTION:   handle mouse move event
 * INPUT:         display     current display
 *                x,y         absolute mouse position
 *                mosue_data  pointer to mousedata
 * OUTPUT:        none
 *************/
void DoMouseMove(DISPLAY *display, int x, int y, MOUSEDATA *mouse_data)
{
	VIEW *view = display->view;
	float cosx,sinx;
	VECTOR u,v;
	float h;
	int mdx, mdy;

	mdx = (int)((x - mouse_data->oldmousex)*SCALE_MOUSE_DELTA);
	mdy = (int)((y - mouse_data->oldmousey)*SCALE_MOUSE_DELTA);

	h = (float)(mdx - mdy);
	if (h > ZOOM_VIEWER_MAXVELOC)
		h = ZOOM_VIEWER_MAXVELOC;
	if (h < -ZOOM_VIEWER_MAXVELOC)
		h = -ZOOM_VIEWER_MAXVELOC;

	mouse_data->dx += mdx;

	if (display->editmode & EDIT_RMB)
		mouse_data->dz -= mdy;
	else
		mouse_data->dy -= mdy;

	mouse_data->oldmousex = x;
	mouse_data->oldmousey = y;

	switch (display->editmode & EDIT_MASK)
	{
		case EDIT_ROTATE:
			switch (view->viewmode)
			{
				case VIEW_CAMERA:
				case VIEW_PERSP:
					if (display->editmode & EDIT_OBJECT)
					{
						SetObjectAlign(display, mouse_data, mouse_data->dy*ALIGN_OBJECT_VELOC,
																	  -mouse_data->dx*ALIGN_OBJECT_VELOC,
																	  -mouse_data->dz*ALIGN_OBJECT_VELOC);
					}
					else
					{
						SetVector(&view->valign,
							mouse_data->oldvalign.x + mouse_data->dx * PI_180*ALIGN_VIEWER_VELOC,
							mouse_data->oldvalign.y + mouse_data->dy * PI_180*ALIGN_VIEWER_VELOC,
							mouse_data->oldvalign.z + mouse_data->dz * PI_180*ALIGN_VIEWER_VELOC);
						SetVector(&v,
							view->valign.x * INV_PI_180,
							view->valign.y * INV_PI_180,
							view->valign.z * INV_PI_180);
						SETCOORDDISPLAY(&v);
						if(view->viewmode == VIEW_CAMERA)
							view->SetCamera(display->camera);
						else
							view->SetVirtualCamera();
					}
					break;
				case VIEW_FRONT:
					if(display->editmode & EDIT_OBJECT)
					{
						SetObjectAlign(display, mouse_data, (float)-mouse_data->dz, (float)-mouse_data->dx, (float)mouse_data->dy);
					}
					break;
				case VIEW_RIGHT:
					if(display->editmode & EDIT_OBJECT)
					{
						SetObjectAlign(display, mouse_data, (float)mouse_data->dy, (float)-mouse_data->dx, (float)mouse_data->dz);
					}
					break;
				case VIEW_TOP:
					if(display->editmode & EDIT_OBJECT)
					{
						SetObjectAlign(display, mouse_data, (float)-mouse_data->dx, (float)mouse_data->dz, (float)mouse_data->dy);
					}
					break;
			}
			break;
		case EDIT_MOVE:
			switch(view->viewmode)
			{
				case VIEW_PERSP:
					if(display->editmode & EDIT_OBJECT)
					{
						cosx = cos(view->valign.x)*view->dist*MOVE_OBJECT_VELOC;
						sinx = sin(view->valign.x)*view->dist*MOVE_OBJECT_VELOC;
						SetObjectPos(display, mouse_data,
										- sinx*mouse_data->dx - cosx*mouse_data->dz,
										mouse_data->dy*view->dist*MOVE_VIEWER_VELOC,
										cosx*mouse_data->dx - sinx*mouse_data->dz);
					}
					else
					{
						cosx = cos(view->valign.x)*view->dist*MOVE_VIEWER_VELOC;
						sinx = sin(view->valign.x)*view->dist*MOVE_VIEWER_VELOC;
						SetVector(&view->vpos,
									mouse_data->oldvpos.x + sinx*mouse_data->dx + cosx*mouse_data->dz,
									mouse_data->oldvpos.y - mouse_data->dy*view->dist*MOVE_VIEWER_VELOC,
									mouse_data->oldvpos.z - cosx*mouse_data->dx + sinx*mouse_data->dz);
						SETCOORDDISPLAY(&view->vpos);
						view->SetVirtualCamera();
					}
					break;
				case VIEW_CAMERA:
					if(display->editmode & EDIT_OBJECT)
					{
						SetVector(&v, (float)mouse_data->dx, (float)mouse_data->dy, (float)mouse_data->dz);
						SetVector(&u,
							dotp(&display->camera->orient_x, &v) * view->dist * MOVE_OBJECT_VELOC,
							dotp(&display->camera->orient_y, &v) * view->dist * MOVE_OBJECT_VELOC,
							dotp(&display->camera->orient_z, &v) * view->dist * MOVE_OBJECT_VELOC);
						SetObjectPos(display, mouse_data, u.x, u.y, u.z);
					}
					break;
				case VIEW_FRONT:
					if(display->editmode & EDIT_OBJECT)
					{
						SetObjectPos(display,mouse_data, mouse_data->dx/view->zoom, mouse_data->dy/view->zoom, mouse_data->dz/view->zoom);
					}
					else
					{
						SetVector(&view->viewpoint,
							mouse_data->oldvp.x - mouse_data->dx/view->zoom,
							mouse_data->oldvp.y - mouse_data->dy/view->zoom,
							mouse_data->oldvp.z - mouse_data->dz/view->zoom);
						SETCOORDDISPLAY(&view->viewpoint);
					}
					break;
				case VIEW_RIGHT:
					if(display->editmode & EDIT_OBJECT)
					{
						SetObjectPos(display, mouse_data, mouse_data->dz/view->zoom, mouse_data->dy/view->zoom, mouse_data->dx/view->zoom);
					}
					else
					{
						SetVector(&view->viewpoint,
							mouse_data->oldvp.x - mouse_data->dz/view->zoom,
							mouse_data->oldvp.y - mouse_data->dy/view->zoom,
							mouse_data->oldvp.z - mouse_data->dx/view->zoom);
						SETCOORDDISPLAY(&view->viewpoint);
					}
					break;
				case VIEW_TOP:
					if(display->editmode & EDIT_OBJECT)
					{
						SetObjectPos(display,mouse_data, mouse_data->dx/view->zoom, mouse_data->dz/view->zoom, mouse_data->dy/view->zoom);
					}
					else
					{
						SetVector(&view->viewpoint,
							mouse_data->oldvp.x - mouse_data->dx/view->zoom,
							mouse_data->oldvp.y - mouse_data->dz/view->zoom,
							mouse_data->oldvp.z - mouse_data->dy/view->zoom);
						SETCOORDDISPLAY(&view->viewpoint);
					}
					break;
			}
			break;
		case EDIT_SCALE:
			if(display->editmode & EDIT_OBJECT)
			{
				if(mouse_data->dx < 0.f)
				{
					h = 1/(-mouse_data->dx*SCALE_OBJECT_VELOC + 1.f);
					SetObjectScale(display,mouse_data, h, h, h);
				}
				else
				{
					h *= SCALE_OBJECT_VELOC2;
					SetObjectScale(display,mouse_data,
						display->size.x + h,
						display->size.y + h,
						display->size.z + h);
				}
			}
			else
			{
				if(view->viewmode == VIEW_PERSP || view->viewmode == VIEW_CAMERA)
				{
					if(view->viewmode != VIEW_CAMERA)      // only virtual camera
					{
						view->dist+= -h*(view->dist/ZOOM_VIEWER_VELOC);
						if(view->dist < MIN_DIST)
							view->dist = MIN_DIST;
						view->SetVirtualCamera();
						SetVector(&v, view->dist, 0.f, 0.f);
						SETCOORDDISPLAY(&v);
					}
				}
				else
				{
					view->zoom += h*(view->zoom/ZOOM_VIEWER_VELOC);
					if(view->zoom < MIN_ZOOM)
						view->zoom = MIN_ZOOM;
					SetVector(&v, view->zoom, 0.f, 0.f);
					SETCOORDDISPLAY(&v);
				}
			}
			break;
	}
}

/*************
 * DESCRIPTION:   handle mouse button event
 * INPUT:         mouse_data
 *                mouse_x, mouse_y  mouse position
 *                flags             button flags
 *                breaked
 * OUTPUT:        none
 *************/
void HandleMouseButtons(MOUSEDATA *mouse_data, int mouse_x, int mouse_y, int flags, BOOL *breaked)
{
	DISPLAY *display = mouse_data->display;
	VIEW *view, *oldview;
	OBJECT *selobj;
	int x,y, i;
	BOOL shift;       // TRUE if shift key pressed
	UNDO_TRANSFORM *undo;
	RECTANGLE r;
	BOOL redraw;

	shift = (flags & (BUTTON_RIGHTSHIFT | BUTTON_LEFTSHIFT));

	if(*breaked && ((flags & BUTTON_LEFTDOWN) || (flags & BUTTON_RIGHTDOWN)))
	{
		x = mouse_x;
		y = mouse_y;
	}

	view = display->view;

	// handles left button down
	if(flags & BUTTON_LEFTDOWN)
	{
		oldview = display->view;
		view = display->SetViewByPoint(x, y);
		if(view && (*breaked) && (view == oldview))
		{
			if(display->editmode & EDIT_SELBOX)
			{
				if(display->boxmode == BOXMODE_RENDERWINDOW)
				{
					// the size of the renderwindow can only be set in perspective camera view
					if(view->viewmode != VIEW_CAMERA)
					{
						display->editmode &= ~EDIT_SELBOX;
						// remove checkmark from menu item
						utility.CheckMenuItem(MENU_SETRENDERWINDOW, FALSE);
						return;
					}
					// render window can only be set inside of view
					display->GetRenderRect(&r);
					if(mouse_x < r.left)
						mouse_x = r.left;
					else
					{
						if(mouse_x > r.right)
							mouse_x = r.right;
					}
					if(mouse_y < r.top)
						mouse_y = r.top;
					else
					{
						if(mouse_y > r.bottom)
							mouse_y = r.bottom;
					}
				}
				display->selbox.left = display->selbox.right = mouse_x;
				display->selbox.top = display->selbox.bottom = mouse_y;
#ifdef __AMIGA__
				MUI_RequestIDCMP(mouse_data->obj,IDCMP_MOUSEMOVE);
#endif
				display->DrawSelBox();
				return;
			}
			redraw = FALSE;
			GetActiveObjects(&selobj);
			if(selobj)
			{
				if(display->editmode & EDIT_ADDPOINTS)
				{
					// add a point to a mesh object
					redraw = selobj->AddPoint(display, x,y);
				}
				else
				{
					if(display->editmode & EDIT_ADDEDGES)
					{
						// add a edge to a mesh object
						redraw = selobj->AddEdge(display, x,y);
					}
					else
					{
						if(display->editmode & EDIT_ADDFACES)
						{
							// add a face to a mesh object
							redraw = selobj->AddFace(display, x,y);
						}
						else
						{
							if(display->editmode & (EDIT_EDITPOINTS | EDIT_EDITEDGES | EDIT_EDITFACES))
							{
								// test if points selected
								redraw = TestSelectedPoints(display,x,y, shift);
							}
						}
					}
				}
			}

			// test if object selected
			if(!redraw)
			{
				if(TestSelected(display,x,y,shift))
				{
					sciBrowserBuild();
					redraw = TRUE;
				}
			}

			if(redraw)
			{
#ifdef __AMIGA__
				Redraw((struct Draw_Data*)INST_DATA(mouse_data->cl, mouse_data->obj), REDRAW_DIRTY, FALSE);
#else
				mouse_data->pView->Redraw(REDRAW_DIRTY, FALSE);
#endif
				return;
			}

			// FRONT, TOP and RIGHT can't be rotated
			if((view->viewmode != VIEW_PERSP) && (view->viewmode != VIEW_CAMERA) && !(display->editmode & EDIT_OBJECT) &&
				(display->editmode & EDIT_ROTATE))
			{
				return;
			}

			// if no object selected -> manipulate object or world
			mouse_data->oldmousex = mouse_x;
			mouse_data->oldmousey = mouse_y;
			mouse_data->dx = mouse_data->dy = mouse_data->dz = 0;
			mouse_data->oldmousex = x;
			mouse_data->oldmousey = y;
			mouse_data->oldvp = view->viewpoint;
			mouse_data->oldvalign = view->valign;
			mouse_data->oldvpos = view->vpos;
			mouse_data->olddist = view->dist;
			mouse_data->oldzoom = view->zoom;
			mouse_data->changed = FALSE;
			mouse_data->olddisplay = display->display;
#ifndef __OPENGL__
			display->display = DISPLAY_BBOX;
#endif // __OPENGL__
			display->views[0]->oldaccrect.set = FALSE;
			display->views[1]->oldaccrect.set = FALSE;
			display->views[2]->oldaccrect.set = FALSE;
			display->views[3]->oldaccrect.set = FALSE;
			SetVector(&display->pos, 0.f, 0.f, 0.f);
			SetVector(&display->orient_x, 1.f, 0.f, 0.f);
			SetVector(&display->orient_y, 0.f, 1.f, 0.f);
			SetVector(&display->orient_z, 0.f, 0.f, 1.f);
			SetVector(&display->size, 1.f, 1.f, 1.f);
			SetVector(&display->align, 0.f, 0.f, 0.f);
			if(selobj)
			{
				SetVector(&display->offset,
					int(selobj->pos.x / display->gridsize) * display->gridsize - selobj->pos.x,
					int(selobj->pos.y / display->gridsize) * display->gridsize - selobj->pos.y,
					int(selobj->pos.z / display->gridsize) * display->gridsize - selobj->pos.z);
			}
			else
				SetVector(&display->offset, 0.f, 0.f, 0.f);

			display->editmode |= EDIT_ACTIVE;

#ifdef __AMIGA__
			SetPointer(_window(mouse_data->obj),nopointer,1,16,0,0);
			MUI_RequestIDCMP(mouse_data->obj,IDCMP_MOUSEMOVE);
#endif
		}
		return;
	}

	// handles left button up
	if(flags & BUTTON_LEFTUP)
	{
		if(display->editmode & EDIT_SELBOX)
		{
#ifdef __AMIGA__
			if(!display->always_coord)
				MUI_RejectIDCMP(mouse_data->obj,IDCMP_MOUSEMOVE);
#endif
			if((mouse_x > display->wleft) &&
				(mouse_x < display->wleft + display->wwidth) &&
				(mouse_y > display->wtop) &&
				(mouse_y < display->wtop + display->wheight))
			{
				if(display->selbox.left > display->selbox.right)
				{
					i = display->selbox.left;
					display->selbox.left = display->selbox.right;
					display->selbox.right = i;
				}
				if(display->selbox.top > display->selbox.bottom)
				{
					i = display->selbox.top;
					display->selbox.top = display->selbox.bottom;
					display->selbox.bottom = i;
				}
				switch(display->boxmode)
				{
					case BOXMODE_SELECT:
						display->DrawSelBox();
						// select objects inside of selection box
						selobj = TestSelectedBox(display, &display->selbox, shift);
						if(selobj)
						{
							sciBrowserBuild();
#ifdef __AMIGA__
							Redraw((struct Draw_Data*)INST_DATA(mouse_data->cl,mouse_data->obj), REDRAW_DIRTY, FALSE);
#else
							mouse_data->pView->Redraw(REDRAW_DIRTY, FALSE);
#endif
						}
						break;
					case BOXMODE_RENDERWINDOW:
						// get visible render area
						display->GetRenderRect(&r);

						global.SetRenderWindow(
							(float)(display->selbox.left - r.left) / (float)(r.right-r.left),
							(float)(display->selbox.top - r.top) / (float)(r.bottom-r.top),
							(float)(display->selbox.right - r.left) / (float)(r.right-r.left),
							(float)(display->selbox.bottom - r.top) / (float)(r.bottom-r.top));
						global.SetEnableArea(TRUE);

						// remove checkmark from menu item
						utility.CheckMenuItem(MENU_SETRENDERWINDOW, FALSE);
						break;
				}
				display->editmode &= ~EDIT_SELBOX;
			}
			return;
		}
		if(display->editmode & EDIT_ACTIVE)
		{
			SETCOORDDISPLAY(NULL);
#ifdef __AMIGA__
			ClearPointer(_window(mouse_data->obj));
			if(!display->always_coord)
				MUI_RejectIDCMP(mouse_data->obj,IDCMP_MOUSEMOVE);
#endif
			// set edit mode to non active
			display->editmode &= ~EDIT_ACTIVE;
			// restore old display mode
			display->display = mouse_data->olddisplay;

			// if the user changed something update objects and redraw display
			if(mouse_data->changed)
			{
				SetProjectChanged(TRUE);
				if((display->editmode & EDIT_OBJECT) || (display->editmode & EDIT_MESH))
				{
					GetActiveObjects(&selobj);

					if(selobj)
					{
						undo = new UNDO_TRANSFORM();
						if(undo)
							undo->Add();
						undo->AddSelectedObjects();

						if(!display->localalign)
						{
							VECTOR pos, ox,oy,oz;
							MATRIX matrix,m,m1;

							// need absolute axis if rotating in world
							selobj->GetObjectMatrix(&m);
							m.GenerateAxis(&ox, &oy, &oz, &pos);
							m.SetOMatrix(&ox, &oy, &oz);
							m1.SetOMatrix(&display->orient_x, &display->orient_y, &display->orient_z);
							matrix.MultMat(&m, &m1);
							SetVector(&display->orient_x, matrix.m[5], matrix.m[9], matrix.m[13]);
							SetVector(&display->orient_y, matrix.m[6], matrix.m[10], matrix.m[14]);
							SetVector(&display->orient_z, matrix.m[7], matrix.m[11], matrix.m[15]);
						}
						SetActiveObjects(display);
						if(view->viewmode == VIEW_CAMERA && display->camera->selected)
						{
							view->SetCamera(display->camera);
#ifdef __AMIGA__
							MUI_Redraw(mouse_data->obj,MADF_DRAWUPDATE);
#else
							mouse_data->pView->Redraw(REDRAW_ALL, FALSE);
#endif
						}
						else
						{
							if(((selobj->GetType() != OBJECT_MESH) && (selobj->GetType() != OBJECT_SOR)) || !(display->editmode & EDIT_MESH))
							{
#ifdef __AMIGA__
								Redraw((struct Draw_Data*)INST_DATA(mouse_data->cl,mouse_data->obj), REDRAW_SELECTED, FALSE);
#else
								mouse_data->pView->Redraw(REDRAW_SELECTED, FALSE);
#endif
							}
						}
					}
				}
				else
				{
#ifdef __AMIGA__
					Redraw((struct Draw_Data*)INST_DATA(mouse_data->cl,mouse_data->obj), REDRAW_CURRENT_VIEW, FALSE);
#else
					mouse_data->pView->Redraw(REDRAW_ALL, FALSE);
#endif
				}
			}
			*breaked = TRUE;
		}
		return;
	}

	// handles right button down
	if (flags & BUTTON_RIGHTDOWN)
	{
		if (display->editmode & EDIT_ACTIVE)
		{
			display->editmode |= EDIT_RMB;
			return;
		}
		// test if object selected
		selobj = TestSelected(display,x,y, shift);

#ifdef __AMIGA__
		SetSleep(TRUE);
		if(selobj)
		{
			if(selobj->flags & OBJECT_DIRTY)
			{
				sciBrowserBuild();
				Redraw((struct Draw_Data*)INST_DATA(mouse_data->cl,mouse_data->obj), REDRAW_DIRTY, FALSE);
			}

			// create a object popup menu
			mouse_data->popup_menu = selobj->GetPopUpList();
			mouse_data->popup_menu->nm_Label = selobj->GetName();
		}
		else
		{
			// create the world popup menu
			mouse_data->popup_menu = world_popupmenu;
		}
		mouse_data->selobj = selobj;
		SetSleep(FALSE);
#else
		CMenu menu;
		int resource;

		if (selobj)
		{
			if(selobj->flags & OBJECT_DIRTY)
			{
				sciBrowserBuild();
				mouse_data->pView->Redraw(REDRAW_DIRTY, FALSE);
			}
			resource = selobj->GetResourceID();
		}
		else
			resource = IDR_POPUP_WORLD;

		CPoint point(mouse_x, mouse_y);
		mouse_data->pView->ClientToScreen(&point);
		if (menu.LoadMenu(resource))
		{
			CMenu *pSubMenu = menu.GetSubMenu(0);
			if (pSubMenu != NULL)
			{
				if (resource == IDR_POPUP_OBJECT)
				{
					if (selobj->external != EXTERNAL_NONE)
						pSubMenu->EnableMenuItem(IDM_TOSCENE, MF_ENABLED);
					else
						pSubMenu->EnableMenuItem(IDM_TOSCENE, MF_GRAYED);
				}
				pSubMenu->TrackPopupMenu(TPM_CENTERALIGN | TPM_RIGHTBUTTON, point.x, point.y, mouse_data->pView);
			}
		}
#endif
		return;
	}

	// handles right button up
	if (flags & BUTTON_RIGHTUP)
	{
		display->editmode &= ~EDIT_RMB;
		return;
	}
}
