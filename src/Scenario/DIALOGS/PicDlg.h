/***************
 * NAME:				picdlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	picture window, definition file
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE		NAME	COMMENT
 *						08.12.96	mh		first release (0.1)
 ***************/

#ifndef PICDLG_H
#define PICDLG_H

#include "resource.h"
#include "afxcmn.h"
#include "picctl.h"

class CPicDlg : public CDialog
{
public:
	CPicDlg(CWnd *pParent = NULL); 
	enum { IDD = IDD_PICTURE };
	
	CPicture picture;

protected:
	BOOL OnInitDialog();
	afx_msg void OnSize(UINT, int, int);
	DECLARE_MESSAGE_MAP()
};

#endif
