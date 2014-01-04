/***************
 * NAME:				transdlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	transformation dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						07.09.1996	mh		comment
 ***************/

#ifndef TRANSDLG_H
#define TRANSDLG_H

#include "CameraDlg.h"
#include "LightDlg.h"
#include "MeshDlg.h"
#include "bsebutt.h"
#include "flares.h"
#include "CSGDlg.h"
#include "PrimDlg.h"

class CTransformPage : public CPropertyPage
{
public:
	CTransformPage(); 
	enum { IDD = IDD_TRANSFORM };

	VECTOR m_Pos, m_Align, m_Size;
	OBJECT *m_pObject, *m_pTrack;
	CBrowseButton m_bbtnTrack;
	CString m_sObjectname, m_sTrackname;
	BOOL m_bInverted;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange*);

	DECLARE_MESSAGE_MAP()
};

// settings sheet
class CSettingsSheet : public CPropertySheet
{
public:
	DECLARE_DYNAMIC(CSettingsSheet)
	CSettingsSheet(CWnd *);

	OBJECT *m_pObject;
	STAR *m_pStar;
	CTransformPage m_pgTransform;
	CCameraSetPage m_pgCameraSet;
	CLightSetPage  m_pgLightSet;
	CMeshSetPage	m_pgMeshSet;
	CFlareSetPage	m_pgFlareSet;
	CStarSetPage	m_pgStarSet;
	CCSGSetPage		m_pgCSGSet;
	CPrimSetPage	m_pgPrimSet;
	BOOL m_bFlares, m_bStar;
	
	void ExtraPagesOnOff();
	
protected:
	DECLARE_MESSAGE_MAP()
};

#endif
