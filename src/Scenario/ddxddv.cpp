/***************
 * MODULE:			ddxddv.cpp
 * PROJECT:			Scenario
 * DESCRIPTION:   DDX/DDV routines
 *	AUTHORS:			Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		06.09.97	mh		initial release
 ***************/

#include "DdxDdv.h"
#include "Resource.h"

/*************
 * DESCRIPTION:	Test if value greater (or equal) minVal
 * INPUT:			pDX
 *						value		value to be tested
 *						minVal	minimum value
 *						bExclude	if TRUE -> minimum value excluded
 * OUTPUT:			-
 *************/
void AFXAPI DDV_MinFloat(CDataExchange* pDX, float const &value, float minVal, BOOL bExclude)
{
	int nID;

	if (value <= minVal)
	{
		if (!bExclude && value == minVal)
			return;	

		nID = bExclude ? IDS_DDV_MINFLOATEXC : IDS_DDV_MINFLOAT;

		if (!pDX->m_bSaveAndValidate)
		{
			TRACE0("Warning: initial dialog data is out of range.\n");
			return;         // don't stop now
		}
		TCHAR szMin[32];
		CString prompt;

		_stprintf(szMin, _T("%g"), minVal);
		AfxFormatString1(prompt, nID, szMin);

		AfxMessageBox(prompt, MB_ICONEXCLAMATION, nID);
		prompt.Empty(); // exception prep
		pDX->Fail();
	}
}

/*************
 * DESCRIPTION:	Test if value is between ]minVal,maxVal[
 * INPUT:			pDX
 *						value		value to be tested
 *						minVal	minimum value
 *						minVal	maximum value
 * OUTPUT:			-
 *************/
void AFXAPI DDV_MinMaxFloatExclude(CDataExchange* pDX, float const &value, float minVal, float maxVal)
{
	ASSERT(minVal <= maxVal);

	if (value <= minVal || value >= maxVal)
	{
		if (!pDX->m_bSaveAndValidate)
		{
			TRACE0("Warning: initial dialog data is out of range.\n");
			return;         // don't stop now
		}
		TCHAR szMin[32], szMax[32];
		CString prompt;

		_stprintf(szMin, _T("%g"), minVal);
		_stprintf(szMax, _T("%g"), maxVal);
		AfxFormatString2(prompt, IDS_DDV_MINMAXFLOATEXC, szMin, szMax);

		AfxMessageBox(prompt, MB_ICONEXCLAMATION, IDS_DDV_MINMAXFLOATEXC);
		prompt.Empty(); // exception prep
		pDX->Fail();
	}
}