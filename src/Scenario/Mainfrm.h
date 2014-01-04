/***************
 * MODULE:			mainfrm.h
 * PROJECT:			Scenario
 * DESCRIPTION:	definition file for mainframe
 * AUTHORS:			Mike Hesser, Andreas Heumann
 * HISTORY:       
 *				DATE		NAME	COMMENT
 *				13.09.96	mh		comment
 ***************/

#include "SplitEx.h"

class CMainFrame : public CFrameWnd
{
protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Implementation
public:
	virtual ~CMainFrame();
	virtual BOOL Create(DWORD, DWORD, const RECT &, CWnd *, UINT, CCreateContext *);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &) const;
#endif

	CStatusBar	m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CSplitterWndEx m_wndSplitterH, m_wndSplitterV;
	CPalette		m_cPalette;
	CSize			sizeOld;
	bool			m_bMaterialPane, m_bBrowserPane;
	
	void InitialShowWindow(UINT);
	virtual BOOL OnCreateClient(LPCREATESTRUCT, CCreateContext*);

protected:
	afx_msg void OnLoadPreferences();
	afx_msg void OnSavePreferences();
	afx_msg void OnMaterialOnOff();
	afx_msg void OnBrowserOnOff();
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnClose();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnUpdatePanes(CCmdUI *);
	afx_msg void OnBrowserOnOffUI(CCmdUI *pUI) { pUI->SetCheck(m_bBrowserPane); }	
	afx_msg void OnMaterialOnOffUI(CCmdUI *pUI) { pUI->SetCheck(m_bMaterialPane); }	

	DECLARE_MESSAGE_MAP()
};
