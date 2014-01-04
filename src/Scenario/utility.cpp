/***************
 * MODULE:			utility.cpp
 * PROJECT:			Scenario
 * DESCRIPTION:   definitions for utility functions which are different on all platforms
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.02.97 ah    initial release
 *		11.06.97 mh		bugfix: Request and AskRequest
 *		07.10.97	mh		bugfix: corrected va_list handling in Request and AskRequest
 *		09.10.97 mh		bugfix: corrected FileRequ, MBYESNO in AskRequest
 ***************/

#include <stdio.h>
#include <string.h>
#include "typedefs.h"
#include "prefs.h"
#include "resource.h"
#include "mainfrm.h"
#include "utility.h"
#include "CamView.h"

class UTILITY utility;

extern char szWorkingDirectory[256];

static FILEREQU_FILETYPE aFileTypes[] =
{
	{ NULL,	  NULL,	  "Select image file", "*.jpg", "JPEG Files (*.jpg)|*.jpg|TGA Files (*.tga)|*.tga|PNG Files (*.png)|*.png|IFF-ILBM Files (*.lbm)|*.lbm|All Files (*.*)|*.*||"},
	{ ID_BRSP, ID_BRSR, "Select brush file", "*.jpg", "JPEG Files (*.jpg)|*.jpg|TGA Files (*.tga)|*.tga|PNG Files (*.png)|*.png|IFF-ILBM Files (*.lbm)|*.lbm|All Files (*.*)|*.*||"},
	{ ID_OBJP, ID_OBJR, "Select object file", "*.rob", "RSOB Files (*.rob)|*.rob|3DS Files (*.3ds)|*.3ds|Imagine Object Files (*.iob)|*.iob|Lightwave object files (*.lwo)|*.lwo|RAW Files (*.raw)|*.raw|All Files (*.*)|*.*||"},
	{ ID_MATP, ID_MATR, "Select material file", "*.mat", "MAT Files (*.mat)|*.mat|All Files (*.*)|*.*||"},
	{ ID_TXTP, ID_TXTR, "Select texture file", "*.rtx", "RTX Files (*.rtx)|*.rtx|All Files (*.*)|*.*||"},
   { ID_TXTP, ID_TXTR, "Select texture file", "*.itx", "ITX Files (*.itx)|*.itx|All Files (*.*)|*.*||"},
   { ID_TXTP, ID_TXTR, "Select texture file", "*.htx", "HTX Files (*.htx)|*.htx|All Files (*.*)|*.*||"},
   { NULL,	  NULL,	  "Select a file", "*.*", "All Files (*.*)|*.*||"},
   { ID_PRJP, ID_PRJR, "Select a project", "*.scn", "Scene Files (*.scn)|*.scn|All Files (*.*)|*.*||"},
	{ ID_LIGP, ID_LIGR, "Select light file", "*.lig", "LIG Files (*.lig)|*.lig|All Files (*.*)|*.*||"},
};

/*************
 * DESCRIPTION:   initialize utility class
 * INPUT:         view		pointer to view
 * OUTPUT:        -
 *************/
void UTILITY::Init(CCamView *v)
{
	view = v;
}

/*************
 * DESCRIPTION:   open a requester
 * INPUT:         format   format string
 *                ...      parameters
 * OUTPUT:        -
 *************/
void UTILITY::Request(char *szFormat, ...)
{
	va_list marker;
	char szBuf[256];

	va_start(marker, szFormat);
	_vstprintf(szBuf, szFormat, marker);
	va_end(marker);

	MessageBox(NULL, szBuf, "Error", MB_OK | MB_ICONEXCLAMATION);
}

/*************
 * DESCRIPTION:   open a requester and displays a resource string
 * INPUT:         nID	ID of ressource string
 * OUTPUT:        -
 *************/
void UTILITY::Request(ULONG nID)
{
	AfxMessageBox(nID, MB_OK | MB_ICONEXCLAMATION);
}

/*************
 * DESCRIPTION:   open a yes|no requester
 * INPUT:         format   format string
 *                ...      parameters
 * OUTPUT:        TRUE -> ok; FALSE -> Cancel
 *************/
int UTILITY::AskRequest(char *szFormat, ...)
{
	va_list marker;
	char szBuf[256];

	va_start(marker, szFormat);
	_vstprintf(szBuf, szFormat, marker);
	va_end(marker);

	if (MessageBox(NULL, szBuf, "Request", MB_YESNO | MB_ICONQUESTION) == IDYES)
		return TRUE;
	else
		return FALSE;
}

/*************
 * DESCRIPTION:   open a file requester
 * INPUT:         file     buffer for file
 *						type		type of file (see FILEREQU_XXX constants)
 *                flags    flags
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL UTILITY::FileRequ(char *szFile, ULONG nType, ULONG nFlags)
{
	char szBuf[256], szInitFile[256];
	char *szDir;
	ULONG nPathID;
	PREFS prefs;
	BOOL bOpen;
	CString sFile;

	nPathID = aFileTypes[nType].pathID;

	szDir = NULL;
	strcpy(szInitFile, "");

	if (nFlags & FILEREQU_SAVE)
		bOpen = FALSE;
	else
		bOpen = TRUE;

	if (nFlags & FILEREQU_INITIALFILE)
	{
		if (szFile)
		{
			// take file for initial setting
			strcpy(szInitFile, FilePart(szFile));

			prefs.id = nPathID;
			if (prefs.GetPrefs())
				szDir = (char*)prefs.data;
		}
	}
	else
	{
		strcpy(szInitFile, aFileTypes[nType].filter);
		prefs.id = nPathID;
		if (prefs.GetPrefs())
			szDir = (char*)prefs.data;
	}
	
	if (nFlags & FILEREQU_INITIALFILEDIR)
	{
		if (szFile)
		{
			// take file for initial setting
			strcpy(szInitFile, FilePart(szFile));
			szDir = szFile;
			if (!szDir)
			{
				prefs.id = nPathID;
				if (prefs.GetPrefs())
					szDir = (char*)prefs.data;
			}
		}
	}
	if (szDir)
		SetCurrentDirectory(szDir);

	CFileDialog dialog(bOpen, aFileTypes[nType].filter, szInitFile,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT /*| OFN_NOCHANGEDIR */| 
					OFN_LONGNAMES | OFN_PATHMUSTEXIST, aFileTypes[nType].filterList);

	if (dialog.DoModal() == IDOK)
	{
		sFile = dialog.GetPathName();
		sFile.MakeLower();
		strcpy(szFile, sFile);
		prefs.id = nPathID;
		if (!(nFlags & FILEREQU_INITIALFILE))
		{
			DirPart(szBuf, (char*)LPCTSTR(sFile), 256);

			prefs.length = strlen(szBuf) + 1;
			prefs.data = (void*)szBuf;
			prefs.AddPrefs();
			prefs.data = NULL;
		}
		if (nFlags & FILEREQU_RELATIVEPATH)
		{
			if (!strnicmp(szFile, szWorkingDirectory, strlen(szWorkingDirectory)))
				memmove(szFile, szFile + strlen(szWorkingDirectory) + 1, strlen(szFile) - strlen(szWorkingDirectory));
		}
		prefs.data = NULL;
//		SetCurrentDirectory(szWorkingDirectory);
		return TRUE;
	}
	prefs.data = NULL;

//	SetCurrentDirectory(szWorkingDirectory);

	return FALSE;
}

/*************
 * DESCRIPTION:   enable/disable menu item
 * INPUT:         id			ID of menu item (not resource ID!)
 *						state		TRUE -> enabled, FALSE -> disabled
 * OUTPUT:        TRUE if ok else FALSE
 *************/
void UTILITY::EnableMenuItem(int nID, BOOL bState)
{
//	view->bMenuStates[id] = bState;
}

/*************
 * DESCRIPTION:   enable a toolbar item
 * INPUT:         id			toolbar item id
 *                enabled
 * OUTPUT:        -
 *************/
void UTILITY::EnableToolbarItem(int nID, BOOL bState)
{
//	view->bToolbarStates[id] = bState;
	return;
	CToolBar *pToolBar;
CMenu *menu;
	if (::IsWindow(view->m_hWnd))
	{
		//pToolBar = &((CMainFrame*)view->GetTopLevelFrame())->m_wndToolBar;
				menu = view->GetTopLevelFrame()->GetMenu();
		switch (nID)
		{
			case TOOLBAR_EDITMESH:
				menu->EnableMenuItem(IDM_ADDPOINTS, MF_GRAYED);
				break;
			case TOOLBAR_EDITSOR:
				//SetAttrs(mainwin, MUIA_MainWindow_EnableEditSOR, enabled, TAG_DONE);
				break;
		}
	}
}

/*************
 * DESCRIPTION:   check a menu item
 * INPUT:         id			menu item id
 *                checked
 * OUTPUT:        -
 *************/
void UTILITY::CheckMenuItem(int nID, BOOL checked)
{
/*	if(strip)
	{
		switch(id)
		{
			case MENU_RENDERWINDOW:
				SetAttrs((Object *)DoMethod(strip,MUIM_FindUData,MEN_RENDERWINDOW),MUIA_Menuitem_Checked, checked, TAG_DONE);
				break;
		}
	}*/
}

/*************
 * DESCRIPTION:   check a menu item
 * INPUT:         id			menu item id
 *                checked
 * OUTPUT:        -
 *************/
void UTILITY::CheckToolbarItem(int nID)
{
// todo
}
