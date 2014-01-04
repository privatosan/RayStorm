/***************
 * MODULE:			CamView.cpp
 * PROJECT:			Scenario
 * DESCRIPTION:	view
 * AUTHORS:			Mike Hesser, Andreas Heumann
 * HISTORY:       
 *				DATE		NAME	COMMENT
 *				10.12.95	ah		initial release
 *				29.10.96	mh		comments
 *				17.06.97 mh		OnSelect
 ***************/

#include "Typedefs.h"
#include "Scenario.h"
#include "Mainfrm.h"
#include "Gfx.h"
#include "Document.h"
#include "GridDlg.h"
#include "SurfDlg.h"
#include "CameraDlg.h"
#include "PrefDlg.h"
#include "GlobalDlg.h"
#include "RendDlg.h"
#include "CamView.h"
#include "Mesh.h"
#include "Resource.h"
#include "Magic.h"
#include "Utility.h"

#ifdef _DEBUG
#include <time.h>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
const LPCTSTR CAMVIEWCLASSNAME = _T("CamView");

IMPLEMENT_DYNCREATE(CCamView, CView)

BEGIN_MESSAGE_MAP(CCamView, CView)
	ON_COMMAND(IDM_SURFACE,							OnAttributes)
	ON_COMMAND(IDM_SETTINGS,						OnSettings)
	ON_COMMAND(IDM_MOVEWORLD,						OnMoveWorld)
	ON_COMMAND(IDM_ROTATEWORLD,					OnRotateWorld)
	ON_COMMAND(IDM_ZOOMWORLD,						OnZoomWorld)
	ON_COMMAND(IDM_MOVEOBJECT,						OnMoveObject)
	ON_COMMAND(IDM_ROTATEOBJECT,					OnRotateObject)
	ON_COMMAND(IDM_SIZEOBJECT,						OnSizeObject)
	ON_COMMAND(IDM_GLOBALSETTINGS,				OnGlobalSet)
	ON_COMMAND(IDM_PREFS,							OnPrefSet)
	ON_COMMAND(IDM_RENDER_SETTINGS,				OnRenderSettings)
	ON_COMMAND(IDM_SETRENDERWINDOW,				OnSetRenderWindow)
	ON_COMMAND(IDM_RENDER,							OnRender)
	ON_COMMAND(IDM_GRID,								OnGridOnOff)
	ON_COMMAND(IDM_GRIDSNAP,						OnSnapToGrid)
	ON_COMMAND(IDM_GRIDSIZE,						OnGridsize)
	ON_COMMAND(IDM_TOP,								OnTop)
	ON_COMMAND(IDM_FRONT,							OnFront)
	ON_COMMAND(IDM_RIGHT,							OnRight)
	ON_COMMAND(IDM_PERSPECTIVE,					OnPerspective)
	ON_COMMAND(IDM_4T,								OnQuadview)
	ON_COMMAND(IDM_CAMERAVIEW,						OnCamera)
	ON_COMMAND(IDM_FOCUSOBJECT,					OnFocus)
	ON_COMMAND(IDM_ZOOM_FIT,						OnZoomFit)
	ON_COMMAND(IDM_ZOOM_IN,							OnZoomIn)
	ON_COMMAND(IDM_ZOOM_OUT,						OnZoomOut)
	ON_COMMAND(IDM_BOUNDING,						OnBounding)
	ON_COMMAND(IDM_LOCAL,							OnLocal)
	ON_COMMAND(IDM_WIRE,								OnWire)
	ON_COMMAND(IDM_SOLID,							OnSolid)
	ON_COMMAND(IDM_SELECT_ALL,						OnSelectAll)
	ON_COMMAND(IDM_DESELECT_ALL,					OnDeselectAll)
	ON_COMMAND(IDM_SELECT_NEXT,					OnSelectNext)
	ON_COMMAND(IDM_SELECT_PREV,					OnSelectPrev)
	ON_COMMAND(IDM_SELECT,							OnSelectByBox)
	ON_COMMAND(IDM_EDITPOINTS,						OnEditPoints)
	ON_COMMAND(IDM_ADDPOINTS,						OnAddPoints)
	ON_COMMAND(IDM_EDITEDGES,						OnEditEdges)
	ON_COMMAND(IDM_ADDEDGES,						OnAddEdges)
	ON_COMMAND(IDM_EDITFACES,						OnEditFaces)
	ON_COMMAND(IDM_ADDFACES,						OnAddFaces)
	ON_COMMAND(IDM_FILTERX,							OnFilterX)
	ON_COMMAND(IDM_FILTERY,							OnFilterY)
	ON_COMMAND(IDM_FILTERZ,							OnFilterZ)
	ON_COMMAND(IDM_SHOWCOORDS,						OnShowCoord)
	ON_COMMAND(IDM_SHOWNAMES,						OnShowName)
	ON_COMMAND(IDM_SHOWALL,							OnShowAll)
	ON_COMMAND(IDM_SHOWCAMERAS,					OnShowCamera)
	ON_COMMAND(IDM_SHOWBRUSHES,					OnShowBrush)
	ON_COMMAND(IDM_SHOWTEXTURES,					OnShowTexture)
	ON_COMMAND(IDM_SHOWLIGHTS,						OnShowLight)
	ON_COMMAND(IDM_SHOWMESHES,						OnShowMesh)
	ON_COMMAND(IDM_SHOWSPHERES,					OnShowSphere)
	ON_COMMAND(IDM_SHOWBOXES,						OnShowBox)
	ON_COMMAND(IDM_SHOWCYLINDERS,					OnShowCylinder)
	ON_COMMAND(IDM_SHOWCONES,						OnShowCone)
	ON_COMMAND(IDM_SHOWSORS,						OnShowSOR)
	ON_COMMAND(IDM_SHOWPLANES,						OnShowPlane)
	ON_COMMAND(IDM_SHOWCSG,							OnShowCSG)
	ON_COMMAND(IDM_POPUP_ACTIVECAMERA,			OnActiveCamera)
	ON_COMMAND(IDM_ACTIVECAMERA,					OnSetActiveCamera)	
	ON_COMMAND(IDM_REDRAW,							OnRedraw)
	ON_COMMAND(IDM_CENTERVIEW,						OnCenterView)
	ON_COMMAND(IDM_CUT,								OnCut)
	ON_COMMAND(IDM_COPY,								OnCopy)
	ON_COMMAND(IDM_PASTE,							OnPaste)
	ON_COMMAND(IDM_UNDO,								OnUndo)
	ON_COMMAND(IDM_REDO,								OnRedo)
	ON_COMMAND(IDM_DELETE,							OnDelete)
	ON_COMMAND(IDM_CAMTOVIEW, 						OnCameraToViewer)
	ON_COMMAND(IDM_TOSCENE, 						OnToScene)
	ON_COMMAND(IDM_MATERIAL_NEW,					OnNewMaterial)
	ON_COMMAND(IDM_MATERIAL_LOADNEW,				OnLoadNewMaterial)
	ON_COMMAND(IDM_MATERIAL_REMOVEUNUSED,		OnRemoveUnused)
	ON_COMMAND(IDM_MATERIAL_REMOVEIDENTICAL,	OnRemoveIdentical)
	ON_UPDATE_COMMAND_UI(IDM_UNDO,				OnUndoUI)
	ON_UPDATE_COMMAND_UI(IDM_REDO,				OnRedoUI)
	ON_UPDATE_COMMAND_UI(IDM_CUT,					OnCutUI)
	ON_UPDATE_COMMAND_UI(IDM_COPY,				OnCopyUI)
	ON_UPDATE_COMMAND_UI(IDM_PASTE,				OnPasteUI)
	ON_UPDATE_COMMAND_UI(IDM_ADDPOINTS,			OnAddPointsUI)
	ON_UPDATE_COMMAND_UI(IDM_EDITPOINTS,		OnEditPointsUI)
	ON_UPDATE_COMMAND_UI(IDM_ADDPOINTS,			OnAddPointsUI)
	ON_UPDATE_COMMAND_UI(IDM_EDITEDGES,			OnEditEdgesUI)
	ON_UPDATE_COMMAND_UI(IDM_ADDEDGES,			OnAddEdgesUI)
	ON_UPDATE_COMMAND_UI(IDM_EDITFACES,			OnEditFacesUI)
	ON_UPDATE_COMMAND_UI(IDM_ADDFACES,			OnAddFacesUI)
	ON_UPDATE_COMMAND_UI(IDM_BOUNDING,			OnBoundingUI)
	ON_UPDATE_COMMAND_UI(IDM_WIRE,				OnWireUI)
	ON_UPDATE_COMMAND_UI(IDM_SOLID,				OnSolidUI)
	ON_UPDATE_COMMAND_UI(IDM_LOCAL,				OnLocalUI)
	ON_UPDATE_COMMAND_UI(IDM_SELECT,				OnSelectUI)
	ON_UPDATE_COMMAND_UI(IDM_FILTERX,			OnFilterXUI)
	ON_UPDATE_COMMAND_UI(IDM_FILTERY,			OnFilterYUI)
	ON_UPDATE_COMMAND_UI(IDM_FILTERZ,			OnFilterZUI)
	ON_UPDATE_COMMAND_UI(IDM_FILTERSHIFTX,		OnFilterShiftXUI)
	ON_UPDATE_COMMAND_UI(IDM_FILTERSHIFTY,		OnFilterShiftYUI)
	ON_UPDATE_COMMAND_UI(IDM_FILTERSHIFTZ,		OnFilterShiftZUI)
	ON_UPDATE_COMMAND_UI(IDM_GRID,				OnGridOnOffUI)
	ON_UPDATE_COMMAND_UI(IDM_GRIDSNAP,			OnSnapToGridUI)
	ON_UPDATE_COMMAND_UI(IDM_MOVEOBJECT,		OnMoveObjectUI)
	ON_UPDATE_COMMAND_UI(IDM_ROTATEOBJECT,		OnRotateObjectUI)
	ON_UPDATE_COMMAND_UI(IDM_SIZEOBJECT,		OnSizeObjectUI)
	ON_UPDATE_COMMAND_UI(IDM_MOVEWORLD,			OnMoveWorldUI)
	ON_UPDATE_COMMAND_UI(IDM_ROTATEWORLD,		OnRotateWorldUI)
	ON_UPDATE_COMMAND_UI(IDM_ZOOMWORLD,			OnZoomWorldUI)
	ON_UPDATE_COMMAND_UI(IDM_TOP,					OnTopUI)
	ON_UPDATE_COMMAND_UI(IDM_FRONT,				OnFrontUI)
	ON_UPDATE_COMMAND_UI(IDM_RIGHT,				OnRightUI)
	ON_UPDATE_COMMAND_UI(IDM_PERSPECTIVE,		OnPerspectiveUI)
	ON_UPDATE_COMMAND_UI(IDM_4T,					OnQuadviewUI)
	ON_UPDATE_COMMAND_UI(IDM_CAMERAVIEW,		OnCameraUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWCOORDS,		OnShowCoordUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWNAMES,			OnShowNameUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWALL,			OnShowAllUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWCAMERAS,		OnShowCameraUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWBRUSHES,		OnShowBrushUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWTEXTURES,		OnShowTextureUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWLIGHTS,		OnShowLightUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWMESHES,		OnShowMeshUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWSPHERES,		OnShowSphereUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWBOXES,			OnShowBoxUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWCYLINDERS,	OnShowCylinderUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWCONES,			OnShowConeUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWSORS,			OnShowSORUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWPLANES,		OnShowPlaneUI)
	ON_UPDATE_COMMAND_UI(IDM_SHOWCSG,			OnShowCSGUI)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

BOOL bCanceled;

/*************
 * DESCRIPTION:	constructor
 *						initializes display, and sets pointers and flags
 *						the two bitmaps are allocated in OnDraw
 * INPUT:			-
 * OUTPUT:			-
 *************/
CCamView::CCamView()
{
	SURFACE *pSurf;
	PREFS prefs;
	int i;

	pDisplay = new DISPLAY;
	if (!pDisplay)
		utility.Request(IDS_ERRNOMEM);

	prefs.id = ID_FLGS;
	if (prefs.GetPrefs())
	{
		if(*((ULONG*)prefs.data) & PREFFLAG_SHOWCOORD)
			pDisplay->always_coord = TRUE;
	}
	prefs.data = NULL;

	mouse_data.pView = this;
	mouse_data.display = pDisplay;

	sciInit(this);
	utility.Init(this);

	pSurf = sciCreateSurface(SURF_DEFAULTNAME);
	pSurf->flags |= SURF_DEFAULT;

	bLeftButton = FALSE;
	bCanceled = TRUE;
	bLockOnSizeRedraw = TRUE;
	
	for (i = 0; i < MENU_MAXSTATES; i++)
		bMenuStates[i] = TRUE;

}

/*************
 * DESCRIPTION:	destructor
 *						deletes all allocated memory DCs, bitmaps and the display
 *						the two bitmaps are allocated in OnDraw
 * INPUT:			-
 * OUTPUT:			-
 *************/
CCamView::~CCamView()
{
	FreeSurfaces();
	gfx.Cleanup();

	delete pDisplay;
}

/*************
 * DESCRIPTION:	delete background
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CCamView::OnEraseBkgnd(CDC *pDC)
{
	CRect client;/*

	GetClientRect(client);

	gfx.SetDC(pDC);
	gfx.BitBlt(GFX_MEMORY1, client.left, client.top, client.Width(), client.Height(), GFX_SCREEN, client.left, client.top);*/
	return FALSE;//TRUE;
}

/*************
 * DESCRIPTION:	called when window is created, allocate screen buffers
 * INPUT:			system
 * OUTPUT:			-
 *************/
int CCamView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	gfx.Init(this);

	return 0;
}

/*************
 * DESCRIPTION:	Called before window is created, change window class so
 *						that we have an own DC
 * INPUT:			system
 * OUTPUT:			-
 *************/
BOOL CCamView::PreCreateWindow(CREATESTRUCT &cs) 
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	if (CView::PreCreateWindow(cs))
	{
		static BOOL bRegistered = FALSE;		// flag for first-time init

		if (!bRegistered)
		{
			// This is the first time a view is being created.
			// Need to register the view class.
			//
			TRACE("CCamView::PreCreateWindow, MFC classname = %s\n", cs.lpszClass);
			WNDCLASSEX wc;
			wc.cbSize = sizeof(WNDCLASSEX);

			// Get class information for MFC default view.
			// The classname is in cs.lpszClass
			GetClassInfoEx(AfxGetInstanceHandle(), cs.lpszClass, &wc);

			// Modify name and style
			wc.lpszClassName = CAMVIEWCLASSNAME;
			// Here's the trick, we want to have our own DC !
			// We don't want to call GetDC all the time!
			wc.style |= CS_OWNDC; 

			// Register new class
			VERIFY(RegisterClassEx(&wc));
		}
		cs.lpszClass = CAMVIEWCLASSNAME;

		return TRUE;
	}
	return FALSE;
}

/*************
 * DESCRIPTION:	cursor position is new center in view
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnCenterView()
{
	CPoint pos;
	GetCursorPos(&pos);

	ScreenToClient(&pos);
	if (Center(pDisplay, pos.x, pos.y))
		Redraw(REDRAW_ALL, FALSE);
}

/************* 
 * DESCRIPTION:	shows current selected object/camera in the status bar
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::UpdateStatusBar()
{
	CMainFrame *pMainFrame;
	OBJECT *pObject;

	GetActiveObjects(&pObject);

	pMainFrame = (CMainFrame*)GetTopLevelFrame();

	if (IsWindow(pMainFrame->m_wndStatusBar.m_hWnd))
	{
		pMainFrame->m_wndStatusBar.SetPaneText(pMainFrame->m_wndStatusBar.CommandToIndex(ID_STATUSBAR_CAMERATXT),
														pDisplay->camera->GetName());
		if (pObject)
			pMainFrame->m_wndStatusBar.SetPaneText(pMainFrame->m_wndStatusBar.CommandToIndex(ID_STATUSBAR_OBJECTTXT), pObject->GetName());
		else
			pMainFrame->m_wndStatusBar.SetPaneText(pMainFrame->m_wndStatusBar.CommandToIndex(ID_STATUSBAR_OBJECTTXT), "");
	}
}

/*************
 * DESCRIPTION:	shows current coordinates
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnShowCoord()
{
	CMainFrame *pMainFrame;

	pMainFrame = (CMainFrame*)GetTopLevelFrame();

	pDisplay->coord_disp ^= TRUE;

	if (IsWindow(pMainFrame->m_wndStatusBar.m_hWnd) && !pDisplay->coord_disp)
	{
		pMainFrame->m_wndStatusBar.SetPaneText(pMainFrame->m_wndStatusBar.CommandToIndex(ID_COORD_X), " ");
		pMainFrame->m_wndStatusBar.SetPaneText(pMainFrame->m_wndStatusBar.CommandToIndex(ID_COORD_Y), " ");
		pMainFrame->m_wndStatusBar.SetPaneText(pMainFrame->m_wndStatusBar.CommandToIndex(ID_COORD_Z), " ");
	}
}

/*************
 * DESCRIPTION:	display coordinates/alignment/scale of current action
 *						(called from mouse.cpp)
 * INPUT:			v	vector contain the x,y,z value to be displayed (can be NULL)
 * OUTPUT:			-
 *************/
void CCamView::SetCoordDisplay(const VECTOR *v)
{
	CMainFrame *pMainFrame;
	char szBuf[30];

	if (!v)
		return;

	pMainFrame = (CMainFrame*)GetTopLevelFrame();

	if (IsWindow(pMainFrame->m_wndStatusBar.m_hWnd))
	{
		sprintf(szBuf, "%g", v->x);
		pMainFrame->m_wndStatusBar.SetPaneText(pMainFrame->m_wndStatusBar.CommandToIndex(ID_COORD_X), szBuf);
		sprintf(szBuf, "%g", v->y);
		pMainFrame->m_wndStatusBar.SetPaneText(pMainFrame->m_wndStatusBar.CommandToIndex(ID_COORD_Y), szBuf);
		sprintf(szBuf, "%g", v->z);
		pMainFrame->m_wndStatusBar.SetPaneText(pMainFrame->m_wndStatusBar.CommandToIndex(ID_COORD_Z), szBuf);
	}
}

/*************
 * DESCRIPTION:	redraws the display
 * INPUT:			nMode		draw mode
 *						bChanged	display changed since editing began
 * OUTPUT:			-
 *************/
void CCamView::Redraw(int nMode, BOOL bChanged)
{
	UpdateStatusBar();

	bCanceled = TRUE;
	pDisplay->Redraw(nMode, bChanged);
}

/*************
 * DESCRIPTION:	handler for refreshing of screen, called from Windows
 * INPUT:			pDC	pointer to device context (not needed)
 * OUTPUT:			-
 *************/
void CCamView::OnDraw(CDC *)
{
	CRect rect;
	gfxOutput oldOutput;

	GetClientRect(rect);
	oldOutput = gfx.SetOutput(GFX_SCREEN);
	if (pDisplay->display != DISPLAY_SOLID)
	{
		gfx.SetClipping(rect.left, rect.top, rect.right, rect.bottom);
		gfx.SetOutput(oldOutput);
		gfx.BitBlt(GFX_MEMORY1, rect.left, rect.top, rect.Width(), rect.Height(), GFX_SCREEN, rect.left, rect.top);
	}
	else
		SwapBuffers(wglGetCurrentDC());

}

/*************
 * DESCRIPTION:	called when window is resized
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CCamView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0 && !bLockOnSizeRedraw)
	{
#ifdef __OPENGL__
		pDisplay->views[0]->opengl.Initialize(this);
		pDisplay->views[1]->opengl.Initialize(this);
		pDisplay->views[2]->opengl.Initialize(this);
		pDisplay->views[3]->opengl.Initialize(this);
#endif // __OPENGL__

		pDisplay->SetViewport(cx, cy);
		sciRedraw();
	}
}

/*************
 * DESCRIPTION:	tests cursor keys and changes x and y according to pressed keys
 * INPUT:			x,y		coordinates to change
 *						code		key code
 *						view		actual view
 * OUTPUT:			TRUE if cursor key pressed else FALSE
 *************/
BOOL CCamView::TestCursorKeys(float *pX, float *pY, UWORD wCode)
{
	VIEW *pView = pDisplay->view;
	
	switch (wCode)
	{
		case VK_LEFT:
			*pX -= (pView->width/pView->zoom)*CURSOR_MOVE;
			return TRUE;
		case VK_RIGHT:
			*pX += (pView->width/pView->zoom)*CURSOR_MOVE;
			return TRUE;
		case VK_UP:
			*pY += (pView->height/pView->zoom)*CURSOR_MOVE;
			return TRUE;
		case VK_DOWN:
			*pY -= (pView->height/pView->zoom)*CURSOR_MOVE;
			return TRUE;
	}
	return FALSE;
}

/*************
 * DESCRIPTION:	ON_WM_KEYDOWN handler
 *						handles movement with cursor keys
 * INPUT:			nChar    Specifies the virtual-key code of the given key.
 *						nRepCnt  Repeat count (the number of times the keystroke 
 *									is repeated as a result of the user holding down the key).
 *						nFlags   Specifies the scan code, key-transition code, previous key 
 *									state, and context code (refer to MFC-Help)
 * OUTPUT:			-
 *************/
void CCamView::OnKeyDown(UINT nChar, UINT, UINT)
{
	VIEW *pView;

	pView = pDisplay->view;

	bCanceled = TRUE;
	
	switch (pView->viewmode)
	{
		case VIEW_FRONT:
			if (!TestCursorKeys(&pView->viewpoint.x, &pView->viewpoint.y, nChar))
				return;
			else
				break;
		case VIEW_RIGHT:
			if (!TestCursorKeys(&pView->viewpoint.z, &pView->viewpoint.y, nChar))
				return;
			else
				break;
		case VIEW_TOP  :
			if (!TestCursorKeys(&pView->viewpoint.x, &pView->viewpoint.z, nChar))
				return;
			else
				break;
		default:
			return;
	}
	sciRedraw();
}

/*************
 * DESCRIPTION:	handles movement of mouse
 *						calls DoMouseMove from mouse.cpp
 * INPUT:			nFlags   Specifies if various flags are set
 *						point    coordinates of mouse cursor
 * OUTPUT:			-
 *************/
void CCamView::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pntScreen;
	RECTANGLE r;

	pntScreen = point;
	ClientToScreen(&point);

	if (!bLeftButton)
		return;

	// return when OnMouseMove was caused from SetCursorPos
	if (pntOldCursor == point)
		return;

	if (pDisplay->editmode & EDIT_SELBOX)
	{
		if (pDisplay->boxmode == BOXMODE_RENDERWINDOW)
			pDisplay->GetRenderRect(&r);
		else
		{
			r.left = pDisplay->view->left;
			r.right = pDisplay->view->right;
			r.top = pDisplay->view->top;
			r.bottom = pDisplay->view->bottom;
		}
		pDisplay->DrawSelBox();
		if (pntScreen.x < r.left)
			pDisplay->selbox.right = r.left;
		else
		{
			if (pntScreen.x > r.right)
				pDisplay->selbox.right = r.right;
			else
				pDisplay->selbox.right = pntScreen.x;
		}
		if (pntScreen.y < r.top)
			pDisplay->selbox.bottom = r.top;
		else
		{
			if (pntScreen.y > r.bottom)
				pDisplay->selbox.bottom = r.bottom;
			else
				pDisplay->selbox.bottom = pntScreen.y;
		}
		pDisplay->DrawSelBox();
	}
	else
	{
		if (pDisplay->editmode & EDIT_ACTIVE)
		{
			if (nFlags & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse_data.oldmousex = pntOldCursor.x;
				mouse_data.oldmousey = pntOldCursor.y;
				DoMouseMove(pDisplay, point.x, point.y, &mouse_data);
		//		Invalidate(FALSE);
				Redraw(REDRAW_OBJECTS, mouse_data.changed);
		
				SetCursorPos(pntOldCursor.x, pntOldCursor.y);

				mouse_data.changed = TRUE;
			}
		}
	}
}

/*************
 * DESCRIPTION:	ON_WM_LBUTTONDOWN handler
 *						handles left mouse button
 *						(selection of object, ...)
 * INPUT:			nFlags	indicates if various keys are pressed
 *						point   Specifies the x- and y-coordinate of the cursor
 * OUTPUT:			-
 *************/
void CCamView::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nFlg;
	
	bLeftButton = TRUE;  // see OnLButtonUp

	SetCapture();
	while (ShowCursor(FALSE) >= 0);

	GetCursorPos(&pntOldCursor);

	nFlg = BUTTON_LEFTDOWN;
	if (nFlags & MK_SHIFT)
		nFlg |= BUTTON_LEFTSHIFT;
	
	HandleMouseButtons(&mouse_data, point.x, point.y, nFlg, &bCanceled);

	UpdateStatusBar();
}

/*************
 * DESCRIPTION:	ON_WM_LBUTTONUP
 *						handles left mouse button click
 * INPUT:			nFlags   Specifies if various flags are set
 *						point    coordinates of mouse cursor
 * OUTPUT:			-
 *************/
void CCamView::OnLButtonUp(UINT nFlags, CPoint point)
{
	int nFlg;
		
	// make sure that OnLButtonDown was called
	if (!bLeftButton)
		return;

	bLeftButton = FALSE;

  	ReleaseCapture();
	while (ShowCursor(TRUE) < 0);

	nFlg = BUTTON_LEFTUP;
	if (nFlags & MK_SHIFT)
		nFlg |= BUTTON_LEFTSHIFT;

	HandleMouseButtons(&mouse_data, point.x, point.y, nFlg, &bCanceled);
	Invalidate(FALSE);
}

/*************
 * DESCRIPTION:	ON_WM_RBUTTONDOWN
 *						handles right mouse button click
 *						display pop up menu
 * INPUT:			nFlags   Specifies if various flags are set
 *						point    coordinates of mouse cursor
 * OUTPUT:			-
 *************/
void CCamView::OnRButtonDown(UINT nFlags, CPoint point)
{
	int nFlg;

	nFlg = BUTTON_RIGHTDOWN;
	if (nFlags & MK_SHIFT)
		nFlg |= BUTTON_LEFTSHIFT;

	HandleMouseButtons(&mouse_data, point.x, point.y, nFlg, &bCanceled);
}

/*************
 * DESCRIPTION:	ON_WM_RBUTTONDOWN
 *						handles right mouse button up
 *						display pop up menu
 * INPUT:			nFlags   Specifies if various flags are set
 *						point    coordinates of mouse cursor
 * OUTPUT:			-
 *************/
void CCamView::OnRButtonUp(UINT nFlags, CPoint point)
{
	int nFlg;

	nFlg = BUTTON_RIGHTUP;
	if (nFlags & MK_SHIFT)
		nFlg |= BUTTON_LEFTSHIFT;

	HandleMouseButtons(&mouse_data, point.x, point.y, nFlg, &bCanceled);
}

/*************
 * DESCRIPTION:	test if user canceled drawing operation
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL TestBreaked()
{
	BOOL bCancel;
	MSG msg;

	bCancel = FALSE;
	// peek a message and test of what sort it is
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		switch (msg.message)
		{
			case WM_KEYDOWN: bCancel = TRUE;
		}
		DispatchMessage(&msg);
	}
	return bCancel;
}

// CCamView diagnostics
#ifdef _DEBUG
void CCamView::AssertValid() const
{
	CView::AssertValid();
}

void CCamView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

