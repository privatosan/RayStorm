// ssedit.cpp : implementation file
//

#include "stdafx.h"
#include "ssedit.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSSEditCtrls

CSSEditCtrls::CSSEditCtrls()
{
	m_nPos = 0;
	m_nRangeMin = 100;
	m_nRangeMax = 0;
	m_dVal = 0.0;
	m_dScale = 0.0;
	m_dOffs = 0.0;
	m_bFPMode = FALSE;
	m_nStartTime = 0;
	m_nLockCnt = 0;
}

CSSEditCtrls::~CSSEditCtrls()
{
	ASSERT(!m_Slider.m_hWnd);
	ASSERT(!m_Spinner.m_hWnd);
	ASSERT(!m_hWnd);
}

BEGIN_MESSAGE_MAP(CSSEditCtrls, CEdit)
	//{{AFX_MSG_MAP(CSSEditCtrls)
	ON_CONTROL_REFLECT(EN_CHANGE, OnEditChange)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSSEditCtrls Attributes

BOOL CSSEditCtrls::IsCtrlMember(CWnd *pWnd) const
{
	return pWnd == (CWnd *)this
		|| pWnd == (CWnd *)&m_Spinner
		|| pWnd == (CWnd *)&m_Slider;
}

double CSSEditCtrls::GetValue() const
{
	ASSERT(m_bFPMode);
	return m_dVal;
}

int CSSEditCtrls::GetPos() const
{
	return m_nPos;
}

void CSSEditCtrls::GetRange(int &nMin, int &nMax) const
{
	nMin = m_nRangeMin;
	nMax = m_nRangeMax;
}

/////////////////////////////////////////////////////////////////////////////
// CSSEditCtrls Functions

BOOL	CSSEditCtrls::SubclassCtrls(CWnd *pParent, int nIDEdit, int nIDSpinner, int nIDSlider)
{
	ASSERT(!m_nLockCnt);
	m_nLockCnt = 1;

	ASSERT(nIDEdit && nIDEdit != -1);
  ASSERT(!nIDSpinner || nIDSpinner != -1 && nIDSpinner != nIDEdit);
  ASSERT(!nIDSlider || nIDSlider != -1
     && nIDSlider != nIDEdit && (!nIDSlider || nIDSlider != nIDSpinner));

	ASSERT(!m_hWnd);
	if (!SubclassDlgItem(nIDEdit, pParent))
		return FALSE;

	ASSERT(!m_Spinner.m_hWnd);
  if (nIDSpinner && !m_Spinner.SubclassDlgItem(nIDSpinner, pParent)) {
		UnsubclassWindow();
		return FALSE;
	}

	ASSERT(!m_Slider.m_hWnd);
  if (nIDSlider && !m_Slider.SubclassDlgItem(nIDSlider, pParent)) {
    if (nIDSpinner)
      m_Spinner.UnsubclassWindow();
		UnsubclassWindow();
		return FALSE;
	}

	ASSERT(m_hWnd && GetDlgCtrlID() == nIDEdit);
  ASSERT(!nIDSpinner || m_Spinner.m_hWnd && m_Spinner.GetDlgCtrlID() == nIDSpinner);
  ASSERT(!nIDSlider || m_Slider.m_hWnd && m_Slider.GetDlgCtrlID() == nIDSlider);

	--m_nLockCnt;
	return TRUE;
}

void	CSSEditCtrls::SetRange(int nMin, int nMax, BOOL bRedrawSlider /*=FALSE*/)
{
	++m_nLockCnt;
	m_nRangeMin = nMin;
	m_nRangeMax = nMax;
	if (m_Spinner.m_hWnd)
		m_Spinner.SetRange(nMin, nMax);
	if (m_Slider.m_hWnd)
		m_Slider.SetRange(nMin, nMax, bRedrawSlider);
	--m_nLockCnt;
}

void	CSSEditCtrls::SetPos(int nPos)
{
	++m_nLockCnt;
	m_nPos = nPos;
	if (m_bFPMode)
		m_dVal = m_dOffs + m_dScale * nPos;
	if (m_hWnd) {
		char	buf[32];
		if (m_bFPMode)
			sprintf(buf, "%f", m_dVal);
		else
			sprintf(buf, "%d", nPos);
		SetWindowText(buf);
	}
	if (m_Slider.m_hWnd)
		m_Slider.SetPos(nPos);
	if (m_Spinner.m_hWnd)
		m_Spinner.SetPos(nPos);
	--m_nLockCnt;
}

void CSSEditCtrls::SetValue(double dVal)
{
	ASSERT(m_bFPMode && m_dScale != 0);

	++m_nLockCnt;

	{
		double	tmp;
		if (dVal > (tmp = m_dOffs + m_nRangeMax * m_dScale)) {
			m_dVal = tmp;
			m_nPos = m_nRangeMax;
		} else if (dVal < (tmp = m_dOffs + m_nRangeMin * m_dScale)) {
			m_dVal = tmp;
			m_nPos = m_nRangeMin;
		} else {
			m_dVal = dVal;
			m_nPos = m_nRangeMin + (int)floor((dVal - tmp) / m_dScale + 0.5);
		}
	}

	if (m_hWnd) {
		char	buf[32];
		sprintf(buf, "%f", m_dVal);
		SetWindowText(buf);
	}

	if (m_Slider.m_hWnd)
		m_Slider.SetPos(m_nPos);
	if (m_Spinner.m_hWnd)
		m_Spinner.SetPos(m_nPos);
	--m_nLockCnt;
}

/////////////////////////////////////////////////////////////////////////////
// CSSEditCtrls message handlers

void CSSEditCtrls::OnEditChange()
{
	if (!m_nLockCnt)
		OnPosUpdate(this);
}

void CSSEditCtrls::OnPosUpdate(CWnd *pSrcCtrl)
{
	if (!m_nLockCnt) {
		++m_nLockCnt;
		char	buf[32];

		if (pSrcCtrl == this) {

			GetWindowText(buf, 31);

			int	nPos;

			if (m_bFPMode) {
				float fVal;
				if (sscanf(buf, " %f", &fVal) == 1)
					m_dVal = fVal;
				else
					m_dVal = 0;

				double tmp;

				if (m_dVal > (tmp = m_dOffs + m_nRangeMax * m_dScale)) {
					m_dVal = tmp;
					m_nPos = m_nRangeMax;
					pSrcCtrl = NULL;
				} else if (m_dVal < (tmp = m_dOffs + m_nRangeMin * m_dScale)) {
					m_dVal = tmp;
					m_nPos = m_nRangeMin;
					pSrcCtrl = NULL;
				} else {
					m_nPos = m_nRangeMin
						+ (int)floor((m_dVal - tmp) / m_dScale + 0.5);
				}
			} else {
				if (sscanf(buf, " %d", &nPos) != 1)
					nPos = 0;
				if (nPos < m_nRangeMin) {
					pSrcCtrl = NULL;
					m_nPos = m_nRangeMin;
				} else if (nPos > m_nRangeMax) {
					pSrcCtrl = NULL;
					m_nPos = m_nRangeMax;
				} else
					m_nPos = nPos;
			}
		} else if (pSrcCtrl == &m_Spinner) {
			m_nPos = m_Spinner.GetPos();
			if (m_bFPMode)
				m_dVal = m_dOffs + m_dScale * m_nPos;
		} else if (pSrcCtrl == &m_Slider) {
			m_nPos = m_Slider.GetPos();
			if (m_bFPMode)
				m_dVal = m_dOffs + m_dScale * m_nPos;
		}

		if (pSrcCtrl != &m_Slider && m_Slider.m_hWnd)
			m_Slider.SetPos(m_nPos);
		if (pSrcCtrl != &m_Spinner && m_Spinner.m_hWnd)
			m_Spinner.SetPos(m_nPos);
		if (pSrcCtrl != this && m_hWnd) {
			if (m_bFPMode)
				sprintf(buf, "%f", m_dVal);
			else
				sprintf(buf, "%d", m_nPos);
			SetWindowText(buf);
		}

		--m_nLockCnt;
	}
}

void CSSEditCtrls::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar != VK_UP && nChar != VK_DOWN) {
		m_nStartTime = 0;
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	if (m_nLockCnt)
		return;

	int	nPos = m_nPos;

	if (nChar == VK_UP) {
		if (nPos == m_nRangeMax) {
			m_nStartTime = 0;
			return;
		}
	} else if (nPos == m_nRangeMin) {
		m_nStartTime = 0;
		return;
	}

	if (!m_nStartTime)
		m_nStartTime = GetTickCount();

	int	nInc = 1;

	{
		UDACCEL	uda[10];
		int nAccel = m_Spinner.GetAccel(10, uda);
		UINT	dt = (GetTickCount() - m_nStartTime) / 1000;
		for (int n = 0; n < nAccel; n++) {
			if (dt >= uda[n].nSec)
				nInc = uda[n].nInc;
			else
				break;
		}
	}

	if (!nInc)
		return;

	if (nChar == VK_UP) {
		if (m_nRangeMax - nPos <= nInc)
			nPos = m_nRangeMax;
		else {
			int n = nInc - (nPos % nInc);
			if (n > nInc)
				n -= nInc;
			nPos += n;
		}
	} else if (nChar == VK_DOWN) {
		if (nPos - m_nRangeMin <= nInc)
			nPos = m_nRangeMin;
		else {
			int	n = nInc + (nPos % nInc);
			if (n > nInc)
				n -= nInc;
			nPos -= n;
		}
	}

	SetPos(nPos);
}

void CSSEditCtrls::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_nStartTime = 0;

	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}

void DDX_SSEControls(CDataExchange *pDX,
	int nIDEdit, int nIDSpinner, int nIDSlider, CSSEditCtrls &ctrls)
{
	if (!ctrls.m_hWnd) {
		ASSERT(!pDX->m_bSaveAndValidate);
		ctrls.SubclassCtrls(pDX->m_pDlgWnd, nIDEdit, nIDSpinner, nIDSlider);
	}
}

void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax, int &nValue)
{
	ctrls.m_bFPMode = FALSE;
	if (!pDX->m_bSaveAndValidate) {
		ctrls.SetRange(nRangeMin, nRangeMax);
		ctrls.SetPos(nValue);
		ctrls.SetSel(0, -1);
	} else
		nValue = ctrls.GetPos();
}

void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax, char &nValue)
{
	ctrls.m_bFPMode = FALSE;
	if (!pDX->m_bSaveAndValidate) {
		ctrls.SetRange(nRangeMin, nRangeMax);
		ctrls.SetPos(nValue);
		ctrls.SetSel(0, -1);
	} else
		nValue = ctrls.GetPos();
}

void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax, unsigned int &nValue)
{
	ctrls.m_bFPMode = FALSE;
	if (!pDX->m_bSaveAndValidate) {
		ctrls.SetRange(nRangeMin, nRangeMax);
		ctrls.SetPos(nValue);
		ctrls.SetSel(0, -1);
	} else
		nValue = ctrls.GetPos();
}

void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax, unsigned char &nValue)
{
	ctrls.m_bFPMode = FALSE;
	if (!pDX->m_bSaveAndValidate) {
		ctrls.SetRange(nRangeMin, nRangeMax);
		ctrls.SetPos(nValue);
		ctrls.SetSel(0, -1);
	} else
		nValue = ctrls.GetPos();
}

void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax,
	double dOffs, double dScale, float &fValue)
{
	ctrls.m_bFPMode = TRUE;
	if (!pDX->m_bSaveAndValidate) {
		ctrls.m_dScale = dScale;
		ctrls.m_dOffs = dOffs;
		ctrls.SetRange(nRangeMin, nRangeMax);
		ctrls.SetValue(fValue);
		ctrls.SetSel(0, -1);
	} else
		fValue = (float)ctrls.GetValue();
}

void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax,
	double dOffs, double dScale, double &dValue)
{
	ctrls.m_bFPMode = TRUE;
	if (!pDX->m_bSaveAndValidate) {
		ctrls.m_dScale = dScale;
		ctrls.m_dOffs = dOffs;
		ctrls.SetRange(nRangeMin, nRangeMax);
		ctrls.SetValue(dValue);
		ctrls.SetSel(0, -1);
	} else
		dValue = ctrls.GetValue();
}
