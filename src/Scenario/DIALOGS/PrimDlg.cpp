/***************
 * MODULE:			PrimDlg.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 15.05.1997
 * DESCRIPTION:	Dialogs for creating mesh objects
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						15.01.1997	mh		initial version
 ***************/

#include "PrimDlg.h"

BEGIN_MESSAGE_MAP(CPrimSetPage, CPropertyPage)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			system
 * OUTPUT:			-
 *************/
CPrimSetPage::CPrimSetPage() : CPropertyPage(CPrimSetPage::IDD)
{
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CPrimSetPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if (m_pObject->GetType() == OBJECT_CONE)
		GetDlgItem(IDC_PRIMSET_CLOSEDTOP)->EnableWindow(FALSE);

	if (m_pObject->GetType() != OBJECT_SOR)
		GetDlgItem(IDC_PRIMSET_SORCALC)->EnableWindow(FALSE);

	((CComboBox*)GetDlgItem(IDC_PRIMSET_SORCALC))->AddString("Fast but inaccurate");
	((CComboBox*)GetDlgItem(IDC_PRIMSET_SORCALC))->AddString("Slow but accurate");
	((CComboBox*)GetDlgItem(IDC_PRIMSET_SORCALC))->SetCurSel(m_nSORAccurate);
	
	return TRUE;
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CPrimSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_PRIMSET_CLOSEDBOTTOM, m_bClosedBottom);
	DDX_Check(pDX, IDC_PRIMSET_CLOSEDTOP, m_bClosedTop);
	DDX_CBIndex(pDX, IDC_PRIMSET_SORCALC, m_nSORAccurate);
}
