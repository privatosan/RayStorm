/***************
 * PROGRAM:       Modeler
 * NAME:          gfx.h
 * DESCRIPTION:   graphic commands
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    23.06.96 mh    initial release
 *    06.02.97 ah    added DIRECTLINE
 ***************/

#ifndef GFX_H
#define GFX_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifdef __AMIGA__

#ifndef LIST_H
#include "list.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifdef __OPENGL__
#ifndef OPENGL_H
#include "opengl.h"
#endif
#endif

#include <graphics/rastport.h>
#include <graphics/view.h>

class PENS : public SLIST
{
	public:
		ULONG pen;
		BOOL allocated;
		COLOR color;
};

#ifndef MUIDEFS_H
#include "MUIDefs.h"
#endif

#else

#include "coldef.h"

#endif // __AMIGA__

enum gfxMode
{
	GFX_NORMAL, GFX_COMPLEMENT, GFX_TRANSPARENT, GFX_DOTTED, GFX_COMPLEMENT_DOTTED
};

enum gfxOutput
{
	GFX_SCREEN, GFX_MEMORY1, GFX_MEMORY2, GFX_FOREIGN
};

class GFX
{
private:
	gfxOutput output;
	gfxMode writemode;
#ifdef __AMIGA__
	DISPLAY *display;
	struct RastPort *rport;          // current rastport
	struct RastPort *directrport;    // rastport for direct rendering
	struct RastPort memrport1;      // first memory rastport
	struct RastPort memrport2;      // second memory rastport
	int clipleft, clipright, cliptop, clipbottom;
	int left, top;
	PENS pens[COLOR_MAX];
	PENS *penlist;
	ColorMap *cmap;
#else
	CDC *pDC, *pScreenDC, *pMem1DC, *pMem2DC, *pForeignDC;
	CBitmap *pBitmap1, *pBitmap2, *pOldBitmap1, *pOldBitmap2;
#endif // __AMIGA__
	int pen;

public:
#ifdef __AMIGA__
	void Init(Object*, DISPLAY*);
	void ChangeSize(int, int, int, int);
	void Cleanup();
	gfxOutput GetOutput() { return output; };
	void SetRastPort(struct RastPort*);
	void AllocPens();
	void FreePens();
#else
	void Init(CWnd *);
	void Cleanup();
	void SetForeignDC(CDC *);
#endif // __AMIGA__
	void Line(int, int, int, int);
	void ClipLine(int, int, int, int);
	void SetPen(int);
	void SetColor(float, float, float);
	gfxMode SetWriteMode(gfxMode);
	gfxOutput SetOutput(gfxOutput);
	void SetClipping(int, int, int, int);
	void Rectangle(int, int, int, int);
	void FillRect(int, int, int, int);
	void BitBlt(gfxOutput, int, int, int, int, gfxOutput, int, int);
	void TextXY(int, int, char *, int len = -1);
	int GetTextExtentX(char *);
	int GetTextExtentY();

private:
#ifdef __AMIGA__
	struct RastPort *GetRastPortFromMode(gfxOutput);
#else
	CDC *GetDCFromMode(gfxOutput);
#endif // __AMIGA__
};

#endif
