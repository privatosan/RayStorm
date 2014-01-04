/***************
 * NAME:				PrimDlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 13.05.1997
 * DESCRIPTION:	attribute dialogs for basic primitives
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						13.05.1997	mh		initial version
 ***************/

#ifndef PRIMDLG_H
#define PRIMDLG_H

#include "typedefs.h"
#include "resource.h"
#include "object.h"

class CPrimSetPage : public CPropertyPage
{
public:
	CPrimSetPage(); 
	enum { IDD = IDD_PRIMSET };

	BOOL m_bClosedTop, m_bClosedBottom;
	int m_nSORAccurate;
	OBJECT *m_pObject;

protected:
	virtual BOOL OnInitDialog();	
	virtual void DoDataExchange(CDataExchange*);

	DECLARE_MESSAGE_MAP()
};

#endif
