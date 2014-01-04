/***************
 * PROGRAM:       Modeler
 * NAME:          prefs.h
 * DESCRIPTION:   definitions for preference files
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    14.12.95 ah    initial release
 *    18.12.96 ah    added ID_UNDO
 *    06.03.97 ah    added ID_TOOL and PREFFLAG_EXTIMAGES
 *    03.11.97 ah    added ID_JEPG for jpeg compression quality
 *    07.12.97 ah    added ID_LIGP, ID_LIGR for light settings
 *    21.01.97 ah    added ID_RPRI for priority of render subtask
 ***************/

#ifndef PREFS_H
#define PREFS_H

#ifdef __AMIGA__
#ifndef IFF_IFFPARSE_H
#include <libraries/iffparse.h>
#endif
#else
#include "iffparse.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

class PREFS : public SLIST
{
	public:
		ULONG id;
		ULONG length;
		void *data;

		PREFS();
		~PREFS();
		BOOL AddPrefs();
		BOOL GetPrefs();
};

BOOL PrefsLoad(char*);
BOOL PrefsSave(char*);
void FreePrefs();

// general modeler prefs
#define ID_VIEW MAKE_ID('V','I','E','W')     // viewer executable path
#define ID_OBJP MAKE_ID('O','B','J','P')     // object path
#define ID_OBJR MAKE_ID('O','B','J','R')     // initial object requester size
#define ID_PRJP MAKE_ID('P','R','J','P')     // project path
#define ID_PRJR MAKE_ID('P','R','J','R')     // initial project requester size
#define ID_BRSP MAKE_ID('B','R','S','P')     // brush path
#define ID_BRSR MAKE_ID('B','R','S','R')     // initial brush requester size
#define ID_TXTP MAKE_ID('T','X','T','P')     // texture path
#define ID_TXTR MAKE_ID('T','X','T','R')     // initial texture requester size
#define ID_MATP MAKE_ID('M','A','T','P')     // materials path
#define ID_MATR MAKE_ID('M','A','T','R')     // initial materials requester size
#define ID_LIGP MAKE_ID('L','I','G','P')     // lights path
#define ID_LIGR MAKE_ID('L','I','G','R')     // initial ligths requester size
#define ID_FLGS MAKE_ID('F','L','G','S')     // various flags
#define ID_UNDO MAKE_ID('U','N','D','O')     // undo memory
#define ID_TOOL MAKE_ID('T','O','O','L')     // size of toolbar images (Amiga only)
#define ID_WGHT MAKE_ID('W','G','H','T')     // weight of object browser and material manager
#define ID_JPEG MAKE_ID('J','P','E','G')     // jpeg compression quality
#define ID_RPRI MAKE_ID('R','P','R','I')     // priority of the render subtask
#define ID_STCK MAKE_ID('S','T','C','K')     // stack size of the render task
#define ID_COLR MAKE_ID('C','O','L','R')     // colors

#define PREFFLAG_SHOWCOORD           0x02    // always show coordinates
#define PREFFLAG_EXTIMAGES           0x04    // load toolbar images from files in drawer 'buttons' (Amiga only)
#define PREFFLAG_USESURFACECOLORS    0x08    // use surface colors to display objects
#define PREFFLAG_OPENGL_DIRECTRENDER 0x10    // use direct rendering for StormMesa (Amiga only)

#endif
