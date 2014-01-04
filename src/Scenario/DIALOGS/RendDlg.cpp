/***************
 * MODULE:			renddlg.cpp
 * PROJECT:			Scenario
 * DESCRIPTION:   Render settings dialog
 * AUTHORS:       Mike Hesser
 * HISTORY:       DATE			NAME	COMMENT
 *						19.03.1996	mh		first release (0.1)
 *						22.09.1996	mh		added VIEW button to view renderd picture
 *						08.10.1997	mh		added support for new keyfile system, sciReadResFile
 ***************/

#include "typedefs.h"
#include "globals.h"
#include "camview.h"
#include "renddlg.h"
#include "document.h"
#include "resource.h"

extern GLOBALS global;

// resolution presets
static char *aszDefaultResolutions[] =
{
	"Snapshot 80x60",
	"NTSC 160x100",
	"NTSC 320x200",
	"NTSC 640x400",
	"PAL 160x128",
	"PAL 320x256",
	"PAL 640x512",
	"VGA 160x120",
	"VGA 320x240",
	"VGA 640x480",
	"SVGA 800x600",
	"SVGA 1024x768",
	"SVGA 1280x1024",
	NULL
};

static ULONG anDefaultRes[] =
{
	80,60,
	160,100,
	320,200,
	640,400,
	160,128,
	320,256,
	640,512,
	160,120,
	320,240,
	640,480,
	800,600,
	1024,768,
	1280,1024
};

// supported formats
static char *aszFormats[] =
{
	"JPEG",
	"ILBM",
	"PNG",
	"TGA",
	NULL
};

// extensions of the file format
static char *aszExtensions[] =
{
	"jpg",
	"lbm",
	"png",
	"tga",
	NULL
};

/*************
 * DESCRIPTION:	display render dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnRenderSettings()
{
	char szBuf[256];

	CRenderSheet dialog(this);

	dialog.m_pgRenderReso.m_bEnableArea = global.enablearea;
	dialog.m_pgRenderReso.m_nXRes = global.xres;
	dialog.m_pgRenderReso.m_nYRes = global.yres;	
	dialog.m_pgRenderReso.m_nXMin = global.xmin * global.xres;
	dialog.m_pgRenderReso.m_nYMin = global.ymin * global.yres;
	dialog.m_pgRenderReso.m_nXMax = global.xmax * (global.xres - 1);
	dialog.m_pgRenderReso.m_nYMax = global.ymax * (global.yres - 1);
	dialog.m_pgRenderOutput.m_bShow = global.show;
	dialog.m_pgRenderOutput.m_sFilename = global.renderdpic;

	int i = 0;
	while (aszExtensions[i] && stricmp(aszExtensions[i], global.picformat))
		i++;
	if (!aszExtensions[i])
		i = 0;

	dialog.m_pgRenderOutput.m_sFormat = aszFormats[i];
	dialog.m_pgRenderMisc.m_nOctreeDepth = global.octreedepth;
	dialog.m_pgRenderMisc.m_nMinObjects = global.minobjects;
	dialog.m_pgRenderMisc.m_bQuick = global.quick;

	if (dialog.DoModal() == IDOK)
	{
		global.octreedepth = dialog.m_pgRenderMisc.m_nOctreeDepth;
		global.minobjects = dialog.m_pgRenderMisc.m_nMinObjects;
		global.quick = dialog.m_pgRenderMisc.m_bQuick;	
		global.enablearea = dialog.m_pgRenderReso.m_bEnableArea;
		global.xres = dialog.m_pgRenderReso.m_nXRes;
		global.yres = dialog.m_pgRenderReso.m_nYRes;
		global.xmin = float(dialog.m_pgRenderReso.m_nXMin)/dialog.m_pgRenderReso.m_nXRes;
		global.ymin = float(dialog.m_pgRenderReso.m_nYMin)/dialog.m_pgRenderReso.m_nYRes;
		global.xmax = float(dialog.m_pgRenderReso.m_nXMax)/(dialog.m_pgRenderReso.m_nXRes - 1);
		global.ymax = float(dialog.m_pgRenderReso.m_nYMax)/(dialog.m_pgRenderReso.m_nXRes - 1);

		global.show = dialog.m_pgRenderOutput.m_bShow;
		if (dialog.m_pgRenderOutput.m_sFilename.IsEmpty())
			global.SetRenderdPic(NULL);
		else
		{
			strcpy(szBuf, (char *)LPCTSTR(dialog.m_pgRenderOutput.m_sFilename));
			if (!FileExt(szBuf))
			{
				i = 0;
				while (aszFormats[i] && stricmp(aszFormats[i], (char *)LPCTSTR(dialog.m_pgRenderOutput.m_sFormat)))
					i++;
				if (!aszFormats[i])
					i = 0;
				strcat(szBuf, ".");
				strcat(szBuf, aszExtensions[i]);
			}
			global.SetRenderdPic(szBuf);
		}
		global.SetPicType((char *)LPCTSTR(dialog.m_pgRenderOutput.m_sFormat));
	}
}

/*************
 * DESCRIPTION:	display render-box
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnRender()
{
	CRenderOutput dialog;

	if (!global.renderdpic && !global.show)
	{
		utility.Request(IDS_ERRNOOUTPUT);
		return;
	}
	dialog.DoModal();
}

IMPLEMENT_DYNAMIC(CRenderSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(CRenderSheet, CPropertySheet)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor, add property sheets
 * INPUT:			system
 * OUTPUT:		-
 *************/
CRenderSheet::CRenderSheet(CWnd* pWndParent)
    : CPropertySheet("Render settings", pWndParent)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	AddPage(&m_pgRenderReso);
	AddPage(&m_pgRenderOutput);
	AddPage(&m_pgRenderMisc);
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:		-
 *************/
int CRenderSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	return bResult;
}

/* CRenderMiscPage */

BEGIN_MESSAGE_MAP(CRenderMiscPage, CPropertyPage)
END_MESSAGE_MAP()

CRenderMiscPage::CRenderMiscPage()
	: CPropertyPage(CRenderMiscPage::IDD)
{
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CRenderMiscPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_spnOctreeDepth.SetRange(1, 8);
	
	return TRUE;
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			pDX	given from system
 * OUTPUT:			-
 *************/
void CRenderMiscPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, ID_RENDER_OCDEPTHCTRL, m_spnOctreeDepth);
	DDX_Text(pDX, ID_RENDER_OCDEPTH, m_nOctreeDepth);
	DDV_MinMaxInt(pDX, m_nOctreeDepth, 1, 8);
	DDX_Text(pDX, ID_RENDER_MINOBJ, m_nMinObjects);
	DDV_MinMaxInt(pDX, m_nMinObjects, 1, 500);
	DDX_Check(pDX, ID_RENDER_QUICK, m_bQuick);
}

// CRenderResoPage
 
BEGIN_MESSAGE_MAP(CRenderResoPage, CPropertyPage)
 	ON_LBN_SELCHANGE(ID_RENDER_COMBOBOX, OnSelResolutionBox)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor, load render resolutions
 * INPUT:			-
 * OUTPUT:			-
 *************/
CRenderResoPage::CRenderResoPage()
	: CPropertyPage(CRenderResoPage::IDD)
{
	if (!sciReadResFile(&m_aszResolutions, &m_anRes))
	{
		m_aszResolutions = aszDefaultResolutions;
		m_anRes = anDefaultRes;
		m_bLoadedResolutions = FALSE;
	}
	else
		m_bLoadedResolutions = TRUE;
}

/*************
 * DESCRIPTION:	destructor, free render resolutions
 * INPUT:			-
 * OUTPUT:			-
 *************/
CRenderResoPage::~CRenderResoPage()
{
	if (m_bLoadedResolutions)
	{
		ASSERT(m_aszResolutions);

		int i = 0;
		while (m_aszResolutions[i])
		{
			delete m_aszResolutions[i];
			i++;
		}
		delete m_aszResolutions;

		ASSERT(m_anRes);
		delete m_anRes;
	}
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CRenderResoPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	VERIFY(m_cboxResolution.SubclassDlgItem(ID_RENDER_COMBOBOX, this));

	int i = 0;
	while (m_aszResolutions[i])
	{
		m_cboxResolution.AddString(m_aszResolutions[i]);
		i++;
	}

	return TRUE;
}

/*************
 * DESCRIPTION:	user changed resolution
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CRenderResoPage::OnSelResolutionBox()
{
	int i, nResX, nResY;

	i = m_cboxResolution.GetCurSel();
	if (i != -1)
	{
		nResX = m_anRes[2*i];
		nResY = m_anRes[2*i + 1];
		SetDlgItemInt(ID_RENDER_RESX, nResX);
		SetDlgItemInt(ID_RENDER_RESY, nResY);
		SetDlgItemInt(ID_RENDER_XMIN, int(nResX * global.xmin));
		SetDlgItemInt(ID_RENDER_YMIN, int(nResY * global.ymin));
		SetDlgItemInt(ID_RENDER_XMAX, int(nResX * global.xmax) - 1);
		SetDlgItemInt(ID_RENDER_YMAX, int(nResY * global.ymax) - 1);
	}			
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			pDX	given from system
 * OUTPUT:			-
 *************/
void CRenderResoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, ID_RENDER_ENABLE, m_bEnableArea);
	DDX_Text(pDX, ID_RENDER_XMIN, m_nXMin);
	DDX_Text(pDX, ID_RENDER_YMIN, m_nYMin);
	DDX_Text(pDX, ID_RENDER_XMAX, m_nXMax);
	DDX_Text(pDX, ID_RENDER_YMAX, m_nYMax);
	DDX_Text(pDX, ID_RENDER_RESX, m_nXRes);
	DDX_Text(pDX, ID_RENDER_RESY, m_nYRes);
}

// CRenderOutputPage
 
BEGIN_MESSAGE_MAP(CRenderOutputPage, CPropertyPage)
END_MESSAGE_MAP()

CRenderOutputPage::CRenderOutputPage()
	: CPropertyPage(CRenderOutputPage::IDD)
{
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CRenderOutputPage::OnInitDialog()
{
	CDialog::OnInitDialog();
		
	VERIFY(m_cboxFormat.SubclassDlgItem(ID_RENDER_FORMAT, this));

	m_fbtnFileName.Init(ID_RENDER_FILENAME, ID_RENDER_FILENAMEBUT, FILEREQU_IMAGE, this);
	m_vbtnFileName.Init(ID_RENDER_FILENAME, ID_RENDER_VIEW_FILE, this);

	int i = 0;
	while (aszFormats[i])
	{
		m_cboxFormat.AddString(aszFormats[i]);
		i++;
	}
	m_cboxFormat.SelectString(0, m_sFormat);
	
	return TRUE;
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			pDX	given from system
 * OUTPUT:			-
 *************/
void CRenderOutputPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_CBString(pDX, ID_RENDER_FORMAT, m_sFormat);
	DDX_Text(pDX, ID_RENDER_FILENAME, m_sFilename);
	DDX_Check(pDX, ID_RENDER_SHOW, m_bShow);
}


// Render box

//void Log(rsiCONTEXT *, char *);
extern GLOBALS global;
extern char szUserName[32];
extern ULONG nUserSernum;

// declare message map
BEGIN_MESSAGE_MAP(CRenderOutput, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(ID_RENDERBOX_SHOW, OnShow)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	Callback routine for RSI
 * INPUT:			-
 * OUTPUT:			-
 *************/
void Cooperate(rsiCONTEXT *rc)
{
	MSG msg;
	// enable multitasking
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		DispatchMessage(&msg);
}

/*************
 * DESCRIPTION:	Callback routine for RSI, display RSI-comments
 * INPUT:			-
 * OUTPUT:			-
 *************/
void Log(rsiCONTEXT *rc, char *szLog)
{
	RENDERINFO *pRenderInfo;

	rsiGetUserData(rc, (void**)&pRenderInfo);
	pRenderInfo->pRenderOutput->m_LogBox.AddString(szLog);
	pRenderInfo->pRenderOutput->m_LogBox.SetCurSel(pRenderInfo->nListBoxCounter);
	pRenderInfo->nListBoxCounter++;
}

/*************
 * DESCRIPTION:	CB for RSI, check if user canceled render process
 * INPUT:			-
 * OUTPUT:			TRUE if user canceled
 *************/
BOOL CheckCancel(rsiCONTEXT *rc)
{
	RENDERINFO *pRenderInfo;

	rsiGetUserData(rc, (void **)&pRenderInfo);

	return pRenderInfo->bCancel;
}

/*************
 * DESCRIPTION:	translate time into string
 * INPUT:			buffer	buffer to write time to
 *						text		additional text information
 *						time		time in seconds
 * OUTPUT:			-
 *************/
void Time(char *buffer, char *text, float time)
{
	clock_t h, m, s;

	h = clock_t(time/3600);
	m = clock_t(fmod((time/60), 60));
	s = clock_t(fmod(time, 60));
	sprintf(buffer, "%s %02d:%02d:%02d", text, h, m, s);
}

/*************
 * DESCRIPTION:	update status display
 * INPUT:			percent	percentage of render status
 *						time		elapsed time
 *						y			last line
 *						c			number of lines to update
 *						line		pointer to renderd line
 * OUTPUT:			-
 *************/
void UpdateStatus(rsiCONTEXT *rc, float percent, float time, int y, int c, rsiSMALL_COLOR *line)
{
	RENDERINFO *pRenderInfo;
	char szBuf[256];

	rsiGetUserData(rc, (void **)&pRenderInfo);

	if (pRenderInfo->bShow)
		pRenderInfo->pRenderOutput->wndPicture.picture.DrawLine(y, c, line);
	
	if (percent < .0)
		percent = .0;
	if (percent > 1.0)
		percent = 1.0;

	pRenderInfo->pProgress->SetPos(int(percent*100));
	sprintf(szBuf, "%3.0f %%", percent*100);
	pRenderInfo->pRenderOutput->SetDlgItemText(ID_RENDERBOX_PERCENT, szBuf);

	if (percent > .0)
	{
		Time(szBuf, "Time elapsed:", time);
		pRenderInfo->pRenderOutput->SetDlgItemText(ID_RENDERBOX_ELAPSED, szBuf);
		Time(szBuf, "Left:", clock_t(time/percent) - time);
		pRenderInfo->pRenderOutput->SetDlgItemText(ID_RENDERBOX_TIMELEFT, szBuf);
		Time(szBuf, "Estimated:", clock_t(time/percent));
		pRenderInfo->pRenderOutput->SetDlgItemText(ID_RENDERBOX_ESTIMAT, szBuf);
	}
}

/*************
 * DESCRIPTION:	constructor
 * INPUT:			system
 * OUTPUT:			-
 *************/
CRenderOutput::CRenderOutput(CWnd* pParent /* =NULL*/)
	: CDialog(CRenderOutput::IDD, pParent)
{
}

/*************
 * DESCRIPTION:	and set cancel flag
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CRenderOutput::OnCancel()
{
	RenderInfo.bCancel = TRUE;
	
	CDialog::OnCancel();
}

/*************
 * DESCRIPTION:	load viewer and show renderd pic
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CRenderOutput::OnShow()
{
	char szBuf1[256], szBuf2[256];
	PREFS p;

	if (global.renderdpic)
	{
		strcpy(szBuf1, global.renderdpic);
		AddFileExt(szBuf1, global.picformat, 256);

		p.id = ID_VIEW;
		if (p.GetPrefs() && ((char *)p.data)[0])
		{
			if (szBuf1[0])
			{
				sprintf(szBuf2, "%s %s", (char *)p.data, szBuf1); 
				if (WinExec(szBuf2, SW_SHOW) < 32)
					utility.Request(IDS_ERRSTARTAPP);
			}
			else
				utility.Request(IDS_ERRNOFILE);
		}
		else
			utility.Request(IDS_ERRNOVIEWER);
		p.data = NULL; // it's a MUST!
	}
	else
		utility.Request(IDS_ERRPICNAME);
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CRenderOutput::OnInitDialog()
{
	rsiResult err;
	
	CDialog::OnInitDialog();

	if (global.show)
	{
		CRect rect(300,200,60,60);
		wndPicture.picture.SetSize(global.xres, global.yres);
		wndPicture.Create(IDD_PICTURE, this);
		wndPicture.ShowWindow(SW_SHOW);
	}
	
	VERIFY(m_LogBox.SubclassDlgItem(ID_RENDERBOX_LISTBOX, this));
	VERIFY(m_Progress.SubclassDlgItem(ID_RENDERBOX_PROGRESS, this));
	m_Progress.SetRange(0, 100);

	ShowWindow(SW_SHOW);

	err = Start();

	return TRUE;
}

/*************
 * DESCRIPTION:	start render process
 * INPUT:			-
 * OUTPUT:			-
 *************/
int CRenderOutput::Start()
{
	char szBuf[256], *szError;
	rsiCONTEXT *rc;
	if (szUserName[0])
		sprintf(szBuf, "Registered for %s; Serial No.: %d", szUserName, nUserSernum);
	else
		strcpy(szBuf, "RayStorm Demo");

	SetWindowText(szBuf);
	GetDlgItem(IDCANCEL)->ShowWindow(TRUE);
	GetDlgItem(IDOK)->ShowWindow(FALSE);
	GetDlgItem(ID_RENDERBOX_SHOW)->EnableWindow(FALSE);

	RenderInfo.pRenderOutput = this;
	RenderInfo.nListBoxCounter = 0;
	RenderInfo.pProgress = &m_Progress;
	RenderInfo.bShow = global.show;
	RenderInfo.bCancel = FALSE;

	m_LogBox.ResetContent();
	szError = sciRenderInit(Log, UpdateStatus, CheckCancel, Cooperate, (void*)&RenderInfo, &rc);
	if (szError)
		MessageBox(szError, "Error");
	else
	{
		szError = sciRender(rc);
		if (szError)
			MessageBox(szError, "Error");
	}
	

	if (RenderInfo.bShow)
	{
		if (::IsWindow(wndPicture.m_hWnd))
			wndPicture.Invalidate(FALSE);
	}

	GetDlgItem(IDCANCEL)->ShowWindow(FALSE);
	GetDlgItem(IDOK)->ShowWindow(TRUE);
	GetDlgItem(ID_RENDERBOX_SHOW)->EnableWindow(TRUE);

	return rsiERR_NONE;
}
