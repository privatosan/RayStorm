/***************
 * NAME:				bsebutt.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	browser button control dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 ***************/

#include "bsebutt.h"
#include "oseldlg.h"

BEGIN_MESSAGE_MAP(CBrowseButton, CButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor, load bitmaps
 * INPUT:			-
 * OUTPUT:			-
 *************/
CBrowseButton::CBrowseButton()
{
}

/*************
 * DESCRIPTION:	initialize button data and load bitmap
 * INPUT:			nIDEdit		resource ID of edit control
 *						nIDButton	resource ID of button
 *						pWnd			pointer to parent window
 * OUTPUT:			-
 *************/
BOOL CBrowseButton::Init(CWnd *pWnd, UINT nID, UINT nButtonID)
{
	if (!SubclassDlgItem(nButtonID, pWnd))
		return FALSE;

	nEditID = nID;

	if (!m_bmpBrowse.LoadBitmap(IDB_BITMAP_BROWSE))
		return FALSE;
	
	SetBitmap((HBITMAP)m_bmpBrowse.GetSafeHandle());
	
	return TRUE;
}

/*************
 * DESCRIPTION:	handles left mouse button
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CBrowseButton::OnLButtonUp(UINT, CPoint)
{
	CObjSelect dialog;
	char *szText;

	if (dialog.DoModal() == IDOK)
	{
		if (dialog.m_pSelectedObject)
		{
			szText = dialog.m_pSelectedObject->GetName();
			if (szText)
				GetParent()->SetDlgItemText(nEditID, szText);
		}
	}
}

