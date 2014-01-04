/***************
 * MODULE:			CamView.h
 * PROJECT:			Scenario
 * DESCRIPTION:	camera window definition file
 * AUTHORS:			Mike Hesser, Andreas Heumann
 * HISTORY:       
 *				DATE		NAME	COMMENT
 *				10.12.95	ah		initial release
 ***************/
 
#ifndef CAMVIEW_H
#define CAMVIEW_H

#include "display.h"
#include "view.h"
#include "mouse.h"
#include "prefs.h"
#include "object.h"
#include "resource.h"
#include "MaterialView.h"
class CBrowserView;
#include "BrowserView.h"
#include "sci.h"
#include "utility.h"
#include "RendDlg.h"

#define VIEW_FOUR 16
#define MENU_MAXSTATES		10
#define TOOLBAR_MAXSTATES	5

class CCamView : public CView
{
public:
	CCamView();
	virtual ~CCamView();

	DECLARE_DYNCREATE(CCamView)

	virtual	void OnDraw(CDC *);
	
#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext&) const;
#endif

	CBrowserView *pBrowserView;
	CMatView *pMatView;
	DISPLAY *pDisplay;
	BOOL bMenuStates[MENU_MAXSTATES];
	BOOL bToolbarStates[TOOLBAR_MAXSTATES];
	BOOL bLockOnSizeRedraw;

	void Redraw(int, BOOL);

	void SetCoordDisplay(const VECTOR *);
	void UpdateStatusBar();
	void MaterialDlg(SURFACE *, OBJECT *);

protected:
	CPoint		pntOldCursor;
	MOUSEDATA	mouse_data;
	BOOL			bLeftButton;

	BOOL TestCursorKeys(float *,float *, UWORD);

	// mouse handler
	afx_msg void OnKeyDown(UINT, UINT, UINT);
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT, CPoint);
	afx_msg void OnRButtonDown(UINT, CPoint);
	afx_msg void OnRButtonUp(UINT, CPoint);
	afx_msg BOOL OnEraseBkgnd(CDC *);
	virtual BOOL PreCreateWindow(CREATESTRUCT &);
	
	// dialogs
	afx_msg void OnAttributes(); 			// surfdlg.cpp
	afx_msg void OnGridsize();				// griddlg.cpp
	afx_msg void OnSettings();				// transdlg.cpp
	afx_msg void OnGlobalSet();			// gendlg.cpp
	afx_msg void OnPrefSet();				// prefdlg.cpp
	afx_msg void OnRenderSettings();		// renddlg.cpp
	afx_msg void OnRender();				// renddlg.cpp
	afx_msg void OnActiveCamera();		// CameraDlg.cpp
	afx_msg void OnSetActiveCamera();	// camdlg.cpp
	afx_msg void OnSetRenderWindow()		{ sciDoMethod(SCIM_SETRENDERWINDOW); }
	afx_msg void OnToScene()				{ sciDoMethod(SCIM_OBJECT2SCENE); }
	afx_msg void OnNewMaterial()			{ pMatView->OnNewMaterial(); }
	afx_msg void OnLoadNewMaterial()		{ pMatView->OnLoadNewMaterial(); }
	afx_msg void OnRemoveUnused()			{ pMatView->OnRemoveUnused(); }
	afx_msg void OnRemoveIdentical()		{ pMatView->OnRemoveIdentical(); }

	// Menu & Toolbar
	afx_msg void OnCenterView();
	afx_msg void OnGridOnOff()	 			{ sciDoMethod(SCIM_GRIDONOFF); }
	afx_msg void OnSnapToGrid() 			{ sciDoMethod(SCIM_GRIDSNAP); }
	afx_msg void OnShowCoord();
	afx_msg void OnShowName()	 			{ sciDoMethod(SCIM_SHOWNAMES); }
	afx_msg void OnShowAll()				{ sciDoMethod(SCIM_SHOWALL); }
	afx_msg void OnShowCamera()			{ sciDoMethod(SCIM_SHOWCAMERA); }
	afx_msg void OnShowBrush() 			{ sciDoMethod(SCIM_SHOWBRUSHES); }
	afx_msg void OnShowTexture()			{ sciDoMethod(SCIM_SHOWTEXTURES); }
	afx_msg void OnShowLight()				{ sciDoMethod(SCIM_SHOWLIGHT); }
	afx_msg void OnShowMesh()				{ sciDoMethod(SCIM_SHOWMESH); }
	afx_msg void OnShowSphere()			{ sciDoMethod(SCIM_SHOWSPHERE); }
	afx_msg void OnShowBox()				{ sciDoMethod(SCIM_SHOWBOX); }
	afx_msg void OnShowCylinder()			{ sciDoMethod(SCIM_SHOWCYLINDER); }
	afx_msg void OnShowCone()				{ sciDoMethod(SCIM_SHOWCONE); }
	afx_msg void OnShowSOR()				{ sciDoMethod(SCIM_SHOWSOR); }
	afx_msg void OnShowPlane()				{ sciDoMethod(SCIM_SHOWPLANE); }
	afx_msg void OnShowCSG()				{ sciDoMethod(SCIM_SHOWCSG); }
	afx_msg void OnLocal()					{ sciDoMethod(SCIM_EDIT_LOCAL); }
	afx_msg void OnMoveWorld()				{ sciDoMethod(SCIM_EDITMOVEWORLD); }
	afx_msg void OnRotateWorld()			{ sciDoMethod(SCIM_EDITROTATEWORLD); }
	afx_msg void OnZoomWorld()				{ sciDoMethod(SCIM_EDITSCALEWORLD); }
	afx_msg void OnMoveObject()			{ sciDoMethod(SCIM_EDITMOVEOBJECT); }
	afx_msg void OnRotateObject()			{ sciDoMethod(SCIM_EDITROTATEOBJECT); }
	afx_msg void OnSizeObject()			{ sciDoMethod(SCIM_EDITSCALEOBJECT); }
	afx_msg void OnCameraToViewer()		{ sciDoMethod(SCIM_CAMERA2VIEWER); }
	afx_msg void OnCut()						{ sciDoMethod(SCIM_CUT); }
	afx_msg void OnCopy()					{ sciDoMethod(SCIM_COPY); }
	afx_msg void OnPaste()					{ sciDoMethod(SCIM_PASTE); }
	afx_msg void OnUndo()					{ sciDoMethod(SCIM_UNDO); }
	afx_msg void OnRedo()					{ sciDoMethod(SCIM_REDO); }
	afx_msg void OnDelete()					{ sciDoMethod(SCIM_DELETE); }
	afx_msg void OnTop() 					{ sciDoMethod(SCIM_VIEWTOP); }
	afx_msg void OnFront()					{ sciDoMethod(SCIM_VIEWFRONT); }
	afx_msg void OnRight()					{ sciDoMethod(SCIM_VIEWRIGHT); }
	afx_msg void OnCamera()					{ sciDoMethod(SCIM_VIEWCAMERA); }
	afx_msg void OnPerspective() 			{ sciDoMethod(SCIM_VIEWPERSP); }
	afx_msg void OnQuadview()				{ sciDoMethod(SCIM_VIEWQUAD); }
	afx_msg void OnRedraw()					{ sciDoMethod(SCIM_REDRAW); }
	afx_msg void OnFocus()					{ sciDoMethod(SCIM_FOCUS); }
	afx_msg void OnZoomFit()				{ sciDoMethod(SCIM_ZOOMFIT); }
	afx_msg void OnZoomIn()					{ sciDoMethod(SCIM_ZOOMIN); }
	afx_msg void OnZoomOut()				{ sciDoMethod(SCIM_ZOOMOUT); }
	afx_msg void OnBounding()				{ sciDoMethod(SCIM_DISPLAYBBOX); }
	afx_msg void OnWire()					{ sciDoMethod(SCIM_DISPLAYWIRE); }
	afx_msg void OnSolid()					{ sciDoMethod(SCIM_DISPLAYSOLID); }
	afx_msg void OnSelectAll()				{ sciDoMethod(SCIM_SELALL); }
	afx_msg void OnDeselectAll()			{ sciDoMethod(SCIM_DESELALL); }
	afx_msg void OnSelectByBox()			{ sciDoMethod(SCIM_SELECTBYBOX); }

public:	
	afx_msg void OnSelectPrev()			{ sciDoMethod(SCIM_SELPREV); }
	afx_msg void OnSelectNext()			{ sciDoMethod(SCIM_SELNEXT); }	

protected:
	afx_msg void OnEditPoints()			{ sciDoMethod(SCIM_EDITPOINTS); }
	afx_msg void OnAddPoints()				{ sciDoMethod(SCIM_ADDPOINTS); }
	afx_msg void OnEditEdges()				{ sciDoMethod(SCIM_EDITEDGES); }
	afx_msg void OnAddEdges()				{ sciDoMethod(SCIM_ADDEDGES); }
	afx_msg void OnEditFaces()				{ sciDoMethod(SCIM_EDITFACES); }
	afx_msg void OnAddFaces()				{ sciDoMethod(SCIM_ADDFACES); }
	afx_msg void OnFilterX()				{ sciDoMethod(SCIM_TOGGLEACTIVEX); }
	afx_msg void OnFilterY()				{ sciDoMethod(SCIM_TOGGLEACTIVEY); }
	afx_msg void OnFilterZ()				{ sciDoMethod(SCIM_TOGGLEACTIVEZ); }
	afx_msg void OnFilterShiftX()			{ sciDoMethod(SCIM_UNSETACTIVEX); }
	afx_msg void OnFilterShiftY()			{ sciDoMethod(SCIM_UNSETACTIVEY); }
	afx_msg void OnFilterShiftZ()			{ sciDoMethod(SCIM_UNSETACTIVEZ); }

	// update routines
	afx_msg int  OnCreate(LPCREATESTRUCT);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnUndoUI(CCmdUI *pUI)				{ pUI->Enable(bMenuStates[MENU_UNDO]); }
	afx_msg void OnRedoUI(CCmdUI *pUI)				{ pUI->Enable(bMenuStates[MENU_REDO]); }
	afx_msg void OnCutUI(CCmdUI *pUI)				{ pUI->Enable(bMenuStates[MENU_CUT]); }
	afx_msg void OnCopyUI(CCmdUI *pUI)				{ pUI->Enable(bMenuStates[MENU_COPY]); }
	afx_msg void OnPasteUI(CCmdUI *pUI)				{ pUI->Enable(bMenuStates[MENU_PASTE]); }
	afx_msg void OnEditPointsUI(CCmdUI *pUI)		{ pUI->SetCheck((pDisplay->editmode & (EDIT_EDITPOINTS | EDIT_MESH)) == (EDIT_EDITPOINTS | EDIT_MESH)); }
	afx_msg void OnAddPointsUI(CCmdUI *pUI)		{ pUI->SetCheck((pDisplay->editmode & (EDIT_ADDPOINTS | EDIT_MESH)) == (EDIT_ADDPOINTS | EDIT_MESH)); }
	afx_msg void OnEditEdgesUI(CCmdUI *pUI)		{ pUI->SetCheck((pDisplay->editmode & (EDIT_EDITEDGES | EDIT_MESH)) == (EDIT_EDITEDGES | EDIT_MESH)); }
	afx_msg void OnAddEdgesUI(CCmdUI *pUI)			{ pUI->SetCheck((pDisplay->editmode & (EDIT_ADDEDGES | EDIT_MESH)) == (EDIT_ADDEDGES | EDIT_MESH)); }
	afx_msg void OnEditFacesUI(CCmdUI *pUI)		{ pUI->SetCheck((pDisplay->editmode & (EDIT_EDITFACES | EDIT_MESH)) == (EDIT_EDITFACES | EDIT_MESH)); }
	afx_msg void OnAddFacesUI(CCmdUI *pUI)			{ pUI->SetCheck((pDisplay->editmode & (EDIT_ADDFACES | EDIT_MESH)) == (EDIT_ADDFACES | EDIT_MESH)); }
	afx_msg void OnBoundingUI(CCmdUI *pUI)			{ pUI->SetCheck(pDisplay->display == DISPLAY_BBOX); }
	afx_msg void OnWireUI(CCmdUI *pUI) 				{ pUI->SetCheck(pDisplay->display == DISPLAY_WIRE); }
	afx_msg void OnSolidUI(CCmdUI *pUI)				{ pUI->SetCheck(pDisplay->display == DISPLAY_SOLID); }
	afx_msg void OnSelectUI(CCmdUI *pUI) 			{ pUI->SetCheck((pDisplay->editmode & EDIT_SELBOX) && (pDisplay->boxmode == BOXMODE_SELECT)); }
	afx_msg void OnLocalUI(CCmdUI *pUI) 			{ pUI->SetCheck(pDisplay->localpos); }
	afx_msg void OnFilterXUI(CCmdUI *pUI) 			{ pUI->SetCheck((pDisplay->active & ACTIVE_X) == ACTIVE_X); }
	afx_msg void OnFilterYUI(CCmdUI *pUI) 			{ pUI->SetCheck((pDisplay->active & ACTIVE_Y) == ACTIVE_Y); }
	afx_msg void OnFilterZUI(CCmdUI *pUI) 			{ pUI->SetCheck((pDisplay->active & ACTIVE_Z) == ACTIVE_Z); }
	afx_msg void OnFilterShiftXUI(CCmdUI *pUI) 	{ pUI->SetCheck((pDisplay->active & ACTIVE_X) == ACTIVE_X); }
	afx_msg void OnFilterShiftYUI(CCmdUI *pUI) 	{ pUI->SetCheck((pDisplay->active & ACTIVE_Y) == ACTIVE_Y); }
	afx_msg void OnFilterShiftZUI(CCmdUI *pUI) 	{ pUI->SetCheck((pDisplay->active & ACTIVE_Z) == ACTIVE_Z); }
	afx_msg void OnGridOnOffUI(CCmdUI *pUI) 		{ pUI->SetCheck(pDisplay->grid); }
	afx_msg void OnSnapToGridUI(CCmdUI *pUI) 		{ pUI->SetCheck(pDisplay->gridsnap); }
	afx_msg void OnMoveWorldUI(CCmdUI *pUI) 		{ pUI->SetRadio(((pDisplay->editmode & EDIT_MASK) == EDIT_MOVE) && !(pDisplay->editmode & EDIT_OBJECT)); }
	afx_msg void OnRotateWorldUI(CCmdUI *pUI) 	{ pUI->SetRadio(((pDisplay->editmode & EDIT_MASK) == EDIT_ROTATE) && !(pDisplay->editmode & EDIT_OBJECT)); }
	afx_msg void OnZoomWorldUI(CCmdUI *pUI) 		{ pUI->SetRadio(((pDisplay->editmode & EDIT_MASK) == EDIT_SCALE) && !(pDisplay->editmode & EDIT_OBJECT)); }
	afx_msg void OnMoveObjectUI(CCmdUI *pUI) 		{ pUI->SetRadio(((pDisplay->editmode & EDIT_MASK) == EDIT_MOVE) && (pDisplay->editmode & EDIT_OBJECT)); }
	afx_msg void OnRotateObjectUI(CCmdUI *pUI)	{ pUI->SetRadio(((pDisplay->editmode & EDIT_MASK) == EDIT_ROTATE) && (pDisplay->editmode & EDIT_OBJECT)); }
	afx_msg void OnSizeObjectUI(CCmdUI *pUI) 		{ pUI->SetRadio(((pDisplay->editmode & EDIT_MASK) == EDIT_SCALE) && (pDisplay->editmode & EDIT_OBJECT)); }
	afx_msg void OnTopUI(CCmdUI *pUI) 				{ pUI->SetRadio(pDisplay->view->viewmode == VIEW_TOP); }
	afx_msg void OnFrontUI(CCmdUI *pUI) 			{ pUI->SetRadio(pDisplay->view->viewmode == VIEW_FRONT); }
	afx_msg void OnRightUI(CCmdUI *pUI) 			{ pUI->SetRadio(pDisplay->view->viewmode == VIEW_RIGHT); }
	afx_msg void OnPerspectiveUI(CCmdUI *pUI)		{ pUI->SetRadio(pDisplay->view->viewmode == VIEW_PERSP); }
	afx_msg void OnCameraUI(CCmdUI *pUI)			{ pUI->SetRadio(pDisplay->view->viewmode == VIEW_CAMERA); }
	afx_msg void OnQuadviewUI(CCmdUI *pUI)			{ pUI->SetCheck(pDisplay->multiview); }
	afx_msg void OnShowCoordUI(CCmdUI *pUI)		{ pUI->SetCheck(pDisplay->coord_disp); }
	afx_msg void OnShowNameUI(CCmdUI *pUI)			{ pUI->SetCheck(pDisplay->name_disp); }
	afx_msg void OnShowAllUI(CCmdUI *pUI)			{ pUI->SetCheck((pDisplay->filter_flags & FILTER_ALL) == FILTER_ALL); }
	afx_msg void OnShowCameraUI(CCmdUI *pUI)		{ pUI->SetCheck((pDisplay->filter_flags & FILTER_CAMERA) == FILTER_CAMERA); }
	afx_msg void OnShowBrushUI(CCmdUI *pUI)		{ pUI->SetCheck((pDisplay->filter_flags & FILTER_BRUSH) == FILTER_BRUSH); }
	afx_msg void OnShowTextureUI(CCmdUI *pUI)		{ pUI->SetCheck((pDisplay->filter_flags & FILTER_TEXTURE) == FILTER_TEXTURE); }
	afx_msg void OnShowLightUI(CCmdUI *pUI)		{ pUI->SetCheck((pDisplay->filter_flags & FILTER_LIGHT) == FILTER_LIGHT); }
	afx_msg void OnShowMeshUI(CCmdUI *pUI)			{ pUI->SetCheck((pDisplay->filter_flags & FILTER_MESH) == FILTER_MESH); }
	afx_msg void OnShowSphereUI(CCmdUI *pUI)		{ pUI->SetCheck((pDisplay->filter_flags & FILTER_SPHERE) == FILTER_SPHERE); }
	afx_msg void OnShowBoxUI(CCmdUI *pUI)			{ pUI->SetCheck((pDisplay->filter_flags & FILTER_BOX) == FILTER_BOX); }
	afx_msg void OnShowCylinderUI(CCmdUI *pUI)	{ pUI->SetCheck((pDisplay->filter_flags & FILTER_CYLINDER) == FILTER_CYLINDER); }
	afx_msg void OnShowConeUI(CCmdUI *pUI)			{ pUI->SetCheck((pDisplay->filter_flags & FILTER_CONE) == FILTER_CONE); }
	afx_msg void OnShowSORUI(CCmdUI *pUI)			{ pUI->SetCheck((pDisplay->filter_flags & FILTER_SOR) == FILTER_SOR); }
	afx_msg void OnShowPlaneUI(CCmdUI *pUI)		{ pUI->SetCheck((pDisplay->filter_flags & FILTER_PLANE) == FILTER_PLANE); }
	afx_msg void OnShowCSGUI(CCmdUI *pUI)			{ pUI->SetCheck((pDisplay->filter_flags & FILTER_CSG) == FILTER_CSG); }

	DECLARE_MESSAGE_MAP()
};

BOOL TestBreaked();

#endif
