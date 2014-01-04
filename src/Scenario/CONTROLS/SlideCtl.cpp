/***************
 * MODULE:			slidectl.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	Slider control
 *						This control is similar to an edit control with spin buttons
 *						except that instead of a spin button a scrollbar is used.
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						26.11.1996	mh		first version
 ***************/

#include "SlideCtl.h"

BEGIN_MESSAGE_MAP(CEditSlider, CScrollBar)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CEditSlider::CEditSlider()
{
}

/*************
 * DESCRIPTION:	set scrollbar and edit control
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CEditSlider::Init(CWnd *pWnd, UINT nID, UINT nButtonID, UINT nMin, UINT nMax)
{
	if (!SubclassDlgItem(nButtonID, pWnd))
		return FALSE;

	nEditID = nID;
	SetScrollRange(nMin, nMax);

	return TRUE;
}

/*************
 * DESCRIPTION:	set scrollbar and edit control
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CEditSlider::Set(UINT nSBCode, UINT nPos)
{
	int nCurPos, minrange, maxrange;
	
	nCurPos = GetScrollPos();

	GetScrollRange(&minrange, &maxrange);
	
	switch (nSBCode)
	{
		case SB_LEFT: //    Scroll to far left.
			nCurPos = minrange;
			break;
		case SB_LINELEFT: //    Scroll left.
			nCurPos--;
			break;
		case SB_LINERIGHT: //    Scroll right.
			nCurPos++;
			break;
		case SB_PAGELEFT: //    Scroll one page left.
			nCurPos -= 10;
			break;
		case SB_PAGERIGHT: //    Scroll one page right.
			nCurPos += 10;
			break;
		case SB_RIGHT: //    Scroll to far right.
			nCurPos = maxrange;
			break;
		case SB_THUMBPOSITION: //    Scroll to absolute position. The current position is specified by the nPos parameter.
		case SB_THUMBTRACK: //    Drag scroll box to specified position. The current position is specified by the nPos parameter.
			nCurPos = nPos;
	}
	if (nCurPos < minrange)
		nCurPos = minrange;
	if (nCurPos > maxrange)
		nCurPos = maxrange;
	
	SetScrollPos(nCurPos);

	GetParent()->SetDlgItemInt(nEditID, nCurPos);
}

/*************
 * DESCRIPTION:	sets value of slider (and set ScrollBar)
 * INPUT:			f	set slider to value f
 * OUTPUT:			-
 *************/
void CEditSlider::SetValue(float f)
{
	char szBuf[20];

	sprintf(szBuf, "%g", f);
	GetParent()->SetDlgItemText(nEditID, szBuf);
	SetScrollPos((int)f);
}

/*************
 * DESCRIPTION:	return values of slider (and set ScrollBar)
 * INPUT:			-
 * OUTPUT:			returns value (float)
 *************/
float CEditSlider::GetValue()
{
	char szBuf[20];
	float f;

	GetParent()->GetDlgItemText(nEditID, szBuf, 20);
	f = atof(szBuf);
	SetScrollPos((int)f);

	return f;
}