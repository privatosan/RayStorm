/***************
 * NAME:				filebutt.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	file button control
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						21.09.1996	mh		comment
 ***************/

#ifndef FILEBUTT_H
#define FILEBUTT_H

#include "typedefs.h"
#include "afxwin.h"

class CFileButton : public CButton
{
	int	editid;
	UINT fileType;
	CBitmap m_bmpFile;

public:
	CFileButton();
	BOOL Init(UINT, UINT, UINT, CWnd *);

protected:
	afx_msg void OnLButtonUp(UINT, CPoint);

	DECLARE_MESSAGE_MAP()
};

class CFolderButton : public CButton
{
	int	editid;
	ULONG path_id;
	CBitmap m_bmpFolder;

public:
	CFolderButton();
	BOOL Init(UINT, UINT, CWnd *);
	
protected:
	static int CALLBACK SetSelProc(HWND, UINT, LPARAM, LPARAM);
	afx_msg void OnLButtonUp(UINT, CPoint);

	DECLARE_MESSAGE_MAP()
};

#endif
