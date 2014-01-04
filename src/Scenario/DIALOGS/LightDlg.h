/***************
 * NAME:				lightdlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	light settings dialog definitions
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						21.09.1996	mh		comment
 ***************/

#ifndef LIGHTDLG_H
#define LIGHTDLG_H

#include "afxcmn.h"
#include "resource.h"
#include "colbutt.h"
#include "slidectl.h"
#include "light.h"
#include "display.h"
#include "flares.h"
#include "star.h"

class CLightSetPage : public CPropertyPage
{
public:
	CLightSetPage(); 
	enum { IDD = IDD_LIGHTSET };

	CColorButton m_cbtnColor;
	float m_Angle, m_Radius, m_Falloff;
	POINT_LIGHT *m_pLight;
	BOOL m_bShadow, m_bFlares, m_bStar;

protected:
	afx_msg void ExtraPagesSwitch();
	virtual void OnOK();
	virtual BOOL OnInitDialog();	
	virtual void DoDataExchange(CDataExchange*);

	DECLARE_MESSAGE_MAP()
};

class CDrawArea : public CStatic
{
public:
	DISPLAY *display;
	FLARE *flares;
	STAR *star;

	CDrawArea();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};

class CFlareSetPage : public CPropertyPage
{
public:
	CFlareSetPage(); 
	enum { IDD = IDD_FLARESET };

	DISPLAY *display;
	POINT_LIGHT *m_pLight;
	FLARE *pFlareRoot;

	void UpdatePreview();
	
protected:
	CEditSlider m_sldNumber, m_sldRadius, m_sldPos, m_sldTilt;
	CColorButton m_cbtnColor;
	CComboBox m_cboxType, m_cboxStyle;
	CDrawArea m_DrawArea;
	FLARE *pCurFlare;
	int m_Number, count, m_Radius;
	BOOL bLock;
	
	USES_EDITSLIDERS
	afx_msg void Add();
	afx_msg void Remove();
	afx_msg void OnLButtonDown(UINT, CPoint);
		
	void UpdateParams();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();	
	
	DECLARE_MESSAGE_MAP()
};


class CStarSetPage : public CPropertyPage
{
public:
	CStarSetPage(); 
	~CStarSetPage(); 
	enum { IDD = IDD_STARSET };

	DISPLAY *display;

protected:
	CDrawArea m_DrawArea;
	CColorButton m_cbtnStarColor, m_cbtnRingColor;
	CEditSlider m_sldRadius, m_sldSpikes, m_sldRange, 
					m_sldHRadius, m_sldHIRadius, m_sldTilt;
	BOOL m_bStarEnable, m_bHaloEnable, m_bRandom, bLock;
	POINT_LIGHT *pLight;
	STAR *pStar;
	
	void UpdateParams();

	virtual void OnOK();
	virtual BOOL OnInitDialog();	
	virtual void DoDataExchange(CDataExchange*);
	USES_EDITSLIDERS

	DECLARE_MESSAGE_MAP()
};

#endif
