/***************
 * NAME:				bsebutt.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	browse button
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 ***************/

#ifndef BSEBUTT_H
#define BSEBUTT_H

#include "typedefs.h"
#include "afxwin.h"

class CBrowseButton : public CButton
{
	UINT nEditID;
	CBitmap m_bmpBrowse;

public:
	CBrowseButton::CBrowseButton();
	BOOL Init(CWnd *, UINT, UINT);
	
protected:
	afx_msg void OnLButtonUp(UINT, CPoint);

	DECLARE_MESSAGE_MAP()
};

#endif
