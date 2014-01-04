/***************
 * NAME:				griddlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	grid size dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						21.09.1996	mh		comment
 ***************/

#include "resource.h"

class CGridsize : public CDialog
{
public:
	CGridsize(CWnd *pParent = NULL); 
	enum { IDD = IDD_GRIDSIZE };

	float m_Gridsize;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
