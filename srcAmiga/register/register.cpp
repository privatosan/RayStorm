/***************
 * MODUL:         Register
 * NAME:          register.cpp
 * DESCRIPTION:   RayStorm registration formular
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    14.09.95 ah    initial release
 *    03.04.96 ah    added special version for both platforms
 *    04.09.96 ah    added HD-version for amiga
 *    11.11.96 ah    new prices
 ***************/

#include <wbstartup.h>
#include <iostream.h>
#include <stdio.h>
#include <string.h>

#include <libraries/asl.h>
#include <libraries/dos.h>

#include <pragma/asl_lib.h>
#include <pragma/dos_lib.h>

#include "interface.h"

UBYTE vers[] = "\0$VER: RayStorm Registration "__VERS__" ("__DATE__")";

struct Library *IntuitionBase = NULL;
struct Library *MUIMasterBase = NULL;
struct Library *AslBase = NULL;

static struct ObjApp *app = NULL;

#define PLATFORM_AMIGA  0
#define PLATFORM_PC     1

#define CURRENCY_DM     0
#define CURRENCY_DOLLAR 1

#define FORM_SMAIL      0
#define FORM_EMAIL      1

// Error messages
enum
{
	ERR_MUIMASTERLIB, ERR_INTUITIONLIB, ERR_ASLLIB,
	ERR_WINDOW, ERR_OPENPRINT, ERR_SHOW
};

static char *errors[] =
{
	"Can't open muimaster library.",
	"Can't open intuition library.",
	"Can't open asl library.",
	"Can't open window.",
	"Can't open printer.",
	"Can't show file."
};

static char *lines[] =
{
	"    RayStorm registration formular",
	"    --------------------------------------------------------------------",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"          Andreas Heumann",
	"          Wittelsbacherstr. 5",
	"          82319 Starnberg",
	"          Germany",
	"",
	"",
	"",
	"",
	"",
	NULL,
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"    Comments:___________________________________________________________",
	"",
	"    ____________________________________________________________________",
	"",
	"    ____________________________________________________________________",
	"",
	"",
	"",
	"    I have read the Copyright note and do agree. I've included the",
	"    demanded sum in cash.",
	"",
	"",
	"",
	"    _____________________________      ________________________________",
	"            (City, Date)                          (Signature)         ",
	NULL
};

static char *smail_amiga[] =
{
	"\033cThe RayStorm Amiga S-Mail distribution consists of one DD\n",
	"or one HD disk. The package includes a keyfile and the latest\n",
	"version of the program.",
	NULL
};

static char *smail_pc[] =
{
	"\033cThe RayStorm PC S-Mail distribution consists of one HD disk.\n",
	"The package includes a keyfile and the latest version of the\n",
	"program.",
	NULL
};

static char *smail_both[] =
{
	"\033cThe RayStorm Amiga & PC S-Mail distribution consists of one DD\n",
	"and one HD disk or two HD disks. The package includes the keyfiles and\n",
	"the latest versions.",
	NULL
};

static char *email_amiga[] =
{
	"\033cRayStorm Amiga E-Mail distribution: a keyfile is e-mailed to you.\n",
	"You have to specify an Internet E-Mail address in the registration form.",
	NULL
};

static char *email_pc[] =
{
	"\033cRayStorm PC E-Mail distribution: a keyfile is e-mailed to you.\n",
	"You have to specify an Internet E-Mail address in the registration\n",
	"form.",
	NULL
};

static char *email_both[] =
{
	"\033cRayStorm Amiga & PC E-Mail distribution: two keyfiles are e-mailed\n",
	"to you. You have to specify  an Internet E-Mail address in the\n",
	"registration form.",
	NULL
};

/*************
 * FUNCTION:      closeall
 * DESCRIPTION:   Close all
 * INPUT:         none
 * OUTPUT:        none
 *************/
void closeall()
{
	// Close window
	if(app)
		DisposeApp(app);

	if (MUIMasterBase)
		CloseLibrary(MUIMasterBase);
	if (IntuitionBase)
		CloseLibrary(IntuitionBase);
	if(AslBase)
		CloseLibrary(AslBase);
}

/*************
 * FUNCTION:      request
 * DESCRIPTION:   show a requester
 * INPUT:         none
 * OUTPUT:        none
 *************/
void request(char *string)
{
	MUI_RequestA(app->App,NULL,0,NULL,"*_Ok",string,TAG_DONE);
}

/*************
 * FUNCTION:      bye
 * DESCRIPTION:   Close all, write errormessage, exit
 * INPUT:         string      pointer to error string
 * OUTPUT:        none
 *************/
void bye(char *string)
{
	request(string);
	closeall();
	exit(1);
}

/*************
 * FUNCTION:      GetIndex
 * DESCRIPTION:   Gets index according to selected buttons and cycles
 * INPUT:         none
 * OUTPUT:
 *    0 : S-Mail, DM,         Amiga
 *    1 : S-Mail, US Dollar,  Amiga
 *    2 : S-Mail, DM,         PC
 *    3 : S-Mail, US Dollar,  PC
 *    4 : S-Mail, DM,         Amiga + PC
 *    5 : S-Mail, US Dollar,  Amiga + PC
 *    6 : E-Mail, DM,         Amiga
 *    7 : E-Mail, US Dollar,  Amiga
 *    8 : E-Mail, DM,         PC
 *    9 : E-Mail, US Dollar,  PC
 *    10: E-Mail, DM,         Amiga + PC
 *    11: E-Mail, US Dollar,  Amiga + PC
 *************/
int GetIndex()
{
	ULONG form,currency,amiga,pc, pos;

	GetAttr(MUIA_Cycle_Active, (Object*)app->cy_cur, &currency);
	GetAttr(MUIA_Cycle_Active, (Object*)app->cy_form, &form);
	GetAttr(MUIA_Selected, (Object*)app->cm_amiga, &amiga);
	GetAttr(MUIA_Selected, (Object*)app->cm_pc, &pc);

	pos = currency + form*6;
	if(pc)
		pos += 2;
	if(amiga && pc)
		pos += 2;

	return pos;
}

/*************
 * FUNCTION:      printit
 * DESCRIPTION:   print registration
 * INPUT:         file     output file
 *                ff       formfeed if TRUE
 * OUTPUT:        none
 *************/
void printit(FILE *file, BOOL ff)
{
	int i;
	char *first,*last,*street,*city,*country,*email;
	ULONG index,index1,amiga,pc,form, cpu;
	float price;
	char *currencies[] = {"DM","US$"};
	char *platform[] = {"Amiga     ","PC        ","Amiga & PC"};
	char *forms[] = {"S-Mail   ","E-Mail   ","HD S-Mail","E-Mail   ",};
	char *cpus[] = {"68030+881", "68040    ", "68060    ", "PPC      " };
	int prices[] = {40, 25, 40, 25, 60, 40, 30, 20, 30, 20, 50, 35};

	GetAttr(MUIA_String_Contents, (Object*)app->s_fname, (ULONG*)&first);
	GetAttr(MUIA_String_Contents, (Object*)app->s_lname, (ULONG*)&last);
	GetAttr(MUIA_String_Contents, (Object*)app->s_street, (ULONG*)&street);
	GetAttr(MUIA_String_Contents, (Object*)app->s_city, (ULONG*)&city);
	GetAttr(MUIA_String_Contents, (Object*)app->s_country, (ULONG*)&country);
	GetAttr(MUIA_String_Contents, (Object*)app->s_mail, (ULONG*)&email);
	if(first[0] == 0 || last[0] == 0 || street[0] == 0 || city[0] == 0 || country[0] == 0)
	{
		request("You have to specify the whole address with\ncomplete name, street, city and country.");
		return;
	}
	GetAttr(MUIA_Cycle_Active, (Object*)app->cy_form, &form);
	GetAttr(MUIA_Cycle_Active, (Object*)app->cy_cpu, &cpu);
	if(form == FORM_EMAIL && email[0] == 0)
	{
		request("You have to specify a E-Mail address\nif you want to order per E-Mail.");
		return;
	}

	i = 0;
	while(lines[i])
	{
		fprintf(file,"%s\n", lines[i]);
		i++;
	}

	GetAttr(MUIA_Selected, (Object*)app->cm_amiga, &amiga);
	GetAttr(MUIA_Selected, (Object*)app->cm_pc, &pc);

	price = (float)(prices[GetIndex()]);

	GetAttr(MUIA_Selected, (Object*)app->cm_hd, &index);
	if(index && amiga)
		form += 2;

	if(amiga)
		index1 = 0;
	if(pc)
		index1 = 1;
	if(amiga && pc)
		index1 = 2;

	GetAttr(MUIA_Cycle_Active, (Object*)app->cy_cur, &index);

	fprintf(file, "-   First   : %s\n",first);
	fprintf(file, "    Name    : %s\n",last);
	fprintf(file, "    Street  : %s\n",street);
	fprintf(file, "    ZIP,City: %s\n",city);
	fprintf(file, "    Country : %s\n\n",country);
	fprintf(file, "    E-Mail  : %s\n\n\n\n\n",email);
	fprintf(file, "    No.    Article                                      Price  Sum\n");
	fprintf(file, "    --------------------------------------------------------------------\n");
	fprintf(file, "     1     RayStorm %s %s %s      %4.2f  %4.2f %s\n",
		platform[index1], forms[form], cpus[cpu], price, price, currencies[index]);
	fprintf(file, "    ====================================================================\n");
	fprintf(file, "    TOTAL                                                      %4.2f %s\n", price, currencies[index]);
	i++;
	while (lines[i])
	{
		fprintf(file, "%s\n", lines[i]);
		i++;
	}
	if(ff)
		fprintf(file, "%c", 12);
}

/*************
 * FUNCTION:      print
 * DESCRIPTION:   print registration formular
 * INPUT:         obj      pointer to calling object
 * OUTPUT:        none
 *************/
void print(Object *obj)
{
	FILE *prt;

	prt = fopen("prt:","w");
	if(!prt)
		request(errors[ERR_OPENPRINT]);
	else
	{
		printit(prt, TRUE);

		fclose(prt);
	}
}

/*************
 * FUNCTION:      info
 * DESCRIPTION:   print informations
 * INPUT:         obj      pointer to calling object
 * OUTPUT:        none
 *************/
void info(Object *obj)
{
	char buffer[256], **s;
	int i;

	char **infos[] =
	{
		smail_amiga,smail_amiga,smail_pc,smail_pc,smail_both,smail_both,
		email_amiga,email_amiga,email_pc,email_pc,email_both,email_both,
	};

	s = infos[GetIndex()];
	i = 0;
	buffer[0] = 0;
	while(s[i])
	{
		strcat(buffer,s[i]);
		i++
	}

	request(buffer);
}

/*************
 * FUNCTION:      price
 * DESCRIPTION:   update price
 * INPUT:         obj      pointer to calling object
 * OUTPUT:        none
 *************/
void price(Object *obj)
{
	char *prices[] =
	{
		"DM 40.-",     // S-Mail, DM,          Amiga
		"US$ 25.-",    // S-Mail, US Dollar,   Amiga,
		"DM 40.-",     // S-Mail, DM,          PC
		"US$ 25.-",    // S-Mail, US Dollar,   PC,
		"DM 60.-",     // S-Mail, DM,          Amiga + PC
		"US$ 40.-",    // S-Mail, US Dollar,   Amiga + PC,
		"DM 30.-",     // E-Mail, DM,          Amiga
		"US$ 20.-",    // E-Mail, US Dollar,   Amiga,
		"DM 30.-",     // E-Mail, DM,          PC
		"US$ 20.-",    // E-Mail, US Dollar,   PC,
		"DM 50.-",     // E-Mail, DM,          Amiga + PC
		"US$ 35.-",    // E-Mail, US Dollar,   Amiga + PC
	};

	SetAttrs((Object*)app->tx_price,MUIA_Text_Contents,
		prices[GetIndex()],TAG_DONE);
}

/*************
 * FUNCTION:      show
 * DESCRIPTION:   show preview of formular
 * INPUT:         obj      pointer to calling object
 * OUTPUT:        none
 *************/
void show(Object *obj)
{
	FILE *f;

	f = fopen("con:0/0/640/512/Registration form/WAIT/CLOSE","w");
	if(!f)
		request(errors[ERR_SHOW]);
	else
	{
		printit(f, FALSE);

		fclose(f);
	}
}

/*************
 * FUNCTION:      save
 * DESCRIPTION:   save preview of formular
 * INPUT:         obj      pointer to calling object
 * OUTPUT:        none
 *************/
void save(Object *obj)
{
	FILE *f;
	struct FileRequester *filerequ;
	char file[256];

	filerequ = (struct FileRequester *)
		AllocAslRequestTags(ASL_FileRequest,
		ASLFR_Window,           _window(obj),
		ASLFR_TitleText,        "Select file",
		ASLFR_RejectIcons,      TRUE,
		ASLFR_DoSaveMode,       TRUE,
		TAG_DONE);

	if (filerequ)
	{
		if(AslRequest(filerequ, NULL))
		{
			strcpy(file, filerequ->rf_Dir);
			AddPart(file, filerequ->rf_File, 255);

			f = fopen(file, "w");
			if(!f)
				request(errors[ERR_SHOW]);
			else
			{
				printit(f, FALSE);
				fclose(f);
			}
		}

		FreeAslRequest(filerequ);
	}
}

void main()
{
	BOOL done = FALSE;
	ULONG signals,id;

	// Open intuition library.
	IntuitionBase = OpenLibrary("intuition.library",0);
	if (!IntuitionBase)
		bye(errors[ERR_INTUITIONLIB]);

	// Open muimaster library.
	MUIMasterBase = OpenLibrary(MUIMASTER_NAME,0);
	if (!MUIMasterBase)
		bye(errors[ERR_MUIMASTERLIB]);

	// Open asl library.
	AslBase = OpenLibrary("asl.library",0);
	if (!AslBase)
		bye(errors[ERR_ASLLIB]);

	// open window
	app = CreateApp();
	if(!app)
		bye(errors[ERR_WINDOW]);

	while(!done)
	{
		id = DoMethod((Object *)app->App,MUIM_Application_Input,&signals);
		switch (id)
		{
			case MUIV_Application_ReturnID_Quit:
				done = TRUE;
				break;
		}
		if (!done && signals)
			Wait(signals);
	}
	closeall();
}

#ifdef __STORM__
void wbmain(struct WBStartup *)
{
	BPTR lock;

	// get a lock to the program directory
	lock = GetProgramDir();
	// make program directory to current directory
	lock = CurrentDir(lock);

	main();

	// change current dir to old dir
	lock = CurrentDir(lock);

	exit(0);
}
#endif
