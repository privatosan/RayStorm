/***************
 * PROGRAM:       codekey
 * NAME:          codekey.cpp
 * DESCRIPTION:   main module for keyfile coder
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.01.97 ah    initial release
 ***************/

#include <libraries/mui.h>
#include <libraries/locale.h>
#include <graphics/gfxmacros.h>

#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/asl_protos.h>
#include <clib/locale_protos.h>

#include <wbstartup.h>
#include <workbench/startup.h>
#include <pragma/muimaster_lib.h>

#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "typedefs.h"

#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "codekey_strings.h"

struct Library *MUIMasterBase;
//struct Library *LocaleBase;
static struct Catalog *catalog;

enum
{
	AMIGA,PC
};

typedef unsigned char UBYTE;

static char *Computer[] =
{
	"AMIGA",
	"PC",
	NULL
};

static char *keyfile_types[] =
{
	"Old",
	"New",
	NULL
};

static Object *files,*names,*compc,*serials, *keyfiletype;
static APTR app,window;

static UBYTE data[33] =
{
	'K','E','Y', 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 ,
	 0 , 0 , 0 , 0
};

typedef struct
{
	char name[32];
	ULONG sernum;
	int flags;
} USER;

#define FLAGS_SYSTEM_AMIGA 1
#define FLAGS_SYSTEM_PC    2

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

char *GetString(LONG id, char *def)
{
	return GetCatalogStr(catalog, id, def);
}

/*************
 * FUNCTION:      code
 * DESCRIPTION:   create keyfile
 * INPUT:         version     AMIGA or PC
 *                name        name of registered user
 *                serial      serial number
 *                file        output filename
 *                type        type of file to create (old = 0, new = 1)
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL code(int version, char name[], int serial, char file[], ULONG type)
{
	int i,j;
	ULONG r,c;
	FILE *keyfile;
	USER user;

	if(!type)
	{
		// copy name to array
		strcpy(&data[4],name);

		data[29] = serial >> 24;
		data[30] = (serial >> 16) & 0xff;
		data[31] = (serial >> 8) & 0xff;
		data[32] = serial & 0xff;

		if(version == AMIGA)
			data[3] = 'A';
		else
			data[3] = 'P';
	}
	else
	{
		strcpy(user.name, name);
		if(strlen(name) < 31)
		{
			// confuse this damned hackers a little bit
			// by inseting random numbers
			for(i=strlen(name)+1; i<32; i++)
				user.name[i] = rand();
		}

#ifdef __AMIGA__
		user.sernum = serial;
#else
		user.sernum = Swap(serial);
#endif
		if(version == AMIGA)
			user.flags = FLAGS_SYSTEM_AMIGA;
		else
			user.flags = FLAGS_SYSTEM_PC;
	}

	keyfile = fopen(file,"wb");
	if(!keyfile)
	{
		MUI_Request(app,window,0,NULL,GetString(MSG_0003, MSG_0003_STR),GetString(MSG_0004, MSG_0004_STR),file);
		return FALSE;
	}

	if(!type)
	{
		if(fwrite(&data,1,33,keyfile) != 33)
		{
			MUI_Request(app,window,0,NULL,GetString(MSG_0003, MSG_0003_STR),GetString(MSG_0006, MSG_0006_STR),file);
			fclose(keyfile);
			return FALSE;
		}
	}
	else
	{
		fprintf(keyfile, "// this file was automatically generated\n");
		fprintf(keyfile, "// do not edit or change it\n");
		fprintf(keyfile, "USER user =\n");
		fprintf(keyfile, "{\n");
		fprintf(keyfile, "	{ ");
		for(i=0; i<32; i++)
			fprintf(keyfile, "%d, ", user.name[i]);
		fprintf(keyfile, "},\n");
		fprintf(keyfile, "   %d,\n", serial);
		fprintf(keyfile, "   %s\n", version == AMIGA ? "FLAGS_SYSTEM_AMIGA" : "FLAGS_SYSTEM_PC");
		fprintf(keyfile, "};\n\n");
		fprintf(keyfile, "ULONG coded_user[40] =\n");
		fprintf(keyfile, "{\n");
	}

	// set random seed
	r = serial;

	for(i=0; i<(type ? 40 : 33); i++)
	{
		if(!type)
			c = data[i];
		else
			c = user.name[i];
		for(j=0; j<i+1; j++)
		{
			r = (240995 + 4194301*r) % 2147483647;
			c += r;
		}
		if(!type)
		{
			if(fwrite(&c,1,4,keyfile) != 4)
			{
				MUI_Request(app,window,0,NULL,GetString(MSG_0003, MSG_0003_STR),GetString(MSG_0006, MSG_0006_STR),file);
				fclose(keyfile);
				return FALSE;
			}
		}
		else
		{
			if(fprintf(keyfile, "   0x%08x,\n", c) < 0)
			{
				MUI_Request(app,window,0,NULL,GetString(MSG_0003, MSG_0003_STR),GetString(MSG_0006, MSG_0006_STR),file);
				fclose(keyfile);
				return FALSE;
			}
		}
	}

	if(type)
		fprintf(keyfile, "};\n");

	fclose(keyfile);

	return TRUE;
}

void Cleanup()
{
	CloseCatalog(catalog);

	if(MUIMasterBase)
		CloseLibrary(MUIMasterBase);
}

/*************
 * FUNCTION:      make
 * DESCRIPTION:   callback function for make button
 * INPUT:         obj
 * OUTPUT:        -
 *************/
void make(Object *obj)
{
	int version, serial;
	char *name, *file;
	ULONG type;

	GetAttr(MUIA_Cycle_Active,compc,(ULONG*)&version);
	GetAttr(MUIA_String_Contents,names,(ULONG*)&name);
	GetAttr(MUIA_String_Integer,serials,(ULONG*)&serial);
	GetAttr(MUIA_String_Contents,files,(ULONG*)&file);
	GetAttr(MUIA_Cycle_Active, keyfiletype, &type);
	if(code(version, name, serial, file, type))
		MUI_Request(app,window,0,NULL,GetString(MSG_0003, MSG_0003_STR),GetString(MSG_0010, MSG_0010_STR),file,name, type);
}

int main(int argc,char *argv[])
{
	Object *makeb,*exitb;
	static const struct Hook makeHook = { { NULL,NULL },(HOOKFUNC)make,NULL,NULL};

	MUIMasterBase = OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN);
	if(!MUIMasterBase)
	{
		printf(GetString(MSG_0011, MSG_0011_STR), MUIMASTER_NAME);
		exit(0);
	}

	catalog = OpenCatalogA(NULL,"codekey.catalog",NULL);

	app = ApplicationObject,
		MUIA_Application_Title      , GetString(MSG_0012, MSG_0012_STR),
		MUIA_Application_Version    , "$VER: RayStorm-Register 2.0 (11.09.97)",
		MUIA_Application_Copyright  , "©1996,97 Andreash Heumann ",
		MUIA_Application_Author     , "Andreas Heumann",
		MUIA_Application_Description, GetString(MSG_0016, MSG_0016_STR),
		MUIA_Application_Base       , "Registration",

		SubWindow, window = WindowObject,
			MUIA_Window_Title, "Codekey",
			MUIA_Window_ID   , MAKE_ID('C','K','E','Y'),

			WindowContents, VGroup,

				Child, ColGroup(2),
					Child, Label2(GetString(MSG_0019, MSG_0019_STR)), Child, names = String("",256),
					Child, Label1(GetString(MSG_0021, MSG_0021_STR)), Child, compc = Cycle(Computer),
					Child, Label2(GetString(MSG_0022, MSG_0022_STR)),
					Child, serials = StringObject,
						MUIA_CycleChain, TRUE,
						StringFrame,
						MUIA_String_Accept,"0123456789",
						MUIA_String_Integer,1000,
					End,
					Child, KeyLabel2(GetString(MSG_0024, MSG_0024_STR),'f'),
					Child, PopaslObject,
						MUIA_CycleChain, TRUE,
						MUIA_Popstring_String, files = KeyString(0,256,'f'),
						MUIA_Popstring_Button, PopButton(MUII_PopFile),
						ASLFR_TitleText, GetString(MSG_0025, MSG_0025_STR),
					End,
					Child, Label2(GetString(MSG_0029, MSG_0029_STR)),
					Child, keyfiletype = CycleObject,
						MUIA_CycleChain, TRUE,
						MUIA_Cycle_Entries, keyfile_types,
						MUIA_Cycle_Active, 1,
					End,
				End,
				Child, HGroup,
					Child, makeb = SimpleButton(GetString(MSG_0026, MSG_0026_STR)),
					Child, exitb = SimpleButton(GetString(MSG_0027, MSG_0027_STR)),
				End,
			End,
		End,
	End;

	if (!app)
	{
		cout << GetString(MSG_0028, MSG_0028_STR) << endl;
		Cleanup();
		exit(0);
	}

	DoMethod((Object*)window,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
		app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

	DoMethod((Object*)makeb,MUIM_Notify,MUIA_Pressed,FALSE,
		makeb,2,MUIM_CallHook,&makeHook);

	DoMethod((Object*)exitb,MUIM_Notify,MUIA_Pressed,FALSE,
		app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

	SetAttrs(files, MUIA_String_Contents, "ray:keyfile/codekey.h", TAG_DONE);
	SetAttrs(window, MUIA_Window_Open, TRUE, TAG_DONE);

	{
		ULONG sigs = 0;

		while (DoMethod((Object*)app,MUIM_Application_NewInput,&sigs) != MUIV_Application_ReturnID_Quit)
		{
			if (sigs)
			{
				sigs = Wait(sigs | SIGBREAKF_CTRL_C);
				if (sigs & SIGBREAKF_CTRL_C) break;
			}
		}
	}

	SetAttrs(window, MUIA_Window_Open, FALSE, TAG_DONE);

	Cleanup();
	exit(0);
}

#ifdef __STORM__
void wbmain(struct WBStartup *msg)
{
	main(0, NULL);

	exit(0);
}
#endif
