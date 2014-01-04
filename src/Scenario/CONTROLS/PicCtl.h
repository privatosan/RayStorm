/***************
 * NAME:				picctl.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	picture control
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.12.1996	mh		first release
 ***************/

#ifndef PICCTL_H
#define PICCTL_H

#include "typedefs.h"
#include "rsi.h"
#include "afxwin.h"

class CPicture : public CStatic
{
public:
	CPicture();
	~CPicture();
	int nWidth, nHeight;

protected:
	CBitmap *pBitmap, *pOldBitmap;
	CDC *pMemDC;

public:
	void SetSize(int, int);
	void DrawLine(int, int, rsiSMALL_COLOR*);

protected:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT);

	DECLARE_MESSAGE_MAP()
};

#endif
