 /***************
  * NAME:			render.cpp
  * PROJECT:		RayStorm
  * VERSION:		1.0 13.09.1995
  * DESCRIPTION:	Render dialog
  * AUTHORS:		Mike Hesser
  * HISTORY:      DATE			NAME	COMMENT
  *					19.03.1996	mh		first release (0.1)
  ***************/

#include "stdafx.h"
#include "resource.h"
#include "math.h"
#include "render.h"
#include "parser.h"

THREAD_DATA data;
int refresh = 0;

// declare message map
BEGIN_MESSAGE_MAP(CRenderOutput, CDialog)
	ON_WM_CLOSE()
	ON_COMMAND(IDCANCEL, Cancel)
END_MESSAGE_MAP()

CRenderOutput::CRenderOutput(CWnd* pParent /* =NULL*/)
: CDialog(CRenderOutput::IDD, pParent)
{
}

void CRenderOutput::OnClose()
{
	ShowWindow(SW_HIDE);
}

BOOL CRenderOutput::OnInitDialog()
{
	CDialog::OnInitDialog();

	VERIFY(m_Progress.SubclassDlgItem(ID_PROGRESS, this));
	m_Progress.SetRange(0, 100);	
	VERIFY(m_LogBox.SubclassDlgItem(ID_LISTBOX, this));

	return TRUE;
}

HANDLE CRenderOutput::Start(char *szProgram)
{
	DWORD threadID;
	HANDLE hThread;
	
	listboxcounter = 0;
	bCancel = FALSE;
	m_LogBox.ResetContent();

	GetDlgItem(IDCANCEL)->ShowWindow(TRUE);
	GetDlgItem(IDOK)->ShowWindow(FALSE);

	data.pDisplay = this;
	data.szProg = szProgram;

//	Execute((DWORD*)&data);
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Execute, &data, 0, &threadID);

	return hThread;
}

void CRenderOutput::End()
{
	MessageBeep(MB_ICONEXCLAMATION);	
	GetDlgItem(IDCANCEL)->ShowWindow(FALSE);
	GetDlgItem(IDOK)->ShowWindow(TRUE);
	InvalidateRect(NULL, TRUE);
}

void CRenderOutput::Time(char *buffer, char *text, float time)
{
	clock_t h, m, s;

	h = clock_t(time/3600);
	m = clock_t(fmod((time/60), 60));
	s = clock_t(fmod(time, 60));
	sprintf(buffer, "%s %02d:%02d:%02d", text, h, m, s);
}

void CRenderOutput::Cooperate()
{
	MSG msg;
	// enable multitasking
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		DispatchMessage(&msg);
}

void CRenderOutput::Log(char *szLog)
{
	if (IsWindow(m_LogBox.m_hWnd))
	{
		m_LogBox.AddString(szLog);
		m_LogBox.SetCurSel(listboxcounter);
	}
	listboxcounter++;
}

void CRenderOutput::UpdateStatus(float percent, float time)
{
	char szBuf[150];

	refresh++;
	if (refresh % 10 == 0 || percent == 1.0 || percent == 0.0)
	{
		if (percent < .0)
			percent = .0;
		if (percent > 1.0)
			percent = 1.0;

		m_Progress.SetPos(int(percent*100));
		sprintf(szBuf, "%3.0f %%", percent*100);
		SetDlgItemText(ID_PERCENT, szBuf);
	
		if (percent > .0)
		{
			Time(szBuf, "Time elapsed:", time);
			SetDlgItemText(ID_ELAPSED, szBuf);
			Time(szBuf, "Left:", (float)clock_t(time/percent) - time);
			SetDlgItemText(ID_TIMELEFT, szBuf);
			Time(szBuf, "Estimated:", (float)clock_t(time/percent));
			SetDlgItemText(ID_ESTIMAT, szBuf);
		}
	}
}

