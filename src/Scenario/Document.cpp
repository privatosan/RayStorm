/* MODULE:			document.cpp
 * PROJECT:			Scenario
 * VERSION:       1.0 29.08.1996
 * DESCRIPTION:   implementation of document class
 *	AUTHORS:			Mike Hesser
 * HISTORY:
 *						DATE		NAME	COMMENT
 *						29.08.96	mh		bug fix with extension & File dialog, added comment
 *						25.09.96	mh		ErrorRequest
 *						15.04.98	mh		OnLoad, OnSave, OnSaveAs
 ***************/

#include <direct.h>
#include "Typedefs.h"
#include "Scenario.h"
#include "Document.h"
#include "Resource.h"
class DISPLAY;
#include "Project.h"
#include "Box.h"
#include "Sphere.h"
#include "Plane.h"
#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "Globals.h"
#include "Sci.h"
#include "Utility.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern GLOBALS global;
extern char szWorkingDirectory[256];

IMPLEMENT_DYNCREATE(CDoc, CDocument)

BEGIN_MESSAGE_MAP(CDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN,	OnLoad)
	ON_COMMAND(ID_FILE_SAVE,	OnSave)
	ON_COMMAND(ID_FILE_SAVE_AS,OnSaveAs)
	ON_COMMAND(IDM_MAXIS,		OnAxis)
	ON_COMMAND(IDM_BOX,			OnBox)
	ON_COMMAND(IDM_SPHERE,		OnSphere)
	ON_COMMAND(IDM_PLANE,		OnPlane)
	ON_COMMAND(IDM_CYLINDER,	OnCylinder)
	ON_COMMAND(IDM_CONE,			OnCone)
	ON_COMMAND(IDM_SOR,			OnSOR)
	ON_COMMAND(IDM_CSG,			OnCSG)
	ON_COMMAND(IDM_CAMERA,		OnCamera)
	ON_COMMAND(IDM_POINTLIGHT,	OnPointlight)
	ON_COMMAND(IDM_SPOTLIGHT,	OnSpotlight)
	ON_COMMAND(IDM_MSPHERE,		OnMeshSphere)
	ON_COMMAND(IDM_MCUBE,		OnMeshCube)
	ON_COMMAND(IDM_MPLANE,		OnMeshPlane)	
	ON_COMMAND(IDM_MCONE,		OnMeshCone)
	ON_COMMAND(IDM_MTUBE,		OnMeshTube)
	ON_COMMAND(IDM_MTORUS,		OnMeshTorus)
	ON_COMMAND(IDM_LOADOBJECT, OnLoadObject)
	ON_COMMAND(IDM_SAVEOBJECT, OnSaveObject)
	ON_COMMAND(IDM_GROUP,		OnGroup)
	ON_COMMAND(IDM_UNGROUP,		OnUngroup)
END_MESSAGE_MAP()

/**************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CDoc::CDoc()
{
	bFirsttime = TRUE;
}

/**************
 * DESCRIPTION:	get pointer to camera view
 * INPUT:			-
 * OUTPUT:			pointer to camera view
 *************/
CCamView *CDoc::GetView()
{
	POSITION pos;

	pos = GetFirstViewPosition();
	GetNextView(pos);

	return (CCamView*)GetNextView(pos);
}

/**************
 * DESCRIPTION:	loads an object (RSOB, TDDD, 3DS, RAW ...
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CDoc::OnLoadObject()
{
	char szFile[256];
	
	if (utility.FileRequ(szFile, FILEREQU_OBJECT, 0))
		sciLoadObject(szFile);
}

/**************
 * DESCRIPTION:	saves an object (RSOB, TDDD, 3DS, RAW ...
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CDoc::OnSaveObject()
{
	char szFile[256], *szErr;

	szErr = TestForSaveObject();
	if (szErr)
	{
		utility.Request(szErr);
		return;
	}

	if (utility.FileRequ(szFile, FILEREQU_OBJECT, FILEREQU_SAVE))
		sciSaveObject(LPSTR(szFile));
}

/**************
 * DESCRIPTION:	creates new document/project
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CDoc::OnNewDocument()
{
	CRect rect;

	if (!CDocument::OnNewDocument())
		return FALSE;

	// ignore first NEW call
	if (!bFirsttime)
		sciNew();
	else
		bFirsttime = FALSE;

	SetProjectChanged(FALSE);
	SetTitle(GetProjectName());

	return TRUE;
}

/**************
 * DESCRIPTION:	load project
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CDoc::OnLoad()
{
	char szBuf[256], *szErr;

	if (IsModified() || GetProjectChanged())
	{
		sprintf(szBuf, "Save changes to %s ?", GetProjectName());
		if(utility.AskRequest(szBuf))
			OnSave();
	}
	SetCurrentDirectory(szWorkingDirectory);
	strcpy(szBuf, "*.scn");
	if (utility.FileRequ(szBuf, FILEREQU_SCENE, 0))
	{
		szErr = sciOpen(szBuf);
		if (szErr)
		{
			utility.Request(szErr);
			OnNewDocument();
			return;
		}
		SetTitle(szBuf);
		SetProjectChanged(FALSE);
		GetView()->pMatView->Invalidate(TRUE);
	}
}

/**************
 * DESCRIPTION:	save project
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CDoc::OnSave()
{
	char szBuf[256], *szErr;

	if (!IsTitled())
		OnSaveAs();
	else
	{
		strcpy(szBuf, GetProjectName());
		szErr = sciSave(szBuf);
		if (szErr)
			utility.Request(szErr);
		else
			SetProjectChanged(FALSE);
	}
}

/**************
 * DESCRIPTION:	save project as
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CDoc::OnSaveAs()
{
	char szBuf[256], *szErr;

	strcpy(szBuf, "");
	if (utility.FileRequ(szBuf, FILEREQU_SCENE, FILEREQU_SAVE))
	{
		szErr = sciSave(szBuf);
		if (szErr)
			utility.Request(szErr);
		else
			SetProjectChanged(FALSE);
	}
}


// CDoc diagnostics
#ifdef _DEBUG
void CDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
