 /***************
 * NAME:				renddlg.h
 * PROJECT:			Scenario
 * VERSION:       1.0 13.09.1995
 * DESCRIPTION:   Render settings dialog definition file (CRender)
 * AUTHORS:       Mike Hesser
 * HISTORY:       DATE			NAME	COMMENT
 *						19.03.1996	mh		first release (0.1)
 *						03.09.1997	mh		Property sheet
 *						06.09.1997	mh		renamed identifiers
 ***************/

#ifndef RENDDLG_H
#define RENDDLG_H

#include "afxcmn.h"
#include "filebutt.h"
#include "viewbutt.h"
#include "picdlg.h"

class CRenderOutput;

typedef struct
{
	CRenderOutput *pRenderOutput;
	int nListBoxCounter;	
	CProgressCtrl *pProgress;
	BOOL bShow, bCancel;
} RENDERINFO;

// miscellaneous page
class CRenderMiscPage : public CPropertyPage
{
public:
	enum { IDD = IDD_RENDER_MISC };

	BOOL m_bQuick;
	int m_nOctreeDepth, m_nMinObjects;
	CSpinButtonCtrl m_spnOctreeDepth;

	CRenderMiscPage(); 

protected:
	virtual void DoDataExchange(CDataExchange*);	
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
};

// output page
class CRenderOutputPage : public CPropertyPage
{
public:
	enum { IDD = IDD_RENDER_OUTPUT };

	CComboBox m_cboxFormat;
	CFileButton m_fbtnFileName;
	CViewButton m_vbtnFileName;
	CString m_sFilename, m_sFormat;
	BOOL m_bShow;

	CRenderOutputPage(); 

protected:
	virtual void DoDataExchange(CDataExchange*);	
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
};

// resolution page
class CRenderResoPage : public CPropertyPage
{
public:
	enum { IDD = IDD_RENDER_RESO };

	CComboBox m_cboxResolution;
	int m_nXMin, m_nYMin, m_nXMax, m_nYMax, m_nXRes, m_nYRes;
	BOOL m_bEnableArea;
	char **m_aszResolutions;
	ULONG *m_anRes;
	BOOL m_bLoadedResolutions;

	CRenderResoPage();
	~CRenderResoPage();

protected:
	void OnSelResolutionBox();
	virtual void DoDataExchange(CDataExchange*);	
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
};

class CRenderSheet : public CPropertySheet
{
public:
	DECLARE_DYNAMIC(CRenderSheet)
	CRenderSheet(CWnd *);

	CRenderMiscPage m_pgRenderMisc;
	CRenderOutputPage m_pgRenderOutput;
	CRenderResoPage m_pgRenderReso;

protected:
	BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

class CRenderOutput : public CDialog
{
public:
	CRenderOutput(CWnd *pParent = NULL);
	enum { IDD = IDD_RENDERBOX };

	CListBox m_LogBox;
	CPicDlg wndPicture;
	CProgressCtrl m_Progress;
	RENDERINFO RenderInfo;
	int Start();

protected:
	afx_msg void OnShow();
	afx_msg void OnClose();
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

#endif
