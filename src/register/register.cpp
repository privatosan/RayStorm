/***************
 * NAME:				register.cpp
 * PROJECT:			Register
 * VERSION:			1.1 02.12.1996
 * DESCRIPTION:	main module
 * AUTHORS:			Mike Hesser
 * BUGS:				none
 * TO DO:         -
 * HISTORY:
 *		DATE			NAME	COMMENT
 *		02.12.96		mh		own program
 ***************/

#include "stdafx.h"
#include "register.h"
#include "dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CRegisterApp, CWinApp)
END_MESSAGE_MAP()

CRegisterApp::CRegisterApp()
{
}

CRegisterApp theApp;

BOOL CRegisterApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	CRegisterBox dialog;
	
	dialog.DoModal();
	return FALSE;
}
