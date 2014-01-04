/***************
 * NAME:				viewbutt.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	view button
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						21.09.1996	mh		comment
 ***************/

#ifndef VIEWBUTT_H
#define VIEWBUTT_H

#include "typedefs.h"
#include "afxwin.h"

class CViewButton : public CButton
{
	UINT nEditID;
	CBitmap m_bmpView;

public:
	CViewButton::CViewButton();
	BOOL Init(UINT, UINT, CWnd *);
	
protected:
	afx_msg void OnLButtonUp(UINT, CPoint);

	DECLARE_MESSAGE_MAP()
};

#endif
