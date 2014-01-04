/***************
 * NAME:				slidectl.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	slider control
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						26.11.1996	mh		first release
 ***************/

#ifndef SLIDECTL_H
#define SLIDECTL_H

#include "typedefs.h"
#include "afxwin.h"

// add 'USES_EDITSLIDERS' to your afx_msg codes of your class definiton
#define USES_EDITSLIDERS \
afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)\
{\
	ASSERT(pScrollBar != NULL);\
	((CEditSlider*)pScrollBar)->Set(nSBCode, nPos);\
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);\
}\

class CEditSlider : public CScrollBar
{
private:
	UINT nEditID;

public:
	CEditSlider::CEditSlider();
	
	BOOL Init(CWnd *, UINT, UINT, UINT nMin = 0, UINT nMax = 100);
	void Set(UINT, UINT);
	void SetValue(float);
	float GetValue();

protected:
	DECLARE_MESSAGE_MAP()
};

#endif
