/***************
 * PROGRAM:       Modeler
 * NAME:          gfx2d.h
 * DESCRIPTION:   graphic commands
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		23.06.96 mh		initial release
 *		06.02.97	ah		added DIRECTLINE
 ***************/

#ifndef GFX2D_H
#define GFX2D_H

#ifdef __AMIGA__
	#include <graphics/gfx.h>
	#include <pragma/graphics_lib.h>

	#define PEN_BACKGROUND		0
	#define PEN_UNSELECTED		1
	#define PEN_SELECTED			2
	#define SETPEN(pen)			SetAPen(display->rport,pen)
	#define SETPEN1(pen)			SetAPen(rport,pen)
	#define TEXTXY(x,y,s)		{ Move(rport,x, y); \
										  Text(rport,s,strlen(s)); }
	#define TEXTXSIZE(s)			(TextLength(rport,s,strlen(s)))
	#define TEXTYSIZE				(rport->Font->tf_YSize)
	#define LINE(x1,y1, x2,y2) clipLine((int)x1,(int)y1,(int)x2,(int)y2);
	#define DIRECTLINE(x1,y1, x2,y2) { Move(directrport, (SHORT)x1, (SHORT)y1); \
										Draw(directrport, (SHORT)x2, (SHORT)y2); }
#else
	#define SETPEN(pen)			SetPen(display->DC, pen)
	#define SETPEN1(pen)			SetPen(DC, pen)
	#define TEXTXSIZE(s)			DC->GetTextExtent(s, strlen(s)).cx
	#define TEXTYSIZE				DC->GetTextExtent(" ", 1).cy
	#define TEXTXY(x,y,s)		{ DC->TextOut(x,y - TEXTYSIZE,s); }
	#define LINE(x1,y1, x2,y2) { MoveToEx(DC->m_hDC, x1,y1, NULL); \
											 LineTo(DC->m_hDC, x2,y2); }
	#define DIRECTLINE(x1,y1, x2,y2) { MoveToEx(DirectDC->m_hDC, x1,y1, NULL); \
											 LineTo(DirectDC->m_hDC, x2,y2); }
#endif

#endif
