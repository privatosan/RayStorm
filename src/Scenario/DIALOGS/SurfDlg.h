/***************
 * NAME:				surfdlg.h
 * PROJECT:			Scenario
 * VERSION:			0.6 30.08.1996
 * DESCRIPTION:	definition file of CSurfaceSheet, CColorPage, CBrushPage, CTexturePage
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						03.09.1996	mh		comment, ShowParam
 *						06.09.1997	mh		renamed identifiers
 ***************/

#include "afxcmn.h"
#include "colbutt.h"
#include "picctl.h"
#include "surface.h"

// color page
class CColorPage : public CPropertyPage
{
	enum { IDD = IDD_COLOR };

public:
	CColorButton m_cbtnDiffuse, m_cbtnSpecular, m_cbtnTranspar,
					 m_cbtnReflect, m_cbtnAmbient, m_cbtnDifftrans,
					 m_cbtnSpectrans;
	CSpinButtonCtrl m_spnSpecExp, m_spnTransExp, m_spnRefract, 
					 m_spnFogLength, m_spnTransluc;

	SURFACE *surf;
	CColorPage();
	void SetValues(SURFACE *);	

protected:
	virtual BOOL OnInitDialog();
	afx_msg LRESULT UpdatePreview(WPARAM, LPARAM);
	afx_msg void UpdatePreview();
	
	DECLARE_MESSAGE_MAP()
};

// brush page
class CBrushPage : public CPropertyPage
{
	enum { IDD = IDD_BRUSH };

public:
	CBitmap m_bmpAdd, m_bmpRemove, m_bmpName, m_bmpUp, m_bmpDown;
	CButton m_btnAdd, m_btnRemove, m_btnName, m_btnUp, m_btnDown;
	CListBox m_lboxBrush;
	CComboBox m_cboxWrap, m_cboxType;
	CPtrArray aBrushItems;
	SURFACE *surf;
	int curbrush;

	CBrushPage();
	~CBrushPage();

	void ShowParam();
	void GetParam();
	void SetValues(SURFACE *);	

protected:
	void *AddBrush();
	void UpdateBrushList();
	
	virtual BOOL OnInitDialog();
	afx_msg void OnSel();
	afx_msg void OnAdd();
	afx_msg void OnName();
	afx_msg void OnRemove();
	afx_msg void OnUp();
	afx_msg void OnDown();

	DECLARE_MESSAGE_MAP()
};

// texture page
class CTexturePage : public CPropertyPage
{
	enum { IDD = IDD_TEXTURE };

public:
	CBitmap m_bmpAdd, m_bmpRemove, m_bmpName, m_bmpUp, m_bmpDown;
	CButton m_btnAdd, m_btnRemove, m_btnName, m_btnUp, m_btnDown;
	CListBox m_lboxTexture, m_lboxParameter;
	CSpinButtonCtrl m_spnValue;
	CPtrArray aTextureItems;
	int curparam, curtexture;
	SURFACE *surf;

	CTexturePage(); 
	void SetValues(SURFACE *);	
	void GetParam();

protected:
	void ShowParam(BOOL);
	void *AddTexture();
	void UpdateTextureList();
	
	virtual BOOL OnInitDialog();
	afx_msg void OnSel();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnName();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void UpdatePreview();

	DECLARE_MESSAGE_MAP()
};

// surface sheet
class CSurfaceSheet : public CPropertySheet
{
public:
	DECLARE_DYNAMIC(CSurfaceSheet)
	CSurfaceSheet(CWnd*);

	CColorPage m_pgColor;
	CBrushPage m_pgBrush;
	CTexturePage m_pgTexture;
	OBJECT *pObject;
	CPicture m_wndPreview;
	CButton m_btnPreview, m_btnLoad, m_btnSave;
	SURFACE *pSurface;
	HANDLE hThread;

	void UpdateValues(SURFACE *, BOOL);
	void OnPreview();
	void OnOK();

	afx_msg void OnDestroy();

protected:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

