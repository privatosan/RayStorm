/***************
 * NAME:				colbutt.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	color button control
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						21.09.1996	mh		comment
 ***************/

#ifndef COLBUTT_H
#define COLBUTT_H

#include "typedefs.h"
#include "color.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"

#define WM_COLORCHANGED (WM_USER + 1)
#define WM_DESTROYME		(WM_USER + 2)

class CColorButton : public CButton
{
public:
	COLOR col;

protected:
	void Draw(BOOL);
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT, CPoint);
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg LRESULT OnDestroyManip(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

class CColorManip : public CDialog
{
	BOOL bExit;

public:
	CColorManip(CWnd *pParent = NULL); 
	enum { IDD = IDD_COLORMANIP };
	COLOR *pCol;
	BOOL Finished() { return bExit; }
	CWnd *pParent;

	CSliderCtrl m_sldRed, m_sldGreen, m_sldBlue;

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT, UINT, CScrollBar *);
	afx_msg void OnMouseMove(UINT, CPoint);

	DECLARE_MESSAGE_MAP()
};

#endif
