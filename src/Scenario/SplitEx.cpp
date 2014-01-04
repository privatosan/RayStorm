/////////////////////////////////////////////////////////////////////////////
// Copyright: 
// Uwe Kotyczka <kotyczka@mipool.uni-jena.de>
// created: July 1998
//
// based on a sample of
// Oleg G. Galkin
//
/////////////////////////////////////////////////////////////////////////////
// SplitterWndAdvanced.cpp: Implementierungsdatei
//

#include "Typedefs.h"
#include "SplitEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitterWndEx

IMPLEMENT_DYNAMIC(CSplitterWndEx, CSplitterWnd)

BEGIN_MESSAGE_MAP(CSplitterWndEx, CSplitterWnd)
//{{AFX_MSG_MAP(CSplitterWndEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitterWndEx Konstruktion/Destruction

CSplitterWndEx::CSplitterWndEx()
{
	m_nHiddenCol = -1;
	m_nHiddenRow = -1;
	m_bEnableResizing = TRUE;
}

CSplitterWndEx::~CSplitterWndEx()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSplitterWndEx Kommandos

void CSplitterWndEx::ShowColumn()
{
	ASSERT_VALID(this);
	ASSERT(m_nCols < m_nMaxCols);
	ASSERT(m_nHiddenCol != -1);

	int colNew = m_nHiddenCol;
	m_nHiddenCol = -1;
	int cxNew = m_pColInfo[m_nCols].nCurSize;
	m_nCols++;  // add a column
	ASSERT(m_nCols == m_nMaxCols);

    // fill the hidden column
	int col;
	for (int row = 0; row < m_nRows; row++)
	{
		CWnd* pPaneShow = GetDlgItem(AFX_IDW_PANE_FIRST + row * 16 + m_nCols);
		ASSERT(pPaneShow != NULL);
		pPaneShow->ShowWindow(SW_SHOWNA);

		for (col = m_nCols - 2; col >= colNew; col--)
		{
			CWnd* pPane = GetPane(row, col);
			ASSERT(pPane != NULL);
			pPane->SetDlgCtrlID(IdFromRowCol(row, col + 1));
		}

	    pPaneShow->SetDlgCtrlID(IdFromRowCol(row, colNew));
	}

    // new panes have been created -- recalculate layout
	for (col = colNew + 1; col < m_nCols; col++)
	    m_pColInfo[col].nIdealSize = m_pColInfo[col - 1].nCurSize;
	m_pColInfo[colNew].nIdealSize = cxNew;
	RecalcLayout();
}

void CSplitterWndEx::ShowRow()
{
	ASSERT_VALID(this);
	ASSERT(m_nRows < m_nMaxRows);
	ASSERT(m_nHiddenRow != -1);

	int rowNew = m_nHiddenRow;
	m_nHiddenRow = -1;
	int cyNew = m_pRowInfo[m_nRows].nCurSize;
	m_nRows++;  // add a row
	ASSERT(m_nCols == m_nMaxCols);

    // fill the hidden row
	int row;
	for (int col = 0; col < m_nCols; col++)
	{
		CWnd* pPaneShow = GetDlgItem(AFX_IDW_PANE_FIRST + m_nRows * 16 + col);
		ASSERT(pPaneShow != NULL);
		pPaneShow->ShowWindow(SW_SHOWNA);

		for (row = m_nRows - 2; row >= rowNew; row--)
		{
			CWnd* pPane = GetPane(row, col);
			ASSERT(pPane != NULL);
			pPane->SetDlgCtrlID(IdFromRowCol(row + 1, col));
		}

	    pPaneShow->SetDlgCtrlID(IdFromRowCol(rowNew, col));
	}

    // new panes have been created -- recalculate layout
	for (row = rowNew + 1; row < m_nRows; row++)
	    m_pRowInfo[row].nIdealSize = m_pRowInfo[row - 1].nCurSize;
	m_pRowInfo[rowNew].nIdealSize = cyNew;
	RecalcLayout();
}

void CSplitterWndEx::HideColumn(int colHide)
{
	ASSERT_VALID(this);
	ASSERT(m_nCols > 1);
	ASSERT(colHide < m_nCols);
	ASSERT(m_nHiddenCol == -1);
	m_nHiddenCol = colHide;

    // if the column has an active window -- change it
	int rowActive, colActive;
	if (GetActivePane(&rowActive, &colActive) != NULL && colActive == colHide)
	{
		if (++colActive >= m_nCols)
		    colActive = 0;
		SetActivePane(rowActive, colActive);
	}

    // hide all column panes
	for (int row = 0; row < m_nRows; row++)
	{
		CWnd* pPaneHide = GetPane(row, colHide);
		ASSERT(pPaneHide != NULL);
		pPaneHide->ShowWindow(SW_HIDE);
		pPaneHide->SetDlgCtrlID(AFX_IDW_PANE_FIRST + row * 16 + m_nCols);

		for (int col = colHide + 1; col < m_nCols; col++)
		{
			CWnd* pPane = GetPane(row, col);
			ASSERT(pPane != NULL);
			pPane->SetDlgCtrlID(IdFromRowCol(row, col - 1));
		}
	}
	m_nCols--;
	m_pColInfo[m_nCols].nCurSize = m_pColInfo[colHide].nCurSize;
	RecalcLayout();
}

void CSplitterWndEx::HideRow(int rowHide)
{
	ASSERT_VALID(this);
	ASSERT(m_nRows > 1);
	ASSERT(rowHide < m_nRows);
	ASSERT(m_nHiddenRow == -1);
	m_nHiddenRow = rowHide;

    // if the row has an active window -- change it
	int rowActive, colActive;
	if (GetActivePane(&rowActive, &colActive) != NULL && rowActive == rowHide)
	{
		if (++rowActive >= m_nRows)
		    rowActive = 0;
		SetActivePane(rowActive, colActive);
	}

    // hide all row panes
	for (int col = 0; col < m_nCols; col++)
	{
		CWnd* pPaneHide = GetPane(rowHide, col);
		ASSERT(pPaneHide != NULL);
		pPaneHide->ShowWindow(SW_HIDE);
		pPaneHide->SetDlgCtrlID(AFX_IDW_PANE_FIRST + m_nRows * 16 + col);

		for (int row = rowHide + 1; row < m_nRows; row++)
		{
			CWnd* pPane = GetPane(row, col);
			ASSERT(pPane != NULL);
			pPane->SetDlgCtrlID(IdFromRowCol(row - 1, col));
		}
	}
	m_nRows--;
	m_pRowInfo[m_nRows].nCurSize = m_pRowInfo[rowHide].nCurSize;
	RecalcLayout();
}

int CSplitterWndEx::GetHiddenColumn()
{
	return m_nHiddenCol;
}

int CSplitterWndEx::GetHiddenRow()
{
	return m_nHiddenRow;
}

void CSplitterWndEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bEnableResizing == FALSE)
		return;
	CSplitterWnd::OnLButtonDown(nFlags, point);
}

BOOL CSplitterWndEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bEnableResizing == FALSE)
		return TRUE;
	else
		return CSplitterWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CSplitterWndEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bEnableResizing == FALSE)
		CWnd::OnMouseMove(nFlags, point);
	else
		CSplitterWnd::OnMouseMove(nFlags, point);
}
