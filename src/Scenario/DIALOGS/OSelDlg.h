/***************
 * NAME:				oseldlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	select object (for tracking)
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						08.06.1997	mh		initial release
 ***************/

#ifndef OSELDLG_H
#define OSELDLG_H

#include "resource.h"
#include "object.h"

enum
{
	BROWSER_ALL,
	BROWSER_CAMERAS,
	BROWSER_SELECTEDSURFACE
};

class CObjSelect : public CDialog
{
public:
	CObjSelect(int mode = BROWSER_ALL, CWnd *pParent = NULL); 
	enum { IDD = IDD_OBJSEL };

	OBJECT *m_pSelectedObject;

protected:
	int nMode;

	virtual BOOL OnInitDialog();
	afx_msg void OnSelchange();

	DECLARE_MESSAGE_MAP()
};

#endif