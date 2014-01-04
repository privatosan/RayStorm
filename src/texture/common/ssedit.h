// ssedit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSSEditCtrls window

class CSSEditCtrls : public CEdit
{
// Construction
public:
	CSSEditCtrls();

// Attributes
public:
	BOOL	IsCtrlMember(CWnd *pWnd) const;
	void	GetRange(int &nMin, int &nMax) const;
	int	GetPos() const;
	double	GetValue() const;

// Operations
public:
	BOOL	SubclassCtrls(CWnd *pParent, int nIDEdit, int nIDSpinner, int nIDSlider);
	void	SetRange(int nMin, int nMax, BOOL bRedrawSlider = FALSE);
	void	SetPos(int nPos);
	void	SetValue(double dVal);

	void	OnPosUpdate(CWnd *pSrcCtrl);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSSEditCtrls)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSSEditCtrls();

	CSpinButtonCtrl	m_Spinner;
	CSliderCtrl	m_Slider;

	int m_nPos;
	double	m_dVal;

	double	m_dScale;
	double	m_dOffs;

	BOOL	m_bFPMode;

protected:
	int m_nRangeMin;
	int m_nRangeMax;

	volatile int m_nLockCnt;

	DWORD	m_nStartTime;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSSEditCtrls)
	afx_msg void OnEditChange();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

void DDX_SSEControls(CDataExchange *pDX,
	int nIDEdit, int nIDSpinner, int nIDSlider, CSSEditCtrls &ctrls);
void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax, int &nValue);
void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax, char &nValue);
void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax, unsigned int &nValue);
void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax, unsigned char &nValue);
void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax, double dOffs, double dScale, float &fValue);
void DDX_SSEData(CDataExchange *pDX, CSSEditCtrls &ctrls,
	int nRangeMin, int nRangeMax, double dScale, double dOffs, double &dValue);

/////////////////////////////////////////////////////////////////////////////
