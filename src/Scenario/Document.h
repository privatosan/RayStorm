/***************
 * MODULE:			document.h
 * PROJECT:			Scenario
 * DESCRIPTION:   implementation of document class, definition file
 *	AUTHORS:			Mike Hesser
 * HISTORY:
 *						DATE		NAME	COMMENT
 *						07.09.96	mh		comment
 ***************/

#include "CamView.h"
#include "CMeshDlg.h"

class CDoc : public CDocument
{
	DECLARE_DYNCREATE(CDoc)

	BOOL bFirsttime;
	CDoc();
	
	void CreateMesh(int type);	// cmeshdlg.cpp

// Implementation
public:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CCamView *GetView();

	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnSaveAs();
	afx_msg void OnAxis()				{ sciDoMethod(SCIM_CREATEAXIS); }
	afx_msg void OnSphere()				{ sciDoMethod(SCIM_CREATESPHERE); }
	afx_msg void OnBox()					{ sciDoMethod(SCIM_CREATEBOX); }
	afx_msg void OnPlane()				{ sciDoMethod(SCIM_CREATEPLANE); }
	afx_msg void OnCylinder()			{ sciDoMethod(SCIM_CREATECYLINDER); }
	afx_msg void OnCone()				{ sciDoMethod(SCIM_CREATECONE); }
	afx_msg void OnSOR()					{ sciDoMethod(SCIM_CREATESOR); }
	afx_msg void OnCSG()					{ sciDoMethod(SCIM_CREATECSG); }
	afx_msg void OnPointlight()		{ sciDoMethod(SCIM_CREATEPOINTLIGHT); }
	afx_msg void OnSpotlight()			{ sciDoMethod(SCIM_CREATESPOTLIGHT); }
	afx_msg void OnCamera()				{ sciDoMethod(SCIM_CREATECAMERA); }
	afx_msg void OnMeshSphere()		{ CreateMesh(IDD_SPHERE); }
	afx_msg void OnMeshCube()			{ CreateMesh(IDD_CUBE); }
	afx_msg void OnMeshTorus()			{ CreateMesh(IDD_TORUS); }
	afx_msg void OnMeshCone() 			{ CreateMesh(IDD_CONE); }
	afx_msg void OnMeshTube()			{ CreateMesh(IDD_TUBE); }
	afx_msg void OnMeshPlane()			{ CreateMesh(IDD_PLANE); }
	afx_msg void OnLoadObject();
	afx_msg void OnSaveObject();
	afx_msg void OnCameraToViewer();
	afx_msg void OnGroup()				{ sciDoMethod(SCIM_GROUP); }
	afx_msg void OnUngroup()			{ sciDoMethod(SCIM_UNGROUP); }
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};

