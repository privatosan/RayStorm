/***************
 * NAME:				meshdlg.h
 * PROJECT:			Scenario
 * VERSION:			0.1 30.08.1996
 * DESCRIPTION:	light settings dialog
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						21.09.1996	mh		comment
 ***************/

#ifndef MESHDLG_H
#define MESHDLG_H

#include "filebutt.h"
#include "mesh.h"

class CMeshSetPage : public CPropertyPage
{
public:
	CMeshSetPage(); 
	enum { IDD = IDD_MESHSET };

	CFileButton m_fbtnFile;
	CString m_sFile;
	BOOL m_bApply;
	MESH *m_pMesh;
	
protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();	
	virtual void DoDataExchange(CDataExchange*);

	DECLARE_MESSAGE_MAP()
};

#endif
