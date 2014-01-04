/***************
 * PROGRAM:       Modeler
 * NAME:          utility.h
 * DESCRIPTION:   definitions for utility functions which are different on all platforms
 * AUTHORS:       Andreas Heumann & Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.02.97 ah    initial release
 *    10.03.97 ah    added EnableMenuItem()
 *    17.08.97 ah    added EnableToolBarItem()
 *    14.09.97 ah    added ASSERT()
 *    16.09.97 ah    added CheckMenuItem()
 *    01.12.97 ah    added CheckToolbarItem()
 ***************/

#ifndef UTILITY_H
#define UTILITY_H

#ifdef __AMIGA__
#ifndef MUIDEFS_H
#include "muidefs.h"
#endif
#else
class CCamView;
#endif

// file requester flags
#define FILEREQU_DIR             1     // directory requester
#define FILEREQU_SAVE            2     // save requester
#define FILEREQU_INITIALFILEDIR  4     // take initial file and drawer from file buffer
#define FILEREQU_INITIALFILE     8     // take initial file from file buffer
#define FILEREQU_RELATIVEPATH    16    // return path relative to current directory

enum
{
	FILEREQU_IMAGE,
	FILEREQU_BRUSH,
	FILEREQU_OBJECT,
	FILEREQU_MATERIAL,
	FILEREQU_RTEXTURE,
	FILEREQU_ITEXTURE,
	FILEREQU_HTEXTURE,
	FILEREQU_ALL,
	FILEREQU_SCENE,
	FILEREQU_LIGHT
};

typedef struct
{
	ULONG pathID, rectID;
	char *fileTitle;
	char *filter;
	char *filterList;
} FILEREQU_FILETYPE;

class UTILITY
{
	private:
#ifdef __AMIGA__
		struct Screen *screen;
		Object *strip, *mainwin, *drawwin;

	public:
		void InitScreen(struct Screen*);
		void InitMainWin(Object *, Object*);
		void InitDrawWin(Object*);
#else
		CCamView *view;

	public:
		void Init(CCamView *);
#endif
	public:
		void Request(char *, ...);
		void Request(ULONG);
		int AskRequest(char *, ...);
		BOOL FileRequ(char *, ULONG, ULONG);
		void EnableMenuItem(int, BOOL);
		void CheckMenuItem(int, BOOL);
		void EnableToolbarItem(int, BOOL);
		void CheckToolbarItem(int);
};

extern UTILITY utility;

enum
{
	MENU_UNDO,
	MENU_REDO,
	MENU_CUT,
	MENU_COPY,
	MENU_PASTE,
	MENU_SETRENDERWINDOW,
	MENU_SHOWBRUSHES,
	MENU_SHOWTEXTURES,
	MENU_SHOWCAMERA,
	MENU_SHOWLIGHT,
	MENU_SHOWMESH,
	MENU_SHOWSPHERE,
	MENU_SHOWBOX,
	MENU_SHOWCYLINDER,
	MENU_SHOWCONE,
	MENU_SHOWSOR,
	MENU_SHOWPLANE,
	MENU_SHOWCSG,
	MENU_VIEWFRONT,
	MENU_VIEWTOP,
	MENU_VIEWRIGHT,
	MENU_VIEWPERSP,
	MENU_VIEWQUAD,
	MENU_VIEWCAMERA,
};

enum
{
	TOOLBAR_EDITSOR, TOOLBAR_EDITMESH
};

enum
{
	TOOLBAR_VIEWFRONT,
	TOOLBAR_VIEWRIGHT,
	TOOLBAR_VIEWTOP,
	TOOLBAR_VIEWPERSP,
	TOOLBAR_VIEWCAMERA,
	TOOLBAR_VIEWFOUR
};

#ifdef __AMIGA__
#ifdef DEBUG
#define ASSERT(condition) if(!(condition)) utility.Request("Assertion failed in file %s line %ld", __FILE__, __LINE__)
#else
#define ASSERT(condition)
#endif // DEBUG
#endif __AMIGA__

#ifndef __AMIGA__
void Request(UINT);
#endif

#endif

