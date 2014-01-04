/***************
 * NAME:				register.cpp
 * PROJECT:			Register
 * DESCRIPTION:	Dialog for registering a program
 * AUTHORS:			Mike Hesser
 * BUGS:				none
 * TO DO:         -
 * HISTORY:
 *		DATE			NAME	COMMENT
 *		13.09.1995	mh		first release (0.1)
 *		16.09.1995	mh		printer established (0.2)
 *		20.09.1995	mh		added text (0.3)
 *		21.09.1995	mh		release version (1.0)
 *		11.10.1995	mh		added Transport form E-Mail, S-Mail, LEFTMARGIN, display of price
 *		23.10.1995	mh		UpdatePrice works with Edit-box
 *		20.03.1996	mh		ported to MFC
 *		27.03.1996	mh		printing works with MFC
 *		09.10.1996	mh		new text
 *		12.11.1996	mh		save, show, new price politic
 ***************/

#include <stdio.h>
#include "resource.h"
#include "dialog.h"

static char *lines[] =
{
	"",
	"",
	"",
	"",
	"",
	"RayStorm registration formular",
	"--------------------------------------------------------------------",
	"","","","","","",
	"      Mike Hesser",
	"      Heilmeyersteige 105",
	"      89075 Ulm",
	"      Germany",
	"","","","","",NULL,"","","","","","","","","",
	"","","","","","","","","","",
	"Comments:___________________________________________________________",
	"",
	"____________________________________________________________________",
	"",
	"____________________________________________________________________",
	"","","",
	"I have read the copyright note and do agree. I've included the",
	"demanded sum in cash. I won't distribute the keyfile.",
	"","","",
	"_____________________________      ________________________________",
	"        (City, Date)                          (Signature)",
	NULL
};

/*
		"DM 40.-",		// S-Mail, DM,				PC/Amiga
		"DM 30.-",		// E-Mail, DM,				PC/Amiga
		"US$ 25.-",		// S-Mail, US Dollar,	PC/Amiga,
		"US$ 20.-",		// E-Mail, US Dollar,	PC/Amiga,
		"DM 60.-",		// S-Mail, DM,				Amiga + PC
		"DM 50.-",		// E-Mail, DM,				Amiga + PC
		"US$ 40.-",		// S-Mail, US Dollar,	Amiga + PC,
		"US$ 35.-",		// E-Mail, US Dollar,	Amiga + PC
*/

static CEdit edit;
static FILE *file;
static float prices[] = {40.0f, 30.0f, 25.0f, 20.0f, 60.0f, 50.0f, 40.0f, 35.0f};
static char *currencies[] = {"DM  ","US$ "};
static char *version[] = {"", "PC        ",	"Amiga     ", "PC & Amiga"};
static char *forms[]	= {"Letter", "E-Mail"};
static char *infos[] =
{
"The RayStorm PC/Amiga Letter distribution consists of one HD disk.\n\
The package includes a keyfile and the latest version of the program.",
"RayStorm PC/Amiga E-Mail distribution: a keyfile is e-mailed to you.\n\
You have to specify an Internet E-Mail address in the registration\n\
form. If you receive the mail, you have to uudecode the keyfile and\n\
copy it to the RayStorm drawer ('S:' drawer for Amiga version). The \n\
latest version of RayStorm is available via FTP or through our WWW \n\
home page.",
};


// declare message map
BEGIN_MESSAGE_MAP(CRegisterBox, CDialog)
	ON_COMMAND(ID_REGISTER_INFO,	OnInfo)
//	ON_COMMAND(ID_REGISTER_SHOW,	Show)
	ON_COMMAND(ID_REGISTER_SAVE,	OnSave)
	ON_COMMAND(ID_REGISTER_PRINT, OnPrint)
	ON_COMMAND(ID_REGISTER_US,		UpdatePrice)
	ON_COMMAND(ID_REGISTER_DM,		UpdatePrice)
	ON_COMMAND(ID_REGISTER_EMAIL,	UpdatePrice)
	ON_COMMAND(ID_REGISTER_SMAIL,	UpdatePrice)
	ON_COMMAND(ID_REGISTER_PC,		UpdatePrice)
	ON_COMMAND(ID_REGISTER_AMIGA,	UpdatePrice)
END_MESSAGE_MAP()


/*************
 * DESCRIPTION:	constructor
 * INPUT:			system relevant
 * OUTPUT:			-
 *************/
CRegisterBox::CRegisterBox(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterBox::IDD, pParent)
{
	device = PRINTER;
}

/*************
 * DESCRIPTION:	update price display
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CRegisterBox::UpdatePrice()
{
	char szBuf[30];

	curr = IsDlgButtonChecked(ID_REGISTER_US) ? 1 : 0;
	form = IsDlgButtonChecked(ID_REGISTER_EMAIL) ? 1 : 0;

	// nobody may order nothing!
	if (!IsDlgButtonChecked(ID_REGISTER_PC) && !IsDlgButtonChecked(ID_REGISTER_AMIGA))
	{
		if (ver & 1)
			CheckDlgButton(ID_REGISTER_PC, 1);
		else
			CheckDlgButton(ID_REGISTER_AMIGA, 1);
	}
	ver = (IsDlgButtonChecked(ID_REGISTER_PC) ? 1 : 0)
		  + (IsDlgButtonChecked(ID_REGISTER_AMIGA) ? 2 : 0);

	ix = 2*curr + form + ((ver == 3) ? 4 : 0);

	sprintf(szBuf, "%4.0f %s", prices[ix], currencies[curr]);
	SetDlgItemText(ID_REGISTER_PRICE, szBuf);
}

/*************
 * DESCRIPTION:	intializes dialog
 * INPUT:			-
 * OUTPUT:			TRUE on success
 *************/
BOOL CRegisterBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	CheckDlgButton(ID_REGISTER_PC, 1);
	CheckDlgButton(ID_REGISTER_AMIGA, 0);
	CheckRadioButton(ID_REGISTER_DM, ID_REGISTER_US, ID_REGISTER_DM);
	CheckRadioButton(ID_REGISTER_EMAIL, ID_REGISTER_SMAIL, ID_REGISTER_EMAIL);

	UpdatePrice();

	return TRUE;
}

/*************
 * DESCRIPTION:	gives information about the current settings
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CRegisterBox::OnInfo()
{
	MessageBox(infos[form], "Info");
}

/*************
 * DESCRIPTION:	print formular on printer
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CRegisterBox::OnPrint()
{
	device = PRINTER;
	MakeFormular();
}

/*************
 * DESCRIPTION:	show formular in a window
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CRegisterBox::OnShow()
{
	device = WINDOW;
	CRect rect(300,500,200,300);

//	edit.Create(WS_CHILD | WS_VISIBLE, rect, this, -1);
//	if (MakeFormular());
}

/*************
 * DESCRIPTION:	save formular in a file
 * INPUT:			-
 * OUTPUT:			-
 *************/
void CRegisterBox::OnSave()
{
	BOOLEAN bRes;

	device = TEXTFILE;

	file = fopen("register.txt", "w");
	if (!file)
	{
		MessageBox("Error saving file !", "Info");
		return;
	}
	
	bRes = MakeFormular();
	fclose(file);

	if (bRes)
		MessageBox("Saved under \"register.txt\"", "Info");
	else
		DeleteFile("register.txt");		
}

/*************
 * DESCRIPTION:	print out line (printer, file, window)
 * INPUT:			hPr	Device Context of printer
 *						text	text to print
 * OUTPUT:			-
 *************/
void CRegisterBox::Print(HDC hPr, char *text)
{
	char szBuf[256];
	
	sprintf(szBuf, "        %s", text);
	switch (device)
	{
		case PRINTER: 
			TextOut(hPr, 0, nCurrentLine*LineSpace, szBuf, strlen(szBuf));
			break;
		case TEXTFILE:
			fprintf(file, "%s\n", szBuf);
			break;
		case WINDOW: 
			break;
	}
	nCurrentLine++;
}

/*************
 * FUNCTION:		CRegisterBox::PrintItem
 * DESCRIPTION:	used for printing address items
 * INPUT:			hPr	Device Context of printer
 *						text	text to print, followed by
 *						item	item text
 * OUTPUT:			-
 *************/
void CRegisterBox::PrintItem(HDC hPr, char *szText, char *szItem)
{
	char szBuf[256];

	sprintf(szBuf, "%s %s", szText, szItem);
	Print(hPr, szBuf);
}

/*************
 * DESCRIPTION:	make formular and put it to printer/file/window
 * INPUT:			-
 * OUTPUT:			-
 *************/
BOOL CRegisterBox::MakeFormular()
{
	TEXTMETRIC tm;
	POINT PhysPageSize;
	CDC *pDC;
	HDC hPr;
	int nLinesPerPage, no;
	char szBuf[128];
	float sum;

	CPrintDialog PrintDlg(FALSE);
	
	GetDlgItemText(ID_REGISTER_FNAME,  fname,   STRMAX);
	GetDlgItemText(ID_REGISTER_LNAME,  lname,   STRMAX);
	GetDlgItemText(ID_REGISTER_STREET, street,  STRMAX);
	GetDlgItemText(ID_REGISTER_CITY,	  city,    STRMAX);
	GetDlgItemText(ID_REGISTER_COUNTRY,country, STRMAX);
	GetDlgItemText(ID_REGISTER_EMAILIT,email,   STRMAX);
					 
	if (!(fname[0] && lname[0] && street[0] && city[0] && country[0]))
	{
		MessageBox("You didn't fill out enough items!", NULL, MB_OK|MB_ICONHAND);
		return FALSE;
	}

	if (form == 1 && !email[0])
	{
		MessageBox("You have to specify an E-Mail address\nif you want to order via E-Mail!", NULL, MB_OK|MB_ICONHAND);
		return FALSE;
	}

	if (device == PRINTER)
	{
		PrintDlg.DoModal();
		if (AfxGetApp()->GetPrinterDeviceDefaults(&PrintDlg.m_pd))
		{
			hPr = PrintDlg.GetPrinterDC();
			if (!hPr)
			{
				MessageBox("Printer error", NULL, MB_OK|MB_ICONHAND);
				return FALSE;
			}
		}
		else
		{
			MessageBox("Printer error", NULL, MB_OK|MB_ICONHAND);
			return FALSE;
		}

		pDC = CDC::FromHandle(hPr);

		if (pDC->Escape(STARTDOC, 14, (LPSTR)"PrntFile text", NULL) < 0)
		{
			MessageBox("Unable to start print job", NULL, MB_OK|MB_ICONHAND);
			DeleteDC(hPr);
			return FALSE;
		}

		pDC->GetTextMetrics(&tm);
		LineSpace = tm.tmHeight + tm.tmExternalLeading;
		pDC->Escape(GETPHYSPAGESIZE, NULL, (LPSTR) NULL, (LPSTR) &PhysPageSize);
		nLinesPerPage = PhysPageSize.y / LineSpace;
	}

	nLineLength = 10;
	nCurrentLine = 0;

	no = 1;
	while (lines[nCurrentLine])
		Print(hPr, lines[nCurrentLine]);

	sum = 0.0f;

	PrintItem(hPr, "     First   :", fname);
	PrintItem(hPr, "     Name    :", lname);
	PrintItem(hPr, "     Street  :", street);
	PrintItem(hPr, "     ZIP,City:", city);
	PrintItem(hPr, "     Country :", country);
	Print(hPr, "");
	PrintItem(hPr, "     E-Mail  :", email);
	Print(hPr, "");
	Print(hPr, "");
	Print(hPr, "");
	Print(hPr, "");
	Print(hPr, "No.    Article                                      Price  Sum");
	Print(hPr, "--------------------------------------------------------------------");

	sprintf(szBuf, " 1     RayStorm for %s via %s           %4.2f  %4.2f %s", version[ver], forms[form], prices[ix], prices[ix] * no, currencies[curr]);
	Print(hPr, szBuf);
	sum+= prices[ix]*no;

	Print(hPr, "====================================================================");
	sprintf(szBuf, "TOTAL                                                      %4.2f %s", sum, currencies[curr]);
	Print(hPr, szBuf);
										 
	while (lines[nCurrentLine])
	 	Print(hPr, lines[nCurrentLine]);

	if (device == PRINTER)
	{
		pDC->Escape(NEWFRAME, 0, 0L, 0L);
		pDC->Escape(ENDDOC, 0, 0L, 0L);
		DeleteDC(hPr);
	}

	return TRUE;
}
