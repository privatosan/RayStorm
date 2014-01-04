// cppcore.cpp : Implementation routines for the DLL "core".
//

#include "stdafx.h"

#include "imdefs.h"
#include "txdefs.h"

#include "cppcore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char *info_text[] =
{
	"X bump size", 
	"Y bump size", 
	"Z bump size", 
	"", "", "", "", "", "", "", "", "", "", "", "", ""
};



/////////////////////////////////////////////////////////////////////////////
// CTextureApp

BEGIN_MESSAGE_MAP(CTextureApp, CWinApp)
	//{{AFX_MSG_MAP(CTextureApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextureApp construction

CTextureApp::CTextureApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	ASSERT(AfxGetModuleState() == AfxGetStaticModuleState());
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTextureApp object

CTextureApp theApp;

/////////////////////////////////////////////////////////////////////////////
// The exported "InquireTextrue" routine

extern "C" {

LONG	table[] =
{
	0x54585449,			// for surface textures
//	0x5458544c,			// for lighting textures
	NULL,				// filled in with 'pinit' - initialization func.
	NULL,				// filled in with 'pcleanup' - initialization func.
	(LONG)Work,			// work function
	(LONG)szName,		// "nice" name for user (17 chars max, + '\0')
	(LONG)ubParamFlags,	// array of byte parameter flags - 
	(LONG)fParams,		// array of 16 default "REAL*4" parameters
	(LONG)&tfAxes,		// pointer to default axis parameters
	0L,					// filled in with 'lCreateArg', for CreatePages()
	(LONG)CreatePages,	// pointer to property page creation function
	(LONG)ReleasePages,	// pointer to property page destroy function
	(LONG)StoreParams,	// pointer to function to store "return" data
};

__declspec(dllexport) LONG InquireTexture(int vers, int type)
{
	// no need for AFX_MANAGE_STATE() here

	if (vers != 0x70 || type != 1)
		return NULL;

	table[1] = (LONG)pInit;
	table[2] = (LONG)pCleanup;
	table[4] = (LONG)info_text;
	table[8] = lCreateArg;

	return (LONG)table;
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyPage helper routines

UINT CALLBACK PSPCallback(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
	if (uMsg == PSPCB_CREATE)
		AfxSetModuleState(AfxGetStaticModuleState());

	ASSERT(ppsp);
	ASSERT(ppsp->dwSize == sizeof(PROPSHEETPAGE));
	PROPSHEETPAGE	psp = *ppsp;

	PROPSHEETPAGE	*ppspOrig = (LPPROPSHEETPAGE)psp.lParam;
	psp.lParam = ppspOrig->lParam;
	psp.pfnCallback = ppspOrig->pfnCallback;

	ASSERT(psp.pfnCallback);

	return psp.pfnCallback(hwnd, uMsg, &psp);
}

};

BOOL AddPSPage(TXHOSTINFO *pHost, CPropertyPage *pPg)
{
	ASSERT(pPg->m_psp.dwSize == sizeof(PROPSHEETPAGE));
	ASSERT(pPg->m_psp.dwFlags & PSP_USECALLBACK);
	ASSERT(pPg->m_psp.lParam == (LPARAM)pPg);

	PROPSHEETPAGE	psp;

#if 1
	memcpy((void*)&psp, (void*)&pPg->m_psp, sizeof(PROPSHEETPAGE));
#else
	psp = pPg->m_psp;
#endif
	psp.lParam = (LPARAM)&pPg->m_psp;
	psp.pfnCallback = PSPCallback;

	return (*pHost->pAddPage)(pHost->pHostData, &psp, pPg);
}

void RemovePSPage(TXHOSTINFO *pHost, CPropertyPage *pPg)
{
	(*pHost->pRemovePage)(pHost->pHostData, pPg);
}

/////////////////////////////////////////////////////////////////////////////
// Color button helper routines

void SubclassColorButton(TXHOSTINFO *pHost, CPropertyPage *pPPg, UINT nIDBtn, COLORREF clrVal)
{
	pHost->pSubclassColorButton(pHost->pHostData, ::GetDlgItem(pPPg->m_hWnd, nIDBtn), clrVal);
}

void SetButtonColor(TXHOSTINFO *pHost, CPropertyPage *pPPg, UINT nIDBtn, COLORREF clrVal)
{
	pHost->pSetButtonColor(pHost->pHostData, ::GetDlgItem(pPPg->m_hWnd, nIDBtn), clrVal);
}

void GetButtonColor(TXHOSTINFO *pHost, CPropertyPage *pPPg, UINT nIDBtn, COLORREF *clrVal)
{
	COLORREF	clr;

	clr = pHost->pGetButtonColor(
		pHost->pHostData, GetDlgItem(pPPg->m_hWnd, nIDBtn));

	if (clr != CLR_INVALID)
		*clrVal = clr;
}

void OnColorButton(TXHOSTINFO *pHost, CPropertyPage *pPPg, UINT nIDBtn, COLORREF *clrVal)
{
	ASSERT(AfxGetModuleState() == AfxGetStaticModuleState());
	ASSERT(pHost);

	HWND	hwndBtn = ::GetDlgItem(pPPg->m_hWnd, nIDBtn);

	COLORREF	clr = pHost->pGetButtonColor(pHost->pHostData, hwndBtn);

	if (clr == CLR_INVALID)
		clr = *clrVal;

	clr = pHost->pChooseColor(pHost->pHostData, clr);

	if (clr == CLR_INVALID)
		return;

	pPPg->SetModified(TRUE);

	if (CLR_INVALID == pHost->pSetButtonColor(pHost->pHostData, hwndBtn, clr))
		*clrVal = clr; // this shouldn't happen
}
