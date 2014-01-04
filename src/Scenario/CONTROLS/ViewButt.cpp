/***************
 * MODULE:			viewbutt.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	viewer button control dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 ***************/

#include "afxdlgs.h"
#include "utility.h"
#include "viewbutt.h"
#include "prefs.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CViewButton, CButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor, load bitmaps
 * INPUT:			-
 * OUTPUT:			-
 *************/
CViewButton::CViewButton()
{
}

/*************
 * DESCRIPTION:	initialize button data and load bitmap
 * INPUT:			nIDEdit		resource ID of edit control
 *						nIDButton	resource ID of button
 *						pWnd			pointer to parent window
 * OUTPUT:			-
 *************/
BOOL CViewButton::Init(UINT nIDEdit, UINT nIDButton, CWnd *pWnd)
{
	if (!SubclassDlgItem(nIDButton, pWnd))
		return FALSE;

	nEditID = nIDEdit;

	if (!m_bmpView.LoadBitmap(IDB_BITMAP_VIEW))
		return FALSE;
	
	SetBitmap((HBITMAP)m_bmpView.GetSafeHandle());
	
	return TRUE;
}

/*************
 * DESCRIPTION:	constructor, load bitmaps
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CViewButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	char szBuf1[256], szBuf2[256];
	PREFS prefs;

	prefs.id = ID_VIEW;
	if (prefs.GetPrefs() && ((char *)prefs.data)[0])
	{
		GetParent()->GetDlgItemText(nEditID, szBuf2, 256);
		if (szBuf2[0])
		{
			sprintf(szBuf1, "%s %s", (char *)prefs.data, szBuf2); 
			if (WinExec(szBuf1, SW_SHOW) < 32)
			{
				utility.Request(IDS_ERRSTARTAPP);
			}
		}
		else
			utility.Request(IDS_ERRNOFILE);
	}
	else
		utility.Request(IDS_ERRNOVIEWER);

	prefs.data = NULL; // it's a MUST!

	CButton::OnLButtonUp(nFlags, point);
}
