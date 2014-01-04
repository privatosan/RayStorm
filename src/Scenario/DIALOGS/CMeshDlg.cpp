/***************
 * MODULEE			CMeshDlg.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	Dialogs for creating mesh objects
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						30.09.1996	mh		comment
 ***************/

#include "typedefs.h"
#include "camview.h"
#include "mesh.h"
#include "sci.h"
#include "document.h"

int nDialogID;

/*************
 * DESCRIPTION:	create mesh dialog
 * INPUT:			ID		dialog ID
 * OUTPUT:			-
 *************/
void CDoc::CreateMesh(int ID)
{
	UNDO_CREATE *pUndo;
	MESH *pMesh;
	BOOL bErr;

	nDialogID = ID;
	CCMeshDlg dialog;

	if (dialog.DoModal() == IDOK)
	{
		pMesh = new MESH;
		if (pMesh)
		{
			pMesh->selected = TRUE;
			pMesh->surf = new SURFACE;
			if (pMesh->surf)
			{
				switch (nDialogID)
				{
					case IDD_CUBE:   bErr = pMesh->CreateCube(&dialog.m_vSize); break;
					case IDD_TORUS:  bErr = pMesh->CreateTorus(dialog.m_Radius, dialog.m_Thickness, dialog.m_nDivs, dialog.m_nSlices); break;
					case IDD_SPHERE: bErr = pMesh->CreateSphere(dialog.m_Radius, dialog.m_nDivs, dialog.m_nSlices); break;
					case IDD_TUBE:   bErr = pMesh->CreateTube(dialog.m_Radius, dialog.m_Height, dialog.m_nDivs, dialog.m_nSlices, dialog.m_bClosedBottom, dialog.m_bClosedTop); break;
					case IDD_PLANE:  bErr = pMesh->CreatePlane(&dialog.m_vSize, dialog.m_nXDivs, dialog.m_nZDivs); break;
					case IDD_CONE:   bErr = pMesh->CreateCone(dialog.m_Radius, dialog.m_Height, dialog.m_nDivs, dialog.m_nSlices, dialog.m_bClosedBottom); break;
				}
				if (!bErr)
					delete pMesh;
				else
				{
					DeselectAll();
					pMesh->Append();
					pMesh->IsFirstSelected();
					
					pUndo = new UNDO_CREATE;
					if (pUndo)
					{
						if (pUndo->AddCreated(pMesh))
							pUndo->Add();
						else
							delete pUndo;
					}	
					sciBrowserBuild();
 				}
			}
			else
				delete pMesh;
		}
	}
	sciRedraw();
}

BEGIN_MESSAGE_MAP(CCMeshDlg, CDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			system
 * OUTPUT:		-
 *************/
CCMeshDlg::CCMeshDlg(CWnd* pParent) : CDialog(nDialogID, pParent)
{
	m_Radius = 1.;
	m_nDivs = 12.;
	m_nSlices = 1.;
	m_bClosedBottom = TRUE;
	m_bClosedTop = TRUE;
	m_Height = 1.;
	m_Thickness = 0.25;
	SetVector(&m_vSize, 1.,1.,1.);
	m_nXDivs = 10;
	m_nZDivs = 10;
	
	switch (nDialogID)
	{
		case IDD_SPHERE:	m_nSlices = 6;	break;
		case IDD_TORUS : 	m_nSlices = 8;	break;
	}
}

/*************
 * DESCRIPTION:	center window
 * INPUT:			system
 * OUTPUT:		-
 *************/
int CCMeshDlg::OnCreate(LPCREATESTRUCT)
{
	CenterWindow();

	return 0;
}

/*************
 * DESCRIPTION:	transfer dialog data
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CCMeshDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	switch (nDialogID)
	{
		case IDD_CUBE  :	DDX_Text(pDX, ID_CUBE_X, m_vSize.x);
								DDX_Text(pDX, ID_CUBE_Y, m_vSize.y);
								DDX_Text(pDX, ID_CUBE_Z, m_vSize.z);
								break;
		case IDD_SPHERE:	DDX_Text(pDX, ID_SPHERE_RADIUS, m_Radius);
								DDX_Text(pDX, ID_SPHERE_DIVISIONS, m_nDivs);
								DDX_Text(pDX, ID_SPHERE_SLICES, m_nSlices);
								break;
		case IDD_TUBE  :	DDX_Text(pDX, ID_TUBE_RADIUS, m_Radius);
								DDX_Text(pDX, ID_TUBE_HEIGHT, m_Height);
								DDX_Text(pDX, ID_TUBE_DIVISIONS, m_nDivs);
								DDX_Text(pDX, ID_TUBE_SLICES, m_nSlices);
								DDX_Check(pDX, ID_TUBE_CLOSEBOTTOM, m_bClosedBottom);
								DDX_Check(pDX, ID_TUBE_CLOSETOP, m_bClosedTop);
								break;
		case IDD_CONE  :	DDX_Text(pDX, ID_CONE_RADIUS, m_Radius);
								DDX_Text(pDX, ID_CONE_HEIGHT, m_Height);
								DDX_Text(pDX, ID_CONE_DIVISIONS, m_nDivs);
								DDX_Text(pDX, ID_CONE_SLICES, m_nSlices);
								DDX_Check(pDX, ID_CONE_CLOSEBOTTOM, m_bClosedBottom);
								break;
		case IDD_TORUS :	DDX_Text(pDX, ID_TORUS_RADIUS, m_Radius);
								DDX_Text(pDX, ID_TORUS_THICKNESS, m_Thickness);
								DDX_Text(pDX, ID_TORUS_DIVISIONS, m_nDivs);
								DDX_Text(pDX, ID_TORUS_SLICES, m_nSlices);
								break;
		case IDD_PLANE	:	DDX_Text(pDX, ID_PLANE_X, m_vSize.x);
								DDX_Text(pDX, ID_PLANE_Y, m_vSize.y);
								DDX_Text(pDX, ID_PLANE_Z, m_vSize.z);
								DDX_Text(pDX, ID_PLANE_XDIV, m_nXDivs);
								DDX_Text(pDX, ID_PLANE_ZDIV, m_nZDivs);
	}
}
