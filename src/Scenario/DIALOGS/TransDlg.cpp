/***************
 * MODULE:			transdlg.cpp
 * PROJECT:			Scenario
 * DESCRIPTION:	transform dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:       
 *				DATE		NAME	COMMENT
 *				01.09.96	mh		comment
 *				23.09.96	mh		bugfix: name was taken, even though CANCEL was pressed
 ***************/

#include "typedefs.h"
#include "camera.h"
#include "camview.h"
#include "transdlg.h"
#include "undo.h"
#include "sci.h"
#include "ddxddv.h"

#define SETFLAG(a,b,c) if (a) b |= c; else b &= ~c;

/*************
 * DESCRIPTION:	call settings dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnSettings()
{
	CRect rect;
	OBJECT *pObject;
	UNDO_TRANSFORM *pUndo;
	VECTOR vNewPos, vNewSize, vNewAlign;

	GetActiveObjects(&pObject);

	if (pObject)
	{
		CSettingsSheet dialog(this);

		dialog.m_pObject = pObject;
		dialog.m_pgFlareSet.display = pDisplay;
		dialog.m_pgStarSet.display = pDisplay;

		dialog.AddPage(&dialog.m_pgTransform);
		dialog.m_pgTransform.m_pObject = pObject;
		Orient2Angle(&dialog.m_pgTransform.m_Align, &pObject->orient_x, &pObject->orient_y, &pObject->orient_z);
		dialog.m_pgTransform.m_Align.y = -dialog.m_pgTransform.m_Align.y;
		dialog.m_pgTransform.m_Pos = pObject->pos;
		dialog.m_pgTransform.m_Size = pObject->size;
		dialog.m_pgTransform.m_sObjectname = pObject->GetName();
		if (pObject->track)
			dialog.m_pgTransform.m_sTrackname = pObject->track->GetName();

		switch (pObject->GetType())
		{
			case OBJECT_CAMERA:
				dialog.AddPage(&dialog.m_pgCameraSet);
				dialog.m_pgCameraSet.m_pCamera = (CAMERA *)(pObject);
				dialog.m_pgCameraSet.m_HFOV = 2*atan(dialog.m_pgCameraSet.m_pCamera->hfov)*INV_PI_180;
				dialog.m_pgCameraSet.m_VFOV = 2*atan(dialog.m_pgCameraSet.m_pCamera->vfov)*INV_PI_180;	
				dialog.m_pgCameraSet.m_FocalDist = dialog.m_pgCameraSet.m_pCamera->focaldist;
				dialog.m_pgCameraSet.m_Aperture = dialog.m_pgCameraSet.m_pCamera->aperture;
				dialog.m_pgCameraSet.m_bEnableVFOV = ((dialog.m_pgCameraSet.m_pCamera->flags & OBJECT_CAMERA_VFOV) == OBJECT_CAMERA_VFOV);
				dialog.m_pgCameraSet.m_bEnableFocus = ((dialog.m_pgCameraSet.m_pCamera->flags & OBJECT_CAMERA_FOCUSTRACK) == OBJECT_CAMERA_FOCUSTRACK);
				dialog.m_pgCameraSet.m_bFastDOF = ((dialog.m_pgCameraSet.m_pCamera->flags & OBJECT_CAMERA_FASTDOF) == OBJECT_CAMERA_FASTDOF);
			break;
			case OBJECT_POINTLIGHT:
			case OBJECT_SPOTLIGHT:	
				{
					dialog.AddPage(&dialog.m_pgLightSet);
					dialog.m_bStar = FALSE;
					dialog.m_bFlares = FALSE;
					dialog.ExtraPagesOnOff();

					dialog.m_pStar = ((POINT_LIGHT *)(pObject))->star;
					dialog.m_pgLightSet.m_pLight = (POINT_LIGHT *)(pObject);
					dialog.m_pgLightSet.m_cbtnColor.col = dialog.m_pgLightSet.m_pLight->color;
					dialog.m_pgLightSet.m_bShadow = (dialog.m_pgLightSet.m_pLight->flags & OBJECT_LIGHT_SHADOW) == OBJECT_LIGHT_SHADOW;
					dialog.m_pgLightSet.m_bFlares = (dialog.m_pgLightSet.m_pLight->flags & OBJECT_LIGHT_FLARES) == OBJECT_LIGHT_FLARES;
					dialog.m_pgLightSet.m_bStar =  (dialog.m_pgLightSet.m_pLight->flags & OBJECT_LIGHT_STAR) == OBJECT_LIGHT_STAR;
					dialog.m_pgLightSet.m_Falloff = dialog.m_pgLightSet.m_pLight->falloff;
					dialog.m_pgLightSet.m_Radius = dialog.m_pgLightSet.m_pLight->r;
					
					if (dialog.m_pgLightSet.m_pLight->GetType() == OBJECT_SPOTLIGHT)
						dialog.m_pgLightSet.m_Angle = ((SPOT_LIGHT *)dialog.m_pgLightSet.m_pLight)->angle/PI_180;
				}
			break;
			case OBJECT_MESH:
				{		
					if (((MESH *)pObject)->external != EXTERNAL_NONE)
						dialog.AddPage(&dialog.m_pgMeshSet);
					dialog.m_pgMeshSet.m_pMesh = (MESH *)pObject;
					dialog.m_pgMeshSet.m_sFile = dialog.m_pgMeshSet.m_pMesh->file;
					dialog.m_pgMeshSet.m_bApply = (dialog.m_pgMeshSet.m_pMesh->flags & OBJECT_APPLYSURF) ? TRUE : FALSE;	
				}
			break;
			case OBJECT_CYLINDER:
			case OBJECT_CONE:
			case OBJECT_SOR:
					{
						dialog.AddPage(&dialog.m_pgPrimSet);
						dialog.m_pgPrimSet.m_pObject = pObject;
						dialog.m_pgPrimSet.m_bClosedTop = (pObject->flags & OBJECT_OPENTOP) ? FALSE : TRUE;
						dialog.m_pgPrimSet.m_bClosedBottom = (pObject->flags & OBJECT_OPENBOTTOM) ? FALSE : TRUE;
						dialog.m_pgPrimSet.m_nSORAccurate = (pObject->flags & OBJECT_SOR_ACCURATE) ? 0 : 1;
					}
			break;
			case OBJECT_CSG:
				{
					dialog.AddPage(&dialog.m_pgCSGSet);
					dialog.m_pgCSGSet.m_nOperation = ((CSG *)pObject)->operation;
				}
			break;	
		}

		dialog.m_pgTransform.m_bInverted = (pObject->flags & OBJECT_INVERTED) ? TRUE : FALSE;
		
		if (dialog.DoModal() == IDOK)
		{
			GetActiveObjects(&pObject);

			pUndo = new UNDO_TRANSFORM;
			if (pUndo)
			{
				pUndo->AddTransformed(pObject);
				pUndo->Add();
			}
			vNewPos = dialog.m_pgTransform.m_Pos;
			vNewSize = dialog.m_pgTransform.m_Size;
			vNewAlign = dialog.m_pgTransform.m_Align;

			sciSetActiveObjects(pUndo, pObject,
				&vNewPos, FALSE,
				&vNewSize, FALSE,
				&vNewAlign, FALSE);

			pObject->track = dialog.m_pgTransform.m_pTrack;
			pObject->SetName((char*)LPCTSTR(dialog.m_pgTransform.m_sObjectname));
			sciBrowserUpdate(pObject);

			if (pObject->GetType() == OBJECT_CAMERA && pDisplay->view->viewmode == VIEW_CAMERA)
			{
				GetClientRect(rect);
				
				pDisplay->view->width = rect.Width();
				pDisplay->view->height = rect.Height();
		//		pDisplay->view->SetCamera();
				sciRedraw();
			}
			else
			{
				Redraw(REDRAW_REMOVE_SELECTED, FALSE);
				Redraw(REDRAW_SELECTED, FALSE);
			}
			switch (pObject->GetType())
			{
				case OBJECT_CYLINDER:
				case OBJECT_CONE:
				case OBJECT_SOR:
					{
						SETFLAG(!dialog.m_pgPrimSet.m_bClosedTop, pObject->flags, OBJECT_OPENTOP)
						SETFLAG(!dialog.m_pgPrimSet.m_bClosedBottom, pObject->flags, OBJECT_OPENBOTTOM)
						SETFLAG(!(BOOL)dialog.m_pgPrimSet.m_nSORAccurate, pObject->flags, OBJECT_SOR_ACCURATE)
						Redraw(REDRAW_REMOVE_SELECTED, FALSE);
						Redraw(REDRAW_SELECTED, FALSE);
					}
				break;
				case OBJECT_CSG:
					{
						((CSG *)pObject)->operation = dialog.m_pgCSGSet.m_nOperation;
					}
				break;
			}
			SETFLAG(dialog.m_pgTransform.m_bInverted,	pObject->flags, OBJECT_INVERTED)
		}
	}
	else
		utility.Request(IDS_ERRNOOBJSELECT);
}

IMPLEMENT_DYNAMIC(CSettingsSheet, CPropertySheet)

// CSettingsSheet
 
BEGIN_MESSAGE_MAP(CSettingsSheet, CPropertySheet)
	ON_WM_CREATE()
END_MESSAGE_MAP()

/************
 * DESCRIPTION:	constructor, adds property pages
 * INPUT:			pWndParent	parent window
 *						obj			pointer to selected objet
 * OUTPUT:			-
 *************/
CSettingsSheet::CSettingsSheet(CWnd* pWndParent)
    : CPropertySheet("Object settings", pWndParent)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

/*************
 * DESCRIPTION:	switch flare/star pages on/off
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CSettingsSheet::ExtraPagesOnOff()
{
	if (((LIGHT*)m_pObject)->flags & OBJECT_LIGHT_FLARES)
	{
		if (!m_bFlares)
		{
			AddPage(&m_pgFlareSet);
			m_bFlares = TRUE;
		}
	}
	else
	{
		if (m_bFlares)
		{
			RemovePage(&m_pgFlareSet);
			m_bFlares = FALSE;
		}
	}

	if (((LIGHT*)m_pObject)->flags & OBJECT_LIGHT_STAR)
	{
		if (!m_bStar)
		{
			AddPage(&m_pgStarSet);
			m_bStar = TRUE;
		}
	}
	else
	{
		if (m_bStar)
		{
			RemovePage(&m_pgStarSet);
			m_bStar = FALSE;
		}
	}
}

// CTransformPage

BEGIN_MESSAGE_MAP(CTransformPage, CPropertyPage)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
*************/
CTransformPage::CTransformPage()
    : CPropertyPage(CTransformPage::IDD)
{
}

/*************
 * DESCRIPTION:	intializes dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CTransformPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bbtnTrack.Init(this, ID_TRANSFORM_TRACK, ID_TRANSFORM_TRACKBUT);

	switch (m_pObject->GetType())
	{
		case OBJECT_SPHERE:
		case OBJECT_BOX:
		case OBJECT_CYLINDER:
		case OBJECT_CONE:
		case OBJECT_SOR:
		case OBJECT_CSG:
			((CButton *)GetDlgItem(IDC_TRANSFORM_INVERTED))->EnableWindow(TRUE);
			break;
		default: ((CButton *)GetDlgItem(IDC_TRANSFORM_INVERTED))->EnableWindow(FALSE);
	}

	return TRUE;
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			(system related)
 * OUTPUT:			-
 *************/
void CTransformPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, ID_TRANSFORM_NAME, m_sObjectname);
	DDX_Text(pDX, ID_TRANSFORM_TRACK, m_sTrackname);
	DDX_Check(pDX, IDC_TRANSFORM_INVERTED, m_bInverted);

	if (!m_sTrackname.IsEmpty())
	{
		m_pTrack = GetObjectByName((char*)LPCTSTR(m_sTrackname));
		if (!m_pTrack)
		{
			utility.Request(IDS_ERRNOTRACK);
			pDX->Fail();
		}
		else
		{
			if (m_pTrack == m_pObject)
			{
				utility.Request(IDS_ERRTRACKTOSELF);
				pDX->Fail();
			}
		}
	}
	else
		m_pTrack = NULL;

	DDX_Text(pDX, ID_TRANSFORM_POSX, m_Pos.x);
	DDX_Text(pDX, ID_TRANSFORM_POSY, m_Pos.y);
	DDX_Text(pDX, ID_TRANSFORM_POSZ, m_Pos.z);

	DDX_Text(pDX, ID_TRANSFORM_ALIGNX, m_Align.x);
	DDX_Text(pDX, ID_TRANSFORM_ALIGNY, m_Align.y);
	DDX_Text(pDX, ID_TRANSFORM_ALIGNZ, m_Align.z);

	DDX_Text(pDX, ID_TRANSFORM_SIZEX, m_Size.x);
	DDV_MinFloat(pDX, m_Size.x, 0.0f, TRUE);
	DDX_Text(pDX, ID_TRANSFORM_SIZEY, m_Size.y);
	DDV_MinFloat(pDX, m_Size.y, 0.0f, TRUE);
	DDX_Text(pDX, ID_TRANSFORM_SIZEZ, m_Size.z);
	DDV_MinFloat(pDX, m_Size.z, 0.0f, TRUE);
}

