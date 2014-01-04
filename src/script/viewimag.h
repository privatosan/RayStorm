 /***************
 * NAME:				viewimag.h
 * PROJECT:			RayStorm
 * VERSION:			v0.1 21.03.1996
 * DESCRIPTION:	definitions for display.cpp
 * AUTHORS:			Mike Hesser
 * BUGS:				none
 * TO DO:		-
 * HISTORY:		DATE			NAME	COMMENT
 *					21.03.1996	mh		first release (0.1)
 ***************/

#ifndef VIEWIMAGE_H
#define VIEWIMAGE_H

#include "typedefs.h"

// colors to quantizize to
#define QUANTCOLORS 256

// dithering methods 
#define IDX_NONE		0	// no dither
#define IDX_ORDERED	1	// ordered dither
#define IDX_JITTER	2	// jitter preprocessing

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define CLIP(x) ((x) > 255 ? 255:((x) < 0 ?0:(x)))
#define COLOR_MAX 32
#define COUNT_LIMIT 0xFFFFL

#define INDEX(r,g,b) (((DWORD)b) | (((DWORD)g)<<5) | (((DWORD)r)<<10) )

#define WIDTHBYTES(bits)	(((bits) + 31) / 32 * 4)

#define JITTER_TABLE_BITS	10
#define JITTER_TABLE_SIZE	(1<<JITTER_TABLE_BITS)
#define JITTER_MASK			(JITTER_TABLE_SIZE - 1)

// jitter macros
#define jitterx(x,y,s) (uranx[((x+(y<<2)) + irand[(x + s)&JITTER_MASK])&JITTER_MASK])
#define jittery(x,y,s) (urany[((y+(x<<2)) + irand[(y + s)&JITTER_MASK])&JITTER_MASK])

struct BOX
{
	int	r0,r1, g0,g1, b0,b1;
	long	rave,gave,bave;
	ULONG	count;
};

struct NODE
{
	int	index;
	ULONG	count; 
};

struct QUANT_BUFFER
{
	FARPROC	lpStatus;
	int	red[QUANTCOLORS], green[QUANTCOLORS], blue[QUANTCOLORS];
	BOX	box[QUANTCOLORS];
	long	SQR[QUANTCOLORS];
	NODE	**lpHisto;
};

struct MYLOGPALETTE
{
	WORD palVersion;
	WORD palNumEntries;
	PALETTEENTRY palPalEntry[QUANTCOLORS];
};

class CViewImage : public CDialog
{
public:
	CViewImage(CWnd *pParent = NULL);
	~CViewImage();

	enum { IDD = IDD_DISPLAY };

	CWnd *pView;

	void	Update(UBYTE *, BOOL, BOOL);
	void	SetParams(int d, BOOL s, BOOL st);

protected:
	int	xRes, yRes;
	BOOL	bStretched, bStatus;
	HPALETTE hPalette;
	QUANT_BUFFER *lpBuffer;
	int	nDither;
	LPSTR	lpDIB;
	UBYTE	*lpBitmap;
	
protected:
	void	UpdateStatus(char *);
	void	Quantize();
	void	Mediancut();
	void	Dither();
	void	Jitter(long, long, int *, int *, int *);
	void	Makebox(int, int, int, int, ULONG);
	void	Squeeze(int);
	void	Addcolor(int, int, int, ULONG);
	void	Force(int, int, int, ULONG);
	int	GetNeighbor(int, int,int);
	void 	AllocDIB();

	afx_msg void OnClose();
	afx_msg void OnPaint();	
	afx_msg void OnPaletteChanged(CWnd *);
	afx_msg BOOL OnQueryNewPalette();
	DECLARE_MESSAGE_MAP()
};

#endif
