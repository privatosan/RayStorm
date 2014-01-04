#ifndef CUSTTREE_H
#define CUSTTREE_H

#include <afxtempl.h>
#include <afxcmn.h>

class CCustomTreeCtrl : public CTreeCtrl
{
public:
	CCustomTreeCtrl();

public:
	BOOL SetItemText(HTREEITEM, LPCTSTR);
	CString GetItemText(HTREEITEM);
	HTREEITEM InsertItem(LPTV_INSERTSTRUCT);
	HTREEITEM InsertItem(UINT, LPCTSTR, int, int, UINT, UINT, LPARAM, HTREEITEM, HTREEITEM);
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertItem(LPCTSTR, int, int, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	BOOL GetItem(TV_ITEM*);
	BOOL SetItem(TV_ITEM*);  
	BOOL SetItem(HTREEITEM, UINT, LPCTSTR, int, int, UINT, UINT, LPARAM);

private:
	HTREEITEM PtInField(POINT);
	void AddTextItem(HTREEITEM, CString &);
	void RemoveTextItem(HTREEITEM);
	BOOL GetTextFromItem(HTREEITEM, CString &);
	void SetItemWidth(HTREEITEM, CDC &, DWORD);

private:	
	BOOL m_bNoPaint, m_bLockPropertySelchanged;
	CMap<HANDLE, HANDLE, CString, CString &>	m_cmapTextItems;

protected:
	virtual DWORD DrawItem(HTREEITEM, CDC &, CRect &, UINT);
	afx_msg void OnPropertySelchanged(LPNMHDR, LRESULT *);
	afx_msg void OnItemExpanded(LPNMHDR, LRESULT *);
	afx_msg void OnRButtonDown(UINT, CPoint);

protected:
	virtual BOOL OnNotify(WPARAM, LPARAM, LRESULT*);

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *);

	DECLARE_MESSAGE_MAP()
};

#endif