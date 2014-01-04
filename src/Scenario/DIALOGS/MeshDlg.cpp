/***************
 * MODULE:			meshdlg.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	light settings (point and spot light)
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 *						26.09.1996	mh		added radius as attribute
 *						03.10.1996	mh		added to settings sheet
 ***************/

#include "typedefs.h"
#include "CamView.h"
#include "MeshDlg.h"
#include "TransDlg.h"
#include "Document.h"

BEGIN_MESSAGE_MAP(CMeshSetPage, CPropertyPage)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CMeshSetPage::CMeshSetPage()
    : CPropertyPage(CMeshSetPage::IDD)
{
}

/*************
 * DESCRIPTION:	initialization
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CMeshSetPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_fbtnFile.Init(ID_MESHSET_FILE, ID_MESHSET_FILEBUT, FILEREQU_OBJECT, this);

	return TRUE;
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMeshSetPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, ID_MESHSET_FILE, m_sFile);
	DDX_Check(pDX, ID_MESHSET_APPLY, m_bApply);
}

/*************
 * DESCRIPTION:	
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMeshSetPage::OnOK()
{
	if (m_pMesh->external == EXTERNAL_ROOT)
	{
		m_pMesh->flags |= OBJECT_APPLYSURF;

		if (!m_sFile.IsEmpty())
			m_pMesh->SetFileName((char*)LPCTSTR(m_sFile));
		else
		{
			if (m_pMesh->file)
				delete m_pMesh->file;
			m_pMesh->file = NULL;
		}
	}
	CPropertyPage::OnOK();
}
