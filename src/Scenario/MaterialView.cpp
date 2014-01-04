/***************
 * MODULE:			MaterialView.cpp
 * PROJECT:			Scenario
 * DESCRIPTION:	view
 * AUTHORS:			Mike Hesser
 * HISTORY:       
 *	DATE		NAME	COMMENT
 *	10.05.97	mh		first version
 * 26.06.97 mh		CRenameDlg
 ***************/

#include "Typedefs.h"
#include "sci.h"
#include "MaterialView.h"
#include "resource.h"
#include "Preview.h"
#include "Sphere.h"
#include "SurfDlg.h"
#include "Document.h"
#include "Utility.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MATWIDTH 50
#define MATHEIGHT 50
#define MATTOTALHEIGHT (MATHEIGHT + 20)

IMPLEMENT_DYNCREATE(CMatView, CScrollView)

BEGIN_MESSAGE_MAP(CMatView, CScrollView)
	ON_COMMAND(IDM_MATERIAL_NEW,	OnNewMaterial)
	ON_COMMAND(IDM_MATERIAL_LOADNEW,	OnLoadNewMaterial)
	ON_COMMAND(IDM_MATERIAL_LOAD,	OnLoadMaterial)
	ON_COMMAND(IDM_MATERIAL_SAVE,	OnSaveMaterial)
	ON_COMMAND(IDM_MATERIAL_ASSIGN, OnAssign)
	ON_COMMAND(IDM_MATERIAL_ATTRIBUTES, OnAttributes)
	ON_COMMAND(IDM_MATERIAL_RENAME, OnRename)
	ON_COMMAND(IDM_MATERIAL_DELETE, OnDelete)
	ON_COMMAND(IDM_MATERIAL_REMOVEUNUSED, OnRemoveUnused)
	ON_COMMAND(IDM_MATERIAL_REMOVEIDENTICAL, OnRemoveIdentical)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

CPalette *pPal;
CMatView *pMatWnd;

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CMatView::CMatView()
{
	SetScrollSizes(MM_TEXT, CSize(0, 0), CSize(MATWIDTH, MATTOTALHEIGHT), CSize(MATWIDTH, MATTOTALHEIGHT));
	nSelectedMaterial = -1;
	m_nIDDrag = RegisterClipboardFormat(_T("MatView"));
	pMatWnd = this;
}

/*************
 * DESCRIPTION:	destructor, deletes all materials
 * INPUT:			-
 * OUTPUT:			-
 *************/
CMatView::~CMatView()
{
	int i;

	for (i = 0; i < aMaterials.GetSize(); i++)
		delete (CMaterial*)(aMaterials[i]);
}

/*************
 * DESCRIPTION:	update material with the given surface
 * INPUT:			pSurf		surface to be updated
 * OUTPUT:			-
 *************/
void CMatView::UpdateMaterial(SURFACE *pSurf)
{
	int i;

	ASSERT(pSurf);

	for (i = 0; i < aMaterials.GetSize(); i++)
	{
		if (pSurf == ((CMaterial*)aMaterials[i])->pSurface)
		{
			((CMaterial*)aMaterials[i])->Update();
			break;
		}
	}
}

/*************
 * DESCRIPTION:	free all materials
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::FreeMaterials()
{
	aMaterials.RemoveAll();
	Invalidate(FALSE);
}

/*************
 * DESCRIPTION:	side-step CView's implementation since we don't want to activate the view
 * INPUT:			(system)
 * OUTPUT:			-
 *************/
int CMatView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

/*************
 * DESCRIPTION:	delete background
 * INPUT:			(system)
 * OUTPUT:			-
 *************/
BOOL CMatView::OnEraseBkgnd(CDC *pDC)
{
	CRect rect;

	GetClientRect(rect);
	pDC->FillRect(rect, CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));

	return TRUE;
}

/*************
 * DESCRIPTION:	Add a new default material to the material list
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::OnNewMaterial()
{
	SURFACE *pSurf;
	
	pSurf = sciCreateSurface(SURF_DEFAULTNAME);
	if (!pSurf)
		return;

	AddMaterial(pSurf);
}

/*************
 * DESCRIPTION:	Load a material and add it to the material list
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::OnLoadNewMaterial()
{
	CString file;
	SURFACE *surf;
	
	surf = sciCreateSurface(SURF_DEFAULTNAME);
	if (!surf)
		return;

	if (sciLoadMaterial(surf))
		AddMaterial(surf);
	else
		sciDeleteSurface(surf);
}

/*************
 * DESCRIPTION:	Load a material and assign it to the selected material
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::OnLoadMaterial()
{
	CString file;
	SURFACE *pSurf;
	CMaterial *pMaterial;
	char *szErr;

	if (!utility.FileRequ((char*)LPCTSTR(file), FILEREQU_MATERIAL, FILEREQU_INITIALFILE))
		return;

	pSurf = sciCreateSurface(NULL);
	if (!pSurf)
		return;

	szErr = pSurf->Load((char*)LPCTSTR(file));
	if (szErr)
	{
		delete pSurf;
		utility.Request(szErr);
	}
	else
	{
		if (nSelectedMaterial != -1)
		{
			pMaterial = ((CMaterial*)aMaterials[nSelectedMaterial]);
			delete pMaterial->pSurface;
			pMaterial->pSurface = pSurf;
			pMaterial->Update();
			Invalidate(FALSE);
		}
	}
}

/*************
 * DESCRIPTION:	Save the selected material
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::OnSaveMaterial()
{
	CString sFile;
	char *szErr;

	if (nSelectedMaterial >= 0)
	{
		if (utility.FileRequ((char*)LPCTSTR(sFile), FILEREQU_MATERIAL, FILEREQU_SAVE))
		{
			szErr = ((CMaterial*)(aMaterials[nSelectedMaterial]))->pSurface->Save((char*)LPCTSTR(sFile));
			if (szErr)
				utility.Request(szErr);
		}
	}
}

/*************
 * DESCRIPTION:	Open Material dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::OnAttributes()
{
	if (nSelectedMaterial != -1)
	{
		sciOpenMaterialRequ(((CMaterial*)aMaterials[nSelectedMaterial])->pSurface);
	}
}

/*************
 * DESCRIPTION:	Assign the selected material to the current selected object(s)
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::OnAssign()
{
	if (nSelectedMaterial == -1)
		return;

//		if (cur_selected)
	sciAssignSurfaceToSelectedObjects(((CMaterial*)aMaterials[nSelectedMaterial])->pSurface);
//		else
//			Request(IDS_ERRNOOBJSELECT);
}

/*************
 * DESCRIPTION:	Rename current material
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::OnRename()
{
	CRenameDlg dialog;
	CMaterial *pMat;

	if (nSelectedMaterial == -1)
		return;
	
	pMat = ((CMaterial*)aMaterials[nSelectedMaterial]);
	dialog.m_sName = pMat->pSurface->name;
	if (dialog.DoModal() == IDOK)
	{
		pMat->pSurface->SetName((char*)LPCTSTR(dialog.m_sName));
		pMat->Update();
		Invalidate(FALSE);
	}
}

/*************
 * DESCRIPTION:	Delete selected material
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::OnDelete()
{
	CMaterial *pM;

	if (nSelectedMaterial == -1)
		return;

	pM = ((CMaterial*)aMaterials[nSelectedMaterial]);

	// delete only if it is not needed
	if (sciRemoveMaterial(pM->pSurface))
	{
		sciDeleteSurface(pM->pSurface);
		delete pM;
		aMaterials.RemoveAt(nSelectedMaterial);
		Invalidate(TRUE);
	}
}

/*************
 * DESCRIPTION:	Add new material to the material list
 * INPUT:			surf	surface of the new material
 * OUTPUT:			-
 *************/
BOOL CMatView::AddMaterial(SURFACE *surf)
{
	CMaterial *pMat;
	CDC *pDC;

	if (IsWindow(m_hWnd))
	{
		pDC = GetDC();
		if (!pDC)
			return FALSE;
	}
	else
		pDC = NULL;

	//pPal = m_pPalette;

	pMat = new CMaterial(pDC, surf);
	if (pDC)
		ReleaseDC(pDC);
	if (!pMat)
		return FALSE;

	/* uncomment this to have a sorted array	
	int i = 0;
	while (i < aMaterials.GetSize() && 
			 stricmp(((CMaterial*)aMaterials[i])->pSurface->name, 
			 surf->name) < 0)
	{
		i++;
	}
	aMaterials.InsertAt(i, pMat);*/
	aMaterials.Add(pMat);
	nSelectedMaterial = aMaterials.GetSize() - 1; //i

	Invalidate(FALSE);

	return TRUE;
}

/*************
 * DESCRIPTION:	remove materials which are not used
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::OnRemoveUnused()
{
	OBJECT *pObject;
	SURFACE *pSurf;
	int i;

	i = 0;
	while (i < aMaterials.GetSize())
	{
		pSurf = ((CMaterial*)aMaterials[i])->pSurface;
		pObject = IsSurfaceUsed(pSurf);
		if (!pObject)
		{
			// nobody needs this surface, delete it
			if (sciRemoveMaterial(pSurf))
			{
				sciDeleteSurface(pSurf);
				delete aMaterials[i];
				aMaterials.RemoveAt(i);
				i--;
			}
		}
		i++;
	}
	Invalidate(TRUE);
}

/*************
 * DESCRIPTION:	remove identical materials
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMatView::OnRemoveIdentical()
{
	SURFACE *pSurf, *pIdent;
	int i;

	i = 0;
	while (i < aMaterials.GetSize())
	{
		pSurf = ((CMaterial*)aMaterials[i])->pSurface;
		pIdent = pSurf->FindIdentical();
		if (pIdent)
		{
			if (!(pIdent->flags & SURF_DEFAULT))
			{
				// we have found an identical surface
				// first assign the current surface to all objects with this identical surface
				sciChangeSurface(pIdent, pSurf);
				// then remove the surface
				if(sciRemoveMaterial(pIdent))
				{
					sciDeleteSurface(pSurf);
					delete aMaterials[i];
					aMaterials.RemoveAt(i);
					i--;
				}
			}
		}
		i++;
	}
	Invalidate(TRUE);
}

/*************
 * DESCRIPTION:	handler for refreshing of screen, called from Windows
 * INPUT:			pDC	pointer to device context
 * OUTPUT:			-
 *************/
void CMatView::OnDraw(CDC *pDC)
{
	CPalette *pOldPalette;
	CRect rect;
	int i, x, y;

	pOldPalette = pDC->SelectPalette(m_pPalette, TRUE);
	pDC->RealizePalette();

	GetClientRect(rect);

	i = 0;
	x = y = 0;
	
	for (i = 0; i < aMaterials.GetSize(); i++)
	{
		((CMaterial*)aMaterials[i])->Show(pDC, x, y);

		x += MATWIDTH;
		if (x + MATWIDTH > rect.right && i < aMaterials.GetSize() - 1)
		{
			x = 0;
			y += MATTOTALHEIGHT;
		}
	}
	SetScrollSizes(MM_TEXT, CSize(rect.Width() - MATWIDTH, y + MATTOTALHEIGHT), CSize(MATWIDTH/2, MATTOTALHEIGHT/2), CSize(MATWIDTH/2, MATTOTALHEIGHT/2));
	
	pDC->SelectPalette(pOldPalette, TRUE);
}

/*************
 * DESCRIPTION:	ON_WM_LBUTTONDOWN
 *						handles left mouse button click
 * INPUT:			nFlags   Specifies if various flags are set
 *						point    coordinates of mouse cursor
 * OUTPUT:			-
 *************/
void CMatView::OnLButtonDown(UINT, CPoint point)
{
	/* TODO: Drag and Drop
		  
	STGMEDIUM st;
	COleDataSource srcItem;
	HGLOBAL hMem;

	hMem = GlobalAlloc(GMEM_FIXED, 4);
	srcItem.CacheGlobalData(m_nIDDrag, hMem);
   GlobalFree(hMem);
CRect rect(0,0,3000,3000);
	DROPEFFECT dropEffect = srcItem.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE, NULL);
	DROPEFFECT DoDragDrop( DWORD dwEffects = DROPEFFECT_COPY|DROPEFFECT_MOVE|DROPEFFECT_LINK, LPCRECT lpRectStartDrag = NULL, COleDropSource* pDropSource = NULL );

	if (dropEffect>0)
MessageBox("asd","asdf");
	*/
}

DROPEFFECT CMatView::OnDragEnter(COleDataObject* pDataObject, DWORD grfKeyState, CPoint point)
{
		return DROPEFFECT_COPY;
}

BOOL CMatView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	return TRUE;
}

DROPEFFECT CMatView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return DROPEFFECT_COPY;
}

void CMatView::OnDragLeave()
{
}

/*************
 * DESCRIPTION:	ON_WM_LBUTTONDBLCLK
 *						handles left mouse button double click
 *						Open Material dialog.
 *						display pop up menu
 * INPUT:			nFlags   Specifies if various flags are set
 *						point    coordinates of mouse cursor
 * OUTPUT:			-
 *************/
void CMatView::OnLButtonDblClk(UINT, CPoint point)
{
	CSurfaceSheet dialog(this);
	int index;

	index = GetMaterialIndex(point);

	if (index >= 0)
	{
		nSelectedMaterial = index;
		OnAttributes();
	}
}

/*************
 * DESCRIPTION:	ON_WM_RBUTTONDOWN
 *						handles right mouse button click
 *						display pop up menu
 * INPUT:			nFlags   Specifies if various flags are set
 *						point    coordinates of mouse cursor
 * OUTPUT:			-
 *************/
void CMatView::OnRButtonDown(UINT, CPoint point)
{
	CMenu Menu;
	int index;
	BOOL b;
	
	index = GetMaterialIndex(point);
	if (index != -1)
	{
		nSelectedMaterial = index;
		b = Menu.LoadMenu(IDR_MATERIAL);
	}
	else
		b = Menu.LoadMenu(IDR_MATERIAL_OUTSIDE);

	ClientToScreen(&point);
	if (b)
	{
		CMenu *pSubMenu = Menu.GetSubMenu(0);
		if (pSubMenu != NULL)
			pSubMenu->TrackPopupMenu(TPM_CENTERALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

/*************
 * DESCRIPTION:	Get index of material at the given point
 * INPUT:			point    coordinates of mouse cursor
 * OUTPUT:			index of material (-1 if user clicked outside)
 *************/
int CMatView::GetMaterialIndex(CPoint point)
{
	CRect rect;
	int index;

	GetClientRect(rect);
	
	point += GetScrollPosition();

	index = int(point.x / MATWIDTH) + int(point.y / MATTOTALHEIGHT)*int(rect.Width()/MATWIDTH);
	if (index < 0 || index >= aMaterials.GetSize())
		index = -1;

	return index;
}

// CMatView diagnostics
#ifdef _DEBUG
void CMatView::AssertValid() const
{
	CView::AssertValid();
}

void CMatView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG


/*************
 * DESCRIPTION:	constructor
 * INPUT:			
 * OUTPUT:			-
 *************/
CMaterial::CMaterial(CDC *pDC, SURFACE *surf)
{
	CFont font;

	ASSERT(surf != NULL);

	pSurface = surf;
	
	Bitmap.CreateCompatibleBitmap(pDC, MATWIDTH, MATTOTALHEIGHT);
	MemDC.CreateCompatibleDC(pDC);
	MemDC.SelectObject(&Bitmap);
	MemDC.SetViewportOrg(0, 0);
	MemDC.SetWindowOrg(0, 0);
	MemDC.SetMapMode(MM_TEXT);
	if (font.CreatePointFont(90, "Helv"))
		MemDC.SelectObject(&font);
	MemDC.SelectPalette(pPal, TRUE);
	MemDC.RealizePalette();

	LOGBRUSH logbrush;
	CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH))->GetLogBrush(&logbrush);
	MemDC.SetBkColor(logbrush.lbColor);

	bThread = FALSE;
	MaterialStruct.hThread = NULL;

	Update();
	return;
}

/*************
 * DESCRIPTION:	destructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CMaterial::~CMaterial()
{
	if (MaterialStruct.hThread)
		CloseHandle(MaterialStruct.hThread);
	MaterialStruct.hThread = NULL;

	Bitmap.DeleteObject();
	MemDC.DeleteDC();
}

/*************
 * DESCRIPTION:	thread for drawing material
 * INPUT:			arg	pointer to PREVIEWSTRUCT
 * OUTPUT:			exit code
 *************/
DWORD WINAPI MaterialThread(LPVOID pMat_)
{
	MATSTRUCT *pMat = (MATSTRUCT*)pMat_;
	PREVIEW preview;
	COLORREF col;
	rsiSMALL_COLOR *line;
	int x, y;

	CRect rect(0, 0, MATWIDTH, MATTOTALHEIGHT);
	pMat->pDC->FillRect(rect, CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));
	pMat->pDC->TextOut(0, MATHEIGHT, pMat->surf->name);

	if (!preview.Init(MATWIDTH - 1, MATHEIGHT - 1, MATHEIGHT - 1, pMat->surf, NULL))
		return 1;
	
	preview.RenderLines(0);
	line = preview.line;
	y = 0;
	while (y < MATHEIGHT - 1)
	{
		for (x = 0; x < MATWIDTH - 1; x++)
		{
			col = RGB(line->r, line->g, line->b);
			pMat->pDC->SetPixelV(x, y, col);
			line++;
		}
		y++;
	}
	pMatWnd->Invalidate(FALSE);

	return 0;
}

/*************
 * DESCRIPTION:	Redraw the material
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CMaterial::Update()
{
	DWORD idThread;

	if (bThread)
		return;

	bThread = TRUE;
	MaterialStruct.pDC = &MemDC;
	MaterialStruct.surf = pSurface;

	// close previous Thread-Object
	if (MaterialStruct.hThread)
		CloseHandle(MaterialStruct.hThread);

	MaterialStruct.hThread = CreateThread(NULL, 0, MaterialThread, (LPVOID)&MaterialStruct, 0, &idThread);

	bThread = FALSE;
}

/*************
 * DESCRIPTION:	Show bitmap on screen
 * INPUT:			pDC	device context
 *						x,y	position of material box
 * OUTPUT:			-
 *************/
void CMaterial::Show(CDC *pDC, int x, int y)
{
	CRect rect;

	rect.SetRect(x, y, x+MATWIDTH, y+MATTOTALHEIGHT);
	pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_ADJUSTRECT);
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &MemDC, 0,0, SRCCOPY);
}

/* Rename Dialog */

BEGIN_MESSAGE_MAP(CRenameDlg, CDialog)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CRenameDlg::CRenameDlg(CWnd* pParent)
    : CDialog(CRenameDlg::IDD, pParent)
{
}

/*************
 * DESCRIPTION:	initialize dialog and set focus to name
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CRenameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_RENAME)->SetFocus();
	((CEdit*)GetDlgItem(IDC_RENAME))->SetSel(0, -1);

	return FALSE;
}

/*************
 * DESCRIPTION:	transfer data
 * INPUT:			(system related)
 * OUTPUT:			-
 *************/
void CRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_RENAME, m_sName);
}


