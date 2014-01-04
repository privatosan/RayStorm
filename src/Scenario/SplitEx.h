/////////////////////////////////////////////////////////////////////////////
// Copyright: 
// Uwe Kotyczka <kotyczka@mipool.uni-jena.de>
// created: July 1998
//
// based on a sample of
// Oleg G. Galkin
//
/////////////////////////////////////////////////////////////////////////////
// SplitterWndAdvanced.h : Header-Datei
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITTERWNDADVANCED_H__09EDD581_CDA3_11D1_B619_0000C0F55FDF__INCLUDED_)
#define AFX_SPLITTERWNDADVANCED_H__09EDD581_CDA3_11D1_B619_0000C0F55FDF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSplitterWndEx : public CSplitterWnd
{
	DECLARE_DYNAMIC(CSplitterWndEx)
public:
	CSplitterWndEx();

// Attribute
protected:
	int m_nHiddenCol;  // hidden column number, -1 if all columns are shown
	int m_nHiddenRow;  // hidden row number, -1 if all rows are shown
	BOOL m_bEnableResizing;

// Operationen
public:
	void ShowColumn();
	void ShowRow();
	void HideColumn(int colHide);
	void HideRow(int rowHide);
	int GetHiddenColumn();
	int GetHiddenRow();

	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSplitterWndEx)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CSplitterWndEx();

// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CSplitterWndEx)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SPLITTERWNDADVANCED_H__09EDD581_CDA3_11D1_B619_0000C0F55FDF__INCLUDED_)
