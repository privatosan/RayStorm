/***************
 * NAME:				filebutt.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	Fileselector and Folderselector buttons
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 ***************/

#include "afxdlgs.h"
#include "filebutt.h"
#include "resource.h"
#include "fileutil.h"
#include "utility.h"
#include <shlobj.h>

extern char szWorkingDirectory[256];

BEGIN_MESSAGE_MAP(CFileButton, CButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor, load bitmaps
 * INPUT:			-
 * OUTPUT:			-
 *************/
CFileButton::CFileButton()
{
	fileType = FILEREQU_ALL;
}

/*************
 * DESCRIPTION:	initialize button data and load bitmap
 * INPUT:			nIDEdit		resource ID of edit control
 *						nIDButton	resource ID of button
 *						filter		filter string
 *						pWnd			pointer to parent window
 * OUTPUT:			-
 *************/
BOOL CFileButton::Init(UINT nIDEdit, UINT nIDButton, UINT fileType_, CWnd *pWnd)
{
	if (!SubclassDlgItem(nIDButton, pWnd))
		return FALSE;

	editid = nIDEdit;
	fileType = fileType_;
	
	if (!m_bmpFile.LoadBitmap(IDB_BITMAP_FILE))
		return FALSE;
	
	SetBitmap((HBITMAP)m_bmpFile.GetSafeHandle());
	
	return TRUE;
}

/*************
 * DESCRIPTION:	display file dialog when button pressed
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CFileButton::OnLButtonUp(UINT, CPoint)
{
	char szFile[256];

	if (utility.FileRequ(szFile, fileType, 0))
		GetParent()->SetDlgItemText(editid, szFile);
}


// Folder Button

BEGIN_MESSAGE_MAP(CFolderButton, CButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor, load bitmaps
 * INPUT:			-
 * OUTPUT:			-
 *************/
CFolderButton::CFolderButton()
{
	path_id = NULL;
}

int CALLBACK CFolderButton::SetSelProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  if (uMsg == BFFM_INITIALIZED)
  {
    ::SendMessage( hWnd, BFFM_SETSELECTION, TRUE, lpData );
  }

  return 0;
}

/*************
 * DESCRIPTION:	initialize button data and load bitmap
 * INPUT:			nIDEdit		resource ID of edit control
 *						nIDButton	resource ID of button
 *						filter		filter string
 *						pWnd			pointer to parent window
 * OUTPUT:			-
 *************/
BOOL CFolderButton::Init(UINT nIDEdit, UINT nIDButton, CWnd *pWnd)
{
	if (!SubclassDlgItem(nIDButton, pWnd))
		return FALSE;

	editid = nIDEdit;

	if (!m_bmpFolder.LoadBitmap(IDB_BITMAP_FOLDER))
		return FALSE;
	
	SetBitmap((HBITMAP)m_bmpFolder.GetSafeHandle());
	
	return TRUE;
}

/*************
 * DESCRIPTION:	display file dialog when button pressed
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CFolderButton::OnLButtonUp(UINT, CPoint)
{
  //bring up the common file open dialog to select the folder

  BYTE WinMajorVersion = LOBYTE(LOWORD(GetVersion()));
  if (WinMajorVersion >= 4) //Running on Windows 95 shell and new style requested
  {
    TCHAR sDisplayName[_MAX_PATH];
    BROWSEINFO bi;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;
    bi.lpszTitle =  "";
    bi.pszDisplayName = sDisplayName;
    bi.ulFlags = BIF_RETURNONLYFSDIRS;
    bi.lpfn = SetSelProc;
    bi.lParam = (LPARAM)szWorkingDirectory;
	  LPITEMIDLIST pItemIDList = SHBrowseForFolder(&bi);

    if (pItemIDList)
    {
      TCHAR sPath[_MAX_PATH];
      if (SHGetPathFromIDList(pItemIDList, sPath))
			GetParent()->SetDlgItemText(editid, LPCTSTR(sPath));

      //avoid memory leaks by deleting the PIDL
      //using the shells task allocator
      IMalloc *pMalloc;
      if (SHGetMalloc(&pMalloc) != NOERROR)
      {
        TRACE("Failed to get pointer to shells task allocator");
        return;
      }
      pMalloc->Free(pItemIDList);
      if (pMalloc)
        pMalloc->Release();
    }
  }
}
