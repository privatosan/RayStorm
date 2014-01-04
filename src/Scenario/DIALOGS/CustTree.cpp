// OwnrTree.cpp : implementation file
//

#include "typedefs.h"
#include "CustTree.h"
#include "object.h"
#include "sci.h"
#include "resource.h"
#include "BrowserView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INSERT_TEXT ""
/////////////////////////////////////////////////////////////////////
// CCustomTreeCtrl

/////////////////////////////////////////////////////////////////////
// CCustomTreeCtrl Construction

CCustomTreeCtrl::CCustomTreeCtrl()
{
	m_bNoPaint = FALSE;
	m_bLockPropertySelchanged = FALSE;
}

/////////////////////////////////////////////////////////////////////
// CCustomTreeCtrl Public Interfaces

BOOL CCustomTreeCtrl::SetItemText(HTREEITEM hItem, LPCTSTR lpszItem)
{
	CString	cstr;
	CRect	crect;

	cstr = lpszItem;

	AddTextItem(hItem, cstr);

	GetItemRect(hItem, &crect, FALSE);
	InvalidateRect(&crect);

	return TRUE;	
}

CString CCustomTreeCtrl::GetItemText(HTREEITEM hItem) 
{
	CString cstr;

	GetTextFromItem(hItem, cstr);

	return cstr;
}
  
HTREEITEM CCustomTreeCtrl::InsertItem(LPTV_INSERTSTRUCT lpInsertStruct)
{
	HTREEITEM hItem;
	CString cstr;

	cstr = lpInsertStruct->item.pszText;

	lpInsertStruct->item.pszText = INSERT_TEXT;

	hItem = CTreeCtrl::InsertItem(lpInsertStruct);
	if (!hItem)
		return NULL;

	AddTextItem(hItem, cstr);
	
	return hItem;
}
  
HTREEITEM CCustomTreeCtrl::InsertItem(UINT nMask, LPCTSTR lpszItem, 
										int nImage, 
										int nSelectedImage, 
										UINT nState,UINT nStateMask,
										LPARAM lParam, 
										HTREEITEM hParent, 
										HTREEITEM hInsertAfter)
{
	CString		cstr;
	HTREEITEM	hItem;

	hItem = CTreeCtrl::InsertItem(nMask, INSERT_TEXT, nImage, 
									nSelectedImage, nState, 
									nStateMask, lParam, hParent, 
									hInsertAfter); 
	if (!hItem)
		return NULL;

	cstr = lpszItem;

	AddTextItem(hItem, cstr);

	return hItem;

}
  
HTREEITEM CCustomTreeCtrl::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	CString		cstr;
	HTREEITEM	hItem;

	cstr = lpszItem;
	hItem = CTreeCtrl::InsertItem(INSERT_TEXT, hParent, 
									hInsertAfter);
	if (!hItem)
		return NULL;

	AddTextItem(hItem, cstr);

	return hItem;
}
  
HTREEITEM CCustomTreeCtrl::InsertItem(LPCTSTR lpszItem, int nImage, 
										int nSelectedImage, 
										HTREEITEM hParent, 
										HTREEITEM hInsertAfter)
{
	CString		cstr;
	HTREEITEM	hItem;

	cstr = lpszItem;

	hItem = CTreeCtrl::InsertItem(INSERT_TEXT, nImage,nSelectedImage,
									hParent, hInsertAfter);
	if (!hItem)
		return NULL;

	AddTextItem(hItem, cstr);

	return hItem;
}

BOOL CCustomTreeCtrl::GetItem(TV_ITEM* pItem)
{
	BOOL	bRet;	
	CString	cstr;

	bRet = CTreeCtrl::GetItem(pItem);

	if (bRet)
	{
		GetTextFromItem(pItem->hItem, cstr);
		strncpy(pItem->pszText, cstr, pItem->cchTextMax);
	}

	return bRet;
}

BOOL CCustomTreeCtrl::SetItem(TV_ITEM* pItem)
{
	BOOL	bRet;
	CString	cstr;
	
	cstr = pItem->pszText;
	pItem->pszText = INSERT_TEXT;
	bRet = CTreeCtrl::SetItem(pItem);

	if (bRet)
	{
		AddTextItem(pItem->hItem, cstr);	
	}

	return bRet;
}

BOOL CCustomTreeCtrl::SetItem(HTREEITEM hItem, UINT nMask, 
							  LPCTSTR lpszItem, int nImage, 
							  int nSelectedImage, UINT nState, 
							  UINT nStateMask, LPARAM lParam)
{
	BOOL	bRet;
	
	bRet = CTreeCtrl::SetItem(hItem, nMask, INSERT_TEXT, nImage, 
								nSelectedImage, nState, nStateMask, 
								lParam);

	if (bRet)
	{
		CString	cstr;
		cstr = lpszItem;
		AddTextItem(hItem, cstr);	
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////
// CCustomTreeCtrl Implementation Methods

HTREEITEM CCustomTreeCtrl::PtInField(POINT pt)
{
	CRect		crect;
	HTREEITEM	htreeitem;

	htreeitem = GetFirstVisibleItem();
		
	while (htreeitem)
	{
		GetItemRect(htreeitem, &crect, TRUE);
		if (crect.PtInRect(pt))
			break;
		
		htreeitem = GetNextVisibleItem(htreeitem);
	}

	return htreeitem;
}

void CCustomTreeCtrl::AddTextItem(HTREEITEM hItem, CString &cstr)
{	
	m_cmapTextItems.SetAt(hItem, cstr);
}

void CCustomTreeCtrl::RemoveTextItem(HTREEITEM hItem)
{	
	m_cmapTextItems.RemoveKey(hItem);
}

BOOL CCustomTreeCtrl::GetTextFromItem(HTREEITEM hItem, CString &cstr)
{
	return m_cmapTextItems.Lookup(hItem, cstr);
}

void CCustomTreeCtrl::SetItemWidth(HTREEITEM htreeitem, CDC &cdc, 
									DWORD dwWidth)
{
	int			nSpaceWidth, nSpaces;
	CString		cstr;

	cdc.GetCharWidth(' ', ' ', &nSpaceWidth);
	
	// I don't know when a space wouldn't have width, but I would 
	// hate to see a divide by zero error.
	if (!nSpaceWidth)
		return;

	nSpaces = dwWidth / nSpaceWidth + ((dwWidth % nSpaceWidth)?1:0);

	cstr = CTreeCtrl::GetItemText(htreeitem);
	if (cstr.GetLength() != nSpaces)
	{
		CTreeCtrl::SetItemText(htreeitem, CString(' ', nSpaces));
	}
}

/////////////////////////////////////////////////////////////////////
// CCustomTreeCtrl Overidables

// This function should normally be overridden, however it is always 
// nice to have a default implementation to fall back on
DWORD CCustomTreeCtrl::DrawItem(HTREEITEM htreeitem, CDC &cdc, CRect &rect, UINT nState)
{
	CFont *oldFont;
	CRect rectBack;
	COLORREF oldCol;
	OBJECT *object;
	char *name;

	rectBack = rect;
	rectBack.right += 4;
	cdc.FillSolidRect(&rectBack, GetSysColor(COLOR_WINDOW));

	oldFont = cdc.SelectObject(GetFont());

	object = (OBJECT*)(GetItemData(htreeitem));
	if (object)
	{
		name = object->GetName();
		if (object->selected)
			oldCol = cdc.SetTextColor(RGB(255,0,0));
		else
			oldCol = cdc.SetTextColor(RGB(0,0,0));

		cdc.DrawText(name, &rect, DT_VCENTER | DT_SINGLELINE);
		cdc.SelectObject(oldFont);
		cdc.SetTextColor(oldCol);
		return cdc.GetTextExtent(name, strlen(name)).cx;
	}
	return 0;
}

BEGIN_MESSAGE_MAP(CCustomTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CCustomTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnPropertySelchanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////
// CCustomTreeCtrl message handlers

// To prevent flickering when using non default bkcolor
BOOL CCustomTreeCtrl::OnEraseBkgnd(CDC *) 
{
	return TRUE;
}

// Disable Tooltips
BOOL CCustomTreeCtrl::OnNotify(WPARAM wParam, LPARAM lParam, 
							   LRESULT* pResult) 
{
	return TRUE;
}

/*************
 * DESCRIPTION:	set which to current selected object
 * INPUT:			system 
 * OUTPUT:			-
 *************/
void CCustomTreeCtrl::OnPropertySelchanged(LPNMHDR pNMHDR, LRESULT *lParam) 
{
	NM_TREEVIEW *pnmtv;

	if (m_bLockPropertySelchanged)
		return;

	pnmtv = (NM_TREEVIEW*)pNMHDR;

	OBJECT *which = (OBJECT*)GetItemData(pnmtv->itemNew.hItem);

	if (which)
	{
		if (!(GetKeyState(VK_SHIFT) & 0xf000))
			DeselectAll();
		
		which->SelectObject();
		// lock us to prevent recursive call
		m_bLockPropertySelchanged = TRUE;
		sciBrowserBuild();
		m_bLockPropertySelchanged = FALSE;
		sciRedraw();
	}
}

void CCustomTreeCtrl::OnPaint() 
{	
	HTREEITEM hTreeItem;
	CRect	rect, rectClip, rectClient, rectTemp;
	CFont	*pcfont, *pcfontOld;
	CBitmap bitmap;
	CDC memDC;
	CRgn rgn;
	CPaintDC dc(this);
	UINT nState;
	BOOL bFocus;
	DWORD dwWidth;

	// If we are flagged to ignore this pain message, 
	// then validate rect and return
	if (m_bNoPaint)
	{
		m_bNoPaint = FALSE;		
		ValidateRect(NULL);
		return;
	}	

	// Create a memory DC compatible with the paint DC
	memDC.CreateCompatibleDC(&dc);
	dc.GetClipBox(&rectClip);
	GetClientRect(&rectClient);

	// Select a compatible bitmap into the memory DC
	bitmap.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height());
	memDC.SelectObject(&bitmap);

	// Set clip region to be same as that in paint DC
	rgn.CreateRectRgnIndirect(&rectClip);
	memDC.SelectClipRgn(&rgn);
	rgn.DeleteObject();

	// First let the control do its default drawing.
	// Retrieve Invalid Rect for use throughout paint handler
	ValidateRect(NULL);
	
	// If we have focus, store this fact, but then turn it off
	if (bFocus = GetFocus() == this)
	{
		// Make sure any paint message generated by 
		// SetFocus() does nothing
		m_bNoPaint = TRUE;
		::SetFocus(NULL);

		// Make sure that or Invalid rect is the same as it 
		// was upon entering the paint handler
		ValidateRect(NULL);
		InvalidateRect(&rectClient);

		// Set Paint Flag so that future paint handling will occur
		m_bNoPaint = FALSE;
	}
	
	// Get that default drawing done... without a focus rect or 
	// selected area	
	CWnd::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);

	// If we did have focus we will restore it now
	if (bFocus)
	{		
		m_bNoPaint = TRUE;
		SetFocus();
		m_bNoPaint = FALSE;
	}

	// We want a valid window rect now... SetFocus() may have 
	// invalidated some portion of the client
	ValidateRect(NULL);

	// As control we need to make sure we do our drawing in 
	// our parents font
	pcfont = GetParent()->GetFont();
	pcfontOld = memDC.SelectObject(pcfont);	

	// Now we will enumerate the visible items in the control
	hTreeItem = GetFirstVisibleItem();
	while (hTreeItem)
	{
		// Where is this item in the client
		GetItemRect(hTreeItem, &rect, TRUE);
		
		// Get the state of the current item to pass to the 
		// DrawItem() Function
		nState = GetItemState(hTreeItem, ~0);

		// Expand / limit the area of the item to the right 
		// edge of the client
		rect.right = rectClient.right;

		// If the item falls inside of our original 
		// invalid rect then redraw it
		// by calling the overidable virtual function DrawItem()
		if (rectTemp.IntersectRect(&rect, &rectClient))
		{
			dwWidth = DrawItem(hTreeItem, memDC, rect, nState);
			SetItemWidth(hTreeItem, memDC, dwWidth);
		}

		// Continue through visible items
		hTreeItem = GetNextVisibleItem(hTreeItem);
	}
	// We do this, because SetItemWidth actually changes
	// the text of the items in the control and it 
	// may have flagged a repaint which we don't want to happen
	ValidateRect(NULL);

	dc.BitBlt(rectClip.left, rectClip.top, rectClip.Width(), rectClip.Height(), &memDC, rectClip.left, rectClip.top,SRCCOPY);
}

/*************
 * DESCRIPTION:	called if tree item is expanded or collapsed
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CCustomTreeCtrl::OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	OBJECT *object;

	object = (OBJECT *)GetItemData(pNMTreeView->itemNew.hItem);
	if (object)
	{
		if (pNMTreeView->action & TVE_COLLAPSE)
			object->flags &= ~OBJECT_NODEOPEN;
		if (pNMTreeView->action & TVE_EXPAND)
			object->flags |= OBJECT_NODEOPEN;
	}

	*pResult = 0;
}

/*************
 * DESCRIPTION:	ON_WM_RBUTTONDOWN
 *						handles right mouse button click
 *						display pop up menu in browser menu
 * INPUT:			nFlags   Specifies if various flags are set
 *						point    coordinates of mouse cursor
 * OUTPUT:			-
 *************/
void CCustomTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;
	int resource;
	HTREEITEM hItem;
	UINT flags;
	OBJECT *object;

	hItem = HitTest(point, &flags);
	if (hItem)
	{
		object = (OBJECT*)GetItemData(hItem);
		resource = object->GetResourceID();
		ClientToScreen(&point);
		if (menu.LoadMenu(resource))
		{
			CMenu* pSubMenu = menu.GetSubMenu(0);
			if (pSubMenu != NULL)
			{
				if (resource == IDR_POPUP_OBJECT)
				{
					if (object->external != EXTERNAL_NONE)
						pSubMenu->EnableMenuItem(IDM_TOSCENE, MF_ENABLED);
					else
						pSubMenu->EnableMenuItem(IDM_TOSCENE, MF_GRAYED);
				}
				pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, ((CBrowserView*)GetParent())->pCamView);
			}
		}
	}
}
