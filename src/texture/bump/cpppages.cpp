// cpppages.cpp : implementation file
//

#include "stdafx.h"

#include "imdefs.h"
#include "txdefs.h"

#include "cppcore.h"

#include "ssedit.h"

#include "cppresou.h"		// main symbols
#include "cpppages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CPageData {
public:

	float	m_params[16];

	TXHOSTINFO	*m_pHost;

	CColorPage	m_ColorPage;
	CInfoPage	m_InfoPage;
	CSizePage	m_SizePage;
	CControlsPage	m_ControlsPage;
};

extern "C" {

LPARAM	lCreateArg;

void *CreatePages(LPARAM lCreateArg, TXHOSTINFO *pHost, float *params)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPageData	*pPageData = NULL;
	try {
		pPageData = new CPageData;
	} catch (CMemoryException) {
	} catch (CResourceException) {
	}

	if (!pPageData)
		return NULL;

	pPageData->m_pHost = pHost;
	pPageData->m_ColorPage.m_pHostInfo = pHost;
	pPageData->m_InfoPage.m_pHostInfo = pHost;
	pPageData->m_SizePage.m_pHostInfo = pHost;
	pPageData->m_ControlsPage.m_pHostInfo = pHost;

	memcpy(pPageData->m_params, params, 16*sizeof(float));

	// Copy defaults into page variables
	pPageData->m_SizePage.m_fXSize = params[0];
	pPageData->m_SizePage.m_fYSize = params[1];
	pPageData->m_SizePage.m_fZSize = params[2];

	pPageData->m_ControlsPage.m_fBlend1 = params[3];
	pPageData->m_ControlsPage.m_fBlend2 = params[4];
	pPageData->m_ControlsPage.m_fBlend3 = params[5];
	pPageData->m_ControlsPage.m_fBlend4 = params[6];

	pPageData->m_ColorPage.m_Color1 = RGB((int)params[7],(int)params[8],(int)params[9]);
	pPageData->m_ColorPage.m_Color2 = RGB((int)params[10],(int)params[11],(int)params[12]);
	pPageData->m_ColorPage.m_Color3 = RGB((int)params[13],(int)params[14],(int)params[15]);

	if (AddPSPage(pHost, &pPageData->m_ColorPage)) {
		if (AddPSPage(pHost, &pPageData->m_SizePage)) {
			if (AddPSPage(pHost, &pPageData->m_ControlsPage)) {
				if (AddPSPage(pHost, &pPageData->m_InfoPage)) {
					return pPageData;
				}
				RemovePSPage(pHost, &pPageData->m_ControlsPage);
			}
			RemovePSPage(pHost, &pPageData->m_ColorPage);
		}
		RemovePSPage(pHost, &pPageData->m_SizePage);
	}

	delete pPageData;
	return NULL;
}

void ReleasePages(void *pClientData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT(pClientData);
	delete (CPageData *)pClientData;
}

void StoreParams(void *pClientData, float *params)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	// unnecessary

	CPageData	*pPageData = (CPageData *)pClientData;

	pPageData->m_params[0] = pPageData->m_SizePage.m_fXSize;
	pPageData->m_params[1] = pPageData->m_SizePage.m_fYSize;
	pPageData->m_params[2] = pPageData->m_SizePage.m_fZSize;

	pPageData->m_params[3] = pPageData->m_ControlsPage.m_fBlend1;
	pPageData->m_params[4] = pPageData->m_ControlsPage.m_fBlend2;
	pPageData->m_params[5] = pPageData->m_ControlsPage.m_fBlend3;
	pPageData->m_params[6] = pPageData->m_ControlsPage.m_fBlend4;

	pPageData->m_params[7] = GetRValue(pPageData->m_ColorPage.m_Color1);
	pPageData->m_params[8] = GetGValue(pPageData->m_ColorPage.m_Color1);
	pPageData->m_params[9] = GetBValue(pPageData->m_ColorPage.m_Color1);

	pPageData->m_params[10] = GetRValue(pPageData->m_ColorPage.m_Color2);
	pPageData->m_params[11] = GetGValue(pPageData->m_ColorPage.m_Color2);
	pPageData->m_params[12] = GetBValue(pPageData->m_ColorPage.m_Color2);

	pPageData->m_params[13] = GetRValue(pPageData->m_ColorPage.m_Color3);
	pPageData->m_params[14] = GetGValue(pPageData->m_ColorPage.m_Color3);
	pPageData->m_params[15] = GetBValue(pPageData->m_ColorPage.m_Color3);

	memcpy(params, pPageData->m_params, 16 * sizeof(float));
}

};

//  --->  Property Page Code <----


/////////////////////////////////////////////////////////////////////////////
// CColorPage property page

IMPLEMENT_DYNCREATE(CColorPage, CPropertyPage)

CColorPage::CColorPage() : CPropertyPage(CColorPage::IDD)
{
	//{{AFX_DATA_INIT(CColorPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CColorPage::~CColorPage()
{
}

void CColorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	ASSERT(m_pHostInfo);
	if (!pDX->m_bSaveAndValidate)
	{
		SubclassColorButton(m_pHostInfo, this, IDC_COLOR1, m_Color1);
		SubclassColorButton(m_pHostInfo, this, IDC_COLOR2, m_Color2);
		SubclassColorButton(m_pHostInfo, this, IDC_COLOR3, m_Color3);

//		... this is not required
//
//		SetButtonColor(m_pHostInfo, this, IDC_COLOR1, m_Color1);
//		SetButtonColor(m_pHostInfo, this, IDC_COLOR2, m_Color2);
//		SetButtonColor(m_pHostInfo, this, IDC_COLOR3, m_Color3);
	} else {
		GetButtonColor(m_pHostInfo, this, IDC_COLOR1, &m_Color1);
		GetButtonColor(m_pHostInfo, this, IDC_COLOR2, &m_Color2);
		GetButtonColor(m_pHostInfo, this, IDC_COLOR3, &m_Color3);
	}
}


BEGIN_MESSAGE_MAP(CColorPage, CPropertyPage)
	//{{AFX_MSG_MAP(CColorPage)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_COLOR1, OnColor1Button)
	ON_BN_CLICKED(IDC_COLOR2, OnColor2Button)
	ON_BN_CLICKED(IDC_COLOR3, OnColor3Button)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPage message handlers

void CColorPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(nIDCtl) {
	default:
		CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
		break;
	case IDC_COLOR1:
	case IDC_COLOR2:
	case IDC_COLOR3:
		::SendMessage(::GetDlgItem(m_hWnd, nIDCtl), WM_DRAWITEM,
			(WPARAM)nIDCtl, (LPARAM)lpDrawItemStruct);
		break;
	}
}

void CColorPage::OnColor1Button()
{
	::OnColorButton(m_pHostInfo, this, IDC_COLOR1, &m_Color1);
}

void CColorPage::OnColor2Button()
{
	::OnColorButton(m_pHostInfo, this, IDC_COLOR2, &m_Color2);
}

void CColorPage::OnColor3Button()
{
	::OnColorButton(m_pHostInfo, this, IDC_COLOR3, &m_Color3);
}

/////////////////////////////////////////////////////////////////////////////
// CInfoPage property page

IMPLEMENT_DYNCREATE(CInfoPage, CPropertyPage)

CInfoPage::CInfoPage() : CPropertyPage(CInfoPage::IDD)
{
	//{{AFX_DATA_INIT(CInfoPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CInfoPage::~CInfoPage()
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void CInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CInfoPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoPage message handlers

BOOL CInfoPage::OnInitDialog()
{
	extern CTextureApp theApp;

	HGLOBAL	hInfoRsc;
	char	*pszInfo;
	HRSRC hRsc;

	CPropertyPage::OnInitDialog();

	hRsc = FindResource(theApp.m_hInstance, MAKEINTRESOURCE(IDS_INFOTEXT), "TEXT");
	if (!hRsc)
	{
		AfxMessageBox("Error loading info text");
		EndDialog(IDCANCEL);
	}

	hInfoRsc = LoadResource(theApp.m_hInstance,hRsc);
	if (!hInfoRsc)
	{
		AfxMessageBox("Error loading info text");
		EndDialog(IDCANCEL);
	}

	pszInfo = (char *)LockResource(hInfoRsc);
	if (!pszInfo)
	{
		FreeResource(hInfoRsc);
		AfxMessageBox("Error locking info text");
		EndDialog(IDCANCEL);
	}

	try
	{
		GetDlgItem(IDC_EDIT)->SetWindowText(pszInfo);
	}
	catch(...)
	{
		AfxMessageBox("Error setting info text");
	}

	UnlockResource(hInfoRsc);
	FreeResource(hInfoRsc);

	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CSizePage property page

IMPLEMENT_DYNCREATE(CSizePage, CPropertyPage)

CSizePage::CSizePage() : CPropertyPage(CSizePage::IDD)
{
	//{{AFX_DATA_INIT(CSizePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSizePage::~CSizePage()
{
}

void CSizePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSizePage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_SSEControls(pDX, IDC_XSIZE_EDIT,
		IDC_XSIZE_SPIN, IDC_XSIZE_SLIDE, m_XSizeCtrls);
	DDX_SSEData(pDX, m_XSizeCtrls, 0, 16384, 0.0, 1.0, m_fXSize);

	DDX_SSEControls(pDX, IDC_YSIZE_EDIT,
		IDC_YSIZE_SPIN, IDC_YSIZE_SLIDE, m_YSizeCtrls);
	DDX_SSEData(pDX, m_YSizeCtrls, 0, 16384, 0.0, 1.0, m_fYSize);

	DDX_SSEControls(pDX, IDC_ZSIZE_EDIT,
		IDC_ZSIZE_SPIN, IDC_ZSIZE_SLIDE, m_ZSizeCtrls);
	DDX_SSEData(pDX, m_ZSizeCtrls, 0, 16384, 0.0, 1.0, m_fZSize);
}

BEGIN_MESSAGE_MAP(CSizePage, CPropertyPage)
	//{{AFX_MSG_MAP(CSizePage)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_EN_UPDATE(IDC_XSIZE_EDIT, OnUpdateEditCtrl)
	ON_EN_UPDATE(IDC_YSIZE_EDIT, OnUpdateEditCtrl)
	ON_EN_UPDATE(IDC_ZSIZE_EDIT, OnUpdateEditCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSizePage message handlers

void CSizePage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_XSizeCtrls.IsCtrlMember(pScrollBar)) {
		m_XSizeCtrls.OnPosUpdate(pScrollBar);
		return;
	}
	if (m_YSizeCtrls.IsCtrlMember(pScrollBar)) {
		m_YSizeCtrls.OnPosUpdate(pScrollBar);
		return;
	}
	if (m_ZSizeCtrls.IsCtrlMember(pScrollBar)) {
		m_ZSizeCtrls.OnPosUpdate(pScrollBar);
		return;
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSizePage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_XSizeCtrls.IsCtrlMember(pScrollBar)) {
		m_XSizeCtrls.OnPosUpdate(pScrollBar);
		return;
	}
	if (m_YSizeCtrls.IsCtrlMember(pScrollBar)) {
		m_YSizeCtrls.OnPosUpdate(pScrollBar);
		return;
	}
	if (m_ZSizeCtrls.IsCtrlMember(pScrollBar)) {
		m_ZSizeCtrls.OnPosUpdate(pScrollBar);
		return;
	}

	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSizePage::OnUpdateEditCtrl()
{
	SetModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CControlsPage property page

IMPLEMENT_DYNCREATE(CControlsPage, CPropertyPage)

CControlsPage::CControlsPage() : CPropertyPage(CControlsPage::IDD)
{
	//{{AFX_DATA_INIT(CControlsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CControlsPage::~CControlsPage()
{
}

void CControlsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_SSEControls(pDX, IDC_BLEND1_EDIT,
		0, IDC_BLEND1_SLIDE, m_Blend1Ctrls);
	DDX_SSEData(pDX, m_Blend1Ctrls, 0, 100, 0.0, 0.01, m_fBlend1);

	DDX_SSEControls(pDX, IDC_BLEND2_EDIT,
		0, IDC_BLEND2_SLIDE, m_Blend2Ctrls);
	DDX_SSEData(pDX, m_Blend2Ctrls, 0, 100, 0.0, 0.01, m_fBlend2);

	DDX_SSEControls(pDX, IDC_BLEND3_EDIT,
		0, IDC_BLEND3_SLIDE, m_Blend3Ctrls);
	DDX_SSEData(pDX, m_Blend3Ctrls, 0, 100, 0.0, 0.01, m_fBlend3);

	DDX_SSEControls(pDX, IDC_BLEND4_EDIT,
		0, IDC_BLEND4_SLIDE, m_Blend4Ctrls);
	DDX_SSEData(pDX, m_Blend4Ctrls, 0, 100, 0.0, 0.01, m_fBlend4);
}

BEGIN_MESSAGE_MAP(CControlsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CControlsPage)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_EN_UPDATE(IDC_BLEND1_EDIT, OnUpdateEditCtrl)
	ON_EN_UPDATE(IDC_BLEND2_EDIT, OnUpdateEditCtrl)
	ON_EN_UPDATE(IDC_BLEND3_EDIT, OnUpdateEditCtrl)
	ON_EN_UPDATE(IDC_BLEND4_EDIT, OnUpdateEditCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSizePage message handlers

void CControlsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_Blend1Ctrls.IsCtrlMember(pScrollBar)) {
		m_Blend1Ctrls.OnPosUpdate(pScrollBar);
		return;
	}
	if (m_Blend2Ctrls.IsCtrlMember(pScrollBar)) {
		m_Blend2Ctrls.OnPosUpdate(pScrollBar);
		return;
	}
	if (m_Blend3Ctrls.IsCtrlMember(pScrollBar)) {
		m_Blend3Ctrls.OnPosUpdate(pScrollBar);
		return;
	}
	if (m_Blend4Ctrls.IsCtrlMember(pScrollBar)) {
		m_Blend4Ctrls.OnPosUpdate(pScrollBar);
		return;
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CControlsPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_Blend1Ctrls.IsCtrlMember(pScrollBar)) {
		m_Blend1Ctrls.OnPosUpdate(pScrollBar);
		return;
	}
	if (m_Blend2Ctrls.IsCtrlMember(pScrollBar)) {
		m_Blend2Ctrls.OnPosUpdate(pScrollBar);
		return;
	}
	if (m_Blend3Ctrls.IsCtrlMember(pScrollBar)) {
		m_Blend3Ctrls.OnPosUpdate(pScrollBar);
		return;
	}
	if (m_Blend4Ctrls.IsCtrlMember(pScrollBar)) {
		m_Blend4Ctrls.OnPosUpdate(pScrollBar);
		return;
	}

	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CControlsPage::OnUpdateEditCtrl()
{
	SetModified(TRUE);
}
