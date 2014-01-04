/***************
 * MODULE:			raystorm.cpp
 * PROJECT:			RayStorm
 * DESCRIPTION:	Raystorm prototypes
 * AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 * --------------------------------------------------------------------
 *		06.09.95	mh		transfer of defines from main prog
 *		27.02.96	mh		STATUS, RAYCOMMAND transferred of .cpp
 *		18.03.96	mh		MFC - Version!
 *		02.06.96	mh		RSI
 *		06.10.96 mh		removed workingdirectory
 *		28.04.96 mh		bugfix with ViewerDlg
 ***************/

#include "stdafx.h"
#include <direct.h>
#include "raystorm.h"
#include "typedefs.h"

// globals
CTheApp NEAR TheApp;

BEGIN_MESSAGE_MAP(CTheApp, CWinApp)
	ON_COMMAND(ID_APP_HELP, CPadView::OnHelp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)     // file commands...
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


BOOL CTheApp::InitInstance()
{
	Enable3dControls();
	SetDialogBkColor();
	LoadStdProfileSettings();

	// start RSI
	if (rsiInit())
		return FALSE;
	
	//rsiGetUser(&user);

	AddDocTemplate
	(
		new CMultiDocTemplate(IDR_TEXTTYPE,
		RUNTIME_CLASS(CPadDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CPadView))
	);
	m_pMainWnd = new CMainFrame;
	((CFrameWnd*)m_pMainWnd)->LoadFrame(IDR_MAINFRAME);
	m_pMainWnd->ShowWindow(m_nCmdShow);

	// enable file manager drag/drop and DDE Execute open
	m_pMainWnd->DragAcceptFiles();
	EnableShellOpen();
	RegisterShellFileTypes();

#ifndef _MAC
	// open file specified on command line
	if (m_lpCmdLine[0])
		OpenDocumentFile(m_lpCmdLine);
#endif
		
	return TRUE;
}

BOOL CTheApp::ExitInstance()
{
	return TRUE;
}

void CTheApp::OnAppAbout()
{
	char szBuf[150], *szUsername;
	int sernum;

	if (0/*user.name*/)
	{
		sprintf(szBuf, "   RayStorm Script © 1995/98 by\nAndreas Heumann & Mike Hesser\n\nRegistered for %s\nSerial number: %d\nThank you for purchasing RayStorm!", szUsername, sernum);
		MessageBox(NULL, szBuf, "About", MB_OK);
	}
	else
		MessageBox(NULL, "        RayStorm © 1995/98 by\nAndreas Heumann & Mike Hesser\n\nUnregistered Version!", "About", MB_OK);

}


IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT buttons[] =
{
	ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_SEPARATOR,
	ID_EDIT_CUT, ID_EDIT_COPY, ID_EDIT_PASTE, ID_SEPARATOR,
	ID_ACTION_RENDER, ID_ACTION_DISPLAY, ID_APP_ABOUT
};

static UINT indicators[] =
{
	ID_SEPARATOR, ID_INDICATOR_CAPS, ID_INDICATOR_NUM, ID_INDICATOR_SCRL
};

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	((m_ToolBar.Create(this) &&
		m_ToolBar.LoadBitmap(IDR_MAINFRAME) &&
		m_ToolBar.SetButtons(buttons, sizeof(buttons)/sizeof(UINT)) &&
		m_StatusBar.Create(this) &&
		m_StatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	  ? 0 : -1);

	CWnd::GetMenu()->EnableMenuItem(ID_FILE_OPEN, MF_GRAYED);
	return CMDIFrameWnd::OnCreate(lpCreateStruct);
}


// PadView
IMPLEMENT_DYNCREATE(CPadView, CEditView)
BEGIN_MESSAGE_MAP(CPadView, CEditView)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_ACTION_RENDER, OnAppRender)
	ON_COMMAND(ID_ACTION_DISPLAY, OnAppDisplay)

	ON_UPDATE_COMMAND_UI(ID_ACTION_RENDER, CheckAccess)
	ON_UPDATE_COMMAND_UI(ID_ACTION_DISPLAY, OnDisplayUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, CheckAccess)
END_MESSAGE_MAP()

CPadView::CPadView()
{
	pRenderOutput = NULL;
	hThread = NULL;

	ViewerDlg.Create(IDD_DISPLAY, this);
	ViewerDlg.pView = this;
	bViewerVisible = FALSE;
}

CPadView::~CPadView()
{
	if (pRenderOutput)
		delete pRenderOutput;

	ParserCleanup();

	ViewerDlg.DestroyWindow();
}

void CPadView::CheckAccess(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(hThread == NULL);
}

void CPadView::OnHelp()
{
	BOOL res;
	PROCESS_INFORMATION prcInfo;
	STARTUPINFO stInfo;
	char buf[256];

	GetStartupInfo(&stInfo);

	stInfo.lpTitle = "Help";

	res = CreateProcess(NULL, "sherpa.exe",
							   NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL,
								&stInfo, &prcInfo);

	sprintf(buf, "Could not start help system (%i)", GetLastError());

	if (!res)
		::MessageBox(NULL, buf, "Error", MB_OK);
}

void CPadView::OnAppRender()
{
	BOOL bWaitResult;
	UBYTE *p;
	int xRes, yRes;
	char *szProg;
	MSG msg;

	CRenderOutput RenderOutput;
	if (!pRenderOutput)
	{
		pRenderOutput = new CRenderOutput();		
		pRenderOutput->Create(IDD_RENDER);
		pRenderOutput->CenterWindow();
	}
	if (pRenderOutput && !pRenderOutput->IsWindowVisible())
		pRenderOutput->ShowWindow(SW_SHOW);

	szProg = (char *)LockBuffer();
	hThread = pRenderOutput->Start(szProg);
	if (hThread)
	{
		do
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			bWaitResult = WaitForSingleObject (hThread, 0);
		}
		while (bWaitResult != WAIT_OBJECT_0);
	}

	CloseHandle(hThread);
	hThread = NULL;

	UnlockBuffer();
	pRenderOutput->End();
	
	if (ViewerDlg.IsWindowVisible())
	{
		if (GetPictureInfo(&p, &xRes, &yRes))
			ViewerDlg.Update(p, xRes, yRes);
	}
//	rsiCleanup();
}

void CPadView::OnDisplayUI(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(bViewerVisible);
}

void CPadView::OnAppDisplay()
{
	int xRes, yRes;
	UBYTE *p;

	if (bViewerVisible)
		ViewerDlg.ShowWindow(SW_HIDE);
	else
	{
		ViewerDlg.ShowWindow(SW_SHOW);
		GetPictureInfo(&p, &xRes, &yRes);
		if (p)
		{
			ViewerDlg.SetParams(IDX_ORDERED, FALSE, FALSE); 
			ViewerDlg.Update(p, xRes, yRes);
		}
	}

	bViewerVisible = !bViewerVisible;	

}

int CPadView::OnCreate(LPCREATESTRUCT lpcs)
{
	LOGFONT lf;
	if (CEditView::OnCreate(lpcs) != 0)
		return -1;

	GetObject(GetStockObject(OEM_FIXED_FONT), sizeof(LOGFONT), &lf);
	SetTabStops(lf.lfWidth);

	Font.CreateFontIndirect(&lf);
	SetFont(&Font);
	return 0;
}


// PadDoc
IMPLEMENT_DYNCREATE(CPadDoc, CDocument)
BEGIN_MESSAGE_MAP(CPadDoc, CDocument)
END_MESSAGE_MAP()

void CPadDoc::Serialize(CArchive& ar)
{
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}
