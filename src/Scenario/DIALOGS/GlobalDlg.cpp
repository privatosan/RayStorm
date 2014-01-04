/***************
 * NAME:				GlobalDlg.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	global settings dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 *						21.09.1996	mh		add ViewButton control
 *						10.05.1996	mh		Combobox for choosing antialias
 ***************/

#include "typedefs.h"
#include "CamView.h"
#include "GlobalDlg.h"
#include "globals.h"
#include "prefs.h"
#include "document.h"
#include "ddxddv.h"

extern GLOBALS global;

// resolution presets
static char *aszSamplings[] =
{
	"None",
	"2x2",
	"3x3",
	"4x4",
	"5x5",
	"6x6",
	"7x7",
	"8x8",
	NULL
};

/*************
 * DESCRIPTION:	call global dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnGlobalSet()
{
	CGlobalsSheet dialog(this);

	dialog.m_pgGlobalsEnv.m_bBackdrop = global.enableback;
	dialog.m_pgGlobalsEnv.m_bReflMap = global.enablerefl;
	dialog.m_pgGlobalsEnv.m_cbtnBackColor.col = global.backcol;
	dialog.m_pgGlobalsEnv.m_cbtnAmbientColor.col = global.ambient;
	dialog.m_pgGlobalsEnv.m_sBackdrop = global.backpic;
	dialog.m_pgGlobalsEnv.m_sReflmap = global.reflmap;
	dialog.m_pgGlobalsEnv.m_FogLen = global.flen;
	dialog.m_pgGlobalsEnv.m_FogHeight = global.fheight;
	dialog.m_pgGlobalsEnv.m_cbtnFogColor.col = global.fog;
	dialog.m_pgGlobalsMisc.m_nAntialias = global.antialias - 1;
	dialog.m_pgGlobalsMisc.m_Gauss = global.gauss;
	dialog.m_pgGlobalsMisc.m_cbtnContrastColor.col = global.anticont;
	dialog.m_pgGlobalsMisc.m_nDistrib = global.distrib;
	dialog.m_pgGlobalsMisc.m_nSoftshad = global.softshad - 1;
	dialog.m_pgGlobalsMisc.m_bRandjit = global.randjit;

	if (dialog.DoModal() == IDOK)
	{
		global.enableback = dialog.m_pgGlobalsEnv.m_bBackdrop;
		global.enablerefl = dialog.m_pgGlobalsEnv.m_bReflMap;
		global.backcol = dialog.m_pgGlobalsEnv.m_cbtnBackColor.col;
		global.ambient = dialog.m_pgGlobalsEnv.m_cbtnAmbientColor.col;

		global.distrib = dialog.m_pgGlobalsMisc.m_nDistrib;
		global.softshad = dialog.m_pgGlobalsMisc.m_nSoftshad + 1;
		global.antialias = dialog.m_pgGlobalsMisc.m_nAntialias + 1;
		global.gauss = dialog.m_pgGlobalsMisc.m_Gauss;
		global.flen = dialog.m_pgGlobalsEnv.m_FogLen;
		global.fheight = dialog.m_pgGlobalsEnv.m_FogHeight;
		global.randjit = dialog.m_pgGlobalsMisc.m_bRandjit;
		global.fog = dialog.m_pgGlobalsEnv.m_cbtnFogColor.col;
		global.anticont = dialog.m_pgGlobalsMisc.m_cbtnContrastColor.col;

		if (dialog.m_pgGlobalsEnv.m_sBackdrop.IsEmpty())
			global.SetBackPic(NULL);
		else
			global.SetBackPic((char *)LPCTSTR(dialog.m_pgGlobalsEnv.m_sBackdrop));

		if (dialog.m_pgGlobalsEnv.m_sReflmap.IsEmpty())
			global.SetReflMap(NULL);
		else
			global.SetReflMap((char *)LPCTSTR(dialog.m_pgGlobalsEnv.m_sReflmap));
	}
}

IMPLEMENT_DYNAMIC(CGlobalsSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(CGlobalsSheet, CPropertySheet)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor, add property sheets
 * INPUT:			system
 * OUTPUT:			-
 *************/
CGlobalsSheet::CGlobalsSheet(CWnd* pWndParent)
    : CPropertySheet("Global settings", pWndParent)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	AddPage(&m_pgGlobalsEnv);
	AddPage(&m_pgGlobalsMisc);
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
int CGlobalsSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	return bResult;
}

// CGlobalsEnvPage
 
BEGIN_MESSAGE_MAP(CGlobalsEnvPage, CPropertyPage)
END_MESSAGE_MAP()

CGlobalsEnvPage::CGlobalsEnvPage()
	: CPropertyPage(CGlobalsEnvPage::IDD)
{
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CGlobalsEnvPage::OnInitDialog()
{
	CDialog::OnInitDialog();
		
	VERIFY(m_cbtnBackColor.SubclassDlgItem(ID_GLOBAL_BACKCOLOR, this));
	VERIFY(m_cbtnAmbientColor.SubclassDlgItem(ID_GLOBAL_AMBIENTCOLOR, this));

	m_fbtnBackdrop.Init(ID_GLOBAL_BACKDROP, ID_GLOBAL_BACKDROPBUT, FILEREQU_BRUSH, this);
	m_vbtnBackdropView.Init(ID_GLOBAL_BACKDROP, ID_GLOBAL_VIEW_BACKDROP, this);

	m_fbtnReflMap.Init(ID_GLOBAL_REFLECTION, ID_GLOBAL_REFLECTIONBUT, FILEREQU_BRUSH, this);
	m_vbtnReflMapView.Init(ID_GLOBAL_REFLECTION, ID_GLOBAL_VIEW_REFLECT, this);

	VERIFY(m_cbtnFogColor.SubclassDlgItem(ID_GLOBAL_FOGCOLOR, this));

	return TRUE;
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			pDX	given from system
 * OUTPUT:			-
 *************/
void CGlobalsEnvPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, ID_GLOBAL_BACKDROP, m_sBackdrop);
	DDX_Text(pDX, ID_GLOBAL_REFLECTION, m_sReflmap);
	DDX_Check(pDX, IDC_GLOBAL_ENABLEBACKDROP, m_bBackdrop);
	DDX_Check(pDX, IDC_GLOBAL_ENABLEREFLMAP, m_bReflMap);
	DDX_Text(pDX, ID_GLOBAL_FOGLENGTH, m_FogLen);
	DDV_MinFloat(pDX, m_FogLen, 0.0f, FALSE);
	DDX_Text(pDX, ID_GLOBAL_FOGHEIGHT, m_FogHeight);
}

// CGlobalsMiscPage
 
BEGIN_MESSAGE_MAP(CGlobalsMiscPage, CPropertyPage)
END_MESSAGE_MAP()

CGlobalsMiscPage::CGlobalsMiscPage()
	: CPropertyPage(CGlobalsMiscPage::IDD)
{
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CGlobalsMiscPage::OnInitDialog()
{
	CDialog::OnInitDialog();
		
	VERIFY(m_cboxSoftshadow.SubclassDlgItem(ID_GLOBAL_SOFT, this));
	VERIFY(m_cboxAntialias.SubclassDlgItem(ID_GLOBAL_ANTI, this));
	VERIFY(m_cbtnContrastColor.SubclassDlgItem(ID_GLOBAL_CONTRAST, this));

	m_spnMotion.SetRange(1, 8);
	
	int i = 0;
	while (aszSamplings[i])
	{
		m_cboxSoftshadow.AddString(aszSamplings[i]);
		m_cboxAntialias.AddString(aszSamplings[i]);
		i++;
	}
	m_cboxSoftshadow.SetCurSel(m_nSoftshad);
	m_cboxAntialias.SetCurSel(m_nAntialias);

	return TRUE;
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			pDX	given from system
 * OUTPUT:			-
 *************/
void CGlobalsMiscPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_GLOBAL_MOTIONSPIN, m_spnMotion);
	DDX_Text(pDX, ID_GLOBAL_MOTION, m_nDistrib);
	DDV_MinMaxInt(pDX, m_nDistrib, 1, 8);
	DDX_CBIndex(pDX, ID_GLOBAL_SOFT, m_nSoftshad);
	DDX_Check(pDX, ID_GLOBAL_RANDJIT, m_bRandjit);
	DDX_CBIndex(pDX, ID_GLOBAL_ANTI, m_nAntialias);
	DDX_Text(pDX, ID_GLOBAL_FILTERWIDTH, m_Gauss);
	DDV_MinFloat(pDX, m_Gauss, 0.0f, FALSE);
}
