/***************
 * MODULE:			Mainfrm.cpp
 * PROJECT:			Scenario
 * VERSION:       1.0 29.08.1996
 * DESCRIPTION:   implementation of mainframe class
 *	AUTHORS:			Mike Hesser
 * HISTORY:
 *						DATE		NAME	COMMENT
 *						29.08.96	mh		added comment
 *						22.09.97 mh		Minimum size of window (GetMinMaxInfo)
 ***************/

#include "Typedefs.h"
#include "Scenario.h"
#include "Mainfrm.h"
#include "CamView.h"
#include "MaterialView.h"
#include "Coldef.h"
#include "Prefs.h"
#include "Resource.h"
#include "Fileutil.h"
#include "Object.h"
#include "Sci.h"

extern char szWorkingDirectory[256];

#define WINDOW_MIN_X 300
#define WINDOW_MIN_Y 250

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// status line indicators
static UINT BASED_CODE anIndicatorIDs[] =
{
	ID_SEPARATOR,
	ID_STATUSBAR_OBJECT,
	ID_STATUSBAR_OBJECTTXT,
	ID_STATUSBAR_CAMERA,
	ID_STATUSBAR_CAMERATXT,
	ID_STATUSBAR_COORDX,
	ID_COORD_X,
	ID_STATUSBAR_COORDY,
	ID_COORD_Y,
	ID_STATUSBAR_COORDZ,
	ID_COORD_Z,
};

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE anButtonIDs[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
		ID_SEPARATOR,
	IDM_EDITPOINTS,
	IDM_ADDPOINTS,
	IDM_EDITEDGES,
	IDM_ADDEDGES,
	IDM_EDITFACES,
	IDM_ADDFACES,
		ID_SEPARATOR,
	IDM_FRONT,
	IDM_RIGHT,
	IDM_TOP,
	IDM_PERSPECTIVE,
	IDM_CAMERAVIEW,
	IDM_4T,
		ID_SEPARATOR,
	IDM_BOUNDING,
	IDM_WIRE,
	IDM_SOLID,
		ID_SEPARATOR,
	IDM_LOCAL,
		ID_SEPARATOR,
	IDM_MOVEOBJECT,
	IDM_ROTATEOBJECT,
	IDM_SIZEOBJECT,
	IDM_MOVEWORLD,
	IDM_ROTATEWORLD,
	IDM_ZOOMWORLD,
		ID_SEPARATOR,
	IDM_SELECT,
		ID_SEPARATOR,
	IDM_FILTERX,
	IDM_FILTERY,
	IDM_FILTERZ
/*
		ID_SEPARATOR,
	ID_VIEW_RENDER*/
};

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_OBJECT, OnUpdatePanes)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_OBJECTTXT, OnUpdatePanes)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_CAMERA, OnUpdatePanes)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_CAMERATXT, OnUpdatePanes)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_COORDX, OnUpdatePanes)
	ON_UPDATE_COMMAND_UI(ID_COORD_X, OnUpdatePanes)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_COORDY, OnUpdatePanes)
	ON_UPDATE_COMMAND_UI(ID_COORD_Y, OnUpdatePanes)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_COORDZ, OnUpdatePanes)
	ON_UPDATE_COMMAND_UI(ID_COORD_Z, OnUpdatePanes)
	ON_UPDATE_COMMAND_UI(IDM_BROWSER_ONOFF, OnBrowserOnOffUI)
	ON_UPDATE_COMMAND_UI(IDM_MATERIAL_ONOFF, OnMaterialOnOffUI)
	ON_COMMAND(IDM_LOADPREFS, OnLoadPreferences)
	ON_COMMAND(IDM_SAVEPREFS, OnSavePreferences)
	ON_COMMAND(IDM_MATERIAL_ONOFF, OnMaterialOnOff)
	ON_COMMAND(IDM_BROWSER_ONOFF, OnBrowserOnOff)
	ON_WM_GETMINMAXINFO()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CMainFrame::CMainFrame()
{
	InitColorObjects();
	sizeOld.cx = sizeOld.cy = 0;
	m_bBrowserPane = m_bMaterialPane = true;
}

/*************
 * DESCRIPTION:	destructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CMainFrame::~CMainFrame()
{
	DeleteColorObjects();
	FreeObjects();
	FreePrefs();
}

BOOL CMainFrame::Create(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, 
	CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	LPCTSTR m_lpszClassName;

  	m_lpszClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,
		LoadCursor(NULL, IDC_ARROW), 
		(HBRUSH)(COLOR_WINDOW+1), NULL);

	// TODO: Add your specialized code here and/or call the base class
	return CreateEx(dwExStyle, m_lpszClassName, _T(""), dwStyle, 
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 
		pParentWnd->GetSafeHwnd(), NULL, NULL );
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CRect rect;

	if (!m_wndSplitterV.CreateStatic(this, 2, 1))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	if (!m_wndSplitterV.CreateView(1, 0, RUNTIME_CLASS(CMatView), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	GetClientRect(rect);

	// add the second splitter pane - which is a nested splitter with 2 rows
	if (!m_wndSplitterH.CreateStatic(
		&m_wndSplitterV,     // our parent window is the first splitter
		1, 2,               // the new splitter is 2 rows, 1 columns
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitterV.IdFromRowCol(0, 0)
			// new splitter is in the first row, 2nd column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}

	// add the first splitter pane - the default view in column 0
	if (!m_wndSplitterH.CreateView(0, 0, RUNTIME_CLASS(CCamView), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// now create the two views inside the nested splitter
	if (!m_wndSplitterH.CreateView(0, 1,
		RUNTIME_CLASS(CBrowserView), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create third pane\n");
		return FALSE;
	}

	// activate the input view
	SetActiveView((CView*)m_wndSplitterH.GetPane(0, 0));

	((CCamView*)m_wndSplitterH.GetPane(0, 0))->pBrowserView = (CBrowserView*)m_wndSplitterH.GetPane(0, 1);
	((CBrowserView*)m_wndSplitterH.GetPane(0, 1))->pCamView = (CCamView*)m_wndSplitterH.GetPane(0, 0);
	((CCamView*)m_wndSplitterH.GetPane(0, 0))->pMatView = (CMatView*)m_wndSplitterV.GetPane(1, 0);

	((CMatView*)m_wndSplitterV.GetPane(1, 0))->m_pPalette = &m_cPalette;

	return TRUE;
}

static TCHAR BASED_CODE szSection[] = _T("Settings");
static TCHAR BASED_CODE szWindowPos[] = _T("WindowPos");
static TCHAR szFormat[] = _T("%u,%u,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d");

/*************
 * DESCRIPTION:	read window placement from INI file
 * INPUT:			system
 * OUTPUT:			-
 *************/
static BOOL PASCAL NEAR ReadWindowPlacement(LPWINDOWPLACEMENT pwp, CSize *psizeSplitter)
{
	WINDOWPLACEMENT wp;
	CSize sizeSplitter;

	CString strBuffer = AfxGetApp()->GetProfileString(szSection, szWindowPos);
	if (strBuffer.IsEmpty())
		return FALSE;

	int nRead = _stscanf(strBuffer, szFormat,
		&wp.flags, &wp.showCmd,
		&wp.ptMinPosition.x, &wp.ptMinPosition.y,
		&wp.ptMaxPosition.x, &wp.ptMaxPosition.y,
		&wp.rcNormalPosition.left, &wp.rcNormalPosition.top,
		&wp.rcNormalPosition.right, &wp.rcNormalPosition.bottom,
		&sizeSplitter.cx, &sizeSplitter.cy);

	if (nRead != 12)
		return FALSE;

	wp.length = sizeof wp;
	*pwp = wp;
	*psizeSplitter = sizeSplitter;

	return TRUE;
}

/*************
 * DESCRIPTION:	save window placement in INI file
 * INPUT:			system
 * OUTPUT:			-
 *************/
static void PASCAL NEAR WriteWindowPlacement(LPWINDOWPLACEMENT pwp, CSize *psizeSplitter)
	// write a window placement to settings section of app's ini file
{
	TCHAR szBuffer[sizeof("-32767")*8 + sizeof("65535")*2];

	wsprintf(szBuffer, szFormat,
				pwp->flags, pwp->showCmd,
				pwp->ptMinPosition.x, pwp->ptMinPosition.y,
				pwp->ptMaxPosition.x, pwp->ptMaxPosition.y,
				pwp->rcNormalPosition.left, pwp->rcNormalPosition.top,
				pwp->rcNormalPosition.right, pwp->rcNormalPosition.bottom,
				psizeSplitter->cx, psizeSplitter->cy);
	AfxGetApp()->WriteProfileString(szSection, szWindowPos, szBuffer);
}

/*************
 * DESCRIPTION:	load window placement set window
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CMainFrame::InitialShowWindow(UINT nCmdShow)
{
	WINDOWPLACEMENT wp;
	CSize sizeSplitter;
	CRect rect;
	
	((CCamView*)m_wndSplitterH.GetPane(0, 0))->bLockOnSizeRedraw = FALSE;

	if (!ReadWindowPlacement(&wp, &sizeSplitter))
	{
		GetClientRect(rect);
		sizeSplitter.cx = rect.right - 100;
		sizeSplitter.cy = rect.bottom - 130;
	}
	else
	{
		SetWindowPlacement(&wp);
		GetClientRect(rect);
	}

	wp.showCmd = nCmdShow;

	m_wndSplitterH.SetColumnInfo(0, sizeSplitter.cx, 0);
	m_wndSplitterV.SetRowInfo(0, sizeSplitter.cy, 0);
	// do the recalc now, otherwise the Splitter-windows would "forget" it
	m_wndSplitterV.RecalcLayout();
	m_wndSplitterH.RecalcLayout();

	sizeOld.cx = rect.Width();
	sizeOld.cy = rect.Height();

	ShowWindow(wp.showCmd);
}

/*************
 * DESCRIPTION:	prevent window from getting too small
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = WINDOW_MIN_X;
	lpMMI->ptMinTrackSize.y = WINDOW_MIN_Y;
	
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

/*************
 * DESCRIPTION:	save the position of the window
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMainFrame::OnClose()
{
	WINDOWPLACEMENT wp;
	int cx, cy, n;

	wp.length = sizeof wp;
	if (GetWindowPlacement(&wp))
	{
		wp.flags = 0;
		if (IsZoomed())
			wp.flags |= WPF_RESTORETOMAXIMIZED;

		m_wndSplitterH.GetColumnInfo(0, cx, n);
		m_wndSplitterV.GetRowInfo(0, cy, n);
		// and write it to the .INI file
		WriteWindowPlacement(&wp, &CSize(cx, cy));
	}

	CFrameWnd::OnClose();
}

/*************
 * DESCRIPTION:	resize the splitter windows
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	int x, y, h;

	if (nType != SIZE_MINIMIZED && IsWindow(m_wndSplitterH.m_hWnd))
	{
		m_wndSplitterH.GetColumnInfo(0, x, h);
		m_wndSplitterV.GetRowInfo(0, y, h);

		// make sure that column and row don't get below zero
		if (cx - sizeOld.cx + x < 0)
			cx = sizeOld.cx;
		if (cy - sizeOld.cy + y < 0)
			cy = sizeOld.cy;

		m_wndSplitterH.SetColumnInfo(0, cx - sizeOld.cx + x, 0);
		m_wndSplitterV.SetRowInfo(0, cy - sizeOld.cy + y, 0);
		sizeOld.cx = cx;
		sizeOld.cy = cy;
	}

	CFrameWnd::OnSize(nType, cx, cy);
}
	
/*************
 * DESCRIPTION:	enable status bar
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CMainFrame::OnUpdatePanes(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

/*************
 * DESCRIPTION:	create window
 * INPUT:			system
 * OUTPUT:			-
 *************/
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	PREFS prefs;

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set window size and position
	InitialShowWindow(SW_SHOW);

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(anButtonIDs,
		  sizeof(anButtonIDs)/sizeof(UINT)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(anIndicatorIDs,
		  sizeof(anIndicatorIDs)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;
	}

	OnLoadPreferences();

	prefs.id = ID_PRJP;
	if (prefs.GetPrefs())
		SetCurrentDirectory((char*)prefs.data);
	prefs.data = NULL;

	// description panes have no 3D border
	m_wndStatusBar.SetPaneStyle(m_wndStatusBar.CommandToIndex(ID_STATUSBAR_CAMERA), SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneStyle(m_wndStatusBar.CommandToIndex(ID_STATUSBAR_OBJECT), SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneStyle(m_wndStatusBar.CommandToIndex(ID_STATUSBAR_COORDX), SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneStyle(m_wndStatusBar.CommandToIndex(ID_STATUSBAR_COORDY), SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneStyle(m_wndStatusBar.CommandToIndex(ID_STATUSBAR_COORDZ), SBPS_NOBORDERS);

	// set the width of current camera pane and selected object pane
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_STATUSBAR_OBJECTTXT), 
										ID_STATUSBAR_OBJECTTXT, SBPS_NORMAL, 100);
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_STATUSBAR_CAMERATXT), 
										ID_STATUSBAR_CAMERATXT, SBPS_NORMAL, 100);

	UINT nID, nStyle;
	int cxWidth;
	m_wndStatusBar.GetPaneInfo(0, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo(0, nID, SBPS_STRETCH | SBPS_NORMAL, 150);

	return 0;
}

/*************
 * DESCRIPTION:	load preferences
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMainFrame::OnLoadPreferences()
{
	if (!sciLoadPrefs())
		OnSavePreferences();
}

/*************
 * DESCRIPTION:	save preferences
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMainFrame::OnSavePreferences()
{
	if (!sciSavePrefs())
		utility.Request(IDS_ERRPREFSSAVE);
}

/*************
 * DESCRIPTION:	Show or hide material manager pane
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMainFrame::OnMaterialOnOff()
{
	m_bMaterialPane = !m_bMaterialPane;

	if (m_bMaterialPane)
		m_wndSplitterV.ShowRow();
	else
		m_wndSplitterV.HideRow(1);
}

/*************
 * DESCRIPTION:	Show or hide browser pane
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMainFrame::OnBrowserOnOff()
{
	m_bBrowserPane = !m_bBrowserPane;

	if (m_bBrowserPane)
		m_wndSplitterH.ShowColumn();
	else
		m_wndSplitterH.HideColumn(1);
}

	
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif
