/***************
 * MODULE:			prefdlg.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	setting of preferences
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE		NAME	COMMENT
 *						07.09.96	mh		comment
 *						21.09.96	mh		took out some unnecessary things
 *						10.12.96	mh		this dialog is exemplary now
 ***************/

#include "typedefs.h"
#include "prefs.h"
#include "camview.h"
#include "prefdlg.h"
#include "document.h"
#include "resource.h"

/*************
 * DESCRIPTION:	start pref dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnPrefSet()
{
	CPrefSheet dialog(this);
	PREFS prefs;
	int datas;

	prefs.id = ID_VIEW;
	if (prefs.GetPrefs())
		dialog.m_pgPath.m_sViewerPath = (char *)prefs.data;
	prefs.id = ID_PRJP;
	if (prefs.GetPrefs())
		dialog.m_pgPath.m_sProjectPath = (char *)prefs.data;
	prefs.id = ID_OBJP;
	if (prefs.GetPrefs())
		dialog.m_pgPath.m_sObjectPath = (char *)prefs.data;
	prefs.id = ID_BRSP;
	if (prefs.GetPrefs())
		dialog.m_pgPath.m_sBrushPath = (char *)prefs.data;
	prefs.id = ID_MATP;
	if (prefs.GetPrefs())
		dialog.m_pgPath.m_sMaterialPath = (char *)prefs.data;
	prefs.id = ID_TXTP;
	if (prefs.GetPrefs())
		dialog.m_pgPath.m_sTexturePath = (char *)prefs.data;
	prefs.id = ID_FLGS;
	if (prefs.GetPrefs())
		dialog.m_pgFlag.m_bShowCoord = (*((ULONG*)prefs.data) & PREFFLAG_SHOWCOORD) ? TRUE:FALSE;
	prefs.id = ID_UNDO;
	if (prefs.GetPrefs())
		dialog.m_pgMisc.m_nMemory = (float)(*(ULONG*)(prefs.data))*0.001;
	else
		dialog.m_pgMisc.m_nMemory = 100;
	prefs.id = ID_JPEG;
	if (prefs.GetPrefs())
		dialog.m_pgMisc.m_nJPEG = (float)(*(ULONG*)(prefs.data));
	else
		dialog.m_pgMisc.m_nJPEG = 100;
	
	prefs.data = NULL; // VERY important !

	if (dialog.DoModal() == IDOK)
	{
		prefs.data = (void*)LPCTSTR(dialog.m_pgPath.m_sViewerPath);
		prefs.id = ID_VIEW;
		prefs.length = strlen((char*)prefs.data) + 1;
		prefs.AddPrefs();

		prefs.data = (void*)LPCTSTR(dialog.m_pgPath.m_sProjectPath);	
		prefs.id = ID_PRJP;
		prefs.length = strlen((char*)prefs.data) + 1;
		prefs.AddPrefs();

		prefs.data = (void*)LPCTSTR(dialog.m_pgPath.m_sObjectPath);
		prefs.id = ID_OBJP;
		prefs.length = strlen((char*)prefs.data) + 1;
		prefs.AddPrefs();

		prefs.data = (void*)LPCTSTR(dialog.m_pgPath.m_sBrushPath);
		prefs.id = ID_BRSP;
		prefs.length = strlen((char*)prefs.data) + 1;
		prefs.AddPrefs();

		prefs.data = (void*)LPCTSTR(dialog.m_pgPath.m_sMaterialPath);
		prefs.id = ID_MATP;
		prefs.length = strlen((char*)prefs.data) + 1;
		prefs.AddPrefs();

		prefs.data = (void*)LPCTSTR(dialog.m_pgPath.m_sTexturePath);
		prefs.id = ID_TXTP;
		prefs.length = strlen((char*)prefs.data) + 1;
		prefs.AddPrefs();

		datas = 0;
		if (dialog.m_pgFlag.m_bShowCoord)
		{
 			datas |= PREFFLAG_SHOWCOORD;
			pDisplay->always_coord = TRUE;
		}
		else
			pDisplay->always_coord = FALSE;

		prefs.data = &datas;
		prefs.id = ID_FLGS;
		prefs.length = sizeof(ULONG);
		prefs.AddPrefs();

		datas = (ULONG)(dialog.m_pgMisc.m_nMemory*1000.);
		prefs.data = &datas;
		prefs.id = ID_UNDO;
		prefs.length = sizeof(ULONG);
		prefs.AddPrefs();
		
		datas = (ULONG)(dialog.m_pgMisc.m_nJPEG);
		prefs.data = &datas;
		prefs.id = ID_JPEG;
		prefs.length = sizeof(ULONG);
		prefs.AddPrefs();

		prefs.data = NULL;
	}
}

IMPLEMENT_DYNAMIC(CPrefSheet, CPropertySheet)

/* CPrefSheet */

BEGIN_MESSAGE_MAP(CPrefSheet, CPropertySheet)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	add the pages
 * INPUT:			-
 * OUTPUT:			-
 *************/
CPrefSheet::CPrefSheet(CWnd* pWndParent) 
 : CPropertySheet("Preferences", pWndParent)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	AddPage(&m_pgPath);
	AddPage(&m_pgFlag);
	AddPage(&m_pgMisc);
}

/* CPathPage */

BEGIN_MESSAGE_MAP(CPathPage, CPropertyPage)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	add the pages
 * INPUT:			-
 * OUTPUT:			-
 *************/
CPathPage::CPathPage()
    : CPropertyPage(CPathPage::IDD)
{
}

/*************
 * DESCRIPTION:	initializes dialo
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CPathPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_fbtnViewer.Init(ID_PREFS_VIEWER, ID_PREFS_VIEWERBUT, FILEREQU_ALL, this);
	m_dbtnProject.Init(ID_PREFS_PROJECT, ID_PREFS_PROJECTBUT, this);
	m_dbtnObject.Init(ID_PREFS_OBJECT, ID_PREFS_OBJECTBUT, this);
	m_dbtnBrush.Init(ID_PREFS_BRUSH, ID_PREFS_BRUSHBUT, this);
	m_dbtnTexture.Init(ID_PREFS_TEXTURE, ID_PREFS_TEXTUREBUT, this);
	m_dbtnMaterial.Init(ID_PREFS_MATERIAL, ID_PREFS_MATERIALBUT, this);

	return TRUE;
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			pDX	given from system
 * OUTPUT:			-
 *************/
void CPathPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, ID_PREFS_VIEWER, m_sViewerPath);
	DDX_Text(pDX, ID_PREFS_OBJECT, m_sObjectPath);
	DDX_Text(pDX, ID_PREFS_PROJECT, m_sProjectPath);
	DDX_Text(pDX, ID_PREFS_BRUSH, m_sBrushPath);
	DDX_Text(pDX, ID_PREFS_TEXTURE, m_sTexturePath);
	DDX_Text(pDX, ID_PREFS_MATERIAL, m_sMaterialPath);
}

// Flags dialog
BEGIN_MESSAGE_MAP(CFlagPage, CPropertyPage)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
CFlagPage::CFlagPage()
    : CPropertyPage(CFlagPage::IDD)
{
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			pDX	given from system
 * OUTPUT:			-
 *************/
void CFlagPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, ID_FLAGS_SHOWCOORD, m_bShowCoord);
}

/* Misc dialog */

BEGIN_MESSAGE_MAP(CMiscPage, CPropertyPage)
	ON_EN_CHANGE(ID_UNDO_SLE_MEMORY, UpdateParams)
	ON_EN_CHANGE(ID_MISC_SLE_JPEG, UpdateParams)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
CMiscPage::CMiscPage()
    : CPropertyPage(CMiscPage::IDD)
{
	m_bLock = FALSE;
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CMiscPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_bLock = TRUE;

	VERIFY(m_sldMemory.Init(this, ID_UNDO_SLE_MEMORY, ID_UNDO_SLB_MEMORY, 1, 200));
	m_sldMemory.SetValue((float)m_nMemory);

	VERIFY(m_sldJPEG.Init(this, ID_MISC_SLE_JPEG, ID_MISC_SLB_JPEG, 1, 100));
	m_sldJPEG.SetValue((float)m_nJPEG);

	m_bLock = FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:	update edit box
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMiscPage::UpdateParams()
{
	if (m_bLock)
		return;

	m_bLock = TRUE;
	m_nMemory = m_sldMemory.GetValue();
	m_nJPEG = m_sldJPEG.GetValue();
	m_bLock = FALSE;
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			pDX	given from system
 * OUTPUT:			-
 *************/
void CMiscPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, ID_UNDO_SLE_MEMORY, m_nMemory);
	DDV_MinMaxInt(pDX, m_nMemory, 1, 1000);
	DDX_Text(pDX, ID_MISC_SLE_JPEG, m_nJPEG);
	DDV_MinMaxInt(pDX, m_nJPEG, 0, 100);
}
