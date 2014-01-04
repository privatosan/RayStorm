/***************
 * MODULE:			griddlg.cpp
 * PROJECT:			Scenario
 * VERSION:			1.0 10.12.1996
 * DESCRIPTION:	gridsize dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 ***************/

#include "typedefs.h"
#include "GridDlg.h"
#include "CamView.h"

/*************
 * DESCRIPTION:	set & call grid size dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnGridsize()
{
	CGridsize dialog;

	dialog.m_Gridsize = pDisplay->gridsize;

	if (dialog.DoModal() == IDOK)
	{
		pDisplay->gridsize = dialog.m_Gridsize;
		sciRedraw();
	}
}

BEGIN_MESSAGE_MAP(CGridsize, CDialog)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CGridsize::CGridsize(CWnd* pParent)
    : CDialog(CGridsize::IDD, pParent)
{
}

/*************
 * DESCRIPTION:	center window in middle of screen
 * INPUT:			(system related)
 * OUTPUT:			-
 *************/
BOOL CGridsize::OnInitDialog()
{
	CDialog::OnInitDialog();
	CenterWindow();

	return TRUE;
}

/*************
 * DESCRIPTION:	transfer data
 * INPUT:			(system related)
 * OUTPUT:			-
 *************/
void CGridsize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, ID_GRIDSIZE_SIZE, m_Gridsize);
	DDV_MinMaxFloat(pDX, m_Gridsize, 0, 1000000);
}


