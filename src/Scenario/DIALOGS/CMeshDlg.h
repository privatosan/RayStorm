/***************
 * NAME:				cmeshdlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	sphere dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						30.09.1996	mh		comment
 ***************/

#ifndef CMESHDLG_H
#define CMESHDLG_H

#include "vecmath.h"

class CCMeshDlg : public CDialog
{
public:
	CCMeshDlg(CWnd *pParent = NULL); 

	BOOL m_bClosedBottom, m_bClosedTop;
	VECTOR m_vSize;
	float m_Radius, m_Thickness, m_Height;
	int m_nDivs, m_nSlices;
	int m_nXDivs, m_nZDivs;
	int m_nType;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT);
	virtual void DoDataExchange(CDataExchange*);

	DECLARE_MESSAGE_MAP()
};

#endif
