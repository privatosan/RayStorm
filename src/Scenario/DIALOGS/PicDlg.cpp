/***************
 * MODULE:			picdlg.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	browser dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						30.08.1996	mh		first release (0.1)
 *						07.09.1996	mh		CObjTree added
 *						16.09.1996	mh		objects are inserted by GetObjects
 ***************/

#include "typedefs.h"
#include "picdlg.h"

BEGIN_MESSAGE_MAP(CPicDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	start browser dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
CPicDlg::CPicDlg(CWnd* pParent)
    : CDialog(CPicDlg::IDD, pParent)
{
}

/*************
 * DESCRIPTION:	init dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CPicDlg::OnInitDialog()
{
	CRect rect;

	CDialog::OnInitDialog();

	GetWindowRect(rect);

	picture.Create(NULL, WS_CHILD|WS_VISIBLE, rect, this, 1);
	MoveWindow(500, 200, picture.nWidth + 8, picture.nHeight + 27);
	
	return TRUE;
}

/*************
 * DESCRIPTION:	init dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CPicDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (::IsWindow(picture.m_hWnd))
		picture.MoveWindow(0, 0, cx, cy);
}
