/***************
 * MODULEE:			lightdlg.cpp
 * PROJECT:			Scenario
 * VERSION:			0.2 30.08.1996
 * DESCRIPTION:	light/flare settings (point and spot light)
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 *						26.09.1996	mh		added radius as attribute
 *						03.10.1996	mh		added lens flares to settings sheet
 *						30.11.1996	mh		releasable version with lens effects
 ***************/

#include "typedefs.h"
#include "camview.h"
#include "lightdlg.h"
#include "transdlg.h"
#include "mainfrm.h"
#include "resource.h"
#include "ddxddv.h"

static char *aszFlareFunctions[] =
{
	"Plain",
	"Ring",
	"Exp 2",
	"Exp 3",
	"Radial",
	NULL
};

static char *aszFlareStyles[] =
{
	"Disc",
	"Triangle",
	"Rectangle",
	"Poly 5",
	"Poly 6",
	"Poly 7",
	"Poly 8",
	NULL
};

/* LightSetPage */

BEGIN_MESSAGE_MAP(CLightSetPage, CPropertyPage)
	ON_BN_CLICKED(ID_LIGHTSET_FLARES, ExtraPagesSwitch)
	ON_BN_CLICKED(ID_LIGHTSET_STAR, ExtraPagesSwitch)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CLightSetPage::CLightSetPage()
    : CPropertyPage(CLightSetPage::IDD)
{
}

/*************
 * DESCRIPTION:	switch flare/light burn page on and off
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CLightSetPage::ExtraPagesSwitch()
{
	UpdateData();
	
	if (m_bFlares)
	{
		if (!m_pLight->flares)
			m_pLight->SetDefaultFlares(NULL);

		m_pLight->flags |= OBJECT_LIGHT_FLARES;
	}
	else
		m_pLight->flags &= ~OBJECT_LIGHT_FLARES;

	if (m_bStar)
	{
		// generate a star if it's not given
		if (!m_pLight->star)
		{
			m_pLight->star = new STAR;
			if (!m_pLight->star)
				return;
			m_pLight->star->SetSpikes(4);
		}
		m_pLight->flags |= OBJECT_LIGHT_STAR;
	}
	else
		m_pLight->flags &= ~OBJECT_LIGHT_STAR;

	((CSettingsSheet*)GetParent())->ExtraPagesOnOff();
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CLightSetPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
		
	VERIFY(m_cbtnColor.SubclassDlgItem(ID_LIGHTSET_COLOR, this));

	if (m_pLight->GetType() != OBJECT_SPOTLIGHT)
	{
		GetDlgItem(ID_LIGHTSET_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(ID_LIGHTSET_ANGLETEXT)->EnableWindow(FALSE);
	}
	
	return TRUE;
}

/*************
 * DESCRIPTION:	transfer data
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CLightSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, ID_LIGHTSET_FALLOFF, m_Falloff);
	DDV_MinFloat(pDX, m_Falloff, 0, FALSE);
	DDX_Text(pDX, ID_LIGHTSET_RADIUS, m_Radius);
	DDV_MinFloat(pDX, m_Radius, 0, FALSE);

	// point light has no opening angle
	if (m_pLight->GetType() == OBJECT_SPOTLIGHT)
	{
		DDX_Text(pDX, ID_LIGHTSET_ANGLE, m_Angle);
		DDV_MinMaxFloatExclude(pDX, m_Angle, 0, 360);
	}
		
	DDX_Check(pDX, ID_LIGHTSET_SHADOW, m_bShadow);
	DDX_Check(pDX, ID_LIGHTSET_FLARES, m_bFlares);
	DDX_Check(pDX, ID_LIGHTSET_STAR, m_bStar);
}

/*************
 * DESCRIPTION:	get light color and opening angle
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CLightSetPage::OnOK()
{
	m_pLight->color = m_cbtnColor.col;
	m_pLight->falloff = m_Falloff;
	m_pLight->r = m_Radius;

	if (m_pLight->GetType() == OBJECT_SPOTLIGHT)
		((SPOT_LIGHT *)m_pLight)->angle = m_Angle * PI_180;
		
	m_pLight->flags = 0;

	if (m_bShadow)
		m_pLight->flags |= OBJECT_LIGHT_SHADOW;

	if (m_bFlares)
		m_pLight->flags |= OBJECT_LIGHT_FLARES;

	if (m_bStar)
		m_pLight->flags |= OBJECT_LIGHT_STAR;

	CPropertyPage::OnOK();
}


/* CDrawArea */

BEGIN_MESSAGE_MAP(CDrawArea, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CDrawArea::CDrawArea()
{
	display = NULL;
	flares = NULL;
	star = NULL;
}

/*************
 * DESCRIPTION:	draws the lensflares
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CDrawArea::OnPaint()
{
	CBitmap *pOldBitmap, bitmap;
	CDC dc, *pDrawDC;
	CRgn region;
	CRect rect;
	RECTANGLE r;
		
	CPaintDC DC(this);
	pDrawDC = &DC;
	
	if (!display)
		return;

	// only paint the rect that needs repainting
	GetClientRect(rect);

	// draw to offscreen bitmap for fast looking repaints
	if (dc.CreateCompatibleDC(&DC))
	{
		if (bitmap.CreateCompatibleBitmap(&DC, rect.Width(), rect.Height()))
		{
			pDrawDC = &dc;

			// offset origin more because bitmap is just piece of the whole drawing
			dc.OffsetViewportOrg(-rect.left, -rect.top);
			pOldBitmap = dc.SelectObject(&bitmap);
			dc.SetBrushOrg(rect.left % 8, rect.top % 8);

			// might as well clip to the same rectangle
			dc.IntersectClipRect(rect);
		}
	}

	// save current display DC
	r.left = rect.left;
	r.top = rect.top;
	r.right = rect.right;
	r.bottom = rect.bottom;

	gfx.SetForeignDC(&dc);
	gfx.SetOutput(GFX_FOREIGN);

	// lens flares must be clipped
	region.CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
	gfx.SetClipping(rect.left, rect.top, rect.right, rect.bottom);
	gfx.SetPen(PEN_BACKGROUND);
	gfx.FillRect(rect.left, rect.top, rect.right, rect.bottom);
	gfx.SetPen(PEN_BLACK);

	// draw flares in draw area
	if (flares)
		flares->Draw(display, &r, rect.right - 15, rect.top + 15, FALSE);
	
	// draw light-star in draw area
	if (star)
	{
		if (!flares)
			star->Draw(display, rect.right >> 1, rect.bottom >> 1, rect.right);
		else
			star->Draw(display, rect.right - 15, rect.top + 15, rect.right);
	}

	if (pDrawDC != &DC)
	{
		DC.SetViewportOrg(0, 0);
		DC.SetWindowOrg(0, 0);
		DC.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);
		dc.SetMapMode(MM_TEXT);
		DC.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),	&dc, 0, 0, SRCCOPY);
		dc.SelectObject(pOldBitmap);
	}
}

/* CFlareSetPage */

BEGIN_MESSAGE_MAP(CFlareSetPage, CPropertyPage)
 	ON_LBN_SELCHANGE(ID_FLARES_STYLES, UpdateParams)
	ON_COMMAND(ID_FLARES_ADD, Add)
	ON_COMMAND(ID_FLARES_REMOVE, Remove)
	ON_EN_CHANGE(ID_FLARES_SLE_NUMBER, UpdateParams)
	ON_EN_CHANGE(ID_FLARES_SLE_RADIUS, UpdateParams)
	ON_EN_CHANGE(ID_FLARES_SLE_POS, UpdateParams)
	ON_EN_CHANGE(ID_FLARES_SLE_TILT, UpdateParams)
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CFlareSetPage::CFlareSetPage()
    : CPropertyPage(CFlareSetPage::IDD)
{
	bLock = FALSE;
	pCurFlare = NULL;
	m_Number = 1;
}

/*************
 * DESCRIPTION:	init dialog, count number of flares
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CFlareSetPage::OnInitDialog()
{
	CSettingsSheet *pParent;
	FLARE *pFlare, *pH;
	int i;
	
	pParent = (CSettingsSheet *)GetParent();
	m_pLight = (POINT_LIGHT *)(pParent)->m_pObject;

	pFlareRoot = NULL;
	pFlare = m_pLight->flares;
	while (pFlare)
	{
		pH = pFlare->Duplicate();
		if (!pH)
			return FALSE;
		pH->Append((SLIST**)&pFlareRoot);
		pFlare = (FLARE *)pFlare->GetNext();
	}

	m_DrawArea.display = display;
	m_DrawArea.flares = pFlareRoot;
	m_DrawArea.star = pParent->m_pStar;
	
	CPropertyPage::OnInitDialog();
	
	VERIFY(m_DrawArea.SubclassDlgItem(ID_FLARES_DRAWAREA, this));
	VERIFY(m_cbtnColor.SubclassDlgItem(ID_FLARES_COLOR, this));
	VERIFY(m_cboxType.SubclassDlgItem(ID_FLARES_FUNCTION, this));
	VERIFY(m_cboxStyle.SubclassDlgItem(ID_FLARES_STYLES, this));
	VERIFY(m_sldNumber.Init(this, ID_FLARES_SLE_NUMBER, ID_FLARES_SLB_NUMBER));
	VERIFY(m_sldRadius.Init(this, ID_FLARES_SLE_RADIUS, ID_FLARES_SLB_RADIUS, 0, 100));
	VERIFY(m_sldPos.Init(this, ID_FLARES_SLE_POS, ID_FLARES_SLB_POS, -250, 250));
	VERIFY(m_sldTilt.Init(this, ID_FLARES_SLE_TILT, ID_FLARES_SLB_TILT, 0, 359));

	// fill combobox with flare functions
	i = 0;
	while (aszFlareFunctions[i])
	{
		m_cboxType.AddString(aszFlareFunctions[i]);
		i++;
	}

	// fill combobox with flare styles
	i = 0;
	while (aszFlareStyles[i])
	{
		m_cboxStyle.AddString(aszFlareStyles[i]);
		i++;
	}

	// count number of flares
	count = 0;
	pFlare = pFlareRoot;

	while (pFlare)
	{
		count++;
		pFlare = (FLARE *)pFlare->GetNext();
	}

	m_sldNumber.SetScrollRange(1, count);

	m_Number = 1;
	pCurFlare = NULL;
	
	bLock = TRUE;
	m_sldNumber.SetValue(1);
	bLock = FALSE;

	UpdateParams();

	return TRUE;
}

/*************
 * DESCRIPTION:	add new flare to flares list
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CFlareSetPage::Add()
{
	FLARE *pFlare;

	pFlare = new FLARE;
	if (!pFlare)
		return;

	pFlare->Append((SLIST**)&pFlareRoot);
	pFlare->edges = pCurFlare->edges;
	pFlare->type = pCurFlare->type;
	pFlare->tilt = pCurFlare->tilt;

	pCurFlare->selected = FALSE;
	pCurFlare = NULL;
	count++;
	m_Number = count;
	m_sldNumber.SetScrollRange(1, count);
	m_sldNumber.SetValue(m_Number);
}

/*************
 * DESCRIPTION:	remove current flare from list
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CFlareSetPage::Remove()
{
	// at least one flare remains
	if (count > 1)
	{
		pCurFlare->Remove((SLIST**)&pFlareRoot);
		count--;
		if (m_Number > count)
			m_Number = count;
		pCurFlare = NULL;
		
		m_sldNumber.SetScrollRange(1, count);
		m_sldNumber.SetValue((float)m_Number);
	}
}

/*************
 * DESCRIPTION:	ON_WM_LBUTTONDOWN handler
 *						handles left mouse button
 *						(selection of flaret, ...)
 * INPUT:			nFlags	indicates if various keys are pressed
 *						point   Specifies the x- and y-coordinate of the cursor
 * OUTPUT:			-
 *************/
void CFlareSetPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;
	int index;

	ClientToScreen(&point);
	GetDlgItem(ID_FLARES_DRAWAREA)->GetWindowRect(&rect);

	if (!rect.PtInRect(point))
		return;

	index = SelectedFlare(pFlareRoot, point.x - rect.left, point.y - rect.top, rect.Width(), rect.Height(), rect.Width() - 15, 15);

	SetDlgItemInt(ID_FLARES_SLE_NUMBER, index);
}

/*************
 * DESCRIPTION:	get values from dialog, change evenutally to another flare
 *						and set dialog values to the flare data
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CFlareSetPage::UpdateParams()
{
	FLARE *pOldFlare, *pFlare;
	int i, style;

	if (bLock)
		return;

	if (pCurFlare)
	{
		m_Number = (int)m_sldNumber.GetValue();
		
		pCurFlare->radius = m_sldRadius.GetValue()/100;
		pCurFlare->pos = m_sldPos.GetValue()/100;
		pCurFlare->color = m_cbtnColor.col;
		pCurFlare->func = m_cboxType.GetCurSel();
		style = m_cboxStyle.GetCurSel();

		pFlare = pFlareRoot;
		while (pFlare)
		{
			pFlare->edges = style + 2;
			if (style == 0)
				pFlare->type = FLARE_DISC;
			else
				pFlare->type = FLARE_POLY;
				
			pFlare->tilt	= m_sldTilt.GetValue()*PI/180;
			pFlare = (FLARE *)pFlare->GetNext();
		}
	}

	pOldFlare = pCurFlare;

	bLock = TRUE;

	if (m_Number > count)
	{
		m_Number = 1;
		m_sldNumber.SetValue(m_Number);
	}
	else
		if (m_Number < 1)
		{
			m_Number = count;
			m_sldNumber.SetValue(m_Number);
		}

	pCurFlare = pFlareRoot;
	m_DrawArea.flares = pFlareRoot;
	
	i = 1;
	while (pCurFlare && i < m_Number)
	{
		pCurFlare = (FLARE*)pCurFlare->GetNext();
		i++;
	}

	if (pCurFlare && pCurFlare != pOldFlare)
	{
		if (pOldFlare)
			pOldFlare->selected = FALSE;
		pCurFlare->selected = TRUE;

		m_sldRadius.SetValue(pCurFlare->radius*100);
		m_sldPos.SetValue(pCurFlare->pos*100);

		m_cbtnColor.col = pCurFlare->color;
		m_cbtnColor.Invalidate(FALSE);
		m_cboxType.SetCurSel(pCurFlare->func);
		if (pCurFlare->type == FLARE_DISC)
			m_cboxStyle.SetCurSel(0);
		else
			m_cboxStyle.SetCurSel(pCurFlare->edges - 2);

		m_sldTilt.SetValue(pCurFlare->tilt*180/PI);
	}
	bLock = FALSE;
	
	m_DrawArea.Invalidate(FALSE);
}

/*************
 * DESCRIPTION:	called when OK pressed
 * INPUT:			-	
 * OUTPUT:			-
 *************/
void CFlareSetPage::OnOK()
{
	FLARE *h;

	if (pCurFlare)
		pCurFlare->selected = FALSE;
	UpdateParams();

	// delete all existing flares
	while (m_pLight->flares)
	{
		h = (FLARE*)m_pLight->flares->GetNext();
		delete m_pLight->flares;
		m_pLight->flares = h;
	}

	// and replace them with the new flares
	m_pLight->flares = pFlareRoot;
	
	CPropertyPage::OnOK();
}

/*************
 * DESCRIPTION:	called when cancel pressed, release flares
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CFlareSetPage::OnCancel()
{
	FLARE *pH;

	// delete all existing flares
	while (pFlareRoot)
	{
		pH = (FLARE*)pFlareRoot->GetNext();
		delete pFlareRoot;
		pFlareRoot = pH;
	}

	CPropertyPage::OnCancel();
}

/* CStarSetPage */

BEGIN_MESSAGE_MAP(CStarSetPage, CPropertyPage)
	ON_EN_CHANGE(ID_STAR_SLE_RADIUS, UpdateParams)
	ON_EN_CHANGE(ID_STAR_SLE_SPIKES, UpdateParams)
	ON_EN_CHANGE(ID_STAR_SLE_RANGE, UpdateParams)
	ON_EN_CHANGE(ID_STAR_SLE_HRADIUS, UpdateParams)
	ON_EN_CHANGE(ID_STAR_SLE_HIRADIUS, UpdateParams)
	ON_EN_KILLFOCUS(ID_STAR_WIDTH, UpdateParams)
	ON_EN_CHANGE(ID_STAR_SLE_TILT, UpdateParams)
	ON_BN_CLICKED(ID_STAR_ENABLE, UpdateParams)
	ON_BN_CLICKED(ID_STAR_HALOENABLE, UpdateParams)
	ON_BN_CLICKED(ID_STAR_RANDOM, UpdateParams)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CStarSetPage::CStarSetPage()
    : CPropertyPage(CStarSetPage::IDD)
{
	pStar = NULL;
}

/*************
 * DESCRIPTION:	destructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CStarSetPage::~CStarSetPage()
{
	if (pStar)
		delete pStar;
}

/*************
 * DESCRIPTION:	init dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CStarSetPage::OnInitDialog()
{
	CSettingsSheet *pParent;

	pParent = (CSettingsSheet *)GetParent();
	pLight = (POINT_LIGHT *)(pParent)->m_pObject;

	pStar = pLight->star->Duplicate();
	if (!pStar)
		return FALSE;
	
	m_DrawArea.display = display;
	m_DrawArea.star = pParent->m_pStar = pStar;
	
	m_bStarEnable = (pStar->flags & STAR_ENABLE) == STAR_ENABLE;
	m_bHaloEnable = (pStar->flags & STAR_HALOENABLE) == STAR_HALOENABLE;
	m_bRandom = (pStar->flags & STAR_RANDOM) == STAR_RANDOM;

	CDialog::OnInitDialog();

	VERIFY(m_cbtnStarColor.SubclassDlgItem(ID_STAR_COLOR, this));
	m_cbtnStarColor.col = pStar->starcolor;
	VERIFY(m_cbtnRingColor.SubclassDlgItem(ID_STAR_RINGCOLOR, this));
	m_cbtnRingColor.col = pStar->ringcolor;
	VERIFY(m_DrawArea.SubclassDlgItem(ID_STAR_DRAWAREA, this));
	VERIFY(m_sldRadius.Init(this, ID_STAR_SLE_RADIUS, ID_STAR_SLB_RADIUS, 0, 100));
	VERIFY(m_sldSpikes.Init(this, ID_STAR_SLE_SPIKES, ID_STAR_SLB_SPIKES, 2, 100));
	VERIFY(m_sldRange.Init(this, ID_STAR_SLE_RANGE, ID_STAR_SLB_RANGE, 0, 100));
	VERIFY(m_sldTilt.Init(this, ID_STAR_SLE_TILT, ID_STAR_SLB_TILT, 0, 359));
	VERIFY(m_sldHRadius.Init(this, ID_STAR_SLE_HRADIUS, ID_STAR_SLB_HRADIUS, 0, 100));
	VERIFY(m_sldHIRadius.Init(this, ID_STAR_SLE_HIRADIUS, ID_STAR_SLB_HIRADIUS, 0, 100));
	
	bLock = TRUE;
	m_sldRadius.SetValue(pStar->starradius*100);
	m_sldSpikes.SetValue(pStar->spikes);
	m_sldRange.SetValue(pStar->range*100);
	m_sldTilt.SetValue(pStar->tilt*INV_PI_180);
	m_sldHRadius.SetValue(pStar->haloradius*100);
	m_sldHIRadius.SetValue(pStar->innerhaloradius*100);
	bLock = FALSE;

	UpdateParams();

	return TRUE;
}

/*************
 * DESCRIPTION:	update dialog control settings
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CStarSetPage::UpdateParams()
{
	int cnt;
	float range;
	BOOL bUpdate;

	if (bLock)
		return;

	UpdateData();

	if (m_bStarEnable)
		pStar->flags |= STAR_ENABLE;
	else
		pStar->flags &= ~STAR_ENABLE;

	if (m_bHaloEnable)
		pStar->flags |= STAR_HALOENABLE;
	else
		pStar->flags &= ~STAR_HALOENABLE;

	// update when random changed
	bUpdate = (!(m_bRandom && (pStar->flags & STAR_RANDOM)));

	if (m_bRandom)
		pStar->flags |= STAR_RANDOM;
	else
		pStar->flags &= ~STAR_RANDOM;

	pStar->starradius = m_sldRadius.GetValue()/100;
	pStar->tilt = m_sldTilt.GetValue()*PI_180;
	pStar->haloradius = m_sldHRadius.GetValue()/100;
	pStar->innerhaloradius = m_sldHIRadius.GetValue()/100;
	range = m_sldRange.GetValue()/100;
	cnt = (int)m_sldSpikes.GetValue();

	if (cnt != pStar->spikes || range != pStar->range || bUpdate)
	{
		pStar->range = range;
		pStar->SetSpikes(cnt);
	}

	m_DrawArea.Invalidate(FALSE);
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			pDX	given from system
 * OUTPUT:			-
 *************/
void CStarSetPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, ID_STAR_NOISE, pStar->noise);
	DDV_MinFloat(pDX, pStar->noise, 0, FALSE);
	DDX_Text(pDX, ID_STAR_BRIGHTNESS, pStar->brightness);
	DDV_MinFloat(pDX, pStar->brightness, 0, FALSE);
	DDX_Check(pDX, ID_STAR_ENABLE, m_bStarEnable);
	DDX_Check(pDX, ID_STAR_HALOENABLE, m_bHaloEnable);
	DDX_Check(pDX, ID_STAR_RANDOM, m_bRandom);
}

/*************
 * DESCRIPTION:	called when OK pressed
 * INPUT:			-	
 * OUTPUT:			-
 *************/
void CStarSetPage::OnOK()
{
	if (pLight->star)
		delete pLight->star;
	
	pStar->starcolor = m_cbtnStarColor.col;
	pStar->ringcolor = m_cbtnRingColor.col;
	pLight->star = pStar->Duplicate();

	CPropertyPage::OnOK();
}
