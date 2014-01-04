/***************
 * MODULE:			coldef.cpp
 * PROJECT:			Scenario
 * VERSION:			1.0 20.02.1996
 * DESCRIPTION:	pen and brush definitions
 * AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:       
 *				DATE		NAME	COMMENT
 *				20.02.95	mh		initial release
 *				23.09.96	mh		PEN_GRID has dotted style
 ***************/

#include "Coldef.h"

CPen	*pens[MAX_PENS];
CBrush	*brushes[MAX_BRUSHES];

static COLORREF RGB_COLORS[] =
{
	RGB(0xc0,0xc0,0xc0),
	RGB(255,255,255),
	RGB(0,0,0),
	RGB(124,124,124),
	RGB(0,0,0),
	RGB(0,0,0)
};

/*************
 * DESCRIPTION: initialize color objects
 * INPUT:		 -
 * OUTPUT:		 -
 *************/
void InitColorObjects()
{
	static DWORD pointarray[] =
	{
		1,1
	};

	LOGBRUSH logBrush;
	
	brushes[BRUSH_BACKGROUND] = new CBrush(RGB_COLORS[RGB_BACKGROUND]);
	brushes[BRUSH_SELECTED] = new CBrush(RGB_COLORS[RGB_SELECTED]);
	brushes[BRUSH_UNSELECTED] = new CBrush(RGB_COLORS[RGB_CLICKUNSEL]);

	pens[PEN_BACKGROUND] = new CPen(PS_SOLID, 0, RGB_COLORS[RGB_BACKGROUND]);
	pens[PEN_SELECTED] = new CPen(PS_SOLID, 0, RGB_COLORS[RGB_SELECTED]);
	pens[PEN_UNSELECTED] = new CPen(PS_SOLID, 0, RGB_COLORS[RGB_UNSELECTED]);
	pens[PEN_GRID] = new CPen(PS_SOLID, 0, RGB_COLORS[RGB_GRID]);
	brushes[BRUSH_BACKGROUND]->GetObject(sizeof(LOGBRUSH), &logBrush);
	pens[PEN_GRIDORTHO] = new CPen(PS_SOLID, 0, RGB_COLORS[PEN_GRID]);
//	pens[PEN_GRIDORTHO] = new CPen(PS_USERSTYLE, 0, &logBrush, 2/*sizeof(pointarray)/sizeof(DWORD)*/, pointarray);
	pens[PEN_BLACK] = new CPen(PS_SOLID, 0, RGB_COLORS[RGB_BLACK]);
//	pens[PEN_XOR] = new CPen(PS_
}

/*************
 * DESCRIPTION:	delete color objects
 * INPUT:			-
 * OUTPUT:			-
 *************/
void DeleteColorObjects()
{
	int i;
	
	for (i = 0; i < MAX_PENS; i++)
	{
		if (pens[i])
			delete pens[i];
	}
	
	for (i = 0; i < MAX_BRUSHES; i++)
	{
		if (brushes[i])
			delete brushes[i];
	}
}

/*************
 * DESCRIPTION:	sets pen color
 * INPUT:			DC		Pointer to DC-object
 *						no		number of pen
 * OUTPUT:			handle to old pen
 *************/
CPen *SetPen(CDC *DC, int no)
{
	return DC->SelectObject(pens[no]);
}

/*************
 * DESCRIPTION:	sets brush color
 * INPUT:			DC		Pointer to DC-object
 *						no		number of brush
 * OUTPUT:			handle to old brush
 *************/
CBrush *SetBrush(CDC *DC, int no)
{
	return DC->SelectObject(brushes[no]);
}

/*************
 * DESCRIPTION:	gets pointer to brush
 * INPUT:			no		number of brush
 * OUTPUT:			pointer to brush
 *************/
CBrush *GetBrush(int no)
{
	return brushes[no];
}

/*************
 * DESCRIPTION:	get RGB color
 * INPUT:			no		RGB constant
 * OUTPUT:			point to brush
 *************/
COLORREF GetRGB(int no)
{
	return RGB_COLORS[no];
}

