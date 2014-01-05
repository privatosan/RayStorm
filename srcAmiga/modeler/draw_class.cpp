/***************
 * PROGRAM:       Modeler
 * NAME:          draw_class.cpp
 * DESCRIPTION:   MUI draw class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    16.12.96 ah    added undo
 *    30.01.97 ah    added status bar and redo
 ***************/

#include <string.h>
#include <libraries/gadtools.h>
#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <intuition/intuition.h>
#include <exec/exec.h>

#include <pragma/intuition_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/exec_lib.h>
#include <pragma/utility_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef DRAW_CLASS_H
#include "draw_class.h"
#endif

#ifndef MUIUTILITY_CLASS_H
#include "MUIutility.h"
#endif

#ifndef POPUP_MENU_H
#include "popup_menu.h"
#endif

#ifndef MAINWINDOW_CLASS_H
#include "mainwindow_class.h"
#endif

#ifndef SETTINGS_CLASS_H
#include "settings_class.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef GFX_H
#include "gfx.h"
#endif

#ifndef MOUSE_H
#include "mouse.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
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

// area parameters
#define MINWIDTH  100
#define DEFWIDTH  256
#define MAXWIDTH  2048

#define MINHEIGHT 100
#define DEFHEIGHT 256
#define MAXHEIGHT 2048

typedef struct
{
	char  key;
	ULONG code;
} MENU_KEYS;

MENU_KEYS menukeys[] =
{
	'a',MUIM_MainWindow_Attributes,
	'e',MUIM_MainWindow_Render,
	'l',MUIM_MainWindow_LoadObj,
	'o',MUIM_MainWindow_Open,
	's',MUIM_MainWindow_Save,
	't',MUIM_MainWindow_Settings,
	'w',MUIM_MainWindow_SaveObj,
	'?',MUIM_MainWindow_About,
	0,0
};

GFX gfx;
static MOUSEDATA mouse_data;

static BOOL breaked = TRUE;

/*************
 * DESCRIPTION:   OM_SET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG mSet(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct Draw_Data *data = (struct Draw_Data *)INST_DATA(cl,obj);
	DISPLAY *display = data->display;
	VIEW *view;
	struct TagItem *tags,*tag;
	// after loading a scene we have to set the buttons, but the button methods
	// call this set method and the screen is updated. So we set update to FALSE
	// before we set the buttons and afterwards to TRUE (this is all done in
	// mainwindow_class.c).
	static BOOL update = TRUE;

	if(display)
		view = display->view;

	tags=msg->ops_AttrList;
	do
	{
		tag = WARPOS_PPC_FUNC(NextTagItem)(&tags);
		if(tag)
		{
			switch (tag->ti_Tag)
			{
				case MUIA_Draw_SetRenderWindow:
					if(update)
						sciDoMethod(SCIM_SETRENDERWINDOW);
					break;
				case MUIA_Draw_SelectByBox:
					if(update)
						sciDoMethod(SCIM_SELECTBYBOX);
					break;
				case MUIA_Draw_SetUpdate:
					update = (BOOL)tag->ti_Data;
					break;
				case MUIA_Draw_Window:
					data->win = (Object *)tag->ti_Data;
					mouse_data.win = data->win;
					break;
				case MUIA_Draw_ViewFour:
					if(update)
						sciDoMethod(SCIM_VIEWQUAD);
					break;
				case MUIA_Draw_View:
					if(update)
					{
						switch(tag->ti_Data)
						{
							case VIEW_FRONT:
								sciDoMethod(SCIM_VIEWFRONT);
								break;
							case VIEW_RIGHT:
								sciDoMethod(SCIM_VIEWRIGHT);
								break;
							case VIEW_TOP:
								sciDoMethod(SCIM_VIEWTOP);
								break;
							case VIEW_PERSP:
								sciDoMethod(SCIM_VIEWPERSP);
								break;
							case VIEW_CAMERA:
								sciDoMethod(SCIM_VIEWCAMERA);
								break;
						}
					}
					break;
				case MUIA_Draw_Display:
					if(update)
					{
						switch((UBYTE)tag->ti_Data)
						{
							case DISPLAY_BBOX:
								sciDoMethod(SCIM_DISPLAYBBOX);
								break;
							case DISPLAY_WIRE:
								sciDoMethod(SCIM_DISPLAYWIRE);
								break;
							case DISPLAY_SOLID:
								sciDoMethod(SCIM_DISPLAYSOLID);
								break;
						}
					}
					break;
				case MUIA_Draw_GridSize:
					if(update)
					{
						display->gridsize = *(float*)(tag->ti_Data);
						if(display->grid)
							MUI_Redraw(obj,MADF_DRAWUPDATE);
					}
					break;
				case MUIA_Draw_CoordDisp:
					display->coord_disp = (BOOL)tag->ti_Data;
					break;
			}
		}
	}
	while(tag);

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   OM_GET method
 * INPUT:         system
 * OUTPUT:
 *************/
static ULONG mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
	struct Draw_Data *data = (struct Draw_Data *)INST_DATA(cl,obj);
	float *v;

	switch (msg->opg_AttrID)
	{
		case MUIA_Draw_GridSize:
			v = (float*)(msg->opg_Storage);
			*v = data->display->gridsize;
			return TRUE;
		case MUIA_Draw_DisplayPtr:
			*(DISPLAY**)(msg->opg_Storage) = data->display;
			return TRUE;
	}

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   set mouse position
 * INPUT:         data     object data
 *                screen   current screen
 *                x,y      new mouse position
 * OUTPUT:
 *************/
static void SetMouse(struct Draw_Data *data, struct Screen *screen, WORD x, WORD y)
{
	data->NeoPix->iepp_Screen = screen;
	data->NeoPix->iepp_Position.X = x;
	data->NeoPix->iepp_Position.Y = y;

	data->FakeEvent->ie_EventAddress = (APTR)data->NeoPix;
	data->FakeEvent->ie_NextEvent = NULL;
	data->FakeEvent->ie_Class = IECLASS_NEWPOINTERPOS;
	data->FakeEvent->ie_SubClass = IESUBCLASS_PIXEL;
	data->FakeEvent->ie_Code = 0;
	data->FakeEvent->ie_Qualifier = NULL;

	data->InputIO->io_Data = (APTR)data->FakeEvent;
	data->InputIO->io_Length = sizeof(struct InputEvent);
	data->InputIO->io_Command = IND_WRITEEVENT;
	DoIO((struct IORequest *)data->InputIO);
}

/*************
 * DESCRIPTION:   AskMinMax method will be called before the window is
 *    opened and before layout takes place. We need to tell MUI the
 *    minimum, maximum and default size of our object.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG AskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	/* let our superclass first fill in what it thinks about sizes.
		this will e.g. add the size of frame and inner spacing. */
	DOSUPERMETHODA(cl,obj,(Msg)msg);

	/* now add the values specific to our object. note that we
		indeed need to *add* these values, not just set them! */
	msg->MinMaxInfo->MinWidth  += MINWIDTH;
	msg->MinMaxInfo->DefWidth  += DEFWIDTH;
	msg->MinMaxInfo->MaxWidth  += MAXWIDTH;

	msg->MinMaxInfo->MinHeight += MINHEIGHT;
	msg->MinMaxInfo->DefHeight += DEFHEIGHT;
	msg->MinMaxInfo->MaxHeight += MAXHEIGHT;

	return(0);
}

/*************
 * DESCRIPTION:   redraws the display
 * INPUT:         data
 *                mode     draw mode
 *                changed  display changed since editing began
 * OUTPUT:        none
 *************/
void Redraw(struct Draw_Data *data, int mode, BOOL changed)
{
	OBJECT *object=NULL;

	if(!(data->display->editmode & EDIT_ACTIVE))
	{
		// Update status bar
		GetActiveObjects(&object);
		if(object)
			SetAttrs(data->win, MUIA_MainWindow_SelectedObject, object->GetName(), TAG_DONE);
		else
			SetAttrs(data->win, MUIA_MainWindow_SelectedObject, "", TAG_DONE);
		if(data->display->camera)
			SetAttrs(data->win, MUIA_MainWindow_ActiveCamera, data->display->camera->GetName(), TAG_DONE);
		else
			SetAttrs(data->win, MUIA_MainWindow_ActiveCamera, "", TAG_DONE);
	}

	breaked = FALSE;

	data->display->Redraw(mode, changed);

	breaked = TRUE;
}

/*************
 * DESCRIPTION:   Draw method is called whenever MUI feels we should render
 *    our object. This usually happens after layout is finished
 *    or when we need to refresh in a simplerefresh window.
 *    Note: You may only render within the rectangle
 *       _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj).
 * INPUT:         system
 * OUTPUT:        0
 *************/
static SAVEDS ULONG Draw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct Draw_Data *data = (struct Draw_Data *)INST_DATA(cl,obj);
	DISPLAY *display = data->display;
	BOOL newbitmap = FALSE;

	/* let our superclass draw itself first, area class would
		e.g. draw the frame and clear the whole region. What
		it does exactly depends on msg->flags.
		Note: You *must* call the super method prior to do
		anything else, otherwise msg->flags will not be set
		properly !!! */
	DOSUPERMETHODA(cl,obj,(Msg)msg);

	if((data->width != _mwidth(obj)) || (data->height != _mheight(obj)))
	{
		if(data->width != -1)
			gfx.Cleanup();

#ifdef __OPENGL__
		data->display->views[0]->opengl.Initialize(obj);
		data->display->views[1]->opengl.Initialize(obj);
		data->display->views[2]->opengl.Initialize(obj);
		data->display->views[3]->opengl.Initialize(obj);
#endif // __OPENGL__

		data->width = _mwidth(obj);
		data->height = _mheight(obj);
		gfx.Init(obj, display);
		data->display->SetViewport(_mwidth(obj), _mheight(obj));
		Redraw(data, REDRAW_ALL, mouse_data.changed);
	}
	else
	{
		if(msg->flags & MADF_DRAWOBJECT)
			gfx.BitBlt(GFX_MEMORY1, 0, 0, _mwidth(obj), _mheight(obj), GFX_SCREEN, 0, 0);

		if(msg->flags & MADF_DRAWUPDATE)
			Redraw(data, REDRAW_ALL, mouse_data.changed);
	}

	return(0);
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG OM_Dispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct Draw_Data *data = (struct Draw_Data *)INST_DATA(cl,obj);

	if(data->display)
		delete data->display;
 
	if(data->contextmenu)
		MUI_DisposeObject(data->contextmenu);

	return(DOSUPERMETHODA(cl,obj,msg));
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG Setup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
	struct Draw_Data *data = (struct Draw_Data *)INST_DATA(cl,obj);
	struct Screen *screen;
	PREFS p;

	if (!(DOSUPERMETHODA(cl,obj,(Msg)msg)))
		return(FALSE);

	if(!data->display)
	{
		data->display = new DISPLAY;
		if(!data->display)
			return FALSE;

		mouse_data.cl = cl;
		mouse_data.obj = obj;
		mouse_data.display = data->display;

		p.id = ID_FLGS;
		if(p.GetPrefs())
		{
			if(*((ULONG*)p.data) & PREFFLAG_SHOWCOORD)
				data->display->always_coord = TRUE;
		}
		p.data = NULL;
	}

	data->InputMP = CreateMsgPort();
	if(data->InputMP)
	{
		data->FakeEvent = (struct InputEvent*)AllocVec(sizeof(struct InputEvent), MEMF_PUBLIC);
		if(data->FakeEvent)
		{
			data->NeoPix = (struct IEPointerPixel*)AllocVec(sizeof(struct IEPointerPixel), MEMF_PUBLIC);
			if(data->NeoPix)
			{
				data->InputIO = (struct IOStdReq *)CreateIORequest(data->InputMP, sizeof(struct IOStdReq));
				if(data->InputIO)
				{
					if(OpenDevice("input.device", NULL, (struct IORequest *)data->InputIO, NULL))
					{
						return FALSE;
					}
				}
			}
		}
	}

	if(!data->display->always_coord)
		MUI_RequestIDCMP(obj,IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_VANILLAKEY);
	else
		MUI_RequestIDCMP(obj,IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_VANILLAKEY|IDCMP_MOUSEMOVE);

	GetAttr(MUIA_Window_Screen, data->win, (ULONG*)&screen);
	utility.InitScreen(screen);

	data->contextmenu = NULL;
	data->width = -1;
	data->height = -1;

	return(TRUE);
}

/*************
 * DESCRIPTION:
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG Cleanup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
	struct Draw_Data *data = (struct Draw_Data *)INST_DATA(cl,obj);

	if(!data->display->always_coord)
		MUI_RejectIDCMP(obj,IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_VANILLAKEY);
	else
		MUI_RejectIDCMP(obj,IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_VANILLAKEY|IDCMP_MOUSEMOVE);

	if(data->InputIO)
	{
		CloseDevice((struct IORequest *)data->InputIO);
		DeleteIORequest(data->InputIO);
		data->InputIO = NULL;
	}
	if(data->NeoPix)
	{
		FreeVec(data->NeoPix);
		data->NeoPix = NULL;
	}
	if(data->FakeEvent)
	{
		FreeVec(data->FakeEvent);
		data->FakeEvent = NULL;
	}
	if(data->InputMP)
	{
		DeleteMsgPort(data->InputMP);
		data->InputMP = NULL;
	}

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   activates the gadget according to edit mode
 * INPUT:         win      window
 *                mode     editmode
 * OUTPUT:        none
 *************/
void SetActiveGadget(Object *win, int mode)
{
	if(mode & EDIT_OBJECT)
	{
		if((mode & EDIT_MASK) == EDIT_MOVE)
			SetAttrs(win,MUIA_MainWindow_Active_Gadget,GADGET_MOVE, TAG_DONE);
		else
		{
			if((mode & EDIT_MASK) == EDIT_ROTATE)
				SetAttrs(win,MUIA_MainWindow_Active_Gadget,GADGET_ROTATE, TAG_DONE);
			else
				SetAttrs(win,MUIA_MainWindow_Active_Gadget,GADGET_SCALE, TAG_DONE);
		}
	}
	else
	{
		if((mode & EDIT_MASK) == EDIT_MOVE)
			SetAttrs(win,MUIA_MainWindow_Active_Gadget,GADGET_WMOVE, TAG_DONE);
		else
		{
			if((mode & EDIT_MASK) == EDIT_ROTATE)
				SetAttrs(win,MUIA_MainWindow_Active_Gadget,GADGET_WROTATE, TAG_DONE);
			else
				SetAttrs(win,MUIA_MainWindow_Active_Gadget,GADGET_WSCALE, TAG_DONE);
		}
	}
}

/*************
 * DESCRIPTION:   this function is called from the draw routines in object.c.
 *    It checks if the user want's to break the redraw. The test is done in
 *    HandleVanillaKey() and checks for the ESC-key.
 * INPUT:         win      window
 *                mode     editmode
 * OUTPUT:        none
 *************/
BOOL TestBreaked()
{
	DOMETHOD(app, MUIM_Application_InputBuffered);
	return breaked;
}

/*************
 * DESCRIPTION:   handle vanilla key event
 * INPUT:         obj      object
 *                imsg     message
 *                data     draw class data
 * OUTPUT:        none
 *************/
static void HandleVanillaKey(Object *obj, struct IntuiMessage *imsg, struct Draw_Data *data)
{
	DISPLAY *display = data->display;
	VIEW *view = display->view;
	int i;
	ULONG active;

	// handle keypress events only if window is active
	GetAttr(MUIA_Window_Activate,data->win,(ULONG*)&active);
	if(!active)
		return;

	switch(imsg->Qualifier & 0x7fff)
	{
		case 0:
			switch (imsg->Code)
			{
				case 127:
					sciDoMethod(SCIM_DELETE);
					break;
				case 'm':
					SetActiveGadget(data->win, EDIT_MOVE | EDIT_OBJECT);
					break;
				case 'r':
					SetActiveGadget(data->win, EDIT_ROTATE | EDIT_OBJECT);
					break;
				case 's':
					SetActiveGadget(data->win, EDIT_SCALE | EDIT_OBJECT);
					break;
				case 't':
					DOMETHOD(obj,MUIM_Draw_OpenSettings);
					break;
				case 'a':
					DOMETHOD(data->win,MUIM_MainWindow_Attributes);
					break;
				case '+':
					sciDoMethod(SCIM_ZOOMIN);
					break;
				case '-':
					sciDoMethod(SCIM_ZOOMOUT);
					break;
				case '1':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_FRONT,TAG_DONE);
					break;
				case '2':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_RIGHT,TAG_DONE);
					break;
				case '3':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_TOP,TAG_DONE);
					break;
				case '4':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_PERSP,TAG_DONE);
					break;
				case '5':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_FOUR,TAG_DONE);
					break;
				case '6':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_CAMERA,TAG_DONE);
					break;
				case 'b':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_SELBOX,TAG_DONE);
					break;
				case 'x':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_X_TOGGLE,TAG_DONE);
					break;
				case 'y':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_Y_TOGGLE,TAG_DONE);
					break;
				case 'z':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_Z_TOGGLE,TAG_DONE);
					break;
			}
			break;
		case IEQUALIFIER_LSHIFT:
		case IEQUALIFIER_RSHIFT:
			switch(imsg->Code)
			{
				case 'M':
					SetActiveGadget(data->win, EDIT_MOVE);
					break;
				case 'R':
					SetActiveGadget(data->win, EDIT_ROTATE);
					break;
				case 'S':
					SetActiveGadget(data->win, EDIT_SCALE);
					break;
				case 'X':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_X,TAG_DONE);
					break;
				case 'Y':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_Y,TAG_DONE);
					break;
				case 'Z':
					SetAttrs(data->win,MUIA_MainWindow_Active_Gadget,GADGET_Z,TAG_DONE);
					break;
			}
			break;
		case (IEQUALIFIER_RCOMMAND | IEQUALIFIER_LSHIFT):
		case (IEQUALIFIER_RCOMMAND | IEQUALIFIER_RSHIFT):
		case IEQUALIFIER_RCOMMAND:
			switch (imsg->Code)
			{
				case '.':
					if(Center(display, imsg->MouseX-_mleft(obj), imsg->MouseY-_mtop(obj)))
						MUI_Redraw(obj,MADF_DRAWUPDATE);
					break;
				case '+':
					sciDoMethod(SCIM_ZOOMIN);
					break;
				case '-':
					sciDoMethod(SCIM_ZOOMOUT);
					break;
				case 'c':
					sciDoMethod(SCIM_COPY);
					break;
				case 'd':
					sciDoMethod(SCIM_DELETE);
					break;
				case 'f':
				case 'F':
					sciDoMethod(SCIM_ZOOMFIT);
					break;
				case 'g':
					sciDoMethod(SCIM_GROUP);
					break;
				case 'n':
					sciDoMethod(SCIM_SELNEXT);
					break;
				case 'p':
					sciDoMethod(SCIM_SELPREV);
					break;
				case 'q':
				case 'Q':
					DOMETHOD(app,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
					break;
				case 'r':
					sciDoMethod(SCIM_REDRAW);
					break;
				case 'u':
					sciDoMethod(SCIM_UNGROUP);
					break;
				case 'v':
					sciDoMethod(SCIM_PASTE);
					break;
				case 'x':
					sciDoMethod(SCIM_CUT);
					break;
				case 'y':
					sciDoMethod(SCIM_REDO);
					break;
				case 'z':
					sciDoMethod(SCIM_UNDO);
					break;
				default:
					i = 0;
					while(menukeys[i].key && (menukeys[i].key != imsg->Code))
						i++;

					if(menukeys[i].code)
						DOMETHOD(data->win,menukeys[i].code);
					break;
			}
	}
}

/*************
 * DESCRIPTION:   tests cursor keys and changes x and y according to pressed
 *    keys
 * INPUT:         x,y      coordinates to change
 *                code     key code
 *                view     actual view
 * OUTPUT:        TRUE if cursor key pressed else FALSE
 *************/
static BOOL TestCursorKeys(float *x,float *y,UWORD code,VIEW *view)
{
	switch (code)
	{
		case CURSORLEFT:
			*x -= (view->width/view->zoom)*.25;
			return TRUE;
		case CURSORRIGHT:
			*x += (view->width/view->zoom)*.25;
			return TRUE;
		case CURSORUP:
			*y += (view->height/view->zoom)*.25;
			return TRUE;
		case CURSORDOWN:
			*y -= (view->height/view->zoom)*.25;
			return TRUE;
	}
	return FALSE;
}

/*************
 * DESCRIPTION:   handle raw key event
 * INPUT:         system
 * OUTPUT:        none
 *************/
static void HandleRawKey(Object *obj,struct IntuiMessage *imsg,struct Draw_Data *data)
{
	VIEW *view = data->display->view;
	ULONG active;
	VECTOR v;

	// handle keypress events only if window is active
	GetAttr(MUIA_Window_Activate,data->win,(ULONG*)&active);
	if(!active)
		return;

	if((imsg->Qualifier & 0x7fff) == 0)
	{
		switch(imsg->Code)
		{
			case 80:
				DOMETHOD(data->win,MUIM_MainWindow_RenderSettings);
				break;
			case CURSORLEFT:
			case CURSORRIGHT:
			case CURSORUP:
			case CURSORDOWN:
				switch(view->viewmode)
				{
					case VIEW_FRONT:
						if(TestCursorKeys(&view->viewpoint.x,&view->viewpoint.y,imsg->Code,view))
							Redraw(data, REDRAW_CURRENT_VIEW, mouse_data.changed);
					break;
					case VIEW_RIGHT:
						if(TestCursorKeys(&view->viewpoint.z,&view->viewpoint.y,imsg->Code,view))
							Redraw(data, REDRAW_CURRENT_VIEW, mouse_data.changed);
						break;
					case VIEW_TOP:
						if(TestCursorKeys(&view->viewpoint.x,&view->viewpoint.z,imsg->Code,view))
							Redraw(data, REDRAW_CURRENT_VIEW, mouse_data.changed);
						break;
				}

				if(data->display->always_coord)
				{
					if(MouseTo3D(data->display, imsg->MouseX-_mleft(obj), imsg->MouseY-_mtop(obj), &v))
						SetCoordDisplay(&v, data->win);
					else
						SetCoordDisplay(NULL, data->win);
				}
				break;
		}
	}
}

/*************
 * FUNCTION:      SetCoordDisplay
 * DESCRIPTION:   sets the values in the coordinate display
 * INPUT:         v        coordinates (if this is NULL, clear display)
 *                win      window
 * OUTPUT:        none
 *************/
void SetCoordDisplay(const VECTOR *v, Object *win)
{
	char buffer[30];

	if(!v)
	{
		SetAttrs(win, MUIA_MainWindow_CoordX, "", TAG_DONE);
		SetAttrs(win, MUIA_MainWindow_CoordY, "", TAG_DONE);
		SetAttrs(win, MUIA_MainWindow_CoordZ, "", TAG_DONE);
	}
	else
	{
		Float2String(v->x, buffer);
		SetAttrs(win,MUIA_MainWindow_CoordX,buffer, TAG_DONE);
		Float2String(v->y, buffer);
		SetAttrs(win,MUIA_MainWindow_CoordY,buffer, TAG_DONE);
		Float2String(v->z, buffer);
		SetAttrs(win,MUIA_MainWindow_CoordZ,buffer, TAG_DONE);
	}
}

/*************
 * DESCRIPTION:   handle intuition input messages
 * INPUT:         system
 * OUTPUT:        none
 *************/
static SAVEDS ULONG HandleInput(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
	struct Draw_Data *data = (struct Draw_Data*)INST_DATA(cl,obj);
	VECTOR v;
	int flags;
	RECTANGLE r;

	if (msg->imsg)
	{
		switch (msg->imsg->Class)
		{
			case IDCMP_MOUSEBUTTONS:
				flags = BUTTON_NONE;
				switch(msg->imsg->Code)
				{
					case SELECTDOWN:
						flags |= BUTTON_LEFTDOWN;
						break;
					case SELECTUP:
						flags |= BUTTON_LEFTUP;
						break;
					case MENUDOWN:
						flags |= BUTTON_RIGHTDOWN;
						break;
					case MENUUP:
						flags |= BUTTON_RIGHTUP;
						break;
				}
				if(flags != BUTTON_NONE)
				{
					if(msg->imsg->Qualifier & IEQUALIFIER_RSHIFT)
						flags |= BUTTON_RIGHTSHIFT;
					if(msg->imsg->Qualifier & IEQUALIFIER_LSHIFT)
						flags |= BUTTON_LEFTSHIFT;
					data->oldmousex = msg->imsg->MouseX;
					data->oldmousey = msg->imsg->MouseY;
					HandleMouseButtons(&mouse_data, msg->imsg->MouseX-_mleft(obj), msg->imsg->MouseY-_mtop(obj), flags, &breaked);
				}
				break;
			case IDCMP_MOUSEMOVE:
				if(data->display->editmode & EDIT_SELBOX)
				{
					if(data->display->boxmode == BOXMODE_RENDERWINDOW)
						data->display->GetRenderRect(&r);
					else
					{
						r.left = data->display->view->left;
						r.right = data->display->view->right;
						r.top = data->display->view->top;
						r.bottom = data->display->view->bottom;
					}
					data->display->DrawSelBox();
					if(msg->imsg->MouseX-_mleft(obj) < r.left)
						data->display->selbox.right = r.left;
					else
					{
						if(msg->imsg->MouseX-_mleft(obj) > r.right)
							data->display->selbox.right = r.right;
						else
							data->display->selbox.right = msg->imsg->MouseX-_mleft(obj);
					}
					if(msg->imsg->MouseY-_mtop(obj) < r.top)
						data->display->selbox.bottom = r.top;
					else
					{
						if(msg->imsg->MouseY-_mtop(obj) > r.bottom)
							data->display->selbox.bottom = r.bottom;
						else
							data->display->selbox.bottom = msg->imsg->MouseY-_mtop(obj);
					}
					data->display->DrawSelBox();
				}
				else
				{
					if(data->display->editmode & EDIT_ACTIVE)
					{
						if((data->oldmousex != msg->imsg->MouseX) ||
							(data->oldmousey != msg->imsg->MouseY))
						{
							mouse_data.oldmousex = data->oldmousex;
							mouse_data.oldmousey = data->oldmousey;
							DoMouseMove(data->display, msg->imsg->MouseX, msg->imsg->MouseY, &mouse_data);
							Redraw(data, REDRAW_OBJECTS, mouse_data.changed);
							SetMouse(data, _screen(obj), _window(obj)->LeftEdge+data->oldmousex, _window(obj)->TopEdge+data->oldmousey);
							mouse_data.changed = TRUE;
						}
					}
					else
					{
						if(data->display->always_coord && breaked)
						{
							if(MouseTo3D(data->display, msg->imsg->MouseX-_mleft(obj), msg->imsg->MouseY-_mtop(obj), &v))
								SetCoordDisplay(&v, data->win);
							else
								SetCoordDisplay(NULL, data->win);
						}
					}
				}
				break;
			case IDCMP_RAWKEY:
				if(breaked)
					HandleRawKey(obj, msg->imsg, data);
				break;
			case IDCMP_VANILLAKEY:
				if(breaked)
				{
					HandleVanillaKey(obj, msg->imsg, data);
				}
				else
				{
					if(!(msg->imsg->Qualifier & 0x7fff) && (msg->imsg->Code == 27))
						breaked = TRUE;
				}
				break;
		}
	}

	return(DOSUPERMETHODA(cl,obj,(Msg)msg));
}

/*************
 * DESCRIPTION:   open setings window
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG OpenSettings(struct IClass *cl,Object *obj,Msg msg)
{
	struct Draw_Data *data = (struct Draw_Data*)INST_DATA(cl,obj);
	OBJECT *object;
	VECTOR align, pos, orient_x, orient_y, orient_z;
	MATRIX m;
	ULONG type;
	UNDO_TRANSFORM *undo;

	GetActiveObjects(&object);

	// - no object selected -> return without opening the window
	if(!object)
		return 0;

	// requester already open -> pop it to front
	if(object->win)
	{
		DOMETHOD(object->win, MUIM_Window_ToFront);
		SetAttrs(object->win, MUIA_Window_Activate, TRUE, TAG_DONE);
		return 0;
	}

	undo = new UNDO_TRANSFORM;
	if(!undo)
		return 0;
	undo->AddSelectedObjects();

	switch(object->GetType())
	{
		case OBJECT_CAMERA:
			type = SETTINGS_SETCAMERA;
			break;
		case OBJECT_POINTLIGHT:
			type = SETTINGS_SETPOINTLIGHT;
			break;
		case OBJECT_SPOTLIGHT:
			type = SETTINGS_SETSPOTLIGHT;
			break;
		case OBJECT_DIRECTIONALLIGHT:
			type = SETTINGS_SETDIRECTIONALLIGHT;
			break;
		case OBJECT_MESH:
			type = SETTINGS_SETMESH;
			break;
		case OBJECT_CYLINDER:
			type = SETTINGS_SETCYLINDER;
			break;
		case OBJECT_CONE:
			type = SETTINGS_SETCONE;
			break;
		case OBJECT_CSG:
			type = SETTINGS_SETCSG;
			break;
		case OBJECT_SOR:
			type = SETTINGS_SETSOR;
			break;
		default:
			type = SETTINGS_SETNONE;
			break;
	}

	// create a settings window
	object->win = (Object*)NewObject(CL_Settings->mcc_Class,NULL,
		MUIA_ChildWindow_Originator,obj,
		MUIA_Settings_Type, type,
		MUIA_Settings_Object, object,
		MUIA_Settings_DrawArea, obj,
		MUIA_Settings_Display, data->display,
		TAG_DONE);

	if(object->win)
	{
		// increase the window counter of all selected objects
		IncOpenCounter(object->win);
		object->opencnt = 0;

		object->GetObjectMatrix(&m);
		m.GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);
		InvOrient(&orient_x, &orient_y, &orient_z, &orient_x, &orient_y, &orient_z);
		SetAttrs(object->win, MUIA_Settings_Pos, &pos, TAG_DONE);
		Orient2Angle(&align, &orient_x, &orient_y, &orient_z);
		SetAttrs(object->win, MUIA_Settings_Align, &align, TAG_DONE);
		SetAttrs(object->win, MUIA_Settings_Size, &object->size, TAG_DONE);
		SetAttrs(object->win, MUIA_Settings_Name, object->GetName(), TAG_DONE);
		SetAttrs(object->win, MUIA_Settings_Undo, undo, TAG_DONE);

		DOMETHOD(app,OM_ADDMEMBER,object->win);
		SetAttrs(object->win,MUIA_Window_Open,TRUE, TAG_DONE);
	}
	return 0;
}

/*************
 * DESCRIPTION:   settings window will finish
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG Settings_Finish(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish *msg)
{
	struct Draw_Data *data = (struct Draw_Data*)INST_DATA(cl,obj);
	DISPLAY *display = data->display;
	OBJECT *object;
	char *s;
	VECTOR newpos, newsize, newalign;
	UNDO_TRANSFORM *undo;
	ULONG poslocal, alignlocal, sizelocal;

	SetAttrs(msg->win, MUIA_Window_Open, FALSE, TAG_DONE);

	GetAttr(MUIA_Settings_Object, msg->win, (ULONG*)&object);
	if(msg->ok)
	{
		SetProjectChanged(TRUE);

		GetAttr(MUIA_Settings_Pos, msg->win, (ULONG*)&newpos);
		GetAttr(MUIA_Settings_PosLocal, msg->win, (ULONG*)&poslocal);
		GetAttr(MUIA_Settings_Size, msg->win, (ULONG*)&newsize);
		GetAttr(MUIA_Settings_SizeLocal, msg->win, (ULONG*)&sizelocal);
		GetAttr(MUIA_Settings_Align, msg->win, (ULONG*)&newalign);
		GetAttr(MUIA_Settings_AlignLocal, msg->win, (ULONG*)&alignlocal);
		GetAttr(MUIA_Settings_Undo, msg->win, (ULONG*)&undo);
		undo->Add();
		sciSetActiveObjects(undo, object, &newpos, poslocal, &newsize, sizelocal, &newalign, alignlocal);

		// change name
		GetAttr(MUIA_Settings_Name, msg->win, (ULONG*)&s);
		object->SetName(s);
		sciBrowserUpdate(object);
		undo->ResetOpenCount();

		if((object->GetType() == OBJECT_CAMERA) && (display->view->viewmode == VIEW_CAMERA))
		{
			display->view->SetCamera(display->camera);
			MUI_Redraw(obj,MADF_DRAWUPDATE);
		}
		else
		{
			Redraw(data, REDRAW_SELECTED, FALSE);
		}
	}
	else
	{
		GetAttr(MUIA_Settings_Undo, msg->win, (ULONG*)&undo);
		undo->ResetOpenCount();
		delete undo;
	}

	DOMETHOD(app, OM_REMMEMBER, msg->win);
	MUI_DisposeObject(msg->win);

	return 0;
}

/*************
 * DESCRIPTION:   set window will finish
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG SetFinish(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish *msg)
{
	struct Draw_Data *data = (struct Draw_Data*)INST_DATA(cl,obj);
	DISPLAY *display = data->display;
	Object *win;

	if(msg->ok)
		SetProjectChanged(TRUE);

	win = msg->win;
	SetAttrs(win,MUIA_Window_Open,FALSE, TAG_DONE);
	DOMETHOD(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);

	if(display->view->viewmode == VIEW_CAMERA)
	{
		display->view->SetCamera(display->camera);
		MUI_Redraw(obj,MADF_DRAWUPDATE);
	}
	else
	{
		Redraw(data, REDRAW_SELECTED, FALSE);
	}

	SetSleep(FALSE);

	return 0;
}

/*************
 * DESCRIPTION:   child window will finish
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG ChildWindow_Finish(struct IClass *cl,Object *obj,struct MUIP_ChildWindow_Finish *msg)
{
	Object *win;

	win = msg->win;
	SetAttrs(win,MUIA_Window_Open,FALSE, TAG_DONE);
	DOMETHOD(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);
	SetSleep(FALSE);

	if(msg->ok)
		SetProjectChanged(TRUE);

	return 0;
}

/*************
 * DESCRIPTION:   Called when MUI is about to build a popup menu. Look if on the specified
 *    position is an object, and build the right menu.
 * INPUT:         system
 * OUTPUT:        0
 *************/
static ULONG ContextMenuBuild(struct IClass *cl,Object *obj,struct MUIP_ContextMenuBuild *msg)
{
	struct Draw_Data *data = (struct Draw_Data*)INST_DATA(cl,obj);
	int flags;

	flags = BUTTON_RIGHTDOWN;
	HandleMouseButtons(&mouse_data, msg->mx-_mleft(obj), msg->my-_mtop(obj), flags, &breaked);

	if(!(data->display->editmode & EDIT_ACTIVE))
	{
		if(data->contextmenu)
			MUI_DisposeObject(data->contextmenu);

		data->contextmenu = MUI_MakeObject(MUIO_MenustripNM, mouse_data.popup_menu, 0);
		return (ULONG)data->contextmenu;
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
	struct Draw_Data *data = (struct Draw_Data*)INST_DATA(cl,obj);
	DISPLAY *display = data->display;
	OBJECT *active;

	switch(muiUserData(msg->item))
	{
		case POP_NONE:
			break;
		case POP_ATTRIBUTE:
			DOMETHOD(data->win,MUIM_MainWindow_Attributes);
			break;
		case POP_CAMERA_ACTIVE:
			GetActiveObjects(&active, FALSE);
			if(active && (active->GetType() == OBJECT_CAMERA))
			{
				display->camera = (CAMERA*)active;
				display->view->SetCamera(display->camera);
				SetAttrs(data->win,MUIA_MainWindow_ActiveCamera, active->GetName(), TAG_DONE);
				if(display->view->viewmode == VIEW_CAMERA)
					MUI_Redraw(obj,MADF_DRAWUPDATE);
			}
			break;
		case POP_SETTINGS:
			DOMETHOD(obj,MUIM_Draw_OpenSettings);
			break;
		case POP_OBJECT_MOVE:
			display->editmode = EDIT_MOVE | EDIT_OBJECT;
			SetActiveGadget(data->win, display->editmode);
			break;
		case POP_OBJECT_ROTATE:
			display->editmode = EDIT_ROTATE | EDIT_OBJECT;
			SetActiveGadget(data->win, display->editmode);
			break;
		case POP_OBJECT_SCALE:
			display->editmode = EDIT_SCALE | EDIT_OBJECT;
			SetActiveGadget(data->win, display->editmode);
			break;
		case POP_WORLD_MOVE:
			display->editmode = EDIT_MOVE;
			SetActiveGadget(data->win, display->editmode);
			break;
		case POP_WORLD_ROTATE:
			display->editmode = EDIT_ROTATE;
			SetActiveGadget(data->win, display->editmode);
			break;
		case POP_WORLD_SCALE:
			display->editmode = EDIT_SCALE;
			SetActiveGadget(data->win, display->editmode);
			break;
		case POP_WORLD_SETTINGS:
			DOMETHOD(data->win,MUIM_MainWindow_Global);
			break;
		case POP_WORLD_RENDER:
			DOMETHOD(data->win,MUIM_MainWindow_Render);
			break;
		case POP_OBJECT2SCENE:
			SetSleep(TRUE);
			sciDoMethod(SCIM_OBJECT2SCENE);
			SetSleep(FALSE);
			break;
		case POP_SPHERE:
			sciDoMethod(SCIM_CREATESPHERE);
			break;
		case POP_BOX:
			sciDoMethod(SCIM_CREATEBOX);
			break;
		case POP_CYLINDER:
			sciDoMethod(SCIM_CREATECYLINDER);
			break;
		case POP_CONE:
			sciDoMethod(SCIM_CREATECONE);
			break;
		case POP_SOR:
			sciDoMethod(SCIM_CREATESOR);
			break;
		case POP_PLANE:
			sciDoMethod(SCIM_CREATEPLANE);
			break;
		case POP_POINTLIGHT:
			sciDoMethod(SCIM_CREATEPOINTLIGHT);
			break;
		case POP_SPOTLIGHT:
			sciDoMethod(SCIM_CREATESPOTLIGHT);
			break;
		case POP_DIRECTIONALLIGHT:
			sciDoMethod(SCIM_CREATEDIRECTIONALLIGHT);
			break;
		case POP_CAMERA:
			sciDoMethod(SCIM_CREATECAMERA);
			break;
		case POP_CSG:
			sciDoMethod(SCIM_CREATECSG);
			break;
		case POP_AXIS:
			sciDoMethod(SCIM_CREATEAXIS);
			break;
		case POP_MESHSPHERE:
			DOMETHOD(data->win, MUIM_MainWindow_MeshSphere);
			break;
		case POP_MESHCUBE:
			DOMETHOD(data->win, MUIM_MainWindow_MeshCube);
			break;
		case POP_MESHTUBE:
			DOMETHOD(data->win, MUIM_MainWindow_MeshTube);
			break;
		case POP_MESHCONE:
			DOMETHOD(data->win, MUIM_MainWindow_MeshCone);
			break;
		case POP_MESHPLANE:
			DOMETHOD(data->win, MUIM_MainWindow_MeshPlane);
			break;
		case POP_MESHTORUS:
			DOMETHOD(data->win, MUIM_MainWindow_MeshTorus);
			break;
	}
	return 0;
}

/*************
 * DESCRIPTION:   Dispatcher for draw class. Unknown/unused methods are
 *    passed to the superclass immediately.
 * INPUT:         system
 * OUTPUT:        none
 *************/
SAVEDS ASM ULONG WARPOS_PPC_FUNC(Draw_Dispatcher)(REG(a0) struct IClass *cl,REG(a2) Object *obj,REG(a1) Msg msg)
{
	struct Draw_Data *data = (struct Draw_Data*)INST_DATA(cl,obj);

	switch(msg->MethodID)
	{
		case OM_DISPOSE:
			return(OM_Dispose(cl,obj,msg));
		case OM_SET:
			return(mSet(cl,obj,(struct opSet*)msg));
		case OM_GET:
			return(mGet(cl,obj,(struct opGet*)msg));
		case MUIM_AskMinMax:
			return(AskMinMax(cl,obj,(struct MUIP_AskMinMax*)msg));
		case MUIM_Draw:
			return(Draw(cl,obj,(struct MUIP_Draw*)msg));
		case MUIM_HandleInput:
			return(HandleInput(cl,obj,(struct MUIP_HandleInput*)msg));
		case MUIM_Setup:
			return(Setup(cl,obj,(struct MUIP_HandleInput*)msg));
		case MUIM_Cleanup:
			return(Cleanup(cl,obj,(struct MUIP_HandleInput*)msg));
		case MUIM_Hide:
			gfx.Cleanup();
#ifdef __OPENGL__
			data->display->views[0]->opengl.DestroyContext();
			data->display->views[1]->opengl.DestroyContext();
			data->display->views[2]->opengl.DestroyContext();
			data->display->views[3]->opengl.DestroyContext();
#endif // __OPENGL__
			return 0;
		case MUIM_ChildWindow_Finish:
			return(ChildWindow_Finish(cl,obj,(struct MUIP_ChildWindow_Finish *)msg));
		case MUIM_Settings_Finish:
			return(Settings_Finish(cl,obj,(struct MUIP_ChildWindow_Finish *)msg));
		case MUIM_Draw_SetFinish:
			return(SetFinish(cl,obj,(struct MUIP_ChildWindow_Finish*)msg));
		case MUIM_ContextMenuBuild:
			return(ContextMenuBuild(cl,obj,(struct MUIP_ContextMenuBuild*)msg));
		case MUIM_ContextMenuChoice:
			return(ContextMenuChoice(cl,obj,(struct MUIP_ContextMenuChoice*)msg));
		case MUIM_Draw_RedrawSelected:
			Redraw(data, REDRAW_SELECTED, FALSE);
			return 0;
		case MUIM_Draw_RedrawCurrentView:
			Redraw(data, REDRAW_CURRENT_VIEW, FALSE);
			return 0;
		case MUIM_Draw_RedrawDirty:
			Redraw(data, REDRAW_DIRTY, FALSE);
			return 0;
		case MUIM_Draw_RemoveSelected:
			Redraw(data, REDRAW_REMOVE_SELECTED, FALSE);
			return 0;
		case MUIM_Draw_OpenSettings:
			return(OpenSettings(cl,obj,msg));
	}

	return(DOSUPERMETHODA(cl,obj,msg));
}
