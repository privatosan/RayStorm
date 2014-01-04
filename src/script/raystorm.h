/***************
 * MODULE:		raystorm.h
 * PROJECT:		RayStorm
 * VERSION:     1.25 27.02.1996
 * DESCRIPTION:	Raystorm prototypes
 * AUTHORS:		Andreas Heumann, Mike Hesser
 * BUGS:		none
 * TO DO:		all
 * HISTORY:
 *			DATE		NAME	COMMENT
 *			06.09.95	mh		transfer of defines from main prog
 *			27.02.96	mh		STATUS, RAYCOMMAND transferred of .cpp
 *			18.03.96	mh		MFC - Version!
 ***************/

#ifndef RAYSTORM_H
#define RAYSTORM_H

#include <afxext.h>
#include "resource.h"

#include "render.h"
#include "viewimag.h"
#include "rsi.h"
#include "parser.h"

class CTheApp : public CWinApp
{
protected:
	BOOL InitInstance();
	BOOL ExitInstance();
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNCREATE(CMainFrame)
	CStatusBar  m_StatusBar;
	CToolBar    m_ToolBar;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

class CPadView : public CEditView
{
	DECLARE_DYNCREATE(CPadView)

	CPadView();
	~CPadView();	

	CRenderOutput *pRenderOutput;
	CViewImage ViewerDlg;
	CFont Font;
	HANDLE hThread;
	BOOL bViewerVisible;
		
public:
	void CheckAccess(CCmdUI*);
	afx_msg void OnDisplayUI(CCmdUI *);
	afx_msg void OnHelp();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnAppRender();
	afx_msg void OnAppDisplay();

	DECLARE_MESSAGE_MAP()
};

class CPadDoc : public CDocument
{
	DECLARE_DYNCREATE(CPadDoc)

public:

	void Serialize(CArchive& ar);

	DECLARE_MESSAGE_MAP()
};

#endif
