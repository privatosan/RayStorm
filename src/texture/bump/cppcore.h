// cppcore.h : main header file for the DLL "core"
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextureApp
// See cppcore.cpp for the implementation of this class
//

class CTextureApp : public CWinApp
{
public:
	CTextureApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTextureApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CTextureApp	theApp;

/////////////////////////////////////////////////////////////////////////////

// These are the functions that the 'cppcore' module requires from
// each "implementation" derived from it.

extern "C" {

extern LONG	(*pInit)();			// init this to NULL if no 'Init' func.
extern void	(*pCleanup)();		// init this to NULL if no 'Cleanup' func.
extern void	Work(float *pParams, PATCH *pPatch, IM_VECTOR *pPosn, TFORM *pAxes);
extern float	fParams[16];	// 16 default parameters
extern TFORM	tfAxes;			// default (relative) texture axes
extern char	szName[18];			// NULL terminated name for list box
extern unsigned char	ubParamFlags[16];	// default flags (TXTF_SCL, etc.)
extern LPARAM lCreateArg;		// init this to something ... 0, at least.
extern void *CreatePages(LPARAM lCreateArg, TXHOSTINFO *pHost, float *params);
extern void ReleasePages(void *pClientData);
extern void StoreParams(void *pClientData, float *params);

};

/////////////////////////////////////////////////////////////////////////////

// This is the "main entry point" exported to Imagine by 'cppcore'.

extern "C" __declspec(dllexport) LONG InquireTexture(int vers, int type);

/////////////////////////////////////////////////////////////////////////////

// These functions are provided by 'cppcore' for use by the implementations
// of the 'CreatePages()' and 'ReleasePages()' routines ... they hook the
// creation callback function for the property pages (with 'PSPCallback')
// to get the pages created with the MFC "module state" of the texture DLL.

BOOL AddPSPage(TXHOSTINFO *pHost, CPropertyPage *pPg);
void RemovePSPage(TXHOSTINFO *pHost, CPropertyPage *pPg);

extern "C" UINT CALLBACK PSPCallback(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp);

/////////////////////////////////////////////////////////////////////////////
// Color button helper routines

extern void SubclassColorButton(TXHOSTINFO *pHost, CPropertyPage *pPPg, UINT nIDBtn, COLORREF clrVal);
extern void SetButtonColor(TXHOSTINFO *pHost, CPropertyPage *pPPg, UINT nIDBtn, COLORREF clrVal);
extern void GetButtonColor(TXHOSTINFO *pHost, CPropertyPage *pPPg, UINT nIDBtn, COLORREF *clrVal);
extern void OnColorButton(TXHOSTINFO *pHost, CPropertyPage *pPPg, UINT nIDBtn, COLORREF *clrVal);

/////////////////////////////////////////////////////////////////////////////
