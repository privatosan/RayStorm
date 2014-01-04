/***************
 * NAME:				colbutt.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	color button control dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 *						18.09.1997	mh		color manipulator
 ***************/

#include "colbutt.h"
#include "afxdlgs.h"

BEGIN_MESSAGE_MAP(CColorButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
//	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_DESTROYME, OnDestroyManip)
END_MESSAGE_MAP()

CColorManip dlg;
BOOL bD = FALSE, bDestroyMe = FALSE, bEntered = FALSE;;

/*************
 * DESCRIPTION:	draw button
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CColorButton::Draw(BOOL bState)
{
	HBRUSH hOldBrush;
	HPEN hOldPen;
	COLORREF c;
	CRect rect;
	CDC *pDC;
	int nState;

	nState = DFCS_BUTTONPUSH | DFCS_ADJUSTRECT;
	if (bState)
		nState |= DFCS_PUSHED;

	GetClientRect(rect);

	pDC = GetDC();
	if (pDC)
	{
		pDC->DrawFrameControl(rect, DFC_BUTTON, nState);
		c = RGB(BYTE(col.r*255), BYTE(col.g*255), BYTE(col.b*255));
		hOldBrush = (HBRUSH)pDC->SelectObject(CreateSolidBrush(c));
		hOldPen = (HPEN)pDC->SelectObject(CreatePen(PS_SOLID, 1, c));
		pDC->Rectangle(rect.left, rect.top, rect.right, rect.bottom);
		pDC->SelectObject(hOldBrush);
		pDC->SelectObject(hOldPen);
		ReleaseDC(pDC);
	}
}

/*************
 * DESCRIPTION:	draw button on system request
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CColorButton::DrawItem(LPDRAWITEMSTRUCT)
{
	Draw(FALSE);
}

/*************
 * DESCRIPTION:	show color requester
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CColorButton::OnLButtonDown(UINT, CPoint)
{
	if (bD)
	{
		bD = FALSE;
		dlg.DestroyWindow();
		ReleaseCapture();
	}
	Draw(TRUE);
}

/*************
 * DESCRIPTION:	show color requester
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CColorButton::OnLButtonUp(UINT, CPoint)
{
	CColorDialog dialog(RGB(BYTE(col.r*255), BYTE(col.g*255), BYTE(col.b*255)), CC_FULLOPEN);
	COLORREF res;

	Draw(FALSE);

	if (dialog.DoModal() == IDOK)
	{
		res = dialog.GetColor();
		col.r = float((BYTE)(res)) / 255;
		col.g = float((BYTE)(res >> 8)) / 255;
		col.b = float((BYTE)(res >> 16)) / 255;
		Invalidate(FALSE);
		GetParent()->SendMessage(WM_COLORCHANGED, 0, 0L);
	}
}

/*************
 * DESCRIPTION:	handle mouse movement over color button (show color manipulation box)
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CColorButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rect;

	dlg.pCol = &col;

	GetClientRect(rect);
	
	if (!rect.PtInRect(point))
	{
		dlg.DestroyWindow();
		ReleaseCapture();
		bD = FALSE;
	}
	else
	{
		if (!bD)
		{
			bD = TRUE;
			SetCapture();
			dlg.pParent = this;
			dlg.Create(IDD_COLORMANIP, this);
		}
	}


	/*
	if (bD && !bEntered)
	{
	bEntered = FALSE;
		if (point.y > rect.bottom)
		{
			ReleaseCapture();
			dlg.SetCapture();
			bEntered = TRUE;
		}
		return;
	}*/
}

LRESULT CColorButton::OnDestroyManip(WPARAM, LPARAM)
{
	dlg.DestroyWindow();
	bD = FALSE;

	return 0;
}

// Color manipulator
BEGIN_MESSAGE_MAP(CColorManip, CDialog)
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CColorManip::CColorManip(CWnd* pPnt)
    : CDialog(CColorManip::IDD, pPnt)
{
	bExit = FALSE;
	pParent = pPnt;
}

/*************
 * DESCRIPTION:	center window in middle of screen
 * INPUT:			(system related)
 * OUTPUT:			-
 *************/
BOOL CColorManip::OnInitDialog()
{
	CRect rectParent, rectOwn;

	CDialog::OnInitDialog();

	pParent->GetWindowRect(rectParent);
	GetClientRect(rectOwn);
	MoveWindow(rectParent.left, rectParent.top + rectParent.Height(), rectOwn.Width(), rectOwn.Height());

	VERIFY(m_sldRed.SubclassDlgItem(IDC_COLMANIP_R, this));
	VERIFY(m_sldGreen.SubclassDlgItem(IDC_COLMANIP_G, this));
	VERIFY(m_sldBlue.SubclassDlgItem(IDC_COLMANIP_B, this));

	m_sldRed.SetRange(0,255);
	m_sldGreen.SetRange(0,255);
	m_sldBlue.SetRange(0,255);

	m_sldRed.SetPageSize(10);
	m_sldGreen.SetPageSize(10);
	m_sldBlue.SetPageSize(10);

	m_sldRed.SetPos(int(pCol->r*255));
	m_sldGreen.SetPos(int(pCol->g*255));
	m_sldBlue.SetPos(int(pCol->b*255));

	return TRUE;
}

/*************
 * DESCRIPTION:	process V-Scroll messages of the sliders
 * INPUT:			(system related)
 * OUTPUT:			-
 *************/
void CColorManip::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	float value;

	if (pScrollBar)
	{
		value = float(((CSliderCtrl*)pScrollBar)->GetPos())/255.0f;
		switch (pScrollBar->GetDlgCtrlID())
		{
			case IDC_COLMANIP_R : pCol->r = value; break;
			case IDC_COLMANIP_G : pCol->g = value; break;
			case IDC_COLMANIP_B : pCol->b = value; break;
		}
		pParent->Invalidate(FALSE);
		pParent->GetParent()->SendMessage(WM_COLORCHANGED, 0, 0L);
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/*************
 * DESCRIPTION:	handle mouse movement over color button (show color manipulation box)
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CColorManip::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rect;

	GetClientRect(rect);
	if (!rect.PtInRect(point))
	{
		ReleaseCapture();
		pParent->SendMessage(WM_DESTROYME, 0, 0L);
	}

	CDialog::OnMouseMove(nFlags, point);
}
