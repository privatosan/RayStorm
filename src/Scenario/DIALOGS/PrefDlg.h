/***************
 * NAME:				prefdlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	preferences settings dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						21.09.1996	mh		comment
 ***************/

#ifndef PREFDLG_H
#define PREFDLG_H 

#include "filebutt.h"
#include "slidectl.h"
#include "resource.h"

class CPathPage : public CPropertyPage
{
public:
	CPathPage(); 
	enum { IDD = IDD_PREFSET };

	CString m_sViewerPath, m_sObjectPath, m_sProjectPath, m_sBrushPath, m_sMaterialPath, m_sTexturePath;
	CFolderButton m_dbtnProject, m_dbtnObject, m_dbtnBrush, m_dbtnMaterial, m_dbtnTexture;
	CFileButton m_fbtnViewer;
	CBitmap m_Bitmap;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange*);	
	DECLARE_MESSAGE_MAP()
};

class CFlagPage : public CPropertyPage
{
public:
	CFlagPage(); 
	enum { IDD = IDD_FLAGSET };

	BOOL m_bShowCoord;

protected:
	virtual void DoDataExchange(CDataExchange*);	
	DECLARE_MESSAGE_MAP()
};

class CMiscPage : public CPropertyPage
{
public:
	CMiscPage(); 
	enum { IDD = IDD_MISCSET };

	CEditSlider m_sldMemory, m_sldJPEG;
	int m_nMemory, m_nJPEG;
	BOOL m_bLock;

	USES_EDITSLIDERS

protected:
	void UpdateParams();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange*);	
	DECLARE_MESSAGE_MAP()
};

class CPrefSheet : public CPropertySheet
{
public:
	DECLARE_DYNAMIC(CPrefSheet)
	CPrefSheet(CWnd*);

	CPathPage m_pgPath;
	CFlagPage m_pgFlag;
	CMiscPage m_pgMisc;
	
protected:
	DECLARE_MESSAGE_MAP()
};

#endif