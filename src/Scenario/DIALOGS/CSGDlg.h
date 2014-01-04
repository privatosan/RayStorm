/***************
 * NAME:				csgdlg.h
 * PROJECT:			Scenario
 * DESCRIPTION:	csg settings dialog
 * AUTHORS:			Andreas Heumann
 * HISTORY:			DATE			NAME	COMMENT
 *						29.04.1997	mh		comment
 *						13.05.1997	mh		added to my project
 ***************/

#ifndef CSGDLG_H
#define CSGDLG_H

#include "csg.h"
#include "resource.h"

// CCSGSetPage dialog
class CCSGSetPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCSGSetPage)

public:
	CCSGSetPage();
	enum { IDD = IDD_CSGSET };

	CComboBox m_cboxOperation;
	int m_nOperation;
	CSG *pCsg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

#endif