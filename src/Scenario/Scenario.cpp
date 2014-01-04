/***************
 * MODULE:			scenario.cpp
 * PROJECT:			Scenario
 * VERSION:       1.0 29.08.1996
 * DESCRIPTION:   mainprog
 *	AUTHORS:			Mike Hesser
 * HISTORY:
 *				DATE		NAME	COMMENT
 *				07.09.96 mh		comment
 *				08.10.97	mh		added support for new keyfile system
 ***************/

#include <direct.h>
#include "Typedefs.h"
#include "MainFrm.h"
#include "Document.h"
#include "Camview.h"
#include "Scenario.h"

#include "undo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

typedef BOOL (*KEYGETINFO)(char *, ULONG *);

char szWorkingDirectory[256];
char szUserName[256];
ULONG nUserSernum;

// create and run application
CTheApp TheApp;

BEGIN_MESSAGE_MAP(CTheApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_CONTENT, OnContents)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CTheApp::CTheApp()
{
}

/*************
 * DESCRIPTION:	destructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CTheApp::~CTheApp()
{
	FreeUndo();
	FreeObjects();
	FreePrefs();
	rsiExit();
}

/*************
 * DESCRIPTION:	do initializations for an instance
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CTheApp::InitInstance()
{
	KEYGETINFO keyGetInfo;
	HMODULE hKeyFile;
	OSVERSIONINFO OSVersionInfo;
	char szBuf[15];

	OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&OSVersionInfo))
		return FALSE;
	
	if (OSVersionInfo.dwMajorVersion < 4)
	{
		AfxMessageBox(IDS_ERR_PLATFORM);
		return FALSE;
	}

	// This is just for setting the current working directory in rsi.dll
	rsiInit();

	// Standard initialization
	// 3d controls and grey background
	Enable3dControls();
	SetDialogBkColor();

	// Drag and drop support
	AfxOleInit();

	strcpy(szBuf, "ke");
	
	// Load standard INI file options (including MRU)
	LoadStdProfileSettings();  

	getcwd(szWorkingDirectory, 256);


	// get user name and serial number out of keyfile
	strcpy(szUserName, "");
	strcat(szBuf, "yfil");

	nUserSernum = 0;

	strcat(szBuf, "e.dll");

	hKeyFile = LoadLibrary(szBuf);
	if (hKeyFile)
	{
		keyGetInfo = (KEYGETINFO)GetProcAddress(hKeyFile, (LPCSTR)2);
		keyGetInfo(szUserName, &nUserSernum);
		FreeLibrary(hKeyFile);
	}	

	// Register document templates
	CSingleDocTemplate *pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME,
					   RUNTIME_CLASS(CDoc),
					   RUNTIME_CLASS(CMainFrame),       // main SDI frame window
					   RUNTIME_CLASS(CCamView));

	AddDocTemplate(pDocTemplate);

	OnFileNew();

	return TRUE;
}

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTheApp::OnContents()
{
	PROCESS_INFORMATION processInfo;
	STARTUPINFO startupInfo;
	char szBuf[256];
	BOOL bRes;

	GetStartupInfo(&startupInfo);

	startupInfo.lpTitle = "Help";

	bRes = CreateProcess(NULL, "sherpa.exe",
			 				   NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL,
								&startupInfo, &processInfo);

	if (!bRes)
	{
		sprintf(szBuf, "Could not start help system (%i)", GetLastError());
		MessageBox(NULL, szBuf, "Error", MB_OK);
	}
}

// About Dialog

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };

protected:
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

/*************
 * DESCRIPTION:	prepare dialog and display information about registration
 * INPUT:			-
 * OUTPUT:			-
 *************/
CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	char szBuf[256];

	if (szUserName[0])
	{
		sprintf(szBuf, "Registered for %s", szUserName);
		SetDlgItemText(ID_ABOUT_LINE1, szBuf);
		sprintf(szBuf, "Serial number: %d", nUserSernum);
		SetDlgItemText(ID_ABOUT_LINE2, szBuf);
		sprintf(szBuf, "Thank you for purchasing RayStorm!");
		SetDlgItemText(ID_ABOUT_LINE3, szBuf);
	}
	else
		SetDlgItemText(ID_ABOUT_LINE2, "Unregistered Version!");

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CTheApp::OnAppAbout()
{
	CAboutDlg().DoModal();
}
