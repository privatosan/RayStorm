/***************
 * PROGRAM:       Modeler
 * NAME:          GFX.h
 * DESCRIPTION:   graphic commands
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		23.06.96 mh		initial release
 *		06.02.97	ah		added DIRECTLINE
 ***************/

#include "gfx.h"
#include "color.h"
#include "coldef.h"
#include "utility.h"
#include "resource.h"
	
GFX gfx;

static CPen *pn = NULL;

void GFX::Init(CWnd *pWnd)
{
	CDC *pDC;
	int xRes, yRes;

	pDC = pWnd->GetDC();
	if (!pDC)
		return;

	pScreenDC = new CDC;
	pScreenDC->Attach(pDC->m_hDC);

	pForeignDC = NULL;

	xRes = pDC->GetDeviceCaps(HORZRES);
	yRes = pDC->GetDeviceCaps(VERTRES);

	// allocate two double buffers
	pBitmap1 = new CBitmap;
	pBitmap2 = new CBitmap;
	if (!pBitmap1 || !pBitmap2)
		utility.Request(IDS_ERRNOMEM);

	pBitmap1->CreateCompatibleBitmap(pDC, xRes, yRes);
	pBitmap2->CreateCompatibleBitmap(pDC, xRes, yRes);

	pMem1DC = new CDC;
	pMem2DC = new CDC;
	if (!pMem1DC || !pMem2DC)
		utility.Request(IDS_ERRNOMEM);

	pMem1DC->CreateCompatibleDC(pDC);
	pMem2DC->CreateCompatibleDC(pDC);

	pOldBitmap1 = pMem1DC->SelectObject(pBitmap1);
	pOldBitmap2 = pMem2DC->SelectObject(pBitmap2);
	pMem1DC->SetViewportOrg(0, 0);
	pMem1DC->SetWindowOrg(0,0);
	pMem1DC->SetMapMode(MM_TEXT);
	pMem2DC->SetViewportOrg(0, 0);
	pMem2DC->SetWindowOrg(0,0);
	pMem2DC->SetMapMode(MM_TEXT);

	pWnd->ReleaseDC(pDC);
}

void GFX::Cleanup()
{
	pMem1DC->SelectObject(pOldBitmap1);
	pMem2DC->SelectObject(pOldBitmap2);

	if (pn)
	{
		delete pn;
		pn = NULL;
	}

	if (pBitmap1)
	{
		pBitmap1->DeleteObject();
		delete pBitmap1;
	}
	if (pBitmap2)
	{
		pBitmap2->DeleteObject();
		delete pBitmap2;
	}

	pMem1DC->DeleteDC();
	pMem2DC->DeleteDC();
	
	if (pMem1DC)
		delete pMem1DC;
	if (pMem2DC)
		delete pMem2DC;
	if (pScreenDC)
		delete pScreenDC;
}

void GFX::SetForeignDC(CDC *pDC)
{
	pForeignDC = pDC;
}

void GFX::Line(int x1, int y1, int x2, int y2)
{
	MoveToEx(pDC->m_hDC, x1, y1, NULL);
	LineTo(pDC->m_hDC, x2, y2);
}

void GFX::ClipLine(int x1, int y1, int x2, int y2)
{
	// prevent Windows95 from TOTAL crash
	if (abs(x1) + abs(y1) > 2000)
		return;
	else
		if (abs(x2) + abs(y2) > 2000)
			return;

	MoveToEx(pDC->m_hDC, x1, y1, NULL);
	LineTo(pDC->m_hDC, x2, y2);
}

void GFX::SetPen(int pen_)
{
	COLOR col;
	COLORREF colref;
	
	GetColorValue(pen_, &col);
	colref = RGB(int(col.r*255), int(col.g*255), int(col.b*255));
	pen = pen_;

	pn = new CPen(PS_SOLID, 0, colref);

	pDC->SelectObject(pn);
//	::SetPen(pDC, pen);

	pDC->SetTextColor(colref);
}


void GFX::SetColor(float r, float g, float b)
{
	COLORREF colref;

	if (pn)
		delete pn;

	colref = RGB(int(r*255),int(g*255),int(b*255));

	pn = new CPen(PS_SOLID, 0, colref);

	pDC->SelectObject(pn);
}

gfxMode GFX::SetWriteMode(gfxMode mode)
{
	gfxMode old;

	old = writemode;
	writemode = mode;

	switch (writemode)
	{
		case GFX_NORMAL		: pDC->SetBkMode(OPAQUE); break;
		case GFX_TRANSPARENT	: pDC->SetBkMode(TRANSPARENT); break;
	}
	return old;
}

CDC *GFX::GetDCFromMode(gfxOutput out)
{
	switch (out)
	{
		case GFX_SCREEN  : return pScreenDC;
		case GFX_MEMORY1 : return pMem1DC;
		case GFX_MEMORY2 : return pMem2DC;
		case GFX_FOREIGN : return pForeignDC;
		default			  : return pScreenDC;
	}
}

gfxOutput GFX::SetOutput(gfxOutput out)
{
	gfxOutput old;
	
	old = output;
	output = out;

	pDC = GetDCFromMode(out);

	return old;
}

void GFX::SetClipping(int l, int t, int r, int b)
{
	CRgn region;

	region.CreateRectRgn(l, t, r + 1, b + 1);
	pDC->SelectObject(&region);
}

void GFX::Rectangle(int l, int t, int r, int b)
{
	CRect rect(l, t, r+1, b+1);

	rect.NormalizeRect();

	if (writemode == GFX_COMPLEMENT ||
		 writemode == GFX_COMPLEMENT_DOTTED)
		pDC->DrawFocusRect(rect);
	else
	{
		MoveToEx(pDC->m_hDC, l, t, NULL);
		LineTo(pDC->m_hDC, r, t);
		LineTo(pDC->m_hDC, r, b);
		LineTo(pDC->m_hDC, l, b);
		LineTo(pDC->m_hDC, l, t);
	}
}

void GFX::FillRect(int l, int t, int r, int b)
{
	COLOR col;
	COLORREF colref;
	
	GetColorValue(pen, &col);
	colref = RGB(int(col.r*255), int(col.g*255), int(col.b*255));

	pDC->FillSolidRect(CRect(l, t, r+1, b+1), colref);
}

void GFX::BitBlt(gfxOutput src, int l, int t, int w, int h, gfxOutput dst, int x, int y)
{
	GetDCFromMode(dst)->BitBlt(l, t, w + 0, h + 0, GetDCFromMode(src), x, y, SRCCOPY);
}

void GFX::TextXY(int x, int y, char *s, int len)
{
	int oldMode;

	if (len == -1)
		len = strlen(s);

	oldMode = pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(x, y - GetTextExtentY(), s, len);
	pDC->SetBkMode(oldMode);
}

int GFX::GetTextExtentX(char *s)
{
	return pDC->GetTextExtent(s, strlen(s)).cx;
}

int GFX::GetTextExtentY()
{
	return pDC->GetTextExtent(" ", 1).cy;
}
