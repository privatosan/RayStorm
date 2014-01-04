/***************
 * NAME:				dialog.h
 * PROJECT:			Register
 * DESCRIPTION:	definitions for register dialog
 * AUTHORS:			Mike Hesser
 * BUGS:				none
 * TO DO:			-
 * HISTORY:       DATE			NAME	COMMENT
 *						13.09.1995	mh		first release
 *						22.03.1996	mh		all works except printing
 ***************/

#ifndef DIALOG_H
#define DIALOG_H

#include "register.h"
#include "resource.h"

#define STRMAX 40
typedef char DLGSTR[STRMAX];

enum DEVICES
{
	PRINTER,
	WINDOW,
	TEXTFILE
};


// CRegisterbox class
class CRegisterBox : public CDialog
{
public:
	CRegisterBox(CWnd *pParent = NULL);
	enum { IDD = IDD_REGISTER };

protected:
	int nCurrentLine, nLineLength, LineSpace, curr, ver, form, ix;
	DLGSTR fname, lname, street, city, country, email;
	int device;

	afx_msg void OnInfo();
	afx_msg void OnPrint();
	afx_msg void OnShow();
	afx_msg void OnSave();
	afx_msg BOOL MakeFormular();
	
	virtual BOOL OnInitDialog();
	void	Print(HDC, char *);
	void	PrintItem(HDC, char *, char *);
	void	UpdatePrice();
	
	DECLARE_MESSAGE_MAP()
};
	
#endif
