/***************
 * MODULE:			CameraDlg.cpp
 * PROJECT:			Scenario
 * DESCRIPTION:	camera dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 *						03.10.1996	mh		added to settings sheet
 ***************/

#include "typedefs.h"
#include "CamView.h"
#include "CameraDlg.h"
#include "vecmath.h"
#include "globals.h"
#include "Mainfrm.h"
#include "object.h"
#include "TransDlg.h"
#include "OSelDlg.h"
#include "ddxddv.h"

extern GLOBALS global;

static int nLensValues[][2] =
{
	50,50,
	HFOV,VFOV,
	12,12
};

// lens presets
static char *aszLenseNames[] =
{
	"Fisheye",
	"Normal",
	"Tele",
	NULL
};

/*************
 * DESCRIPTION:	make selected camera to active camera
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnActiveCamera()
{
	VIEW *pView = pDisplay->view;
		
	if (pView->viewmode == VIEW_CAMERA)
	{
		pView->SetCamera((CAMERA*)mouse_data.selobj);
		Redraw(REDRAW_ALL, FALSE);
	}
	UpdateStatusBar();
}

/*************
 * DESCRIPTION:	get active camera via select window
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnSetActiveCamera()
{
	CObjSelect dialog(BROWSER_CAMERAS);

	VIEW *pView = pDisplay->view;
	
	if (dialog.DoModal() == IDOK)
	{
		if (dialog.m_pSelectedObject)
		{
			if (pDisplay->view->viewmode == VIEW_CAMERA)
			{
				pView->SetCamera((CAMERA*)dialog.m_pSelectedObject);
				Redraw(REDRAW_ALL, FALSE);
			}
		}
		UpdateStatusBar();
	}
}

// Camera settings dialog

BEGIN_MESSAGE_MAP(CCameraSetPage, CPropertyPage)
	ON_LBN_SELCHANGE(ID_CAMERASET_LISTBOX, OnSel)
	ON_BN_CLICKED(ID_CAMERASET_FROMRES, UpdateParams)
	ON_BN_CLICKED(ID_CAMERASET_FOCUS, UpdateParams)
	ON_EN_KILLFOCUS(ID_CAMERASET_FOVH, UpdateParams)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CCameraSetPage::CCameraSetPage()
    : CPropertyPage(CCameraSetPage::IDD)
{
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CCameraSetPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CenterWindow();

	VERIFY(m_lboxLenses.SubclassDlgItem(ID_CAMERASET_LISTBOX, this));

	int i = 0;
	while (aszLenseNames[i])
	{
		m_lboxLenses.AddString(aszLenseNames[i]);
		i++;
	}
		
	UpdateParams();

	return TRUE;
}

/*************
 * DESCRIPTION:	update/enable/disable vfov and focal distance
 * INPUT:			-
 * OUTPUT:		-
 *************/
void CCameraSetPage::UpdateParams()
{
	char szBuf[20];
	BOOL bName;

	UpdateData();
	
	if (m_bEnableVFOV)
	{
		((CButton *)GetDlgItem(ID_CAMERASET_FOVV))->EnableWindow(FALSE);
		GetDlgItemText(ID_CAMERASET_FOVH, szBuf, 20);
		m_VFOV = (float(global.yres)/global.xres) * atof(szBuf);
		sprintf(szBuf, "%g", m_VFOV);
		SetDlgItemText(ID_CAMERASET_FOVV, szBuf);
	}
	else
		((CButton *)GetDlgItem(ID_CAMERASET_FOVV))->EnableWindow(TRUE);

	bName = (((CSettingsSheet*)(GetParent()))->m_pgTransform.m_pTrack) != NULL;
	if (!bName)
		m_bEnableFocus = FALSE;

	((CButton *)GetDlgItem(ID_CAMERASET_FOCAL))->EnableWindow(!m_bEnableFocus);

	// trackname specified ? enable user to select "Focus track"
	((CButton *)GetDlgItem(ID_CAMERASET_FOCUS))->EnableWindow(bName);
}

/*************
 * DESCRIPTION:	called when Fovh or Fovv was chosen
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCameraSetPage::OnSel()
{
	int i;

	i = m_lboxLenses.GetCurSel();
	if (i != -1)
	{
		SetDlgItemInt(ID_CAMERASET_FOVH, nLensValues[i][0]);
		SetDlgItemInt(ID_CAMERASET_FOVV, nLensValues[i][1]);
	}
	UpdateParams();
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCameraSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, ID_CAMERASET_FOCAL, m_FocalDist);
	DDV_MinFloat(pDX, m_FocalDist, 0.0f, TRUE);
	DDX_Text(pDX, ID_CAMERASET_APERT, m_Aperture);
	DDV_MinFloat(pDX, m_Aperture, 0.0f, FALSE);
	DDX_Text(pDX, ID_CAMERASET_FOVH, m_HFOV);
	DDV_MinMaxFloatExclude(pDX, m_HFOV, 0, 360);
	DDX_Text(pDX, ID_CAMERASET_FOVV, m_VFOV);
	DDV_MinMaxFloatExclude(pDX, m_VFOV, 0, 360);
	DDX_Check(pDX, ID_CAMERASET_FROMRES, m_bEnableVFOV);
	DDX_Check(pDX, ID_CAMERASET_FOCUS, m_bEnableFocus);
	DDX_Check(pDX, ID_CAMERASET_FASTDOF, m_bFastDOF);
}

/*************
 * DESCRIPTION:	update data
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CCameraSetPage::OnSetActive()
{
	if (IsWindow(m_hWnd))
		UpdateParams();
	
	return CPropertyPage::OnSetActive();
}

/*************
 * DESCRIPTION:	copy data
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCameraSetPage::OnOK()
{
	m_pCamera->hfov = tan(0.5*m_HFOV*PI_180);
	m_pCamera->vfov = tan(0.5*m_VFOV*PI_180);
	m_pCamera->focaldist = m_FocalDist;
	m_pCamera->aperture = m_Aperture;

	if (m_bEnableVFOV)
		m_pCamera->flags |= OBJECT_CAMERA_VFOV;
	else
		m_pCamera->flags &= ~OBJECT_CAMERA_VFOV;

	if (m_bEnableFocus)
		m_pCamera->flags |= OBJECT_CAMERA_FOCUSTRACK;
	else
		m_pCamera->flags &= ~OBJECT_CAMERA_FOCUSTRACK;

	if (m_bFastDOF)
		m_pCamera->flags |= OBJECT_CAMERA_FASTDOF;
	else
		m_pCamera->flags &= ~OBJECT_CAMERA_FASTDOF;

	CPropertyPage::OnOK();
}
