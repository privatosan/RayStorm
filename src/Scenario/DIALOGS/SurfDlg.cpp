/***************
 * MODULE:			surfdlg.cpp
 * PROJECT:			Scenario
 * VERSION:			0.7 30.08.1996
 * DESCRIPTION:	dialog for all surface parameters
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						01.08.1996	mh		Texture parameter names
 *						16.09.1996	mh		OnDbClk(), CAddTexture
 *						02.09.1996	mh		OnSetActive
 *						07.05.1997  mh		new style of dialog for v2.0
 *						15.09.1997	mh		spin buttons for color page
 ***************/

#include "typedefs.h"
#include "stdio.h"
#include "resource.h"
#include "surfdlg.h"
#include "camview.h"
#include "texture.h"
#include "brush.h"
#include "mainfrm.h"
#include "preview.h"
#include "document.h"
#include "oseldlg.h"
#include "utility.h"

#define PREVIEW_XRES 64
#define PREVIEW_YRES 64
#define TEXTURE_PARAMCOUNT 16

static char *aszDefTextures[] =
{
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
};

static char *aszBrushTypes[] =
{
	"Color", "Reflect", "Filter", "Altitude", "Specular", NULL
};

static char *aszBrushWrapmodes[] =
{
	"Flat", "Wrap X", "Wrap Y", "Wrap XY", NULL
};

typedef IM_TTABLE *(*INQUIRETEXTURE)(int, int);

// hooks for brushes
typedef struct
{
	BRUSH *brush;
	BOOL added;
	int type;
	int wrap;
	int flags;
} BRUSH_ITEM;

// hooks for textures
typedef struct
{
	HINSTANCE hInst;
	IMAGINE_TEXTURE *texture;
	BOOL added;
	char *infotext[TEXTURE_PARAMCOUNT];
	float params[TEXTURE_PARAMCOUNT];
} ITEXTURE_ITEM;

/*************
 * DESCRIPTION:	destructor for ITEXTURE_ITEM
 * INPUT:			ti		pointer to ITEXTURE_ITEM
 * OUTPUT:			-
 *************/
void DeleteTextureItem(ITEXTURE_ITEM *ti)
{
	int i;

	if (ti->added)
		if (ti->texture)
			delete ti->texture;

	for (i = 0; i < TEXTURE_PARAMCOUNT; i++)
		if (ti->infotext[i])
			delete ti->infotext[i];

	delete ti;
}

typedef struct
{
	SURFACE *surf;
	OBJECT *object;
	CPicture *wnd;
	BOOL bCanceled;
	HANDLE hThread;
} PREVIEWSTRUCT;

BOOL ReadTextureParams(ITEXTURE_ITEM *, char *);
PREVIEWSTRUCT prevstruct;

/*************
 * DESCRIPTION:	show attributes requester
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::MaterialDlg(SURFACE *pSurf, OBJECT *pObject)
{
	ASSERT(pSurf != NULL);

	CSurfaceSheet dialog(this);

	dialog.pObject = pObject;
	dialog.pSurface =
	dialog.m_pgColor.surf =
	dialog.m_pgBrush.surf =
	dialog.m_pgTexture.surf = pSurf;

	if (dialog.DoModal() == IDOK)
	{
		pMatView->UpdateMaterial(pSurf);
		UpdateSurfaceAssignment(pSurf);
	}
}

/*************
 * DESCRIPTION:	show attributes requester
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CCamView::OnAttributes()
{
	OBJECT *pObject;

	if (GetActiveObjects(&pObject, TRUE))
	{
		CObjSelect dialog(BROWSER_SELECTEDSURFACE);
		VIEW *view = pDisplay->view;
	
		if (dialog.DoModal() == IDOK)
		{
			if (dialog.m_pSelectedObject)
				pObject = dialog.m_pSelectedObject;
			else
				return;
		}
	}

	if (pObject)
	{
		if (pObject->surf)
		{
			if (pObject->surf->flags & SURF_DEFAULT)
				utility.Request(IDS_ERRNOTASSIGNED);
			else
				MaterialDlg(pObject->surf, pObject);
		}
		else
			utility.Request(IDS_ERRNOTASSIGNED);
	}
	else
		utility.Request(IDS_ERRNOOBJSELECT);
}
	
IMPLEMENT_DYNAMIC(CSurfaceSheet, CPropertySheet)

// CSurfaceSheet
 
BEGIN_MESSAGE_MAP(CSurfaceSheet, CPropertySheet)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor, add property sheets
 * INPUT:			system
 * OUTPUT:			-
 *************/
CSurfaceSheet::CSurfaceSheet(CWnd* pWndParent)
    : CPropertySheet("Surface properties", pWndParent)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	AddPage(&m_pgColor);
	AddPage(&m_pgBrush);
	AddPage(&m_pgTexture);

	prevstruct.hThread = NULL;
	prevstruct.bCanceled = FALSE;
	hThread = NULL;
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
int CSurfaceSheet::OnInitDialog()
{
	BOOL bRes = CPropertySheet::OnInitDialog();

	m_pgTexture.SetValues(pSurface);
	m_pgBrush.SetValues(pSurface);

	// add the preview window to the property sheet.
	CRect rectWnd;
	GetTabControl()->GetWindowRect(rectWnd);

	CRect rectPreview(rectWnd.Width() - PREVIEW_XRES - 10, rectWnd.Height() - PREVIEW_YRES - 10, rectWnd.Width() - 10, rectWnd.Height() - PREVIEW_YRES);
	m_wndPreview.SetSize(PREVIEW_XRES, PREVIEW_YRES);
	m_wndPreview.Create(NULL, WS_CHILD | WS_VISIBLE, rectPreview, this, 999);

	OnPreview();

	return bRes;
}

/*************
 * DESCRIPTION:	thread for drawing preview window
 * INPUT:			arg	pointer to PREVIEWSTRUCT
 * OUTPUT:			handle to thread
 *************/
DWORD WINAPI PreviewThread(LPVOID)
{
	PREVIEW preview;
	int y;

	if (!preview.Init(PREVIEW_XRES, PREVIEW_YRES, 16, prevstruct.surf, prevstruct.object))
		return -1;

	y = 0;
	while (y < PREVIEW_YRES && !prevstruct.bCanceled)
	{
		preview.RenderLines(y);
		prevstruct.wnd->DrawLine(y, 16, preview.line + y*PREVIEW_XRES);
		y += 16;
	}
	delete prevstruct.surf;

	return 0;
}

/*************
 * DESCRIPTION:	stop preview thread
 * INPUT:			-
 * OUTPUT:			-
 *************/
void StopPreviewThread()
{
	if (!prevstruct.hThread)
		return;

	prevstruct.bCanceled = TRUE;
	WaitForSingleObject(prevstruct.hThread, INFINITE);
	prevstruct.hThread = NULL;
	prevstruct.bCanceled = FALSE;
	CloseHandle(prevstruct.hThread);
}

/*************
 * DESCRIPTION:	close surface sheet
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CSurfaceSheet::OnOK()
{
	UpdateValues(pSurface, TRUE);
	StopPreviewThread();
	EndDialog(IDOK);
}

/*************
 * DESCRIPTION:	stop preview thread
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CSurfaceSheet::OnDestroy()
{
	StopPreviewThread();

	CPropertySheet::OnDestroy();
}

/*************
 * DESCRIPTION:	draw preview
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CSurfaceSheet::OnPreview()
{
	SURFACE *pSurf;
	DWORD idThread;

	StopPreviewThread();

	pSurf = pSurface->Duplicate();
	if (!pSurf)
		return;

	UpdateValues(pSurf, FALSE);

	prevstruct.surf = pSurf;
	prevstruct.object = pObject;
	prevstruct.wnd = &m_wndPreview;

	prevstruct.hThread = CreateThread(NULL, 0, PreviewThread, &prevstruct, 0, &idThread);
}

/*************
 * DESCRIPTION:	update surface with all parameters of the pages
 * INPUT:			pSurf			Surface to update
 *						bModifyList Bush/Texture list has been modified
 * OUTPUT:			-
 *************/
void CSurfaceSheet::UpdateValues(SURFACE *pSurf, BOOL bModifyList)
{
	BRUSH *btmplist, *bnext, *brush;
	BRUSH_ITEM *bi;
	TEXTURE *ttmplist, *tnext, *texture;
	ITEXTURE_ITEM *iti;
	char szBuf[20];
	int i;

	// color page
	pSurf->diffuse		= m_pgColor.m_cbtnDiffuse.col;
	pSurf->ambient		= m_pgColor.m_cbtnAmbient.col;
	pSurf->specular	= m_pgColor.m_cbtnSpecular.col;
	pSurf->reflect		= m_pgColor.m_cbtnReflect.col;
	pSurf->transpar	= m_pgColor.m_cbtnTranspar.col;
	pSurf->difftrans  = m_pgColor.m_cbtnDifftrans.col;		
	pSurf->spectrans  = m_pgColor.m_cbtnSpectrans.col;

	if (((CButton *)m_pgColor.GetDlgItem(IDC_COLOR_BRIGHT))->GetCheck())
		pSurf->flags |= SURF_BRIGHT;
	else
		pSurf->flags &= ~SURF_BRIGHT;
		
	m_pgColor.GetDlgItemText(IDC_COLOR_SPECEXP, szBuf, 20);
	pSurf->refphong = atof(szBuf);
	m_pgColor.GetDlgItemText(IDC_COLOR_TRANSEXP, szBuf, 20);
	pSurf->transphong = atof(szBuf);
	m_pgColor.GetDlgItemText(IDC_COLOR_REFRACT, szBuf, 20);
	pSurf->refrindex = atof(szBuf);
	m_pgColor.GetDlgItemText(IDC_COLOR_FOGLENGTH, szBuf, 20);
	pSurf->foglength = atof(szBuf);
	m_pgColor.GetDlgItemText(IDC_COLOR_TRANSLUC, szBuf, 20);
	pSurf->translucency = atof(szBuf);

	// brush page
	
	m_pgBrush.GetParam();

	// read brushes from listview
	// build brush list in surface according to position in list view:
	// highest priority -> top of list -> last of brush list
	// lowest priority -> bottom of list -> first of brush list
	btmplist = pSurf->brush;
	pSurf->brush = NULL;

	for (i = 0; i < m_pgBrush.aBrushItems.GetSize(); i++)
	{
		bi = (BRUSH_ITEM *)m_pgBrush.aBrushItems.GetAt(i);

		if (bModifyList)
		{
			// copy parameters
			bi->brush->type = bi->type;
			bi->brush->wrap = bi->wrap;
			bi->brush->flags = bi->flags;
			if (!bi->added)
			{
				// remove from temp list
				bi->brush->SLIST::DeChain((SLIST**)&btmplist);
			}

			// and add to surface brush list
			pSurf->AddBrush(bi->brush);

			// this must be set to FALSE to tell the Destructor of the list
			// not to delete the brush
			bi->added = FALSE;
			delete bi;
		}
		else
		{
			brush = (BRUSH*)bi->brush->DupObj();
			if(!brush)
				break;

			brush->type = bi->type;
			brush->wrap = bi->wrap;
			brush->flags = bi->flags;

			pSurf->AddBrush(brush);
		}
	}

	// delete removed brushes
	while (btmplist)
	{
		bnext = (BRUSH*)btmplist->GetNext();
		delete btmplist;
		btmplist = bnext;
	}

	// texture page
	m_pgTexture.GetParam();

	// read textures from listview
	ttmplist = pSurf->texture;
	pSurf->texture = NULL;

	for (i = 0; i < m_pgTexture.aTextureItems.GetSize(); i++)
	{
		iti = (ITEXTURE_ITEM *)m_pgTexture.aTextureItems.GetAt(i);
	
		if (bModifyList)
		{
			memcpy(iti->texture->params, iti->params, sizeof(float)*TEXTURE_PARAMCOUNT);

			if (!iti->added)
			{
				// remove from temp list
				iti->texture->SLIST::DeChain((SLIST**)&ttmplist);
			}

			// and add to surface texture list
			pSurf->AddTexture(iti->texture);

			// this must be set to FALSE to tell the Destructor of the list
			// not to delete the texture
			iti->added = FALSE;
			DeleteTextureItem(iti);
		}
		else
		{
			texture = (IMAGINE_TEXTURE*)iti->texture->DupObj();
			if (!texture)
				break;

			memcpy(((IMAGINE_TEXTURE*)texture)->params, iti->params, sizeof(float)*TEXTURE_PARAMCOUNT);
			pSurf->AddTexture(texture);
		}
	}

	// delete removed textures
	while (ttmplist)
	{
		tnext = (TEXTURE*)ttmplist->GetNext();
		delete ttmplist;
		ttmplist = tnext;
	}
}

/*************
   CColorPage
 *************/

BEGIN_MESSAGE_MAP(CColorPage, CPropertyPage)
	ON_MESSAGE(WM_COLORCHANGED, UpdatePreview)
	ON_EN_CHANGE(IDC_COLOR_SPECEXP, UpdatePreview)
	ON_EN_CHANGE(IDC_COLOR_TRANSEXP, UpdatePreview)
	ON_EN_CHANGE(IDC_COLOR_REFRACT, UpdatePreview)
	ON_EN_CHANGE(IDC_COLOR_FOGLENGTH, UpdatePreview)
	ON_EN_CHANGE(IDC_COLOR_TRANSLUC, UpdatePreview)
	ON_BN_CLICKED(IDC_COLOR_BRIGHT, UpdatePreview)
END_MESSAGE_MAP()

CColorPage::CColorPage()
	: CPropertyPage(CColorPage::IDD)
{
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CColorPage::OnInitDialog()
{
	CString s;

	CDialog::OnInitDialog();

	VERIFY(m_cbtnDiffuse.SubclassDlgItem(IDC_COLOR_DIFFUSE, this));
	VERIFY(m_cbtnSpecular.SubclassDlgItem(IDC_COLOR_SPECULAR, this));
	VERIFY(m_cbtnTranspar.SubclassDlgItem(IDC_COLOR_TRANSPAR, this));
	VERIFY(m_cbtnReflect.SubclassDlgItem(IDC_COLOR_REFLECT, this));
	VERIFY(m_cbtnAmbient.SubclassDlgItem(IDC_COLOR_AMBIENT, this));
	VERIFY(m_cbtnDifftrans.SubclassDlgItem(IDC_COLOR_DIFFTRANS, this));
	VERIFY(m_cbtnSpectrans.SubclassDlgItem(IDC_COLOR_SPECTRANS, this));
	VERIFY(m_spnSpecExp.SubclassDlgItem(IDC_COLOR_SPECEXPSPIN, this));
	VERIFY(m_spnTransExp.SubclassDlgItem(IDC_COLOR_TRANSEXPSPIN, this));
	VERIFY(m_spnRefract.SubclassDlgItem(IDC_COLOR_REFRACTSPIN, this));
	VERIFY(m_spnFogLength.SubclassDlgItem(IDC_COLOR_FOGLENGTHSPIN, this));
	VERIFY(m_spnTransluc.SubclassDlgItem(IDC_COLOR_TRANSLUCSPIN, this));

	m_spnSpecExp.SetRange(0, 500);
	m_spnTransExp.SetRange(0, 500);
	m_spnRefract.SetRange(0, 500);
	m_spnFogLength.SetRange(0, 5000);
	m_spnTransluc.SetRange(0, 500);

	SetValues(surf);

	return TRUE;
}

/*************
 * DESCRIPTION:	call parent preview function, uses by own message
 * INPUT:			-
 * OUTPUT:			-
 *************/
LRESULT CColorPage::UpdatePreview(WPARAM, LPARAM)
{
	((CSurfaceSheet*)GetParent())->OnPreview();

	return 0;
}

/*************
 * DESCRIPTION:	call parent preview function
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CColorPage::UpdatePreview()
{
	((CSurfaceSheet*)GetParent())->OnPreview();
}

/*************
 * DESCRIPTION:	fill out dialog items
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CColorPage::SetValues(SURFACE *surf)
{
	char szBuf[20];

	m_cbtnDiffuse.col = surf->diffuse;
	m_cbtnAmbient.col = surf->ambient;
	m_cbtnSpecular.col = surf->specular;
	m_cbtnReflect.col = surf->reflect;
	m_cbtnTranspar.col = surf->transpar;
	m_cbtnDifftrans.col = surf->difftrans;		
	m_cbtnSpectrans.col = surf->spectrans;
	((CButton *)GetDlgItem(IDC_COLOR_BRIGHT))->SetCheck(surf->flags & SURF_BRIGHT ? 1 : 0);
	sprintf(szBuf, "%g", surf->refphong);
	SetDlgItemText(IDC_COLOR_SPECEXP, szBuf);
	sprintf(szBuf, "%g", surf->transphong);
	SetDlgItemText(IDC_COLOR_TRANSEXP, szBuf);
	sprintf(szBuf, "%g", surf->refrindex);
	SetDlgItemText(IDC_COLOR_REFRACT, szBuf);
	sprintf(szBuf, "%g", surf->foglength);
	SetDlgItemText(IDC_COLOR_FOGLENGTH, szBuf);
	sprintf(szBuf, "%g", surf->translucency);
	SetDlgItemText(IDC_COLOR_TRANSLUC, szBuf);
}

// CBrushPage

BEGIN_MESSAGE_MAP(CBrushPage, CPropertyPage)
	ON_COMMAND(IDC_BRUSH_ADD, OnAdd)
	ON_COMMAND(IDC_BRUSH_REMOVE, OnRemove)
	ON_COMMAND(IDC_BRUSH_NAME, OnName)
	ON_COMMAND(IDC_BRUSH_UP, OnUp)
	ON_COMMAND(IDC_BRUSH_DOWN, OnDown)
	ON_LBN_SELCHANGE(IDC_BRUSH_NAMES, OnSel)
END_MESSAGE_MAP()

CBrushPage::CBrushPage()
	: CPropertyPage(CBrushPage::IDD)
{
	curbrush = 0;
}

/*************
 * DESCRIPTION:	destructor, free all brush items
 * INPUT:			-
 * OUTPUT:			-
 *************/
CBrushPage::~CBrushPage()
{
	for (int i = 0; i < aBrushItems.GetSize(); i++)
	{
//		delete (BRUSH_ITEM *)aBrushItems.GetAt(i);
	}
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CBrushPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	int i;

	VERIFY(m_cboxType.SubclassDlgItem(IDC_BRUSH_TYPES, this));
	VERIFY(m_cboxWrap.SubclassDlgItem(IDC_BRUSH_WRAP, this));
	VERIFY(m_lboxBrush.SubclassDlgItem(IDC_BRUSH_NAMES, this));

	if (m_lboxBrush.GetCount() > 0)
		m_lboxBrush.SetCurSel(0);

	VERIFY(m_btnAdd.SubclassDlgItem(IDC_BRUSH_ADD, this));
	m_bmpAdd.LoadBitmap(IDB_BITMAP_ADD);
	m_btnAdd.SetBitmap((HBITMAP)m_bmpAdd.GetSafeHandle());

	VERIFY(m_btnRemove.SubclassDlgItem(IDC_BRUSH_REMOVE, this));
	m_bmpRemove.LoadBitmap(IDB_BITMAP_REMOVE);
	m_btnRemove.SetBitmap((HBITMAP)m_bmpRemove.GetSafeHandle());

	VERIFY(m_btnName.SubclassDlgItem(IDC_BRUSH_NAME, this));
	m_bmpName.LoadBitmap(IDB_BITMAP_NAME);
	m_btnName.SetBitmap((HBITMAP)m_bmpName.GetSafeHandle());

	VERIFY(m_btnUp.SubclassDlgItem(IDC_BRUSH_UP, this));
	m_bmpUp.LoadBitmap(IDB_BITMAP_UP);
	m_btnUp.SetBitmap((HBITMAP)m_bmpUp.GetSafeHandle());

	VERIFY(m_btnDown.SubclassDlgItem(IDC_BRUSH_DOWN, this));
	m_bmpDown.LoadBitmap(IDB_BITMAP_DOWN);
	m_btnDown.SetBitmap((HBITMAP)m_bmpDown.GetSafeHandle());

	i = 0;
	while (aszBrushTypes[i])
	{
		m_cboxType.AddString(aszBrushTypes[i]);
		i++;
	}

	i = 0;
	while (aszBrushWrapmodes[i])
	{
		m_cboxWrap.AddString(aszBrushWrapmodes[i]);
		i++;
	}

	UpdateBrushList();
	ShowParam();

	return TRUE;
}

/*************
 * DESCRIPTION:	fill out dialog items
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrushPage::SetValues(SURFACE *surf)
{
	BRUSH_ITEM *bi;
	BRUSH *brush;

	// add brushes to list
	brush = surf->brush;
	while (brush)
	{
		bi = (BRUSH_ITEM *)AddBrush();
		if (bi)
		{
			bi->brush = brush;
			bi->added = FALSE;
			bi->type = brush->type;
			bi->wrap = brush->wrap;
			bi->flags = brush->flags;
		}		
		brush = (BRUSH*)brush->GetNext();
	}
	if (aBrushItems.GetSize() > 0)
		curbrush = 0;
}

/*************
 * DESCRIPTION:	Update the list of brushes
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrushPage::UpdateBrushList()
{
	m_lboxBrush.SetRedraw(FALSE);
	m_lboxBrush.ResetContent();

	for (int i = 0; i < aBrushItems.GetSize(); i++)
		m_lboxBrush.AddString(((BRUSH_ITEM*)aBrushItems.GetAt(i))->brush->GetName());

	m_lboxBrush.SetCurSel(curbrush);
	m_lboxBrush.SetRedraw(TRUE);
}

/*************
 * DESCRIPTION:	update selection
 * INPUT:			-
 * OUTPUT:			-
 *************/
 void CBrushPage::OnSel()
{
	GetParam();
	curbrush = m_lboxBrush.GetCurSel();
	ShowParam();
}

/*************
 * DESCRIPTION:	add a new brush
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrushPage::OnAdd()
{
	BRUSH_ITEM *bi;
	OBJECT *object;
	char szBuf[256];
	
	if (!utility.FileRequ(szBuf, FILEREQU_BRUSH, 0))
		return;

	bi = (BRUSH_ITEM *)AddBrush();
	if (!bi)
		return;

	// add to brush list
	bi->brush = new BRUSH;
	if (!bi->brush)
		return;

	GetActiveObjects(&object);
	//		bi->brush->pos = object->pos;
	//		bi->brush->orient_x = object->orient_x;
	//		bi->brush->orient_y = object->orient_y;
	//		bi->brush->orient_z = object->orient_z;
	if (bi->brush->SetName(LPSTR(szBuf)))
	{
		bi->added = TRUE;
		bi->type = bi->brush->type;
		bi->wrap = bi->brush->wrap;
		bi->flags = bi->brush->flags;
	}
	
	curbrush = aBrushItems.GetSize() - 1;
	UpdateBrushList();
	ShowParam();
}

/*************
 * DESCRIPTION:	remove selected brush
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrushPage::OnRemove()
{
	BRUSH_ITEM *bi;

	if (!(curbrush >= 0 && curbrush < aBrushItems.GetSize()))
		return;

	bi = (BRUSH_ITEM *)aBrushItems.GetAt(curbrush);

	if (bi->added)
		delete bi->brush;
	delete bi;
	aBrushItems.RemoveAt(curbrush);

	if (curbrush == aBrushItems.GetSize())
		curbrush--;

	UpdateBrushList();
	
	ShowParam();
}

/*************
 * DESCRIPTION:	change name of current selected brush
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrushPage::OnName()
{
	BRUSH_ITEM *bi;
	char szBuf[256];
	
	if (!(curbrush >= 0 && curbrush < aBrushItems.GetSize()))
		return;

	bi = (BRUSH_ITEM *)aBrushItems.GetAt(curbrush);
	strcpy(szBuf, bi->brush->GetName());

	if (utility.FileRequ(szBuf, FILEREQU_BRUSH, FILEREQU_INITIALFILEDIR))
		bi->brush->SetName(szBuf);
}

/*************
 * DESCRIPTION:	move brush one step up (higher priority)
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrushPage::OnUp()
{
	BRUSH_ITEM *bi;
	
	if (!(curbrush > 0 && curbrush < aBrushItems.GetSize()))
		return;

	bi = (BRUSH_ITEM *)aBrushItems.GetAt(curbrush - 1);
	aBrushItems.SetAt(curbrush - 1, (BRUSH_ITEM *)aBrushItems.GetAt(curbrush));
	aBrushItems.SetAt(curbrush, bi);
	curbrush--;
	UpdateBrushList();
}

/*************
 * DESCRIPTION:	move brush one step down (lower priority)
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrushPage::OnDown()
{
	BRUSH_ITEM *bi;
	
	if (!(curbrush >= 0 && curbrush < aBrushItems.GetSize() - 1))
		return;

	bi = (BRUSH_ITEM *)aBrushItems.GetAt(curbrush + 1);
	aBrushItems.SetAt(curbrush + 1, (BRUSH_ITEM *)aBrushItems.GetAt(curbrush));
	aBrushItems.SetAt(curbrush, bi);
	curbrush++;
	UpdateBrushList();
}

/*************
 * DESCRIPTION:	create a brush item and add it to the list box
 * INPUT:			-
 * OUTPUT:			-
 *************/
void *CBrushPage::AddBrush()
{
	BRUSH_ITEM *bi;
		
	bi = new BRUSH_ITEM;
	if (bi)
		curbrush = aBrushItems.Add(bi);
	
	return (void *)bi;
}

/*************
 * DESCRIPTION:	get data from dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrushPage::GetParam()
{
	BRUSH_ITEM *bi;

	if (!::IsWindow(m_hWnd))
		return;

	if (!(curbrush >= 0 && curbrush < aBrushItems.GetSize()))
		return;

	bi = (BRUSH_ITEM *)aBrushItems.GetAt(curbrush);

	bi->type = m_cboxType.GetCurSel();
	bi->wrap = m_cboxWrap.GetCurSel();

	bi->flags = 0;
	if (((CButton *)GetDlgItem(IDC_BRUSH_REPEAT))->GetCheck())
		bi->flags |= BRUSH_REPEAT;
	if (((CButton *)GetDlgItem(IDC_BRUSH_MIRROR))->GetCheck())
		bi->flags |= BRUSH_MIRROR;
	if (((CButton *)GetDlgItem(IDC_BRUSH_SOFT))->GetCheck())
		bi->flags |= BRUSH_SOFT;
}

/*************
 * DESCRIPTION:	set dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrushPage::ShowParam()
{
	BRUSH_ITEM *bi;
	
	if (!::IsWindow(m_hWnd))
		return;

	if (!(curbrush >= 0 && curbrush < aBrushItems.GetSize()))
		return;

	bi = (BRUSH_ITEM *)aBrushItems.GetAt(curbrush);

	m_cboxType.SetCurSel(bi->type);
	m_cboxWrap.SetCurSel(bi->wrap);

	((CButton *)GetDlgItem(IDC_BRUSH_REPEAT))->SetCheck(bi->flags & BRUSH_REPEAT ? 1 : 0);
	((CButton *)GetDlgItem(IDC_BRUSH_MIRROR))->SetCheck(bi->flags & BRUSH_MIRROR ? 1 : 0);
	((CButton *)GetDlgItem(IDC_BRUSH_SOFT))->SetCheck(bi->flags & BRUSH_SOFT ? 1 : 0);
}


// CTexturePage

BEGIN_MESSAGE_MAP(CTexturePage, CPropertyPage)
	ON_COMMAND(IDC_TEXTURE_ADD, OnAdd)
	ON_COMMAND(IDC_TEXTURE_REMOVE, OnRemove)
	ON_COMMAND(IDC_TEXTURE_NAME, OnName)
	ON_COMMAND(IDC_TEXTURE_UP, OnUp)
	ON_COMMAND(IDC_TEXTURE_DOWN, OnDown)
	ON_LBN_SELCHANGE(IDC_TEXTURE_NAMES, OnSel)
	ON_LBN_SELCHANGE(IDC_TEXTURE_PARAMETERS, OnSel)
	ON_EN_CHANGE(IDC_TEXTURE_VALUE, UpdatePreview)
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CTexturePage::CTexturePage()
	: CPropertyPage(CTexturePage::IDD)
{
	curtexture = 0;
	curparam = 0;
}

/*************
 * DESCRIPTION:	initializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CTexturePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
		
	VERIFY(m_lboxParameter.SubclassDlgItem(IDC_TEXTURE_PARAMETERS, this));
	VERIFY(m_lboxTexture.SubclassDlgItem(IDC_TEXTURE_NAMES, this));

	VERIFY(m_btnAdd.SubclassDlgItem(IDC_TEXTURE_ADD, this));
	m_bmpAdd.LoadBitmap(IDB_BITMAP_ADD);
	m_btnAdd.SetBitmap((HBITMAP)m_bmpAdd.GetSafeHandle());

	VERIFY(m_btnRemove.SubclassDlgItem(IDC_TEXTURE_REMOVE, this));
	m_bmpRemove.LoadBitmap(IDB_BITMAP_REMOVE);
	m_btnRemove.SetBitmap((HBITMAP)m_bmpRemove.GetSafeHandle());

	VERIFY(m_btnName.SubclassDlgItem(IDC_TEXTURE_NAME, this));
	m_bmpName.LoadBitmap(IDB_BITMAP_NAME);
	m_btnName.SetBitmap((HBITMAP)m_bmpName.GetSafeHandle());

	VERIFY(m_btnUp.SubclassDlgItem(IDC_TEXTURE_UP, this));
	m_bmpUp.LoadBitmap(IDB_BITMAP_UP);
	m_btnUp.SetBitmap((HBITMAP)m_bmpUp.GetSafeHandle());

	VERIFY(m_btnDown.SubclassDlgItem(IDC_TEXTURE_DOWN, this));
	m_bmpDown.LoadBitmap(IDB_BITMAP_DOWN);
	m_btnDown.SetBitmap((HBITMAP)m_bmpDown.GetSafeHandle());

	m_lboxTexture.SetCurSel(curtexture);

	VERIFY(m_spnValue.SubclassDlgItem(IDC_TEXTURE_VALUESPIN, this));
	m_spnValue.SetRange(0, 1000);

	UpdateTextureList();
	ShowParam(TRUE);

	return TRUE;
}

/*************
 * DESCRIPTION:	call parent preview function
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTexturePage::UpdatePreview()
{
	if (!IsWindow(m_lboxTexture.m_hWnd))
		return;

	GetParam();
	((CSurfaceSheet*)GetParent())->OnPreview();
}

/*************
 * DESCRIPTION:	set dialog 	
 * INPUT:			surf	surface to update
 * OUTPUT:			-
 *************/
void CTexturePage::SetValues(SURFACE *pSurf)
{
	ITEXTURE_ITEM *ti;
	TEXTURE *texture;

	texture = pSurf->texture;
	while (texture)
	{
		ti = (ITEXTURE_ITEM *)AddTexture();
		ti->added = FALSE;
		ti->texture = (IMAGINE_TEXTURE*)texture;
		if (!ReadTextureParams(ti, texture->name))
		{
			DeleteTextureItem(ti);
			aTextureItems.RemoveAt(curtexture);
		}
		else
			memcpy(ti->params, ((IMAGINE_TEXTURE*)texture)->params, sizeof(float)*TEXTURE_PARAMCOUNT);

		texture = (TEXTURE*)texture->GetNext();
	}

	if (aTextureItems.GetSize() > 0)
		curtexture = 0;

	ShowParam(TRUE);
}

/*************
 * DESCRIPTION:	create and add texture to listbox
 * INPUT:			surf	surface to update
 * OUTPUT:			-
 *************/
void *CTexturePage::AddTexture()
{
	ITEXTURE_ITEM *ti;
		
	ti = new ITEXTURE_ITEM;
	if (ti)
		curtexture = aTextureItems.Add(ti);
	
	return (void *)ti;
}

/*************
 * DESCRIPTION:	get data from dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTexturePage::GetParam()
{
	ITEXTURE_ITEM *ti;
	char szBuf[20];

	if (!::IsWindow(m_hWnd))
		return;

	// is curtexture in valid range ?
	if (!(curtexture >= 0 && curtexture < aTextureItems.GetSize()))
		return;

	ti = (ITEXTURE_ITEM *)aTextureItems.GetAt(curtexture);
	GetDlgItemText(IDC_TEXTURE_VALUE, szBuf, 20);
	ti->params[curparam] = atof(szBuf);
}

/*************
 * DESCRIPTION:	show texture data in dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTexturePage::ShowParam(BOOL bRefresh)
{
	ITEXTURE_ITEM *ti;
	char szBuf[256];
	int i;

	if (!::IsWindow(m_hWnd))
		return;

	if (!(curtexture >= 0 && curtexture < aTextureItems.GetSize()))
	{
		SetDlgItemText(IDC_TEXTURE_VALUE, "");
		m_lboxParameter.ResetContent();
		return;
	}

	ti = (ITEXTURE_ITEM *)aTextureItems.GetAt(curtexture);
	if (bRefresh)
	{
		m_lboxParameter.SetRedraw(FALSE);
		m_lboxParameter.ResetContent();
		for (i = 0; i < TEXTURE_PARAMCOUNT; i++)
		{
			if (ti->infotext[i])
				m_lboxParameter.AddString((char *)ti->infotext[i]);
		}
		m_lboxParameter.SetRedraw(TRUE);
		curparam = 0;
	}
	sprintf(szBuf, "%g", ti->params[curparam]);
	SetDlgItemText(IDC_TEXTURE_VALUE, szBuf);
	m_lboxParameter.SetCurSel(curparam);
}

/*************
 * DESCRIPTION:	Update the list of textures
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTexturePage::UpdateTextureList()
{
	m_lboxTexture.SetRedraw(FALSE);
	m_lboxTexture.ResetContent();

	for (int i = 0; i < aTextureItems.GetSize(); i++)
		m_lboxTexture.AddString(((ITEXTURE_ITEM *)aTextureItems.GetAt(i))->texture->GetName());
	
	m_lboxTexture.SetCurSel(curtexture);
	m_lboxTexture.SetRedraw(TRUE);
}

/*************
 * DESCRIPTION:	update cursor selection
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTexturePage::OnSel()
{
	int oldsel;

	GetParam();
	oldsel = curtexture;
	curtexture = m_lboxTexture.GetCurSel();
	curparam = m_lboxParameter.GetCurSel();

	ShowParam(oldsel != curtexture);
}

/*************
 * DESCRIPTION:	add a texture
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTexturePage::OnAdd()
{
	ITEXTURE_ITEM *ti;
	char szBuf[256];
	
	if (!utility.FileRequ(szBuf, FILEREQU_ITEXTURE, 0))
		return;

	ti = (ITEXTURE_ITEM *)AddTexture();
	if (!ti)
		return;
		
	ti->texture = new IMAGINE_TEXTURE;
	if (!ti->texture)
		return;
	
	if (!ti->texture->SetName(LPSTR(szBuf)))
	{
		DeleteTextureItem(ti);
		return;
	}

	if (!ReadTextureParams(ti, LPSTR(szBuf)))
	{
		DeleteTextureItem(ti);
		m_lboxTexture.DeleteString(curtexture);
		return;
	}

	ti->added = TRUE;
	
	// copy default parameters
	for (int i = 0; i < TEXTURE_PARAMCOUNT; i++)
		ti->texture->params[i] = ti->params[i];

	UpdateTextureList();
	ShowParam(TRUE);
}

/*************
 * DESCRIPTION:	remove texture from listbox
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTexturePage::OnRemove()
{
	ITEXTURE_ITEM *ti;
	
	if (!(curtexture >= 0 && curtexture < aTextureItems.GetSize()))
		return;

	ti = (ITEXTURE_ITEM *)aTextureItems.GetAt(curtexture);

	DeleteTextureItem(ti);
	aTextureItems.RemoveAt(curtexture);

	if (curtexture == aTextureItems.GetSize())
		curtexture--;

	UpdateTextureList();
		
	ShowParam(TRUE);
	UpdatePreview();
}

/*************
 * DESCRIPTION:	change name of current selected texture
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTexturePage::OnName()
{
	ITEXTURE_ITEM *ti;
	char szBuf[256];

	if (!(curtexture >= 0 && curtexture < aTextureItems.GetSize()))
		return;

	ti = (ITEXTURE_ITEM *)aTextureItems.GetAt(curtexture);
	strcpy(szBuf, ti->texture->GetName());

	if (utility.FileRequ(szBuf, FILEREQU_ITEXTURE, FILEREQU_INITIALFILEDIR))
		ti->texture->SetName(szBuf);
}

/*************
 * DESCRIPTION:	move texture one step up
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTexturePage::OnUp()
{
	ITEXTURE_ITEM *ti;
	
	if (!(curtexture > 0 && curtexture < aTextureItems.GetSize()))
		return;

	ti = (ITEXTURE_ITEM *)aTextureItems.GetAt(curtexture - 1);
	aTextureItems.SetAt(curtexture - 1, (ITEXTURE_ITEM *)aTextureItems.GetAt(curtexture));
	aTextureItems.SetAt(curtexture, ti);
	curtexture--;
	UpdateTextureList();
	UpdatePreview();
}

/*************
 * DESCRIPTION:	move texture one step down
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CTexturePage::OnDown()
{
	ITEXTURE_ITEM *ti;
	
	if (!(curtexture >= 0 && curtexture < aTextureItems.GetSize() - 1))
		return;

	ti = (ITEXTURE_ITEM *)aTextureItems.GetAt(curtexture + 1);
	aTextureItems.SetAt(curtexture + 1, (ITEXTURE_ITEM *)aTextureItems.GetAt(curtexture));
	aTextureItems.SetAt(curtexture, ti);
	curtexture++;
	UpdateTextureList();
	UpdatePreview();
}

/*************
 * DESCRIPTION:	load a texture, read the parameters and infotexts and store them in the itemlist
 * INPUT:			ti				pointer to texture item
 *						name			name of texture
 * OUTPUT:			FALSE if failed else TRUE
 *************/
BOOL ReadTextureParams(ITEXTURE_ITEM *ti, char *name)
{
	INQUIRETEXTURE InquireTexture;
	HINSTANCE hInst;
	IM_TTABLE *ttable;
	int i;

	ASSERT(ti && name);

	hInst = LoadLibrary(name);
	i = GetLastError();
	if (!hInst)
	{
		for (i = 0; i < TEXTURE_PARAMCOUNT; i++)
			ti->infotext[i] = NULL;

		utility.Request(IDS_ERRLOADTEXTURE);
		return FALSE;
	}
			
	InquireTexture = (INQUIRETEXTURE)GetProcAddress(hInst, "InquireTexture");
	if (!InquireTexture)
	{
		FreeLibrary(hInst);
		utility.Request(IDS_ERRLOADTEXTURE);
		return FALSE;
	}
			
	ttable = InquireTexture(0x70, 0x1);
	if (!ttable)
	{
		FreeLibrary(hInst);
		utility.Request(IDS_ERRWRONGTEXTURE);

		return FALSE;
	}

	if (!ttable->infotext)
	{
		for (i = 0; i < TEXTURE_PARAMCOUNT; i++)
			ttable->infotext[i] = (unsigned char*)aszDefTextures[i];
	}
		
	// copy parameter names
	for (i = 0; i < TEXTURE_PARAMCOUNT; i++)
	{
		if (strlen((char *)ttable->infotext[i]))
		{
			ti->infotext[i] = new char[strlen((char *)ttable->infotext[i]) + 1];
			if(!ti->infotext[i])
			{
				FreeLibrary(hInst);
				utility.Request(IDS_ERRNOMEM);
				return FALSE;
			}
			strcpy((char *)ti->infotext[i], (char *)ttable->infotext[i]);
		}
		else
			ti->infotext[i] = NULL;
	}

	// copy default parameters
	for (i = 0; i < TEXTURE_PARAMCOUNT; i++)
		ti->params[i] = ttable->params[i];

	// Set default geometry
	//	memcpy(&pos, texture->ttable->tform, 5*sizeof(VECTOR));

	if(ttable->cleanup)
		ttable->cleanup();

	FreeLibrary(hInst);

	return TRUE;
}

