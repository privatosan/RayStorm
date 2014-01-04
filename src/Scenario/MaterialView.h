/***************
 * MODULE:			MatView.h
 * PROJECT:			Scenario
 * DESCRIPTION:	camera window definition file
 * AUTHORS:			Mike Hesser, Andreas Heumann
 * HISTORY:       
 *				DATE		NAME	COMMENT
 *				10.12.95	ah		initial release
 ***************/
 
#ifndef MATVIEW_H
#define MATVIEW_H

#include "surface.h"
#include "resource.h"

typedef struct
{
	SURFACE *surf;
	CDC *pDC;
	HANDLE hThread;
} MATSTRUCT;

class CMaterial
{
	CBitmap Bitmap;
	CDC MemDC;
	MATSTRUCT MaterialStruct;
	BOOL bThread;

public:
	SURFACE *pSurface;

	CMaterial(CDC *, SURFACE *);
	~CMaterial();
	void Update();
	void Show(CDC *, int, int);
};

class CMatView : public CScrollView
{
protected: // create from serialization only
	CMatView();
	DECLARE_DYNCREATE(CMatView)

	CPtrArray aMaterials;
	int nSelectedMaterial;
	int m_nIDDrag;
	CPalette *m_pPalette;

public:
	virtual ~CMatView();
	BOOL AddMaterial(SURFACE *);
	void UpdateMaterial(SURFACE *pSurf);
	void FreeMaterials();

	virtual void OnDraw(CDC *);
	afx_msg BOOL OnEraseBkgnd(CDC *);
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonDblClk(UINT, CPoint);
	afx_msg void OnRButtonDown(UINT, CPoint);
	afx_msg void OnNewMaterial();
	afx_msg void OnLoadNewMaterial();
	afx_msg void OnLoadMaterial();
	afx_msg void OnSaveMaterial();
	afx_msg void OnAssign();
	afx_msg void OnAttributes();
	afx_msg void OnRename();
	afx_msg void OnDelete();
	afx_msg void OnRemoveUnused();
	afx_msg void OnRemoveIdentical();
	afx_msg int OnMouseActivate(CWnd *, UINT, UINT);
	virtual DROPEFFECT OnDragOver(COleDataObject*, DWORD, CPoint);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD grfKeyState, CPoint point);
	virtual void OnDragLeave();

private:
	int GetMaterialIndex(CPoint);

#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext&) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};

class CRenameDlg : public CDialog
{
public:
	CRenameDlg(CWnd *pParent = NULL); 
	enum { IDD = IDD_RENAME };

	CString m_sName;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

#endif
