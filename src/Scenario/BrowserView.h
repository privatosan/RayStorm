/***************
 * MODULE:			BrowserView.h
 * PROJECT:			Scenario
 * DESCRIPTION:	browser dialog, definition file
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						30.08.1996	mh		first release (0.1)
 *						09.05.1997	mh		-> converted to CTreeView
 ***************/

#ifndef BROWSERVIEW_H
#define BROWSERVIEW_H

#include <afxcview.h>
#include <afxole.h>
#include "afxcmn.h"
#include "object.h"
#include "camview.h"
#include "CustTree.h"

class CBrowserView : public CView
{
	DECLARE_DYNCREATE(CBrowserView)
	
	COleDropTarget m_DropTarget;
	CCustomTreeCtrl m_TreeCtrl;
	CImageList m_ImageList;
	// D&D stuff
	BOOL m_bDragging;
	HTREEITEM m_hItemDrag;
	HTREEITEM m_hItemDrop;
	CImageList *m_pImageList;
	UINT m_DropFlags;
	int mode;

public:
	CBrowserView();
	CCamView *pCamView;
		
	virtual void OnDraw(CDC *);
	void Build();
	void Clear();
	void Delete(OBJECT *);
	void Insert(OBJECT *);
	void Update(OBJECT *);
	void Select(OBJECT *);
	BOOL TransferItem(HTREEITEM, HTREEITEM);
	BOOL IsChildNodeOf(HTREEITEM, HTREEITEM);

protected:
	DROPEFFECT OnDragEnter(COleDataObject *pDataObject, DWORD dwKeyState, CPoint point);
	HTREEITEM GetHandleFromObject(OBJECT *);
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT &);

	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg BOOL OnEraseBkgnd(CDC *);
	afx_msg int OnMouseActivate(CWnd *, UINT, UINT);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnPropertySelchanged(NMHDR *, LRESULT *);
	afx_msg void OnEndLabelEdit(LPNMHDR, LRESULT *);
	afx_msg void OnBegindrag(NMHDR *, LRESULT *);
	afx_msg void OnLButtonUp(UINT, CPoint);
	afx_msg void OnItemExpanded(NMHDR *, LRESULT *);

	DECLARE_MESSAGE_MAP()
};

#endif
