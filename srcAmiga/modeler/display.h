/***************
 * PROJECT:       Scenario
 * MODULE:        display.h
 * DESCRIPTION:   display definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    23.06.96 mh    compatible for Amiga & PC
 *    31.01.97 ah    added EDIT_SELBOX
 *    07.02.97 ah    added ToDefaults()
 *    22.02.97 ah    added EDIT_POINTS and DrawTrack()
 ***************/

#ifndef DISPLAY_H
#define DISPLAY_H

typedef struct
{
	int left, top, right, bottom;
} RECTANGLE2D;

class DISPLAY;
class OBJECT;

#ifndef VIEW_H
#include "view.h"
#endif

#ifndef GFX_H
#include "gfx.h"
#endif

#ifdef __AMIGA__
#include <graphics/rastport.h>
#else
#include <afxwin.h>
#endif

#define SET_DISPMODE    0x100
#define SET_VIEWMODE    0x200
#define SET_EDITMODE    0x300

// display mode
enum
{
	DISPLAY_BBOX,        // bounding box
	DISPLAY_WIRE,        // wire frame
	DISPLAY_SOLID,       // solid
};

// edit mode
enum
{
	EDIT_MOVE,EDIT_ROTATE,EDIT_SCALE
};

#define EDIT_MASK       3
#define EDIT_OBJECT     0x0004   // manipulate object not world
#define EDIT_RMB        0x0008   // right mouse button pressed
#define EDIT_ACTIVE     0x0010   // I'm doing something
#define EDIT_SELBOX     0x0020   // select objects with drag box
#define EDIT_MESH       0x0040   // edit a mesh object (the following 6 defines determine the exact edit mode)
#define EDIT_EDITPOINTS 0x0080   // edit points of mesh objects
#define EDIT_ADDPOINTS  0x0100   // add points to mesh objects
#define EDIT_EDITEDGES  0x0200   // edit edges to mesh objects
#define EDIT_ADDEDGES   0x0400   // add edges to mesh objects
#define EDIT_EDITFACES  0x0800   // edit faces to mesh objects
#define EDIT_ADDFACES   0x1000   // add faces to mesh objects

#define ACTIVE_X        1
#define ACTIVE_Y        2
#define ACTIVE_Z        4

// redraw mode
enum
{
	REDRAW_ALL,
	REDRAW_OBJECTS,
	REDRAW_SELECTED,
	REDRAW_DIRTY,
	REDRAW_CURRENT_VIEW,
	REDRAW_REMOVE_SELECTED
};

// box modes
enum
{
	BOXMODE_SELECT, BOXMODE_RENDERWINDOW
};

// axis flags
#define AXIS_SELECTED   1
#define AXIS_REMOVE     2

// filter flags
enum FILTER_FLAGS
{
	FILTER_CAMERA =   0x0001,
	FILTER_LIGHT =    0x0002,
	FILTER_MESH =     0x0004,
	FILTER_SPHERE =   0x0008,
	FILTER_BOX =      0x0010,
	FILTER_CYLINDER = 0x0020,
	FILTER_CONE =     0x0040,
	FILTER_SOR =      0x0080,
	FILTER_PLANE =    0x0100,
	FILTER_CSG =      0x0200,
	FILTER_BRUSH =    0x0400,
	FILTER_TEXTURE =  0x0800,
	FILTER_ALL =      0xFFFF
};

extern GFX gfx;

class CAMERA;

class DISPLAY
{
public:
	VECTOR bboxmin, bboxmax;   // scene bounding box
	SHORT vpwidth, vpheight;   // size of viewport
	SHORT wleft,wtop;          // top left corner of window
	SHORT wwidth,wheight;      // width and height of window
	UWORD display;             // display type
	UWORD editmode;
	UWORD active;              // active axes
	UWORD boxmode;             // mode of selection box
	BOOL grid;                 // grid on/off
	BOOL gridsnap;             // gridsnap on/off
	BOOL always_coord;         // always show coordinates on/off
	BOOL coord_disp;           // coordinate display on/off
	BOOL name_disp;            // name display on/off
	BOOL multiview;            // multi view on/off
	BOOL usesurfacecolors;     // use surface color for objects
#ifdef __AMIGA__
	BOOL directrender;         // use direct rendering with StormMesa
#endif // __AMIGA__
	BOOL localpos,localalign,localsize;
	float gridsize;
	FILTER_FLAGS filter_flags;

	CAMERA *camera;            // active camera
	VIEW *views[4], *view;     // pointer to views

	// current modified parameters (relative)
	VECTOR pos;                // position
	VECTOR orient_x;           // orientation
	VECTOR orient_y;
	VECTOR orient_z;
	VECTOR size;               // size

	VECTOR offset;             // offset of object for snap to grid
	RECTANGLE2D selbox;        // selection box

private:
	RECTANGLE accrect;         // bounding rectangle of current object position

public:
	DISPLAY();
	~DISPLAY();
	void ToDefaults();

#ifdef __OPENGL__
	void GetViewRange(float*, float*);
	void SetupOpenGL();
#endif
	void SetViewport(int, int);
	void SetMultiView(BOOL);
	void AccumulateBoundingRect(OBJECT*, MATRIX_STACK*);
	void AccumulateRect2D(int, int, int, int);
	void DrawLine(VECTOR*, VECTOR*);
	void DrawCross();
	void DrawGrid();
	void DrawSelBox(MATRIX_STACK*, VECTOR*, VECTOR*);
	void DrawBox(MATRIX_STACK*, VECTOR*, VECTOR*);
	void DrawSphere(MATRIX_STACK*, float);
	void DrawCylinder(MATRIX_STACK*, VECTOR*, ULONG);
	void DrawName(MATRIX_STACK*, char*);
	void DrawAxis(MATRIX_STACK*, ULONG, VECTOR*);
	BOOL OutOfScreen(MATRIX_STACK*, VECTOR*, VECTOR*);
	BOOL CompletelyInScreen(MATRIX_STACK*, VECTOR*, VECTOR*);
	void TransformDraw(VECTOR*, VECTOR*, MATRIX_STACK*);
	VIEW *SetViewByPoint(int, int);
	void GetRenderRect(RECTANGLE *);
	void DrawRenderLines(BOOL);
	void Redraw(int, BOOL);
	void DrawSelBox();
	void DrawTrack(int, int);
	void SetDisplayMode(ULONG);

private:
	void RectangleTo2D(RECTANGLE*);
	void AccumulateRect(VECTOR*, MATRIX_STACK*);
	void UnionRectangle(RECTANGLE*, RECTANGLE*, SHORT,SHORT,SHORT,SHORT);
	void RedrawView(VIEW*, VIEW*, int, BOOL);
};

#endif
