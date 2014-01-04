// cpppages.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorPage dialog

class CColorPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CColorPage)

// Construction
public:
	CColorPage();
	~CColorPage();

// Dialog Data
	//{{AFX_DATA(CColorPage)
	enum { IDD = IDD_COLOR };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// Used for callbacks
	TXHOSTINFO	*m_pHostInfo;

	// Texture parameters
	COLORREF	m_Color1;
	COLORREF	m_Color2;
	COLORREF	m_Color3;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColorPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColorPage)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnColor1Button();
	afx_msg void OnColor2Button();
	afx_msg void OnColor3Button();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CInfoPage dialog

class CInfoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CInfoPage)

// Construction
public:
	CInfoPage();
	~CInfoPage();

// Dialog Data
	//{{AFX_DATA(CInfoPage)
	enum { IDD = IDD_INFO };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// Used for callbacks
	TXHOSTINFO	*m_pHostInfo;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInfoPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInfoPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSizePage dialog

class CSizePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSizePage)

// Construction
public:
	CSizePage();
	~CSizePage();

// Dialog Data
	//{{AFX_DATA(CSizePage)
	enum { IDD = IDD_SIZE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// Used for callbacks
	TXHOSTINFO	*m_pHostInfo;

	// Texture parameters
	float	m_fXSize;
	float	m_fYSize;
	float	m_fZSize;

	CSSEditCtrls	m_XSizeCtrls;
	CSSEditCtrls	m_YSizeCtrls;
	CSSEditCtrls	m_ZSizeCtrls;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSizePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSizePage)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateEditCtrl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CControlsPage dialog

class CControlsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CControlsPage)

// Construction
public:
	CControlsPage();
	~CControlsPage();

// Dialog Data
	//{{AFX_DATA(CControlsPage)
	enum { IDD = IDD_CONTROLS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// Used for callbacks
	TXHOSTINFO	*m_pHostInfo;

	// Texture parameters
	float	m_fBlend1;
	float	m_fBlend2;
	float	m_fBlend3;
	float	m_fBlend4;

	CSSEditCtrls	m_Blend1Ctrls;
	CSSEditCtrls	m_Blend2Ctrls;
	CSSEditCtrls	m_Blend3Ctrls;
	CSSEditCtrls	m_Blend4Ctrls;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CControlsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CControlsPage)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateEditCtrl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
