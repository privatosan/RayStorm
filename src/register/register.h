/***************
 * NAME:				register.h
 * PROJECT:			Register
 * VERSION:			1.1 02.12.1996
 * DESCRIPTION:	main module definition file
 * AUTHORS:			Mike Hesser
 * BUGS:				none
 * TO DO:         -
 * HISTORY:
 *		DATE			NAME	COMMENT
 *		02.12.96		mh		own program
 ***************/

#ifndef REGISTER_H
#define REGISTER_H

#include "stdafx.h"
#include "resource.h"       // main symbols

class CRegisterApp : public CWinApp
{
public:
	CRegisterApp();

	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

#endif