/***************
 * MODULE:			CSGdlg.cpp
 * PROJECT:			Scenario
 * DESCRIPTION:	CSG settings
 * AUTHORS:			Andreas Heumann
 * HISTORY:			DATE			NAME	COMMENT
 *						29.04.1997	ah		initial release
 *						13.05.1997	mh		added to my project
 ***************/

#include "typedefs.h"
#include "csgdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char *aszCsgOperations[] =
{
	"Union",
	"Intersection",
	"Difference",
	NULL
};

// CCSGSetPage property page
IMPLEMENT_DYNCREATE(CCSGSetPage, CPropertyPage)

CCSGSetPage::CCSGSetPage() : CPropertyPage(CCSGSetPage::IDD)
{
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			-
 * OUTPUT:		-
 *************/
void CCSGSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_CSG_TYPE, m_nOperation);
}


BEGIN_MESSAGE_MAP(CCSGSetPage, CPropertyPage)
END_MESSAGE_MAP()


/*************
 * DESCRIPTION:	initialization
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CCSGSetPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	VERIFY(m_cboxOperation.SubclassDlgItem(IDC_CSG_TYPE, this));

	int i = 0;
	while (aszCsgOperations[i])
	{
		m_cboxOperation.AddString(aszCsgOperations[i]);
		i++;
	}

	m_cboxOperation.SetCurSel(m_nOperation);
	
	return TRUE;
}
