/***************
 * PROGRAM:       Modeler
 * NAME:          utility.cpp
 * DESCRIPTION:   definitions for utility functions which are different on all platforms
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.02.97 ah    initial release
 *    10.03.97 ah    added EnableMenuItem()
 *    14.10.97 ah    changed FileRequ() to preset
 ***************/

#include <stdio.h>
#include <string.h>

#include <pragma/asl_lib.h>
#include <pragma/dos_lib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

#ifndef MUIUTILITY_H
#include "MUIutility.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef MAINWINDOW_CLASS_H
#include "mainwindow_class.h"
#endif

class UTILITY utility;

static FILEREQU_FILETYPE fileTypes[] =
{
	{ NULL,    NULL,    "Select image file", "#?.(iff|png|jpg|tga)", NULL },
	{ ID_BRSP, ID_BRSR, "Select brush file", "#?.(iff|png|jpg|tga)", NULL },
	{ ID_OBJP, ID_OBJR, "Select object file", "#?.(rob|3ds|iob|raw|lwob|nff)", NULL },
	{ ID_MATP, ID_MATR, "Select material file", "#?.mat", NULL },
	{ ID_TXTP, ID_TXTR, "Select texture file", "#?.rtx", NULL },
	{ ID_TXTP, ID_TXTR, "Select texture file", "#?.itx", NULL },
	{ NULL,    NULL,    "Select a file", "", NULL },
	{ ID_PRJP, ID_PRJR, "Select a project", "#?.scn", NULL },
	{ ID_LIGP, ID_LIGR, "Select light file", "#?.lig", NULL }
};

/*************
 * DESCRIPTION:   initialize utility class
 * INPUT:         s        screen
 * OUTPUT:        -
 *************/
void UTILITY::InitScreen(struct Screen *s)
{
	screen = s;
}

/*************
 * DESCRIPTION:   initialize utility class
 * INPUT:         main     main window
 * OUTPUT:        -
 *************/
void UTILITY::InitMainWin(Object *main, Object *mstrip)
{
	strip = mstrip;
	mainwin = main;
}

/*************
 * DESCRIPTION:   initialize utility class
 * INPUT:         drawmain draw window
 * OUTPUT:        -
 *************/
void UTILITY::InitDrawWin(Object *draw)
{
	drawwin = draw;
}

/*************
 * DESCRIPTION:   open a requester
 * INPUT:         format   format string
 *                ...      parameters
 * OUTPUT:        -
 *************/
void UTILITY::Request(char *format, ...)
{
	MUI_RequestA(app,NULL,0,NULL,"*_Ok",format,(APTR)(((ULONG)&format)+4));
}

/*************
 * FUNCTION:      Request
 * DESCRIPTION:   open a requester and displays a resource string
 * INPUT:         nID   ID of ressource string
 * OUTPUT:        -
 *************/
void UTILITY::Request(ULONG nID)
{
// AfxMessageBox(nID, MB_OK | MB_ICONEXCLAMATION);
}

/*************
 * DESCRIPTION:   open a yes|no requester
 * INPUT:         format   format string
 *                ...      parameters
 * OUTPUT:        TRUE -> ok; FALSE -> Cancel
 *************/
int UTILITY::AskRequest(char *format, ...)
{
	return(MUI_RequestA(app,NULL,0,NULL,"*_Ok|_Cancel",format,(APTR)(((ULONG)&format)+4)));
}

/*************
 * DESCRIPTION:   open a file requester
 * INPUT:         file     buffer for file
 *                type     type of file (see FILEREQU_XXX constants)
 *                flags    flags
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL UTILITY::FileRequ(char *file, ULONG type, ULONG flags)
{
	struct FileRequester *filerequ;
	int rect[4];
	PREFS *p;
	BOOL success = FALSE;
	char *dir, *f, initfile[256], initdir[256], *s;

	p = new PREFS;
	if(!p)
		return FALSE;

	initfile[0] = 0;
	initdir[0] = 0;

	f = initfile;
	dir = initdir;

	if(flags & FILEREQU_INITIALFILE)
	{
		if(file)
		{
			// take file for initial setting
			strcpy(initfile, FilePart(file));

			p->id = fileTypes[type].pathID;
			if(p->GetPrefs())
				dir = (char*)p->data;
		}
	}
	else
	{
		p->id = fileTypes[type].pathID;
		if(p->GetPrefs())
		{
			strcpy(initdir, (char*)p->data);
			s = strchr(initdir, ';');
			if(s)
				*s = 0;
		}
	}

	if(flags & FILEREQU_INITIALFILEDIR)
	{
		if(file)
		{
			// take file for initial setting
			strcpy(initfile, FilePart(file));
			strcpy(initdir, file);
			f = PathPart(initdir);
			if(f != initdir)
				*f = 0;
			else
			{
				p->id = fileTypes[type].pathID;
				if(p->GetPrefs())
					dir = (char*)p->data;
			}
		}
	}

	p->id = fileTypes[type].rectID;
	if(!p->GetPrefs())
	{
		filerequ = (struct FileRequester *)
			AllocAslRequestTags(ASL_FileRequest,
			ASLFR_Screen,           screen,
			ASLFR_TitleText,        fileTypes[type].fileTitle,
			ASLFR_InitialDrawer,    dir,
			ASLFR_InitialFile,      f,
			ASLFR_InitialPattern,   fileTypes[type].filter,
			ASLFR_RejectIcons,      TRUE,
			ASLFR_DoPatterns,       TRUE,
			ASLFR_DrawersOnly,      (flags & FILEREQU_DIR) ? TRUE : FALSE,
			ASLFR_DoSaveMode,       (flags & FILEREQU_SAVE) ? TRUE : FALSE,
			TAG_DONE);
	}
	else
	{
		rect[0] = ((int*)(p->data))[0];
		rect[1] = ((int*)(p->data))[1];
		rect[2] = ((int*)(p->data))[2];
		rect[3] = ((int*)(p->data))[3];
		filerequ = (struct FileRequester *)
			AllocAslRequestTags(ASL_FileRequest,
			ASLFR_Screen,           screen,
			ASLFR_TitleText,        fileTypes[type].fileTitle,
			ASLFR_InitialDrawer,    dir,
			ASLFR_InitialFile,      f,
			ASLFR_InitialPattern,   fileTypes[type].filter,
			ASLFR_InitialLeftEdge,  rect[0],
			ASLFR_InitialTopEdge,   rect[1],
			ASLFR_InitialWidth,     rect[2],
			ASLFR_InitialHeight,    rect[3],
			ASLFR_RejectIcons,      TRUE,
			ASLFR_DoPatterns,       TRUE,
			ASLFR_DrawersOnly,      (flags & FILEREQU_DIR) ? TRUE : FALSE,
			ASLFR_DoSaveMode,       (flags & FILEREQU_SAVE) ? TRUE : FALSE,
			TAG_DONE);
	}

	if(filerequ)
	{
		SetSleep(TRUE);
		success = AslRequest(filerequ, NULL);
		if(success)
		{
			if(flags & FILEREQU_DIR)
				strcpy(file, filerequ->rf_Dir);
			else
			{
				strcpy(file, filerequ->rf_Dir);
				AddPart(file, filerequ->rf_File, 255);
			}

			if(!(flags & FILEREQU_INITIALFILEDIR))
			{
				p->id = fileTypes[type].pathID;
				if(flags & FILEREQU_DIR)
				{
					dir = PathPart(filerequ->rf_Dir);
					*dir = 0;
				}
				p->length = strlen(filerequ->rf_Dir)+1;
				p->data = filerequ->rf_Dir;
				p->AddPrefs();
			}
		}
		SetSleep(FALSE);
		rect[0] = filerequ->fr_LeftEdge;
		rect[1] = filerequ->fr_TopEdge;
		rect[2] = filerequ->fr_Width;
		rect[3] = filerequ->fr_Height;

		p->id = fileTypes[type].rectID;
		p->length = sizeof(int)*4;
		p->data = rect;
		p->AddPrefs();

		FreeAslRequest(filerequ);
	}
	p->data = NULL;
	delete p;

	return success;
}

/*************
 * DESCRIPTION:   enable a menu item
 * INPUT:         id       menu item id
 *                enabled
 * OUTPUT:        -
 *************/
void UTILITY::EnableMenuItem(int id, BOOL enabled)
{
	if(strip)
	{
		switch(id)
		{
			case MENU_UNDO:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_UNDO),MUIA_Menuitem_Enabled, enabled, TAG_DONE);
				break;
			case MENU_REDO:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_REDO),MUIA_Menuitem_Enabled, enabled, TAG_DONE);
				break;
			case MENU_CUT:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_CUT),MUIA_Menuitem_Enabled, enabled, TAG_DONE);
				break;
			case MENU_COPY:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_COPY),MUIA_Menuitem_Enabled, enabled, TAG_DONE);
				break;
			case MENU_PASTE:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_PASTE),MUIA_Menuitem_Enabled, enabled, TAG_DONE);
				break;
			case MENU_SETRENDERWINDOW:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_RENDERWINDOW),MUIA_Menuitem_Enabled, enabled, TAG_DONE);
				break;
		}
	}
}

/*************
 * DESCRIPTION:   check a menu item
 * INPUT:         id       menu item id
 *                checked
 * OUTPUT:        -
 *************/
void UTILITY::CheckMenuItem(int id, BOOL checked)
{
	if(strip)
	{
		switch(id)
		{
			case MENU_SETRENDERWINDOW:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_RENDERWINDOW),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWBRUSHES:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWBRUSHES),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWTEXTURES:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWTEXTURES),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWCAMERA:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWCAMERA),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWLIGHT:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWLIGHT),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWMESH:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWMESH),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWSPHERE:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWSPHERE),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWBOX:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWBOX),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWCYLINDER:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWCYLINDER),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWCONE:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWCONE),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWSOR:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWSOR),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWPLANE:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWPLANE),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_SHOWCSG:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_SHOWCSG),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_VIEWFRONT:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_VIEW_FRONT),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_VIEWRIGHT:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_VIEW_RIGHT),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_VIEWTOP:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_VIEW_TOP),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_VIEWPERSP:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_VIEW_PERSP),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_VIEWQUAD:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_VIEW_FOUR),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
			case MENU_VIEWCAMERA:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_VIEW_CAMERA),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
		}
	}
}

/*************
 * DESCRIPTION:   enable a toolbar item
 * INPUT:         id       toolbar item id
 *                enabled
 * OUTPUT:        -
 *************/
void UTILITY::EnableToolbarItem(int id, BOOL enabled)
{
	if(mainwin)
	{
		switch(id)
		{
			case TOOLBAR_EDITMESH:
				SetAttrs(mainwin, MUIA_MainWindow_EnableEditMesh, enabled, TAG_DONE);
				break;
			case TOOLBAR_EDITSOR:
				SetAttrs(mainwin, MUIA_MainWindow_EnableEditSOR, enabled, TAG_DONE);
				break;
		}
	}
}

/*************
 * DESCRIPTION:   check a toolbar item
 * INPUT:         id       toolbar item id
 * OUTPUT:        -
 *************/
void UTILITY::CheckToolbarItem(int id)
{
	if(mainwin)
	{
		switch(id)
		{
			case TOOLBAR_VIEWFRONT:
				SetAttrs(drawwin, MUIA_Draw_SetUpdate, FALSE, TAG_DONE);
				SetAttrs(mainwin, MUIA_MainWindow_Active_Gadget, GADGET_FRONT, TAG_DONE);
				SetAttrs(drawwin, MUIA_Draw_SetUpdate, TRUE, TAG_DONE);
				break;
			case TOOLBAR_VIEWRIGHT:
				SetAttrs(drawwin, MUIA_Draw_SetUpdate, FALSE, TAG_DONE);
				SetAttrs(mainwin, MUIA_MainWindow_Active_Gadget, GADGET_RIGHT, TAG_DONE);
				SetAttrs(drawwin, MUIA_Draw_SetUpdate, TRUE, TAG_DONE);
				break;
			case TOOLBAR_VIEWTOP:
				SetAttrs(drawwin, MUIA_Draw_SetUpdate, FALSE, TAG_DONE);
				SetAttrs(mainwin, MUIA_MainWindow_Active_Gadget, GADGET_TOP, TAG_DONE);
				SetAttrs(drawwin, MUIA_Draw_SetUpdate, TRUE, TAG_DONE);
				break;
			case TOOLBAR_VIEWPERSP:
				SetAttrs(drawwin, MUIA_Draw_SetUpdate, FALSE, TAG_DONE);
				SetAttrs(mainwin, MUIA_MainWindow_Active_Gadget, GADGET_PERSP, TAG_DONE);
				SetAttrs(drawwin, MUIA_Draw_SetUpdate, TRUE, TAG_DONE);
				break;
			case TOOLBAR_VIEWCAMERA:
				SetAttrs(drawwin, MUIA_Draw_SetUpdate, FALSE, TAG_DONE);
				SetAttrs(mainwin, MUIA_MainWindow_Active_Gadget, GADGET_CAMERA, TAG_DONE);
				SetAttrs(drawwin, MUIA_Draw_SetUpdate, TRUE, TAG_DONE);
				break;
			case TOOLBAR_VIEWFOUR:
				SetAttrs(mainwin, MUIA_MainWindow_Active_Gadget, GADGET_FOUR, TAG_DONE);
				break;
		}
	}
}

