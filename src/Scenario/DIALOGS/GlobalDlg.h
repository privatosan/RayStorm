/***************
 * NAME:				GlobalDlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	global settings dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						21.09.1996	mh		comment
 *						04.09.1997	mh		property sheet
 ***************/

#include "afxcmn.h"
#include "colbutt.h"
#include "filebutt.h"
#include "viewbutt.h"

// environment page
class CGlobalsEnvPage : public CPropertyPage
{
public:
	enum { IDD = IDD_GLOBAL_ENV };

	CGlobalsEnvPage(); 

	CString				m_sBackdrop, m_sReflmap;
	CFileButton			m_fbtnBackdrop, m_fbtnReflMap;
	CColorButton		m_cbtnBackColor, m_cbtnAmbientColor, m_cbtnFogColor;
	CViewButton			m_vbtnBackdropView, m_vbtnReflMapView;
	BOOL					m_bReflMap, m_bBackdrop;
	float					m_FogLen, m_FogHeight;

protected:
	virtual void DoDataExchange(CDataExchange*);	
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
};

// distrib page
class CGlobalsMiscPage : public CPropertyPage
{
public:
	enum { IDD = IDD_GLOBAL_MISC };

	CGlobalsMiscPage();

	CComboBox			m_cboxSoftshadow, m_cboxAntialias;
	CSpinButtonCtrl	m_spnMotion;
	CColorButton		m_cbtnContrastColor;
	int					m_nDistrib, m_nSoftshad, m_nAntialias;
	BOOL					m_bRandjit;
	float					m_Gauss;

protected:
	virtual void DoDataExchange(CDataExchange*);	
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
};

class CGlobalsSheet : public CPropertySheet
{
public:
	DECLARE_DYNAMIC(CGlobalsSheet)
	CGlobalsSheet(CWnd *);

	CGlobalsEnvPage		m_pgGlobalsEnv;
	CGlobalsMiscPage		m_pgGlobalsMisc;

protected:
	BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
