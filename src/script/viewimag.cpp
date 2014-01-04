 /***************
 * NAME:				viewimag.cpp
 * PROJECT:			RayStorm
 * VERSION:			0.1 21.03.1996
 * DESCRIPTION:	quantizizes RGB 24-Bit picture and displays it
 * AUTHORS:			Mike Hesser, Ed McCreary
 * HISTORY:			DATE		NAME	COMMENT
 *						21.03.96	mh		first release (0.1)
 *						05.04.96	mh		quantization finally works!
 *						28.02.97 mh		bugfix with GlobalAlloc and some source corrections
 ***************/

#include "stdafx.h"
#include <windowsx.h>
#include "assert.h"
#include "resource.h"
#include "rgbvtab.h"
#include "viewimag.h"

#include "raystorm.h"

// ordered dither matrix
int ord_dith[4][4] =
{
	{-7,  1, -5,  3},
	{ 5, -3,  7, -1},
	{-4,  4, -6,  2},
	{ 8,  0,  6, -2}
};

// declare message map
BEGIN_MESSAGE_MAP(CViewImage, CDialog)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
END_MESSAGE_MAP()

// constructor
CViewImage::CViewImage(CWnd* pParent /* =NULL*/)
	: CDialog(CViewImage::IDD, pParent)
{
	ULONG i, len;

	pView = pParent;
	hPalette = NULL;
	lpDIB = NULL;
	nDither = IDX_NONE;
	bStatus = FALSE;
	bStretched = FALSE;
	lpBitmap = NULL;
 
	lpBuffer = (QUANT_BUFFER*)malloc(sizeof(QUANT_BUFFER));
  
	// lut for x^2
	for (i = 0; i < QUANTCOLORS; i++)
		lpBuffer->SQR[i] = (long)i*(long)i;
 
	len = (long)COLOR_MAX*(long)COLOR_MAX*(long)COLOR_MAX;
 
	// buffer to store every color in image
	lpBuffer->lpHisto = (NODE **)malloc(len*sizeof(NODE *));
	if (lpBuffer->lpHisto)
		memset(lpBuffer->lpHisto, 0, len*sizeof(NODE *));
}

// destructor
CViewImage::~CViewImage()
{
	if (lpDIB)
		GlobalFree(lpDIB);

	if (hPalette)
		DeleteObject(hPalette);

	if (lpBuffer)
	{
		if (lpBuffer->lpHisto)
			free(lpBuffer->lpHisto);
		free(lpBuffer);
	}
}

void CViewImage::SetParams(int d, BOOL s, BOOL st)
{
	nDither = d;
	bStretched = s;
	bStatus = st;
}

void CViewImage::OnPaint()
{
	CPaintDC DC(this);
	HPALETTE hOldPal;
	BITMAPINFOHEADER *pDIB;
	UBYTE *pBits;

	if (!lpDIB)
		return;

	pDIB = (BITMAPINFOHEADER *)GlobalLock(lpDIB);
	pBits = (UBYTE *)pDIB + *(LPDWORD)pDIB + QUANTCOLORS*sizeof(RGBQUAD);

	hOldPal = SelectPalette(DC.m_hDC, hPalette, FALSE);
	
	DC.RealizePalette();

	if (bStretched)
	{
	}
	else
	{
		SetDIBitsToDevice(DC.m_hDC, 0,0, xRes, yRes, 0,0, 0,yRes, pBits, (BITMAPINFO *)pDIB, DIB_RGB_COLORS);
	}
	GlobalUnlock(lpDIB);
	
	if (hOldPal)
		SelectPalette(DC.m_hDC, hOldPal, FALSE);
}

void CViewImage::Update(UBYTE *pPic, int w, int h)
{
	RECT rect;

	assert(pPic != NULL);
	
	lpBitmap = pPic;
	xRes = w;
	yRes = h;
	
	UpdateStatus("Wait please");

	Quantize();
		
	UpdateStatus("Picture");
	GetWindowRect(&rect);
	rect.right = rect.left + xRes + 8;
	rect.bottom = rect.top + yRes + 27;
	MoveWindow(&rect, TRUE);
	UpdateWindow();
	ShowWindow(SW_SHOW);
	InvalidateRect(NULL, FALSE);
}	

void CViewImage::OnClose()
{
	((CPadView*)pView)->bViewerVisible = FALSE;
	ShowWindow(SW_HIDE);	

	CDialog::OnClose();
}

void CViewImage::UpdateStatus(char *text)
{
	SetWindowText(text);
}

// main procedures, quantize 24-bit Bitmap
void CViewImage::Quantize()
{
	UBYTE	*lpLine;
	long	i, j, len;
	int	r, g, b;
	long	dib_width;
	char	buffer[80];
 
	assert(lpBitmap != NULL);
	
	// store handle to status indicator callback
	dib_width = WIDTHBYTES(32*xRes);

	for (j = 0; j < yRes; j++)
	{
		lpLine = lpBitmap + dib_width*j;
		if (bStatus)
		{
			wsprintf(buffer, "Finding all colors, %d%%", (int)(100.0*((float)j/(float)yRes)));
			UpdateStatus(buffer);
		}
		for (i = 0; i < xRes; i++)
		{
			r = (int)*lpLine++;    
			g = (int)*lpLine++;    
			b = (int)*lpLine++;
			*lpLine++;	// skip oversample byte
			Addcolor(r >> 3, g >> 3, b >> 3, 1L);
		}  
	} 
	// call status if needed
	if (bStatus)
		UpdateStatus("Building new palette...");
 
	Mediancut();
 	Dither();

	// delete history
	len = (long)COLOR_MAX*(long)COLOR_MAX*(long)COLOR_MAX;
 
	if (lpBuffer->lpHisto)
	{
		for (i = 0; i < len; i++)
		{
			if (lpBuffer->lpHisto[i] != NULL)
			{
				free(lpBuffer->lpHisto[i]);
				lpBuffer->lpHisto[i] = NULL;
			}
		}
	}
}

// dither image
void CViewImage::Dither()
{
	LPSTR	lpDestData;
	long	src_dib_width,dest_dib_width;
	UBYTE	*lpSrcLine;
	UBYTE	*lpDestLine;
	long	i,j;
	char	buffer[80];
	int	r,g,b;
 
	AllocDIB();

	if (!lpDIB)
		return; 
 
	lpDestData = (lpDIB + *(LPDWORD)lpDIB + QUANTCOLORS * sizeof(RGBQUAD));

	src_dib_width = WIDTHBYTES(32*xRes);
	dest_dib_width = WIDTHBYTES(8*xRes); 
  
	for (j = 0; j < yRes; j++)
	{
		lpSrcLine = (UBYTE *)lpBitmap + src_dib_width*j;
		lpDestLine = (UBYTE *)lpDestData + dest_dib_width*(yRes - j - 1);
    
		if (bStatus)
		{
			wsprintf(buffer,"Building new Image, %d%%", (int)(100.0*((float)j/(float)yRes)));
			UpdateStatus(buffer);
		} 
    
		for (i = 0; i < xRes; i++)
		{    
			r = (int)*lpSrcLine++;     
			g = (int)*lpSrcLine++;     
			b = (int)*lpSrcLine++;     
			*lpSrcLine++;
     
			if (nDither == IDX_JITTER)
				Jitter(i,j, &r, &g, &b);
     
			if (nDither == IDX_ORDERED)
			{ 
				r += 2*ord_dith[i%4][j%4]; r = CLIP(r); 
				g += 2*ord_dith[i%4][j%4]; g = CLIP(g); 
				b += 2*ord_dith[i%4][j%4]; b = CLIP(b); 
			}
          
			*lpDestLine++ = (UBYTE)GetNeighbor(r, g, b);
		}  
	}   
   
	if (bStatus)
	{
		wsprintf(buffer, "Building new Image, 100%%");
		UpdateStatus(buffer);
	} 
}


// build new palette with median cut algorithm
// I didn't write this, if you know who did, please let me know!
void CViewImage::Mediancut()
{
	int	i, j, max, dr, dg, db;    
	char buffer[80];
	// Force the counts in the corners to be zero

	Force( 0,  0,  0, 0L);
	Force(COLOR_MAX-1,  0,  0, 0L);
	Force( 0, COLOR_MAX-1,  0, 0L);
	Force( 0,  0, COLOR_MAX-1, 0L);
	Force(COLOR_MAX-1, COLOR_MAX-1,  0, 0L);
	Force( 0, COLOR_MAX-1, COLOR_MAX-1, 0L);
	Force(COLOR_MAX-1,  0, COLOR_MAX-1, 0L);
	Force(COLOR_MAX-1, COLOR_MAX-1, COLOR_MAX-1, 0L);

	// assign the 1st eight boxes to be the corners
	Makebox( 0,  0,  0, 0, 1L);
	Makebox(COLOR_MAX-1,  0,  0, 1, 1L);
	Makebox( 0, COLOR_MAX-1,  0, 2, 1L);
	Makebox( 0,  0, COLOR_MAX-1, 3, 1L);
	Makebox(COLOR_MAX-1, COLOR_MAX-1,  0, 4, 1L);
	Makebox( 0, COLOR_MAX-1, COLOR_MAX-1, 5, 1L);
	Makebox(COLOR_MAX-1,  0, COLOR_MAX-1, 6, 1L);
	Makebox(COLOR_MAX-1, COLOR_MAX-1, COLOR_MAX-1, 7, 1L);

	// set up 9th box to hold the rest of the world
	lpBuffer->box[8].r0 = 0;
	lpBuffer->box[8].r1 = COLOR_MAX-1;
	lpBuffer->box[8].g0 = 0;
	lpBuffer->box[8].g1 = COLOR_MAX-1;
	lpBuffer->box[8].b0 = 0;
	lpBuffer->box[8].b1 = COLOR_MAX-1;
	Squeeze(8);

	// split the rest of the boxes

	for (i = 9; i < QUANTCOLORS; i++)    
	{
		if (bStatus)
		{
			wsprintf(buffer, "splitting box %d", i);
			UpdateStatus(buffer);
		}     
     
		// find biggest box
		max = 8;
		for (j = 8; j < i; j++)
			if (lpBuffer->box[j].count > lpBuffer->box[max].count)
				max = j;

		// decide which side to split the box along, and split it

		dr = lpBuffer->box[max].r1 - lpBuffer->box[max].r0;
		dg = lpBuffer->box[max].g1 - lpBuffer->box[max].g0;
		db = lpBuffer->box[max].b1 - lpBuffer->box[max].b0;
		lpBuffer->box[i] = lpBuffer->box[max];              // copy info over

		if (dr >= dg && dr >= db) 
		{	// red!
			if (dr == 2)
			{	// tight Squeeze
				lpBuffer->box[i].r1 = lpBuffer->box[i].r0;
				lpBuffer->box[max].r0 = lpBuffer->box[max].r1;
			}
			else 
			{	// figure out where to split
				j = lpBuffer->box[max].rave;
				if(j == lpBuffer->box[max].r1)
					j--;
				lpBuffer->box[max].r1 = j;
				lpBuffer->box[i].r0 = j+1;
			}
			Squeeze(i);
			Squeeze(max);
		} 
		else if (dg >= db) 
		{	// green!
			if (dg == 2) 
			{	// tight Squeeze
				lpBuffer->box[i].g1 = lpBuffer->box[i].g0;
				lpBuffer->box[max].g0 = lpBuffer->box[max].g1;
			} 
			else 
			{	// figure out where to split
				j = lpBuffer->box[max].gave;
				if (j==lpBuffer->box[max].g1)
					j--;
				lpBuffer->box[max].g1 = j;
				lpBuffer->box[i].g0 = j+1;
			}
			Squeeze(i);
			Squeeze(max);
		} 
		else
		{	// blue!
			if (db == 2) 
			{	// tight Squeeze
				lpBuffer->box[i].b1 = lpBuffer->box[i].b0;
				lpBuffer->box[max].b0 = lpBuffer->box[max].b1;
			} 
			else 
			{	// figure out where to split
				j = lpBuffer->box[max].bave;
				if (j == lpBuffer->box[max].b1)
					j--;
				lpBuffer->box[max].b1 = j;
				lpBuffer->box[i].b0 = j+1;
			}
			Squeeze(i);
			Squeeze(max);
		}
	} // end of i loop, all the boxes are found

	// get palette colors for each box
	for (i = 0; i < QUANTCOLORS; i++) 
	{
		lpBuffer->red[i] = (lpBuffer->box[i].r0 + lpBuffer->box[i].r1)/2;
		lpBuffer->green[i] = (lpBuffer->box[i].g0 + lpBuffer->box[i].g1)/2;
		lpBuffer->blue[i] = (lpBuffer->box[i].b0 + lpBuffer->box[i].b1)/2;
	}
	for (i = 0; i < QUANTCOLORS; i++)
	{
		lpBuffer->red[i] *= 255;
		lpBuffer->red[i] /= (COLOR_MAX - 1);
		lpBuffer->green[i] *= 255;
		lpBuffer->green[i] /= (COLOR_MAX - 1);
		lpBuffer->blue[i] *= 255;
		lpBuffer->blue[i] /= (COLOR_MAX - 1);
	}
}


// make a 1x1x1 box at index with color rgb count c
void CViewImage::Makebox(int r, int g, int b, int index, ULONG c)
{
	lpBuffer->box[index].r0 = r;
	lpBuffer->box[index].r1 = r;
	lpBuffer->box[index].g0 = g;
	lpBuffer->box[index].g1 = g;
	lpBuffer->box[index].b0 = b;
	lpBuffer->box[index].b1 = b;
	lpBuffer->box[index].count = c;
}

// Squeeze -- shrink a boxes extremes to fit tightly
// if a box is 1x1x1 change its count to 1
void CViewImage::Squeeze(int b)
{
	int	r0, r1, g0, g1, b0, b1;
	long	i, j, k;
	ULONG	count = 0;
	NODE	*ptr;
	DWORD	index;

	r0 = lpBuffer->box[b].r0;
	r1 = lpBuffer->box[b].r1;
	g0 = lpBuffer->box[b].g0;
	g1 = lpBuffer->box[b].g1;
	b0 = lpBuffer->box[b].b0;
	b1 = lpBuffer->box[b].b1;

	lpBuffer->box[b].r0 = COLOR_MAX - 1; lpBuffer->box[b].r1 = 0;
	lpBuffer->box[b].g0 = COLOR_MAX - 1; lpBuffer->box[b].g1 = 0;
	lpBuffer->box[b].b0 = COLOR_MAX - 1; lpBuffer->box[b].b1 = 0;
	lpBuffer->box[b].rave = 0;
	lpBuffer->box[b].gave = 0;
	lpBuffer->box[b].bave = 0;

	for (i = r0; i <= r1; i++)
	{
		for (j = g0; j <= g1; j++)
		{
			for (k = b0; k <= b1; k++) 
			{    
				index = INDEX(i,j,k);
				ptr = lpBuffer->lpHisto[index];
				if (ptr)
				{
					if (ptr->count > 0L) 
					{
						lpBuffer->box[b].r0 = MIN(i, lpBuffer->box[b].r0);
						lpBuffer->box[b].r1 = MAX(i, lpBuffer->box[b].r1);
						lpBuffer->box[b].g0 = MIN(j, lpBuffer->box[b].g0);
						lpBuffer->box[b].g1 = MAX(j, lpBuffer->box[b].g1);
						lpBuffer->box[b].b0 = MIN(k, lpBuffer->box[b].b0);
						lpBuffer->box[b].b1 = MAX(k, lpBuffer->box[b].b1);
						lpBuffer->box[b].rave += (ULONG)i * (ULONG)ptr->count;
						lpBuffer->box[b].gave += (ULONG)j * (ULONG)ptr->count;
						lpBuffer->box[b].bave += (ULONG)k * (ULONG)ptr->count;
						count += (ULONG)ptr->count;
					}
				}
			}
		}
	}
	// box is now shrunk

	if (count) 
	{
		lpBuffer->box[b].rave /= count;
		lpBuffer->box[b].gave /= count;
		lpBuffer->box[b].bave /= count;
	}

	lpBuffer->box[b].count = MIN(count, COUNT_LIMIT);

	if (lpBuffer->box[b].r0 == lpBuffer->box[b].r1 &&
		lpBuffer->box[b].g0 == lpBuffer->box[b].g1 &&
		lpBuffer->box[b].b0 == lpBuffer->box[b].b1) 
	{	// box is min size
		lpBuffer->box[b].count = 1L;       // so it won't get split again
	}
}

void CViewImage::Addcolor(int r, int g, int b, ULONG c)
{
	NODE	*ptr;
	ULONG	ltmp;
    
	DWORD index = INDEX(r, g, b);
	c = MIN(c, COUNT_LIMIT);
	if ((ptr = lpBuffer->lpHisto[index]) == NULL)  // new color
	{
		ptr = lpBuffer->lpHisto[index] = (NODE *)malloc(sizeof(NODE));
		ptr->index = -1;
		ptr->count = c;
	}
	else
	{    
		ltmp = ptr->count;
		ltmp += c;
		ptr->count = MIN(ltmp, COUNT_LIMIT);
	}
}

void CViewImage::Force(int r, int g, int b, ULONG c)
{
	NODE	*ptr;
	DWORD	index;
    
	c = MIN(c, COUNT_LIMIT);    
	index = INDEX(r, g, b);
    
	if ((ptr = lpBuffer->lpHisto[index]) == NULL)  // new color
	{
		ptr = lpBuffer->lpHisto[index] = (NODE *)malloc(sizeof(NODE));
		ptr->index = -1; 
		ptr->count = 0L;
	}
	ptr->count = c;
}

void CViewImage::AllocDIB()
{
	BITMAPINFOHEADER *lpbmih;
	BITMAPINFO *lpbmi; 
	long	i;
	long	dest_dib_width;
	DWORD	dwSize;
	MYLOGPALETTE Logpalette;
 
	dest_dib_width = WIDTHBYTES(8*xRes);
 	dwSize = dest_dib_width*yRes + ((long)sizeof(BITMAPINFOHEADER) + (long)(QUANTCOLORS * sizeof(RGBQUAD)));

	if (lpDIB)
		GlobalFree(lpDIB);
 
	lpDIB = (LPSTR)GlobalAllocPtr(GHND | GMEM_SHARE, dwSize);

	if(!lpDIB)
		return;
  
	lpbmih = (BITMAPINFOHEADER *)lpDIB;
	lpbmih->biSize				= sizeof(BITMAPINFOHEADER);
	lpbmih->biWidth			= (DWORD)xRes;
	lpbmih->biHeight			= (DWORD)yRes;
	lpbmih->biPlanes			= 1;
	lpbmih->biBitCount		= 8;
	lpbmih->biCompression	= BI_RGB;
	lpbmih->biSizeImage		= (DWORD)dwSize;
	lpbmih->biClrUsed			= QUANTCOLORS;
	lpbmih->biClrImportant	= QUANTCOLORS;
 
	lpbmi = (BITMAPINFO *)lpDIB;
	Logpalette.palVersion = 0x300;
	Logpalette.palNumEntries = QUANTCOLORS;

	for (i = 0; i < QUANTCOLORS; i++)
	{
		lpbmi->bmiColors[i].rgbRed =
		Logpalette.palPalEntry[i].peRed =	(UBYTE)(lpBuffer->red[i]);
		lpbmi->bmiColors[i].rgbGreen =
		Logpalette.palPalEntry[i].peGreen = (UBYTE)(lpBuffer->green[i]);
		lpbmi->bmiColors[i].rgbBlue =
		Logpalette.palPalEntry[i].peBlue =	(UBYTE)(lpBuffer->blue[i]);
		lpbmi->bmiColors[i].rgbReserved =
		Logpalette.palPalEntry[i].peFlags =	0;
	}

	if (hPalette)
		DeleteObject(hPalette);
	hPalette = (HPALETTE)CreatePalette((LOGPALETTE*)&Logpalette);
}


int CViewImage::GetNeighbor(int r, int g, int b)
{
	int	bReturn; 

	DWORD	index = 0;
	long	min_dist = 0;
	long	dist = 0;
	int	c;
	int	dr,dg,db;
	int	i,j,k; 
	NODE	*ptr;
  
	index = INDEX((r>>3), (g>>3), (b>>3));
	if ((ptr = lpBuffer->lpHisto[index]) == NULL)
	{
		lpBuffer->lpHisto[index] = ptr = (NODE *)malloc(sizeof(NODE));
		ptr->count = 0L;
		ptr->index = -1;
	}
  
	if ((bReturn = ptr->index) == -1)
	{   
		ptr->index = 0;
		i = lpBuffer->red[0];
		j = lpBuffer->green[0];
		k = lpBuffer->blue[0];

		dr = i - r;
		dg = j - g;
		db = k - b;
		min_dist = lpBuffer->SQR[abs(dr)] + lpBuffer->SQR[abs(dg)] + lpBuffer->SQR[abs(db)];
   
		for (c = 1; c < QUANTCOLORS; c++)
		{
			i = lpBuffer->red[c];
			j = lpBuffer->green[c];
			k = lpBuffer->blue[c];
   	
			dr = i - r;
			dg = j - g;
			db = k - b;   
			dist = lpBuffer->SQR[abs(dr)] + lpBuffer->SQR[abs(dg)] + lpBuffer->SQR[abs(db)];
    	
			if (dist < min_dist)
			{
				ptr->index = c; 
				min_dist = dist;
			}
		}
		bReturn = ptr->index;
	}
	return bReturn;
}


void CViewImage::Jitter(long x, long y, int *r, int *g, int *b)
{
	int p,q;
	int tmp;
 
	tmp = *r;
	if (tmp < 248)
	{
		p = tmp & 7;
		q = jitterx(x,y,0);
		if (p <= q)
			tmp += 8;
   
		q = tmp + jittery(x,y,0);
		if (q >= 0 && q <= 255)
		tmp = q;
  
		*r = tmp & 0xF8;
	}
  
	tmp = *g;
	if (tmp < 248)
	{
		p = tmp & 7;
		q = jitterx(x,y,1);
		if (p <= q)
			tmp += 8;
   
		q = tmp + jittery(x, y, 1);
		if (q >= 0 && q <= 255)
			tmp = q;
  
		*g = tmp & 0xF8;
	} 
  
	tmp = *b;
	if (tmp < 248)
	{
		p = tmp & 7;
		q = jitterx(x,y,2);
		if (p <= q)
			tmp += 8;
   
		q = tmp + jittery(x,y,2);
		if (q >= 0 && q <= 255)
			tmp = q;
  
		*b = tmp & 0xF8;
	}
}

void CViewImage::OnPaletteChanged(CWnd *pWnd)
{
	CDC *pDC;
	HPALETTE hOldPal;
	int i;
	
	if (pWnd != this)
	{
		pDC = GetDC();
		hOldPal = SelectPalette(pDC->m_hDC, hPalette, FALSE);
		i = pDC->RealizePalette();       // Realize drawing palette

		if (i)                         // Did the realization change?
			pDC->UpdateColors();

		if (hOldPal)
			SelectPalette(pDC->m_hDC, hOldPal, FALSE);

		pDC->RealizePalette();
		ReleaseDC(pDC);
		OnQueryNewPalette();
	}
}

BOOL CViewImage::OnQueryNewPalette()
{
	CDC *pDC;
	HPALETTE hOldPal;
	int i;
	
	pDC = GetDC();
	hOldPal = SelectPalette(pDC->m_hDC, hPalette, FALSE);
	i = pDC->RealizePalette();       // Realize drawing palette.

	if (i)                         // Did the realization change?
		InvalidateRect(NULL, TRUE);

	if (hOldPal)
		SelectPalette(pDC->m_hDC, hOldPal, FALSE);
	pDC->RealizePalette();
	ReleaseDC(pDC);

	return TRUE;
}

