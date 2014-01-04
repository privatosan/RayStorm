/***************
 * NAME:				scenario.h
 * PROJECT:			Scenario
 * VERSION:			0.6 30.08.1996
 * DESCRIPTION:	definition file for the Main Application
 * AUTHORS:			Mike Hesser
 * BUGS:          none
 * TO DO:         -
 * HISTORY:			DATE			NAME	COMMENT
 *						22.09.1996	mh		comment title
 ***************/

#include "resource.h"
#include "rsi.h"

class CTheApp : public CWinApp
{
public:
	CTheApp();
	~CTheApp();

	virtual BOOL InitInstance();
	afx_msg void OnAppAbout();
	afx_msg void OnContents();
	
	DECLARE_MESSAGE_MAP()
};


