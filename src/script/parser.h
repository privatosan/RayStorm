 /***************
  * NAME:			parser.h
  * PROJECT:		RayStorm
  * DESCRIPTION:	parses the RayStorm script language
  * AUTHORS:		Mike Hesser
  * HISTORY:		DATE          NAME    COMMENT
  *					19.03.1996	mh		first release (0.1)
  ***************/

#ifndef PARSER_H
#define PARSER_H

#include "raystorm.h"
#include "render.h"

// prototypes
ULONG WINAPI Execute(LPDWORD);
BOOL GetPictureInfo(UBYTE **, int *, int *);
void open_error_request(unsigned char *);
void ParserCleanup();
void Cooperate(rsiCONTEXT *);
void WriteLog(rsiCONTEXT *, char *);
BOOL CheckCancel(rsiCONTEXT *);
void __cdecl UpdateStatus(rsiCONTEXT *, float, float, int, int, rsiSMALL_COLOR *);

#endif
