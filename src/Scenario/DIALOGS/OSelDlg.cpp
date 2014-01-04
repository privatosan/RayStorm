/***************
 * MODULE:			oseldlg.cpp
 * PROJECT:			Scenario
 * VERSION:			1.0 10.12.1996
 * DESCRIPTION:	select object dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						08.06.1997	mh		initial release
 ***************/

#include "typedefs.h"
#include "oseldlg.h"

CListBox *pListBox;
int nSelmode;

BEGIN_MESSAGE_MAP(CObjSelect, CDialog)
	ON_LBN_SELCHANGE(IDC_OBJSEL_LIST, OnSelchange)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CObjSelect::CObjSelect(int nMode_, CWnd* pParent)
    : CDialog(CObjSelect::IDD, pParent)
{
	nMode = nMode_;
	nSelmode = nMode;
	m_pSelectedObject = NULL;
}

/*************
 * DESCRIPTION:	Callback routine for GetObjects
 * INPUT:			obj
 * OUTPUT:			-
 *************/
void AddSelObjFunction(OBJECT *pObject, void *)
{
	if (!pObject)
		return;

	switch (nSelmode)
	{
		case BROWSER_SELECTEDSURFACE:
			if (!(pObject->selected && pObject->surf))
				return;
		break;
		case BROWSER_CAMERAS:
			if(!(pObject->GetType() == OBJECT_CAMERA))
				return;
		break;
	}

	int index = pListBox->AddString(pObject->GetName());
	pListBox->SetItemData(index, (DWORD)pObject);
}

/*************
 * DESCRIPTION:	do initialisations
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CObjSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	CenterWindow();

	pListBox = ((CListBox*)GetDlgItem(IDC_OBJSEL_LIST));
	GetObjects(AddSelObjFunction, NULL, 0);

	switch (nMode)
	{
		case BROWSER_SELECTEDSURFACE:
			SetWindowText("Surface selector");
			break;
		case BROWSER_CAMERAS:
			SetWindowText("Camera selector");
			break;
	}

	return TRUE;
}

/*************
 * DESCRIPTION:	set current object
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CObjSelect::OnSelchange() 
{
	m_pSelectedObject = (OBJECT*)pListBox->GetItemData(pListBox->GetCurSel());
}