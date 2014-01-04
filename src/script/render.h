 /***************
 * NAME:		  render.h
 * PROJECT:		  RayStorm
 * VERSION:       1.0 13.09.1995
 * DESCRIPTION:   Render dialog
 * AUTHORS:       Mike Hesser
 * BUGS:          none
 * TO DO:         -
 * HISTORY:       DATE          NAME    COMMENT
 *				  19.03.1996	mh		first release (0.1)
 ***************/

#ifndef RENDER_H
#define RENDER_H

#include "resource.h"
#include "afxcmn.h"
#include "typedefs.h"

class CRenderOutput : public CDialog
{
public:
	CRenderOutput(CWnd *pParent = NULL);
	enum { IDD = IDD_RENDER };

	CListBox m_LogBox;
	CProgressCtrl m_Progress;
	BOOL bCancel;
	
	int listboxcounter;

	void Cooperate();
	void Time(char *, char *, float);
	void Cancel() { bCancel = TRUE; }
	BOOL CheckCancel() { return bCancel; }
	void Log(char *);
	void UpdateStatus(float, float);
	HANDLE Start(char *);
	void End();
	
protected:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

typedef struct
{
	CRenderOutput *pDisplay;
	char *szProg;
} THREAD_DATA;

#endif
