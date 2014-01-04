/***************
 * NAME:				BrowserView.cpp
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	browser dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:
 * DATE			NAME	COMMENT
 *	30.08.1996	mh		first release (0.1)
 *	07.09.1996	mh		CObjTree added
 *	16.09.1996	mh		objects are inserted by GetObjects
 *	15.03.1997	mh		Build, Delete, Insert
 *	08.07.1997	mh		drag & drop from andi
 *	22.09.1997	mh		Clear
 ***************/

#include "Typedefs.h"
#include "BrowserView.h"
#include "Vecmath.h"
#include "Prefs.h"
#include "Csg.h"
#include "Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// data for AddFunction() call-back
struct AddDataView
{
	CBrowserView *ObjTree;
	TV_INSERTSTRUCT TreeCtrlItem;
	HTREEITEM hParentItem;
	int nMode;
	BOOL bSameLevel;
};

IMPLEMENT_DYNCREATE(CBrowserView, CView)

BEGIN_MESSAGE_MAP(CBrowserView, CView)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*************
 * DESCRIPTION:	constructor
 * INPUT:			-
 * OUTPUT:			-
 *************/
CBrowserView::CBrowserView()
{
	m_bDragging = FALSE;
	m_pImageList = NULL;
}

/*************
 * DESCRIPTION:	initialize drag & drop
 * INPUT:			system
 * OUTPUT:			-
 *************/
int CBrowserView::OnCreate(LPCREATESTRUCT)
{
	m_DropTarget.Register(this);

	return 0;
}

/*************
 * DESCRIPTION:	on draw member function
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CBrowserView::OnDraw(CDC *pDC)
{
}

/*************
 * DESCRIPTION:	resize m_TreeCtrl
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CBrowserView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (IsWindow(m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.MoveWindow(0,0, cx, cy, TRUE);
	}
}


DROPEFFECT CBrowserView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return DROPEFFECT_COPY;
}

/*************
 * DESCRIPTION:	Callback routine for GetObjects
 * INPUT:			obj
 * OUTPUT:			-
 *************/
void AddFunction(OBJECT *obj, void *data)
{
	int image;
	struct AddDataView *adddata = (struct AddDataView*)data;

	// NULL ? go to parent
	if (!obj)
	{
		if (!adddata->bSameLevel)
			adddata->hParentItem = adddata->ObjTree->m_TreeCtrl.GetParentItem(adddata->hParentItem);
		else
			adddata->bSameLevel = FALSE;
		return;
	}
	adddata->bSameLevel = TRUE;
	adddata->TreeCtrlItem.hParent = adddata->hParentItem;
	adddata->TreeCtrlItem.item.pszText = obj->GetName();
	adddata->TreeCtrlItem.item.lParam = (LPARAM)obj;

	switch (obj->GetType())
	{
		case OBJECT_SPHERE: image = 0; break;
		case OBJECT_CONE: image = 1; break;
		case OBJECT_CYLINDER: image = 2;	break;
		case OBJECT_BOX: image = 3; break;
		case OBJECT_CAMERA: image = 4; break;
		case OBJECT_MESH: image = 5; break;
		case OBJECT_CSG:
			switch (((CSG*)obj)->operation)
			{
				case CSG_UNION: image = 6;	break;
				case CSG_DIFFERENCE:	image = 7; break;
				case CSG_INTERSECTION: image = 8; break;
			}
			break;
		case OBJECT_POINTLIGHT: image = 9; break;
		case OBJECT_SPOTLIGHT: image = 10; break;
		case OBJECT_PLANE: image = 11; break;
		case OBJECT_SOR: image = 12; break;
	}
	adddata->TreeCtrlItem.item.iImage = image;
	adddata->TreeCtrlItem.item.iSelectedImage = image;

	if (obj->GoDown())
	{
		adddata->hParentItem = adddata->ObjTree->m_TreeCtrl.InsertItem(&adddata->TreeCtrlItem);
		if(obj->flags & OBJECT_NODEOPEN)
			adddata->ObjTree->m_TreeCtrl.SetItemState(adddata->hParentItem, TVIS_EXPANDED, TVIS_EXPANDED);
	}
	else
		adddata->ObjTree->m_TreeCtrl.InsertItem(&adddata->TreeCtrlItem);
}

/*************
 * DESCRIPTION:	returns handle to a given object
 * INPUT:			obj
 * OUTPUT:			handle of item
 *************/
HTREEITEM CBrowserView::GetHandleFromObject(OBJECT *obj)
{
	HTREEITEM hItem, hItemOld;

	hItem = m_TreeCtrl.GetRootItem();

	do
	{
		if (m_TreeCtrl.ItemHasChildren(hItem))
			hItem = m_TreeCtrl.GetChildItem(hItem);
		else
		{	
			hItemOld = m_TreeCtrl.GetNextItem(hItem, TVGN_NEXT);
			if (!hItemOld)
			{
				hItem = m_TreeCtrl.GetParentItem(hItem);
				if (hItem == m_TreeCtrl.GetRootItem())
					hItem = NULL;
				else
					hItem = m_TreeCtrl.GetNextItem(hItem, TVGN_NEXT);
			}
			else
				hItem = hItemOld;
		}
	} while (hItem && (OBJECT *)(m_TreeCtrl.GetItemData(hItem)) != obj);

	return hItem;
}

/*************
 * DESCRIPTION:	init dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrowserView::Build()
{
	struct AddDataView data;
	CRect rect;
	CDC *pDC;

/*alidateRect(NULL);
	pDC = GetDC();
	GetClientRect(rect);
	pDC->FillRect(rect, CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));
	ReleaseDC(pDC);
*/
	m_TreeCtrl.DeleteAllItems();
	data.TreeCtrlItem.hParent = TVI_ROOT;
	data.TreeCtrlItem.hInsertAfter = TVI_LAST;
	data.TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	data.TreeCtrlItem.item.iImage = 13;
	data.TreeCtrlItem.item.iSelectedImage = 13;
	data.ObjTree = this;
	data.hParentItem = TVI_ROOT;
	data.TreeCtrlItem.item.pszText = "Root";
	data.TreeCtrlItem.item.lParam = NULL;
	data.hParentItem = m_TreeCtrl.InsertItem(&data.TreeCtrlItem);
	m_TreeCtrl.SetItemState(data.hParentItem, TVIS_EXPANDED, TVIS_EXPANDED);
	GetObjects(AddFunction, (void*)&data, 0);
}

/*************
 * DESCRIPTION:	Delete all entries in browser;
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrowserView::Clear()
{
	m_TreeCtrl.DeleteAllItems();
}

/*************
 * DESCRIPTION:	Delete object
 * INPUT:			obj	object to be deleted
 * OUTPUT:			-
 *************/
void CBrowserView::Delete(OBJECT *obj)
{
	HTREEITEM hItem;
	
	ASSERT(obj != NULL);

	hItem = GetHandleFromObject(obj);
	ASSERT(hItem != NULL);

	m_TreeCtrl.DeleteItem(hItem);
}

void CBrowserView::Insert(OBJECT *obj)
{
}

void CBrowserView::Update(OBJECT *obj)
{
}

/*************
 * DESCRIPTION:	select object
 * INPUT:			obj	object to be selected
 * OUTPUT:			-
 *************/
void CBrowserView::Select(OBJECT *obj)
{
	HTREEITEM hItem;
	COLORREF oldCol;
	CDC *pDC;
	CRect rect;
	CSize size;
	char *szName;
	CFont *oldFont;
	
	ASSERT(obj != NULL);

	HTREEITEM hItemOld;

	hItem = m_TreeCtrl.GetRootItem();

	pDC = m_TreeCtrl.GetDC();
	oldFont = pDC->SelectObject(m_TreeCtrl.GetFont());

	do
	{
		if (hItem != m_TreeCtrl.GetRootItem())
		{
			if (m_TreeCtrl.GetItemRect(hItem, rect, TRUE))
			{
				szName = ((OBJECT*)(m_TreeCtrl.GetItemData(hItem)))->GetName();
				if (((OBJECT*)(m_TreeCtrl.GetItemData(hItem)))->selected)
					oldCol = pDC->SetTextColor(RGB(255,0,0));
				else
					oldCol = pDC->SetTextColor(RGB(0,0,0));
				// center text withing rectangle
				size = pDC->GetTextExtent(szName, strlen(szName));
				rect.left += ((rect.Width() - size.cx) >> 1);
				rect.top += ((rect.Height() - size.cy) >> 1);
//				pDC->ExtTextOut(rect.left, rect.top, ETO_OPAQUE, rect, name, strlen(name), NULL);
				pDC->TextOut(rect.left, rect.top, szName, strlen(szName));
				pDC->SetTextColor(oldCol);
			}
		}
		if (m_TreeCtrl.ItemHasChildren(hItem))
			hItem = m_TreeCtrl.GetChildItem(hItem);
		else
		{	
			hItemOld = m_TreeCtrl.GetNextItem(hItem, TVGN_NEXT);
			if (!hItemOld)
			{
				hItem = m_TreeCtrl.GetParentItem(hItem);
				if (hItem == m_TreeCtrl.GetRootItem())
					hItem = NULL;
				else
					hItem = m_TreeCtrl.GetNextItem(hItem, TVGN_NEXT);
			}
			else
				hItem = hItemOld;
		}
	} while (hItem);

	pDC->SelectObject(oldFont);
	m_TreeCtrl.ReleaseDC(pDC);
}

int CBrowserView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// side-step CView's implementation since we don't want to activate
	//  this view
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

/*************
 * DESCRIPTION:	set look of TreeCtrl
 * INPUT:			(system)
 * OUTPUT:			-
 *************/
BOOL CBrowserView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CView::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:	init dialog
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrowserView::OnInitialUpdate()
{
	CRect rect;

	GetClientRect(rect);

	if (!IsWindow(m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.Create(WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS, rect, this, 1024);
  
		if(!m_TreeCtrl.GetImageList(TVSIL_NORMAL))
		{
			if(m_ImageList.Create(IDB_BROWSER, 16, 0, RGB(0,255,0)))
				m_TreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);
		}
		m_TreeCtrl.SetIndent(5);
	}
	Build();
}

/*************
 * DESCRIPTION:	change name of object when label was edited
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CBrowserView::OnEndLabelEdit(LPNMHDR pnmhdr, LRESULT *pLResult)
{
	TV_DISPINFO *pTvInfo;

	pTvInfo = (TV_DISPINFO *)pnmhdr;
	if (pTvInfo->item.pszText != NULL)
	{
		pTvInfo->item.mask = TVIF_TEXT;
		((OBJECT *)(pTvInfo->item.lParam))->SetName(pTvInfo->item.pszText);
		pTvInfo->item.pszText = ((OBJECT *)(pTvInfo->item.lParam))->GetName();
		m_TreeCtrl.SetItem(&pTvInfo->item);
	}
	*pLResult = TRUE;
}

/*************
 * DESCRIPTION:	test for a valid item, prepare D&d
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CBrowserView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CPoint pntAction;
	UINT nFlags;

	GetCursorPos(&pntAction);
	ScreenToClient(&pntAction);
	m_hItemDrag = m_TreeCtrl.HitTest(pntAction, &nFlags);

	if (m_hItemDrag)
	{
		// get the image list for dragging
		m_pImageList = m_TreeCtrl.CreateDragImage(m_hItemDrag);
		if(m_pImageList)
		{
			m_hItemDrop = NULL;
			m_bDragging = TRUE;
			m_pImageList->DragShowNolock(TRUE);
			m_pImageList->SetDragCursorImage(0, CPoint(0, 0));
			m_pImageList->BeginDrag(0, CPoint(0,0));
			m_pImageList->DragMove(pntAction);
			m_pImageList->DragEnter(this, pntAction);
			SetCapture();
		}
	}
	
	*pResult = 0;
}

/*************
 * DESCRIPTION:	indicate valid drop destination
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CBrowserView::OnMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM hItem;

	if (m_bDragging)
	{
		m_pImageList->DragMove(point);
		if ((hItem = m_TreeCtrl.HitTest(point, &m_DropFlags)) != NULL)
		{
			m_pImageList->DragLeave(this);
			m_TreeCtrl.SelectDropTarget(hItem);
			m_hItemDrop = hItem;
			m_pImageList->DragEnter(this, point);
		}
	}

	CView::OnMouseMove(nFlags, point);
}

/*************
 * DESCRIPTION:	test if one item is a child of another
 * INPUT:			hItemChild
 *						hItemSuspectedParent
 * OUTPUT:			TRUE if it is a child
 *************/
BOOL CBrowserView::IsChildNodeOf(HTREEITEM hItemChild, HTREEITEM hItemSuspectedParent)
{
	do
	{
		if (hItemChild == hItemSuspectedParent)
			break;
	}
	while ((hItemChild = m_TreeCtrl.GetParentItem(hItemChild)) != NULL);

	return (hItemChild != NULL);
}

/*************
 * DESCRIPTION:	transfer drag item to drop position
 * INPUT:			hItemDrag
 *						hitmeDrop
 * OUTPUT:			TRUE
 *************/
BOOL CBrowserView::TransferItem(HTREEITEM hItemDrag, HTREEITEM hItemDrop)
{
	TV_INSERTSTRUCT tvstruct;
	TCHAR sztBuffer[256];
	HTREEITEM hNewItem, hFirstChild;

	// avoid an infinite recursion situation
	tvstruct.item.hItem = hItemDrag;
	tvstruct.item.cchTextMax = 255;
	tvstruct.item.pszText = sztBuffer;
	tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	m_TreeCtrl.GetItem(&tvstruct.item);  // get information of the dragged element
	tvstruct.hParent = hItemDrop;
	tvstruct.hInsertAfter = TVI_FIRST;
	tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	hNewItem = m_TreeCtrl.InsertItem(&tvstruct);

	while((hFirstChild = m_TreeCtrl.GetChildItem(hItemDrag)) != NULL)
	{
		TransferItem(hFirstChild, hNewItem);  // recursively transfer all the items
		m_TreeCtrl.DeleteItem(hFirstChild);		// delete the first child and all its children
	}
	return TRUE;
}

/*************
 * DESCRIPTION:	terminate D&D operation
 * INPUT:			system
 * OUTPUT:			-
 *************/
void CBrowserView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	OBJECT *pObject;
	VECTOR p, ox,oy,oz;
	MATRIX m,m1,matrix;

	if (m_bDragging)
	{
		m_pImageList->DragLeave(this);
		m_pImageList->EndDrag();
		if(m_hItemDrag != m_hItemDrop &&
			!IsChildNodeOf(m_hItemDrop, m_hItemDrag) &&
			m_TreeCtrl.GetParentItem(m_hItemDrag) != m_hItemDrop)
		{
			pObject = (OBJECT*)m_TreeCtrl.GetItemData(m_hItemDrag);

			if (1)
			{
				((OBJECT*)m_TreeCtrl.GetItemData(m_hItemDrop))->GetObjectMatrix(&m1);
				m1.InvMat(&m);
				pObject->GetObjectMatrix(&m1);
				matrix.MultMat(&m1, &m);
				SetVector(&pObject->pos, matrix.m[1], matrix.m[2], matrix.m[3]);
				SetVector(&pObject->orient_x, matrix.m[5], matrix.m[9], matrix.m[13]);
				SetVector(&pObject->orient_y, matrix.m[6], matrix.m[10], matrix.m[14]);
				SetVector(&pObject->orient_z, matrix.m[7], matrix.m[11], matrix.m[15]);

				pObject->DeChainObject();
				pObject->Insert((OBJECT*)m_TreeCtrl.GetItemData(m_hItemDrop), INSERT_VERT);
			}
			else
			{
				pObject->GetObjectMatrix(&matrix);
				matrix.GenerateAxis(&pObject->orient_x, &pObject->orient_y, &pObject->orient_z, &pObject->pos);
				pObject->DeChainObject();
				pObject->Insert(NULL, INSERT_VERT);
			}
//			if(updateall)
//				sciBrowserBuild();

			TransferItem(m_hItemDrag, m_hItemDrop);
			m_TreeCtrl.DeleteItem(m_hItemDrag);
		}
		else
			MessageBeep(0);

		ReleaseCapture();
		m_bDragging = FALSE;
		m_TreeCtrl.SelectDropTarget(NULL);
	}
}
