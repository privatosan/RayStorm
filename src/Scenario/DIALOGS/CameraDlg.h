/***************
 * NAME:				CameraDlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	definition file for camera dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						21.09.1996	mh		comment
 ***************/

#ifndef CAMERADLG_H
#define CAMERADLG_H

#include "camera.h"

class CCameraSetPage : public CPropertyPage
{
public:
	CCameraSetPage(); 
	enum { IDD = IDD_CAMERASET };

	float m_HFOV, m_VFOV, m_FocalDist, m_Aperture;
	BOOL m_bEnableVFOV, m_bEnableFocus, m_bFastDOF;
	CListBox m_lboxLenses;
	CAMERA *m_pCamera;
	
protected:
	virtual BOOL OnInitDialog();
	void OnSel();
	void UpdateParams();
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange*);

	DECLARE_MESSAGE_MAP()
};

#endif
