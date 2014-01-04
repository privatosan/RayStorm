/***************
 * MODULE:			picctl.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	Picture control
 *						This control display pictures
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						26.11.1996	mh		first version
 ***************/

#include "picctl.h"
#include "utility.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CPicture, CStatic)
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CPicture::CPicture()
{
	nWidth = 0;
	nHeight = 0;
	pBitmap = NULL;
	pMemDC = NULL;
}

/*************
 * DESCRIPTION:	destructor, free bitmap and memdc
 * INPUT:			-
 * OUTPUT:			-
 *************/
CPicture::~CPicture()
{
	if (pMemDC)
	{
		pMemDC->SelectObject(pOldBitmap);
		if (pBitmap)
		{
			pBitmap->DeleteObject();
			delete pBitmap;
		}
		delete pMemDC;
	}
}

/*************
 * DESCRIPTION:	do some initialisations
 * INPUT:			-
 * OUTPUT:			-
 *************/
int CPicture::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect rect;
	CDC *pDC;

	lpCreateStruct->cx += 8;
	lpCreateStruct->cy += 27;

	if (!pBitmap)
	{
		pDC = GetDC();
		pBitmap = new CBitmap;
		if (!pBitmap)
			utility.Request(IDS_ERRNOMEM);

		pBitmap->CreateCompatibleBitmap(pDC, nWidth, nHeight);

		pMemDC = new CDC;
		if (!pMemDC)
			utility.Request(IDS_ERRNOMEM);
		pMemDC->CreateCompatibleDC(pDC);

		pOldBitmap = pMemDC->SelectObject(pBitmap);
		CRect rect(0, 0, nWidth, nHeight);
		pMemDC->FillRect(rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
		pMemDC->SetViewportOrg(0, 0);
		pMemDC->SetWindowOrg(0, 0);
		pMemDC->SetMapMode(MM_TEXT);
		ReleaseDC(pDC);
	}
	return 0;
}

/*************
 * DESCRIPTION:	handles ON_WM_PAINT message
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CPicture::OnPaint()
{
	CPaintDC DC(this);

	if (pMemDC)
		DC.BitBlt(0, 0, nWidth, nHeight, pMemDC, 0, 0, SRCCOPY);
}

/*************
 * DESCRIPTION:	set width and height of screen
 * INPUT:			w, h	width and height of screen
 * OUTPUT:			-
 *************/
void CPicture::SetSize(int w, int h)
{
	nWidth = w;
	nHeight = h;
}

/*************
 * DESCRIPTION:	draw line(s) into buffer and to screen
 * INPUT:			y		y coord.
 *						c		number of lines to draw
 *						line	pointer to line
 * OUTPUT:			-
 *************/
void CPicture::DrawLine(int y, int c, rsiSMALL_COLOR *pLine)
{
	COLORREF col;
	CDC *pDC;
	int x, i;

	if (!pMemDC)
		return;

	if (!pLine || !(::IsWindow(m_hWnd)))
		return;

	for (i = 0; i < c; i++)
	{
		for (x = 0; x < nWidth; x++)
		{
			col = RGB(pLine->r, pLine->g, pLine->b);
			pMemDC->SetPixelV(x, y, col);
			pLine++;
		}
		y++;
	}
	pDC = GetDC();
	pDC->BitBlt(0, y - c, nWidth, y, pMemDC, 0, y - c, SRCCOPY);
	ReleaseDC(pDC);
}
