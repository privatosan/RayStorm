/***************
 * NAME:				parser.cpp
 * PROJECT:			RayStorm
 * DESCRIPTION:	parses the RayStorm script language
 * AUTHORS:			Mike Hesser
 * HISTORY:			DATE			NAME	COMMENT
 *						19.03.1996	mh		first release (0.1)
 *					   03.02.1997	mh		rsiStarTilt
 ***************/

#include <math.h>
#include "parser.h"
#include "rsi.h"
#include "fileutil.h"
#include "itemlist.h"
#include "csgstack.h"
#include "interface.h"

#ifndef PI
#define PI 3.141592654
#endif

extern rsiCONTEXT *rc;
extern char *app_errors[];

enum
{
	RETURN_OK
};

extern "C"
{
	ULONG picturepath(ULONG *);
	ULONG includepath(ULONG *);
	ULONG include(ULONG *);
}

// structures
struct RAYCOMMAND
{
	char *name;
	ULONG (*func)(ULONG *arg);
};

struct INCLUDE
{
	char name[256];
	FILE *hFile;
	int linenumber;
};

// maximum nesting of includes
#define MAXINCLUDE	10
#define ERROR_BASE	100

#define MAXARGS 30
#define KEYWORD(a,b) (a ? (!stricmp((char*)(a), b)) : FALSE)

// all commands RayStorm knows
static RAYCOMMAND commands[] =
{
	{"TRIANGLE",		triangle},
	{"NEWSURFACE",		newsurface},
	{"SPHERE",			sphere},
	{"PLANE",			plane},
	{"BOX",				box},
	{"CYLINDER",		cylinder},
	{"SOR",				sor},
	{"CONE",				cone},
	{"CSG",				csg},		
	{"POINTLIGHT",		pointlight},
	{"SPOTLIGHT",		spotlight},
	{"LOADOBJ",			loadobj},
	{"AMBIENT",			ambient},
	{"DIFFUSE",			diffuse},
	{"FOGLEN",			foglen},
	{"SPECULAR",		specular},
	{"DIFFTRANS",		difftrans},
	{"SPECTRANS",		spectrans},
	{"REFEXP",			refexp},
	{"TRANSEXP",		transexp},
	{"REFRINDEX",		refrindex},
	{"REFLECT",			reflect},
	{"TRANSPAR",		transpar},
	{"TRANSLUC",		transluc},
	{"IMTEXTURE",		imtexture},
	{"HYPERTEXTURE",	hypertexture},
	{"BRUSH",			brush},
	{"SETCAMERA",		setcamera},
	{"SETSCREEN",		setscreen},
	{"SETWORLD",		setworld},
	{"SAVEPIC",			savepic},
	{"CLEANUP",			cleanup},
	{"STARTRENDER",	startrender},
	{"ANTIALIAS",		antialias},
	{"BRUSHPATH",		brushpath},
	{"TEXTUREPATH",	texturepath},
	{"OBJECTPATH",		objectpath},
	{"PICTUREPATH", 	picturepath},
	{"INCLUDEPATH", 	includepath},
	{"ALIGNMENT",		alignment},
	{"NEWACTOR",		newactor},
	{"POSITION",		position},
	{"SIZE",				size},
	{"DISTRIB",			distrib},
	{"INCLUDE",			include},
	{"STAR",				star},
	{"FLARE",			flare},
	{NULL, 0L},
};

// global variables
static int		nIncludeDepth;				// current include nesting
static INCLUDE	Includefile[MAXINCLUDE];
static char		szIncludePath[256];
static char		szPicturePath[256];
static CRenderOutput *pDisplay;
static ITEMLIST *surf = NULL;
static ITEMLIST *actor = NULL;
static int		xRes=160, yRes=128;
		 rsiResult err;
static rsiVECTOR 	v;
static char 	buf[256];
static void 	*obj;
static void		*light = NULL;
rsiSMALL_COLOR	*scrarray = NULL;

// amount of objects needed for CSG
static int insideCSG = 0;

void Cooperate(rsiCONTEXT *rc)
{
	pDisplay->Cooperate();
}

void WriteLog(rsiCONTEXT *rc, char *text)
{
	pDisplay->Log(text);
}

BOOL CheckCancel(rsiCONTEXT *rc)
{
	return pDisplay->CheckCancel();
}

void __cdecl UpdateStatus(rsiCONTEXT *rc, float percent, float time, int, int, rsiSMALL_COLOR *)
{
	pDisplay->UpdateStatus(percent, time);
}

BOOL GetPictureInfo(UBYTE **p, int *xR, int *yR)
{
//	*p = (UBYTE*)rc->scrarray;
//	*xR = rc->camera.xres;
//	*yR = rc->camera.yres;

	return (*p != NULL);
}

void ParserCleanup()
{
	if (scrarray)
		delete [ ] scrarray;
}

void open_error_request(unsigned char *err)
{
	MessageBox(NULL, (char *)err, "Oh Oh ! - Error", MB_OK | MB_TASKMODAL);
}

void GetError(char *buffer, int no)
{
	if (no >= ERROR_BASE)
		strcpy(buffer, app_errors[no - ERROR_BASE]);
	else
		rsiGetErrorMsg(rc, buffer, no);
}

/*************
 * FUNCTION:		read_vector
 * DESCRIPTION:	read vector from string
 * INPUT:			v		pointer to vector
 *						s		vector string; format '<x,y,z>'
 * OUTPUT:			TRUE if ok else FALSE
 *************/
BOOL read_vector(rsiVECTOR *v, char *s)
{
	if (s)
	{
		if (s[0] != '<')	
			return FALSE;
		if (s[strlen(s)-1] != '>')
			return FALSE;
		v->x = float(atof(strtok(&s[1],",")));
		v->y = float(atof(strtok(NULL,",")));
		v->z = float(atof(strtok(NULL,">")));

		return TRUE;
	}
	else
		return FALSE;
}

/*************
 * DESCRIPTION:   read 2d-vector from string
 * INPUT:         v     pointer to vector
 *                s     vector string; format '<x,y>'
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL read_vector2d(rsiVECTOR2D *v, char *s)
{
	char buf[256];
	
	if (s)
	{
		strcpy(buf, s);
		
	   if(buf[0] != '<')
			return FALSE;
  		if(buf[strlen(buf)-1] != '>')
			return FALSE;
		v->x = float(atof(strtok(&buf[1],",")));
		v->y = float(atof(strtok(NULL,">")));
		return TRUE;
	}
	else
		return FALSE;
}

/*************
 * DESCRIPTION:	read color from string
 * INPUT:			c		pointer to color
 *						s		color string; format '[r,g,b]'
 * OUTPUT:			TRUE if ok else FALSE
 *************/
BOOL read_color(rsiCOLOR *c, char *s)
{
	if (s)
	{
		if (s[0] != '<')
			return FALSE;
		if (s[strlen(s)-1] != '>')
			return FALSE;
		c->r = float(atol(strtok(&s[1],",")))/255;
		c->g = float(atol(strtok(NULL,",")))/255;
		c->b = float(atol(strtok(NULL,"<")))/255;

		return TRUE;
	}
	return FALSE;
}

ULONG includepath(ULONG *arg)
{
	strcpy(szIncludePath, (char *)arg[0]);
	
	return RETURN_OK;
}

ULONG picturepath(ULONG *arg)
{
	strcpy(szPicturePath, (char *)arg[0]);
	
	return RETURN_OK;
}

ULONG include(ULONG *arg)
{
	char buf[256];

	nIncludeDepth++;
	if (nIncludeDepth == MAXINCLUDE)
		return ERROR_MAXINCLUDE;
	if (!arg[0])
		return ERROR_SYNTAX;
		
	strcpy(Includefile[nIncludeDepth].name, (char *)arg[0]);
	Includefile[nIncludeDepth].linenumber = 0;
	if (Includefile[nIncludeDepth].name[0])
	{
		//ExpandPath(szIncludePath, Includefile[include].name, buf);
		if (!Includefile[nIncludeDepth].name[0])
		{
			// cannot find file
			return ERROR_READ;
		}
	}
	Includefile[nIncludeDepth].hFile = fopen(buf, "rt");
	if (!Includefile[nIncludeDepth].hFile)
		return ERROR_READ;

	return RETURN_OK;
}

// Scan dismantels the given string <buf> into its components.
// The components are divided either by <,> or <">
// the results are saved in arg
int Scan(char *line, char *buf, ULONG *arg)
{
	int i = 0, argi, len;
	char *end, *pos;
	pos = line;
	strcpy(buf, "");
	if (!pos)
		return 0;

	argi = 0;

	while (pos[i] && (iscntrl(pos[i]) || pos[i] == ' '))	// ignore spaces
		i++;
	if (!pos[i] || pos[i] == '\n' || (pos[i] == '/' && pos[i + 1] == '/'))	
		return 1;								// return if empty line or comment

	// read as long as there are arguments
	while (!(!pos[i] || pos[i] == '\n' || (pos[i] == '/' && pos[i + 1] == '/')))
	{
		while (pos[i] && (iscntrl(pos[i]) || pos[i] == ' '))	// ignore spaces
			i++;

		if (pos[i] == ',' && argi > 0)
		{
			i++;
			while (pos[i] && (iscntrl(pos[i]) || pos[i] == ' '))	// ignore spaces
				i++;
		}
		
		if (pos[i] == '<')
			end = strpbrk(&pos[i+1], ">\n") + 1;
		else
			if (pos[i] == '"')
				end = strpbrk(&pos[++i], "\"\n") + 1;
			else
		    	end = strpbrk(&pos[i], ", \"\n");

		if (end)
			len = int(end - &pos[i]);
		else
			len = strlen(&pos[i]);

		if (argi == 0)
		{
			strncpy(buf, &pos[i], len);
			buf[len] = '\0';
		}
		else
		{
			if (len > 0)
			{
				arg[argi - 1] = (ULONG)(new char[len + 1]);
				strncpy((char *)arg[argi - 1], &pos[i], len);
				((char *)(arg[argi - 1]))[len] = 0;
				if (pos[i + len - 1] == '"')
					((char *)(arg[argi - 1]))[len - 1] = 0;  // that's SOOO dirty!!!
			}
		}
		i+= len;
		argi++;
	}
	return 0;
}

void GetLine(char **prog, char *line, int *linenumber)
{
	int nLength;
	char *end;
	
	// are we including a file ?
	if (nIncludeDepth)
	{
		if (feof(Includefile[nIncludeDepth].hFile))
		{
			fclose(Includefile[nIncludeDepth].hFile);
			nIncludeDepth--;
			(*linenumber)++;
		}
	}
	if (nIncludeDepth)
	{
		fgets(line, 256, Includefile[nIncludeDepth].hFile);
		Includefile[nIncludeDepth].linenumber++;
	}
	else
	{
		end = strpbrk(*prog, "\n\r\0");
		if (end)
			nLength = int(end - *prog);
		else
			nLength = strlen(*prog);
		memcpy(line, *prog, nLength);
		*prog+= nLength;
		while (**prog == '\n' || **prog == '\r')
		{
			if (**prog == '\n')
				(*linenumber)++;
			(*prog)++;
		}
		line[nLength] = '\0'; 
	}
}


DWORD WINAPI Execute(LPDWORD data)
{
	int i, linenumber;
	char line[256], buf[256], logbuf[100], *prog;
	BOOL error = FALSE;

	obj = NULL;
	light = NULL;

	ULONG arg[MAXARGS];

	InitInterface();

	for (i = 0; i < MAXARGS; i++)
		arg[i] = NULL;

	prog = ((THREAD_DATA *)data)->szProg;
	pDisplay = ((THREAD_DATA *)data)->pDisplay;
	strcpy(szIncludePath, "");
	strcpy(szPicturePath, "");

	linenumber = 0;
	nIncludeDepth = 0;

	while ((*prog || nIncludeDepth > 0) && !error)
	{ 
		GetLine(&prog, line, &linenumber);
		// read command first
		if (!Scan(line, buf, arg))
		{
			i = 0;
			// find command in list	
  			while (commands[i].name && stricmp(buf, commands[i].name))
  		 		i++;
				
			// call corresponding function with "arg" as argument
			if (commands[i].func)
				error = commands[i].func(arg);
			else
				error = ERROR_UNKNOWNCOM;
				
			for (i = 0; i < MAXARGS; i++)
			{
				if (arg[i])
					delete (ULONG *)arg[i];
				arg[i] = NULL;
			}

			if (error)
			{
				if (nIncludeDepth)
				{
					sprintf(logbuf, "In include file: \"%s\"", Includefile[nIncludeDepth].name);
					pDisplay->Log(logbuf);
					linenumber = Includefile[nIncludeDepth].linenumber;
				}
				sprintf(logbuf, "Error executing command in line %i", linenumber);
				pDisplay->Log(logbuf);
				pDisplay->Log(line);
				GetError(logbuf, error);
				pDisplay->Log(logbuf);
			}
		}
	}
	if (error)
	{
		pDisplay->Log("Error occurred");
		pDisplay->Log("Cancel execution");
		// close open include files
		while (nIncludeDepth > 0)
		{
			if (Includefile[nIncludeDepth].hFile)
				fclose(Includefile[nIncludeDepth].hFile);
			nIncludeDepth--;
		}
	}
	else
	{
		if (pDisplay->CheckCancel())
			pDisplay->Log("Execution canceled");
		else
			pDisplay->Log("Execution successful");
	}


	CleanupInterface();
	/*rsiCleanup(rc);
	rsiExit();*/

	return TRUE;
}


